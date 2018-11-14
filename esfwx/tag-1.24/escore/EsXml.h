#ifndef _es_xml_h_
#define _es_xml_h_

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

// Define version macro; evaluates to major * 100 + minor so that it's safe to use in less-than comparisons
#define ESXML_VERSION 140

// Tree node types
enum EsXmlNodeType
{
	xmlNodeNull,				    ///< Empty (null) node handle
	xmlNodeDocument,		    ///< A document tree's absolute root
	xmlNodeElement,			    ///< Element tag, i.e. '<node/>'
	xmlNodePcdata,			    ///< Plain character data, i.e. 'text'
	xmlNodeCdata,				    ///< Character data, i.e. '<![CDATA[text]]>'
	xmlNodeComment,			    ///< Comment tag, i.e. '<!-- text -->'
	xmlNodeProcInstr,		    ///< Processing instruction, i.e. '<?name?>'
	xmlNodeDocDecl,	        ///< Document declaration, i.e. '<?xml version="1.0"?>'
	xmlNodeDoctype			    ///< Document type declaration, i.e. '<!DOCTYPE doc>'
};

enum
{
  /// Parsing options
  ///
	// Minimal parsing mode (equivalent to turning all other flags off).
	// Only elements and PCDATA sections are added to the DOM tree, no text conversions are performed.
	xmlParseMinimal					= 0x0000,
	// This flag determines if processing instructions (node_pi) are added to the DOM tree. This flag is off by default.
	xmlParseProcInstr				= 0x0001,
	// This flag determines if comments (node_comment) are added to the DOM tree. This flag is off by default.
	xmlParseComments				= 0x0002,
	// This flag determines if CDATA sections (node_cdata) are added to the DOM tree. This flag is on by default.
	xmlParseCdata						= 0x0004,
	// This flag determines if plain character data (node_pcdata) that consist only of whitespace are added to the DOM tree.
	// This flag is off by default; turning it on usually results in slower parsing and more memory consumption.
	xmlParseWhitespacePcdata = 0x0008,
	// This flag determines if character and entity references are expanded during parsing. This flag is on by default.
	xmlParseEscapes					= 0x0010,
	// This flag determines if EOL characters are normalized (converted to #xA) during parsing. This flag is on by default.
	xmlParseNormalizeEol		= 0x0020,
	// This flag determines if attribute values are normalized using CDATA normalization rules during parsing. This flag is on by default.
	xmlParseWconvAttribute	= 0x0040,
	// This flag determines if attribute values are normalized using NMTOKENS normalization rules during parsing. This flag is off by default.
	xmlParseWnormAttribute	= 0x0080,
	// This flag determines if document declaration (node_declaration) is added to the DOM tree. This flag is off by default.
	xmlParseDeclaration			= 0x0100,
	// This flag determines if document type declaration (node_doctype) is added to the DOM tree. This flag is off by default.
	xmlParseDoctype					= 0x0200,
	// This flag determines if plain character data (node_pcdata) that is the only child of the parent node and that consists only
	// of whitespace is added to the DOM tree.
	// This flag is off by default; turning it on may resultGet in slower parsing and more memory consumption.
	xmlParseWhitespacePcdataSingle = 0x0400,
	// This flag determines if leading and trailing whitespace is to be removed from plain character data. This flag is off by default.
	xmlParseTrimPcdata      = 0x0800,
	// This flag determines if plain character data that does not have a parent node is added to the DOM tree, and if an empty document
	// is a valid document. This flag is off by default.
	xmlParseFragment        = 0x1000,  
	// The default parsing mode.
	// Elements, PCDATA and CDATA sections are added to the DOM tree, character/reference entities are expanded,
	// End-of-Line characters are normalized, attribute values are normalized using CDATA normalization rules.
	xmlParseDefault					= xmlParseCdata|
                          	xmlParseEscapes|
                            xmlParseWconvAttribute|
                            xmlParseNormalizeEol,
	// The full parsing mode.
	// Nodes of all types are added to the DOM tree, character/reference entities are expanded,
	// End-of-Line characters are normalized, attribute values are normalized using CDATA normalization rules.
	xmlParseFull						= xmlParseDefault|
                            xmlParseProcInstr|
                            xmlParseComments|
                            xmlParseDeclaration|
                            xmlParseDoctype,

  /// Formatting options
  ///
	// Indent the nodes that are written to output stream with as many indentation strings as deep the node is in DOM tree. This flag is on by default.
	xmlFormatIndent			= 0x01,
	// Write encoding-specific BOM to the output stream. This flag is off by default.
	xmlFormatWriteBom		= 0x02,
	// Use raw output mode (no indentation and no line breaks are written). This flag is off by default.
	xmlFormatRaw				= 0x04,
	// Omit default XML declaration even if there is no declaration in the document. This flag is off by default.
	xmlFormatNoDocDecl	= 0x08,
	// Don't escape attribute values and PCDATA contents. This flag is off by default.
	xmlFormatNoEscapes	= 0x10,
	// The default set of formatting flags.
	// Nodes are indented depending on their depth in DOM tree, a default declaration is output if document has none.
	xmlFormatDefault		= xmlFormatIndent
};

