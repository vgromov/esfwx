#ifndef _Data_File_Parser_h_
#define _Data_File_Parser_h_

#include <hubs/common/dataFile.h>

// read-only access to device data file structure
//
class EKOSF_CORE_CLASS DataFileParser
{
public:
	// fwd decl;
	class Page;

protected:
	// page container type
	typedef std::vector<int> PagesT;
	// misc constants
	enum {
		RESERVE_MIN_CHILDREN = 256,
		RESERVE_MIN_MAIN_HEADERS = 8,
	};

public:
	// page parser registration 
	class EKOSF_CORE_CLASS FileAccessorRegEntry
	{
	public:
		// item parsers indexes
		enum ItemParserId {
			MAIN_HDR_ITEM_PARSER,
			HDR_ITEM_PARSER,
			DATA_ITEM_PARSER,
			itemParserCount
		};
		
	protected:
		typedef std::list<const FileAccessorRegEntry*> RegistryT;

	public:
		FileAccessorRegEntry() 
		{ 
			m_itemParsers[MAIN_HDR_ITEM_PARSER] = 0; 
			m_itemParsers[HDR_ITEM_PARSER] = 0; 
			m_itemParsers[DATA_ITEM_PARSER] = 0; 
			s_registry.push_back(this); 
		}
		virtual ~FileAccessorRegEntry() { s_registry.remove(this); }

		// check if bundled entry parsers support specified main header page as well as its children
		virtual bool checkSupport(const Page& mainHdrPage) const = 0;
		// find file accessor entry in registry por specified main header page
		static const FileAccessorRegEntry* findAccessor(const Page& mainHdrPage);
		// page item parser registration helper
		FileAccessorRegEntry& add(ItemParserId id, const ClassInfo& parserInfo) 
		{ m_itemParsers[id] = &parserInfo; return *this; }
		// return page item parser given parser id && pointer to reflected data file page. return empty variant if no parser is registered
		EsVariant getItemParser(ItemParserId id, const EsReflectedClassIntf::Ptr& page, long itemIdx) const 
		{ 
			wxASSERT( id < itemParserCount ); 
			EsVariant result;
			
			if( m_itemParsers[id] )
				result = m_itemParsers[id]->classCall(c_reflectedCtrName, page.request<EsBaseIntf>(), itemIdx);
			
			return result; 
		}

	private:
		// prohibited functionality
		FileAccessorRegEntry(const FileAccessorRegEntry&);
		FileAccessorRegEntry& operator= (const FileAccessorRegEntry&);

	protected:
		// page item parser class infos
		const ClassInfo* m_itemParsers[itemParserCount];
#pragma warning(push)
#pragma warning(disable: 4251)
		static RegistryT s_registry;
#pragma warning(pop)		
	};

	// generic page data accessor
	//
	class EKOSF_CORE_CLASS Page : public EsReflectedClassIntf
	{
	public:
		// invalid page constructor
		Page();
		// copy ctor
		Page(const DataFileParser::Page& src);
		// assignment
		DataFileParser::Page& operator =(const DataFileParser::Page& src);
		
		// reflection declaration
		ES_DECL_REFLECTED_CLASS_BASE(Page)
		ES_NON_RC_INTF_MAP_BEGIN(Page)
			ES_INTF_SUPPORTS(Page, EsReflectedClassIntf)
		ES_NON_RC_INTF_MAP_END
		{ /* destroy does nothing */ }

		// generic page services (reflected)
		//
		// check page validity && integrity (CRC16)
		ES_DECL_REFLECTED_CONST_METHOD0(bool, isOk) { return m_file && m_crcOk && m_sizeOk; }
		// return index of page in file
		ES_DECL_REFLECTED_CONST_METHOD0(long, getIndex) { return m_idx; }
		// check if page is empty
		ES_DECL_REFLECTED_CONST_METHOD0(bool, isEmpty) 
		{ 
			wxASSERT(isOk()); 
			const FilePage& data = getRawData();
			return filePageHasTag(&data, c_pageTagEmpty) || data.entryCnt == 0; 
		}
		// return size of page entry
		ES_DECL_REFLECTED_CONST_METHOD0(long, getEntrySize) 
		{ 
			wxASSERT(isOk()); 
			const FilePage& data = getRawData();
			return filePageHasTag(&data, c_pageTagEmpty) ? 0 : data.entrySize; 
		}
		// return true if page is data page
		ES_DECL_REFLECTED_CONST_METHOD0(bool, isData) 
		{ 
			wxASSERT(isOk()); 
			const FilePage& data = getRawData();
			return 0 != filePageHasTag(&data, c_pageTagData); 
		}
		// return true if page is main header
		ES_DECL_REFLECTED_CONST_METHOD0(bool, isMainHeader) 
		{ 
			wxASSERT(isOk()); 
			const FilePage& data = getRawData();
			return 0 != filePageHasTag(&data, c_pageTagHdrMain); 
		}
		// return true if page is header
		ES_DECL_REFLECTED_CONST_METHOD0(bool, isHeader) 
		{ 
			wxASSERT(isOk()); 
			const FilePage& data = getRawData();
			return 0 != filePageHasTag(&data, c_pageTagHdr); 
		}
		// return page tag
		ES_DECL_REFLECTED_CONST_METHOD0(EsString, getTag);
		// access to the raw user data
		ES_DECL_REFLECTED_CONST_METHOD0(EsBinBuffer, getRawUserData);
		// access raw entry data
		ES_DECL_REFLECTED_CONST_METHOD1(EsBinBuffer, getRawEntry, long);
		// indexed opbject standard services
		//
		// return number of page entries
		ES_DECL_REFLECTED_CONST_METHOD0(long, getCount) 
		{ 
			wxASSERT(isOk()); 
			const FilePage& data = getRawData();
			return filePageHasTag(&data, c_pageTagEmpty) ? 0 : data.entryCnt; 
		}
		// return page entry item at specified index. returned is either raw entry data as binary buffer format
		// if no entry parser is registered, or reflected object - entry parser, passed by value, as embedded
		ES_DECL_REFLECTED_CONST_METHOD1(EsVariant, getItem, long);

