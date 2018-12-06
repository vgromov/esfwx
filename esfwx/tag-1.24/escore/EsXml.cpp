#include "escorepch.h"
#pragma hdrstop

#include "EsXml.h"

// Static linking under BCC - use dependencies ordering for modules
// and static objects initialization
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
# pragma package(smart_init)
#endif
//---------------------------------------------------------------------------

/**
 * pugixml parser - version 1.4
 * --------------------------------------------------------
 * Copyright (C) 2006-2014, by Arseny Kapoulkine (arseny.kapoulkine@gmail.com)
 * Report bugs and download new versions at http://pugixml.org/
 *
 * This library is distributed under the MIT License. See notice at the end
 * of this file.
 *
 * This work is based on the pugxml parser, which is:
 * Copyright (C) 2003, by Kristen Wegner (kristen@tima.net)
 */

#include <stdint.h>
// For placement new
#include <new>

// compiler tune-ups
//
#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_MS
#  pragma warning(push)
#  pragma warning(disable: 4127) // conditional expression is constant
#  pragma warning(disable: 4324) // structure was padded due to __declspec(align())
#  pragma warning(disable: 4702) // unreachable code
#  pragma warning(disable: 4996) // this function or variable may be unsafe
#elif ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_INTEL
#  pragma warning(disable: 177) // function was declared but never referenced
#  pragma warning(disable: 279) // controlling expression is constant
#  pragma warning(disable: 1478 1786) // function was declared "deprecated"
#  pragma warning(disable: 1684) // conversion from pointer to same-sized integral type
#elif ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
#  pragma option push
#  pragma warn -8008 // condition is always false
#  pragma warn -8066 // unreachable code
#endif


namespace EsXml {

  void* default_allocate(size_t size)
  {
    return malloc(size);
  }

  void default_deallocate(void* ptr)
  {
    free(ptr);
  }

  template <typename T>
  struct xml_memory_management_function_storage
  {
    static allocation_function allocate;
    static deallocation_function deallocate;
  };

  template <typename T> allocation_function xml_memory_management_function_storage<T>::allocate = default_allocate;
  template <typename T> deallocation_function xml_memory_management_function_storage<T>::deallocate = default_deallocate;

  typedef xml_memory_management_function_storage<int> Memory;

  void set_memory_management_functions(allocation_function allocate, deallocation_function deallocate)
  {
    Memory::allocate = allocate;
    Memory::deallocate = deallocate;
  }

  allocation_function get_memory_allocation_function()
  {
    return Memory::allocate;
  }

  deallocation_function get_memory_deallocation_function()
  {
    return Memory::deallocate;
  }
} // namespace EsXml
//---------------------------------------------------------------------------

// String utilities
//
// Get string length
static size_t strlength(EsString::const_pointer s)
{
  ES_ASSERT(s);
  return esStrlen(s);
}

// Compare two strings
static bool strequal(EsString::const_pointer src, EsString::const_pointer dst)
{
  ES_ASSERT(src && dst);
  return 0 == esStrcmp(src, dst);
}

// Compare lhs with [rhs_begin, rhs_end)
static bool strequalrange(EsString::const_pointer lhs, EsString::const_pointer rhs, size_t count)
{
  for (size_t i = 0; i < count; ++i)
    if (lhs[i] != rhs[i])
      return false;

  return lhs[count] == 0;
}

// Get length of wide string, even if CRT lacks wide character support
static size_t strlength_wide(const ES_WCHAR* s)
{
  ES_ASSERT(s);

  const ES_WCHAR* end = s;
  while (*end) end++;
  return static_cast<size_t>(end - s);
}

#if !defined(ES_USE_NARROW_ES_CHAR)
// Convert string to wide string, assuming all symbols are ASCII
static void widen_ascii(EsString::value_type* dest, const char* source)
{
  for (const char* i = source; *i; ++i) *dest++ = *i;
  *dest = 0;
}
#endif

#if !defined(ES_XML_NO_XPATH)
// unique_ptr-like buffer holder for exception recovery
struct buffer_holder
{
  void* data;
  void (*deleter)(void*);

  buffer_holder(void* data_, void (*deleter_)(void*)): data(data_), deleter(deleter_)
  {
  }

  ~buffer_holder()
  {
    if (data) deleter(data);
  }

  void* release()
  {
    void* result = data;
    data = 0;
    return result;
  }
};
#endif
//---------------------------------------------------------------------------

typedef EsXml::Memory EsXmlMemory;

static const size_t xml_memory_page_size =
#ifdef ES_XML_MEMORY_PAGE_SIZE
  ES_XML_MEMORY_PAGE_SIZE
#else
  32768
#endif
  ;

static const uintptr_t xml_memory_page_alignment = 32;
static const uintptr_t xml_memory_page_pointer_mask = ~(xml_memory_page_alignment - 1);
static const uintptr_t xml_memory_page_name_allocated_mask = 16;
static const uintptr_t xml_memory_page_value_allocated_mask = 8;
static const uintptr_t xml_memory_page_type_mask = 7;

struct EsXmlAllocator;

struct EsXmlMemoryPage
{
  static EsXmlMemoryPage* construct(void* memory)
  {
    if (!memory) return 0; //$ redundant, left for performance

    EsXmlMemoryPage* result = static_cast<EsXmlMemoryPage*>(memory);

    result->allocator = 0;
    result->memory = 0;
    result->prev = 0;
    result->next = 0;
    result->busy_size = 0;
    result->freed_size = 0;

    return result;
  }

  EsXmlAllocator* allocator;

  void* memory;

  EsXmlMemoryPage* prev;
  EsXmlMemoryPage* next;

  size_t busy_size;
  size_t freed_size;

  char data[1];
};

struct EsXmlMemoryStringHeader
{
  uint16_t page_offset; // offset from page->data
  uint16_t full_size; // 0 if string occupies whole page
};

struct EsXmlAllocator
{
  EsXmlAllocator(EsXmlMemoryPage* root): _root(root), _busy_size(root->busy_size)
  {
  }

  EsXmlMemoryPage* allocate_page(size_t data_size)
  {
    size_t size = offsetof(EsXmlMemoryPage, data) + data_size;

    // allocate block with some alignment, leaving memory for worst-case padding
    void* memory = EsXmlMemory::allocate(size + xml_memory_page_alignment);
    if (!memory) return 0;

    // align upwards to page boundary
    void* page_memory = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(memory) + (xml_memory_page_alignment - 1)) & ~(xml_memory_page_alignment - 1));

    // prepare page structure
    EsXmlMemoryPage* page = EsXmlMemoryPage::construct(page_memory);
    ES_ASSERT(page);

    page->memory = memory;
    page->allocator = _root->allocator;

    return page;
  }

  static void deallocate_page(EsXmlMemoryPage* page)
  {
    EsXmlMemory::deallocate(page->memory);
  }

  void* allocate_memory_oob(size_t size, EsXmlMemoryPage*& out_page);

  void* allocate_memory(size_t size, EsXmlMemoryPage*& out_page)
  {
    if (_busy_size + size > xml_memory_page_size) return allocate_memory_oob(size, out_page);

    void* buf = _root->data + _busy_size;

    _busy_size += size;

    out_page = _root;

    return buf;
  }

  void deallocate_memory(void* ptr, size_t size, EsXmlMemoryPage* page)
  {
    if (page == _root) page->busy_size = _busy_size;

    ES_ASSERT(ptr >= page->data && ptr < page->data + page->busy_size);
    (void)!ptr;

    page->freed_size += size;
    ES_ASSERT(page->freed_size <= page->busy_size);

    if (page->freed_size == page->busy_size)
    {
      if (page->next == 0)
      {
        ES_ASSERT(_root == page);

        // top page freed, just reset sizes
        page->busy_size = page->freed_size = 0;
        _busy_size = 0;
      }
      else
      {
        ES_ASSERT(_root != page);
        ES_ASSERT(page->prev);

        // remove from the list
        page->prev->next = page->next;
        page->next->prev = page->prev;

        // deallocate
        deallocate_page(page);
      }
    }
  }

  EsString::pointer allocate_string(size_t length)
  {
    // allocate memory for string and header block
    size_t size = sizeof(EsXmlMemoryStringHeader) + length * sizeof(EsString::value_type);

    // round size up to pointer alignment boundary
    size_t full_size = (size + (sizeof(void*) - 1)) & ~(sizeof(void*) - 1);

    EsXmlMemoryPage* page;
    EsXmlMemoryStringHeader* header = static_cast<EsXmlMemoryStringHeader*>(allocate_memory(full_size, page));

    if (!header) return 0;

    // setup header
    ptrdiff_t page_offset = reinterpret_cast<char*>(header) - page->data;

    ES_ASSERT(page_offset >= 0 && page_offset < (1 << 16));
    header->page_offset = static_cast<uint16_t>(page_offset);

    // full_size == 0 for large strings that occupy the whole page
    ES_ASSERT(full_size < (1 << 16) || (page->busy_size == full_size && page_offset == 0));
    header->full_size = static_cast<uint16_t>(full_size < (1 << 16) ? full_size : 0);

    // round-trip through void* to avoid 'cast increases required alignment of target type' warning
    // header is guaranteed a pointer-sized alignment, which should be enough for EsString::value_type
    return static_cast<EsString::pointer>(static_cast<void*>(header + 1));
  }

  void deallocate_string(EsString::pointer string)
  {
    // this function casts pointers through void* to avoid 'cast increases required alignment of target type' warnings
    // we're guaranteed the proper (pointer-sized) alignment on the input string if it was allocated via allocate_string

    // get header
    EsXmlMemoryStringHeader* header = static_cast<EsXmlMemoryStringHeader*>(static_cast<void*>(string)) - 1;

    // deallocate
    size_t page_offset = offsetof(EsXmlMemoryPage, data) + header->page_offset;
    EsXmlMemoryPage* page = reinterpret_cast<EsXmlMemoryPage*>(static_cast<void*>(reinterpret_cast<char*>(header) - page_offset));

    // if full_size == 0 then this string occupies the whole page
    size_t full_size = header->full_size == 0 ? page->busy_size : header->full_size;

    deallocate_memory(header, full_size, page);
  }

  EsXmlMemoryPage* _root;
  size_t _busy_size;
};

void* EsXmlAllocator::allocate_memory_oob(size_t size, EsXmlMemoryPage*& out_page)
{
  const size_t large_allocation_threshold = xml_memory_page_size / 4;

  EsXmlMemoryPage* page = allocate_page(size <= large_allocation_threshold ? xml_memory_page_size : size);
  out_page = page;

  if (!page) return 0;

  if (size <= large_allocation_threshold)
  {
    _root->busy_size = _busy_size;

    // insert page at the end of linked list
    page->prev = _root;
    _root->next = page;
    _root = page;

    _busy_size = size;
  }
  else
  {
    // insert page before the end of linked list, so that it is deleted as soon as possible
    // the last page is not deleted even if it's empty (see deallocate_memory)
    ES_ASSERT(_root->prev);

    page->prev = _root->prev;
    page->next = _root;

    _root->prev->next = page;
    _root->prev = page;
  }

  // allocate inside page
  page->busy_size = size;

  return page->data;
}


/// A 'name=value' XML attribute structure.
struct EsXmlAttributeStruct
{
  /// Default ctor
  EsXmlAttributeStruct(EsXmlMemoryPage* page): header(reinterpret_cast<uintptr_t>(page)), name(0), value(0), prev_attribute_c(0), next_attribute(0)
  {
  }

  uintptr_t header;

  EsString::pointer name;  ///< Pointer to attribute name.
  EsString::pointer  value;  ///< Pointer to attribute value.

  EsXmlAttributeStruct* prev_attribute_c;  ///< Previous attribute (cyclic list)
  EsXmlAttributeStruct* next_attribute;  ///< Next attribute
};

/// An XML document tree node.
struct EsXmlNodeStruct
{
  /// Default ctor
  /// \param type - node type
  EsXmlNodeStruct(EsXmlMemoryPage* page, EsXmlNodeType type): header(reinterpret_cast<uintptr_t>(page) | (type - 1)), parent(0), name(0), value(0), first_child(0), prev_sibling_c(0), next_sibling(0), first_attribute(0)
  {
  }

  uintptr_t header;

  EsXmlNodeStruct*    parent;          ///< Pointer to parent

  EsString::pointer    name;            ///< Pointer to element name.
  EsString::pointer     value;          ///< Pointer to any associated string data.

  EsXmlNodeStruct*    first_child;    ///< First child

  EsXmlNodeStruct*    prev_sibling_c;  ///< Left brother (cyclic list)
  EsXmlNodeStruct*    next_sibling;    ///< Right brother

  EsXmlAttributeStruct*  first_attribute; ///< First attribute
};

struct EsXmlExtraBuffer
{
  EsString::pointer buffer;
  EsXmlExtraBuffer* next;
};

struct EsXmlDocumentStruct: public EsXmlNodeStruct, public EsXmlAllocator
{
  EsXmlDocumentStruct(EsXmlMemoryPage* page): EsXmlNodeStruct(page, xmlNodeDocument), EsXmlAllocator(page), buffer(0), extra_buffers(0)
  {
  }

  EsString::const_pointer buffer;

  EsXmlExtraBuffer* extra_buffers;
};

inline EsXmlAllocator& get_allocator(const EsXmlNodeStruct* node)
{
  ES_ASSERT(node);

  return *reinterpret_cast<EsXmlMemoryPage*>(node->header & xml_memory_page_pointer_mask)->allocator;
}

// Low-level DOM operations
inline EsXmlAttributeStruct* allocate_attribute(EsXmlAllocator& alloc)
{
  EsXmlMemoryPage* page;
  void* memory = alloc.allocate_memory(sizeof(EsXmlAttributeStruct), page);

  return new (memory) EsXmlAttributeStruct(page);
}

inline EsXmlNodeStruct* allocate_node(EsXmlAllocator& alloc, EsXmlNodeType type)
{
  EsXmlMemoryPage* page;
  void* memory = alloc.allocate_memory(sizeof(EsXmlNodeStruct), page);

  return new (memory) EsXmlNodeStruct(page, type);
}

inline void destroy_attribute(EsXmlAttributeStruct* a, EsXmlAllocator& alloc)
{
  uintptr_t header = a->header;

  if (header & xml_memory_page_name_allocated_mask) alloc.deallocate_string(a->name);
  if (header & xml_memory_page_value_allocated_mask) alloc.deallocate_string(a->value);

  alloc.deallocate_memory(a, sizeof(EsXmlAttributeStruct), reinterpret_cast<EsXmlMemoryPage*>(header & xml_memory_page_pointer_mask));
}

inline void destroy_node(EsXmlNodeStruct* n, EsXmlAllocator& alloc)
{
  uintptr_t header = n->header;

  if (header & xml_memory_page_name_allocated_mask) alloc.deallocate_string(n->name);
  if (header & xml_memory_page_value_allocated_mask) alloc.deallocate_string(n->value);

  for (EsXmlAttributeStruct* attr = n->first_attribute; attr; )
  {
    EsXmlAttributeStruct* next = attr->next_attribute;

    destroy_attribute(attr, alloc);

    attr = next;
  }

  for (EsXmlNodeStruct* child = n->first_child; child; )
  {
    EsXmlNodeStruct* next = child->next_sibling;

    destroy_node(child, alloc);

    child = next;
  }

  alloc.deallocate_memory(n, sizeof(EsXmlNodeStruct), reinterpret_cast<EsXmlMemoryPage*>(header & xml_memory_page_pointer_mask));
}

EsXmlNodeStruct* append_node(EsXmlNodeStruct* node, EsXmlAllocator& alloc, EsXmlNodeType type = xmlNodeElement)
{
  EsXmlNodeStruct* child = allocate_node(alloc, type);
  if (!child) return 0;

  child->parent = node;

  EsXmlNodeStruct* first_child = node->first_child;

  if (first_child)
  {
    EsXmlNodeStruct* last_child = first_child->prev_sibling_c;

    last_child->next_sibling = child;
    child->prev_sibling_c = last_child;
    first_child->prev_sibling_c = child;
  }
  else
  {
    node->first_child = child;
    child->prev_sibling_c = child;
  }

  return child;
}

EsXmlAttributeStruct* append_attribute_ll(EsXmlNodeStruct* node, EsXmlAllocator& alloc)
{
  EsXmlAttributeStruct* a = allocate_attribute(alloc);
  if (!a) return 0;

  EsXmlAttributeStruct* first_attribute = node->first_attribute;

  if (first_attribute)
  {
    EsXmlAttributeStruct* lastAttributeGet = first_attribute->prev_attribute_c;

    lastAttributeGet->next_attribute = a;
    a->prev_attribute_c = lastAttributeGet;
    first_attribute->prev_attribute_c = a;
  }
  else
  {
    node->first_attribute = a;
    a->prev_attribute_c = a;
  }

  return a;
}

// Helper classes for code generation
struct opt_false
{
  enum { value = 0 };
};

struct opt_true
{
  enum { value = 1 };
};

// Unicode utilities
inline uint16_t endian_swap(uint16_t value)
{
  return static_cast<uint16_t>(((value & 0xff) << 8) | (value >> 8));
}

inline uint32_t endian_swap(uint32_t value)
{
  return ((value & 0xff) << 24) | ((value & 0xff00) << 8) | ((value & 0xff0000) >> 8) | (value >> 24);
}

struct utf8_counter
{
  typedef size_t value_type;

  static value_type low(value_type result, uint32_t ch)
  {
    // U+0000..U+007F
    if (ch < 0x80) return result + 1;
    // U+0080..U+07FF
    else if (ch < 0x800) return result + 2;
    // U+0800..U+FFFF
    else return result + 3;
  }

  static value_type high(value_type result, uint32_t)
  {
    // U+10000..U+10FFFF
    return result + 4;
  }
};

struct utf8_writer
{
  typedef uint8_t* value_type;

  static value_type low(value_type result, uint32_t ch)
  {
    // U+0000..U+007F
    if (ch < 0x80)
    {
      *result = static_cast<uint8_t>(ch);
      return result + 1;
    }
    // U+0080..U+07FF
    else if (ch < 0x800)
    {
      result[0] = static_cast<uint8_t>(0xC0 | (ch >> 6));
      result[1] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
      return result + 2;
    }
    // U+0800..U+FFFF
    else
    {
      result[0] = static_cast<uint8_t>(0xE0 | (ch >> 12));
      result[1] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
      result[2] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
      return result + 3;
    }
  }

  static value_type high(value_type result, uint32_t ch)
  {
    // U+10000..U+10FFFF
    result[0] = static_cast<uint8_t>(0xF0 | (ch >> 18));
    result[1] = static_cast<uint8_t>(0x80 | ((ch >> 12) & 0x3F));
    result[2] = static_cast<uint8_t>(0x80 | ((ch >> 6) & 0x3F));
    result[3] = static_cast<uint8_t>(0x80 | (ch & 0x3F));
    return result + 4;
  }

  static value_type any(value_type result, uint32_t ch)
  {
    return (ch < 0x10000) ? low(result, ch) : high(result, ch);
  }
};

struct utf16_counter
{
  typedef size_t value_type;

  static value_type low(value_type result, uint32_t)
  {
    return result + 1;
  }

  static value_type high(value_type result, uint32_t)
  {
    return result + 2;
  }
};

struct utf16_writer
{
  typedef uint16_t* value_type;

  static value_type low(value_type result, uint32_t ch)
  {
    *result = static_cast<uint16_t>(ch);

    return result + 1;
  }

  static value_type high(value_type result, uint32_t ch)
  {
    uint32_t msh = static_cast<uint32_t>(ch - 0x10000) >> 10;
    uint32_t lsh = static_cast<uint32_t>(ch - 0x10000) & 0x3ff;

    result[0] = static_cast<uint16_t>(0xD800 + msh);
    result[1] = static_cast<uint16_t>(0xDC00 + lsh);

    return result + 2;
  }

  static value_type any(value_type result, uint32_t ch)
  {
    return (ch < 0x10000) ? low(result, ch) : high(result, ch);
  }
};

struct utf32_counter
{
  typedef size_t value_type;

  static value_type low(value_type result, uint32_t)
  {
    return result + 1;
  }

  static value_type high(value_type result, uint32_t)
  {
    return result + 1;
  }
};

struct utf32_writer
{
  typedef uint32_t* value_type;

  static value_type low(value_type result, uint32_t ch)
  {
    *result = ch;

    return result + 1;
  }

  static value_type high(value_type result, uint32_t ch)
  {
    *result = ch;

    return result + 1;
  }

  static value_type any(value_type result, uint32_t ch)
  {
    *result = ch;

    return result + 1;
  }
};

struct latin1_writer
{
  typedef uint8_t* value_type;

  static value_type low(value_type result, uint32_t ch)
  {
    *result = static_cast<uint8_t>(ch > 255 ? '?' : ch);

    return result + 1;
  }

  static value_type high(value_type result, uint32_t ch)
  {
    (void)ch;

    *result = '?';

    return result + 1;
  }
};

template <size_t size> struct wchar_selector;

template <> struct wchar_selector<2>
{
  typedef uint16_t type;
  typedef utf16_counter counter;
  typedef utf16_writer writer;
};

template <> struct wchar_selector<4>
{
  typedef uint32_t type;
  typedef utf32_counter counter;
  typedef utf32_writer writer;
};

typedef wchar_selector<sizeof(ES_WCHAR)>::counter wchar_counter;
typedef wchar_selector<sizeof(ES_WCHAR)>::writer wchar_writer;

template <typename Traits, typename opt_swap = opt_false> struct utf_decoder
{
  static inline typename Traits::value_type decode_utf8_block(const uint8_t* data, size_t size, typename Traits::value_type result)
  {
    const uint8_t utf8_byte_mask = 0x3f;

    while (size)
    {
      uint8_t lead = *data;

      // 0xxxxxxx -> U+0000..U+007F
      if (lead < 0x80)
      {
        result = Traits::low(result, lead);
        data += 1;
        size -= 1;

        // process aligned single-byte (ascii) blocks
        if ((reinterpret_cast<uintptr_t>(data) & 3) == 0)
        {
          // round-trip through void* to silence 'cast increases required alignment of target type' warnings
          while (size >= 4 && (*static_cast<const uint32_t*>(static_cast<const void*>(data)) & 0x80808080) == 0)
          {
            result = Traits::low(result, data[0]);
            result = Traits::low(result, data[1]);
            result = Traits::low(result, data[2]);
            result = Traits::low(result, data[3]);
            data += 4;
            size -= 4;
          }
        }
      }
      // 110xxxxx -> U+0080..U+07FF
      else if (static_cast<unsigned int>(lead - 0xC0) < 0x20 && size >= 2 && (data[1] & 0xc0) == 0x80)
      {
        result = Traits::low(result, ((lead & ~0xC0) << 6) | (data[1] & utf8_byte_mask));
        data += 2;
        size -= 2;
      }
      // 1110xxxx -> U+0800-U+FFFF
      else if (static_cast<unsigned int>(lead - 0xE0) < 0x10 && size >= 3 && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80)
      {
        result = Traits::low(result, ((lead & ~0xE0) << 12) | ((data[1] & utf8_byte_mask) << 6) | (data[2] & utf8_byte_mask));
        data += 3;
        size -= 3;
      }
      // 11110xxx -> U+10000..U+10FFFF
      else if (static_cast<unsigned int>(lead - 0xF0) < 0x08 && size >= 4 && (data[1] & 0xc0) == 0x80 && (data[2] & 0xc0) == 0x80 && (data[3] & 0xc0) == 0x80)
      {
        result = Traits::high(result, ((lead & ~0xF0) << 18) | ((data[1] & utf8_byte_mask) << 12) | ((data[2] & utf8_byte_mask) << 6) | (data[3] & utf8_byte_mask));
        data += 4;
        size -= 4;
      }
      // 10xxxxxx or 11111xxx -> invalid
      else
      {
        data += 1;
        size -= 1;
      }
    }

    return result;
  }

  static inline typename Traits::value_type decode_utf16_block(const uint16_t* data, size_t size, typename Traits::value_type result)
  {
    const uint16_t* end = data + size;

    while (data < end)
    {
      unsigned int lead = opt_swap::value ? endian_swap(*data) : *data;

      // U+0000..U+D7FF
      if (lead < 0xD800)
      {
        result = Traits::low(result, lead);
        data += 1;
      }
      // U+E000..U+FFFF
      else if (static_cast<unsigned int>(lead - 0xE000) < 0x2000)
      {
        result = Traits::low(result, lead);
        data += 1;
      }
      // surrogate pair lead
      else if (static_cast<unsigned int>(lead - 0xD800) < 0x400 && data + 1 < end)
      {
        uint16_t next = opt_swap::value ? endian_swap(data[1]) : data[1];

        if (static_cast<unsigned int>(next - 0xDC00) < 0x400)
        {
          result = Traits::high(result, 0x10000 + ((lead & 0x3ff) << 10) + (next & 0x3ff));
          data += 2;
        }
        else
        {
          data += 1;
        }
      }
      else
      {
        data += 1;
      }
    }

    return result;
  }

  static inline typename Traits::value_type decode_utf32_block(const uint32_t* data, size_t size, typename Traits::value_type result)
  {
    const uint32_t* end = data + size;

    while (data < end)
    {
      uint32_t lead = opt_swap::value ? endian_swap(*data) : *data;

      // U+0000..U+FFFF
      if (lead < 0x10000)
      {
        result = Traits::low(result, lead);
        data += 1;
      }
      // U+10000..U+10FFFF
      else
      {
        result = Traits::high(result, lead);
        data += 1;
      }
    }

    return result;
  }

  static inline typename Traits::value_type decode_latin1_block(const uint8_t* data, size_t size, typename Traits::value_type result)
  {
    for (size_t i = 0; i < size; ++i)
    {
      result = Traits::low(result, data[i]);
    }

    return result;
  }

  static inline typename Traits::value_type decode_wchar_block_impl(const uint16_t* data, size_t size, typename Traits::value_type result)
  {
    return decode_utf16_block(data, size, result);
  }

  static inline typename Traits::value_type decode_wchar_block_impl(const uint32_t* data, size_t size, typename Traits::value_type result)
  {
    return decode_utf32_block(data, size, result);
  }

  template < typename wcT >
  static inline typename Traits::value_type decode_wchar_block(const wcT* data, size_t size, typename Traits::value_type result)
  {
    return decode_wchar_block_impl(reinterpret_cast<const typename wchar_selector<sizeof(wcT)>::type* >(data), size, result);
  }
};

template <typename T> void convert_utf_endian_swap(T* result, const T* data, size_t length)
{
  for (size_t i = 0; i < length; ++i) result[i] = endian_swap(data[i]);
}

template <typename wcT> void convert_wchar_endian_swap(wcT* result, const wcT* data, size_t length)
{
  for (size_t i = 0; i < length; ++i) result[i] = static_cast<wcT>(endian_swap(static_cast< typename wchar_selector<sizeof(wcT)>::type >(data[i])));
}

enum chartype_t
{
  ct_parse_pcdata = 1,  // \0, &, \r, <
  ct_parse_attr = 2,    // \0, &, \r, ', "
  ct_parse_attr_ws = 4,  // \0, &, \r, ', ", \n, tab
  ct_space = 8,      // \r, \n, space, tab
  ct_parse_cdata = 16,  // \0, ], >, \r
  ct_parse_comment = 32,  // \0, -, >, \r
  ct_symbol = 64,      // Any symbol > 127, a-z, A-Z, 0-9, _, :, -, .
  ct_start_symbol = 128  // Any symbol > 127, a-z, A-Z, _, :
};

static const unsigned char chartype_table[256] =
{
  55,  0,   0,   0,   0,   0,   0,   0,      0,   12,  12,  0,   0,   63,  0,   0,   // 0-15
  0,   0,   0,   0,   0,   0,   0,   0,      0,   0,   0,   0,   0,   0,   0,   0,   // 16-31
  8,   0,   6,   0,   0,   0,   7,   6,      0,   0,   0,   0,   0,   96,  64,  0,   // 32-47
  64,  64,  64,  64,  64,  64,  64,  64,     64,  64,  192, 0,   1,   0,   48,  0,   // 48-63
  0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 64-79
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0,   0,   16,  0,   192, // 80-95
  0,   192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 96-111
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 0, 0, 0, 0, 0,           // 112-127

  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192, // 128+
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192,
  192, 192, 192, 192, 192, 192, 192, 192,    192, 192, 192, 192, 192, 192, 192, 192
};

enum chartypex_t
{
  ctx_special_pcdata = 1,   // Any symbol >= 0 and < 32 (except \t, \r, \n), &, <, >
  ctx_special_attr = 2,     // Any symbol >= 0 and < 32 (except \t), &, <, >, "
  ctx_start_symbol = 4,    // Any symbol > 127, a-z, A-Z, _
  ctx_digit = 8,        // 0-9
  ctx_symbol = 16        // Any symbol > 127, a-z, A-Z, 0-9, _, -, .
};

static const unsigned char chartypex_table[256] =
{
  3,  3,  3,  3,  3,  3,  3,  3,     3,  0,  2,  3,  3,  2,  3,  3,     // 0-15
  3,  3,  3,  3,  3,  3,  3,  3,     3,  3,  3,  3,  3,  3,  3,  3,     // 16-31
  0,  0,  2,  0,  0,  0,  3,  0,     0,  0,  0,  0,  0, 16, 16,  0,     // 32-47
  24, 24, 24, 24, 24, 24, 24, 24,    24, 24, 0,  0,  3,  0,  3,  0,     // 48-63

  0,  20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 64-79
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 0,  0,  0,  0,  20,    // 80-95
  0,  20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 96-111
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 0,  0,  0,  0,  0,     // 112-127

  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,    // 128+
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20,
  20, 20, 20, 20, 20, 20, 20, 20,    20, 20, 20, 20, 20, 20, 20, 20
};

template <typename C>
__inline unsigned char PUGI__IS_CHARTYPE_IMPL(C c, int ct, const unsigned char* table)
{
  return
    ((static_cast<unsigned int>(c) < 128) ?
      table[static_cast<unsigned int>(c)] :
      table[128]) &
    ct;
}

template <>
__inline unsigned char PUGI__IS_CHARTYPE_IMPL(char c, int ct, const unsigned char* table)
{
  return
    ((static_cast<unsigned int>(c) < 128) ?
      table[static_cast<unsigned int>(c)] :
      table[128]) &
    ct;
}

template <typename C>
__inline bool PUGI__IS_CHARTYPE(C c, int ct)
{
  return PUGI__IS_CHARTYPE_IMPL<C>(c, ct, chartype_table);
}

template <typename C>
__inline bool PUGI__IS_CHARTYPEX(C c, int ct)
{
  return PUGI__IS_CHARTYPE_IMPL<C>(c, ct, chartypex_table);
}

#if ES_ENDIAN == ES_LITTLE_ENDIAN
# define ES_XMLUTF16_ENDIAN xmlEncodingUtf16_le
# define ES_XMLUTF32_ENDIAN xmlEncodingUtf32_le
#else
# define ES_XMLUTF16_ENDIAN xmlEncodingUtf16_be
# define ES_XMLUTF32_ENDIAN xmlEncodingUtf32_be
#endif

EsXmlEncoding get_wchar_encoding()
{
  ES_COMPILE_TIME_ASSERT(sizeof(ES_WCHAR) == 2 || sizeof(ES_WCHAR) == 4, sizeof_wchar_t);

  if (sizeof(ES_WCHAR) == 2)
    return ES_XMLUTF16_ENDIAN;
  else
    return ES_XMLUTF32_ENDIAN;
}

EsXmlEncoding guess_buffer_encoding(uint8_t d0, uint8_t d1, uint8_t d2, uint8_t d3)
{
  // look for BOM in first few bytes
  if (d0 == 0 && d1 == 0 && d2 == 0xfe && d3 == 0xff) return xmlEncodingUtf32_be;
  if (d0 == 0xff && d1 == 0xfe && d2 == 0 && d3 == 0) return xmlEncodingUtf32_le;
  if (d0 == 0xfe && d1 == 0xff) return xmlEncodingUtf16_be;
  if (d0 == 0xff && d1 == 0xfe) return xmlEncodingUtf16_le;
  if (d0 == 0xef && d1 == 0xbb && d2 == 0xbf) return xmlEncodingUtf8;

  // look for <, <? or <?xm in various encodings
  if (d0 == 0 && d1 == 0 && d2 == 0 && d3 == 0x3c) return xmlEncodingUtf32_be;
  if (d0 == 0x3c && d1 == 0 && d2 == 0 && d3 == 0) return xmlEncodingUtf32_le;
  if (d0 == 0 && d1 == 0x3c && d2 == 0 && d3 == 0x3f) return xmlEncodingUtf16_be;
  if (d0 == 0x3c && d1 == 0 && d2 == 0x3f && d3 == 0) return xmlEncodingUtf16_le;
  if (d0 == 0x3c && d1 == 0x3f && d2 == 0x78 && d3 == 0x6d) return xmlEncodingUtf8;

  // look for utf16 < followed by node name (this may fail, but is better than utf8 since it's zero terminated so early)
  if (d0 == 0 && d1 == 0x3c) return xmlEncodingUtf16_be;
  if (d0 == 0x3c && d1 == 0) return xmlEncodingUtf16_le;

  // no known BOM detected, assume utf8
  return xmlEncodingUtf8;
}

EsXmlEncoding get_buffer_encoding(EsXmlEncoding encoding, const void* contents, size_t size)
{
  // replace wchar encoding with utf implementation
  if (encoding == xmlEncodingWchar) return get_wchar_encoding();

  // replace utf16 encoding with utf16 with specific endianness
  if (encoding == xmlEncodingUtf16) return ES_XMLUTF16_ENDIAN;

  // replace utf32 encoding with utf32 with specific endianness
  if (encoding == xmlEncodingUtf32) return ES_XMLUTF32_ENDIAN;

  // only do autodetection if no explicit encoding is requested
  if (encoding != xmlEncodingAuto) return encoding;

  // skip encoding autodetection if input buffer is too small
  if (size < 4) return xmlEncodingUtf8;

  // try to guess encoding (based on XML specification, Appendix F.1)
  const uint8_t* data = static_cast<const uint8_t*>(contents);

  volatile uint8_t d0 = data[0], d1 = data[1], d2 = data[2], d3 = data[3];

  return guess_buffer_encoding(d0, d1, d2, d3);
}