// These flags determine the encoding of input data for XML document
enum EsXmlEncoding
{
	xmlEncodingAuto,		  ///< Auto-detect input encoding using BOM or < / <? detection; use UTF8 if BOM is not found
	xmlEncodingUtf8,		  ///< UTF8 encoding
	xmlEncodingUtf16_le,	///< Little-endian UTF16
	xmlEncodingUtf16_be,	///< Big-endian UTF16
	xmlEncodingUtf16,		  ///< UTF16 with native endianness
	xmlEncodingUtf32_le,	///< Little-endian UTF32
	xmlEncodingUtf32_be,	///< Big-endian UTF32
	xmlEncodingUtf32,		  ///< UTF32 with native endianness
	xmlEncodingWchar,		  ///< The same encoding ES_WCHAR has (either UTF16 or UTF32)
	xmlEncodingLatin1     ///< ANSI encoding
};

// Forward declarations
struct EsXmlAttributeStruct;
struct EsXmlNodeStruct;

class EsXmlNodeIterator;
class EsXmlAttributeIterator;
class EsXmlNamedNodeIterator;

class EsXmlTreeWalker;
class EsXmlNode;
class EsXmlText;

#ifndef ES_XML_NO_XPATH
class EsXmlXpathNode;
class EsXmlXpathNodeSet;
class EsXmlXpathQuery;
class EsXmlXpathVariableSet;
#endif

// Range-based for loop support
template <typename It> class EsXmlObjectRange
{
public:
	typedef It const_iterator;
	typedef It iterator;

	EsXmlObjectRange(It b, It e): _begin(b), _end(e)
	{
	}

	It begin() const { return _begin; }
	It end() const { return _end; }

private:
	It _begin, _end;
};

// Writer interface for node printing (see EsXmlNode::print)
class ESCORE_CLASS EsXmlWriterIntf
{
public:
	virtual ~EsXmlWriterIntf() {}

	// Write memory chunk into stream/file/whatever
	virtual void write(const void* data, size_t size) = 0;
};

// EsXmlWriterIntf implementation for EsFile
class ESCORE_CLASS EsXmlWriterFile: public EsXmlWriterIntf
{
public:
	EsXmlWriterFile(EsFile& file);

	virtual void write(const void* data, size_t size);

private:
	EsFile& m_file;
};

// EsXmlWriterIntf implementation for EsByteString
class ESCORE_CLASS EsXmlWriterByteString : public EsXmlWriterIntf
{
public:
	EsXmlWriterByteString(EsByteString& str);

	virtual void write(const void* data, size_t size);

private:
	EsByteString& m_str;
};

// EsXmlWriterIntf implementation for EsBinBuffer
class ESCORE_CLASS EsXmlWriterBinBuffer : public EsXmlWriterIntf
{
public:
	EsXmlWriterBinBuffer(EsBinBuffer& buff);

	virtual void write(const void* data, size_t size);

private:
	EsBinBuffer& m_buff;
};

// A light-weight handle for manipulating attributes in DOM tree
class ESCORE_CLASS EsXmlAttribute
{
	friend class EsXmlAttributeIterator;
	friend class EsXmlNode;

private:
	EsXmlAttributeStruct* _attr;

	typedef void (*unspecified_bool_type)(EsXmlAttribute***);

public:
	// Default constructor. Constructs an empty attribute.
	EsXmlAttribute();

	// Constructs attribute from internal pointer
	explicit EsXmlAttribute(EsXmlAttributeStruct* attr);

	// Safe bool conversion operator
	operator unspecified_bool_type() const;

	// Borland C++ workaround
	bool operator!() const;

	// Comparison operators (compares wrapped attribute pointers)
	bool operator==(const EsXmlAttribute& r) const;
	bool operator!=(const EsXmlAttribute& r) const;
	bool operator<(const EsXmlAttribute& r) const;
	bool operator>(const EsXmlAttribute& r) const;
	bool operator<=(const EsXmlAttribute& r) const;
	bool operator>=(const EsXmlAttribute& r) const;

	// Check if attribute is empty
	bool empty() const;

	// Get attribute name/value, or "" if attribute is empty
	EsString::const_pointer nameGet() const;
	EsString::const_pointer valueGet() const;

	// Get attribute value, or the default value if attribute is empty
	EsString::const_pointer asString(const EsString& def = EsString::null()) const;

	// Get attribute value as a number, or the default value if conversion did not succeed or attribute is empty
	int asInt(int def = 0) const;
	unsigned int asUint(unsigned int def = 0) const;
	double asDouble(double def = 0) const;
	float asFloat(float def = 0) const;

	long long asLLong(long long def = 0) const;
	unsigned long long asULLong(unsigned long long def = 0) const;

	// Get attribute value as bool (returns true if first character is in '1tTyY' set), or the default value if attribute is empty
	bool asBool(bool def = false) const;

	// Set attribute name/value (returns false if attribute is empty or there is not enough memory)
	bool nameSet(EsString::const_pointer rhs);
	bool valueSet(EsString::const_pointer rhs);

	// Set attribute value with type conversion (numbers are converted to strings, boolean is converted to "true"/"false")
	bool valueSet(int rhs);
	bool valueSet(unsigned int rhs);
	bool valueSet(double rhs);
	bool valueSet(bool rhs);
	bool valueSet(long long rhs);
	bool valueSet(unsigned long long rhs);

	// Set attribute value (equivalent to set_value without error checking)
	EsXmlAttribute& operator=(EsString::const_pointer rhs);
	EsXmlAttribute& operator=(int rhs);
	EsXmlAttribute& operator=(unsigned int rhs);
	EsXmlAttribute& operator=(double rhs);
	EsXmlAttribute& operator=(bool rhs);
	EsXmlAttribute& operator=(long long rhs);
	EsXmlAttribute& operator=(unsigned long long rhs);

	// Get next/previous attribute in the attribute list of the parent node
	EsXmlAttribute nextAttributeGet() const;
	EsXmlAttribute previousAttributeGet() const;