		// page hierarchy services
		//
		// get page header. invalid page is returned if page has no header
		const Page& getHeader() const 
		{ 
			if( m_header > -1 ) 
				return m_file->m_pages[m_header]; 
			else 
				return s_invalidPage; 
		}
		// get main header for page
		const Page& getMainHeader() const;
		// return true if page has children
		bool hasChildren() const { return !m_children.empty(); }
		// get count of children pages
		size_t getChildrenCount() const { return m_children.size(); }
		// get child page at specified index
		const Page& getChild(size_t idx) const;
		// direct access to the page data
		const FilePage& getRawData() const;

	protected:		
		// normal constructor, only accessible to DataFileParser
		Page(const DataFileParser& file, size_t offs);
		// append child page. only accessible to DataFileParser
		// pageIdx - index of page in m_pages collection
		void appendChild(Page& child, int childIdx, int thisPageIdx);
	
	protected:
		const DataFileParser* m_file;
		size_t m_offs;
		size_t m_idx;
		bool m_crcOk;
		bool m_sizeOk;
		// index of the parent header page
		int m_header;
#pragma warning(push)
#pragma warning(disable: 4251)
		// children pages
		DataFileParser::PagesT m_children;
#pragma warning(pop)
		// special class value - invalid page
		static const Page s_invalidPage;
		
		friend class DataFileParser;
	};
	friend class Page;
	
public:
	DataFileParser(const EsBinBuffer& data, wxDateTime timeStamp = wxDateTime::Now(), const EsString& path = EsString::s_null);
	DataFileParser(const DataFileParser& src);
	DataFileParser& operator=(const DataFileParser& src); 	
	
	// data file attributes
	//
	const EsString& getPath() const { return m_path; }
	EsDateTime getTimeStamp() const { return m_timeStamp; }
	EsString getType() const;

	// plain data file access
	//
	// return file pages count
	size_t getPageCount() const { return m_pages.size(); }
	// random page access
	const DataFileParser::Page& getPage(size_t idx) const;

	// parsed page hierarchy access
	//
	bool hasMainHeaders() const { return !m_mainHeaders.empty(); }
	size_t getMainHeaderCount() const { return m_mainHeaders.size(); }
	const DataFileParser::Page& getMainHeader(size_t idx) const;

	// write data from parsed binary file to xml document.
	void writeTo(wxXmlDocument& out, const EsString& session = EsString::s_null) const;
	
	// static xml helpers
	//
	// add node at the end of the parent node's children
	static void addNode(wxXmlNode* parent, wxXmlNode* node);
	// create simple element node with text value
	static wxXmlNode* createSimpleElement(const EsString& name, const EsString& value);
	
protected:
	EsString m_path;
	EsDateTime m_timeStamp;
#pragma warning(push)
#pragma warning(disable: 4251)
	EsBinBuffer m_data;
	// vector of parsed data file pages
	std::vector<DataFileParser::Page> m_pages;
	PagesT m_mainHeaders;
#pragma warning(pop)
	// FileAccessorEntry search cache
	mutable const FileAccessorRegEntry* m_cache;
};

// automatic data file item accessor registrar declaration
#define DECL_ACCESSOR_ENTRY_REGISTRAR( DataTypeName ) \
class DataTypeName ##FileAccessorEntry : public DataFileParser::FileAccessorRegEntry \
{ \
protected: \
	DataTypeName ##FileAccessorEntry(); \
public: \
	virtual bool checkSupport(const DataFileParser::Page& mainHdrPage) const; \
private: \
	static DataTypeName ##FileAccessorEntry s_accessor; \
};

// automatic data file item accessor registrar implementation macros
// use in source files only
#define IMPL_ACCESSOR_ENTRY_REGISTRAR_BEGIN( DataTypeName ) \
DataTypeName ##FileAccessorEntry DataTypeName ##FileAccessorEntry::s_accessor; \
DataTypeName ##FileAccessorEntry::## DataTypeName ##FileAccessorEntry() : \
DataFileParser::FileAccessorRegEntry() {

#define MAIN_HDR_ITEM_ACCESSOR_ENTRY(ItemAccessorClass) \
	add( DataFileParser::FileAccessorRegEntry::MAIN_HDR_ITEM_PARSER, ItemAccessorClass ##::getClassInfoStatic() );

#define HDR_ITEM_ACCESSOR_ENTRY(ItemAccessorClass) \
	add( DataFileParser::FileAccessorRegEntry::HDR_ITEM_PARSER, ItemAccessorClass ##::getClassInfoStatic() );

#define DATA_ITEM_ACCESSOR_ENTRY(ItemAccessorClass) \
	add( DataFileParser::FileAccessorRegEntry::DATA_ITEM_PARSER, ItemAccessorClass ##::getClassInfoStatic() );

#define IMPL_ACCESSOR_ENTRY_REGISTRAR_END }

#define IMPL_ACCESSOR_ENTRY_SUPPORT_BEGIN( DataTypeName ) \
bool DataTypeName ##FileAccessorEntry::checkSupport(const DataFileParser::Page& mainHdrPage) const \
{ \
	wxASSERT(mainHdrPage.isOk()); \
	wxASSERT(mainHdrPage.isMainHeader());

#define IMPL_ACCESSOR_ENTRY_SUPPORT_END }

#endif // _Data_File_Parser_h_