bool get_mutable_buffer(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
{
  size_t length = size / sizeof(EsString::value_type);

  if (is_mutable)
  {
    out_buffer = static_cast<EsString::pointer>(const_cast<void*>(contents));
    out_length = length;
  }
  else
  {
    EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
    if (!buffer) return false;

    memcpy(buffer, contents, length * sizeof(EsString::value_type));
    buffer[length] = 0;

    out_buffer = buffer;
    out_length = length + 1;
  }

  return true;
}

#if !defined(ES_USE_NARROW_ES_CHAR)
bool need_endian_swap_utf(EsXmlEncoding le, EsXmlEncoding re)
{
  return (le == xmlEncodingUtf16_be && re == xmlEncodingUtf16_le) || (le == xmlEncodingUtf16_le && re == xmlEncodingUtf16_be) ||
       (le == xmlEncodingUtf32_be && re == xmlEncodingUtf32_le) || (le == xmlEncodingUtf32_le && re == xmlEncodingUtf32_be);
}

bool convert_buffer_endian_swap(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
{
  EsString::const_pointer data = static_cast<EsString::const_pointer>(contents);
  size_t length = size / sizeof(EsString::value_type);

  if (is_mutable)
  {
    EsString::pointer buffer = const_cast<EsString::pointer>(data);

    convert_wchar_endian_swap<EsString::value_type>(buffer, data, length);

    out_buffer = buffer;
    out_length = length;
  }
  else
  {
    EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
    if (!buffer) return false;

    convert_wchar_endian_swap<EsString::value_type>(buffer, data, length);
    buffer[length] = 0;

    out_buffer = buffer;
    out_length = length + 1;
  }

  return true;
}

bool convert_buffer_utf8(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size)
{
  const uint8_t* data = static_cast<const uint8_t*>(contents);
  size_t data_length = size;

  // first pass: get length in ES_WCHAR units
  size_t length = utf_decoder<wchar_counter>::decode_utf8_block(data, data_length, 0);

  // allocate buffer of suitable length
  EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
  if (!buffer) return false;

  // second pass: convert utf8 input to ES_WCHAR
  wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
  wchar_writer::value_type oend = utf_decoder<wchar_writer>::decode_utf8_block(data, data_length, obegin);

  ES_ASSERT(oend == obegin + length);
  *oend = 0;

  out_buffer = buffer;
  out_length = length + 1;

  return true;
}

template <typename opt_swap> bool convert_buffer_utf16(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
{
  const uint16_t* data = static_cast<const uint16_t*>(contents);
  size_t data_length = size / sizeof(uint16_t);

  // first pass: get length in ES_WCHAR units
  size_t length = utf_decoder<wchar_counter, opt_swap>::decode_utf16_block(data, data_length, 0);

  // allocate buffer of suitable length
  EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
  if (!buffer) return false;

  // second pass: convert utf16 input to ES_WCHAR
  wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
  wchar_writer::value_type oend = utf_decoder<wchar_writer, opt_swap>::decode_utf16_block(data, data_length, obegin);

  ES_ASSERT(oend == obegin + length);
  *oend = 0;

  out_buffer = buffer;
  out_length = length + 1;

  return true;
}

template <typename opt_swap> bool convert_buffer_utf32(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
{
  const uint32_t* data = static_cast<const uint32_t*>(contents);
  size_t data_length = size / sizeof(uint32_t);

  // first pass: get length in ES_WCHAR units
  size_t length = utf_decoder<wchar_counter, opt_swap>::decode_utf32_block(data, data_length, 0);

  // allocate buffer of suitable length
  EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
  if (!buffer) return false;

  // second pass: convert utf32 input to ES_WCHAR
  wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
  wchar_writer::value_type oend = utf_decoder<wchar_writer, opt_swap>::decode_utf32_block(data, data_length, obegin);

  ES_ASSERT(oend == obegin + length);
  *oend = 0;

  out_buffer = buffer;
  out_length = length + 1;

  return true;
}

bool convert_buffer_latin1(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size)
{
  const uint8_t* data = static_cast<const uint8_t*>(contents);
  size_t data_length = size;

  // get length in ES_WCHAR units
  size_t length = data_length;

  // allocate buffer of suitable length
  EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
  if (!buffer) return false;

  // convert latin1 input to ES_WCHAR
  wchar_writer::value_type obegin = reinterpret_cast<wchar_writer::value_type>(buffer);
  wchar_writer::value_type oend = utf_decoder<wchar_writer>::decode_latin1_block(data, data_length, obegin);

  ES_ASSERT(oend == obegin + length);
  *oend = 0;

  out_buffer = buffer;
  out_length = length + 1;

  return true;
}

bool convert_buffer(EsString::pointer& out_buffer, size_t& out_length, EsXmlEncoding encoding, const void* contents, size_t size, bool is_mutable)
{
  // get native encoding
  EsXmlEncoding wchar_encoding = get_wchar_encoding();

  // fast path: no conversion required
  if (encoding == wchar_encoding) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

  // only endian-swapping is required
  if (need_endian_swap_utf(encoding, wchar_encoding)) return convert_buffer_endian_swap(out_buffer, out_length, contents, size, is_mutable);

  // source encoding is utf8
  if (encoding == xmlEncodingUtf8) return convert_buffer_utf8(out_buffer, out_length, contents, size);

  // source encoding is utf16
  if (encoding == xmlEncodingUtf16_be || encoding == xmlEncodingUtf16_le)
  {
    EsXmlEncoding native_encoding = ES_XMLUTF16_ENDIAN;

    return (native_encoding == encoding) ?
      convert_buffer_utf16(out_buffer, out_length, contents, size, opt_false()) :
      convert_buffer_utf16(out_buffer, out_length, contents, size, opt_true());
  }

  // source encoding is utf32
  if (encoding == xmlEncodingUtf32_be || encoding == xmlEncodingUtf32_le)
  {
    EsXmlEncoding native_encoding = ES_XMLUTF32_ENDIAN;

    return (native_encoding == encoding) ?
      convert_buffer_utf32(out_buffer, out_length, contents, size, opt_false()) :
      convert_buffer_utf32(out_buffer, out_length, contents, size, opt_true());
  }

  // source encoding is latin1
  if (encoding == xmlEncodingLatin1) return convert_buffer_latin1(out_buffer, out_length, contents, size);

  ES_ASSERT(!"Invalid encoding");
  return false;
}
#else
template <typename opt_swap> bool convert_buffer_utf16(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
{
  const uint16_t* data = static_cast<const uint16_t*>(contents);
  size_t data_length = size / sizeof(uint16_t);

  // first pass: get length in utf8 units
  size_t length = utf_decoder<utf8_counter, opt_swap>::decode_utf16_block(data, data_length, 0);

  // allocate buffer of suitable length
  EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
  if (!buffer) return false;

  // second pass: convert utf16 input to utf8
  uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
  uint8_t* oend = utf_decoder<utf8_writer, opt_swap>::decode_utf16_block(data, data_length, obegin);

  ES_ASSERT(oend == obegin + length);
  *oend = 0;

  out_buffer = buffer;
  out_length = length + 1;

  return true;
}

template <typename opt_swap> bool convert_buffer_utf32(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size, opt_swap)
{
  const uint32_t* data = static_cast<const uint32_t*>(contents);
  size_t data_length = size / sizeof(uint32_t);

  // first pass: get length in utf8 units
  size_t length = utf_decoder<utf8_counter, opt_swap>::decode_utf32_block(data, data_length, 0);

  // allocate buffer of suitable length
  EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
  if (!buffer) return false;

  // second pass: convert utf32 input to utf8
  uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
  uint8_t* oend = utf_decoder<utf8_writer, opt_swap>::decode_utf32_block(data, data_length, obegin);

  ES_ASSERT(oend == obegin + length);
  *oend = 0;

  out_buffer = buffer;
  out_length = length + 1;

  return true;
}

size_t get_latin1_7bit_prefix_length(const uint8_t* data, size_t size)
{
  for (size_t i = 0; i < size; ++i)
    if (data[i] > 127)
      return i;

  return size;
}

bool convert_buffer_latin1(EsString::pointer& out_buffer, size_t& out_length, const void* contents, size_t size, bool is_mutable)
{
  const uint8_t* data = static_cast<const uint8_t*>(contents);
  size_t data_length = size;

  // get size of prefix that does not need utf8 conversion
  size_t prefix_length = get_latin1_7bit_prefix_length(data, data_length);
  ES_ASSERT(prefix_length <= data_length);

  const uint8_t* postfix = data + prefix_length;
  size_t postfix_length = data_length - prefix_length;

  // if no conversion is needed, just return the original buffer
  if (postfix_length == 0) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

  // first pass: get length in utf8 units
  size_t length = prefix_length + utf_decoder<utf8_counter>::decode_latin1_block(postfix, postfix_length, 0);

  // allocate buffer of suitable length
  EsString::pointer buffer = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
  if (!buffer) return false;

  // second pass: convert latin1 input to utf8
  memcpy(buffer, data, prefix_length);

  uint8_t* obegin = reinterpret_cast<uint8_t*>(buffer);
  uint8_t* oend = utf_decoder<utf8_writer>::decode_latin1_block(postfix, postfix_length, obegin + prefix_length);

  ES_ASSERT(oend == obegin + length);
  *oend = 0;

  out_buffer = buffer;
  out_length = length + 1;

  return true;
}

bool convert_buffer(EsString::pointer& out_buffer, size_t& out_length, EsXmlEncoding encoding, const void* contents, size_t size, bool is_mutable)
{
  // fast path: no conversion required
  if (encoding == xmlEncodingUtf8) return get_mutable_buffer(out_buffer, out_length, contents, size, is_mutable);

  // source encoding is utf16
  if (encoding == xmlEncodingUtf16_be || encoding == xmlEncodingUtf16_le)
  {
    EsXmlEncoding native_encoding = ES_XMLUTF16_ENDIAN;

    return (native_encoding == encoding) ?
      convert_buffer_utf16(out_buffer, out_length, contents, size, opt_false()) :
      convert_buffer_utf16(out_buffer, out_length, contents, size, opt_true());
  }

  // source encoding is utf32
  if (encoding == xmlEncodingUtf32_be || encoding == xmlEncodingUtf32_le)
  {
    EsXmlEncoding native_encoding = ES_XMLUTF32_ENDIAN;

    return (native_encoding == encoding) ?
      convert_buffer_utf32(out_buffer, out_length, contents, size, opt_false()) :
      convert_buffer_utf32(out_buffer, out_length, contents, size, opt_true());
  }

  // source encoding is latin1
  if (encoding == xmlEncodingLatin1) return convert_buffer_latin1(out_buffer, out_length, contents, size, is_mutable);

  ES_ASSERT(!"Invalid encoding");
  return false;
}
#endif

size_t as_utf8_begin(const ES_WCHAR* str, size_t length)
{
  // get length in utf8 characters
  return utf_decoder<utf8_counter>::decode_wchar_block(str, length, 0);
}

void as_utf8_end(char* buffer, size_t size, const ES_WCHAR* str, size_t length)
{
  // convert to utf8
  uint8_t* begin = reinterpret_cast<uint8_t*>(buffer);
  uint8_t* end = utf_decoder<utf8_writer>::decode_wchar_block(str, length, begin);

  ES_ASSERT(begin + size == end);
  (void)!end;

  // zero-terminate
  buffer[size] = 0;
}

EsByteString as_utf8_impl(const ES_WCHAR* str, size_t length)
{
  // first pass: get length in utf8 characters
  size_t size = as_utf8_begin(str, length);

  // allocate resulting string
  EsByteString result;
  result.resize(size);

  // second pass: convert to utf8
  if (size > 0) as_utf8_end(&result[0], size, str, length);

  return result;
}

EsWideString as_wide_impl(const char* str, size_t size)
{
  const uint8_t* data = reinterpret_cast<const uint8_t*>(str);

  // first pass: get length in ES_WCHAR units
  size_t length = utf_decoder<wchar_counter>::decode_utf8_block(data, size, 0);

  // allocate resulting string
  EsWideString result;
  result.resize(length);

  // second pass: convert to ES_WCHAR
  if (length > 0)
  {
    wchar_writer::value_type begin = reinterpret_cast<wchar_writer::value_type>(&result[0]);
    wchar_writer::value_type end = utf_decoder<wchar_writer>::decode_utf8_block(data, size, begin);

    ES_ASSERT(begin + length == end);
    (void)!end;
  }

  return result;
}

EsByteString as_utf8(const ES_WCHAR* str)
{
  ES_ASSERT(str);

  return as_utf8_impl(str, strlength_wide(str));
}

EsByteString as_utf8(const EsWideString& str)
{
  return as_utf8_impl(str.c_str(), str.size());
}

EsWideString as_wide(const char* str)
{
  ES_ASSERT(str);

  return as_wide_impl(str, strlen(str));
}

EsWideString as_wide(const EsByteString& str)
{
  return as_wide_impl(str.c_str(), str.size());
}

inline bool strcpy_insitu_allow(size_t length, uintptr_t allocated, EsString::pointer target)
{
  ES_ASSERT(target);
  size_t target_length = strlength(target);

  // always reuse document buffer memory if possible
  if (!allocated) return target_length >= length;

  // reuse heap memory if waste is not too great
  const size_t reuse_threshold = 32;

  return target_length >= length && (target_length < reuse_threshold || target_length - length < target_length / 2);
}

bool strcpy_insitu(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, EsString::const_pointer source)
{
  ES_ASSERT(header);

  size_t source_length = strlength(source);

  if (source_length == 0)
  {
    // empty string and null pointer are equivalent, so just deallocate old memory
    EsXmlAllocator* alloc = reinterpret_cast<EsXmlMemoryPage*>(header & xml_memory_page_pointer_mask)->allocator;

    if (header & header_mask) alloc->deallocate_string(dest);

    // mark the string as not allocated
    dest = 0;
    header &= ~header_mask;

    return true;
  }
  else if (dest && strcpy_insitu_allow(source_length, header & header_mask, dest))
  {
    // we can reuse old buffer, so just copy the new data (including zero terminator)
    memcpy(dest, source, (source_length + 1) * sizeof(EsString::value_type));

    return true;
  }
  else
  {
    EsXmlAllocator* alloc = reinterpret_cast<EsXmlMemoryPage*>(header & xml_memory_page_pointer_mask)->allocator;

    // allocate new buffer
    EsString::pointer buf = alloc->allocate_string(source_length + 1);
    if (!buf) return false;

    // copy the string (including zero terminator)
    memcpy(buf, source, (source_length + 1) * sizeof(EsString::value_type));

    // deallocate old buffer (*after* the above to protect against overlapping memory and/or allocation failures)
    if (header & header_mask) alloc->deallocate_string(dest);

    // the string is now allocated, so set the flag
    dest = buf;
    header |= header_mask;

    return true;
  }
}

struct gap
{
  EsString::pointer end;
  size_t size;

  gap(): end(0), size(0)
  {
  }

  // Push new gap, move s count bytes further (skipping the gap).
  // Collapse previous gap.
  void push(EsString::pointer& s, size_t count)
  {
    if (end) // there was a gap already; collapse it
    {
      // Move [old_gap_end, new_gap_start) to [old_gap_start, ...)
      ES_ASSERT(s >= end);
      memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));
    }

    s += count; // end of current gap

    // "merge" two gaps
    end = s;
    size += count;
  }

  // Collapse all gaps, return past-the-end pointer
  EsString::pointer flush(EsString::pointer s)
  {
    if (end)
    {
      // Move [old_gap_end, current_pos) to [old_gap_start, ...)
      ES_ASSERT(s >= end);
      memmove(end - size, end, reinterpret_cast<char*>(s) - reinterpret_cast<char*>(end));

      return s - size;
    }
    else return s;
  }
};

EsString::pointer strconv_escape(EsString::pointer s, gap& g)
{
  EsString::pointer stre = s + 1;

  switch (*stre)
  {
    case '#':  // &#...
    {
      unsigned int ucsc = 0;

      if (stre[1] == 'x') // &#x... (hex code)
      {
        stre += 2;

        EsString::value_type ch = *stre;

        if (ch == ';') return stre;

        for (;;)
        {
          if (static_cast<unsigned int>(ch - '0') <= 9)
            ucsc = 16 * ucsc + (ch - '0');
          else if (static_cast<unsigned int>((ch | ' ') - 'a') <= 5)
            ucsc = 16 * ucsc + ((ch | ' ') - 'a' + 10);
          else if (ch == ';')
            break;
          else // cancel
            return stre;

          ch = *++stre;
        }

        ++stre;
      }
      else  // &#... (dec code)
      {
        EsString::value_type ch = *++stre;

        if (ch == ';') return stre;

        for (;;)
        {
          if (static_cast<unsigned int>(static_cast<unsigned int>(ch) - '0') <= 9)
            ucsc = 10 * ucsc + (ch - '0');
          else if (ch == ';')
            break;
          else // cancel
            return stre;

          ch = *++stre;
        }

        ++stre;
      }

    #if !defined(ES_USE_NARROW_ES_CHAR)
      s = reinterpret_cast<EsString::pointer>(wchar_writer::any(reinterpret_cast<wchar_writer::value_type>(s), ucsc));
    #else
      s = reinterpret_cast<EsString::pointer>(utf8_writer::any(reinterpret_cast<uint8_t*>(s), ucsc));
    #endif

      g.push(s, stre - s);
      return stre;
    }

    case 'a':  // &a
    {
      ++stre;

      if (*stre == 'm') // &am
      {
        if (*++stre == 'p' && *++stre == ';') // &amp;
        {
          *s++ = '&';
          ++stre;

          g.push(s, stre - s);
          return stre;
        }
      }
      else if (*stre == 'p') // &ap
      {
        if (*++stre == 'o' && *++stre == 's' && *++stre == ';') // &apos;
        {
          *s++ = '\'';
          ++stre;

          g.push(s, stre - s);
          return stre;
        }
      }
      break;
    }

    case 'g': // &g
    {
      if (*++stre == 't' && *++stre == ';') // &gt;
      {
        *s++ = '>';
        ++stre;

        g.push(s, stre - s);
        return stre;
      }
      break;
    }

    case 'l': // &l
    {
      if (*++stre == 't' && *++stre == ';') // &lt;
      {
        *s++ = '<';
        ++stre;

        g.push(s, stre - s);
        return stre;
      }
      break;
    }

    case 'q': // &q
    {
      if (*++stre == 'u' && *++stre == 'o' && *++stre == 't' && *++stre == ';') // &quot;
      {
        *s++ = '"';
        ++stre;

        g.push(s, stre - s);
        return stre;
      }
      break;
    }

    default:
      break;
  }

  return stre;
}

// Utility macro for last character handling
#define ENDSWITH(c, e) ((c) == (e) || ((c) == 0 && endch == (e)))

EsString::pointer strconv_comment(EsString::pointer s, EsString::value_type endch)
{
  gap g;

  while (true)
  {
    while (!PUGI__IS_CHARTYPE(*s, ct_parse_comment)) ++s;

    if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
    {
      *s++ = '\n'; // replace first one with 0x0a

      if (*s == '\n') g.push(s, 1);
    }
    else if (s[0] == '-' && s[1] == '-' && ENDSWITH(s[2], '>')) // comment ends here
    {
      *g.flush(s) = 0;

      return s + (s[2] == '>' ? 3 : 2);
    }
    else if (*s == 0)
    {
      return 0;
    }
    else ++s;
  }
}

EsString::pointer strconv_cdata(EsString::pointer s, EsString::value_type endch)
{
  gap g;

  while (true)
  {
    while (!PUGI__IS_CHARTYPE(*s, ct_parse_cdata)) ++s;

    if (*s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
    {
      *s++ = '\n'; // replace first one with 0x0a

      if (*s == '\n') g.push(s, 1);
    }
    else if (s[0] == ']' && s[1] == ']' && ENDSWITH(s[2], '>')) // CDATA ends here
    {
      *g.flush(s) = 0;

      return s + 1;
    }
    else if (*s == 0)
    {
      return 0;
    }
    else ++s;
  }
}

typedef EsString::pointer (*strconv_pcdata_t)(EsString::pointer);

template <typename opt_trim, typename opt_eol, typename opt_escape> struct strconv_pcdata_impl
{
  static EsString::pointer parse(EsString::pointer s)
  {
    gap g;

    EsString::pointer begin = s;

    while (true)
    {
      while (!PUGI__IS_CHARTYPE(*s, ct_parse_pcdata)) ++s;

      if (*s == '<') // PCDATA ends here
      {
        EsString::pointer end = g.flush(s);

        if (opt_trim::value)
          while (end > begin && PUGI__IS_CHARTYPE(end[-1], ct_space))
            --end;

        *end = 0;

        return s + 1;
      }
      else if (opt_eol::value && *s == '\r') // Either a single 0x0d or 0x0d 0x0a pair
      {
        *s++ = '\n'; // replace first one with 0x0a

        if (*s == '\n') g.push(s, 1);
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (*s == 0)
      {
        EsString::pointer end = g.flush(s);

        if (opt_trim::value)
          while (end > begin && PUGI__IS_CHARTYPE(end[-1], ct_space))
            --end;

        *end = 0;

        return s;
      }
      else ++s;
    }
  }
};

strconv_pcdata_t get_strconv_pcdata(unsigned int optmask)
{
  ES_COMPILE_TIME_ASSERT(xmlParseEscapes == 0x10 && xmlParseNormalizeEol == 0x20 && xmlParseTrimPcdata == 0x0800, parseFlagsCheck);

  switch (((optmask >> 4) & 3) | ((optmask >> 9) & 4)) // get bitmask for flags (eol escapes trim)
  {
  case 0: return strconv_pcdata_impl<opt_false, opt_false, opt_false>::parse;
  case 1: return strconv_pcdata_impl<opt_false, opt_false, opt_true>::parse;
  case 2: return strconv_pcdata_impl<opt_false, opt_true, opt_false>::parse;
  case 3: return strconv_pcdata_impl<opt_false, opt_true, opt_true>::parse;
  case 4: return strconv_pcdata_impl<opt_true, opt_false, opt_false>::parse;
  case 5: return strconv_pcdata_impl<opt_true, opt_false, opt_true>::parse;
  case 6: return strconv_pcdata_impl<opt_true, opt_true, opt_false>::parse;
  case 7: return strconv_pcdata_impl<opt_true, opt_true, opt_true>::parse;
  default: ES_ASSERT(false); return 0; // should not get here
  }
}

typedef EsString::pointer (*strconv_attribute_t)(EsString::pointer, EsString::value_type);

template <typename opt_escape> struct strconv_attribute_impl
{
  static EsString::pointer parse_wnorm(EsString::pointer s, EsString::value_type end_quote)
  {
    gap g;

    // trim leading whitespaces
    if (PUGI__IS_CHARTYPE(*s, ct_space))
    {
      EsString::pointer str = s;

      do ++str;
      while (PUGI__IS_CHARTYPE(*str, ct_space));

      g.push(s, str - s);
    }

    while (true)
    {
      while (!PUGI__IS_CHARTYPE(*s, ct_parse_attr_ws | ct_space)) ++s;

      if (*s == end_quote)
      {
        EsString::pointer str = g.flush(s);

        do *str-- = 0;
        while (PUGI__IS_CHARTYPE(*str, ct_space));

        return s + 1;
      }
      else if (PUGI__IS_CHARTYPE(*s, ct_space))
      {
        *s++ = ' ';

        if (PUGI__IS_CHARTYPE(*s, ct_space))
        {
          EsString::pointer str = s + 1;
          while (PUGI__IS_CHARTYPE(*str, ct_space)) ++str;

          g.push(s, str - s);
        }
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else ++s;
    }
  }

  static EsString::pointer parse_wconv(EsString::pointer s, EsString::value_type end_quote)
  {
    gap g;

    while (true)
    {
      while (!PUGI__IS_CHARTYPE(*s, ct_parse_attr_ws)) ++s;

      if (*s == end_quote)
      {
        *g.flush(s) = 0;

        return s + 1;
      }
      else if (PUGI__IS_CHARTYPE(*s, ct_space))
      {
        if (*s == '\r')
        {
          *s++ = ' ';

          if (*s == '\n') g.push(s, 1);
        }
        else *s++ = ' ';
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else ++s;
    }
  }

  static EsString::pointer xmlParseNormalizeEol(EsString::pointer s, EsString::value_type end_quote)
  {
    gap g;

    while (true)
    {
      while (!PUGI__IS_CHARTYPE(*s, ct_parse_attr)) ++s;

      if (*s == end_quote)
      {
        *g.flush(s) = 0;

        return s + 1;
      }
      else if (*s == '\r')
      {
        *s++ = '\n';

        if (*s == '\n') g.push(s, 1);
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else ++s;
    }
  }

  static EsString::pointer parse_simple(EsString::pointer s, EsString::value_type end_quote)
  {
    gap g;

    while (true)
    {
      while (!PUGI__IS_CHARTYPE(*s, ct_parse_attr)) ++s;

      if (*s == end_quote)
      {
        *g.flush(s) = 0;

        return s + 1;
      }
      else if (opt_escape::value && *s == '&')
      {
        s = strconv_escape(s, g);
      }
      else if (!*s)
      {
        return 0;
      }
      else ++s;
    }
  }
};

strconv_attribute_t get_strconv_attribute(unsigned int optmask)
{
  ES_COMPILE_TIME_ASSERT(xmlParseEscapes == 0x10 && xmlParseNormalizeEol == 0x20 && xmlParseWconvAttribute == 0x40 && xmlParseWnormAttribute == 0x80, xmlParseFlagsCheck);

  switch ((optmask >> 4) & 15) // get bitmask for flags (wconv wnorm eol escapes)
  {
  case 0:  return strconv_attribute_impl<opt_false>::parse_simple;
  case 1:  return strconv_attribute_impl<opt_true>::parse_simple;
  case 2:  return strconv_attribute_impl<opt_false>::xmlParseNormalizeEol;
  case 3:  return strconv_attribute_impl<opt_true>::xmlParseNormalizeEol;
  case 4:  return strconv_attribute_impl<opt_false>::parse_wconv;
  case 5:  return strconv_attribute_impl<opt_true>::parse_wconv;
  case 6:  return strconv_attribute_impl<opt_false>::parse_wconv;
  case 7:  return strconv_attribute_impl<opt_true>::parse_wconv;
  case 8:  return strconv_attribute_impl<opt_false>::parse_wnorm;
  case 9:  return strconv_attribute_impl<opt_true>::parse_wnorm;
  case 10: return strconv_attribute_impl<opt_false>::parse_wnorm;
  case 11: return strconv_attribute_impl<opt_true>::parse_wnorm;
  case 12: return strconv_attribute_impl<opt_false>::parse_wnorm;
  case 13: return strconv_attribute_impl<opt_true>::parse_wnorm;
  case 14: return strconv_attribute_impl<opt_false>::parse_wnorm;
  case 15: return strconv_attribute_impl<opt_true>::parse_wnorm;
  default: ES_ASSERT(false); return 0; // should not get here
  }
}

inline EsXmlParseResult make_parse_result(EsXmlParseStatus status, ptrdiff_t offset = 0)
{
  EsXmlParseResult result;
  result.status = status;
  result.offset = offset;

  return result;
}

struct EsXmlParser
{
  EsXmlAllocator alloc;
  EsString::pointer error_offset;
  EsXmlParseStatus error_status;

  // Parser utilities.
  #define PUGI__SKIPWS()      { while (PUGI__IS_CHARTYPE(*s, ct_space)) ++s; }
  #define PUGI__OPTSET(OPT)      ( optmsk & (OPT) )
  #define PUGI__PUSHNODE(TYPE)    { cursor = append_node(cursor, alloc, TYPE); if (!cursor) PUGI__THROW_ERROR(xmlParseStatusOutOfMemory, s); }
  #define PUGI__POPNODE()      { cursor = cursor->parent; }
  #define PUGI__SCANFOR(X)      { while (*s != 0 && !(X)) ++s; }
  #define PUGI__SCANWHILE(X)    { while ((X)) ++s; }
  #define PUGI__ENDSEG()      { ch = *s; *s = 0; ++s; }
  #define PUGI__THROW_ERROR(err, m)  return error_offset = m, error_status = err, static_cast<EsString::pointer>(0)
  #define PUGI__CHECK_ERROR(err, m)  { if (*s == 0) PUGI__THROW_ERROR(err, m); }

  EsXmlParser(const EsXmlAllocator& alloc_): alloc(alloc_), error_offset(0), error_status(xmlParseStatusOk)
  {
  }

  // DOCTYPE consists of nested sections of the following possible types:
  // <!-- ... -->, <? ... ?>, "...", '...'
  // <![...]]>
  // <!...>
  // First group can not contain nested groups
  // Second group can contain nested groups of the same type
  // Third group can contain all other groups
  EsString::pointer parse_doctype_primitive(EsString::pointer s)
  {
    if (*s == '"' || *s == '\'')
    {
      // quoted string
      EsString::value_type ch = *s++;
      PUGI__SCANFOR(*s == ch);
      if (!*s) PUGI__THROW_ERROR(xmlParseStatusBadDoctype, s);

      s++;
    }
    else if (s[0] == '<' && s[1] == '?')
    {
      // <? ... ?>
      s += 2;
      PUGI__SCANFOR(s[0] == '?' && s[1] == '>'); // no need for ENDSWITH because ?> can't terminate proper doctype
      if (!*s) PUGI__THROW_ERROR(xmlParseStatusBadDoctype, s);

      s += 2;
    }
    else if (s[0] == '<' && s[1] == '!' && s[2] == '-' && s[3] == '-')
    {
      s += 4;
      PUGI__SCANFOR(s[0] == '-' && s[1] == '-' && s[2] == '>'); // no need for ENDSWITH because --> can't terminate proper doctype
      if (!*s) PUGI__THROW_ERROR(xmlParseStatusBadDoctype, s);

      s += 4;
    }
    else PUGI__THROW_ERROR(xmlParseStatusBadDoctype, s);

    return s;
  }

  EsString::pointer parse_doctype_ignore(EsString::pointer s)
  {
    ES_ASSERT(s[0] == '<' && s[1] == '!' && s[2] == '[');
    s++;

    while (*s)
    {
      if (s[0] == '<' && s[1] == '!' && s[2] == '[')
      {
        // nested ignore section
        s = parse_doctype_ignore(s);
        if (!s) return s;
      }
      else if (s[0] == ']' && s[1] == ']' && s[2] == '>')
      {
        // ignore section end
        s += 3;

        return s;
      }
      else s++;
    }

    PUGI__THROW_ERROR(xmlParseStatusBadDoctype, s);
  }

  EsString::pointer parse_doctype_group(EsString::pointer s, EsString::value_type endch, bool toplevel)
  {
    ES_ASSERT((s[0] == '<' || s[0] == 0) && s[1] == '!');
    s++;

    while (*s)
    {
      if (s[0] == '<' && s[1] == '!' && s[2] != '-')
      {
        if (s[2] == '[')
        {
          // ignore
          s = parse_doctype_ignore(s);
          if (!s) return s;
        }
        else
        {
          // some control group
          s = parse_doctype_group(s, endch, false);
          if (!s) return s;

          // skip >
          ES_ASSERT(*s == '>');
          s++;
        }
      }
      else if (s[0] == '<' || s[0] == '"' || s[0] == '\'')
      {
        // unknown tag (forbidden), or some primitive group
        s = parse_doctype_primitive(s);
        if (!s) return s;
      }
      else if (*s == '>')
      {
        return s;
      }
      else s++;
    }

    if (!toplevel || endch != '>') PUGI__THROW_ERROR(xmlParseStatusBadDoctype, s);

    return s;
  }

  EsString::pointer parse_exclamation(EsString::pointer s, EsXmlNodeStruct* cursor, unsigned int optmsk, EsString::value_type endch)
  {
    // parse node contents, starting with exclamation mark
    ++s;

    if (*s == '-') // '<!-...'
    {
      ++s;

      if (*s == '-') // '<!--...'
      {
        ++s;

        if (PUGI__OPTSET(xmlParseComments))
        {
          PUGI__PUSHNODE(xmlNodeComment); // Append a new node on the tree.
          cursor->value = s; // Save the offset.
        }

        if (PUGI__OPTSET(xmlParseNormalizeEol) && PUGI__OPTSET(xmlParseComments))
        {
          s = strconv_comment(s, endch);

          if (!s) PUGI__THROW_ERROR(xmlParseStatusBadComment, cursor->value);
        }
        else
        {
          // Scan for terminating '-->'.
          PUGI__SCANFOR(s[0] == '-' && s[1] == '-' && ENDSWITH(s[2], '>'));
          PUGI__CHECK_ERROR(xmlParseStatusBadComment, s);

          if (PUGI__OPTSET(xmlParseComments))
            *s = 0; // Zero-terminate this segment at the first terminating '-'.

          s += (s[2] == '>' ? 3 : 2); // Step over the '\0->'.
        }
      }
      else PUGI__THROW_ERROR(xmlParseStatusBadComment, s);
    }
    else if (*s == '[')
    {
      // '<![CDATA[...'
      if (*++s=='C' && *++s=='D' && *++s=='A' && *++s=='T' && *++s=='A' && *++s == '[')
      {
        ++s;

        if (PUGI__OPTSET(xmlParseCdata))
        {
          PUGI__PUSHNODE(xmlNodeCdata); // Append a new node on the tree.
          cursor->value = s; // Save the offset.

          if (PUGI__OPTSET(xmlParseNormalizeEol))
          {
            s = strconv_cdata(s, endch);

            if (!s) PUGI__THROW_ERROR(xmlParseStatusBadCdata, cursor->value);
          }
          else
          {
            // Scan for terminating ']]>'.
            PUGI__SCANFOR(s[0] == ']' && s[1] == ']' && ENDSWITH(s[2], '>'));
            PUGI__CHECK_ERROR(xmlParseStatusBadCdata, s);

            *s++ = 0; // Zero-terminate this segment.
          }
        }
        else // Flagged for discard, but we still have to scan for the terminator.
        {
          // Scan for terminating ']]>'.
          PUGI__SCANFOR(s[0] == ']' && s[1] == ']' && ENDSWITH(s[2], '>'));
          PUGI__CHECK_ERROR(xmlParseStatusBadCdata, s);

          ++s;
        }

        s += (s[1] == '>' ? 2 : 1); // Step over the last ']>'.
      }
      else PUGI__THROW_ERROR(xmlParseStatusBadCdata, s);
    }
    else if (s[0] == 'D' && s[1] == 'O' && s[2] == 'C' && s[3] == 'T' && s[4] == 'Y' && s[5] == 'P' && ENDSWITH(s[6], 'E'))
    {
      s -= 2;

      if (cursor->parent) PUGI__THROW_ERROR(xmlParseStatusBadDoctype, s);

      EsString::pointer mark = s + 9;

      s = parse_doctype_group(s, endch, true);
      if (!s) return s;

      ES_ASSERT((*s == 0 && endch == '>') || *s == '>');
      if (*s) *s++ = 0;

      if (PUGI__OPTSET(xmlParseDoctype))
      {
        while (PUGI__IS_CHARTYPE(*mark, ct_space)) ++mark;

        PUGI__PUSHNODE(xmlNodeDoctype);

        cursor->value = mark;

        PUGI__POPNODE();
      }
    }
    else if (*s == 0 && endch == '-') PUGI__THROW_ERROR(xmlParseStatusBadComment, s);
    else if (*s == 0 && endch == '[') PUGI__THROW_ERROR(xmlParseStatusBadCdata, s);
    else PUGI__THROW_ERROR(xmlParseStatusUnrecognizedTag, s);

    return s;
  }

  EsString::pointer parse_question(EsString::pointer s, EsXmlNodeStruct*& ref_cursor, unsigned int optmsk, EsString::value_type endch)
  {
    // load into registers
    EsXmlNodeStruct* cursor = ref_cursor;
    EsString::value_type ch = 0;

    // parse node contents, starting with question mark
    ++s;

    // read PI target
    EsString::pointer target = s;

    if (!PUGI__IS_CHARTYPE(*s, ct_start_symbol)) PUGI__THROW_ERROR(xmlParseStatusBadProcInstr, s);

    PUGI__SCANWHILE(PUGI__IS_CHARTYPE(*s, ct_symbol));
    PUGI__CHECK_ERROR(xmlParseStatusBadProcInstr, s);

    // determine node type; stricmp / strcasecmp is not portable
    bool declaration = (target[0] | ' ') == 'x' && (target[1] | ' ') == 'm' && (target[2] | ' ') == 'l' && target + 3 == s;

    if (declaration ? PUGI__OPTSET(xmlParseDeclaration) : PUGI__OPTSET(xmlParseProcInstr))
    {
      if (declaration)
      {
        // disallow non top-level declarations
        if (cursor->parent) PUGI__THROW_ERROR(xmlParseStatusBadProcInstr, s);

        PUGI__PUSHNODE(xmlNodeDocDecl);
      }
      else
      {
        PUGI__PUSHNODE(xmlNodeProcInstr);
      }

      cursor->name = target;

      PUGI__ENDSEG();

      // parse value/attributes
      if (ch == '?')
      {
        // empty node
        if (!ENDSWITH(*s, '>')) PUGI__THROW_ERROR(xmlParseStatusBadProcInstr, s);
        s += (*s == '>');

        PUGI__POPNODE();
      }
      else if (PUGI__IS_CHARTYPE(ch, ct_space))
      {
        PUGI__SKIPWS();

        // scan for tag end
        EsString::pointer value = s;

        PUGI__SCANFOR(s[0] == '?' && ENDSWITH(s[1], '>'));
        PUGI__CHECK_ERROR(xmlParseStatusBadProcInstr, s);

        if (declaration)
        {
          // replace ending ? with / so that 'element' terminates properly
          *s = '/';

          // we exit from this function with cursor at xmlNodeDocDecl, which is a signal to parse() to go to LOC_ATTRIBUTES
          s = value;
        }
        else
        {
          // store value and step over >
          cursor->value = value;
          PUGI__POPNODE();

          PUGI__ENDSEG();

          s += (*s == '>');
        }
      }
      else PUGI__THROW_ERROR(xmlParseStatusBadProcInstr, s);
    }
    else
    {
      // scan for tag end
      PUGI__SCANFOR(s[0] == '?' && ENDSWITH(s[1], '>'));
      PUGI__CHECK_ERROR(xmlParseStatusBadProcInstr, s);

      s += (s[1] == '>' ? 2 : 1);
    }

    // store from registers
    ref_cursor = cursor;

    return s;
  }

  EsString::pointer parse_tree(EsString::pointer s, EsXmlNodeStruct* root, unsigned int optmsk, EsString::value_type endch)
  {
    strconv_attribute_t strconv_attribute = get_strconv_attribute(optmsk);
    strconv_pcdata_t strconv_pcdata = get_strconv_pcdata(optmsk);

    EsString::value_type ch = 0;
    EsXmlNodeStruct* cursor = root;
    EsString::pointer mark = s;

    while (*s != 0)
    {
      if (*s == '<')
      {
        ++s;

      LOC_TAG:
        if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) // '<#...'
        {
          PUGI__PUSHNODE(xmlNodeElement); // Append a new node to the tree.

          cursor->name = s;

          PUGI__SCANWHILE(PUGI__IS_CHARTYPE(*s, ct_symbol)); // Scan for a terminator.
          PUGI__ENDSEG(); // Save char in 'ch', terminate & step over.

          if (ch == '>')
          {
            // end of tag
          }
          else if (PUGI__IS_CHARTYPE(ch, ct_space))
          {
          LOC_ATTRIBUTES:
            while (true)
            {
              PUGI__SKIPWS(); // Eat any whitespace.

              if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) // <... #...
              {
                EsXmlAttributeStruct* a = append_attribute_ll(cursor, alloc); // Make space for this attribute.
                if (!a) PUGI__THROW_ERROR(xmlParseStatusOutOfMemory, s);

                a->name = s; // Save the offset.

                PUGI__SCANWHILE(PUGI__IS_CHARTYPE(*s, ct_symbol)); // Scan for a terminator.
                PUGI__CHECK_ERROR(xmlParseStatusBadAttribute, s); //$ redundant, left for performance

                PUGI__ENDSEG(); // Save char in 'ch', terminate & step over.
                PUGI__CHECK_ERROR(xmlParseStatusBadAttribute, s); //$ redundant, left for performance

                if (PUGI__IS_CHARTYPE(ch, ct_space))
                {
                  PUGI__SKIPWS(); // Eat any whitespace.
                  PUGI__CHECK_ERROR(xmlParseStatusBadAttribute, s); //$ redundant, left for performance

                  ch = *s;
                  ++s;
                }

                if (ch == '=') // '<... #=...'
                {
                  PUGI__SKIPWS(); // Eat any whitespace.

                  if (*s == '"' || *s == '\'') // '<... #="...'
                  {
                    ch = *s; // Save quote char to avoid breaking on "''" -or- '""'.
                    ++s; // Step over the quote.
                    a->value = s; // Save the offset.

                    s = strconv_attribute(s, ch);

                    if (!s) PUGI__THROW_ERROR(xmlParseStatusBadAttribute, a->value);

                    // After this line the loop continues from the start;
                    // Whitespaces, / and > are ok, symbols and EOF are wrong,
                    // everything else will be detected
                    if (PUGI__IS_CHARTYPE(*s, ct_start_symbol)) PUGI__THROW_ERROR(xmlParseStatusBadAttribute, s);
                  }
                  else PUGI__THROW_ERROR(xmlParseStatusBadAttribute, s);
                }
                else PUGI__THROW_ERROR(xmlParseStatusBadAttribute, s);
              }
              else if (*s == '/')
              {
                ++s;

                if (*s == '>')
                {
                  PUGI__POPNODE();
                  s++;
                  break;
                }
                else if (*s == 0 && endch == '>')
                {
                  PUGI__POPNODE();
                  break;
                }
                else PUGI__THROW_ERROR(xmlParseStatusBadStartElement, s);
              }
              else if (*s == '>')
              {
                ++s;

                break;
              }
              else if (*s == 0 && endch == '>')
              {
                break;
              }
              else PUGI__THROW_ERROR(xmlParseStatusBadStartElement, s);
            }

            // !!!
          }
          else if (ch == '/') // '<#.../'
          {
            if (!ENDSWITH(*s, '>')) PUGI__THROW_ERROR(xmlParseStatusBadStartElement, s);

            PUGI__POPNODE(); // Pop.

            s += (*s == '>');
          }
          else if (ch == 0)
          {
            // we stepped over null terminator, backtrack & handle closing tag
            --s;

            if (endch != '>') PUGI__THROW_ERROR(xmlParseStatusBadStartElement, s);
          }
          else PUGI__THROW_ERROR(xmlParseStatusBadStartElement, s);
        }
        else if (*s == '/')
        {
          ++s;

          EsString::pointer name = cursor->name;
          if (!name) PUGI__THROW_ERROR(xmlParseStatusEndElementMismatch, s);

          while (PUGI__IS_CHARTYPE(*s, ct_symbol))
          {
            if (*s++ != *name++) PUGI__THROW_ERROR(xmlParseStatusEndElementMismatch, s);
          }

          if (*name)
          {
            if (*s == 0 && name[0] == endch && name[1] == 0) PUGI__THROW_ERROR(xmlParseStatusBadEndElement, s);
            else PUGI__THROW_ERROR(xmlParseStatusEndElementMismatch, s);
          }

          PUGI__POPNODE(); // Pop.

          PUGI__SKIPWS();

          if (*s == 0)
          {
            if (endch != '>') PUGI__THROW_ERROR(xmlParseStatusBadEndElement, s);
          }
          else
          {
            if (*s != '>') PUGI__THROW_ERROR(xmlParseStatusBadEndElement, s);
            ++s;
          }
        }
        else if (*s == '?') // '<?...'
        {
          s = parse_question(s, cursor, optmsk, endch);
          if (!s) return s;

          ES_ASSERT(cursor);
          if ((cursor->header & xml_memory_page_type_mask) + 1 == xmlNodeDocDecl) goto LOC_ATTRIBUTES;
        }
        else if (*s == '!') // '<!...'
        {
          s = parse_exclamation(s, cursor, optmsk, endch);
          if (!s) return s;
        }
        else if (*s == 0 && endch == '?') PUGI__THROW_ERROR(xmlParseStatusBadProcInstr, s);
        else PUGI__THROW_ERROR(xmlParseStatusUnrecognizedTag, s);
      }
      else
      {
        mark = s; // Save this offset while searching for a terminator.

        PUGI__SKIPWS(); // Eat whitespace if no genuine PCDATA here.

        if (*s == '<' || !*s)
        {
          // We skipped some whitespace characters because otherwise we would take the tag branch instead of PCDATA one
          ES_ASSERT(mark != s);

          if (!PUGI__OPTSET(xmlParseWhitespacePcdata | xmlParseWhitespacePcdataSingle) || PUGI__OPTSET(xmlParseTrimPcdata))
          {
            continue;
          }
          else if (PUGI__OPTSET(xmlParseWhitespacePcdataSingle))
          {
            if (s[0] != '<' || s[1] != '/' || cursor->first_child) continue;
          }
        }

        if (!PUGI__OPTSET(xmlParseTrimPcdata))
          s = mark;

        if (cursor->parent || PUGI__OPTSET(xmlParseFragment))
        {
          PUGI__PUSHNODE(xmlNodePcdata); // Append a new node on the tree.
          cursor->value = s; // Save the offset.

          s = strconv_pcdata(s);

          PUGI__POPNODE(); // Pop since this is a standalone.

          if (!*s) break;
        }
        else
        {
          PUGI__SCANFOR(*s == '<'); // '...<'
          if (!*s) break;

          ++s;
        }

        // We're after '<'
        goto LOC_TAG;
      }
    }

    // check that last tag is closed
    if (cursor != root) PUGI__THROW_ERROR(xmlParseStatusEndElementMismatch, s);

    return s;
  }

#if !defined(ES_USE_NARROW_ES_CHAR)
  static EsString::pointer parse_skip_bom(EsString::pointer s)
  {
    unsigned int bom = 0xfeff;
    return (s[0] == static_cast<ES_WCHAR>(bom)) ? s + 1 : s;
  }
#else
  static EsString::pointer parse_skip_bom(EsString::pointer s)
  {
    return (s[0] == '\xef' && s[1] == '\xbb' && s[2] == '\xbf') ? s + 3 : s;
  }
#endif

  static bool has_element_node_siblings(EsXmlNodeStruct* node)
  {
    while (node)
    {
      EsXmlNodeType type = static_cast<EsXmlNodeType>((node->header & xml_memory_page_type_mask) + 1);
      if (type == xmlNodeElement) return true;

      node = node->next_sibling;
    }

    return false;
  }

  static EsXmlParseResult parse(EsString::pointer buffer, size_t length, EsXmlDocumentStruct* xmldoc, EsXmlNodeStruct* root, unsigned int optmsk)
  {
    // allocator object is a part of document object
    EsXmlAllocator& alloc = *static_cast<EsXmlAllocator*>(xmldoc);

    // early-out for empty documents
    if (length == 0)
      return make_parse_result(PUGI__OPTSET(xmlParseFragment) ? xmlParseStatusOk : xmlParseStatusNoDocumentElement);

    // get last child of the root before parsing
    EsXmlNodeStruct* last_root_child = root->first_child ? root->first_child->prev_sibling_c : 0;

    // create parser on stack
    EsXmlParser parser(alloc);

    // save last character and make buffer zero-terminated (speeds up parsing)
    EsString::value_type endch = buffer[length - 1];
    buffer[length - 1] = 0;

    // skip BOM to make sure it does not end up as part of parse output
    EsString::pointer buffer_data = parse_skip_bom(buffer);

    // perform actual parsing
    parser.parse_tree(buffer_data, root, optmsk, endch);

    // update allocator state
    alloc = parser.alloc;

    EsXmlParseResult result = make_parse_result(parser.error_status, parser.error_offset ? parser.error_offset - buffer : 0);
    ES_ASSERT(result.offset >= 0 && static_cast<size_t>(result.offset) <= length);

    if (result)
    {
      // since we removed last character, we have to handle the only possible false positive (stray <)
      if (endch == '<')
        return make_parse_result(xmlParseStatusUnrecognizedTag, length - 1);

      // check if there are any element nodes parsed
      EsXmlNodeStruct* first_root_child_parsed = last_root_child ? last_root_child->next_sibling : root->first_child;

      if (!PUGI__OPTSET(xmlParseFragment) && !has_element_node_siblings(first_root_child_parsed))
        return make_parse_result(xmlParseStatusNoDocumentElement, length - 1);
    }
    else
    {
      // roll back offset if it occurs on a null terminator in the source buffer
      if (result.offset > 0 && static_cast<size_t>(result.offset) == length - 1 && endch == 0)
        result.offset--;
    }

    return result;
  }
};

// Output facilities
EsXmlEncoding get_write_native_encoding()
{
#if !defined(ES_USE_NARROW_ES_CHAR)
  return get_wchar_encoding();
#else
  return xmlEncodingUtf8;
#endif
}

EsXmlEncoding get_write_encoding(EsXmlEncoding encoding)
{
  // replace wchar encoding with utf implementation
  if (encoding == xmlEncodingWchar) return get_wchar_encoding();

  // replace utf16 encoding with utf16 with specific endianness
  if (encoding == xmlEncodingUtf16) return ES_XMLUTF16_ENDIAN;

  // replace utf32 encoding with utf32 with specific endianness
  if (encoding == xmlEncodingUtf32) return ES_XMLUTF32_ENDIAN;

  // only do autodetection if no explicit encoding is requested
  if (encoding != xmlEncodingAuto) return encoding;

  // assume utf8 encoding
  return xmlEncodingUtf8;
}

#if !defined(ES_USE_NARROW_ES_CHAR)
size_t get_valid_length(EsString::const_pointer data, size_t length)
{
  ES_ASSERT(length > 0);

  // discard last character if it's the lead of a surrogate pair
  return (sizeof(ES_WCHAR) == 2 && static_cast<unsigned int>(static_cast<uint16_t>(data[length - 1]) - 0xD800) < 0x400) ? length - 1 : length;
}

size_t convert_buffer_output(EsString::pointer r_char, uint8_t* r_u8, uint16_t* r_u16, uint32_t* r_u32, EsString::const_pointer data, size_t length, EsXmlEncoding encoding)
{
  // only endian-swapping is required
  if (need_endian_swap_utf(encoding, get_wchar_encoding()))
  {
    convert_wchar_endian_swap(r_char, data, length);

    return length * sizeof(EsString::value_type);
  }

  // convert to utf8
  if (encoding == xmlEncodingUtf8)
  {
    uint8_t* dest = r_u8;
    uint8_t* end = utf_decoder<utf8_writer>::decode_wchar_block<EsString::value_type>(data, length, dest);

    return static_cast<size_t>(end - dest);
  }

  // convert to utf16
  if (encoding == xmlEncodingUtf16_be || encoding == xmlEncodingUtf16_le)
  {
    uint16_t* dest = r_u16;

    // convert to native utf16
    uint16_t* end = utf_decoder<utf16_writer>::decode_wchar_block<EsString::value_type>(data, length, dest);

    // swap if necessary
    EsXmlEncoding native_encoding = ES_XMLUTF16_ENDIAN;

    if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

    return static_cast<size_t>(end - dest) * sizeof(uint16_t);
  }

  // convert to utf32
  if (encoding == xmlEncodingUtf32_be || encoding == xmlEncodingUtf32_le)
  {
    uint32_t* dest = r_u32;

    // convert to native utf32
    uint32_t* end = utf_decoder<utf32_writer>::decode_wchar_block(data, length, dest);

    // swap if necessary
    EsXmlEncoding native_encoding = ES_XMLUTF32_ENDIAN;

    if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

    return static_cast<size_t>(end - dest) * sizeof(uint32_t);
  }

  // convert to latin1
  if (encoding == xmlEncodingLatin1)
  {
    uint8_t* dest = r_u8;
    uint8_t* end = utf_decoder<latin1_writer>::decode_wchar_block(data, length, dest);

    return static_cast<size_t>(end - dest);
  }

  ES_ASSERT(!"Invalid encoding");
  return 0;
}
#else
size_t get_valid_length(EsString::const_pointer data, size_t length)
{
  ES_ASSERT(length > 4);

  for (size_t i = 1; i <= 4; ++i)
  {
    uint8_t ch = static_cast<uint8_t>(data[length - i]);

    // either a standalone character or a leading one
    if ((ch & 0xc0) != 0x80) return length - i;
  }

  // there are four non-leading characters at the end, sequence tail is broken so might as well process the whole chunk
  return length;
}

size_t convert_buffer_output(EsString::pointer /* r_char */, uint8_t* r_u8, uint16_t* r_u16, uint32_t* r_u32, EsString::const_pointer data, size_t length, EsXmlEncoding encoding)
{
  if (encoding == xmlEncodingUtf16_be || encoding == xmlEncodingUtf16_le)
  {
    uint16_t* dest = r_u16;

    // convert to native utf16
    uint16_t* end = utf_decoder<utf16_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

    // swap if necessary
    EsXmlEncoding native_encoding = ES_XMLUTF16_ENDIAN;

    if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

    return static_cast<size_t>(end - dest) * sizeof(uint16_t);
  }

  if (encoding == xmlEncodingUtf32_be || encoding == xmlEncodingUtf32_le)
  {
    uint32_t* dest = r_u32;

    // convert to native utf32
    uint32_t* end = utf_decoder<utf32_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

    // swap if necessary
    EsXmlEncoding native_encoding = ES_XMLUTF32_ENDIAN;

    if (native_encoding != encoding) convert_utf_endian_swap(dest, dest, static_cast<size_t>(end - dest));

    return static_cast<size_t>(end - dest) * sizeof(uint32_t);
  }

  if (encoding == xmlEncodingLatin1)
  {
    uint8_t* dest = r_u8;
    uint8_t* end = utf_decoder<latin1_writer>::decode_utf8_block(reinterpret_cast<const uint8_t*>(data), length, dest);

    return static_cast<size_t>(end - dest);
  }

  ES_ASSERT(!"Invalid encoding");
  return 0;
}
#endif

class EsXmlBufferedWriter
{
  EsXmlBufferedWriter(const EsXmlBufferedWriter&);
  EsXmlBufferedWriter& operator=(const EsXmlBufferedWriter&);

public:
  EsXmlBufferedWriter(EsXmlWriterIntf& writer_, EsXmlEncoding user_encoding): writer(writer_), bufsize(0), encoding(get_write_encoding(user_encoding))
  {
    ES_COMPILE_TIME_ASSERT(bufcapacity >= 8, bufcapacityCheck);
  }

  ~EsXmlBufferedWriter()
  {
    flush();
  }

  void flush()
  {
    flush(buffer, bufsize);
    bufsize = 0;
  }

  void flush(EsString::const_pointer data, size_t size)
  {
    if (size == 0) return;

    // fast path, just write data
    if (encoding == get_write_native_encoding())
      writer.write(data, size * sizeof(EsString::value_type));
    else
    {
      // convert chunk
      size_t result = convert_buffer_output(scratch.data_char, scratch.data_u8, scratch.data_u16, scratch.data_u32, data, size, encoding);
      ES_ASSERT(result <= sizeof(scratch));

      // write data
      writer.write(scratch.data_u8, result);
    }
  }

  void write(EsString::const_pointer data, size_t length)
  {
    if (bufsize + length > bufcapacity)
    {
      // flush the remaining buffer contents
      flush();

      // handle large chunks
      if (length > bufcapacity)
      {
        if (encoding == get_write_native_encoding())
        {
          // fast path, can just write data chunk
          writer.write(data, length * sizeof(EsString::value_type));
          return;
        }

        // need to convert in suitable chunks
        while (length > bufcapacity)
        {
          // get chunk size by selecting such number of characters that are guaranteed to fit into scratch buffer
          // and form a complete codepoint sequence (i.e. discard start of last codepoint if necessary)
          size_t chunk_size = get_valid_length(data, bufcapacity);

          // convert chunk and write
          flush(data, chunk_size);

          // iterate
          data += chunk_size;
          length -= chunk_size;
        }

        // small tail is copied below
        bufsize = 0;
      }
    }

    memcpy(buffer + bufsize, data, length * sizeof(EsString::value_type));
    bufsize += length;
  }

  void write(EsString::const_pointer data)
  {
    write(data, strlength(data));
  }

  void write(EsString::value_type d0)
  {
    if (bufsize + 1 > bufcapacity) flush();

    buffer[bufsize + 0] = d0;
    bufsize += 1;
  }

  void write(EsString::value_type d0, EsString::value_type d1)
  {
    if (bufsize + 2 > bufcapacity) flush();

    buffer[bufsize + 0] = d0;
    buffer[bufsize + 1] = d1;
    bufsize += 2;
  }

  void write(EsString::value_type d0, EsString::value_type d1, EsString::value_type d2)
  {
    if (bufsize + 3 > bufcapacity) flush();

    buffer[bufsize + 0] = d0;
    buffer[bufsize + 1] = d1;
    buffer[bufsize + 2] = d2;
    bufsize += 3;
  }

  void write(EsString::value_type d0, EsString::value_type d1, EsString::value_type d2, EsString::value_type d3)
  {
    if (bufsize + 4 > bufcapacity) flush();

    buffer[bufsize + 0] = d0;
    buffer[bufsize + 1] = d1;
    buffer[bufsize + 2] = d2;
    buffer[bufsize + 3] = d3;
    bufsize += 4;
  }

  void write(EsString::value_type d0, EsString::value_type d1, EsString::value_type d2, EsString::value_type d3, EsString::value_type d4)
  {
    if (bufsize + 5 > bufcapacity) flush();

    buffer[bufsize + 0] = d0;
    buffer[bufsize + 1] = d1;
    buffer[bufsize + 2] = d2;
    buffer[bufsize + 3] = d3;
    buffer[bufsize + 4] = d4;
    bufsize += 5;
  }

  void write(EsString::value_type d0, EsString::value_type d1, EsString::value_type d2, EsString::value_type d3, EsString::value_type d4, EsString::value_type d5)
  {
    if (bufsize + 6 > bufcapacity) flush();

    buffer[bufsize + 0] = d0;
    buffer[bufsize + 1] = d1;
    buffer[bufsize + 2] = d2;
    buffer[bufsize + 3] = d3;
    buffer[bufsize + 4] = d4;
    buffer[bufsize + 5] = d5;
    bufsize += 6;
  }

  // utf8 maximum expansion: x4 (-> utf32)
  // utf16 maximum expansion: x2 (-> utf32)
  // utf32 maximum expansion: x1
  enum
  {
    bufcapacitybytes =
    #ifdef PUGIXML_MEMORY_OUTPUT_STACK
      PUGIXML_MEMORY_OUTPUT_STACK
    #else
      10240
    #endif
    ,
    bufcapacity = bufcapacitybytes / (sizeof(EsString::value_type) + 4)
  };

  EsString::value_type buffer[bufcapacity];

  union
  {
    uint8_t data_u8[4 * bufcapacity];
    uint16_t data_u16[2 * bufcapacity];
    uint32_t data_u32[bufcapacity];
    EsString::value_type data_char[bufcapacity * (sizeof(uint32_t)/sizeof(EsString::value_type))];

  } scratch;

  EsXmlWriterIntf& writer;
  size_t bufsize;
  EsXmlEncoding encoding;
};

void text_output_escaped(EsXmlBufferedWriter& writer, EsString::const_pointer s, chartypex_t type)
{
  while (*s)
  {
    EsString::const_pointer prev = s;

    // While *s is a usual symbol
    while (!PUGI__IS_CHARTYPEX(*s, type)) ++s;

    writer.write(prev, static_cast<size_t>(s - prev));

    switch (*s)
    {
      case 0: break;
      case '&':
        writer.write('&', 'a', 'm', 'p', ';');
        ++s;
        break;
      case '<':
        writer.write('&', 'l', 't', ';');
        ++s;
        break;
      case '>':
        writer.write('&', 'g', 't', ';');
        ++s;
        break;
      case '"':
        writer.write('&', 'q', 'u', 'o', 't', ';');
        ++s;
        break;
      default: // s is not a usual symbol
      {
        unsigned int ch = static_cast<unsigned int>(*s++);
        ES_ASSERT(ch < 32);

        writer.write('&', '#', static_cast<EsString::value_type>((ch / 10) + '0'), static_cast<EsString::value_type>((ch % 10) + '0'), ';');
      }
    }
  }
}

void text_output(EsXmlBufferedWriter& writer, EsString::const_pointer s, chartypex_t type, unsigned int flags)
{
  if (flags & xmlFormatNoEscapes)
    writer.write(s);
  else
    text_output_escaped(writer, s, type);
}

void text_output_cdata(EsXmlBufferedWriter& writer, EsString::const_pointer s)
{
  do
  {
    writer.write('<', '!', '[', 'C', 'D');
    writer.write('A', 'T', 'A', '[');

    EsString::const_pointer prev = s;

    // look for ]]> sequence - we can't output it as is since it terminates CDATA
    while (*s && !(s[0] == ']' && s[1] == ']' && s[2] == '>')) ++s;

    // skip ]] if we stopped at ]]>, > will go to the next CDATA section
    if (*s) s += 2;

    writer.write(prev, static_cast<size_t>(s - prev));

    writer.write(']', ']', '>');
  }
  while (*s);
}

void node_output_attributes(EsXmlBufferedWriter& writer, const EsXmlNode& node, unsigned int flags)
{
  EsString::const_pointer default_name = esT(":anonymous");

  for (EsXmlAttribute a = node.firstAttributeGet(); a; a = a.nextAttributeGet())
  {
    writer.write(' ');
    writer.write(a.nameGet()[0] ? a.nameGet() : default_name);
    writer.write('=', '"');

    text_output(writer, a.valueGet(), ctx_special_attr, flags);

    writer.write('"');
  }
}

void node_output(EsXmlBufferedWriter& writer, const EsXmlNode& node, EsString::const_pointer indent, unsigned int flags, unsigned int depth)
{
  EsString::const_pointer default_name = esT(":anonymous");

  if ((flags & xmlFormatIndent) != 0 && (flags & xmlFormatRaw) == 0)
    for (unsigned int i = 0; i < depth; ++i) writer.write(indent);

  switch (node.typeGet())
  {
  case xmlNodeDocument:
  {
    for (EsXmlNode n = node.firstChildGet(); n; n = n.nextSiblingGet())
      node_output(writer, n, indent, flags, depth);
    break;
  }

  case xmlNodeElement:
  {
    EsString::const_pointer name = node.nameGet()[0] ? node.nameGet() : default_name;

    writer.write('<');
    writer.write(name);

    node_output_attributes(writer, node, flags);

    if (flags & xmlFormatRaw)
    {
      if (!node.firstChildGet())
        writer.write(' ', '/', '>');
      else
      {
        writer.write('>');

        for (EsXmlNode n = node.firstChildGet(); n; n = n.nextSiblingGet())
          node_output(writer, n, indent, flags, depth + 1);

        writer.write('<', '/');
        writer.write(name);
        writer.write('>');
      }
    }
    else if (!node.firstChildGet())
      writer.write(' ', '/', '>', '\n');
    else if (node.firstChildGet() == node.lastChildGet() && (node.firstChildGet().typeGet() == xmlNodePcdata || node.firstChildGet().typeGet() == xmlNodeCdata))
    {
      writer.write('>');

      if (node.firstChildGet().typeGet() == xmlNodePcdata)
        text_output(writer, node.firstChildGet().valueGet(), ctx_special_pcdata, flags);
      else
        text_output_cdata(writer, node.firstChildGet().valueGet());

      writer.write('<', '/');
      writer.write(name);
      writer.write('>', '\n');
    }
    else
    {
      writer.write('>', '\n');

      for (EsXmlNode n = node.firstChildGet(); n; n = n.nextSiblingGet())
        node_output(writer, n, indent, flags, depth + 1);

      if ((flags & xmlFormatIndent) != 0 && (flags & xmlFormatRaw) == 0)
        for (unsigned int i = 0; i < depth; ++i) writer.write(indent);

      writer.write('<', '/');
      writer.write(name);
      writer.write('>', '\n');
    }

    break;
  }

  case xmlNodePcdata:
    text_output(writer, node.valueGet(), ctx_special_pcdata, flags);
    if ((flags & xmlFormatRaw) == 0) writer.write('\n');
    break;

  case xmlNodeCdata:
    text_output_cdata(writer, node.valueGet());
    if ((flags & xmlFormatRaw) == 0) writer.write('\n');
    break;

  case xmlNodeComment:
    writer.write('<', '!', '-', '-');
    writer.write(node.valueGet());
    writer.write('-', '-', '>');
    if ((flags & xmlFormatRaw) == 0) writer.write('\n');
    break;

  case xmlNodeProcInstr:
  case xmlNodeDocDecl:
    writer.write('<', '?');
    writer.write(node.nameGet()[0] ? node.nameGet() : default_name);

    if (node.typeGet() == xmlNodeDocDecl)
    {
      node_output_attributes(writer, node, flags);
    }
    else if (node.valueGet()[0])
    {
      writer.write(' ');
      writer.write(node.valueGet());
    }

    writer.write('?', '>');
    if ((flags & xmlFormatRaw) == 0) writer.write('\n');
    break;

  case xmlNodeDoctype:
    writer.write('<', '!', 'D', 'O', 'C');
    writer.write('T', 'Y', 'P', 'E');

    if (node.valueGet()[0])
    {
      writer.write(' ');
      writer.write(node.valueGet());
    }

    writer.write('>');
    if ((flags & xmlFormatRaw) == 0) writer.write('\n');
    break;

  default:
    ES_ASSERT(!"Invalid node type");
  }
}

inline bool has_declaration(const EsXmlNode& node)
{
  for (EsXmlNode child = node.firstChildGet(); child; child = child.nextSiblingGet())
  {
    EsXmlNodeType type = child.typeGet();

    if (type == xmlNodeDocDecl) return true;
    if (type == xmlNodeElement) return false;
  }

  return false;
}

inline bool allow_insert_child(EsXmlNodeType parent, EsXmlNodeType child)
{
  if (parent != xmlNodeDocument && parent != xmlNodeElement) return false;
  if (child == xmlNodeDocument || child == xmlNodeNull) return false;
  if (parent != xmlNodeDocument && (child == xmlNodeDocDecl || child == xmlNodeDoctype)) return false;

  return true;
}

void recursive_copy_skip(EsXmlNode& dest, const EsXmlNode& source, const EsXmlNode& skip)
{
  ES_ASSERT(dest.typeGet() == source.typeGet());

  switch (source.typeGet())
  {
  case xmlNodeElement:
  {
    dest.nameSet(source.nameGet());

    for (EsXmlAttribute a = source.firstAttributeGet(); a; a = a.nextAttributeGet())
      dest.attributeAppend(a.nameGet()).valueSet(a.valueGet());

    for (EsXmlNode c = source.firstChildGet(); c; c = c.nextSiblingGet())
    {
      if (c == skip) continue;

      EsXmlNode cc = dest.childAppend(c.typeGet());
      ES_ASSERT(cc);

      recursive_copy_skip(cc, c, skip);
    }

    break;
  }

  case xmlNodePcdata:
  case xmlNodeCdata:
  case xmlNodeComment:
  case xmlNodeDoctype:
    dest.valueSet(source.valueGet());
    break;

  case xmlNodeProcInstr:
    dest.nameSet(source.nameGet());
    dest.valueSet(source.valueGet());
    break;

  case xmlNodeDocDecl:
  {
    dest.nameSet(source.nameGet());

    for (EsXmlAttribute a = source.firstAttributeGet(); a; a = a.nextAttributeGet())
      dest.attributeAppend(a.nameGet()).valueSet(a.valueGet());

    break;
  }

  default:
    ES_ASSERT(!"Invalid node type");
  }
}

inline bool is_text_node(EsXmlNodeStruct* node)
{
  EsXmlNodeType type = static_cast<EsXmlNodeType>((node->header & xml_memory_page_type_mask) + 1);

  return type == xmlNodePcdata || type == xmlNodeCdata;
}

// get value with conversion functions
int get_integer_base(EsString::const_pointer value)
{
  EsString::const_pointer s = value;

  while (PUGI__IS_CHARTYPE(*s, ct_space))
    s++;

  if (*s == '-')
    s++;

  return (s[0] == '0' && (s[1] == 'x' || s[1] == 'X')) ? 16 : 10;
}

int get_value_int(EsString::const_pointer value, int def)
{
  if (!value) return def;

  try
  {
    int base = get_integer_base(value);
    return static_cast<int>(EsString::toLong(value, base));
  }
  catch(...)
  {
  }

  return def;
}

unsigned int get_value_uint(EsString::const_pointer value, unsigned int def)
{
  if (!value) return def;

  try
  {
    int base = get_integer_base(value);
    return static_cast<unsigned int>(EsString::toULong(value, base));
  }
  catch(...)
  {
  }

  return def;
}

double get_value_double(EsString::const_pointer value, double def)
{
  if (!value) return def;

  try
  {
    return EsString::toDouble(value);
  }
  catch(...)
  {
  }

  return def;
}

float get_value_float(EsString::const_pointer value, float def)
{
  if (!value) return def;

  try
  {
    return static_cast<float>(EsString::toDouble(value));
  }
  catch(...)
  {
  }

  return def;
}

bool get_value_bool(EsString::const_pointer value, bool def)
{
  if (!value) return def;

  try
  {
    // only look at first char
    EsString::value_type first = *value;

    // 1*, t* (true), T* (True), y* (yes), Y* (YES)
    return (first == '1' || first == 't' || first == 'T' || first == 'y' || first == 'Y');
  }
  catch(...)
  {
  }

  return def;
}

long long get_value_llong(EsString::const_pointer value, long long def)
{
  if (!value) return def;

  try
  {
    int base = get_integer_base(value);
    return EsString::toInt64(value, base);
  }
  catch(...)
  {
  }

  return def;
}

unsigned long long get_value_ullong(EsString::const_pointer value, unsigned long long def)
{
  if (!value) return def;

  try
  {
    int base = get_integer_base(value);

    return EsString::toUInt64(value, base);
  }
  catch(...)
  {
  }

  return def;
}

// set value with conversion functions
bool set_value_buffer(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, char (&buf)[128])
{
#if !defined(ES_USE_NARROW_ES_CHAR)
  EsString::value_type wbuf[128];
  widen_ascii(wbuf, buf);

  return strcpy_insitu(dest, header, header_mask, wbuf);
#else
  return strcpy_insitu(dest, header, header_mask, buf);
#endif
}

bool set_value_convert(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, int value)
{
  char buf[128];
  sprintf(buf, "%d", value);

  return set_value_buffer(dest, header, header_mask, buf);
}

bool set_value_convert(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, unsigned int value)
{
  char buf[128];
  sprintf(buf, "%u", value);

  return set_value_buffer(dest, header, header_mask, buf);
}

bool set_value_convert(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, double value)
{
  char buf[128];
  sprintf(buf, "%g", value);

  return set_value_buffer(dest, header, header_mask, buf);
}

bool set_value_convert(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, bool value)
{
  return strcpy_insitu(dest, header, header_mask, value ? esT("true") : esT("false"));
}

bool set_value_convert(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, long long value)
{
  char buf[128];
  sprintf(buf, "%lld", value);

  return set_value_buffer(dest, header, header_mask, buf);
}

bool set_value_convert(EsString::pointer& dest, uintptr_t& header, uintptr_t header_mask, unsigned long long value)
{
  char buf[128];
  sprintf(buf, "%llu", value);

  return set_value_buffer(dest, header, header_mask, buf);
}

EsXmlParseResult load_buffer_impl(EsXmlDocumentStruct* doc, EsXmlNodeStruct* root, void* contents, size_t size, unsigned int options, EsXmlEncoding encoding, bool is_mutable, bool own, EsString::pointer* out_buffer)
{
  // check input buffer
  ES_ASSERT(contents || size == 0);

  // get actual encoding
  EsXmlEncoding buffer_encoding = get_buffer_encoding(encoding, contents, size);

  // get private buffer
  EsString::pointer buffer = 0;
  size_t length = 0;

  if (!convert_buffer(buffer, length, buffer_encoding, contents, size, is_mutable)) return make_parse_result(xmlParseStatusOutOfMemory);

  // delete original buffer if we performed a conversion
  if (own && buffer != contents && contents) EsXmlMemory::deallocate(contents);

  // store buffer for offsetDebugGet
  doc->buffer = buffer;

  // parse
  EsXmlParseResult res = EsXmlParser::parse(buffer, length, doc, root, options);

  // remember encoding
  res.encoding = buffer_encoding;

  // grab onto buffer if it's our buffer, user is responsible for deallocating contents himself
  if (own || buffer != contents) *out_buffer = buffer;

  return res;
}

bool EsXmlDocument::saveFile(const EsString& path, const EsString& indent, unsigned int flags, EsXmlEncoding encoding) const
{
  EsFile file(path, static_cast<ulong>(EsFileFlag::Write)|static_cast<ulong>(EsFileFlag::Exclusive));
  if( file.open() )
  {
    EsXmlWriterFile writer(file);
    save(writer, indent, flags, encoding);
    file.close();
    return true;
  }
  else
    EsException::ThrowOsError( file.recentErrorGet() );

  return false;
}

static EsXmlParseResult load_file_impl(EsXmlDocument& doc, EsFile& file, unsigned int options, EsXmlEncoding encoding)
{
  // get file size (can result in I/O errors)
  EsXmlParseStatus size_status = xmlParseStatusOk;
  size_t size = (size_t)file.get_length();

  // allocate buffer for the whole file
  char* contents = static_cast<char*>(EsXmlMemory::allocate(size > 0 ? size : 1));

  if (!contents)
  {
    file.close();
    return make_parse_result(xmlParseStatusOutOfMemory);
  }

  // read file in memory
  size_t read_size = (size_t)file.rawRead(contents, size);
  file.close();

  if(read_size != size)
  {
    EsXmlMemory::deallocate(contents);
    return make_parse_result(xmlParseStatusIoError);
  }

  return doc.loadBufferInplaceOwn(contents, size, options, encoding);
}

EsXmlParseResult EsXmlDocument::loadFile(const EsString& path, unsigned int options, EsXmlEncoding encoding)
{
  reset();
  EsFile file(path, static_cast<ulong>(EsFileFlag::Read)|static_cast<ulong>(EsFileFlag::Exclusive));
  if( file.open() )
    return load_file_impl(*this, file, options, encoding);
  else
    EsException::ThrowOsError( file.recentErrorGet() );

  // pacify compiler
  return make_parse_result( xmlParseStatusFileNotFound );
}
//---------------------------------------------------------------------------

// xml writer specifications for basic escore classes
//
// EsFile writer
EsXmlWriterFile::EsXmlWriterFile(EsFile& file) :
m_file(file)
{}

void EsXmlWriterFile::write(const void* data, size_t size)
{
  m_file.rawWrite(data, size);
}
//---------------------------------------------------------------------------

// EsByteString writer
EsXmlWriterByteString::EsXmlWriterByteString(EsByteString& str) :
m_str(str)
{}

void EsXmlWriterByteString::write(const void* data, size_t size)
{
  m_str += EsByteString(static_cast<EsByteString::const_pointer>(data), size);
}
//---------------------------------------------------------------------------

// EsXmlWriterIntf implementation for EsBinBuffer
EsXmlWriterBinBuffer::EsXmlWriterBinBuffer(EsBinBuffer& buff) :
m_buff(buff)
{}

void EsXmlWriterBinBuffer::write(const void* data, size_t size)
{
  m_buff.append(size, static_cast<const esU8*>(data));
}
//---------------------------------------------------------------------------

EsXmlTreeWalker::EsXmlTreeWalker(): _depth(0)
{
}

EsXmlTreeWalker::~EsXmlTreeWalker()
{
}

int EsXmlTreeWalker::depth() const
{
  return _depth;
}

bool EsXmlTreeWalker::begin(EsXmlNode&)
{
  return true;
}

bool EsXmlTreeWalker::end(EsXmlNode&)
{
  return true;
}

EsXmlAttribute::EsXmlAttribute(): _attr(0)
{
}

EsXmlAttribute::EsXmlAttribute(EsXmlAttributeStruct* attr): _attr(attr)
{
}

static void unspecified_bool_xml_attribute(EsXmlAttribute***)
{
}

EsXmlAttribute::operator EsXmlAttribute::unspecified_bool_type() const
{
  return _attr ? unspecified_bool_xml_attribute : 0;
}

bool EsXmlAttribute::operator!() const
{
  return !_attr;
}

bool EsXmlAttribute::operator==(const EsXmlAttribute& r) const
{
  return (_attr == r._attr);
}

bool EsXmlAttribute::operator!=(const EsXmlAttribute& r) const
{
  return (_attr != r._attr);
}

bool EsXmlAttribute::operator<(const EsXmlAttribute& r) const
{
  return (_attr < r._attr);
}

bool EsXmlAttribute::operator>(const EsXmlAttribute& r) const
{
  return (_attr > r._attr);
}

bool EsXmlAttribute::operator<=(const EsXmlAttribute& r) const
{
  return (_attr <= r._attr);
}

bool EsXmlAttribute::operator>=(const EsXmlAttribute& r) const
{
  return (_attr >= r._attr);
}

EsXmlAttribute EsXmlAttribute::nextAttributeGet() const
{
  return _attr ? EsXmlAttribute(_attr->next_attribute) : EsXmlAttribute();
}

EsXmlAttribute EsXmlAttribute::previousAttributeGet() const
{
  return _attr && _attr->prev_attribute_c->next_attribute ? EsXmlAttribute(_attr->prev_attribute_c) : EsXmlAttribute();
}

EsString::const_pointer EsXmlAttribute::asString(const EsString& def) const
{
  return (_attr && _attr->value) ? _attr->value : def.c_str();
}

int EsXmlAttribute::asInt(int def) const
{
  return get_value_int(_attr ? _attr->value : 0, def);
}

unsigned int EsXmlAttribute::asUint(unsigned int def) const
{
  return get_value_uint(_attr ? _attr->value : 0, def);
}

double EsXmlAttribute::asDouble(double def) const
{
  return get_value_double(_attr ? _attr->value : 0, def);
}

float EsXmlAttribute::asFloat(float def) const
{
  return get_value_float(_attr ? _attr->value : 0, def);
}

bool EsXmlAttribute::asBool(bool def) const
{
  return get_value_bool(_attr ? _attr->value : 0, def);
}

long long EsXmlAttribute::asLLong(long long def) const
{
  return get_value_llong(_attr ? _attr->value : 0, def);
}

unsigned long long EsXmlAttribute::asULLong(unsigned long long def) const
{
  return get_value_ullong(_attr ? _attr->value : 0, def);
}

bool EsXmlAttribute::empty() const
{
  return !_attr;
}

EsString::const_pointer EsXmlAttribute::nameGet() const
{
  return (_attr && _attr->name) ? _attr->name : esT("");
}

EsString::const_pointer EsXmlAttribute::valueGet() const
{
  return (_attr && _attr->value) ? _attr->value : esT("");
}

size_t EsXmlAttribute::hashGet() const
{
  return static_cast<size_t>(reinterpret_cast<uintptr_t>(_attr) / sizeof(EsXmlAttributeStruct));
}

EsXmlAttributeStruct* EsXmlAttribute::internalObjectGet() const
{
  return _attr;
}

EsXmlAttribute& EsXmlAttribute::operator=(EsString::const_pointer rhs)
{
  valueSet(rhs);
  return *this;
}

EsXmlAttribute& EsXmlAttribute::operator=(int rhs)
{
  valueSet(rhs);
  return *this;
}

EsXmlAttribute& EsXmlAttribute::operator=(unsigned int rhs)
{
  valueSet(rhs);
  return *this;
}

EsXmlAttribute& EsXmlAttribute::operator=(double rhs)
{
  valueSet(rhs);
  return *this;
}

EsXmlAttribute& EsXmlAttribute::operator=(bool rhs)
{
  valueSet(rhs);
  return *this;
}

#ifdef PUGIXML_HAS_LONG_LONG
EsXmlAttribute& EsXmlAttribute::operator=(long long rhs)
{
  valueSet(rhs);
  return *this;
}

EsXmlAttribute& EsXmlAttribute::operator=(unsigned long long rhs)
{
  valueSet(rhs);
  return *this;
}
#endif

bool EsXmlAttribute::nameSet(EsString::const_pointer rhs)
{
  if (!_attr) return false;

  return strcpy_insitu(_attr->name, _attr->header, xml_memory_page_name_allocated_mask, rhs);
}

bool EsXmlAttribute::valueSet(EsString::const_pointer rhs)
{
  if (!_attr) return false;

  return strcpy_insitu(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
}

bool EsXmlAttribute::valueSet(int rhs)
{
  if (!_attr) return false;

  return set_value_convert(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
}

bool EsXmlAttribute::valueSet(unsigned int rhs)
{
  if (!_attr) return false;

  return set_value_convert(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
}

bool EsXmlAttribute::valueSet(double rhs)
{
  if (!_attr) return false;

  return set_value_convert(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
}

bool EsXmlAttribute::valueSet(bool rhs)
{
  if (!_attr) return false;

  return set_value_convert(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
}

#ifdef PUGIXML_HAS_LONG_LONG
bool EsXmlAttribute::valueSet(long long rhs)
{
  if (!_attr) return false;

  return set_value_convert(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
}

bool EsXmlAttribute::valueSet(unsigned long long rhs)
{
  if (!_attr) return false;

  return set_value_convert(_attr->value, _attr->header, xml_memory_page_value_allocated_mask, rhs);
}
#endif

#ifdef __BORLANDC__
bool operator&&(const EsXmlAttribute& lhs, bool rhs)
{
  return (bool)lhs && rhs;
}

bool operator||(const EsXmlAttribute& lhs, bool rhs)
{
  return (bool)lhs || rhs;
}
#endif

EsXmlNode::EsXmlNode(): _root(0)
{
}

EsXmlNode::EsXmlNode(EsXmlNodeStruct* p): _root(p)
{
}

static void unspecified_bool_xml_node(EsXmlNode***)
{
}

EsXmlNode::operator EsXmlNode::unspecified_bool_type() const
{
  return _root ? unspecified_bool_xml_node : 0;
}

bool EsXmlNode::operator!() const
{
  return !_root;
}

EsXmlNode::iterator EsXmlNode::begin() const
{
  return iterator(_root ? _root->first_child : 0, _root);
}

EsXmlNode::iterator EsXmlNode::end() const
{
  return iterator(0, _root);
}

EsXmlNode::attribute_iterator EsXmlNode::attributes_begin() const
{
  return attribute_iterator(_root ? _root->first_attribute : 0, _root);
}

EsXmlNode::attribute_iterator EsXmlNode::attributes_end() const
{
  return attribute_iterator(0, _root);
}

EsXmlObjectRange<EsXmlNodeIterator> EsXmlNode::children() const
{
  return EsXmlObjectRange<EsXmlNodeIterator>(begin(), end());
}

EsXmlObjectRange<EsXmlNamedNodeIterator> EsXmlNode::children(EsString::const_pointer name_) const
{
  return EsXmlObjectRange<EsXmlNamedNodeIterator>(EsXmlNamedNodeIterator(childGet(name_)._root, _root, name_), EsXmlNamedNodeIterator(0, _root, name_));
}

EsXmlObjectRange<EsXmlAttributeIterator> EsXmlNode::attributes() const
{
  return EsXmlObjectRange<EsXmlAttributeIterator>(attributes_begin(), attributes_end());
}

bool EsXmlNode::operator==(const EsXmlNode& r) const
{
  return (_root == r._root);
}

bool EsXmlNode::operator!=(const EsXmlNode& r) const
{
  return (_root != r._root);
}

bool EsXmlNode::operator<(const EsXmlNode& r) const
{
  return (_root < r._root);
}

bool EsXmlNode::operator>(const EsXmlNode& r) const
{
  return (_root > r._root);
}

bool EsXmlNode::operator<=(const EsXmlNode& r) const
{
  return (_root <= r._root);
}

bool EsXmlNode::operator>=(const EsXmlNode& r) const
{
  return (_root >= r._root);
}

bool EsXmlNode::empty() const
{
  return !_root;
}

EsString::const_pointer EsXmlNode::nameGet() const
{
  return (_root && _root->name) ? _root->name : esT("");
}

EsXmlNodeType EsXmlNode::typeGet() const
{
  return _root ? static_cast<EsXmlNodeType>((_root->header & xml_memory_page_type_mask) + 1) : xmlNodeNull;
}

EsString::const_pointer EsXmlNode::valueGet() const
{
  return (_root && _root->value) ? _root->value : esT("");
}

EsXmlNode EsXmlNode::childGet(EsString::const_pointer name_) const
{
  if (!_root) return EsXmlNode();

  for (EsXmlNodeStruct* i = _root->first_child; i; i = i->next_sibling)
    if (i->name && strequal(name_, i->name)) return EsXmlNode(i);

  return EsXmlNode();
}

EsXmlAttribute EsXmlNode::attributeGet(EsString::const_pointer name_) const
{
  if (!_root) return EsXmlAttribute();

  for (EsXmlAttributeStruct* i = _root->first_attribute; i; i = i->next_attribute)
    if (i->name && strequal(name_, i->name))
      return EsXmlAttribute(i);

  return EsXmlAttribute();
}

EsXmlNode EsXmlNode::nextSiblingGet(EsString::const_pointer name_) const
{
  if (!_root) return EsXmlNode();

  for (EsXmlNodeStruct* i = _root->next_sibling; i; i = i->next_sibling)
    if (i->name && strequal(name_, i->name)) return EsXmlNode(i);

  return EsXmlNode();
}

EsXmlNode EsXmlNode::nextSiblingGet() const
{
  if (!_root) return EsXmlNode();

  if (_root->next_sibling) return EsXmlNode(_root->next_sibling);
  else return EsXmlNode();
}

EsXmlNode EsXmlNode::previousSiblingGet(EsString::const_pointer name_) const
{
  if (!_root) return EsXmlNode();

  for (EsXmlNodeStruct* i = _root->prev_sibling_c; i->next_sibling; i = i->prev_sibling_c)
    if (i->name && strequal(name_, i->name)) return EsXmlNode(i);

  return EsXmlNode();
}

EsXmlNode EsXmlNode::previousSiblingGet() const
{
  if (!_root) return EsXmlNode();

  if (_root->prev_sibling_c->next_sibling) return EsXmlNode(_root->prev_sibling_c);
  else return EsXmlNode();
}

EsXmlNode EsXmlNode::parentGet() const
{
  return _root ? EsXmlNode(_root->parent) : EsXmlNode();
}

EsXmlNode EsXmlNode::rootGet() const
{
  if (!_root) return EsXmlNode();

  EsXmlMemoryPage* page = reinterpret_cast<EsXmlMemoryPage*>(_root->header & xml_memory_page_pointer_mask);

  return EsXmlNode(static_cast<EsXmlDocumentStruct*>(page->allocator));
}

EsXmlText EsXmlNode::textGet() const
{
  return EsXmlText(_root);
}

EsString::const_pointer EsXmlNode::childValueGet() const
{
  if (!_root) return esT("");

  for (EsXmlNodeStruct* i = _root->first_child; i; i = i->next_sibling)
    if (i->value && is_text_node(i))
      return i->value;

  return esT("");
}

EsString::const_pointer EsXmlNode::childValueGet(EsString::const_pointer name_) const
{
  return childGet(name_).childValueGet();
}

EsXmlAttribute EsXmlNode::firstAttributeGet() const
{
  return _root ? EsXmlAttribute(_root->first_attribute) : EsXmlAttribute();
}

EsXmlAttribute EsXmlNode::lastAttributeGet() const
{
  return _root && _root->first_attribute ? EsXmlAttribute(_root->first_attribute->prev_attribute_c) : EsXmlAttribute();
}

EsXmlNode EsXmlNode::firstChildGet() const
{
  return _root ? EsXmlNode(_root->first_child) : EsXmlNode();
}

EsXmlNode EsXmlNode::lastChildGet() const
{
  return _root && _root->first_child ? EsXmlNode(_root->first_child->prev_sibling_c) : EsXmlNode();
}

bool EsXmlNode::nameSet(EsString::const_pointer rhs)
{
  switch (typeGet())
  {
  case xmlNodeProcInstr:
  case xmlNodeDocDecl:
  case xmlNodeElement:
    return strcpy_insitu(_root->name, _root->header, xml_memory_page_name_allocated_mask, rhs);

  default:
    return false;
  }
}

bool EsXmlNode::valueSet(EsString::const_pointer rhs)
{
  switch (typeGet())
  {
  case xmlNodeProcInstr:
  case xmlNodeCdata:
  case xmlNodePcdata:
  case xmlNodeComment:
  case xmlNodeDoctype:
    return strcpy_insitu(_root->value, _root->header, xml_memory_page_value_allocated_mask, rhs);

  default:
    return false;
  }
}

EsXmlAttribute EsXmlNode::attributeAppend(EsString::const_pointer name_)
{
  if (typeGet() != xmlNodeElement && typeGet() != xmlNodeDocDecl) return EsXmlAttribute();

  EsXmlAttribute a(append_attribute_ll(_root, get_allocator(_root)));
  a.nameSet(name_);

  return a;
}

EsXmlAttribute EsXmlNode::attributePrepend(EsString::const_pointer name_)
{
  if (typeGet() != xmlNodeElement && typeGet() != xmlNodeDocDecl) return EsXmlAttribute();

  EsXmlAttribute a(allocate_attribute(get_allocator(_root)));
  if (!a) return EsXmlAttribute();

  a.nameSet(name_);

  EsXmlAttributeStruct* head = _root->first_attribute;

  if (head)
  {
    a._attr->prev_attribute_c = head->prev_attribute_c;
    head->prev_attribute_c = a._attr;
  }
  else
    a._attr->prev_attribute_c = a._attr;

  a._attr->next_attribute = head;
  _root->first_attribute = a._attr;

  return a;
}

EsXmlAttribute EsXmlNode::attributeInsertBefore(EsString::const_pointer name_, const EsXmlAttribute& attr)
{
  if ((typeGet() != xmlNodeElement && typeGet() != xmlNodeDocDecl) || attr.empty()) return EsXmlAttribute();

  // check that attribute belongs to *this
  EsXmlAttributeStruct* cur = attr._attr;

  while (cur->prev_attribute_c->next_attribute) cur = cur->prev_attribute_c;

  if (cur != _root->first_attribute) return EsXmlAttribute();

  EsXmlAttribute a(allocate_attribute(get_allocator(_root)));
  if (!a) return EsXmlAttribute();

  a.nameSet(name_);

  if (attr._attr->prev_attribute_c->next_attribute)
    attr._attr->prev_attribute_c->next_attribute = a._attr;
  else
    _root->first_attribute = a._attr;

  a._attr->prev_attribute_c = attr._attr->prev_attribute_c;
  a._attr->next_attribute = attr._attr;
  attr._attr->prev_attribute_c = a._attr;

  return a;
}

EsXmlAttribute EsXmlNode::attributeInsertAfter(EsString::const_pointer name_, const EsXmlAttribute& attr)
{
  if ((typeGet() != xmlNodeElement && typeGet() != xmlNodeDocDecl) || attr.empty()) return EsXmlAttribute();

  // check that attribute belongs to *this
  EsXmlAttributeStruct* cur = attr._attr;

  while (cur->prev_attribute_c->next_attribute) cur = cur->prev_attribute_c;

  if (cur != _root->first_attribute) return EsXmlAttribute();

  EsXmlAttribute a(allocate_attribute(get_allocator(_root)));
  if (!a) return EsXmlAttribute();

  a.nameSet(name_);

  if (attr._attr->next_attribute)
    attr._attr->next_attribute->prev_attribute_c = a._attr;
  else
    _root->first_attribute->prev_attribute_c = a._attr;

  a._attr->next_attribute = attr._attr->next_attribute;
  a._attr->prev_attribute_c = attr._attr;
  attr._attr->next_attribute = a._attr;

  return a;
}

EsXmlAttribute EsXmlNode::attributeCopyAppend(const EsXmlAttribute& proto)
{
  if (!proto) return EsXmlAttribute();

  EsXmlAttribute result = attributeAppend(proto.nameGet());
  result.valueSet(proto.valueGet());

  return result;
}

EsXmlAttribute EsXmlNode::attributeCopyPrepend(const EsXmlAttribute& proto)
{
  if (!proto) return EsXmlAttribute();

  EsXmlAttribute result = attributePrepend(proto.nameGet());
  result.valueSet(proto.valueGet());

  return result;
}

EsXmlAttribute EsXmlNode::attributeCopyInsertAfter(const EsXmlAttribute& proto, const EsXmlAttribute& attr)
{
  if (!proto) return EsXmlAttribute();

  EsXmlAttribute result = attributeInsertAfter(proto.nameGet(), attr);
  result.valueSet(proto.valueGet());

  return result;
}

EsXmlAttribute EsXmlNode::attributeCopyInsertBefore(const EsXmlAttribute& proto, const EsXmlAttribute& attr)
{
  if (!proto) return EsXmlAttribute();

  EsXmlAttribute result = attributeInsertBefore(proto.nameGet(), attr);
  result.valueSet(proto.valueGet());

  return result;
}

EsXmlNode EsXmlNode::childAppend(EsXmlNodeType type_)
{
  if (!allow_insert_child(this->typeGet(), type_)) return EsXmlNode();

  EsXmlNode n(append_node(_root, get_allocator(_root), type_));

  if (type_ == xmlNodeDocDecl) n.nameSet(esT("xml"));

  return n;
}

EsXmlNode EsXmlNode::childPrepend(EsXmlNodeType type_)
{
  if (!allow_insert_child(this->typeGet(), type_)) return EsXmlNode();

  EsXmlNode n(allocate_node(get_allocator(_root), type_));
  if (!n) return EsXmlNode();

  n._root->parent = _root;

  EsXmlNodeStruct* head = _root->first_child;

  if (head)
  {
    n._root->prev_sibling_c = head->prev_sibling_c;
    head->prev_sibling_c = n._root;
  }
  else
    n._root->prev_sibling_c = n._root;

  n._root->next_sibling = head;
  _root->first_child = n._root;

  if (type_ == xmlNodeDocDecl) n.nameSet(esT("xml"));

  return n;
}

EsXmlNode EsXmlNode::childInsertBefore(EsXmlNodeType type_, const EsXmlNode& node)
{
  if (!allow_insert_child(this->typeGet(), type_)) return EsXmlNode();
  if (!node._root || node._root->parent != _root) return EsXmlNode();

  EsXmlNode n(allocate_node(get_allocator(_root), type_));
  if (!n) return EsXmlNode();

  n._root->parent = _root;

  if (node._root->prev_sibling_c->next_sibling)
    node._root->prev_sibling_c->next_sibling = n._root;
  else
    _root->first_child = n._root;

  n._root->prev_sibling_c = node._root->prev_sibling_c;
  n._root->next_sibling = node._root;
  node._root->prev_sibling_c = n._root;

  if (type_ == xmlNodeDocDecl) n.nameSet(esT("xml"));

  return n;
}

EsXmlNode EsXmlNode::childInsertAfter(EsXmlNodeType type_, const EsXmlNode& node)
{
  if (!allow_insert_child(this->typeGet(), type_)) return EsXmlNode();
  if (!node._root || node._root->parent != _root) return EsXmlNode();

  EsXmlNode n(allocate_node(get_allocator(_root), type_));
  if (!n) return EsXmlNode();

  n._root->parent = _root;

  if (node._root->next_sibling)
    node._root->next_sibling->prev_sibling_c = n._root;
  else
    _root->first_child->prev_sibling_c = n._root;

  n._root->next_sibling = node._root->next_sibling;
  n._root->prev_sibling_c = node._root;
  node._root->next_sibling = n._root;

  if (type_ == xmlNodeDocDecl) n.nameSet(esT("xml"));

  return n;
}

EsXmlNode EsXmlNode::childAppend(EsString::const_pointer name_)
{
  EsXmlNode result = childAppend(xmlNodeElement);

  result.nameSet(name_);

  return result;
}

EsXmlNode EsXmlNode::childPrepend(EsString::const_pointer name_)
{
  EsXmlNode result = childPrepend(xmlNodeElement);

  result.nameSet(name_);

  return result;
}

EsXmlNode EsXmlNode::childInsertAfter(EsString::const_pointer name_, const EsXmlNode& node)
{
  EsXmlNode result = childInsertAfter(xmlNodeElement, node);

  result.nameSet(name_);

  return result;
}

EsXmlNode EsXmlNode::childInsertBefore(EsString::const_pointer name_, const EsXmlNode& node)
{
  EsXmlNode result = childInsertBefore(xmlNodeElement, node);

  result.nameSet(name_);

  return result;
}

EsXmlNode EsXmlNode::childCopyAppend(const EsXmlNode& proto)
{
  EsXmlNode result = childAppend(proto.typeGet());

  if (result) recursive_copy_skip(result, proto, result);

  return result;
}

EsXmlNode EsXmlNode::childCopyPrepend(const EsXmlNode& proto)
{
  EsXmlNode result = childPrepend(proto.typeGet());

  if (result) recursive_copy_skip(result, proto, result);

  return result;
}

EsXmlNode EsXmlNode::childCopyInsertAfter(const EsXmlNode& proto, const EsXmlNode& node)
{
  EsXmlNode result = childInsertAfter(proto.typeGet(), node);

  if (result) recursive_copy_skip(result, proto, result);

  return result;
}

EsXmlNode EsXmlNode::childCopyInsertBefore(const EsXmlNode& proto, const EsXmlNode& node)
{
  EsXmlNode result = childInsertBefore(proto.typeGet(), node);

  if (result) recursive_copy_skip(result, proto, result);

  return result;
}

bool EsXmlNode::attributeRemove(EsString::const_pointer name_)
{
  return attributeRemove(attributeGet(name_));
}

bool EsXmlNode::attributeRemove(const EsXmlAttribute& a)
{
  if (!_root || !a._attr) return false;

  // check that attribute belongs to *this
  EsXmlAttributeStruct* attr = a._attr;

  while (attr->prev_attribute_c->next_attribute) attr = attr->prev_attribute_c;

  if (attr != _root->first_attribute) return false;

  if (a._attr->next_attribute) a._attr->next_attribute->prev_attribute_c = a._attr->prev_attribute_c;
  else if (_root->first_attribute) _root->first_attribute->prev_attribute_c = a._attr->prev_attribute_c;

  if (a._attr->prev_attribute_c->next_attribute) a._attr->prev_attribute_c->next_attribute = a._attr->next_attribute;
  else _root->first_attribute = a._attr->next_attribute;

  destroy_attribute(a._attr, get_allocator(_root));

  return true;
}

bool EsXmlNode::childRemove(EsString::const_pointer name_)
{
  return childRemove(childGet(name_));
}

bool EsXmlNode::childRemove(const EsXmlNode& n)
{
  if (!_root || !n._root || n._root->parent != _root) return false;

  if (n._root->next_sibling) n._root->next_sibling->prev_sibling_c = n._root->prev_sibling_c;
  else if (_root->first_child) _root->first_child->prev_sibling_c = n._root->prev_sibling_c;

  if (n._root->prev_sibling_c->next_sibling) n._root->prev_sibling_c->next_sibling = n._root->next_sibling;
  else _root->first_child = n._root->next_sibling;

  destroy_node(n._root, get_allocator(_root));

  return true;
}

EsXmlParseResult EsXmlNode::bufferAppend(const void* contents, size_t size, unsigned int options, EsXmlEncoding encoding)
{
  // bufferAppend is only valid for elements/documents
  if (!allow_insert_child(typeGet(), xmlNodeElement)) return make_parse_result(xmlParseStatusAppendInvalidRoot);

  // get document node
  EsXmlDocumentStruct* doc = static_cast<EsXmlDocumentStruct*>(rootGet()._root);
  ES_ASSERT(doc);

  // get extra buffer element (we'll store the document fragment buffer there so that we can deallocate it later)
  EsXmlMemoryPage* page = 0;
  EsXmlExtraBuffer* extra = static_cast<EsXmlExtraBuffer*>(doc->allocate_memory(sizeof(EsXmlExtraBuffer), page));
  (void)page;

  if (!extra) return make_parse_result(xmlParseStatusOutOfMemory);

  // save name; name of the root has to be NULL before parsing - otherwise closing node mismatches will not be detected at the top level
  EsString::pointer rootname = _root->name;
  _root->name = 0;

  // parse
  EsString::pointer buffer = 0;
  EsXmlParseResult res = load_buffer_impl(doc, _root, const_cast<void*>(contents), size, options, encoding, false, false, &buffer);

  // restore name
  _root->name = rootname;

  // add extra buffer to the list
  extra->buffer = buffer;
  extra->next = doc->extra_buffers;
  doc->extra_buffers = extra;

  return res;
}

EsXmlNode EsXmlNode::childFindByAttribute(EsString::const_pointer name_, EsString::const_pointer attr_name, EsString::const_pointer attr_value) const
{
  if (!_root) return EsXmlNode();

  for (EsXmlNodeStruct* i = _root->first_child; i; i = i->next_sibling)
    if (i->name && strequal(name_, i->name))
    {
      for (EsXmlAttributeStruct* a = i->first_attribute; a; a = a->next_attribute)
        if (a->name && strequal(attr_name, a->name) && strequal(attr_value, a->value ? a->value : esT("")))
          return EsXmlNode(i);
    }

  return EsXmlNode();
}

EsXmlNode EsXmlNode::childFindByAttribute(EsString::const_pointer attr_name, EsString::const_pointer attr_value) const
{
  if (!_root) return EsXmlNode();

  for (EsXmlNodeStruct* i = _root->first_child; i; i = i->next_sibling)
    for (EsXmlAttributeStruct* a = i->first_attribute; a; a = a->next_attribute)
      if (a->name && strequal(attr_name, a->name) && strequal(attr_value, a->value ? a->value : esT("")))
        return EsXmlNode(i);

  return EsXmlNode();
}

EsString EsXmlNode::pathGet(EsString::value_type delimiter) const
{
  EsXmlNode cursor = *this; // Make a copy.

  EsString result = cursor.nameGet();

  while (cursor.parentGet())
  {
    cursor = cursor.parentGet();

    EsString temp = cursor.nameGet();
    temp += delimiter;
    temp += result;
    result.swap(temp);
  }

  return result;
}

EsXmlNode EsXmlNode::firstElementGetByPath(EsString::const_pointer path_, EsString::value_type delimiter) const
{
  EsXmlNode found = *this; // Current search context.

  if (!_root || !path_ || !path_[0]) return found;

  if (path_[0] == delimiter)
  {
    // Absolute path; e.g. '/foo/bar'
    found = found.rootGet();
    ++path_;
  }

  EsString::const_pointer path_segment = path_;

  while (*path_segment == delimiter) ++path_segment;

  EsString::const_pointer path_segment_end = path_segment;

  while (*path_segment_end && *path_segment_end != delimiter) ++path_segment_end;

  if (path_segment == path_segment_end) return found;

  EsString::const_pointer next_segment = path_segment_end;

  while (*next_segment == delimiter) ++next_segment;

  if (*path_segment == '.' && path_segment + 1 == path_segment_end)
    return found.firstElementGetByPath(next_segment, delimiter);
  else if (*path_segment == '.' && *(path_segment+1) == '.' && path_segment + 2 == path_segment_end)
    return found.parentGet().firstElementGetByPath(next_segment, delimiter);
  else
  {
    for (EsXmlNodeStruct* j = found._root->first_child; j; j = j->next_sibling)
    {
      if (j->name && strequalrange(j->name, path_segment, static_cast<size_t>(path_segment_end - path_segment)))
      {
        EsXmlNode subsearch = EsXmlNode(j).firstElementGetByPath(next_segment, delimiter);

        if (subsearch) return subsearch;
      }
    }

    return EsXmlNode();
  }
}

bool EsXmlNode::traverse(EsXmlTreeWalker& walker)
{
  walker._depth = -1;

  EsXmlNode arg_begin = *this;
  if (!walker.begin(arg_begin)) return false;

  EsXmlNode cur = firstChildGet();

  if (cur)
  {
    ++walker._depth;

    do
    {
      EsXmlNode arg_for_each = cur;
      if (!walker.for_each(arg_for_each))
        return false;

      if (cur.firstChildGet())
      {
        ++walker._depth;
        cur = cur.firstChildGet();
      }
      else if (cur.nextSiblingGet())
        cur = cur.nextSiblingGet();
      else
      {
        // Borland C++ workaround
        while (!cur.nextSiblingGet() && cur != *this && !cur.parentGet().empty())
        {
          --walker._depth;
          cur = cur.parentGet();
        }

        if (cur != *this)
          cur = cur.nextSiblingGet();
      }
    }
    while (cur && cur != *this);
  }

  ES_ASSERT(walker._depth == -1);

  EsXmlNode arg_end = *this;
  return walker.end(arg_end);
}

size_t EsXmlNode::hashGet() const
{
  return static_cast<size_t>(reinterpret_cast<uintptr_t>(_root) / sizeof(EsXmlNodeStruct));
}

EsXmlNodeStruct* EsXmlNode::internalObjectGet() const
{
  return _root;
}

void EsXmlNode::print(EsXmlWriterIntf& writer, const EsString& indent, unsigned int flags, EsXmlEncoding encoding, unsigned int depth) const
{
  if (!_root) return;

  EsXmlBufferedWriter buffered_writer(writer, encoding);

  node_output(buffered_writer, *this, indent.c_str(), flags, depth);
}

ptrdiff_t EsXmlNode::offsetDebugGet() const
{
  EsXmlNodeStruct* r = rootGet()._root;

  if (!r) return -1;

  EsString::const_pointer buffer = static_cast<EsXmlDocumentStruct*>(r)->buffer;

  if (!buffer) return -1;

  switch (typeGet())
  {
  case xmlNodeDocument:
    return 0;

  case xmlNodeElement:
  case xmlNodeDocDecl:
  case xmlNodeProcInstr:
    return (_root->header & xml_memory_page_name_allocated_mask) ? -1 : _root->name - buffer;

  case xmlNodePcdata:
  case xmlNodeCdata:
  case xmlNodeComment:
  case xmlNodeDoctype:
    return (_root->header & xml_memory_page_value_allocated_mask) ? -1 : _root->value - buffer;

  default:
    return -1;
  }
}

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
bool operator&&(const EsXmlNode& lhs, bool rhs)
{
  return (bool)lhs && rhs;
}

bool operator||(const EsXmlNode& lhs, bool rhs)
{
  return (bool)lhs || rhs;
}
#endif

EsXmlText::EsXmlText(EsXmlNodeStruct* root): _root(root)
{
}

EsXmlNodeStruct* EsXmlText::_data() const
{
  if (!_root || is_text_node(_root)) return _root;

  for (EsXmlNodeStruct* node = _root->first_child; node; node = node->next_sibling)
    if (is_text_node(node))
      return node;

  return 0;
}

EsXmlNodeStruct* EsXmlText::_data_new()
{
  EsXmlNodeStruct* d = _data();
  if (d) return d;

  return EsXmlNode(_root).childAppend(xmlNodePcdata).internalObjectGet();
}

EsXmlText::EsXmlText(): _root(0)
{
}

static void unspecified_bool_xml_text(EsXmlText***)
{
}

EsXmlText::operator EsXmlText::unspecified_bool_type() const
{
  return _data() ? unspecified_bool_xml_text : 0;
}

bool EsXmlText::operator!() const
{
  return !_data();
}

bool EsXmlText::empty() const
{
  return _data() == 0;
}

EsString::const_pointer EsXmlText::get() const
{
  EsXmlNodeStruct* d = _data();

  return (d && d->value) ? d->value : EsString::null().c_str();
}

EsString::const_pointer EsXmlText::asString(const EsString& def) const
{
  EsXmlNodeStruct* d = _data();

  return (d && d->value) ? d->value : def.c_str();
}

int EsXmlText::asInt(int def) const
{
  EsXmlNodeStruct* d = _data();

  return get_value_int(d ? d->value : 0, def);
}

unsigned int EsXmlText::asUint(unsigned int def) const
{
  EsXmlNodeStruct* d = _data();

  return get_value_uint(d ? d->value : 0, def);
}

double EsXmlText::asDouble(double def) const
{
  EsXmlNodeStruct* d = _data();

  return get_value_double(d ? d->value : 0, def);
}

float EsXmlText::asFloat(float def) const
{
  EsXmlNodeStruct* d = _data();

  return get_value_float(d ? d->value : 0, def);
}

bool EsXmlText::asBool(bool def) const
{
  EsXmlNodeStruct* d = _data();

  return get_value_bool(d ? d->value : 0, def);
}

long long EsXmlText::asLLong(long long def) const
{
  EsXmlNodeStruct* d = _data();

  return get_value_llong(d ? d->value : 0, def);
}

unsigned long long EsXmlText::asULLong(unsigned long long def) const
{
  EsXmlNodeStruct* d = _data();

  return get_value_ullong(d ? d->value : 0, def);
}

bool EsXmlText::set(EsString::const_pointer rhs)
{
  EsXmlNodeStruct* dn = _data_new();

  return dn ? strcpy_insitu(dn->value, dn->header, xml_memory_page_value_allocated_mask, rhs) : false;
}

bool EsXmlText::set(int rhs)
{
  EsXmlNodeStruct* dn = _data_new();

  return dn ? set_value_convert(dn->value, dn->header, xml_memory_page_value_allocated_mask, rhs) : false;
}

bool EsXmlText::set(unsigned int rhs)
{
  EsXmlNodeStruct* dn = _data_new();

  return dn ? set_value_convert(dn->value, dn->header, xml_memory_page_value_allocated_mask, rhs) : false;
}

bool EsXmlText::set(double rhs)
{
  EsXmlNodeStruct* dn = _data_new();

  return dn ? set_value_convert(dn->value, dn->header, xml_memory_page_value_allocated_mask, rhs) : false;
}

bool EsXmlText::set(bool rhs)
{
  EsXmlNodeStruct* dn = _data_new();

  return dn ? set_value_convert(dn->value, dn->header, xml_memory_page_value_allocated_mask, rhs) : false;
}

bool EsXmlText::set(long long rhs)
{
  EsXmlNodeStruct* dn = _data_new();

  return dn ? set_value_convert(dn->value, dn->header, xml_memory_page_value_allocated_mask, rhs) : false;
}

bool EsXmlText::set(unsigned long long rhs)
{
  EsXmlNodeStruct* dn = _data_new();

  return dn ? set_value_convert(dn->value, dn->header, xml_memory_page_value_allocated_mask, rhs) : false;
}

EsXmlText& EsXmlText::operator=(EsString::const_pointer rhs)
{
  set(rhs);
  return *this;
}

EsXmlText& EsXmlText::operator=(int rhs)
{
  set(rhs);
  return *this;
}

EsXmlText& EsXmlText::operator=(unsigned int rhs)
{
  set(rhs);
  return *this;
}

EsXmlText& EsXmlText::operator=(double rhs)
{
  set(rhs);
  return *this;
}

EsXmlText& EsXmlText::operator=(bool rhs)
{
  set(rhs);
  return *this;
}

EsXmlText& EsXmlText::operator=(long long rhs)
{
  set(rhs);
  return *this;
}

EsXmlText& EsXmlText::operator=(unsigned long long rhs)
{
  set(rhs);
  return *this;
}

EsXmlNode EsXmlText::data() const
{
  return EsXmlNode(_data());
}

#ifdef __BORLANDC__
bool operator&&(const EsXmlText& lhs, bool rhs)
{
  return (bool)lhs && rhs;
}

bool operator||(const EsXmlText& lhs, bool rhs)
{
  return (bool)lhs || rhs;
}
#endif

EsXmlNodeIterator::EsXmlNodeIterator()
{
}

EsXmlNodeIterator::EsXmlNodeIterator(const EsXmlNode& node): _wrap(node), _parent(node.parentGet())
{
}

EsXmlNodeIterator::EsXmlNodeIterator(EsXmlNodeStruct* ref, EsXmlNodeStruct* parent): _wrap(ref), _parent(parent)
{
}

bool EsXmlNodeIterator::operator==(const EsXmlNodeIterator& rhs) const
{
  return _wrap._root == rhs._wrap._root && _parent._root == rhs._parent._root;
}

bool EsXmlNodeIterator::operator!=(const EsXmlNodeIterator& rhs) const
{
  return _wrap._root != rhs._wrap._root || _parent._root != rhs._parent._root;
}

EsXmlNode& EsXmlNodeIterator::operator*() const
{
  ES_ASSERT(_wrap._root);
  return _wrap;
}

EsXmlNode* EsXmlNodeIterator::operator->() const
{
  ES_ASSERT(_wrap._root);
  return const_cast<EsXmlNode*>(&_wrap); // BCC32 workaround
}

const EsXmlNodeIterator& EsXmlNodeIterator::operator++()
{
  ES_ASSERT(_wrap._root);
  _wrap._root = _wrap._root->next_sibling;
  return *this;
}

EsXmlNodeIterator EsXmlNodeIterator::operator++(int)
{
  EsXmlNodeIterator temp = *this;
  ++*this;
  return temp;
}

const EsXmlNodeIterator& EsXmlNodeIterator::operator--()
{
  _wrap = _wrap._root ? _wrap.previousSiblingGet() : _parent.lastChildGet();
  return *this;
}

EsXmlNodeIterator EsXmlNodeIterator::operator--(int)
{
  EsXmlNodeIterator temp = *this;
  --*this;
  return temp;
}

EsXmlAttributeIterator::EsXmlAttributeIterator()
{
}

EsXmlAttributeIterator::EsXmlAttributeIterator(const EsXmlAttribute& attr, const EsXmlNode& parent): _wrap(attr), _parent(parent)
{
}

EsXmlAttributeIterator::EsXmlAttributeIterator(EsXmlAttributeStruct* ref, EsXmlNodeStruct* parent): _wrap(ref), _parent(parent)
{
}

bool EsXmlAttributeIterator::operator==(const EsXmlAttributeIterator& rhs) const
{
  return _wrap._attr == rhs._wrap._attr && _parent._root == rhs._parent._root;
}

bool EsXmlAttributeIterator::operator!=(const EsXmlAttributeIterator& rhs) const
{
  return _wrap._attr != rhs._wrap._attr || _parent._root != rhs._parent._root;
}

EsXmlAttribute& EsXmlAttributeIterator::operator*() const
{
  ES_ASSERT(_wrap._attr);
  return _wrap;
}

EsXmlAttribute* EsXmlAttributeIterator::operator->() const
{
  ES_ASSERT(_wrap._attr);
  return const_cast<EsXmlAttribute*>(&_wrap); // BCC32 workaround
}

const EsXmlAttributeIterator& EsXmlAttributeIterator::operator++()
{
  ES_ASSERT(_wrap._attr);
  _wrap._attr = _wrap._attr->next_attribute;
  return *this;
}

EsXmlAttributeIterator EsXmlAttributeIterator::operator++(int)
{
  EsXmlAttributeIterator temp = *this;
  ++*this;
  return temp;
}

const EsXmlAttributeIterator& EsXmlAttributeIterator::operator--()
{
  _wrap = _wrap._attr ? _wrap.previousAttributeGet() : _parent.lastAttributeGet();
  return *this;
}

EsXmlAttributeIterator EsXmlAttributeIterator::operator--(int)
{
  EsXmlAttributeIterator temp = *this;
  --*this;
  return temp;
}

EsXmlNamedNodeIterator::EsXmlNamedNodeIterator(): _name(0)
{
}

EsXmlNamedNodeIterator::EsXmlNamedNodeIterator(const EsXmlNode& node, EsString::const_pointer name): _wrap(node), _parent(node.parentGet()), _name(name)
{
}

EsXmlNamedNodeIterator::EsXmlNamedNodeIterator(EsXmlNodeStruct* ref, EsXmlNodeStruct* parent, EsString::const_pointer name): _wrap(ref), _parent(parent), _name(name)
{
}

bool EsXmlNamedNodeIterator::operator==(const EsXmlNamedNodeIterator& rhs) const
{
  return _wrap._root == rhs._wrap._root && _parent._root == rhs._parent._root;
}

bool EsXmlNamedNodeIterator::operator!=(const EsXmlNamedNodeIterator& rhs) const
{
  return _wrap._root != rhs._wrap._root || _parent._root != rhs._parent._root;
}

EsXmlNode& EsXmlNamedNodeIterator::operator*() const
{
  ES_ASSERT(_wrap._root);
  return _wrap;
}

EsXmlNode* EsXmlNamedNodeIterator::operator->() const
{
  ES_ASSERT(_wrap._root);
  return const_cast<EsXmlNode*>(&_wrap); // BCC32 workaround
}

const EsXmlNamedNodeIterator& EsXmlNamedNodeIterator::operator++()
{
  ES_ASSERT(_wrap._root);
  _wrap = _wrap.nextSiblingGet(_name);
  return *this;
}

EsXmlNamedNodeIterator EsXmlNamedNodeIterator::operator++(int)
{
  EsXmlNamedNodeIterator temp = *this;
  ++*this;
  return temp;
}

const EsXmlNamedNodeIterator& EsXmlNamedNodeIterator::operator--()
{
  if (_wrap._root)
    _wrap = _wrap.previousSiblingGet(_name);
  else
  {
    _wrap = _parent.lastChildGet();

    if (!strequal(_wrap.nameGet(), _name))
      _wrap = _wrap.previousSiblingGet(_name);
  }

  return *this;
}

EsXmlNamedNodeIterator EsXmlNamedNodeIterator::operator--(int)
{
  EsXmlNamedNodeIterator temp = *this;
  --*this;
  return temp;
}

EsXmlParseResult::EsXmlParseResult(): status(xmlParseStatusInternalError), offset(0), encoding(xmlEncodingAuto)
{
}

EsXmlParseResult::operator bool() const
{
  return status == xmlParseStatusOk;
}

EsString::const_pointer EsXmlParseResult::descriptionGet() const
{
  switch (status)
  {
  case xmlParseStatusOk: return esT("No error");
  case xmlParseStatusFileNotFound: return esT("File was not found");
  case xmlParseStatusIoError: return esT("Error reading from file/stream");
  case xmlParseStatusOutOfMemory: return esT("Could not allocate memory");
  case xmlParseStatusInternalError: return esT("Internal error occurred");
  case xmlParseStatusUnrecognizedTag: return esT("Could not determine tag type");
  case xmlParseStatusBadProcInstr: return esT("Error parsing document declaration/processing instruction");
  case xmlParseStatusBadComment: return esT("Error parsing comment");
  case xmlParseStatusBadCdata: return esT("Error parsing CDATA section");
  case xmlParseStatusBadDoctype: return esT("Error parsing document type declaration");
  case xmlParseStatusBadPcdata: return esT("Error parsing PCDATA section");
  case xmlParseStatusBadStartElement: return esT("Error parsing start element tag");
  case xmlParseStatusBadAttribute: return esT("Error parsing element attribute");
  case xmlParseStatusBadEndElement: return esT("Error parsing end element tag");
  case xmlParseStatusEndElementMismatch: return esT("Start-end tags mismatch");
  case xmlParseStatusAppendInvalidRoot: return esT("Unable to append nodes: root is not an element or document");
  case xmlParseStatusNoDocumentElement: return esT("No document element found");
  default: return esT("Unknown error");
  }
}

EsXmlDocument::EsXmlDocument(): _buffer(0)
{
  create();
}

EsXmlDocument::~EsXmlDocument()
{
  destroy();
}

void EsXmlDocument::reset()
{
  destroy();
  create();
}

void EsXmlDocument::reset(const EsXmlDocument& proto)
{
  reset();

  for (EsXmlNode cur = proto.firstChildGet(); cur; cur = cur.nextSiblingGet())
    childCopyAppend(cur);
}

void EsXmlDocument::create()
{
  ES_ASSERT(!_root);

  // initialize sentinel page
  ES_COMPILE_TIME_ASSERT(sizeof(EsXmlMemoryPage) + sizeof(EsXmlDocumentStruct) + xml_memory_page_alignment <= sizeof(_memory), xmlMemoryAlignmentCheck);

  // align upwards to page boundary
  void* page_memory = reinterpret_cast<void*>((reinterpret_cast<uintptr_t>(_memory) + (xml_memory_page_alignment - 1)) & ~(xml_memory_page_alignment - 1));

  // prepare page structure
  EsXmlMemoryPage* page = EsXmlMemoryPage::construct(page_memory);
  ES_ASSERT(page);

  page->busy_size = xml_memory_page_size;

  // allocate new root
  _root = new (page->data) EsXmlDocumentStruct(page);
  _root->prev_sibling_c = _root;

  // setup sentinel page
  page->allocator = static_cast<EsXmlDocumentStruct*>(_root);
}

void EsXmlDocument::destroy()
{
      ES_ASSERT(_root);

  // destroy static storage
  if (_buffer)
  {
    EsXmlMemory::deallocate(_buffer);
    _buffer = 0;
  }

  // destroy extra buffers (note: no need to destroy linked list nodes, they're allocated using document allocator)
  for (EsXmlExtraBuffer* extra = static_cast<EsXmlDocumentStruct*>(_root)->extra_buffers; extra; extra = extra->next)
  {
    if (extra->buffer) EsXmlMemory::deallocate(extra->buffer);
  }

  // destroy dynamic storage, leave sentinel page (it's in static memory)
      EsXmlMemoryPage* root_page = reinterpret_cast<EsXmlMemoryPage*>(_root->header & xml_memory_page_pointer_mask);
      ES_ASSERT(root_page && !root_page->prev && !root_page->memory);

      for (EsXmlMemoryPage* page = root_page->next; page; )
      {
          EsXmlMemoryPage* next = page->next;

          EsXmlAllocator::deallocate_page(page);

          page = next;
      }

      _root = 0;
}

EsXmlParseResult EsXmlDocument::load(const EsString& contents, unsigned int options)
{
  // Force native encoding (skip autodetection)
#if !defined(ES_USE_NARROW_ES_CHAR)
  EsXmlEncoding encoding = xmlEncodingWchar;
#else
  EsXmlEncoding encoding = xmlEncodingUtf8;
#endif

  return loadBuffer(contents.c_str(), contents.size() * sizeof(EsString::value_type), options, encoding);
}

EsXmlParseResult EsXmlDocument::loadBuffer(const void* contents, size_t size, unsigned int options, EsXmlEncoding encoding)
{
  reset();

  return load_buffer_impl(static_cast<EsXmlDocumentStruct*>(_root), _root, const_cast<void*>(contents), size, options, encoding, false, false, &_buffer);
}

EsXmlParseResult EsXmlDocument::loadBufferInplace(void* contents, size_t size, unsigned int options, EsXmlEncoding encoding)
{
  reset();

  return load_buffer_impl(static_cast<EsXmlDocumentStruct*>(_root), _root, contents, size, options, encoding, true, false, &_buffer);
}

EsXmlParseResult EsXmlDocument::loadBufferInplaceOwn(void* contents, size_t size, unsigned int options, EsXmlEncoding encoding)
{
  reset();

  return load_buffer_impl(static_cast<EsXmlDocumentStruct*>(_root), _root, contents, size, options, encoding, true, true, &_buffer);
}

void EsXmlDocument::save(EsXmlWriterIntf& writer, const EsString& indent, unsigned int flags, EsXmlEncoding encoding) const
{
  EsXmlBufferedWriter buffered_writer(writer, encoding);

  if ((flags & xmlFormatWriteBom) && encoding != xmlEncodingLatin1)
  {
    // BOM always represents the codepoint U+FEFF, so just write it in native encoding
  #if !defined(ES_USE_NARROW_ES_CHAR)
    unsigned int bom = 0xfeff;
    buffered_writer.write(static_cast<ES_WCHAR>(bom));
  #else
    buffered_writer.write('\xef', '\xbb', '\xbf');
  #endif
  }

  if (!(flags & xmlFormatNoDocDecl) && !has_declaration(*this))
  {
    buffered_writer.write(esT("<?xml version=\"1.0\""));
    if (encoding == xmlEncodingLatin1) buffered_writer.write(esT(" encoding=\"ISO-8859-1\""));
    buffered_writer.write('?', '>');
    if (!(flags & xmlFormatRaw)) buffered_writer.write('\n');
  }

  node_output(buffered_writer, *this, indent.c_str(), flags, 0);
}

EsXmlNode EsXmlDocument::elementGet() const
{
  ES_ASSERT(_root);

  for (EsXmlNodeStruct* i = _root->first_child; i; i = i->next_sibling)
    if ((i->header & xml_memory_page_type_mask) + 1 == xmlNodeElement)
      return EsXmlNode(i);

  return EsXmlNode();
}

#if (defined(_MSC_VER) || defined(__ICC))
namespace std
{
// Workarounds for (non-standard) iterator category detection for older versions (MSVC7/IC8 and earlier)
std::bidirectional_iterator_tag _Iter_cat(const EsXmlNodeIterator&)
{
  return std::bidirectional_iterator_tag();
}

std::bidirectional_iterator_tag _Iter_cat(const EsXmlAttributeIterator&)
{
  return std::bidirectional_iterator_tag();
}

std::bidirectional_iterator_tag _Iter_cat(const EsXmlNamedNodeIterator&)
{
  return std::bidirectional_iterator_tag();
}
}
#endif

#if defined(__SUNPRO_CC)
namespace std
{
// Workarounds for (non-standard) iterator category detection
std::bidirectional_iterator_tag __iterator_category(const EsXmlNodeIterator&)
{
  return std::bidirectional_iterator_tag();
}

std::bidirectional_iterator_tag __iterator_category(const EsXmlAttributeIterator&)
{
  return std::bidirectional_iterator_tag();
}

std::bidirectional_iterator_tag __iterator_category(const EsXmlNamedNodeIterator&)
{
  return std::bidirectional_iterator_tag();
}
}
#endif

#ifndef ES_XML_NO_XPATH
namespace EsXml
{

// STL replacements
struct equal_to
{
  template <typename T> bool operator()(const T& lhs, const T& rhs) const
  {
    return lhs == rhs;
  }
};

struct not_equal_to
{
  template <typename T> bool operator()(const T& lhs, const T& rhs) const
  {
    return lhs != rhs;
  }
};

struct less
{
  template <typename T> bool operator()(const T& lhs, const T& rhs) const
  {
    return lhs < rhs;
  }
};

struct less_equal
{
  template <typename T> bool operator()(const T& lhs, const T& rhs) const
  {
    return lhs <= rhs;
  }
};

template <typename T> void swap(T& lhs, T& rhs)
{
  T temp = lhs;
  lhs = rhs;
  rhs = temp;
}

template <typename I, typename Pred> I min_element(I begin, I end, const Pred& pred)
{
  I result = begin;

  for (I it = begin + 1; it != end; ++it)
    if (pred(*it, *result))
      result = it;

  return result;
}

template <typename I> void reverse(I begin, I end)
{
  while (end - begin > 1) EsXml::swap(*begin++, *--end);
}

template <typename I> I unique(I begin, I end)
{
  // fast skip head
  while (end - begin > 1 && *begin != *(begin + 1)) begin++;

  if (begin == end) return begin;

  // last written element
  I write = begin++;

  // merge unique elements
  while (begin != end)
  {
    if (*begin != *write)
      *++write = *begin++;
    else
      begin++;
  }

  // past-the-end (write points to live element)
  return write + 1;
}

template <typename I> void copy_backwards(I begin, I end, I target)
{
  while (begin != end) *--target = *--end;
}

template <typename I, typename Pred, typename T> void insertion_sort(I begin, I end, const Pred& pred, T*)
{
  ES_ASSERT(begin != end);

  for (I it = begin + 1; it != end; ++it)
  {
    T val = *it;

    if (pred(val, *begin))
    {
      // move to front
      EsXml::copy_backwards(begin, it, it + 1);
      *begin = val;
    }
    else
    {
      I hole = it;

      // move hole backwards
      while (pred(val, *(hole - 1)))
      {
        *hole = *(hole - 1);
        hole--;
      }

      // fill hole with element
      *hole = val;
    }
  }
}

// std variant for elements with ==
template <typename I, typename Pred> void partition(I begin, I middle, I end, const Pred& pred, I* out_eqbeg, I* out_eqend)
{
  I eqbeg = middle, eqend = middle + 1;

  // expand equal range
  while (eqbeg != begin && *(eqbeg - 1) == *eqbeg) --eqbeg;
  while (eqend != end && *eqend == *eqbeg) ++eqend;

  // process outer elements
  I ltend = eqbeg, gtbeg = eqend;

  for (;;)
  {
    // find the element from the right side that belongs to the left one
    for (; gtbeg != end; ++gtbeg)
      if (!pred(*eqbeg, *gtbeg))
      {
        if (*gtbeg == *eqbeg) EsXml::swap(*gtbeg, *eqend++);
        else break;
      }

    // find the element from the left side that belongs to the right one
    for (; ltend != begin; --ltend)
      if (!pred(*(ltend - 1), *eqbeg))
      {
        if (*eqbeg == *(ltend - 1)) EsXml::swap(*(ltend - 1), *--eqbeg);
        else break;
      }

    // scanned all elements
    if (gtbeg == end && ltend == begin)
    {
      *out_eqbeg = eqbeg;
      *out_eqend = eqend;
      return;
    }

    // make room for elements by moving equal area
    if (gtbeg == end)
    {
      if (--ltend != --eqbeg) swap(*ltend, *eqbeg);
      EsXml::swap(*eqbeg, *--eqend);
    }
    else if (ltend == begin)
    {
      if (eqend != gtbeg) swap(*eqbeg, *eqend);
      ++eqend;
      EsXml::swap(*gtbeg++, *eqbeg++);
    }
    else EsXml::swap(*gtbeg++, *--ltend);
  }
}

template <typename I, typename Pred> void median3(I first, I middle, I last, const Pred& pred)
{
  if (pred(*middle, *first)) EsXml::swap(*middle, *first);
  if (pred(*last, *middle)) EsXml::swap(*last, *middle);
  if (pred(*middle, *first)) EsXml::swap(*middle, *first);
}

template <typename I, typename Pred> void median(I first, I middle, I last, const Pred& pred)
{
  if (last - first <= 40)
  {
    // median of three for small chunks
    EsXml::median3(first, middle, last, pred);
  }
  else
  {
    // median of nine
    size_t step = (last - first + 1) / 8;

    EsXml::median3(first, first + step, first + 2 * step, pred);
    EsXml::median3(middle - step, middle, middle + step, pred);
    EsXml::median3(last - 2 * step, last - step, last, pred);
    EsXml::median3(first + step, middle, last - step, pred);
  }
}

template <typename I, typename Pred> void sort(I begin, I end, const Pred& pred)
{
  // sort large chunks
  while (end - begin > 32)
  {
    // find median element
    I middle = begin + (end - begin) / 2;
    EsXml::median(begin, middle, end - 1, pred);

    // partition in three chunks (< = >)
    I eqbeg, eqend;
    EsXml::partition(begin, middle, end, pred, &eqbeg, &eqend);

    // loop on larger half
    if (eqbeg - begin > end - eqend)
    {
      EsXml::sort(eqend, end, pred);
      end = eqbeg;
    }
    else
    {
      EsXml::sort(begin, eqbeg, pred);
      begin = eqend;
    }
  }

  // insertion sort small chunk
  if (begin != end) EsXml::insertion_sort(begin, end, pred, &*begin);
}

} // namespace EsXml

// Allocator used for AST and evaluation stacks
struct xpath_memory_block
{
  xpath_memory_block* next;

  char data[
#ifdef PUGIXML_MEMORY_XPATH_PAGE_SIZE
    PUGIXML_MEMORY_XPATH_PAGE_SIZE
#else
    4096
#endif
  ];
};

class xpath_allocator
{
  xpath_memory_block* _root;
  size_t _root_size;

public:
#ifdef PUGIXML_NO_EXCEPTIONS
  jmp_buf* error_handler;
#endif

  xpath_allocator(xpath_memory_block* root, size_t root_size = 0): _root(root), _root_size(root_size)
  {
  #ifdef PUGIXML_NO_EXCEPTIONS
    error_handler = 0;
  #endif
  }

  void* allocate_nothrow(size_t size)
  {
    const size_t block_capacity = sizeof(_root->data);

    // align size so that we're able to store pointers in subsequent blocks
    size = (size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    if (_root_size + size <= block_capacity)
    {
      void* buf = _root->data + _root_size;
      _root_size += size;
      return buf;
    }
    else
    {
      size_t block_data_size = (size > block_capacity) ? size : block_capacity;
      size_t block_size = block_data_size + offsetof(xpath_memory_block, data);

      xpath_memory_block* block = static_cast<xpath_memory_block*>(EsXmlMemory::allocate(block_size));
      if (!block) return 0;

      block->next = _root;

      _root = block;
      _root_size = size;

      return block->data;
    }
  }

  void* allocate(size_t size)
  {
    void* result = allocate_nothrow(size);

    if (!result)
    {
    #ifdef PUGIXML_NO_EXCEPTIONS
      ES_ASSERT(error_handler);
      longjmp(*error_handler, 1);
    #else
      throw std::bad_alloc();
    #endif
    }

    return result;
  }

  void* reallocate(void* ptr, size_t old_size, size_t new_size)
  {
    // align size so that we're able to store pointers in subsequent blocks
    old_size = (old_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);
    new_size = (new_size + sizeof(void*) - 1) & ~(sizeof(void*) - 1);

    // we can only reallocate the last object
    ES_ASSERT(ptr == 0 || static_cast<char*>(ptr) + old_size == _root->data + _root_size);

    // adjust root size so that we have not allocated the object at all
    bool only_object = (_root_size == old_size);

    if (ptr) _root_size -= old_size;

    // allocate a new version (this will obviously reuse the memory if possible)
    void* result = allocate(new_size);
    ES_ASSERT(result);

    // we have a new block
    if (result != ptr && ptr)
    {
      // copy old data
      ES_ASSERT(new_size >= old_size);
      memcpy(result, ptr, old_size);

      // free the previous page if it had no other objects
      if (only_object)
      {
        ES_ASSERT(_root->data == result);
        ES_ASSERT(_root->next);

        xpath_memory_block* next = _root->next->next;

        if (next)
        {
          // deallocate the whole page, unless it was the first one
          EsXmlMemory::deallocate(_root->next);
          _root->next = next;
        }
      }
    }

    return result;
  }

  void revert(const xpath_allocator& state)
  {
    // free all new pages
    xpath_memory_block* cur = _root;

    while (cur != state._root)
    {
      xpath_memory_block* next = cur->next;

      EsXmlMemory::deallocate(cur);

      cur = next;
    }

    // restore state
    _root = state._root;
    _root_size = state._root_size;
  }

  void release()
  {
    xpath_memory_block* cur = _root;
    ES_ASSERT(cur);

    while (cur->next)
    {
      xpath_memory_block* next = cur->next;

      EsXmlMemory::deallocate(cur);

      cur = next;
    }
  }
};

struct xpath_allocator_capture
{
  xpath_allocator_capture(xpath_allocator* alloc): _target(alloc), _state(*alloc)
  {
  }

  ~xpath_allocator_capture()
  {
    _target->revert(_state);
  }

  xpath_allocator* _target;
  xpath_allocator _state;
};

struct xpath_stack
{
  xpath_allocator* result;
  xpath_allocator* temp;
};

struct xpath_stack_data
{
  xpath_memory_block blocks[2];
  xpath_allocator result;
  xpath_allocator temp;
  xpath_stack stack;

#ifdef PUGIXML_NO_EXCEPTIONS
  jmp_buf error_handler;
#endif

  xpath_stack_data(): result(blocks + 0), temp(blocks + 1)
  {
    blocks[0].next = blocks[1].next = 0;

    stack.result = &result;
    stack.temp = &temp;

  #ifdef PUGIXML_NO_EXCEPTIONS
    result.error_handler = temp.error_handler = &error_handler;
  #endif
  }

  ~xpath_stack_data()
  {
    result.release();
    temp.release();
  }
};

// String class
class EsXmlXpathString
{
  EsString::const_pointer _buffer;
  bool _uses_heap;

  static EsString::pointer duplicate_string(EsString::const_pointer string, size_t length, xpath_allocator* alloc)
  {
    EsString::pointer result = static_cast<EsString::pointer>(alloc->allocate((length + 1) * sizeof(EsString::value_type)));
    ES_ASSERT(result);

    memcpy(result, string, length * sizeof(EsString::value_type));
    result[length] = 0;

    return result;
  }

  static EsString::pointer duplicate_string(EsString::const_pointer string, xpath_allocator* alloc)
  {
    return duplicate_string(string, strlength(string), alloc);
  }

public:
  EsXmlXpathString(): _buffer(esT("")), _uses_heap(false)
  {
  }

  explicit EsXmlXpathString(EsString::const_pointer str, xpath_allocator* alloc)
  {
    bool empty_ = (*str == 0);

    _buffer = empty_ ? esT("") : duplicate_string(str, alloc);
    _uses_heap = !empty_;
  }

  explicit EsXmlXpathString(EsString::const_pointer str, bool use_heap): _buffer(str), _uses_heap(use_heap)
  {
  }

  EsXmlXpathString(EsString::const_pointer begin, EsString::const_pointer end, xpath_allocator* alloc)
  {
    ES_ASSERT(begin <= end);

    bool empty_ = (begin == end);

    _buffer = empty_ ? esT("") : duplicate_string(begin, static_cast<size_t>(end - begin), alloc);
    _uses_heap = !empty_;
  }

  void append(const EsXmlXpathString& o, xpath_allocator* alloc)
  {
    // skip empty sources
    if (!*o._buffer) return;

    // fast append for constant empty target and constant source
    if (!*_buffer && !_uses_heap && !o._uses_heap)
    {
      _buffer = o._buffer;
    }
    else
    {
      // need to make heap copy
      size_t target_length = strlength(_buffer);
      size_t source_length = strlength(o._buffer);
      size_t result_length = target_length + source_length;

      // allocate new buffer
      EsString::pointer result = static_cast<EsString::pointer>(alloc->reallocate(_uses_heap ? const_cast<EsString::pointer>(_buffer) : 0, (target_length + 1) * sizeof(EsString::value_type), (result_length + 1) * sizeof(EsString::value_type)));
      ES_ASSERT(result);

      // append first string to the new buffer in case there was no reallocation
      if (!_uses_heap) memcpy(result, _buffer, target_length * sizeof(EsString::value_type));

      // append second string to the new buffer
      memcpy(result + target_length, o._buffer, source_length * sizeof(EsString::value_type));
      result[result_length] = 0;

      // finalize
      _buffer = result;
      _uses_heap = true;
    }
  }

  EsString::const_pointer c_str() const
  {
    return _buffer;
  }

  size_t length() const
  {
    return strlength(_buffer);
  }

  EsString::pointer data(xpath_allocator* alloc)
  {
    // make private heap copy
    if (!_uses_heap)
    {
      _buffer = duplicate_string(_buffer, alloc);
      _uses_heap = true;
    }

    return const_cast<EsString::pointer>(_buffer);
  }

  bool empty() const
  {
    return *_buffer == 0;
  }

  bool operator==(const EsXmlXpathString& o) const
  {
    return strequal(_buffer, o._buffer);
  }

  bool operator!=(const EsXmlXpathString& o) const
  {
    return !strequal(_buffer, o._buffer);
  }

  bool uses_heap() const
  {
    return _uses_heap;
  }
};

EsXmlXpathString xpath_string_const(EsString::const_pointer str)
{
  return EsXmlXpathString(str, false);
}

bool starts_with(EsString::const_pointer string, EsString::const_pointer pattern)
{
  while (*pattern && *string == *pattern)
  {
    string++;
    pattern++;
  }

  return *pattern == 0;
}

EsString::const_pointer find_char(EsString::const_pointer s, EsString::value_type c)
{
  return esStrchr(s, c);
}

EsString::const_pointer find_substring(EsString::const_pointer s, EsString::const_pointer p)
{
  return esStrstr(s, p);
}

// Converts symbol to lower case, if it is an ASCII one
EsString::value_type tolower_ascii(EsString::value_type ch)
{
  return static_cast<unsigned int>(ch - 'A') < 26 ? static_cast<EsString::value_type>(ch | ' ') : ch;
}

EsXmlXpathString string_value(const EsXmlXpathNode& na, xpath_allocator* alloc)
{
  if (na.attributeGet())
    return xpath_string_const(na.attributeGet().valueGet());
  else
  {
    const EsXmlNode& n = na.nodeGet();

    switch (n.typeGet())
    {
    case xmlNodePcdata:
    case xmlNodeCdata:
    case xmlNodeComment:
    case xmlNodeProcInstr:
      return xpath_string_const(n.valueGet());

    case xmlNodeDocument:
    case xmlNodeElement:
    {
      EsXmlXpathString result;

      EsXmlNode cur = n.firstChildGet();

      while (cur && cur != n)
      {
        if (cur.typeGet() == xmlNodePcdata || cur.typeGet() == xmlNodeCdata)
          result.append(xpath_string_const(cur.valueGet()), alloc);

        if (cur.firstChildGet())
          cur = cur.firstChildGet();
        else if (cur.nextSiblingGet())
          cur = cur.nextSiblingGet();
        else
        {
          while (!cur.nextSiblingGet() && cur != n)
            cur = cur.parentGet();

          if (cur != n) cur = cur.nextSiblingGet();
        }
      }

      return result;
    }

    default:
      return EsXmlXpathString();
    }
  }
}

unsigned int node_height(EsXmlNode n)
{
  unsigned int result = 0;

  while (n)
  {
    ++result;
    n = n.parentGet();
  }

  return result;
}

bool node_is_before(EsXmlNode ln, unsigned int lh, EsXmlNode rn, unsigned int rh)
{
  // normalize heights
  for (unsigned int i = rh; i < lh; i++) ln = ln.parentGet();
  for (unsigned int j = lh; j < rh; j++) rn = rn.parentGet();

  // one node is the ancestor of the other
  if (ln == rn) return lh < rh;

  // find common ancestor
  while (ln.parentGet() != rn.parentGet())
  {
    ln = ln.parentGet();
    rn = rn.parentGet();
  }

  // there is no common ancestor (the shared parent is null), nodes are from different documents
  if (!ln.parentGet()) return ln < rn;

  // determine sibling order
  for (; ln; ln = ln.nextSiblingGet())
    if (ln == rn)
      return true;

  return false;
}

bool node_is_ancestor(EsXmlNode parent, EsXmlNode node)
{
  while (node && node != parent) node = node.parentGet();

  return parent && node == parent;
}

const void* document_order(const EsXmlXpathNode& xnode)
{
  EsXmlNodeStruct* node = xnode.nodeGet().internalObjectGet();

  if (node)
  {
    if (node->name && (node->header & xml_memory_page_name_allocated_mask) == 0) return node->name;
    if (node->value && (node->header & xml_memory_page_value_allocated_mask) == 0) return node->value;
    return 0;
  }

  EsXmlAttributeStruct* attr = xnode.attributeGet().internalObjectGet();

  if (attr)
  {
    if ((attr->header & xml_memory_page_name_allocated_mask) == 0) return attr->name;
    if ((attr->header & xml_memory_page_value_allocated_mask) == 0) return attr->value;
    return 0;
  }

  return 0;
}

struct document_order_comparator
{
  bool operator()(const EsXmlXpathNode& lhs, const EsXmlXpathNode& rhs) const
  {
    // optimized document order based check
    const void* lo = document_order(lhs);
    const void* ro = document_order(rhs);

    if (lo && ro) return lo < ro;

    // slow comparison
    EsXmlNode ln = lhs.nodeGet(), rn = rhs.nodeGet();

    // compare attributes
    if (lhs.attributeGet() && rhs.attributeGet())
    {
      // shared parent
      if (lhs.parentGet() == rhs.parentGet())
      {
        // determine sibling order
        for (EsXmlAttribute a = lhs.attributeGet(); a; a = a.nextAttributeGet())
          if (a == rhs.attributeGet())
            return true;

        return false;
      }

      // compare attribute parents
      ln = lhs.parentGet();
      rn = rhs.parentGet();
    }
    else if (lhs.attributeGet())
    {
      // attributes go after the parent element
      if (lhs.parentGet() == rhs.nodeGet()) return false;

      ln = lhs.parentGet();
    }
    else if (rhs.attributeGet())
    {
      // attributes go after the parent element
      if (rhs.parentGet() == lhs.nodeGet()) return true;

      rn = rhs.parentGet();
    }

    if (ln == rn) return false;

    unsigned int lh = node_height(ln);
    unsigned int rh = node_height(rn);

    return node_is_before(ln, lh, rn, rh);
  }
};

struct duplicate_comparator
{
  bool operator()(const EsXmlXpathNode& lhs, const EsXmlXpathNode& rhs) const
  {
    if (lhs.attributeGet()) return rhs.attributeGet() ? lhs.attributeGet() < rhs.attributeGet() : true;
    else return rhs.attributeGet() ? false : lhs.nodeGet() < rhs.nodeGet();
  }
};

double gen_nan()
{
#if defined(__STDC_IEC_559__) || ((FLT_RADIX - 0 == 2) && (FLT_MAX_EXP - 0 == 128) && (FLT_MANT_DIG - 0 == 24))
  union { float f; uint32_t i; } u[sizeof(float) == sizeof(uint32_t) ? 1 : -1];
  u[0].i = 0x7fc00000;
  return u[0].f;
#else
  // fallback
  const volatile double zero = 0.0;
  return zero / zero;
#endif
}

EsString::const_pointer convert_number_to_string_special(double value)
{
#if defined(PUGI__MSVC_CRT_VERSION) || defined(__BORLANDC__)
  if (esFinite(value)) return (value == 0) ? esT("0") : 0;
  if (esIsNan(value)) return esT("NaN");
  return value > 0 ? esT("Infinity") : esT("-Infinity");
#elif defined(fpclassify) && defined(FP_NAN) && defined(FP_INFINITE) && defined(FP_ZERO)
  switch (fpclassify(value))
  {
  case FP_NAN:
    return esT("NaN");

  case FP_INFINITE:
    return value > 0 ? esT("Infinity") : esT("-Infinity");

  case FP_ZERO:
    return esT("0");

  default:
    return 0;
  }
#else
  // fallback
  const volatile double v = value;

  if (v == 0) return esT("0");
  if (v != v) return esT("NaN");
  if (v * 2 == v) return value > 0 ? esT("Infinity") : esT("-Infinity");
  return 0;
#endif
}

bool convert_number_to_boolean(double value)
{
  return (value != 0 && !esIsNan(value));
}

void truncate_zeros(char* begin, char* end)
{
  while (begin != end && end[-1] == '0') end--;

  *end = 0;
}

// gets mantissa digits in the form of 0.xxxxx with 0. implied and the exponent
#if defined(PUGI__MSVC_CRT_VERSION) && PUGI__MSVC_CRT_VERSION >= 1400 && !defined(_WIN32_WCE)
void convert_number_to_mantissa_exponent(double value, char* buffer, size_t buffer_size, char** out_mantissa, int* out_exponent)
{
  // get base values
  int sign, exponent;
  _ecvt_s(buffer, buffer_size, value, DBL_DIG + 1, &exponent, &sign);

  // truncate redundant zeros
  truncate_zeros(buffer, buffer + strlen(buffer));

  // fill results
  *out_mantissa = buffer;
  *out_exponent = exponent;
}
#else
void convert_number_to_mantissa_exponent(double value, char* buffer, size_t buffer_size, char** out_mantissa, int* out_exponent)
{
  // get a scientific notation value with IEEE DBL_DIG decimals
  sprintf(buffer, "%.*e", DBL_DIG, value);
  ES_ASSERT(strlen(buffer) < buffer_size);
  (void)!buffer_size;

  // get the exponent (possibly negative)
  char* exponent_string = strchr(buffer, 'e');
  ES_ASSERT(exponent_string);

  int exponent = atoi(exponent_string + 1);

  // extract mantissa string: skip sign
  char* mantissa = buffer[0] == '-' ? buffer + 1 : buffer;
  ES_ASSERT(mantissa[0] != '0' && mantissa[1] == '.');

  // divide mantissa by 10 to eliminate integer part
  mantissa[1] = mantissa[0];
  mantissa++;
  exponent++;

  // remove extra mantissa digits and zero-terminate mantissa
  truncate_zeros(mantissa, exponent_string);

  // fill results
  *out_mantissa = mantissa;
  *out_exponent = exponent;
}
#endif

EsXmlXpathString convert_number_to_string(double value, xpath_allocator* alloc)
{
  // try special number conversion
  EsString::const_pointer special = convert_number_to_string_special(value);
  if (special) return xpath_string_const(special);

  // get mantissa + exponent form
  char mantissa_buffer[32];

  char* mantissa;
  int exponent;
  convert_number_to_mantissa_exponent(value, mantissa_buffer, sizeof(mantissa_buffer), &mantissa, &exponent);

  // allocate a buffer of suitable length for the number
  size_t result_size = strlen(mantissa_buffer) + (exponent > 0 ? exponent : -exponent) + 4;
  EsString::pointer result = static_cast<EsString::pointer>(alloc->allocate(sizeof(EsString::value_type) * result_size));
  ES_ASSERT(result);

  // make the number!
  EsString::pointer s = result;

  // sign
  if (value < 0) *s++ = '-';

  // integer part
  if (exponent <= 0)
  {
    *s++ = '0';
  }
  else
  {
    while (exponent > 0)
    {
      ES_ASSERT(*mantissa == 0 || static_cast<unsigned int>(static_cast<unsigned int>(*mantissa) - '0') <= 9);
      *s++ = *mantissa ? *mantissa++ : '0';
      exponent--;
    }
  }

  // fractional part
  if (*mantissa)
  {
    // decimal point
    *s++ = '.';

    // extra zeroes from negative exponent
    while (exponent < 0)
    {
      *s++ = '0';
      exponent++;
    }

    // extra mantissa digits
    while (*mantissa)
    {
      ES_ASSERT(static_cast<unsigned int>(*mantissa - '0') <= 9);
      *s++ = *mantissa++;
    }
  }

  // zero-terminate
  ES_ASSERT(s < result + result_size);
  *s = 0;

  return EsXmlXpathString(result, true);
}

bool check_string_to_number_format(EsString::const_pointer string)
{
  // parse leading whitespace
  while (PUGI__IS_CHARTYPE(*string, ct_space)) ++string;

  // parse sign
  if (*string == '-') ++string;

  if (!*string) return false;

  // if there is no integer part, there should be a decimal part with at least one digit
  if (!PUGI__IS_CHARTYPEX(string[0], ctx_digit) && (string[0] != '.' || !PUGI__IS_CHARTYPEX(string[1], ctx_digit))) return false;

  // parse integer part
  while (PUGI__IS_CHARTYPEX(*string, ctx_digit)) ++string;

  // parse decimal part
  if (*string == '.')
  {
    ++string;

    while (PUGI__IS_CHARTYPEX(*string, ctx_digit)) ++string;
  }

  // parse trailing whitespace
  while (PUGI__IS_CHARTYPE(*string, ct_space)) ++string;

  return *string == 0;
}

double convert_string_to_number(EsString::const_pointer string)
{
  // check string format
  if (!check_string_to_number_format(string)) return gen_nan();

  return EsString::toDouble(string);
}

bool convert_string_to_number_scratch(EsString::value_type (&buffer)[32], EsString::const_pointer begin, EsString::const_pointer end, double* out_result)
{
  size_t length = static_cast<size_t>(end - begin);
  EsString::pointer scratch = buffer;

  if (length >= sizeof(buffer) / sizeof(buffer[0]))
  {
    // need to make dummy on-heap copy
    scratch = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
    if (!scratch) return false;
  }

  // copy string to zero-terminated buffer and perform conversion
  memcpy(scratch, begin, length * sizeof(EsString::value_type));
  scratch[length] = 0;

  *out_result = convert_string_to_number(scratch);

  // free dummy buffer
  if (scratch != buffer) EsXmlMemory::deallocate(scratch);

  return true;
}

double round_nearest(double value)
{
  return floor(value + 0.5);
}

double round_nearest_nzero(double value)
{
  // same as round_nearest, but returns -0 for [-0.5, -0]
  // ceil is used to differentiate between +0 and -0 (we return -0 for [-0.5, -0] and +0 for +0)
  return (value >= -0.5 && value <= 0) ? ceil(value) : floor(value + 0.5);
}

EsString::const_pointer qualified_name(const EsXmlXpathNode& node)
{
  return node.attributeGet() ? node.attributeGet().nameGet() : node.nodeGet().nameGet();
}

EsString::const_pointer local_name(const EsXmlXpathNode& node)
{
  EsString::const_pointer name = qualified_name(node);
  EsString::const_pointer p = find_char(name, ':');

  return p ? p + 1 : name;
}

struct namespace_uri_predicate
{
  EsString::const_pointer prefix;
  size_t prefix_length;

  namespace_uri_predicate(EsString::const_pointer name)
  {
    EsString::const_pointer pos = find_char(name, ':');

    prefix = pos ? name : 0;
    prefix_length = pos ? static_cast<size_t>(pos - name) : 0;
  }

  bool operator()(const EsXmlAttribute& a) const
  {
    EsString::const_pointer name = a.nameGet();

    if (!starts_with(name, esT("xmlns"))) return false;

    return prefix ? name[5] == ':' && strequalrange(name + 6, prefix, prefix_length) : name[5] == 0;
  }
};

EsString::const_pointer namespace_uri(const EsXmlNode& node)
{
  namespace_uri_predicate pred = node.nameGet();

  EsXmlNode p = node;

  while (p)
  {
    EsXmlAttribute a = p.attributeFind(pred);

    if (a) return a.valueGet();

    p = p.parentGet();
  }

  return esT("");
}

EsString::const_pointer namespace_uri(const EsXmlAttribute& attr, const EsXmlNode& parent)
{
  namespace_uri_predicate pred = attr.nameGet();

  // Default namespace does not apply to attributes
  if (!pred.prefix) return esT("");

  EsXmlNode p = parent;

  while (p)
  {
    EsXmlAttribute a = p.attributeFind(pred);

    if (a) return a.valueGet();

    p = p.parentGet();
  }

  return esT("");
}

EsString::const_pointer namespace_uri(const EsXmlXpathNode& node)
{
  return node.attributeGet() ? namespace_uri(node.attributeGet(), node.parentGet()) : namespace_uri(node.nodeGet());
}

void normalize_space(EsString::pointer buffer)
{
  EsString::pointer write = buffer;

  for (EsString::pointer it = buffer; *it; )
  {
    EsString::value_type ch = *it++;

    if (PUGI__IS_CHARTYPE(ch, ct_space))
    {
      // replace whitespace sequence with single space
      while (PUGI__IS_CHARTYPE(*it, ct_space)) it++;

      // avoid leading spaces
      if (write != buffer) *write++ = ' ';
    }
    else *write++ = ch;
  }

  // remove trailing space
  if (write != buffer && PUGI__IS_CHARTYPE(write[-1], ct_space)) write--;

  // zero-terminate
  *write = 0;
}

void translate(EsString::pointer buffer, EsString::const_pointer from, EsString::const_pointer to)
{
  size_t to_length = strlength(to);

  EsString::pointer write = buffer;

  while (*buffer)
  {
    volatile EsString::value_type ch = *buffer++;

    EsString::const_pointer pos = find_char(from, ch);

    if (!pos)
      *write++ = ch; // do not process
    else if (static_cast<size_t>(pos - from) < to_length)
      *write++ = to[pos - from]; // replace
  }

  // zero-terminate
  *write = 0;
}

struct xpath_variable_boolean: EsXmlXpathVariable
{
  xpath_variable_boolean(): value(false)
  {
  }

  bool value;
  EsString::value_type name[1];
};

struct xpath_variable_number: EsXmlXpathVariable
{
  xpath_variable_number(): value(0)
  {
  }

  double value;
  EsString::value_type name[1];
};

struct xpath_variable_string: EsXmlXpathVariable
{
  xpath_variable_string(): value(0)
  {
  }

  ~xpath_variable_string()
  {
    if (value) EsXmlMemory::deallocate(value);
  }

  EsString::pointer value;
  EsString::value_type name[1];
};

struct xpath_variable_node_set: EsXmlXpathVariable
{
  EsXmlXpathNodeSet value;
  EsString::value_type name[1];
};

static const EsXmlXpathNodeSet dummy_node_set;

unsigned int hash_string(EsString::const_pointer str)
{
  // Jenkins one-at-a-time hash (http://en.wikipedia.org/wiki/Jenkins_hash_function#one-at-a-time)
  unsigned int result = 0;

  while (*str)
  {
    result += static_cast<unsigned int>(*str++);
    result += result << 10;
    result ^= result >> 6;
  }

  result += result << 3;
  result ^= result >> 11;
  result += result << 15;

  return result;
}

template <typename T> T* new_xpath_variable(EsString::const_pointer name)
{
  size_t length = strlength(name);
  if (length == 0) return 0; // empty variable names are invalid

  // $$ we can't use offsetof(T, name) because T is non-POD, so we just allocate additional length characters
  void* memory = EsXmlMemory::allocate(sizeof(T) + length * sizeof(EsString::value_type));
  if (!memory) return 0;

  T* result = new (memory) T();

  memcpy(result->name, name, (length + 1) * sizeof(EsString::value_type));

  return result;
}

EsXmlXpathVariable* new_xpath_variable(EsXmlXpathValueType type, EsString::const_pointer name)
{
  switch (type)
  {
  case xpathTypeNodeSet:
    return new_xpath_variable<xpath_variable_node_set>(name);

  case xpathTypeNumber:
    return new_xpath_variable<xpath_variable_number>(name);

  case xpathTypeString:
    return new_xpath_variable<xpath_variable_string>(name);

  case xpathTypeBoolean:
    return new_xpath_variable<xpath_variable_boolean>(name);

  default:
    return 0;
  }
}

template <typename T> void delete_xpath_variable(T* var)
{
  var->~T();
  EsXmlMemory::deallocate(var);
}

void delete_xpath_variable(EsXmlXpathValueType type, EsXmlXpathVariable* var)
{
  switch (type)
  {
  case xpathTypeNodeSet:
    delete_xpath_variable(static_cast<xpath_variable_node_set*>(var));
    break;

  case xpathTypeNumber:
    delete_xpath_variable(static_cast<xpath_variable_number*>(var));
    break;

  case xpathTypeString:
    delete_xpath_variable(static_cast<xpath_variable_string*>(var));
    break;

  case xpathTypeBoolean:
    delete_xpath_variable(static_cast<xpath_variable_boolean*>(var));
    break;

  default:
    ES_ASSERT(!"Invalid variable type");
  }
}

EsXmlXpathVariable* get_variable_scratch(EsString::value_type (&buffer)[32], EsXmlXpathVariableSet* set, EsString::const_pointer begin, EsString::const_pointer end)
{
  size_t length = static_cast<size_t>(end - begin);
  EsString::pointer scratch = buffer;

  if (length >= sizeof(buffer) / sizeof(buffer[0]))
  {
    // need to make dummy on-heap copy
    scratch = static_cast<EsString::pointer>(EsXmlMemory::allocate((length + 1) * sizeof(EsString::value_type)));
    if (!scratch) return 0;
  }

  // copy string to zero-terminated buffer and perform lookup
  memcpy(scratch, begin, length * sizeof(EsString::value_type));
  scratch[length] = 0;

  EsXmlXpathVariable* result = set->get(scratch);

  // free dummy buffer
  if (scratch != buffer) EsXmlMemory::deallocate(scratch);

  return result;
}

// Internal node set class
EsXmlXpathNodeSet::type_t xpath_sort(EsXmlXpathNode* begin, EsXmlXpathNode* end, EsXmlXpathNodeSet::type_t type, bool rev)
{
  EsXmlXpathNodeSet::type_t order = rev ? EsXmlXpathNodeSet::type_sorted_reverse : EsXmlXpathNodeSet::type_sorted;

  if (type == EsXmlXpathNodeSet::type_unsorted)
  {
    EsXml::sort(begin, end, document_order_comparator());

    type = EsXmlXpathNodeSet::type_sorted;
  }

  if (type != order) EsXml::reverse(begin, end);

  return order;
}

EsXmlXpathNode xpath_first(const EsXmlXpathNode* begin, const EsXmlXpathNode* end, EsXmlXpathNodeSet::type_t type)
{
  if (begin == end) return EsXmlXpathNode();

  switch (type)
  {
  case EsXmlXpathNodeSet::type_sorted:
    return *begin;

  case EsXmlXpathNodeSet::type_sorted_reverse:
    return *(end - 1);

  case EsXmlXpathNodeSet::type_unsorted:
    return *EsXml::min_element(begin, end, document_order_comparator());

  default:
    ES_ASSERT(!"Invalid node set type");
    return EsXmlXpathNode();
  }
}

class xpath_node_set_raw
{
  EsXmlXpathNodeSet::type_t _type;

  EsXmlXpathNode* _begin;
  EsXmlXpathNode* _end;
  EsXmlXpathNode* _eos;

public:
  xpath_node_set_raw(): _type(EsXmlXpathNodeSet::type_unsorted), _begin(0), _end(0), _eos(0)
  {
  }

  EsXmlXpathNode* begin() const
  {
    return _begin;
  }

  EsXmlXpathNode* end() const
  {
    return _end;
  }

  bool empty() const
  {
    return _begin == _end;
  }

  size_t size() const
  {
    return static_cast<size_t>(_end - _begin);
  }

  EsXmlXpathNode first() const
  {
    return xpath_first(_begin, _end, _type);
  }

  void push_back(const EsXmlXpathNode& node, xpath_allocator* alloc)
  {
    if (_end == _eos)
    {
      size_t capacity = static_cast<size_t>(_eos - _begin);

      // get new capacity (1.5x rule)
      size_t new_capacity = capacity + capacity / 2 + 1;

      // reallocate the old array or allocate a new one
      EsXmlXpathNode* data = static_cast<EsXmlXpathNode*>(alloc->reallocate(_begin, capacity * sizeof(EsXmlXpathNode), new_capacity * sizeof(EsXmlXpathNode)));
      ES_ASSERT(data);

      // finalize
      _begin = data;
      _end = data + capacity;
      _eos = data + new_capacity;
    }

    *_end++ = node;
  }

  void append(const EsXmlXpathNode* begin_, const EsXmlXpathNode* end_, xpath_allocator* alloc)
  {
    size_t size_ = static_cast<size_t>(_end - _begin);
    size_t capacity = static_cast<size_t>(_eos - _begin);
    size_t count = static_cast<size_t>(end_ - begin_);

    if (size_ + count > capacity)
    {
      // reallocate the old array or allocate a new one
      EsXmlXpathNode* data = static_cast<EsXmlXpathNode*>(alloc->reallocate(_begin, capacity * sizeof(EsXmlXpathNode), (size_ + count) * sizeof(EsXmlXpathNode)));
      ES_ASSERT(data);

      // finalize
      _begin = data;
      _end = data + size_;
      _eos = data + size_ + count;
    }

    memcpy(_end, begin_, count * sizeof(EsXmlXpathNode));
    _end += count;
  }

  void sort_do()
  {
    _type = xpath_sort(_begin, _end, _type, false);
  }

  void truncate(EsXmlXpathNode* pos)
  {
    ES_ASSERT(_begin <= pos && pos <= _end);

    _end = pos;
  }

  void remove_duplicates()
  {
    if (_type == EsXmlXpathNodeSet::type_unsorted)
      EsXml::sort(_begin, _end, duplicate_comparator());

    _end = EsXml::unique(_begin, _end);
  }

  EsXmlXpathNodeSet::type_t typeGet() const
  {
    return _type;
  }

  void set_type(EsXmlXpathNodeSet::type_t value)
  {
    _type = value;
  }
};

struct xpath_context
{
  EsXmlXpathNode n;
  size_t position, size;

  xpath_context(const EsXmlXpathNode& n_, size_t position_, size_t size_): n(n_), position(position_), size(size_)
  {
  }
};

enum lexeme_t
{
  lex_none = 0,
  lex_equal,
  lex_not_equal,
  lex_less,
  lex_greater,
  lex_less_or_equal,
  lex_greater_or_equal,
  lex_plus,
  lex_minus,
  lex_multiply,
  lex_union,
  lex_var_ref,
  lex_open_brace,
  lex_close_brace,
  lex_quoted_string,
  lex_number,
  lex_slash,
  lex_double_slash,
  lex_open_square_brace,
  lex_close_square_brace,
  lex_string,
  lex_comma,
  lex_axis_attribute,
  lex_dot,
  lex_double_dot,
  lex_double_colon,
  lex_eof
};

struct xpath_lexer_string
{
  EsString::const_pointer begin;
  EsString::const_pointer end;

  xpath_lexer_string(): begin(0), end(0)
  {
  }

  bool operator==(EsString::const_pointer other) const
  {
    size_t length = static_cast<size_t>(end - begin);

    return strequalrange(other, begin, length);
  }
};

class xpath_lexer
{
  EsString::const_pointer _cur;
  EsString::const_pointer _cur_lexeme_pos;
  xpath_lexer_string _cur_lexeme_contents;

  lexeme_t _cur_lexeme;

public:
  explicit xpath_lexer(EsString::const_pointer query): _cur(query)
  {
    next();
  }

  EsString::const_pointer state() const
  {
    return _cur;
  }

  void next()
  {
    EsString::const_pointer cur = _cur;

    while (PUGI__IS_CHARTYPE(*cur, ct_space)) ++cur;

    // save lexeme position for error reporting
    _cur_lexeme_pos = cur;

    switch (*cur)
    {
    case 0:
      _cur_lexeme = lex_eof;
      break;

    case '>':
      if (*(cur+1) == '=')
      {
        cur += 2;
        _cur_lexeme = lex_greater_or_equal;
      }
      else
      {
        cur += 1;
        _cur_lexeme = lex_greater;
      }
      break;

    case '<':
      if (*(cur+1) == '=')
      {
        cur += 2;
        _cur_lexeme = lex_less_or_equal;
      }
      else
      {
        cur += 1;
        _cur_lexeme = lex_less;
      }
      break;

    case '!':
      if (*(cur+1) == '=')
      {
        cur += 2;
        _cur_lexeme = lex_not_equal;
      }
      else
      {
        _cur_lexeme = lex_none;
      }
      break;

    case '=':
      cur += 1;
      _cur_lexeme = lex_equal;

      break;

    case '+':
      cur += 1;
      _cur_lexeme = lex_plus;

      break;

    case '-':
      cur += 1;
      _cur_lexeme = lex_minus;

      break;

    case '*':
      cur += 1;
      _cur_lexeme = lex_multiply;

      break;

    case '|':
      cur += 1;
      _cur_lexeme = lex_union;

      break;

    case '$':
      cur += 1;

      if (PUGI__IS_CHARTYPEX(*cur, ctx_start_symbol))
      {
        _cur_lexeme_contents.begin = cur;

        while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;

        if (cur[0] == ':' && PUGI__IS_CHARTYPEX(cur[1], ctx_symbol)) // qname
        {
          cur++; // :

          while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;
        }

        _cur_lexeme_contents.end = cur;

        _cur_lexeme = lex_var_ref;
      }
      else
      {
        _cur_lexeme = lex_none;
      }

      break;

    case '(':
      cur += 1;
      _cur_lexeme = lex_open_brace;

      break;

    case ')':
      cur += 1;
      _cur_lexeme = lex_close_brace;

      break;

    case '[':
      cur += 1;
      _cur_lexeme = lex_open_square_brace;

      break;

    case ']':
      cur += 1;
      _cur_lexeme = lex_close_square_brace;

      break;

    case ',':
      cur += 1;
      _cur_lexeme = lex_comma;

      break;

    case '/':
      if (*(cur+1) == '/')
      {
        cur += 2;
        _cur_lexeme = lex_double_slash;
      }
      else
      {
        cur += 1;
        _cur_lexeme = lex_slash;
      }
      break;

    case '.':
      if (*(cur+1) == '.')
      {
        cur += 2;
        _cur_lexeme = lex_double_dot;
      }
      else if (PUGI__IS_CHARTYPEX(*(cur+1), ctx_digit))
      {
        _cur_lexeme_contents.begin = cur; // .

        ++cur;

        while (PUGI__IS_CHARTYPEX(*cur, ctx_digit)) cur++;

        _cur_lexeme_contents.end = cur;

        _cur_lexeme = lex_number;
      }
      else
      {
        cur += 1;
        _cur_lexeme = lex_dot;
      }
      break;

    case '@':
      cur += 1;
      _cur_lexeme = lex_axis_attribute;

      break;

    case '"':
    case '\'':
    {
      EsString::value_type terminator = *cur;

      ++cur;

      _cur_lexeme_contents.begin = cur;
      while (*cur && *cur != terminator) cur++;
      _cur_lexeme_contents.end = cur;

      if (!*cur)
        _cur_lexeme = lex_none;
      else
      {
        cur += 1;
        _cur_lexeme = lex_quoted_string;
      }

      break;
    }

    case ':':
      if (*(cur+1) == ':')
      {
        cur += 2;
        _cur_lexeme = lex_double_colon;
      }
      else
      {
        _cur_lexeme = lex_none;
      }
      break;

    default:
      if (PUGI__IS_CHARTYPEX(*cur, ctx_digit))
      {
        _cur_lexeme_contents.begin = cur;

        while (PUGI__IS_CHARTYPEX(*cur, ctx_digit)) cur++;

        if (*cur == '.')
        {
          cur++;

          while (PUGI__IS_CHARTYPEX(*cur, ctx_digit)) cur++;
        }

        _cur_lexeme_contents.end = cur;

        _cur_lexeme = lex_number;
      }
      else if (PUGI__IS_CHARTYPEX(*cur, ctx_start_symbol))
      {
        _cur_lexeme_contents.begin = cur;

        while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;

        if (cur[0] == ':')
        {
          if (cur[1] == '*') // namespace test ncname:*
          {
            cur += 2; // :*
          }
          else if (PUGI__IS_CHARTYPEX(cur[1], ctx_symbol)) // namespace test qname
          {
            cur++; // :

            while (PUGI__IS_CHARTYPEX(*cur, ctx_symbol)) cur++;
          }
        }

        _cur_lexeme_contents.end = cur;

        _cur_lexeme = lex_string;
      }
      else
      {
        _cur_lexeme = lex_none;
      }
    }

    _cur = cur;
  }

  lexeme_t current() const
  {
    return _cur_lexeme;
  }

  EsString::const_pointer current_pos() const
  {
    return _cur_lexeme_pos;
  }

  const xpath_lexer_string& contents() const
  {
    ES_ASSERT(_cur_lexeme == lex_var_ref || _cur_lexeme == lex_number || _cur_lexeme == lex_string || _cur_lexeme == lex_quoted_string);

    return _cur_lexeme_contents;
  }
};

enum ast_type_t
{
  ast_unknown,
  ast_op_or,            // left or right
  ast_op_and,            // left and right
  ast_op_equal,          // left = right
  ast_op_not_equal,        // left != right
  ast_op_less,          // left < right
  ast_op_greater,          // left > right
  ast_op_less_or_equal,      // left <= right
  ast_op_greater_or_equal,    // left >= right
  ast_op_add,            // left + right
  ast_op_subtract,        // left - right
  ast_op_multiply,        // left * right
  ast_op_divide,          // left / right
  ast_op_mod,            // left % right
  ast_op_negate,          // left - right
  ast_op_union,          // left | right
  ast_predicate,          // apply predicate to set; next points to next predicate
  ast_filter,            // select * from left where right
  ast_filter_posinv,        // select * from left where right; proximity position invariant
  ast_string_constant,      // string constant
  ast_number_constant,      // number constant
  ast_variable,          // variable
  ast_func_last,          // last()
  ast_func_position,        // position()
  ast_func_count,          // count(left)
  ast_func_id,          // id(left)
  ast_func_local_name_0,      // local-nameGet()
  ast_func_local_name_1,      // local-name(left)
  ast_func_namespace_uri_0,    // namespace-uri()
  ast_func_namespace_uri_1,    // namespace-uri(left)
  ast_func_name_0,        // nameGet()
  ast_func_name_1,        // name(left)
  ast_func_string_0,        // string()
  ast_func_string_1,        // string(left)
  ast_func_concat,        // concat(left, right, siblings)
  ast_func_starts_with,      // starts_with(left, right)
  ast_func_contains,        // contains(left, right)
  ast_func_substring_before,    // substring-before(left, right)
  ast_func_substring_after,    // substring-after(left, right)
  ast_func_substring_2,      // substring(left, right)
  ast_func_substring_3,      // substring(left, right, third)
  ast_func_string_length_0,    // string-length()
  ast_func_string_length_1,    // string-length(left)
  ast_func_normalize_space_0,    // normalize-space()
  ast_func_normalize_space_1,    // normalize-space(left)
  ast_func_translate,        // translate(left, right, third)
  ast_func_boolean,        // boolean(left)
  ast_func_not,          // not(left)
  ast_func_true,          // true()
  ast_func_false,          // false()
  ast_func_lang,          // lang(left)
  ast_func_number_0,        // number()
  ast_func_number_1,        // number(left)
  ast_func_sum,          // sum(left)
  ast_func_floor,          // floor(left)
  ast_func_ceiling,        // ceiling(left)
  ast_func_round,          // round(left)
  ast_step,            // process set left with step
  ast_step_root          // select root node
};

enum axis_t
{
  axis_ancestor,
  axis_ancestor_or_self,
  axis_attribute,
  axis_child,
  axis_descendant,
  axis_descendant_or_self,
  axis_following,
  axis_following_sibling,
  axis_namespace,
  axis_parent,
  axis_preceding,
  axis_preceding_sibling,
  axis_self
};

enum nodetest_t
{
  nodetest_none,
  nodetest_name,
  nodetest_type_node,
  nodetest_type_comment,
  nodetest_type_pi,
  nodetest_type_text,
  nodetest_pi,
  nodetest_all,
  nodetest_all_in_namespace
};

template <axis_t N> struct axis_to_type
{
  static const axis_t axis;
};

template <axis_t N> const axis_t axis_to_type<N>::axis = N;

class xpath_ast_node
{
private:
  // node type
  char _type;
  char _rettype;

  // for ast_step / ast_predicate
  char _axis;
  char _test;

  // tree node structure
  xpath_ast_node* _left;
  xpath_ast_node* _right;
  xpath_ast_node* _next;

  union
  {
    // value for ast_string_constant
    EsString::const_pointer string;
    // value for ast_number_constant
    double number;
    // variable for ast_variable
    EsXmlXpathVariable* variable;
    // node test for ast_step (node name/namespace/node type/pi target)
    EsString::const_pointer nodetest;
  } _data;

  xpath_ast_node(const xpath_ast_node&);
  xpath_ast_node& operator=(const xpath_ast_node&);

  template <class Comp> static bool compare_eq(xpath_ast_node* lhs, xpath_ast_node* rhs, const xpath_context& c, const xpath_stack& stack, const Comp& comp)
  {
    EsXmlXpathValueType lt = lhs->rettype(), rt = rhs->rettype();

    if (lt != xpathTypeNodeSet && rt != xpathTypeNodeSet)
    {
      if (lt == xpathTypeBoolean || rt == xpathTypeBoolean)
        return comp(lhs->eval_boolean(c, stack), rhs->eval_boolean(c, stack));
      else if (lt == xpathTypeNumber || rt == xpathTypeNumber)
        return comp(lhs->eval_number(c, stack), rhs->eval_number(c, stack));
      else if (lt == xpathTypeString || rt == xpathTypeString)
      {
        xpath_allocator_capture cr(stack.result);

        EsXmlXpathString ls = lhs->eval_string(c, stack);
        EsXmlXpathString rs = rhs->eval_string(c, stack);

        return comp(ls, rs);
      }
    }
    else if (lt == xpathTypeNodeSet && rt == xpathTypeNodeSet)
    {
      xpath_allocator_capture cr(stack.result);

      xpath_node_set_raw ls = lhs->eval_node_set(c, stack);
      xpath_node_set_raw rs = rhs->eval_node_set(c, stack);

      for (const EsXmlXpathNode* li = ls.begin(); li != ls.end(); ++li)
        for (const EsXmlXpathNode* ri = rs.begin(); ri != rs.end(); ++ri)
        {
          xpath_allocator_capture cri(stack.result);

          if (comp(string_value(*li, stack.result), string_value(*ri, stack.result)))
            return true;
        }

      return false;
    }
    else
    {
      if (lt == xpathTypeNodeSet)
      {
        EsXml::swap(lhs, rhs);
        EsXml::swap(lt, rt);
      }

      if (lt == xpathTypeBoolean)
        return comp(lhs->eval_boolean(c, stack), rhs->eval_boolean(c, stack));
      else if (lt == xpathTypeNumber)
      {
        xpath_allocator_capture cr(stack.result);

        double l = lhs->eval_number(c, stack);
        xpath_node_set_raw rs = rhs->eval_node_set(c, stack);

        for (const EsXmlXpathNode* ri = rs.begin(); ri != rs.end(); ++ri)
        {
          xpath_allocator_capture cri(stack.result);

          if (comp(l, convert_string_to_number(string_value(*ri, stack.result).c_str())))
            return true;
        }

        return false;
      }
      else if (lt == xpathTypeString)
      {
        xpath_allocator_capture cr(stack.result);

        EsXmlXpathString l = lhs->eval_string(c, stack);
        xpath_node_set_raw rs = rhs->eval_node_set(c, stack);

        for (const EsXmlXpathNode* ri = rs.begin(); ri != rs.end(); ++ri)
        {
          xpath_allocator_capture cri(stack.result);

          if (comp(l, string_value(*ri, stack.result)))
            return true;
        }

        return false;
      }
    }

    ES_ASSERT(!"Wrong types");
    return false;
  }

  template <class Comp> static bool compare_rel(xpath_ast_node* lhs, xpath_ast_node* rhs, const xpath_context& c, const xpath_stack& stack, const Comp& comp)
  {
    EsXmlXpathValueType lt = lhs->rettype(), rt = rhs->rettype();

    if (lt != xpathTypeNodeSet && rt != xpathTypeNodeSet)
      return comp(lhs->eval_number(c, stack), rhs->eval_number(c, stack));
    else if (lt == xpathTypeNodeSet && rt == xpathTypeNodeSet)
    {
      xpath_allocator_capture cr(stack.result);

      xpath_node_set_raw ls = lhs->eval_node_set(c, stack);
      xpath_node_set_raw rs = rhs->eval_node_set(c, stack);

      for (const EsXmlXpathNode* li = ls.begin(); li != ls.end(); ++li)
      {
        xpath_allocator_capture cri(stack.result);

        double l = convert_string_to_number(string_value(*li, stack.result).c_str());

        for (const EsXmlXpathNode* ri = rs.begin(); ri != rs.end(); ++ri)
        {
          xpath_allocator_capture crii(stack.result);

          if (comp(l, convert_string_to_number(string_value(*ri, stack.result).c_str())))
            return true;
        }
      }

      return false;
    }
    else if (lt != xpathTypeNodeSet && rt == xpathTypeNodeSet)
    {
      xpath_allocator_capture cr(stack.result);

      double l = lhs->eval_number(c, stack);
      xpath_node_set_raw rs = rhs->eval_node_set(c, stack);

      for (const EsXmlXpathNode* ri = rs.begin(); ri != rs.end(); ++ri)
      {
        xpath_allocator_capture cri(stack.result);

        if (comp(l, convert_string_to_number(string_value(*ri, stack.result).c_str())))
          return true;
      }

      return false;
    }
    else if (lt == xpathTypeNodeSet && rt != xpathTypeNodeSet)
    {
      xpath_allocator_capture cr(stack.result);

      xpath_node_set_raw ls = lhs->eval_node_set(c, stack);
      double r = rhs->eval_number(c, stack);

      for (const EsXmlXpathNode* li = ls.begin(); li != ls.end(); ++li)
      {
        xpath_allocator_capture cri(stack.result);

        if (comp(convert_string_to_number(string_value(*li, stack.result).c_str()), r))
          return true;
      }

      return false;
    }
    else
    {
      ES_ASSERT(!"Wrong types");
      return false;
    }
  }

  void apply_predicate(xpath_node_set_raw& ns, size_t first, xpath_ast_node* expr, const xpath_stack& stack)
  {
    ES_ASSERT(ns.size() >= first);

    size_t i = 1;
    size_t size = ns.size() - first;

    EsXmlXpathNode* last = ns.begin() + first;

    // remove_if... or well, sort of
    for (EsXmlXpathNode* it = last; it != ns.end(); ++it, ++i)
    {
      xpath_context c(*it, i, size);

      if (expr->rettype() == xpathTypeNumber)
      {
        if (expr->eval_number(c, stack) == i)
          *last++ = *it;
      }
      else if (expr->eval_boolean(c, stack))
        *last++ = *it;
    }

    ns.truncate(last);
  }

  void apply_predicates(xpath_node_set_raw& ns, size_t first, const xpath_stack& stack)
  {
    if (ns.size() == first) return;

    for (xpath_ast_node* pred = _right; pred; pred = pred->_next)
    {
      apply_predicate(ns, first, pred->_left, stack);
    }
  }

  void step_push(xpath_node_set_raw& ns, const EsXmlAttribute& a, const EsXmlNode& parent, xpath_allocator* alloc)
  {
    if (!a) return;

    EsString::const_pointer name = a.nameGet();

    // There are no attribute nodes corresponding to attributes that declare namespaces
    // That is, "xmlns:..." or "xmlns"
    if (starts_with(name, esT("xmlns")) && (name[5] == 0 || name[5] == ':')) return;

    switch (_test)
    {
    case nodetest_name:
      if (strequal(name, _data.nodetest)) ns.push_back(EsXmlXpathNode(a, parent), alloc);
      break;

    case nodetest_type_node:
    case nodetest_all:
      ns.push_back(EsXmlXpathNode(a, parent), alloc);
      break;

    case nodetest_all_in_namespace:
      if (starts_with(name, _data.nodetest))
        ns.push_back(EsXmlXpathNode(a, parent), alloc);
      break;

    default:
      ;
    }
  }

  void step_push(xpath_node_set_raw& ns, const EsXmlNode& n, xpath_allocator* alloc)
  {
    if (!n) return;

    switch (_test)
    {
    case nodetest_name:
      if (n.typeGet() == xmlNodeElement && strequal(n.nameGet(), _data.nodetest)) ns.push_back(n, alloc);
      break;

    case nodetest_type_node:
      ns.push_back(n, alloc);
      break;

    case nodetest_type_comment:
      if (n.typeGet() == xmlNodeComment)
        ns.push_back(n, alloc);
      break;

    case nodetest_type_text:
      if (n.typeGet() == xmlNodePcdata || n.typeGet() == xmlNodeCdata)
        ns.push_back(n, alloc);
      break;

    case nodetest_type_pi:
      if (n.typeGet() == xmlNodeProcInstr)
        ns.push_back(n, alloc);
      break;

    case nodetest_pi:
      if (n.typeGet() == xmlNodeProcInstr && strequal(n.nameGet(), _data.nodetest))
        ns.push_back(n, alloc);
      break;

    case nodetest_all:
      if (n.typeGet() == xmlNodeElement)
        ns.push_back(n, alloc);
      break;

    case nodetest_all_in_namespace:
      if (n.typeGet() == xmlNodeElement && starts_with(n.nameGet(), _data.nodetest))
        ns.push_back(n, alloc);
      break;

    default:
      ES_ASSERT(!"Unknown axis");
    }
  }

  template <class T> void step_fill(xpath_node_set_raw& ns, const EsXmlNode& n, xpath_allocator* alloc, T)
  {
    const axis_t axis = T::axis;

    switch (axis)
    {
    case axis_attribute:
    {
      for (EsXmlAttribute a = n.firstAttributeGet(); a; a = a.nextAttributeGet())
        step_push(ns, a, n, alloc);

      break;
    }

    case axis_child:
    {
      for (EsXmlNode c = n.firstChildGet(); c; c = c.nextSiblingGet())
        step_push(ns, c, alloc);

      break;
    }

    case axis_descendant:
    case axis_descendant_or_self:
    {
      if (axis == axis_descendant_or_self)
        step_push(ns, n, alloc);

      EsXmlNode cur = n.firstChildGet();

      while (cur && cur != n)
      {
        step_push(ns, cur, alloc);

        if (cur.firstChildGet())
          cur = cur.firstChildGet();
        else if (cur.nextSiblingGet())
          cur = cur.nextSiblingGet();
        else
        {
          while (!cur.nextSiblingGet() && cur != n)
            cur = cur.parentGet();

          if (cur != n) cur = cur.nextSiblingGet();
        }
      }

      break;
    }

    case axis_following_sibling:
    {
      for (EsXmlNode c = n.nextSiblingGet(); c; c = c.nextSiblingGet())
        step_push(ns, c, alloc);

      break;
    }

    case axis_preceding_sibling:
    {
      for (EsXmlNode c = n.previousSiblingGet(); c; c = c.previousSiblingGet())
        step_push(ns, c, alloc);

      break;
    }

    case axis_following:
    {
      EsXmlNode cur = n;

      // exit from this node so that we don't include descendants
      while (cur && !cur.nextSiblingGet()) cur = cur.parentGet();
      cur = cur.nextSiblingGet();

      for (;;)
      {
        step_push(ns, cur, alloc);

        if (cur.firstChildGet())
          cur = cur.firstChildGet();
        else if (cur.nextSiblingGet())
          cur = cur.nextSiblingGet();
        else
        {
          while (cur && !cur.nextSiblingGet()) cur = cur.parentGet();
          cur = cur.nextSiblingGet();

          if (!cur) break;
        }
      }

      break;
    }

    case axis_preceding:
    {
      EsXmlNode cur = n;

      while (cur && !cur.previousSiblingGet()) cur = cur.parentGet();
      cur = cur.previousSiblingGet();

      for (;;)
      {
        if (cur.lastChildGet())
          cur = cur.lastChildGet();
        else
        {
          // leaf node, can't be ancestor
          step_push(ns, cur, alloc);

          if (cur.previousSiblingGet())
            cur = cur.previousSiblingGet();
          else
          {
            do
            {
              cur = cur.parentGet();
              if (!cur) break;

              if (!node_is_ancestor(cur, n)) step_push(ns, cur, alloc);
            }
            while (!cur.previousSiblingGet());

            cur = cur.previousSiblingGet();

            if (!cur) break;
          }
        }
      }

      break;
    }

    case axis_ancestor:
    case axis_ancestor_or_self:
    {
      if (axis == axis_ancestor_or_self)
        step_push(ns, n, alloc);

      EsXmlNode cur = n.parentGet();

      while (cur)
      {
        step_push(ns, cur, alloc);

        cur = cur.parentGet();
      }

      break;
    }

    case axis_self:
    {
      step_push(ns, n, alloc);

      break;
    }

    case axis_parent:
    {
      if (n.parentGet()) step_push(ns, n.parentGet(), alloc);

      break;
    }

    default:
      ES_ASSERT(!"Unimplemented axis");
    }
  }

  template <class T> void step_fill(xpath_node_set_raw& ns, const EsXmlAttribute& a, const EsXmlNode& p, xpath_allocator* alloc, T v)
  {
    const axis_t axis = T::axis;

    switch (axis)
    {
    case axis_ancestor:
    case axis_ancestor_or_self:
    {
      if (axis == axis_ancestor_or_self && _test == nodetest_type_node) // reject attributes based on principal node type test
        step_push(ns, a, p, alloc);

      EsXmlNode cur = p;

      while (cur)
      {
        step_push(ns, cur, alloc);

        cur = cur.parentGet();
      }

      break;
    }

    case axis_descendant_or_self:
    case axis_self:
    {
      if (_test == nodetest_type_node) // reject attributes based on principal node type test
        step_push(ns, a, p, alloc);

      break;
    }

    case axis_following:
    {
      EsXmlNode cur = p;

      for (;;)
      {
        if (cur.firstChildGet())
          cur = cur.firstChildGet();
        else if (cur.nextSiblingGet())
          cur = cur.nextSiblingGet();
        else
        {
          while (cur && !cur.nextSiblingGet()) cur = cur.parentGet();
          cur = cur.nextSiblingGet();

          if (!cur) break;
        }

        step_push(ns, cur, alloc);
      }

      break;
    }

    case axis_parent:
    {
      step_push(ns, p, alloc);

      break;
    }

    case axis_preceding:
    {
      // preceding:: axis does not include attribute nodes and attribute ancestors (they are the same as parent's ancestors), so we can reuse node preceding
      step_fill(ns, p, alloc, v);
      break;
    }

    default:
      ES_ASSERT(!"Unimplemented axis");
    }
  }

  template <class T> xpath_node_set_raw step_do(const xpath_context& c, const xpath_stack& stack, T v)
  {
    const axis_t axis = T::axis;
    bool attributes = (axis == axis_ancestor || axis == axis_ancestor_or_self || axis == axis_descendant_or_self || axis == axis_following || axis == axis_parent || axis == axis_preceding || axis == axis_self);

    xpath_node_set_raw ns;
    ns.set_type((axis == axis_ancestor || axis == axis_ancestor_or_self || axis == axis_preceding || axis == axis_preceding_sibling) ? EsXmlXpathNodeSet::type_sorted_reverse : EsXmlXpathNodeSet::type_sorted);

    if (_left)
    {
      xpath_node_set_raw s = _left->eval_node_set(c, stack);

      // self axis preserves the original order
      if (axis == axis_self) ns.set_type(s.typeGet());

      for (const EsXmlXpathNode* it = s.begin(); it != s.end(); ++it)
      {
        size_t size = ns.size();

        // in general, all axes generate elements in a particular order, but there is no order guarantee if axis is applied to two nodes
        if (axis != axis_self && size != 0) ns.set_type(EsXmlXpathNodeSet::type_unsorted);

        if (it->nodeGet())
          step_fill(ns, it->nodeGet(), stack.result, v);
        else if (attributes)
          step_fill(ns, it->attributeGet(), it->parentGet(), stack.result, v);

        apply_predicates(ns, size, stack);
      }
    }
    else
    {
      if (c.n.nodeGet())
        step_fill(ns, c.n.nodeGet(), stack.result, v);
      else if (attributes)
        step_fill(ns, c.n.attributeGet(), c.n.parentGet(), stack.result, v);

      apply_predicates(ns, 0, stack);
    }

    // child, attribute and self axes always generate unique set of nodes
    // for other axis, if the set stayed sorted, it stayed unique because the traversal algorithms do not visit the same node twice
    if (axis != axis_child && axis != axis_attribute && axis != axis_self && ns.typeGet() == EsXmlXpathNodeSet::type_unsorted)
      ns.remove_duplicates();

    return ns;
  }

public:
  xpath_ast_node(ast_type_t type, EsXmlXpathValueType rettype_, EsString::const_pointer value):
    _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(0), _right(0), _next(0)
  {
    ES_ASSERT(type == ast_string_constant);
    _data.string = value;
  }

  xpath_ast_node(ast_type_t type, EsXmlXpathValueType rettype_, double value):
    _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(0), _right(0), _next(0)
  {
    ES_ASSERT(type == ast_number_constant);
    _data.number = value;
  }

  xpath_ast_node(ast_type_t type, EsXmlXpathValueType rettype_, EsXmlXpathVariable* value):
    _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(0), _right(0), _next(0)
  {
    ES_ASSERT(type == ast_variable);
    _data.variable = value;
  }

  xpath_ast_node(ast_type_t type, EsXmlXpathValueType rettype_, xpath_ast_node* left = 0, xpath_ast_node* right = 0):
    _type(static_cast<char>(type)), _rettype(static_cast<char>(rettype_)), _axis(0), _test(0), _left(left), _right(right), _next(0)
  {
  }

  xpath_ast_node(ast_type_t type, xpath_ast_node* left, axis_t axis, nodetest_t test, EsString::const_pointer contents):
    _type(static_cast<char>(type)), _rettype(xpathTypeNodeSet), _axis(static_cast<char>(axis)), _test(static_cast<char>(test)), _left(left), _right(0), _next(0)
  {
    _data.nodetest = contents;
  }

  void set_next(xpath_ast_node* value)
  {
    _next = value;
  }

  void set_right(xpath_ast_node* value)
  {
    _right = value;
  }

  bool eval_boolean(const xpath_context& c, const xpath_stack& stack)
  {
    switch (_type)
    {
    case ast_op_or:
      return _left->eval_boolean(c, stack) || _right->eval_boolean(c, stack);

    case ast_op_and:
      return _left->eval_boolean(c, stack) && _right->eval_boolean(c, stack);

    case ast_op_equal:
      return compare_eq(_left, _right, c, stack, EsXml::equal_to());

    case ast_op_not_equal:
      return compare_eq(_left, _right, c, stack, EsXml::not_equal_to());

    case ast_op_less:
      return compare_rel(_left, _right, c, stack, EsXml::less());

    case ast_op_greater:
      return compare_rel(_right, _left, c, stack, EsXml::less());

    case ast_op_less_or_equal:
      return compare_rel(_left, _right, c, stack, EsXml::less_equal());

    case ast_op_greater_or_equal:
      return compare_rel(_right, _left, c, stack, EsXml::less_equal());

    case ast_func_starts_with:
    {
      xpath_allocator_capture cr(stack.result);

      EsXmlXpathString lr = _left->eval_string(c, stack);
      EsXmlXpathString rr = _right->eval_string(c, stack);

      return starts_with(lr.c_str(), rr.c_str());
    }

    case ast_func_contains:
    {
      xpath_allocator_capture cr(stack.result);

      EsXmlXpathString lr = _left->eval_string(c, stack);
      EsXmlXpathString rr = _right->eval_string(c, stack);

      return find_substring(lr.c_str(), rr.c_str()) != 0;
    }

    case ast_func_boolean:
      return _left->eval_boolean(c, stack);

    case ast_func_not:
      return !_left->eval_boolean(c, stack);

    case ast_func_true:
      return true;

    case ast_func_false:
      return false;

    case ast_func_lang:
    {
      if (c.n.attributeGet()) return false;

      xpath_allocator_capture cr(stack.result);

      EsXmlXpathString lang = _left->eval_string(c, stack);

      for (EsXmlNode n = c.n.nodeGet(); n; n = n.parentGet())
      {
        EsXmlAttribute a = n.attributeGet(esT("xml:lang"));

        if (a)
        {
          EsString::const_pointer value = a.valueGet();

          // strnicmp / strncasecmp is not portable
          for (EsString::const_pointer lit = lang.c_str(); *lit; ++lit)
          {
            if (tolower_ascii(*lit) != tolower_ascii(*value)) return false;
            ++value;
          }

          return *value == 0 || *value == '-';
        }
      }

      return false;
    }

    case ast_variable:
    {
      ES_ASSERT(_rettype == _data.variable->typeGet());

      if (_rettype == xpathTypeBoolean)
        return _data.variable->booleanGet();

      // fallthrough to type conversion
    }

    default:
    {
      switch (_rettype)
      {
      case xpathTypeNumber:
        return convert_number_to_boolean(eval_number(c, stack));

      case xpathTypeString:
      {
        xpath_allocator_capture cr(stack.result);

        return !eval_string(c, stack).empty();
      }

      case xpathTypeNodeSet:
      {
        xpath_allocator_capture cr(stack.result);

        return !eval_node_set(c, stack).empty();
      }

      default:
        ES_ASSERT(!"Wrong expression for return type boolean");
        return false;
      }
    }
    }
  }

  double eval_number(const xpath_context& c, const xpath_stack& stack)
  {
    switch (_type)
    {
    case ast_op_add:
      return _left->eval_number(c, stack) + _right->eval_number(c, stack);

    case ast_op_subtract:
      return _left->eval_number(c, stack) - _right->eval_number(c, stack);

    case ast_op_multiply:
      return _left->eval_number(c, stack) * _right->eval_number(c, stack);

    case ast_op_divide:
      return _left->eval_number(c, stack) / _right->eval_number(c, stack);

    case ast_op_mod:
      return fmod(_left->eval_number(c, stack), _right->eval_number(c, stack));

    case ast_op_negate:
      return -_left->eval_number(c, stack);

    case ast_number_constant:
      return _data.number;

    case ast_func_last:
      return static_cast<double>(c.size);

    case ast_func_position:
      return static_cast<double>(c.position);

    case ast_func_count:
    {
      xpath_allocator_capture cr(stack.result);

      return static_cast<double>(_left->eval_node_set(c, stack).size());
    }

    case ast_func_string_length_0:
    {
      xpath_allocator_capture cr(stack.result);

      return static_cast<double>(string_value(c.n, stack.result).length());
    }

    case ast_func_string_length_1:
    {
      xpath_allocator_capture cr(stack.result);

      return static_cast<double>(_left->eval_string(c, stack).length());
    }

    case ast_func_number_0:
    {
      xpath_allocator_capture cr(stack.result);

      return convert_string_to_number(string_value(c.n, stack.result).c_str());
    }

    case ast_func_number_1:
      return _left->eval_number(c, stack);

    case ast_func_sum:
    {
      xpath_allocator_capture cr(stack.result);

      double r = 0;

      xpath_node_set_raw ns = _left->eval_node_set(c, stack);

      for (const EsXmlXpathNode* it = ns.begin(); it != ns.end(); ++it)
      {
        xpath_allocator_capture cri(stack.result);

        r += convert_string_to_number(string_value(*it, stack.result).c_str());
      }

      return r;
    }

    case ast_func_floor:
    {
      double r = _left->eval_number(c, stack);

      return r == r ? floor(r) : r;
    }

    case ast_func_ceiling:
    {
      double r = _left->eval_number(c, stack);

      return r == r ? ceil(r) : r;
    }

    case ast_func_round:
      return round_nearest_nzero(_left->eval_number(c, stack));

    case ast_variable:
    {
      ES_ASSERT(_rettype == _data.variable->typeGet());

      if (_rettype == xpathTypeNumber)
        return _data.variable->numberGet();

      // fallthrough to type conversion
    }

    default:
    {
      switch (_rettype)
      {
      case xpathTypeBoolean:
        return eval_boolean(c, stack) ? 1 : 0;

      case xpathTypeString:
      {
        xpath_allocator_capture cr(stack.result);

        return convert_string_to_number(eval_string(c, stack).c_str());
      }

      case xpathTypeNodeSet:
      {
        xpath_allocator_capture cr(stack.result);

        return convert_string_to_number(eval_string(c, stack).c_str());
      }

      default:
        ES_ASSERT(!"Wrong expression for return type number");
        return 0;
      }

    }
    }
  }

  EsXmlXpathString eval_string_concat(const xpath_context& c, const xpath_stack& stack)
  {
    ES_ASSERT(_type == ast_func_concat);

    xpath_allocator_capture ct(stack.temp);

    // count the string number
    size_t count = 1;
    for (xpath_ast_node* nc = _right; nc; nc = nc->_next) count++;

    // gather all strings
    EsXmlXpathString static_buffer[4];
    EsXmlXpathString* buffer = static_buffer;

    // allocate on-heap for large concats
    if (count > sizeof(static_buffer) / sizeof(static_buffer[0]))
    {
      buffer = static_cast<EsXmlXpathString*>(stack.temp->allocate(count * sizeof(EsXmlXpathString)));
      ES_ASSERT(buffer);
    }

    // evaluate all strings to temporary stack
    xpath_stack swapped_stack = {stack.temp, stack.result};

    buffer[0] = _left->eval_string(c, swapped_stack);

    size_t pos = 1;
    for (xpath_ast_node* n = _right; n; n = n->_next, ++pos) buffer[pos] = n->eval_string(c, swapped_stack);
    ES_ASSERT(pos == count);

    // get total length
    size_t length = 0;
    for (size_t i = 0; i < count; ++i) length += buffer[i].length();

    // create final string
    EsString::pointer result = static_cast<EsString::pointer>(stack.result->allocate((length + 1) * sizeof(EsString::value_type)));
    ES_ASSERT(result);

    EsString::pointer ri = result;

    for (size_t j = 0; j < count; ++j)
      for (EsString::const_pointer bi = buffer[j].c_str(); *bi; ++bi)
        *ri++ = *bi;

    *ri = 0;

    return EsXmlXpathString(result, true);
  }

  EsXmlXpathString eval_string(const xpath_context& c, const xpath_stack& stack)
  {
    switch (_type)
    {
    case ast_string_constant:
      return xpath_string_const(_data.string);

    case ast_func_local_name_0:
    {
      EsXmlXpathNode na = c.n;

      return xpath_string_const(local_name(na));
    }

    case ast_func_local_name_1:
    {
      xpath_allocator_capture cr(stack.result);

      xpath_node_set_raw ns = _left->eval_node_set(c, stack);
      EsXmlXpathNode na = ns.first();

      return xpath_string_const(local_name(na));
    }

    case ast_func_name_0:
    {
      EsXmlXpathNode na = c.n;

      return xpath_string_const(qualified_name(na));
    }

    case ast_func_name_1:
    {
      xpath_allocator_capture cr(stack.result);

      xpath_node_set_raw ns = _left->eval_node_set(c, stack);
      EsXmlXpathNode na = ns.first();

      return xpath_string_const(qualified_name(na));
    }

    case ast_func_namespace_uri_0:
    {
      EsXmlXpathNode na = c.n;

      return xpath_string_const(namespace_uri(na));
    }

    case ast_func_namespace_uri_1:
    {
      xpath_allocator_capture cr(stack.result);

      xpath_node_set_raw ns = _left->eval_node_set(c, stack);
      EsXmlXpathNode na = ns.first();

      return xpath_string_const(namespace_uri(na));
    }

    case ast_func_string_0:
      return string_value(c.n, stack.result);

    case ast_func_string_1:
      return _left->eval_string(c, stack);

    case ast_func_concat:
      return eval_string_concat(c, stack);

    case ast_func_substring_before:
    {
      xpath_allocator_capture cr(stack.temp);

      xpath_stack swapped_stack = {stack.temp, stack.result};

      EsXmlXpathString s = _left->eval_string(c, swapped_stack);
      EsXmlXpathString p = _right->eval_string(c, swapped_stack);

      EsString::const_pointer pos = find_substring(s.c_str(), p.c_str());

      return pos ? EsXmlXpathString(s.c_str(), pos, stack.result) : EsXmlXpathString();
    }

    case ast_func_substring_after:
    {
      xpath_allocator_capture cr(stack.temp);

      xpath_stack swapped_stack = {stack.temp, stack.result};

      EsXmlXpathString s = _left->eval_string(c, swapped_stack);
      EsXmlXpathString p = _right->eval_string(c, swapped_stack);

      EsString::const_pointer pos = find_substring(s.c_str(), p.c_str());
      if (!pos) return EsXmlXpathString();

      EsString::const_pointer result = pos + p.length();

      return s.uses_heap() ? EsXmlXpathString(result, stack.result) : xpath_string_const(result);
    }

    case ast_func_substring_2:
    {
      xpath_allocator_capture cr(stack.temp);

      xpath_stack swapped_stack = {stack.temp, stack.result};

      EsXmlXpathString s = _left->eval_string(c, swapped_stack);
      size_t s_length = s.length();

      double first = round_nearest(_right->eval_number(c, stack));

      if (esIsNan(first)) return EsXmlXpathString(); // NaN
      else if (first >= s_length + 1) return EsXmlXpathString();

      size_t pos = first < 1 ? 1 : static_cast<size_t>(first);
      ES_ASSERT(1 <= pos && pos <= s_length + 1);

      EsString::const_pointer rbegin = s.c_str() + (pos - 1);

      return s.uses_heap() ? EsXmlXpathString(rbegin, stack.result) : xpath_string_const(rbegin);
    }

    case ast_func_substring_3:
    {
      xpath_allocator_capture cr(stack.temp);

      xpath_stack swapped_stack = {stack.temp, stack.result};

      EsXmlXpathString s = _left->eval_string(c, swapped_stack);
      size_t s_length = s.length();

      double first = round_nearest(_right->eval_number(c, stack));
      double last = first + round_nearest(_right->_next->eval_number(c, stack));

      if (esIsNan(first) || esIsNan(last)) return EsXmlXpathString();
      else if (first >= s_length + 1) return EsXmlXpathString();
      else if (first >= last) return EsXmlXpathString();
      else if (last < 1) return EsXmlXpathString();

      size_t pos = first < 1 ? 1 : static_cast<size_t>(first);
      size_t end = last >= s_length + 1 ? s_length + 1 : static_cast<size_t>(last);

      ES_ASSERT(1 <= pos && pos <= end && end <= s_length + 1);
      EsString::const_pointer rbegin = s.c_str() + (pos - 1);
      EsString::const_pointer rend = s.c_str() + (end - 1);

      return (end == s_length + 1 && !s.uses_heap()) ? xpath_string_const(rbegin) : EsXmlXpathString(rbegin, rend, stack.result);
    }

    case ast_func_normalize_space_0:
    {
      EsXmlXpathString s = string_value(c.n, stack.result);

      normalize_space(s.data(stack.result));

      return s;
    }

    case ast_func_normalize_space_1:
    {
      EsXmlXpathString s = _left->eval_string(c, stack);

      normalize_space(s.data(stack.result));

      return s;
    }

    case ast_func_translate:
    {
      xpath_allocator_capture cr(stack.temp);

      xpath_stack swapped_stack = {stack.temp, stack.result};

      EsXmlXpathString s = _left->eval_string(c, stack);
      EsXmlXpathString from = _right->eval_string(c, swapped_stack);
      EsXmlXpathString to = _right->_next->eval_string(c, swapped_stack);

      translate(s.data(stack.result), from.c_str(), to.c_str());

      return s;
    }

    case ast_variable:
    {
      ES_ASSERT(_rettype == _data.variable->typeGet());

      if (_rettype == xpathTypeString)
        return xpath_string_const(_data.variable->stringGet());

      // fallthrough to type conversion
    }

    default:
    {
      switch (_rettype)
      {
      case xpathTypeBoolean:
        return xpath_string_const(eval_boolean(c, stack) ? esT("true") : esT("false"));

      case xpathTypeNumber:
        return convert_number_to_string(eval_number(c, stack), stack.result);

      case xpathTypeNodeSet:
      {
        xpath_allocator_capture cr(stack.temp);

        xpath_stack swapped_stack = {stack.temp, stack.result};

        xpath_node_set_raw ns = eval_node_set(c, swapped_stack);
        return ns.empty() ? EsXmlXpathString() : string_value(ns.first(), stack.result);
      }

      default:
        ES_ASSERT(!"Wrong expression for return type string");
        return EsXmlXpathString();
      }
    }
    }
  }

  xpath_node_set_raw eval_node_set(const xpath_context& c, const xpath_stack& stack)
  {
    switch (_type)
    {
    case ast_op_union:
    {
      xpath_allocator_capture cr(stack.temp);

      xpath_stack swapped_stack = {stack.temp, stack.result};

      xpath_node_set_raw ls = _left->eval_node_set(c, swapped_stack);
      xpath_node_set_raw rs = _right->eval_node_set(c, stack);

      // we can optimize merging two sorted sets, but this is a very rare operation, so don't bother
      rs.set_type(EsXmlXpathNodeSet::type_unsorted);

      rs.append(ls.begin(), ls.end(), stack.result);
      rs.remove_duplicates();

      return rs;
    }

    case ast_filter:
    case ast_filter_posinv:
    {
      xpath_node_set_raw set = _left->eval_node_set(c, stack);

      // either expression is a number or it contains position() call; sort by document order
      if (_type == ast_filter) set.sort_do();

      apply_predicate(set, 0, _right, stack);

      return set;
    }

    case ast_func_id:
      return xpath_node_set_raw();

    case ast_step:
    {
      switch (_axis)
      {
      case axis_ancestor:
        return step_do(c, stack, axis_to_type<axis_ancestor>());

      case axis_ancestor_or_self:
        return step_do(c, stack, axis_to_type<axis_ancestor_or_self>());

      case axis_attribute:
        return step_do(c, stack, axis_to_type<axis_attribute>());

      case axis_child:
        return step_do(c, stack, axis_to_type<axis_child>());

      case axis_descendant:
        return step_do(c, stack, axis_to_type<axis_descendant>());

      case axis_descendant_or_self:
        return step_do(c, stack, axis_to_type<axis_descendant_or_self>());

      case axis_following:
        return step_do(c, stack, axis_to_type<axis_following>());

      case axis_following_sibling:
        return step_do(c, stack, axis_to_type<axis_following_sibling>());

      case axis_namespace:
        // namespaced axis is not supported
        return xpath_node_set_raw();

      case axis_parent:
        return step_do(c, stack, axis_to_type<axis_parent>());

      case axis_preceding:
        return step_do(c, stack, axis_to_type<axis_preceding>());

      case axis_preceding_sibling:
        return step_do(c, stack, axis_to_type<axis_preceding_sibling>());

      case axis_self:
        return step_do(c, stack, axis_to_type<axis_self>());

      default:
        ES_ASSERT(!"Unknown axis");
        return xpath_node_set_raw();
      }
    }

    case ast_step_root:
    {
      ES_ASSERT(!_right); // root step can't have any predicates

      xpath_node_set_raw ns;

      ns.set_type(EsXmlXpathNodeSet::type_sorted);

      if (c.n.nodeGet()) ns.push_back(c.n.nodeGet().rootGet(), stack.result);
      else if (c.n.attributeGet()) ns.push_back(c.n.parentGet().rootGet(), stack.result);

      return ns;
    }

    case ast_variable:
    {
      ES_ASSERT(_rettype == _data.variable->typeGet());

      if (_rettype == xpathTypeNodeSet)
      {
        const EsXmlXpathNodeSet& s = _data.variable->nodesetGet();

        xpath_node_set_raw ns;

        ns.set_type(s.typeGet());
        ns.append(s.begin(), s.end(), stack.result);

        return ns;
      }

      // fallthrough to type conversion
    }

    default:
      ES_ASSERT(!"Wrong expression for return type node set");
      return xpath_node_set_raw();
    }
  }

  bool is_posinv()
  {
    switch (_type)
    {
    case ast_func_position:
      return false;

    case ast_string_constant:
    case ast_number_constant:
    case ast_variable:
      return true;

    case ast_step:
    case ast_step_root:
      return true;

    case ast_predicate:
    case ast_filter:
    case ast_filter_posinv:
      return true;

    default:
      if (_left && !_left->is_posinv()) return false;

      for (xpath_ast_node* n = _right; n; n = n->_next)
        if (!n->is_posinv()) return false;

      return true;
    }
  }

  EsXmlXpathValueType rettype() const
  {
    return static_cast<EsXmlXpathValueType>(_rettype);
  }
};

struct xpath_parser
{
  xpath_allocator* _alloc;
  xpath_lexer _lexer;

  EsString::const_pointer _query;
  EsXmlXpathVariableSet* _variables;

  EsXmlXpathParseResult* _result;

  EsString::value_type _scratch[32];

  void throw_error(EsString::const_pointer message)
  {
    _result->error = message;
    _result->offset = _lexer.current_pos() - _query;

    EsXmlXpathException::Throw(*_result);
  }

  void throw_error_oom()
  {
    throw std::bad_alloc();
  }

  void* alloc_node()
  {
    void* result = _alloc->allocate_nothrow(sizeof(xpath_ast_node));

    if (!result) throw_error_oom();

    return result;
  }

  EsString::const_pointer alloc_string(const xpath_lexer_string& value)
  {
    if (value.begin)
    {
      size_t length = static_cast<size_t>(value.end - value.begin);

      EsString::pointer c = static_cast<EsString::pointer>(_alloc->allocate_nothrow((length + 1) * sizeof(EsString::value_type)));
      if (!c) throw_error_oom();
      ES_ASSERT(c); // workaround for clang static analysis

      memcpy(c, value.begin, length * sizeof(EsString::value_type));
      c[length] = 0;

      return c;
    }
    else return 0;
  }

  xpath_ast_node* parse_function_helper(ast_type_t type0, ast_type_t type1, size_t argc, xpath_ast_node* args[2])
  {
    ES_ASSERT(argc <= 1);

    if (argc == 1 && args[0]->rettype() != xpathTypeNodeSet) throw_error(esT("Function has to be applied to node set"));

    return new (alloc_node()) xpath_ast_node(argc == 0 ? type0 : type1, xpathTypeString, args[0]);
  }

  xpath_ast_node* parse_function(const xpath_lexer_string& name, size_t argc, xpath_ast_node* args[2])
  {
    switch (name.begin[0])
    {
    case 'b':
      if (name == esT("boolean") && argc == 1)
        return new (alloc_node()) xpath_ast_node(ast_func_boolean, xpathTypeBoolean, args[0]);

      break;

    case 'c':
      if (name == esT("count") && argc == 1)
      {
        if (args[0]->rettype() != xpathTypeNodeSet) throw_error(esT("Function has to be applied to node set"));
        return new (alloc_node()) xpath_ast_node(ast_func_count, xpathTypeNumber, args[0]);
      }
      else if (name == esT("contains") && argc == 2)
        return new (alloc_node()) xpath_ast_node(ast_func_contains, xpathTypeBoolean, args[0], args[1]);
      else if (name == esT("concat") && argc >= 2)
        return new (alloc_node()) xpath_ast_node(ast_func_concat, xpathTypeString, args[0], args[1]);
      else if (name == esT("ceiling") && argc == 1)
        return new (alloc_node()) xpath_ast_node(ast_func_ceiling, xpathTypeNumber, args[0]);

      break;

    case 'f':
      if (name == esT("false") && argc == 0)
        return new (alloc_node()) xpath_ast_node(ast_func_false, xpathTypeBoolean);
      else if (name == esT("floor") && argc == 1)
        return new (alloc_node()) xpath_ast_node(ast_func_floor, xpathTypeNumber, args[0]);

      break;

    case 'i':
      if (name == esT("id") && argc == 1)
        return new (alloc_node()) xpath_ast_node(ast_func_id, xpathTypeNodeSet, args[0]);

      break;

    case 'l':
      if (name == esT("last") && argc == 0)
        return new (alloc_node()) xpath_ast_node(ast_func_last, xpathTypeNumber);
      else if (name == esT("lang") && argc == 1)
        return new (alloc_node()) xpath_ast_node(ast_func_lang, xpathTypeBoolean, args[0]);
      else if (name == esT("local-name") && argc <= 1)
        return parse_function_helper(ast_func_local_name_0, ast_func_local_name_1, argc, args);

      break;

    case 'n':
      if (name == esT("name") && argc <= 1)
        return parse_function_helper(ast_func_name_0, ast_func_name_1, argc, args);
      else if (name == esT("namespace-uri") && argc <= 1)
        return parse_function_helper(ast_func_namespace_uri_0, ast_func_namespace_uri_1, argc, args);
      else if (name == esT("normalize-space") && argc <= 1)
        return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_normalize_space_0 : ast_func_normalize_space_1, xpathTypeString, args[0], args[1]);
      else if (name == esT("not") && argc == 1)
        return new (alloc_node()) xpath_ast_node(ast_func_not, xpathTypeBoolean, args[0]);
      else if (name == esT("number") && argc <= 1)
        return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_number_0 : ast_func_number_1, xpathTypeNumber, args[0]);

      break;

    case 'p':
      if (name == esT("position") && argc == 0)
        return new (alloc_node()) xpath_ast_node(ast_func_position, xpathTypeNumber);

      break;

    case 'r':
      if (name == esT("round") && argc == 1)
        return new (alloc_node()) xpath_ast_node(ast_func_round, xpathTypeNumber, args[0]);

      break;

    case 's':
      if (name == esT("string") && argc <= 1)
        return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_string_0 : ast_func_string_1, xpathTypeString, args[0]);
      else if (name == esT("string-length") && argc <= 1)
        return new (alloc_node()) xpath_ast_node(argc == 0 ? ast_func_string_length_0 : ast_func_string_length_1, xpathTypeNumber, args[0]);
      else if (name == esT("starts-with") && argc == 2)
        return new (alloc_node()) xpath_ast_node(ast_func_starts_with, xpathTypeBoolean, args[0], args[1]);
      else if (name == esT("substring-before") && argc == 2)
        return new (alloc_node()) xpath_ast_node(ast_func_substring_before, xpathTypeString, args[0], args[1]);
      else if (name == esT("substring-after") && argc == 2)
        return new (alloc_node()) xpath_ast_node(ast_func_substring_after, xpathTypeString, args[0], args[1]);
      else if (name == esT("substring") && (argc == 2 || argc == 3))
        return new (alloc_node()) xpath_ast_node(argc == 2 ? ast_func_substring_2 : ast_func_substring_3, xpathTypeString, args[0], args[1]);
      else if (name == esT("sum") && argc == 1)
      {
        if (args[0]->rettype() != xpathTypeNodeSet) throw_error(esT("Function has to be applied to node set"));
        return new (alloc_node()) xpath_ast_node(ast_func_sum, xpathTypeNumber, args[0]);
      }

      break;

    case 't':
      if (name == esT("translate") && argc == 3)
        return new (alloc_node()) xpath_ast_node(ast_func_translate, xpathTypeString, args[0], args[1]);
      else if (name == esT("true") && argc == 0)
        return new (alloc_node()) xpath_ast_node(ast_func_true, xpathTypeBoolean);

      break;

    default:
      break;
    }

    throw_error(esT("Unrecognized function or wrong parameter count"));

    return 0;
  }

  axis_t parse_axis_name(const xpath_lexer_string& name, bool& specified)
  {
    specified = true;

    switch (name.begin[0])
    {
    case 'a':
      if (name == esT("ancestor"))
        return axis_ancestor;
      else if (name == esT("ancestor-or-self"))
        return axis_ancestor_or_self;
      else if (name == esT("attribute"))
        return axis_attribute;

      break;

    case 'c':
      if (name == esT("child"))
        return axis_child;

      break;

    case 'd':
      if (name == esT("descendant"))
        return axis_descendant;
      else if (name == esT("descendant-or-self"))
        return axis_descendant_or_self;

      break;

    case 'f':
      if (name == esT("following"))
        return axis_following;
      else if (name == esT("following-sibling"))
        return axis_following_sibling;

      break;

    case 'n':
      if (name == esT("namespace"))
        return axis_namespace;

      break;

    case 'p':
      if (name == esT("parent"))
        return axis_parent;
      else if (name == esT("preceding"))
        return axis_preceding;
      else if (name == esT("preceding-sibling"))
        return axis_preceding_sibling;

      break;

    case 's':
      if (name == esT("self"))
        return axis_self;

      break;

    default:
      break;
    }

    specified = false;
    return axis_child;
  }

  nodetest_t parse_node_test_type(const xpath_lexer_string& name)
  {
    switch (name.begin[0])
    {
    case 'c':
      if (name == esT("comment"))
        return nodetest_type_comment;

      break;

    case 'n':
      if (name == esT("node"))
        return nodetest_type_node;

      break;

    case 'p':
      if (name == esT("processing-instruction"))
        return nodetest_type_pi;

      break;

    case 't':
      if (name == esT("text"))
        return nodetest_type_text;

      break;

    default:
      break;
    }

    return nodetest_none;
  }

  // PrimaryExpr ::= VariableReference | '(' Expr ')' | Literal | Number | FunctionCall
  xpath_ast_node* parse_primary_expression()
  {
    switch (_lexer.current())
    {
    case lex_var_ref:
    {
      xpath_lexer_string name = _lexer.contents();

      if (!_variables)
        throw_error(esT("Unknown variable: variable set is not provided"));

      EsXmlXpathVariable* var = get_variable_scratch(_scratch, _variables, name.begin, name.end);

      if (!var)
        throw_error(esT("Unknown variable: variable set does not contain the given name"));

      _lexer.next();

      return new (alloc_node()) xpath_ast_node(ast_variable, var->typeGet(), var);
    }

    case lex_open_brace:
    {
      _lexer.next();

      xpath_ast_node* n = parse_expression();

      if (_lexer.current() != lex_close_brace)
        throw_error(esT("Unmatched braces"));

      _lexer.next();

      return n;
    }

    case lex_quoted_string:
    {
      EsString::const_pointer value = alloc_string(_lexer.contents());

      xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_string_constant, xpathTypeString, value);
      _lexer.next();

      return n;
    }

    case lex_number:
    {
      double value = 0;

      if (!convert_string_to_number_scratch(_scratch, _lexer.contents().begin, _lexer.contents().end, &value))
        throw_error_oom();

      xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_number_constant, xpathTypeNumber, value);
      _lexer.next();

      return n;
    }

    case lex_string:
    {
      xpath_ast_node* args[2] = {0};
      size_t argc = 0;

      xpath_lexer_string function = _lexer.contents();
      _lexer.next();

      xpath_ast_node* last_arg = 0;

      if (_lexer.current() != lex_open_brace)
        throw_error(esT("Unrecognized function call"));
      _lexer.next();

      if (_lexer.current() != lex_close_brace)
        args[argc++] = parse_expression();

      while (_lexer.current() != lex_close_brace)
      {
        if (_lexer.current() != lex_comma)
          throw_error(esT("No comma between function arguments"));
        _lexer.next();

        xpath_ast_node* n = parse_expression();

        if (argc < 2) args[argc] = n;
        else last_arg->set_next(n);

        argc++;
        last_arg = n;
      }

      _lexer.next();

      return parse_function(function, argc, args);
    }

    default:
      throw_error(esT("Unrecognizable primary expression"));

      return 0;
    }
  }

  // FilterExpr ::= PrimaryExpr | FilterExpr Predicate
  // Predicate ::= '[' PredicateExpr ']'
  // PredicateExpr ::= Expr
  xpath_ast_node* parse_filter_expression()
  {
    xpath_ast_node* n = parse_primary_expression();

    while (_lexer.current() == lex_open_square_brace)
    {
      _lexer.next();

      xpath_ast_node* expr = parse_expression();

      if (n->rettype() != xpathTypeNodeSet) throw_error(esT("Predicate has to be applied to node set"));

      bool posinv = expr->rettype() != xpathTypeNumber && expr->is_posinv();

      n = new (alloc_node()) xpath_ast_node(posinv ? ast_filter_posinv : ast_filter, xpathTypeNodeSet, n, expr);

      if (_lexer.current() != lex_close_square_brace)
        throw_error(esT("Unmatched square brace"));

      _lexer.next();
    }

    return n;
  }

  // Step ::= AxisSpecifier NodeTest Predicate* | AbbreviatedStep
  // AxisSpecifier ::= AxisName '::' | '@'?
  // NodeTest ::= NameTest | NodeType '(' ')' | 'processing-instruction' '(' Literal ')'
  // NameTest ::= '*' | NCName ':' '*' | QName
  // AbbreviatedStep ::= '.' | '..'
  xpath_ast_node* parse_step(xpath_ast_node* set)
  {
    if (set && set->rettype() != xpathTypeNodeSet)
      throw_error(esT("Step has to be applied to node set"));

    bool axis_specified = false;
    axis_t axis = axis_child; // implied child axis

    if (_lexer.current() == lex_axis_attribute)
    {
      axis = axis_attribute;
      axis_specified = true;

      _lexer.next();
    }
    else if (_lexer.current() == lex_dot)
    {
      _lexer.next();

      return new (alloc_node()) xpath_ast_node(ast_step, set, axis_self, nodetest_type_node, 0);
    }
    else if (_lexer.current() == lex_double_dot)
    {
      _lexer.next();

      return new (alloc_node()) xpath_ast_node(ast_step, set, axis_parent, nodetest_type_node, 0);
    }

    nodetest_t nt_type = nodetest_none;
    xpath_lexer_string nt_name;

    if (_lexer.current() == lex_string)
    {
      // node name test
      nt_name = _lexer.contents();
      _lexer.next();

      // was it an axis name?
      if (_lexer.current() == lex_double_colon)
      {
        // parse axis name
        if (axis_specified) throw_error(esT("Two axis specifiers in one step"));

        axis = parse_axis_name(nt_name, axis_specified);

        if (!axis_specified) throw_error(esT("Unknown axis"));

        // read actual node test
        _lexer.next();

        if (_lexer.current() == lex_multiply)
        {
          nt_type = nodetest_all;
          nt_name = xpath_lexer_string();
          _lexer.next();
        }
        else if (_lexer.current() == lex_string)
        {
          nt_name = _lexer.contents();
          _lexer.next();
        }
        else throw_error(esT("Unrecognized node test"));
      }

      if (nt_type == nodetest_none)
      {
        // node type test or processing-instruction
        if (_lexer.current() == lex_open_brace)
        {
          _lexer.next();

          if (_lexer.current() == lex_close_brace)
          {
            _lexer.next();

            nt_type = parse_node_test_type(nt_name);

            if (nt_type == nodetest_none) throw_error(esT("Unrecognized node type"));

            nt_name = xpath_lexer_string();
          }
          else if (nt_name == esT("processing-instruction"))
          {
            if (_lexer.current() != lex_quoted_string)
              throw_error(esT("Only literals are allowed as arguments to processing-instruction()"));

            nt_type = nodetest_pi;
            nt_name = _lexer.contents();
            _lexer.next();

            if (_lexer.current() != lex_close_brace)
              throw_error(esT("Unmatched brace near processing-instruction()"));
            _lexer.next();
          }
          else
            throw_error(esT("Unmatched brace near node type test"));

        }
        // QName or NCName:*
        else
        {
          if (nt_name.end - nt_name.begin > 2 && nt_name.end[-2] == ':' && nt_name.end[-1] == '*') // NCName:*
          {
            nt_name.end--; // erase *

            nt_type = nodetest_all_in_namespace;
          }
          else nt_type = nodetest_name;
        }
      }
    }
    else if (_lexer.current() == lex_multiply)
    {
      nt_type = nodetest_all;
      _lexer.next();
    }
    else throw_error(esT("Unrecognized node test"));

    xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step, set, axis, nt_type, alloc_string(nt_name));

    xpath_ast_node* last = 0;

    while (_lexer.current() == lex_open_square_brace)
    {
      _lexer.next();

      xpath_ast_node* expr = parse_expression();

      xpath_ast_node* pred = new (alloc_node()) xpath_ast_node(ast_predicate, xpathTypeNodeSet, expr);

      if (_lexer.current() != lex_close_square_brace)
        throw_error(esT("Unmatched square brace"));
      _lexer.next();

      if (last) last->set_next(pred);
      else n->set_right(pred);

      last = pred;
    }

    return n;
  }

  // RelativeLocationPath ::= Step | RelativeLocationPath '/' Step | RelativeLocationPath '//' Step
  xpath_ast_node* parse_relative_location_path(xpath_ast_node* set)
  {
    xpath_ast_node* n = parse_step(set);

    while (_lexer.current() == lex_slash || _lexer.current() == lex_double_slash)
    {
      lexeme_t l = _lexer.current();
      _lexer.next();

      if (l == lex_double_slash)
        n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);

      n = parse_step(n);
    }

    return n;
  }

  // LocationPath ::= RelativeLocationPath | AbsoluteLocationPath
  // AbsoluteLocationPath ::= '/' RelativeLocationPath? | '//' RelativeLocationPath
  xpath_ast_node* parse_location_path()
  {
    if (_lexer.current() == lex_slash)
    {
      _lexer.next();

      xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step_root, xpathTypeNodeSet);

      // relative location path can start from axis_attribute, dot, double_dot, multiply and string lexemes; any other lexeme means standalone root path
      lexeme_t l = _lexer.current();

      if (l == lex_string || l == lex_axis_attribute || l == lex_dot || l == lex_double_dot || l == lex_multiply)
        return parse_relative_location_path(n);
      else
        return n;
    }
    else if (_lexer.current() == lex_double_slash)
    {
      _lexer.next();

      xpath_ast_node* n = new (alloc_node()) xpath_ast_node(ast_step_root, xpathTypeNodeSet);
      n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);

      return parse_relative_location_path(n);
    }

    // else clause moved outside of if because of bogus warning 'control may reach end of non-void function being inlined' in gcc 4.0.1
    return parse_relative_location_path(0);
  }

  // PathExpr ::= LocationPath
  //        | FilterExpr
  //        | FilterExpr '/' RelativeLocationPath
  //        | FilterExpr '//' RelativeLocationPath
  // UnionExpr ::= PathExpr | UnionExpr '|' PathExpr
  // UnaryExpr ::= UnionExpr | '-' UnaryExpr
  xpath_ast_node* parse_path_or_unary_expression()
  {
    // Clarification.
    // PathExpr begins with either LocationPath or FilterExpr.
    // FilterExpr begins with PrimaryExpr
    // PrimaryExpr begins with '$' in case of it being a variable reference,
    // '(' in case of it being an expression, string literal, number constant or
    // function call.

    if (_lexer.current() == lex_var_ref || _lexer.current() == lex_open_brace ||
      _lexer.current() == lex_quoted_string || _lexer.current() == lex_number ||
      _lexer.current() == lex_string)
    {
      if (_lexer.current() == lex_string)
      {
        // This is either a function call, or not - if not, we shall proceed with location path
        EsString::const_pointer state = _lexer.state();

        while (PUGI__IS_CHARTYPE(*state, ct_space)) ++state;

        if (*state != '(') return parse_location_path();

        // This looks like a function call; however this still can be a node-test. Check it.
        if (parse_node_test_type(_lexer.contents()) != nodetest_none) return parse_location_path();
      }

      xpath_ast_node* n = parse_filter_expression();

      if (_lexer.current() == lex_slash || _lexer.current() == lex_double_slash)
      {
        lexeme_t l = _lexer.current();
        _lexer.next();

        if (l == lex_double_slash)
        {
          if (n->rettype() != xpathTypeNodeSet) throw_error(esT("Step has to be applied to node set"));

          n = new (alloc_node()) xpath_ast_node(ast_step, n, axis_descendant_or_self, nodetest_type_node, 0);
        }

        // select from location path
        return parse_relative_location_path(n);
      }

      return n;
    }
    else if (_lexer.current() == lex_minus)
    {
      _lexer.next();

      // precedence 7+ - only parses union expressions
      xpath_ast_node* expr = parse_expression_rec(parse_path_or_unary_expression(), 7);

      return new (alloc_node()) xpath_ast_node(ast_op_negate, xpathTypeNumber, expr);
    }
    else
      return parse_location_path();
  }

  struct binary_op_t
  {
    ast_type_t asttype;
    EsXmlXpathValueType rettype;
    int precedence;

    binary_op_t(): asttype(ast_unknown), rettype(xpathTypeNone), precedence(0)
    {
    }

    binary_op_t(ast_type_t asttype_, EsXmlXpathValueType rettype_, int precedence_): asttype(asttype_), rettype(rettype_), precedence(precedence_)
    {
    }

    static binary_op_t parse(xpath_lexer& lexer)
    {
      switch (lexer.current())
      {
      case lex_string:
        if (lexer.contents() == esT("or"))
          return binary_op_t(ast_op_or, xpathTypeBoolean, 1);
        else if (lexer.contents() == esT("and"))
          return binary_op_t(ast_op_and, xpathTypeBoolean, 2);
        else if (lexer.contents() == esT("div"))
          return binary_op_t(ast_op_divide, xpathTypeNumber, 6);
        else if (lexer.contents() == esT("mod"))
          return binary_op_t(ast_op_mod, xpathTypeNumber, 6);
        else
          return binary_op_t();

      case lex_equal:
        return binary_op_t(ast_op_equal, xpathTypeBoolean, 3);

      case lex_not_equal:
        return binary_op_t(ast_op_not_equal, xpathTypeBoolean, 3);

      case lex_less:
        return binary_op_t(ast_op_less, xpathTypeBoolean, 4);

      case lex_greater:
        return binary_op_t(ast_op_greater, xpathTypeBoolean, 4);

      case lex_less_or_equal:
        return binary_op_t(ast_op_less_or_equal, xpathTypeBoolean, 4);

      case lex_greater_or_equal:
        return binary_op_t(ast_op_greater_or_equal, xpathTypeBoolean, 4);

      case lex_plus:
        return binary_op_t(ast_op_add, xpathTypeNumber, 5);

      case lex_minus:
        return binary_op_t(ast_op_subtract, xpathTypeNumber, 5);

      case lex_multiply:
        return binary_op_t(ast_op_multiply, xpathTypeNumber, 6);

      case lex_union:
        return binary_op_t(ast_op_union, xpathTypeNodeSet, 7);

      default:
        return binary_op_t();
      }
    }
  };

  xpath_ast_node* parse_expression_rec(xpath_ast_node* lhs, int limit)
  {
    binary_op_t op = binary_op_t::parse(_lexer);

    while (op.asttype != ast_unknown && op.precedence >= limit)
    {
      _lexer.next();

      xpath_ast_node* rhs = parse_path_or_unary_expression();

      binary_op_t nextop = binary_op_t::parse(_lexer);

      while (nextop.asttype != ast_unknown && nextop.precedence > op.precedence)
      {
        rhs = parse_expression_rec(rhs, nextop.precedence);

        nextop = binary_op_t::parse(_lexer);
      }

      if (op.asttype == ast_op_union && (lhs->rettype() != xpathTypeNodeSet || rhs->rettype() != xpathTypeNodeSet))
        throw_error(esT("Union operator has to be applied to node sets"));

      lhs = new (alloc_node()) xpath_ast_node(op.asttype, op.rettype, lhs, rhs);

      op = binary_op_t::parse(_lexer);
    }

    return lhs;
  }

  // Expr ::= OrExpr
  // OrExpr ::= AndExpr | OrExpr 'or' AndExpr
  // AndExpr ::= EqualityExpr | AndExpr 'and' EqualityExpr
  // EqualityExpr ::= RelationalExpr
  //          | EqualityExpr '=' RelationalExpr
  //          | EqualityExpr '!=' RelationalExpr
  // RelationalExpr ::= AdditiveExpr
  //            | RelationalExpr '<' AdditiveExpr
  //            | RelationalExpr '>' AdditiveExpr
  //            | RelationalExpr '<=' AdditiveExpr
  //            | RelationalExpr '>=' AdditiveExpr
  // AdditiveExpr ::= MultiplicativeExpr
  //          | AdditiveExpr '+' MultiplicativeExpr
  //          | AdditiveExpr '-' MultiplicativeExpr
  // MultiplicativeExpr ::= UnaryExpr
  //              | MultiplicativeExpr '*' UnaryExpr
  //              | MultiplicativeExpr 'div' UnaryExpr
  //              | MultiplicativeExpr 'mod' UnaryExpr
  xpath_ast_node* parse_expression()
  {
    return parse_expression_rec(parse_path_or_unary_expression(), 0);
  }

  xpath_parser(EsString::const_pointer query, EsXmlXpathVariableSet* variables, xpath_allocator* alloc, EsXmlXpathParseResult* result): _alloc(alloc), _lexer(query), _query(query), _variables(variables), _result(result)
  {
  }

  xpath_ast_node* parse()
  {
    xpath_ast_node* result = parse_expression();

    if (_lexer.current() != lex_eof)
    {
      // there are still unparsed tokens left, error
      throw_error(esT("Incorrect query"));
    }

    return result;
  }

  static xpath_ast_node* parse(EsString::const_pointer query, EsXmlXpathVariableSet* variables, xpath_allocator* alloc, EsXmlXpathParseResult* result)
  {
    xpath_parser parser(query, variables, alloc, result);

    return parser.parse();
  }
};

struct xpath_query_impl
{
  static xpath_query_impl* create()
  {
    void* memory = EsXmlMemory::allocate(sizeof(xpath_query_impl));

    return new (memory) xpath_query_impl();
  }

  static void destroy(void* ptr)
  {
    if (!ptr) return;

    // free all allocated pages
    static_cast<xpath_query_impl*>(ptr)->alloc.release();

    // free allocator memory (with the first page)
    EsXmlMemory::deallocate(ptr);
  }

  xpath_query_impl(): root(0), alloc(&block)
  {
    block.next = 0;
  }

  xpath_ast_node* root;
  xpath_allocator alloc;
  xpath_memory_block block;
};

EsXmlXpathString evaluate_string_impl(xpath_query_impl* impl, const EsXmlXpathNode& n, xpath_stack_data& sd)
{
  if (!impl) return EsXmlXpathString();

  xpath_context c(n, 1, 1);

  return impl->root->eval_string(c, sd.stack);
}

EsXmlXpathException::EsXmlXpathException(const EsXmlXpathParseResult& result):
EsException(0, EsException::severityGeneric, EsException::facilityEsXml, result.error)
{
  ES_ASSERT(result.error);
}

void EsXmlXpathException::Throw(const EsXmlXpathParseResult& result)
{
  throw EsXmlXpathException(result);
}

EsXmlXpathNode::EsXmlXpathNode()
{
}

EsXmlXpathNode::EsXmlXpathNode(const EsXmlNode& node_): _node(node_)
{
}

EsXmlXpathNode::EsXmlXpathNode(const EsXmlAttribute& attribute_, const EsXmlNode& parent_): _node(attribute_ ? parent_ : EsXmlNode()), _attribute(attribute_)
{
}

EsXmlNode EsXmlXpathNode::nodeGet() const
{
  return _attribute ? EsXmlNode() : _node;
}

EsXmlAttribute EsXmlXpathNode::attributeGet() const
{
  return _attribute;
}

EsXmlNode EsXmlXpathNode::parentGet() const
{
  return _attribute ? _node : _node.parentGet();
}

static void unspecified_bool_xpath_node(EsXmlXpathNode***)
{
}

EsXmlXpathNode::operator EsXmlXpathNode::unspecified_bool_type() const
{
  return (_node || _attribute) ? unspecified_bool_xpath_node : 0;
}

bool EsXmlXpathNode::operator!() const
{
  return !(_node || _attribute);
}

bool EsXmlXpathNode::operator==(const EsXmlXpathNode& n) const
{
  return _node == n._node && _attribute == n._attribute;
}

bool EsXmlXpathNode::operator!=(const EsXmlXpathNode& n) const
{
  return _node != n._node || _attribute != n._attribute;
}

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
bool operator&&(const EsXmlXpathNode& lhs, bool rhs)
{
  return (bool)lhs && rhs;
}

bool operator||(const EsXmlXpathNode& lhs, bool rhs)
{
  return (bool)lhs || rhs;
}
#endif

void EsXmlXpathNodeSet::_assign(const_iterator begin_, const_iterator end_)
{
  ES_ASSERT(begin_ <= end_);

  size_t size_ = static_cast<size_t>(end_ - begin_);

  if (size_ <= 1)
  {
    // deallocate old buffer
    if (_begin != &_storage) EsXmlMemory::deallocate(_begin);

    // use internal buffer
    if (begin_ != end_) _storage = *begin_;

    _begin = &_storage;
    _end = &_storage + size_;
  }
  else
  {
    // make heap copy
    EsXmlXpathNode* storage = static_cast<EsXmlXpathNode*>(EsXmlMemory::allocate(size_ * sizeof(EsXmlXpathNode)));

    if (!storage)
    {
      throw std::bad_alloc();
    }

    memcpy(storage, begin_, size_ * sizeof(EsXmlXpathNode));

    // deallocate old buffer
    if (_begin != &_storage) EsXmlMemory::deallocate(_begin);

    // finalize
    _begin = storage;
    _end = storage + size_;
  }
}

EsXmlXpathNodeSet::EsXmlXpathNodeSet(): _type(type_unsorted), _begin(&_storage), _end(&_storage)
{
}

EsXmlXpathNodeSet::EsXmlXpathNodeSet(const_iterator begin_, const_iterator end_, type_t type_): _type(type_), _begin(&_storage), _end(&_storage)
{
  _assign(begin_, end_);
}

EsXmlXpathNodeSet::~EsXmlXpathNodeSet()
{
  if (_begin != &_storage) EsXmlMemory::deallocate(_begin);
}

EsXmlXpathNodeSet::EsXmlXpathNodeSet(const EsXmlXpathNodeSet& ns): _type(ns._type), _begin(&_storage), _end(&_storage)
{
  _assign(ns._begin, ns._end);
}

EsXmlXpathNodeSet& EsXmlXpathNodeSet::operator=(const EsXmlXpathNodeSet& ns)
{
  if (this == &ns) return *this;

  _type = ns._type;
  _assign(ns._begin, ns._end);

  return *this;
}

EsXmlXpathNodeSet::type_t EsXmlXpathNodeSet::typeGet() const
{
  return _type;
}

size_t EsXmlXpathNodeSet::size() const
{
  return _end - _begin;
}

bool EsXmlXpathNodeSet::empty() const
{
  return _begin == _end;
}

const EsXmlXpathNode& EsXmlXpathNodeSet::operator[](size_t index) const
{
  ES_ASSERT(index < size());
  return _begin[index];
}

EsXmlXpathNodeSet::const_iterator EsXmlXpathNodeSet::begin() const
{
  return _begin;
}

EsXmlXpathNodeSet::const_iterator EsXmlXpathNodeSet::end() const
{
  return _end;
}

void EsXmlXpathNodeSet::sort(bool reverse)
{
  _type = xpath_sort(_begin, _end, _type, reverse);
}

EsXmlXpathNode EsXmlXpathNodeSet::first() const
{
  return xpath_first(_begin, _end, _type);
}

EsXmlXpathParseResult::EsXmlXpathParseResult(): error(esT("Internal error")), offset(0)
{
}

EsXmlXpathParseResult::operator bool() const
{
  return error == 0;
}

EsString::const_pointer EsXmlXpathParseResult::descriptionGet() const
{
  return error ? error : esT("No error");
}

EsXmlXpathVariable::EsXmlXpathVariable(): _type(xpathTypeNone), _next(0)
{
}

EsString::const_pointer EsXmlXpathVariable::nameGet() const
{
  switch (_type)
  {
  case xpathTypeNodeSet:
    return static_cast<const xpath_variable_node_set*>(this)->name;

  case xpathTypeNumber:
    return static_cast<const xpath_variable_number*>(this)->name;

  case xpathTypeString:
    return static_cast<const xpath_variable_string*>(this)->name;

  case xpathTypeBoolean:
    return static_cast<const xpath_variable_boolean*>(this)->name;

  default:
    ES_ASSERT(!"Invalid variable type");
    return 0;
  }
}

EsXmlXpathValueType EsXmlXpathVariable::typeGet() const
{
  return _type;
}

bool EsXmlXpathVariable::booleanGet() const
{
  return (_type == xpathTypeBoolean) ? static_cast<const xpath_variable_boolean*>(this)->value : false;
}

double EsXmlXpathVariable::numberGet() const
{
  return (_type == xpathTypeNumber) ? static_cast<const xpath_variable_number*>(this)->value : gen_nan();
}

EsString::const_pointer EsXmlXpathVariable::stringGet() const
{
  EsString::const_pointer value = (_type == xpathTypeString) ? static_cast<const xpath_variable_string*>(this)->value : 0;
  return value ? value : esT("");
}

const EsXmlXpathNodeSet& EsXmlXpathVariable::nodesetGet() const
{
  return (_type == xpathTypeNodeSet) ? static_cast<const xpath_variable_node_set*>(this)->value : dummy_node_set;
}

bool EsXmlXpathVariable::set(bool value)
{
  if (_type != xpathTypeBoolean) return false;

  static_cast<xpath_variable_boolean*>(this)->value = value;
  return true;
}

bool EsXmlXpathVariable::set(double value)
{
  if (_type != xpathTypeNumber) return false;

  static_cast<xpath_variable_number*>(this)->value = value;
  return true;
}

bool EsXmlXpathVariable::set(EsString::const_pointer value)
{
  if (_type != xpathTypeString) return false;

  xpath_variable_string* var = static_cast<xpath_variable_string*>(this);

  // duplicate string
  size_t size = (strlength(value) + 1) * sizeof(EsString::value_type);

  EsString::pointer copy = static_cast<EsString::pointer>(EsXmlMemory::allocate(size));
  if (!copy) return false;

  memcpy(copy, value, size);

  // replace old string
  if (var->value) EsXmlMemory::deallocate(var->value);
  var->value = copy;

  return true;
}

bool EsXmlXpathVariable::set(const EsXmlXpathNodeSet& value)
{
  if (_type != xpathTypeNodeSet) return false;

  static_cast<xpath_variable_node_set*>(this)->value = value;
  return true;
}

EsXmlXpathVariableSet::EsXmlXpathVariableSet()
{
  for (size_t i = 0; i < sizeof(_data) / sizeof(_data[0]); ++i) _data[i] = 0;
}

EsXmlXpathVariableSet::~EsXmlXpathVariableSet()
{
  for (size_t i = 0; i < sizeof(_data) / sizeof(_data[0]); ++i)
  {
    EsXmlXpathVariable* var = _data[i];

    while (var)
    {
      EsXmlXpathVariable* next = var->_next;

      delete_xpath_variable(var->_type, var);

      var = next;
    }
  }
}

EsXmlXpathVariable* EsXmlXpathVariableSet::find(EsString::const_pointer name) const
{
  const size_t hash_size = sizeof(_data) / sizeof(_data[0]);
  size_t hash = hash_string(name) % hash_size;

  // look for existing variable
  for (EsXmlXpathVariable* var = _data[hash]; var; var = var->_next)
    if (strequal(var->nameGet(), name))
      return var;

  return 0;
}

EsXmlXpathVariable* EsXmlXpathVariableSet::add(EsString::const_pointer name, EsXmlXpathValueType type)
{
  const size_t hash_size = sizeof(_data) / sizeof(_data[0]);
  size_t hash = hash_string(name) % hash_size;

  // look for existing variable
  for (EsXmlXpathVariable* var = _data[hash]; var; var = var->_next)
    if (strequal(var->nameGet(), name))
      return var->typeGet() == type ? var : 0;

  // add new variable
  EsXmlXpathVariable* result = new_xpath_variable(type, name);

  if (result)
  {
    result->_type = type;
    result->_next = _data[hash];

    _data[hash] = result;
  }

  return result;
}

bool EsXmlXpathVariableSet::set(EsString::const_pointer name, bool value)
{
  EsXmlXpathVariable* var = add(name, xpathTypeBoolean);
  return var ? var->set(value) : false;
}

bool EsXmlXpathVariableSet::set(EsString::const_pointer name, double value)
{
  EsXmlXpathVariable* var = add(name, xpathTypeNumber);
  return var ? var->set(value) : false;
}

bool EsXmlXpathVariableSet::set(EsString::const_pointer name, EsString::const_pointer value)
{
  EsXmlXpathVariable* var = add(name, xpathTypeString);
  return var ? var->set(value) : false;
}

bool EsXmlXpathVariableSet::set(EsString::const_pointer name, const EsXmlXpathNodeSet& value)
{
  EsXmlXpathVariable* var = add(name, xpathTypeNodeSet);
  return var ? var->set(value) : false;
}

EsXmlXpathVariable* EsXmlXpathVariableSet::get(EsString::const_pointer name)
{
  return find(name);
}

const EsXmlXpathVariable* EsXmlXpathVariableSet::get(EsString::const_pointer name) const
{
  return find(name);
}

EsXmlXpathQuery::EsXmlXpathQuery(EsString::const_pointer query, EsXmlXpathVariableSet* variables): _impl(0)
{
  xpath_query_impl* qimpl = xpath_query_impl::create();

  if (!qimpl)
  {
    throw std::bad_alloc();
  }
  else
  {
    buffer_holder impl_holder(qimpl, xpath_query_impl::destroy);

    qimpl->root = xpath_parser::parse(query, variables, &qimpl->alloc, &_result);

    if (qimpl->root)
    {
      _impl = static_cast<xpath_query_impl*>(impl_holder.release());
      _result.error = 0;
    }
  }
}

EsXmlXpathQuery::~EsXmlXpathQuery()
{
  xpath_query_impl::destroy(_impl);
}

EsXmlXpathValueType EsXmlXpathQuery::returnTypeGet() const
{
  if (!_impl) return xpathTypeNone;

  return static_cast<xpath_query_impl*>(_impl)->root->rettype();
}

bool EsXmlXpathQuery::asBool(const EsXmlXpathNode& n) const
{
  if (!_impl) return false;

  xpath_context c(n, 1, 1);
  xpath_stack_data sd;

  return static_cast<xpath_query_impl*>(_impl)->root->eval_boolean(c, sd.stack);
}

double EsXmlXpathQuery::asNumber(const EsXmlXpathNode& n) const
{
  if (!_impl) return gen_nan();

  xpath_context c(n, 1, 1);
  xpath_stack_data sd;

  return static_cast<xpath_query_impl*>(_impl)->root->eval_number(c, sd.stack);
}

EsString EsXmlXpathQuery::asString(const EsXmlXpathNode& n) const
{
  xpath_stack_data sd;

  return evaluate_string_impl(static_cast<xpath_query_impl*>(_impl), n, sd).c_str();
}

size_t EsXmlXpathQuery::asString(EsString::pointer buffer, size_t capacity, const EsXmlXpathNode& n) const
{
  xpath_stack_data sd;

  EsXmlXpathString r = evaluate_string_impl(static_cast<xpath_query_impl*>(_impl), n, sd);

  size_t full_size = r.length() + 1;

  if (capacity > 0)
  {
    size_t size = (full_size < capacity) ? full_size : capacity;
    ES_ASSERT(size > 0);

    memcpy(buffer, r.c_str(), (size - 1) * sizeof(EsString::value_type));
    buffer[size - 1] = 0;
  }

  return full_size;
}

EsXmlXpathNodeSet EsXmlXpathQuery::asNodeSet(const EsXmlXpathNode& n) const
{
  if (!_impl) return EsXmlXpathNodeSet();

  xpath_ast_node* root = static_cast<xpath_query_impl*>(_impl)->root;

  if (root->rettype() != xpathTypeNodeSet)
  {
    EsXmlXpathParseResult res;
    res.error = esT("Expression does not evaluate to node set");

    EsXmlXpathException::Throw(res);
  }

  xpath_context c(n, 1, 1);
  xpath_stack_data sd;

  xpath_node_set_raw r = root->eval_node_set(c, sd.stack);

  return EsXmlXpathNodeSet(r.begin(), r.end(), r.typeGet());
}

const EsXmlXpathParseResult& EsXmlXpathQuery::resultGet() const
{
  return _result;
}

static void unspecified_bool_xpath_query(EsXmlXpathQuery***)
{
}

EsXmlXpathQuery::operator EsXmlXpathQuery::unspecified_bool_type() const
{
  return _impl ? unspecified_bool_xpath_query : 0;
}

bool EsXmlXpathQuery::operator!() const
{
  return !_impl;
}

EsXmlXpathNode EsXmlNode::singleNodeSelect(EsString::const_pointer query, EsXmlXpathVariableSet* variables) const
{
  EsXmlXpathQuery q(query, variables);
  return singleNodeSelect(q);
}

EsXmlXpathNode EsXmlNode::singleNodeSelect(const EsXmlXpathQuery& query) const
{
  EsXmlXpathNodeSet s = query.asNodeSet(*this);
  return s.empty() ? EsXmlXpathNode() : s.first();
}

EsXmlXpathNodeSet EsXmlNode::nodesSelect(EsString::const_pointer query, EsXmlXpathVariableSet* variables) const
{
  EsXmlXpathQuery q(query, variables);
  return nodesSelect(q);
}

EsXmlXpathNodeSet EsXmlNode::nodesSelect(const EsXmlXpathQuery& query) const
{
  return query.asNodeSet(*this);
}
#endif // #ifndef ES_XML_NO_XPATH

#ifdef __BORLANDC__
#  pragma option pop
#endif

// Intel C++ does not properly keep warning state for function templates,
// so popping warning state at the end of translation unit leads to warnings in the middle.
#if defined(_MSC_VER) && !defined(__INTEL_COMPILER)
#  pragma warning(pop)
#endif