	// Get hash value (unique for handles to the same object)
	size_t hashGet() const;

	// Get internal pointer
	EsXmlAttributeStruct* internalObjectGet() const;
};

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
// Borland C++ workaround
ESCORE_FUNC( bool, operator&&(const EsXmlAttribute& lhs, bool rhs) );
ESCORE_FUNC( bool, operator||(const EsXmlAttribute& lhs, bool rhs) );
#endif

// Parsing status, returned as part of EsXmlParseResult object
enum EsXmlParseStatus
{
	xmlParseStatusOk = 0,								///< No error
	xmlParseStatusFileNotFound,					///< File was not found during loadFile()
	xmlParseStatusIoError,							///< Error reading from file/stream
	xmlParseStatusOutOfMemory,					///< Could not allocate memory
	xmlParseStatusInternalError,				///< Internal error occurred
	xmlParseStatusUnrecognizedTag,			///< Parser could not determine tag type
	xmlParseStatusBadProcInstr,					///< Parsing error occurred while parsing document declaration/processing instruction
	xmlParseStatusBadComment,						///< Parsing error occurred while parsing comment
	xmlParseStatusBadCdata,							///< Parsing error occurred while parsing CDATA section
	xmlParseStatusBadDoctype,						///< Parsing error occurred while parsing document type declaration
	xmlParseStatusBadPcdata,						///< Parsing error occurred while parsing PCDATA section
	xmlParseStatusBadStartElement,			///< Parsing error occurred while parsing start element tag
	xmlParseStatusBadAttribute,					///< Parsing error occurred while parsing element attribute
	xmlParseStatusBadEndElement,				///< Parsing error occurred while parsing end element tag
	xmlParseStatusEndElementMismatch,		///< There was a mismatch of start-end tags (closing tag had incorrect name, some tag was not closed or there was an excessive closing tag)
	xmlParseStatusAppendInvalidRoot,	  ///< Unable to append nodes since root type is not xmlNodeElement or xmlNodeDocument (exclusive to EsXmlNode::append_buffer)
	xmlParseStatusNoDocumentElement	    ///< Parsing resulted in a document without element nodes
};

// Parsing resultGet
struct ESCORE_CLASS EsXmlParseResult
{
	// Parsing status (see EsXmlParseStatus)
	EsXmlParseStatus status;

	// Last parsed offset (in EsString::value_type units from start of input data)
	ptrdiff_t offset;

	// Source document encoding
	EsXmlEncoding encoding;

	// Default constructor, initializes object to failed state
	EsXmlParseResult();

	// Cast to bool operator
	operator bool() const;

	// Get error description
	EsString::const_pointer descriptionGet() const;
};

// A light-weight handle for manipulating nodes in DOM tree
class ESCORE_CLASS EsXmlNode
{
	friend class EsXmlAttributeIterator;
	friend class EsXmlNodeIterator;
	friend class EsXmlNamedNodeIterator;

protected:
	EsXmlNodeStruct* _root;

	typedef void (*unspecified_bool_type)(EsXmlNode***);

public:
	// Default constructor. Constructs an empty node.
	EsXmlNode();

	// Constructs node from internal pointer
	explicit EsXmlNode(EsXmlNodeStruct* p);

	// Safe bool conversion operator
	operator unspecified_bool_type() const;

	// Borland C++ workaround
	bool operator!() const;

	// Comparison operators (compares wrapped node pointers)
	bool operator==(const EsXmlNode& r) const;
	bool operator!=(const EsXmlNode& r) const;
	bool operator<(const EsXmlNode& r) const;
	bool operator>(const EsXmlNode& r) const;
	bool operator<=(const EsXmlNode& r) const;
	bool operator>=(const EsXmlNode& r) const;

	// Check if node is empty.
	bool empty() const;

	// Get node type
	EsXmlNodeType typeGet() const;

	// Get node name, or "" if node is empty or it has no name
	EsString::const_pointer nameGet() const;
	// Get node value, or "" if node is empty or it has no value
	// Note: For <node>text</node> node.value() does not return "text"! Use child_value() or text() methods to access text inside nodes.
	EsString::const_pointer valueGet() const;

	// Get attribute list
	EsXmlAttribute firstAttributeGet() const;
	EsXmlAttribute lastAttributeGet() const;

	// Get children list
	EsXmlNode firstChildGet() const;
	EsXmlNode lastChildGet() const;

	// Get next/previous sibling in the children list of the parent node
	EsXmlNode nextSiblingGet() const;
	EsXmlNode previousSiblingGet() const;

	// Get parent node
	EsXmlNode parentGet() const;

	// Get root of DOM tree this node belongs to
	EsXmlNode rootGet() const;

	// Get text object for the current node
	EsXmlText textGet() const;

	// Get child, attribute or next/previous sibling with the specified name
	EsXmlNode childGet(EsString::const_pointer name) const;
	EsXmlAttribute attributeGet(EsString::const_pointer name) const;
	EsXmlNode nextSiblingGet(EsString::const_pointer name) const;
	EsXmlNode previousSiblingGet(EsString::const_pointer name) const;

	// Get child value of current node; that is, value of the first child node of type PCDATA/CDATA
	EsString::const_pointer childValueGet() const;

	// Get child value of child with specified name. Equivalent to child(name).child_value().
	EsString::const_pointer childValueGet(EsString::const_pointer name) const;

	// Set node name/value (returns false if node is empty, there is not enough memory, or node can not have name/value)
	bool nameSet(EsString::const_pointer rhs);
	bool valueSet(EsString::const_pointer rhs);

	// Add attribute with specified name. Returns added attribute, or empty attribute on errors.
	EsXmlAttribute attributeAppend(EsString::const_pointer name);
	EsXmlAttribute attributePrepend(EsString::const_pointer name);
	EsXmlAttribute attributeInsertAfter(EsString::const_pointer name, const EsXmlAttribute& attr);
	EsXmlAttribute attributeInsertBefore(EsString::const_pointer name, const EsXmlAttribute& attr);

	// Add a copy of the specified attribute. Returns added attribute, or empty attribute on errors.
	EsXmlAttribute attributeCopyAppend(const EsXmlAttribute& proto);
	EsXmlAttribute attributeCopyPrepend(const EsXmlAttribute& proto);
	EsXmlAttribute attributeCopyInsertAfter(const EsXmlAttribute& proto, const EsXmlAttribute& attr);
	EsXmlAttribute attributeCopyInsertBefore(const EsXmlAttribute& proto, const EsXmlAttribute& attr);

	// Add child node with specified type. Returns added node, or empty node on errors.
	EsXmlNode childAppend(EsXmlNodeType type = xmlNodeElement);
	EsXmlNode childPrepend(EsXmlNodeType type = xmlNodeElement);
	EsXmlNode childInsertAfter(EsXmlNodeType type, const EsXmlNode& node);
	EsXmlNode childInsertBefore(EsXmlNodeType type, const EsXmlNode& node);

	// Add child element with specified name. Returns added node, or empty node on errors.
	EsXmlNode childAppend(EsString::const_pointer name);
	EsXmlNode childPrepend(EsString::const_pointer name);
	EsXmlNode childInsertAfter(EsString::const_pointer name, const EsXmlNode& node);
	EsXmlNode childInsertBefore(EsString::const_pointer name, const EsXmlNode& node);

	// Add a copy of the specified node as a child. Returns added node, or empty node on errors.
	EsXmlNode childCopyAppend(const EsXmlNode& proto);
	EsXmlNode childCopyPrepend(const EsXmlNode& proto);
	EsXmlNode childCopyInsertAfter(const EsXmlNode& proto, const EsXmlNode& node);
	EsXmlNode childCopyInsertBefore(const EsXmlNode& proto, const EsXmlNode& node);

	// Remove specified attribute
	bool attributeRemove(const EsXmlAttribute& a);
	bool attributeRemove(EsString::const_pointer name);

	// Remove specified child
	bool childRemove(const EsXmlNode& n);
	bool childRemove(EsString::const_pointer name);

	// Parses buffer as an XML document fragment and appends all nodes as children of the current node.
	// Copies/converts the buffer, so it may be deleted or changed after the function returns.
	// Note: append_buffer allocates memory that has the lifetime of the owning document; removing the appended nodes does not immediately reclaim that memory.
	EsXmlParseResult bufferAppend(const void* contents, size_t size, unsigned int options = xmlParseDefault, EsXmlEncoding encoding = xmlEncodingAuto);

	// Find attribute using predicate. Returns first attribute for which predicate returned true.
	template <typename Predicate> EsXmlAttribute attributeFind(Predicate pred) const
	{
		if (!_root) return EsXmlAttribute();

		for (EsXmlAttribute attrib = firstAttributeGet(); attrib; attrib = attrib.nextAttributeGet())
			if (pred(attrib))
				return attrib;

		return EsXmlAttribute();
	}

	// Find child node using predicate. Returns first child for which predicate returned true.
	template <typename Predicate> EsXmlNode childFind(Predicate pred) const
	{
		if (!_root) return EsXmlNode();

		for (EsXmlNode node = firstChildGet(); node; node = node.nextSiblingGet())
			if (pred(node))
				return node;

		return EsXmlNode();
	}

	// Find node from subtree using predicate. Returns first node from subtree (depth-first), for which predicate returned true.
	template <typename Predicate> EsXmlNode nodeFind(Predicate pred) const
	{
		if (!_root) return EsXmlNode();

		EsXmlNode cur = firstChildGet();

		while (cur._root && cur._root != _root)
		{
			if (pred(cur)) return cur;

			if (cur.firstChildGet()) cur = cur.firstChildGet();
			else if (cur.nextSiblingGet()) cur = cur.nextSiblingGet();
			else
			{
				while (!cur.nextSiblingGet() && cur._root != _root) cur = cur.parentGet();

				if (cur._root != _root) cur = cur.nextSiblingGet();
			}
		}

		return EsXmlNode();
	}

	// Find child node by attribute name/value
	EsXmlNode childFindByAttribute(EsString::const_pointer name, EsString::const_pointer attr_name, EsString::const_pointer attr_value) const;
	EsXmlNode childFindByAttribute(EsString::const_pointer attr_name, EsString::const_pointer attr_value) const;

	// Get the absolute node path from root as a text string.
	EsString pathGet(EsString::value_type delimiter = esT('/')) const;

	// Search for a node by path consisting of node names and . or .. elements.
	EsXmlNode firstElementGetByPath(EsString::const_pointer path, EsString::value_type delimiter = esT('/')) const;

	// Recursively traverse subtree with EsXmlTreeWalker
	bool traverse(EsXmlTreeWalker& walker);

#ifndef ES_XML_NO_XPATH
	// Select single node by evaluating XPath query. Returns first node from the resulting node set.
	EsXmlXpathNode singleNodeSelect(EsString::const_pointer query, EsXmlXpathVariableSet* variables = 0) const;
	EsXmlXpathNode singleNodeSelect(const EsXmlXpathQuery& query) const;

	// Select node set by evaluating XPath query
	EsXmlXpathNodeSet nodesSelect(EsString::const_pointer query, EsXmlXpathVariableSet* variables = 0) const;
	EsXmlXpathNodeSet nodesSelect(const EsXmlXpathQuery& query) const;
#endif

	// Print subtree using a writer object
	void print(EsXmlWriterIntf& writer, const EsString& indent = EsString::tab(), unsigned int flags = xmlFormatDefault, EsXmlEncoding encoding = xmlEncodingAuto, unsigned int depth = 0) const;

	// Child nodes iterators
	typedef EsXmlNodeIterator iterator;

	iterator begin() const;
	iterator end() const;

	// Attribute iterators
	typedef EsXmlAttributeIterator attribute_iterator;

	attribute_iterator attributes_begin() const;
	attribute_iterator attributes_end() const;

	// Range-based for support
	EsXmlObjectRange<EsXmlNodeIterator> children() const;
	EsXmlObjectRange<EsXmlNamedNodeIterator> children(EsString::const_pointer name) const;
	EsXmlObjectRange<EsXmlAttributeIterator> attributes() const;

	// Get node offset in parsed file/string (in EsString::value_type units) for debugging purposes
	ptrdiff_t offsetDebugGet() const;

	// Get hash value (unique for handles to the same object)
	size_t hashGet() const;

	// Get internal pointer
	EsXmlNodeStruct* internalObjectGet() const;
};

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
// Borland C++ workaround
ESCORE_FUNC( bool, operator&&(const EsXmlNode& lhs, bool rhs) );
ESCORE_FUNC( bool, operator||(const EsXmlNode& lhs, bool rhs) );
#endif

// A helper for working with text inside PCDATA nodes
class ESCORE_CLASS EsXmlText
{
	friend class EsXmlNode;

	EsXmlNodeStruct* _root;

	typedef void (*unspecified_bool_type)(EsXmlText***);

	explicit EsXmlText(EsXmlNodeStruct* root);

	EsXmlNodeStruct* _data_new();
	EsXmlNodeStruct* _data() const;

public:
	// Default constructor. Constructs an empty object.
	EsXmlText();

	// Safe bool conversion operator
	operator unspecified_bool_type() const;

	// Borland C++ workaround
	bool operator!() const;

	// Check if text object is empty
	bool empty() const;

	// Get text, or "" if object is empty
	EsString::const_pointer get() const;

	// Get text, or the default value if object is empty
	EsString::const_pointer asString( const EsString& def = EsString::null() ) const;

	// Get text as a number, or the default value if conversion did not succeed or object is empty
	int asInt(int def = 0) const;
	unsigned int asUint(unsigned int def = 0) const;
	double asDouble(double def = 0) const;
	float asFloat(float def = 0) const;
	long long asLLong(long long def = 0) const;
	unsigned long long asULLong(unsigned long long def = 0) const;

	// Get text as bool (returns true if first character is in '1tTyY' set), or the default value if object is empty
	bool asBool(bool def = false) const;

	// Set text (returns false if object is empty or there is not enough memory)
	bool set(EsString::const_pointer rhs);

	// Set text with type conversion (numbers are converted to strings, boolean is converted to "true"/"false")
	bool set(int rhs);
	bool set(unsigned int rhs);
	bool set(double rhs);
	bool set(bool rhs);
	bool set(long long rhs);
	bool set(unsigned long long rhs);

	// Set text (equivalent to set without error checking)
	EsXmlText& operator=(EsString::const_pointer rhs);
	EsXmlText& operator=(int rhs);
	EsXmlText& operator=(unsigned int rhs);
	EsXmlText& operator=(double rhs);
	EsXmlText& operator=(bool rhs);
	EsXmlText& operator=(long long rhs);
	EsXmlText& operator=(unsigned long long rhs);

	// Get the data node (xmlNodePcdata or xmlNodeCdata) for this object
	EsXmlNode data() const;
};

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
// Borland C++ workaround
ESCORE_FUNC( bool, operator&&(const EsXmlText& lhs, bool rhs) );
ESCORE_FUNC( bool, operator||(const EsXmlText& lhs, bool rhs) );
#endif

// Child node iterator (a bidirectional iterator over a collection of EsXmlNode)
class ESCORE_CLASS EsXmlNodeIterator
{
	friend class EsXmlNode;

private:
	mutable EsXmlNode _wrap;
	EsXmlNode _parent;

	EsXmlNodeIterator(EsXmlNodeStruct* ref, EsXmlNodeStruct* parent);

public:
	// Iterator traits
	typedef ptrdiff_t difference_type;
	typedef EsXmlNode value_type;
	typedef EsXmlNode* pointer;
	typedef EsXmlNode& reference;
	typedef std::bidirectional_iterator_tag iterator_category;

	// Default constructor
	EsXmlNodeIterator();

	// Construct an iterator which points to the specified node
	EsXmlNodeIterator(const EsXmlNode& node);

	// Iterator operators
	bool operator==(const EsXmlNodeIterator& rhs) const;
	bool operator!=(const EsXmlNodeIterator& rhs) const;

	EsXmlNode& operator*() const;
	EsXmlNode* operator->() const;

	const EsXmlNodeIterator& operator++();
	EsXmlNodeIterator operator++(int);

	const EsXmlNodeIterator& operator--();
	EsXmlNodeIterator operator--(int);
};

// Attribute iterator (a bidirectional iterator over a collection of EsXmlAttribute)
class ESCORE_CLASS EsXmlAttributeIterator
{
	friend class EsXmlNode;

private:
	mutable EsXmlAttribute _wrap;
	EsXmlNode _parent;

	EsXmlAttributeIterator(EsXmlAttributeStruct* ref, EsXmlNodeStruct* parent);

public:
	// Iterator traits
	typedef ptrdiff_t difference_type;
	typedef EsXmlAttribute value_type;
	typedef EsXmlAttribute* pointer;
	typedef EsXmlAttribute& reference;
	typedef std::bidirectional_iterator_tag iterator_category;

	// Default constructor
	EsXmlAttributeIterator();

	// Construct an iterator which points to the specified attribute
	EsXmlAttributeIterator(const EsXmlAttribute& attr, const EsXmlNode& parent);

	// Iterator operators
	bool operator==(const EsXmlAttributeIterator& rhs) const;
	bool operator!=(const EsXmlAttributeIterator& rhs) const;

	EsXmlAttribute& operator*() const;
	EsXmlAttribute* operator->() const;

	const EsXmlAttributeIterator& operator++();
	EsXmlAttributeIterator operator++(int);

	const EsXmlAttributeIterator& operator--();
	EsXmlAttributeIterator operator--(int);
};

// Named node range helper
class ESCORE_CLASS EsXmlNamedNodeIterator
{
	friend class EsXmlNode;

public:
	// Iterator traits
	typedef ptrdiff_t difference_type;
	typedef EsXmlNode value_type;
	typedef EsXmlNode* pointer;
	typedef EsXmlNode& reference;
	typedef std::bidirectional_iterator_tag iterator_category;

	// Default constructor
	EsXmlNamedNodeIterator();

	// Construct an iterator which points to the specified node
	EsXmlNamedNodeIterator(const EsXmlNode& node, EsString::const_pointer name);

	// Iterator operators
	bool operator==(const EsXmlNamedNodeIterator& rhs) const;
	bool operator!=(const EsXmlNamedNodeIterator& rhs) const;

	EsXmlNode& operator*() const;
	EsXmlNode* operator->() const;

	const EsXmlNamedNodeIterator& operator++();
	EsXmlNamedNodeIterator operator++(int);

	const EsXmlNamedNodeIterator& operator--();
	EsXmlNamedNodeIterator operator--(int);

private:
	mutable EsXmlNode _wrap;
	EsXmlNode _parent;
	EsString::const_pointer _name;

	EsXmlNamedNodeIterator(EsXmlNodeStruct* ref, EsXmlNodeStruct* parent, EsString::const_pointer name);
};

// Abstract tree walker class (see EsXmlNode::traverse)
class ESCORE_CLASS EsXmlTreeWalker
{
	friend class EsXmlNode;

private:
	int _depth;

protected:
	// Get current traversal depth
	int depth() const;

public:
	EsXmlTreeWalker();
	virtual ~EsXmlTreeWalker();

	// Callback that is called when traversal begins
	virtual bool begin(EsXmlNode& node);

	// Callback that is called for each node traversed
	virtual bool for_each(EsXmlNode& node) = 0;

	// Callback that is called when traversal ends
	virtual bool end(EsXmlNode& node);
};

// Document class (DOM tree root)
class ESCORE_CLASS EsXmlDocument: public EsXmlNode
{
private:
	EsString::value_type* _buffer;

	char _memory[192];

	// Non-copyable semantics
	EsXmlDocument(const EsXmlDocument&);
	const EsXmlDocument& operator=(const EsXmlDocument&);

	void create();
	void destroy();

public:
	// Default constructor, makes empty document
	EsXmlDocument();

	// Destructor, invalidates all node/attribute handles to this document
	~EsXmlDocument();

	// Removes all nodes, leaving the empty document
	void reset();

	// Removes all nodes, then copies the entire contents of the specified document
	void reset(const EsXmlDocument& proto);

	// Load document from zero-terminated string. Optional wchar encoding conversions are applied.
	EsXmlParseResult load(const EsString& str, unsigned int options = xmlParseDefault);

	// Load document from file
	EsXmlParseResult loadFile(const EsString& path, unsigned int options = xmlParseDefault, EsXmlEncoding encoding = xmlEncodingAuto);

	// Load document from buffer. Copies/converts the buffer, so it may be deleted or changed after the function returns.
	EsXmlParseResult loadBuffer(const void* contents, size_t size, unsigned int options = xmlParseDefault, EsXmlEncoding encoding = xmlEncodingAuto);

	// Load document from buffer, using the buffer for in-place parsing (the buffer is modified and used for storage of document data).
	// You should ensure that buffer data will persist throughout the document's lifetime, and free the buffer memory manually once document is destroyed.
	EsXmlParseResult loadBufferInplace(void* contents, size_t size, unsigned int options = xmlParseDefault, EsXmlEncoding encoding = xmlEncodingAuto);

	// Load document from buffer, using the buffer for in-place parsing (the buffer is modified and used for storage of document data).
	// You should allocate the buffer with pugixml allocation function; document will free the buffer when it is no longer needed (you can't use it anymore).
	EsXmlParseResult loadBufferInplaceOwn(void* contents, size_t size, unsigned int options = xmlParseDefault, EsXmlEncoding encoding = xmlEncodingAuto);

	// Save XML document to writer (semantics is slightly different from EsXmlNode::print, see documentation for details).
	void save(EsXmlWriterIntf& writer, const EsString& indent = EsString::tab(), unsigned int flags = xmlFormatDefault, EsXmlEncoding encoding = xmlEncodingAuto) const;

	// Save XML to file
	bool saveFile(const EsString& path, const EsString& indent = EsString::tab(), unsigned int flags = xmlFormatDefault, EsXmlEncoding encoding = xmlEncodingAuto) const;

	// Get document element
	EsXmlNode elementGet() const;
};

#ifndef ES_XML_NO_XPATH
// XPath query return type
enum EsXmlXpathValueType
{
	xpathTypeNone,			// Unknown type (query failed to compile)
	xpathTypeNodeSet,		// Node set (EsXmlXpathNodeSet)
	xpathTypeNumber,	  // Number
	xpathTypeString,	  // String
	xpathTypeBoolean	  // Boolean
};

// XPath parsing result
struct ESCORE_CLASS EsXmlXpathParseResult
{
	// Error message (0 if no error)
	EsString::const_pointer error;

	// Last parsed offset (in EsString::value_type units from string start)
	ptrdiff_t offset;

	// Default constructor, initializes object to failed state
	EsXmlXpathParseResult();

	// Cast to bool operator
	operator bool() const;

	// Get error description
	EsString::const_pointer descriptionGet() const;
};

// A single XPath variable
class ESCORE_CLASS EsXmlXpathVariable
{
	friend class EsXmlXpathVariableSet;

protected:
	EsXmlXpathValueType _type;
	EsXmlXpathVariable* _next;

	EsXmlXpathVariable();

	// Non-copyable semantics
	EsXmlXpathVariable(const EsXmlXpathVariable&);
	EsXmlXpathVariable& operator=(const EsXmlXpathVariable&);

public:
	// Get variable name
	EsString::const_pointer nameGet() const;

	// Get variable type
	EsXmlXpathValueType typeGet() const;

	// Get variable value; no type conversion is performed, default value (false, NaN, empty string, empty node set) is returned on type mismatch error
	bool booleanGet() const;
	double numberGet() const;
	EsString::const_pointer stringGet() const;
	const EsXmlXpathNodeSet& nodesetGet() const;

	// Set variable value; no type conversion is performed, false is returned on type mismatch error
	bool set(bool value);
	bool set(double value);
	bool set(EsString::const_pointer value);
	bool set(const EsXmlXpathNodeSet& value);
};

// A set of XPath variables
class ESCORE_CLASS EsXmlXpathVariableSet
{
private:
	EsXmlXpathVariable* _data[64];

	// Non-copyable semantics
	EsXmlXpathVariableSet(const EsXmlXpathVariableSet&);
	EsXmlXpathVariableSet& operator=(const EsXmlXpathVariableSet&);

	EsXmlXpathVariable* find(EsString::const_pointer name) const;

public:
	// Default constructor/destructor
	EsXmlXpathVariableSet();
	~EsXmlXpathVariableSet();

	// Add a new variable or get the existing one, if the types match
	EsXmlXpathVariable* add(EsString::const_pointer name, EsXmlXpathValueType type);

	// Set value of an existing variable; no type conversion is performed, false is returned if there is no such variable or if types mismatch
	bool set(EsString::const_pointer name, bool value);
	bool set(EsString::const_pointer name, double value);
	bool set(EsString::const_pointer name, EsString::const_pointer value);
	bool set(EsString::const_pointer name, const EsXmlXpathNodeSet& value);

	// Get existing variable by name
	EsXmlXpathVariable* get(EsString::const_pointer name);
	const EsXmlXpathVariable* get(EsString::const_pointer name) const;
};

// A compiled XPath query object
class ESCORE_CLASS EsXmlXpathQuery
{
private:
	void* _impl;
	EsXmlXpathParseResult _result;

	typedef void (*unspecified_bool_type)(EsXmlXpathQuery***);

	// Non-copyable semantics
	EsXmlXpathQuery(const EsXmlXpathQuery&);
	EsXmlXpathQuery& operator=(const EsXmlXpathQuery&);

public:
	// Construct a compiled object from XPath expression.
	// Throws EsXmlXpathException on compilation errors.
	explicit EsXmlXpathQuery(EsString::const_pointer query, EsXmlXpathVariableSet* variables = 0);

	// Destructor
	~EsXmlXpathQuery();

	// Get query expression return type
	EsXmlXpathValueType returnTypeGet() const;

	// Evaluate expression as boolean value in the specified context; performs type conversion if necessary.
	// Throws std::bad_alloc on out of memory errors.
	bool asBool(const EsXmlXpathNode& n) const;

	// Evaluate expression as double value in the specified context; performs type conversion if necessary.
	// Throws std::bad_alloc on out of memory errors.
	double asNumber(const EsXmlXpathNode& n) const;

	// Evaluate expression as string value in the specified context; performs type conversion if necessary.
	// Throws std::bad_alloc on out of memory errors.
	EsString asString(const EsXmlXpathNode& n) const;

	// Evaluate expression as string value in the specified context; performs type conversion if necessary.
	// At most capacity characters are written to the destination buffer, full result size is returned (includes terminating zero).
	// Throws std::bad_alloc on out of memory errors.
	size_t asString(EsString::value_type* buffer, size_t capacity, const EsXmlXpathNode& n) const;

	// Evaluate expression as node set in the specified context.
	// Throws EsXpathException on type mismatch and std::bad_alloc on out of memory errors.
	EsXmlXpathNodeSet asNodeSet(const EsXmlXpathNode& n) const;

	// Get parsing result (used to get compilation errors in PUGIXML_NO_EXCEPTIONS mode)
	const EsXmlXpathParseResult& resultGet() const;

	// Safe bool conversion operator
	operator unspecified_bool_type() const;

	// Borland C++ workaround
	bool operator!() const;
};

// XPath exception class
class ESCORE_CLASS EsXmlXpathException : public EsException
{
public:
	// Construct exception from parse result
	explicit EsXmlXpathException(const EsXmlXpathParseResult& result);
	static void Throw(const EsXmlXpathParseResult& result);
};

// XPath node class (either EsXmlNode or EsXmlAttribute)
class ESCORE_CLASS EsXmlXpathNode
{
private:
	EsXmlNode _node;
	EsXmlAttribute _attribute;

	typedef void (*unspecified_bool_type)(EsXmlXpathNode***);

public:
	// Default constructor; constructs empty XPath node
	EsXmlXpathNode();

	// Construct XPath node from XML node/attribute
	EsXmlXpathNode(const EsXmlNode& node);
	EsXmlXpathNode(const EsXmlAttribute& attribute, const EsXmlNode& parent);

	// Get node/attribute, if any
	EsXmlNode nodeGet() const;
	EsXmlAttribute attributeGet() const;

	// Get parent of contained node/attribute
	EsXmlNode parentGet() const;

	// Safe bool conversion operator
	operator unspecified_bool_type() const;

	// Borland C++ workaround
	bool operator!() const;

	// Comparison operators
	bool operator==(const EsXmlXpathNode& n) const;
	bool operator!=(const EsXmlXpathNode& n) const;
};

#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND
// Borland C++ workaround
ESCORE_FUNC( bool, operator&&(const EsXmlXpathNode& lhs, bool rhs) );
ESCORE_FUNC( bool, operator||(const EsXmlXpathNode& lhs, bool rhs) );
#endif

// A fixed-size collection of XPath nodes
class ESCORE_CLASS EsXmlXpathNodeSet
{
public:
	// Collection type
	enum type_t
	{
		type_unsorted,			// Not ordered
		type_sorted,			// Sorted by document order (ascending)
		type_sorted_reverse		// Sorted by document order (descending)
	};

	// Constant iterator type
	typedef const EsXmlXpathNode* const_iterator;

	// Default constructor. Constructs empty set.
	EsXmlXpathNodeSet();

	// Constructs a set from iterator range; data is not checked for duplicates and is not sorted according to provided type, so be careful
	EsXmlXpathNodeSet(const_iterator begin, const_iterator end, type_t type = type_unsorted);

	// Destructor
	~EsXmlXpathNodeSet();

	// Copy constructor/assignment operator
	EsXmlXpathNodeSet(const EsXmlXpathNodeSet& ns);
	EsXmlXpathNodeSet& operator=(const EsXmlXpathNodeSet& ns);

	// Get collection type
	type_t typeGet() const;

	// Get collection size
	size_t size() const;

	// Indexing operator
	const EsXmlXpathNode& operator[](size_t index) const;

	// Collection iterators
	const_iterator begin() const;
	const_iterator end() const;

	// Sort the collection in ascending/descending order by document order
	void sort(bool reverse = false);

	// Get first node in the collection by document order
	EsXmlXpathNode first() const;

	// Check if collection is empty
	bool empty() const;

private:
	type_t _type;

	EsXmlXpathNode _storage;

	EsXmlXpathNode* _begin;
	EsXmlXpathNode* _end;

	void _assign(const_iterator begin, const_iterator end);
};
#endif

namespace EsXml
{
// Convert wide string to UTF8
ESCORE_FUNC( EsByteString, as_utf8(const ES_WCHAR* str) );
ESCORE_FUNC( EsByteString, as_utf8(const EsWideString& str) );

// Convert UTF8 to wide string
ESCORE_FUNC( EsWideString, as_wide(const char* str) );
ESCORE_FUNC( EsWideString, as_wide(const EsByteString& str) );

// Memory allocation function interface; returns pointer to allocated memory or NULL on failure
typedef void* (*allocation_function)(size_t size);

// Memory deallocation function interface
typedef void (*deallocation_function)(void* ptr);

// Override default memory management functions. All subsequent allocations/deallocations will be performed via supplied functions.
ESCORE_FUNC( void, set_memory_management_functions(allocation_function allocate, deallocation_function deallocate) );

// Get current memory management functions
ESCORE_FUNC( allocation_function, get_memory_allocation_function() );
ESCORE_FUNC( deallocation_function, get_memory_deallocation_function() );
}

#if (defined(_MSC_VER) || defined(__ICC))
namespace std
{
	// Workarounds for (non-standard) iterator category detection for older versions (MSVC7/IC8 and earlier)
ESCORE_FUNC( std::bidirectional_iterator_tag, _Iter_cat(const EsXmlNodeIterator&) );
ESCORE_FUNC( std::bidirectional_iterator_tag, _Iter_cat(const EsXmlAttributeIterator&) );
ESCORE_FUNC( std::bidirectional_iterator_tag, _Iter_cat(const EsXmlNamedNodeIterator&) );
}
#endif

#if defined(__SUNPRO_CC)
namespace std
{
	// Workarounds for (non-standard) iterator category detection
ESCORE_FUNC( std::bidirectional_iterator_tag, __iterator_category(const EsXmlNodeIterator&) );
ESCORE_FUNC( std::bidirectional_iterator_tag, __iterator_category(const EsXmlAttributeIterator&) );
ESCORE_FUNC( std::bidirectional_iterator_tag, __iterator_category(const EsXmlNamedNodeIterator&) );
}
#endif

#endif // _es_xml_h_

/**
 * Copyright (c) 2006-2014 Arseny Kapoulkine
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */
