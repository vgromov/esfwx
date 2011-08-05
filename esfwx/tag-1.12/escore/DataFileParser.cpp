#include <stdafx.h>
#pragma hdrstop

#include <hubs/common/dataFile.h>
#include "DataFileParser.h"
#include "DataFileUtils.h"

// file accessor registry implementation
//
DataFileParser::FileAccessorRegEntry::RegistryT DataFileParser::FileAccessorRegEntry::s_registry;

// find file accessor entry in registry por specified main header page
const DataFileParser::FileAccessorRegEntry* 
DataFileParser::FileAccessorRegEntry::findAccessor(const DataFileParser::Page& mainHdrPage)
{
	for(DataFileParser::FileAccessorRegEntry::RegistryT::const_iterator cit = s_registry.begin(); 
		cit != s_registry.end(); ++cit)
	{
		if( (*cit)->checkSupport(mainHdrPage) )
			return *cit;
	}

	return 0;
}

// data file page implementation
//
const DataFileParser::Page DataFileParser::Page::s_invalidPage;

// reflection initialization
ES_DECL_NESTED_BASE_CLASS_INFO_BEGIN(DataFileParser::Page, DataFileParserPage, NO_CLASS_DESCRIPTION)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, isOk, bool_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, getIndex, long_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, isEmpty, bool_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, getEntrySize, long_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, isData, bool_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, isMainHeader, bool_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, isHeader, bool_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, getTag, EsString_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, getRawUserData, EsBinBuffer_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, getCount, long_CallConst, NO_METHOD_DESCR)
	ES_DECL_REFLECTED_METHOD_INFO_STD(DataFileParser::Page, getItem, EsVariant_CallConst_long, NO_METHOD_DESCR)
ES_DECL_CLASS_INFO_END

// invalid page constructor
DataFileParser::Page::Page() :
m_file(0),
m_offs(0),
m_idx(0),
m_crcOk(false),
m_sizeOk(false),
m_header(-1)
{}

// copy ctor
DataFileParser::Page::Page(const DataFileParser::Page& src) :
m_file(src.m_file),
m_offs(src.m_offs),
m_idx(src.m_idx),
m_crcOk(src.m_crcOk),
m_sizeOk(src.m_sizeOk),
m_header(src.m_header),
m_children(src.m_children)
{}

// normal page constructor
DataFileParser::Page::Page(const DataFileParser& file, size_t offs) :
m_file(&file),
m_offs(offs),
m_idx(offs/FilePage_SZE),
m_crcOk(false),
m_sizeOk(false),
m_header(-1)
{
	if( m_offs + FilePage_SZE > file.m_data.size() )
		EsException::Throw(0, EsException::Generic, _("Invalid data file page offset specified: %d"), m_offs);
	const FilePage* data = reinterpret_cast<const FilePage*>(&(m_file->m_data[0]) + m_offs);
	m_crcOk = 0 != filePageCrcIsOk(data);
	m_sizeOk = data->entryCnt*data->entrySize <= FileUserData_SZE;
}

// destructor
DataFileParser::Page::~Page() {}

// assignment
DataFileParser::Page& DataFileParser::Page::operator =(const DataFileParser::Page& src)
{
	m_file = src.m_file;
	m_offs = src.m_offs;
	m_idx = src.m_idx;
	m_crcOk = src.m_crcOk;
	m_sizeOk = src.m_sizeOk;
	m_header = src.m_header;
	m_children = src.m_children;
	
	return *this;
}

// get main header for page
const DataFileParser::Page& DataFileParser::Page::getMainHeader() const
{
	wxASSERT(isOk());
	if( isMainHeader() )
		return *this;
	else if( m_header > -1 )
		return m_file->m_pages[m_header].getMainHeader();
	else
		return s_invalidPage;
}

// return page tag
EsString DataFileParser::Page::getTag() const
{
	wxASSERT(isOk()); 
	return wxString(reinterpret_cast<const char*>(getRawData().tag), *wxConvCurrent, FileTag_SZE);
}

// direct access to the page data
const FilePage& DataFileParser::Page::getRawData() const
{
	wxASSERT(isOk());
	return *reinterpret_cast<const FilePage*>(&(m_file->m_data[0]) + m_offs);	
}

EsBinBuffer DataFileParser::Page::getRawUserData() const
{
	wxASSERT(isOk());
	const FilePage& data = getRawData();
	return EsBinBuffer(data.userData, data.userData+FileUserData_SZE);
}

// access raw entry data
EsBinBuffer DataFileParser::Page::getRawEntry(long idx) const 
{ 
	wxASSERT(isOk());
	const FilePage& data = getRawData();
	wxASSERT( 0 <= idx && idx < (long)data.entryCnt );
	
	const BYTE* beg = data.userData+idx*data.entrySize; 
	return EsBinBuffer(beg, beg+data.entrySize); 
}

// return page entry item at specified index. returned is either raw entry data in binary buffer format
// if no entry parser is registered, or reflected object - entry parser, passed by value, as embedded
//
EsVariant DataFileParser::Page::getItem(long idx) const
{
	wxASSERT(isOk());
	// get main header page for this page
	const Page& mainHdr = getMainHeader();
	// try to find the accessors registered for this kind of main page
	const DataFileParser::FileAccessorRegEntry* entry = 0;
	if( m_file->m_cache && m_file->m_cache->checkSupport(mainHdr) )
		entry = m_file->m_cache;
	else	
		entry = DataFileParser::FileAccessorRegEntry::findAccessor(mainHdr);
	 
	EsVariant result;
	if( entry )
	{
		m_file->m_cache = entry;
		EsReflectedClassIntf::Ptr page( static_cast<EsReflectedClassIntf*>( const_cast<Page*>(this) ) );
		if( isMainHeader() )
			result = entry->getItemParser(DataFileParser::FileAccessorRegEntry::MAIN_HDR_ITEM_PARSER, page, idx);
		else if( isHeader() )
			result = entry->getItemParser(DataFileParser::FileAccessorRegEntry::HDR_ITEM_PARSER, page, idx);
		else
			result = entry->getItemParser(DataFileParser::FileAccessorRegEntry::DATA_ITEM_PARSER, page, idx);
	}
	
	// interpret item as raw binary buffer
	if( result.isEmpty() )
		result = getRawEntry(idx);
	
	return result;
}

// get child page at specified index
const DataFileParser::Page& DataFileParser::Page::getChild(size_t idx) const
{
	wxASSERT(isOk());
	if( idx >= m_children.size() )
		EsException::Throw(0, EsException::Generic, _("Child page index %d is out of [0, %d[ range."),
			idx, m_children.size());

	return m_file->m_pages[ m_children[idx] ];
}

// append child page. only accessible to DataFileParser
void DataFileParser::Page::appendChild(Page& child, int childIdx, int thisPageIdx)
{
	// reserve some space for data pages
	if( child.isData() && m_children.capacity() < RESERVE_MIN_CHILDREN && (isHeader() || isMainHeader()) )
		m_children.reserve(RESERVE_MIN_CHILDREN);

	child.m_header = thisPageIdx;
	m_children.push_back( childIdx );
}
		
// data file implementation
//
DataFileParser::DataFileParser(const EsBinBuffer& data, 
															 wxDateTime timeStamp /*= wxDateTime::Now()*/, 
															 const EsString& path/* = EsString::s_null*/) :
m_path(path),
m_timeStamp(timeStamp),
m_data(data),
m_cache(0)
{
	size_t cnt = data.size() / FilePage_SZE;
	size_t offs = 0;
	m_pages.reserve( cnt );
	m_mainHeaders.reserve(RESERVE_MIN_MAIN_HEADERS);
	if( cnt )
	{
		// build linear pages array
		while( offs < data.size() )
		{
			m_pages.push_back( Page(*this, offs) );		
			offs += FilePage_SZE;
		}

		// build page hierarchy on top of linear array
		int header = -1;
		for( size_t idx = 0; idx < m_pages.size(); ++idx )
		{
			Page& pg = m_pages[idx];
			if( pg.isOk() ) // only include Ok pages
			{
				if( pg.isMainHeader() )
				{
					header = idx;
					m_mainHeaders.push_back(header);
				}
				else
				{
					if( header > -1 )
					{
						if( pg.isHeader() ) // secondary header page(s)
						{
							m_pages[header].appendChild(pg, idx, header);
							header = idx;
						}
						else // data page(s)
							m_pages[header].appendChild(pg, idx, header);
					}
				}
			}			
		}
	}
}

DataFileParser::DataFileParser(const DataFileParser& src) :
m_path(src.m_path),
m_timeStamp(src.m_timeStamp),
m_data(src.m_data),
m_pages(src.m_pages),
m_mainHeaders(src.m_mainHeaders),
m_cache(0)
{
	// adjust pointer to file in pages container
	for(size_t idx = 0; idx < m_pages.size(); ++idx)
		m_pages[idx].m_file = this;
}

DataFileParser& DataFileParser::operator=(const DataFileParser& src) 	
{
	m_path = src.m_path;
	m_timeStamp = src.m_timeStamp;
	m_data = src.m_data;
	m_pages = src.m_pages;
	m_mainHeaders = src.m_mainHeaders;
	m_cache = 0;
	// adjust pointer to file in pages container
	for(size_t idx = 0; idx < m_pages.size(); ++idx)
		m_pages[idx].m_file = this;
		
	return *this;
}

// return file type tag
EsString DataFileParser::getType() const
{
	EsString result;
	
	// if file is empty, return empty tag
	// if file contains only one main header, use its type tag,
	// otherwise, analyse all main headers in file
	for( size_t idx = 0; idx < m_mainHeaders.size(); ++idx )
	{
		const FilePage& page = m_pages[ m_mainHeaders[idx] ].getRawData();
		// try to extract custom tag from the first FileTag_SZE bytes of the user data
		EsString typeTag = wxString( reinterpret_cast<const char*>(page.userData), *wxConvCurrent, FileTag_SZE);
		if( !result.empty() && result != typeTag )
		{
			result = DataFile::getCompoundTypeTag();
			break;
		}
		else
			result = typeTag;
	}	
	
	return result;
}

// random page access
const DataFileParser::Page& DataFileParser::getPage(size_t idx) const
{
	if( idx >= m_pages.size() )
		EsException::Throw(0, EsException::Generic, _("Page index %d out of [0, %d[ range."),
			idx, m_pages.size());
	
	return m_pages[idx];
}

const DataFileParser::Page& DataFileParser::getMainHeader(size_t idx) const
{
	if( idx >= m_mainHeaders.size() )
		EsException::Throw(0, EsException::Generic, _("Main header page index %d out of [0, %d[ range."),
			idx, m_mainHeaders.size());
	
	return m_pages[ m_mainHeaders[idx] ];
}

// add node at the end of the parent node's children
void DataFileParser::addNode(wxXmlNode* parent, wxXmlNode* node)
{
	wxASSERT(parent);
	wxASSERT(node);
	wxXmlNode* children = parent->GetChildren();
	if( children )
	{
		wxXmlNode* lastChild = 0;
		wxXmlNode* child = children;
		while( child )
		{
			lastChild = child;
			child = child->GetNext();
		}
		
		wxASSERT(lastChild);
		lastChild->SetNext(node);
	}
	else
		parent->AddChild(node);
}

// create simple element node with text value
wxXmlNode* DataFileParser::createSimpleElement(const EsString& name, const EsString& value)
{
	wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, name);
	wxASSERT(node);
	wxXmlNode* text = new wxXmlNode(node, wxXML_TEXT_NODE, EsString::s_null, value);
	wxASSERT( text );
	
	return node;
}

// internal service to stream compound (object) properties into XML 
static void writePropertyToNode( wxXmlNode* parent, const EsReflectedClassIntf::Ptr& obj, const PropertyInfo& prop, 
	const std::locale& loc )
{
	wxASSERT(parent);
	EsVariant val = prop.get( obj.get() );
	if( val.isObject() )
	{
		EsReflectedClassIntf::Ptr propObj = val.asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(propObj);
		wxXmlNode* objNode = new wxXmlNode(wxXML_ELEMENT_NODE, prop.getName());
		wxASSERT(objNode);
		DataFileParser::addNode(parent, objNode);
		// if asString service is present
		if( propObj->hasMethod(MethodInfoKeyT(0, wxT("asString"))) ) 
		{
			wxXmlNode* text = new wxXmlNode(wxXML_TEXT_NODE, EsString::s_null, val.asString(loc));
			wxASSERT(text);
			objNode->AddChild(text);
		}
		else // otherwise, save object's properties
		{
			ClassInfo::PropertyInfosT props = propObj->getClassInfo().getPropertyInfos();
			for( ClassInfo::PropertyInfosT::const_iterator cit = props.begin(); cit != props.end(); ++cit )
				writePropertyToNode(objNode, propObj, *(*cit), loc);
		}
	}
	else
	{
		wxXmlNode* node = DataFileParser::createSimpleElement( prop.getName(), val.asString(loc) );
		DataFileParser::addNode(parent, node);
	}
}

// internal service to augment writing header|data item contained in EsVariant to XML node
static void writeItemToXml(wxXmlNode* parent, const EsVariant& item, const std::locale& loc)
{
	if( item.isObject() )
	{
		const EsReflectedClassIntf::Ptr obj = item.asExistingObject().request<EsReflectedClassIntf>();
		wxASSERT(obj);
		ClassInfo::PropertyInfosT props = obj->getClassInfo().getPropertyInfos();
		for( ClassInfo::PropertyInfosT::const_iterator cit = props.begin(); cit != props.end(); ++cit )
			writePropertyToNode(parent, obj, *(*cit), loc);
	}
	else
	{
		wxXmlNode* node = DataFileParser::createSimpleElement(wxT("raw"), item.asString(loc));
		wxASSERT(node);
		DataFileParser::addNode(parent, node);
	}
}

// write data from parsed binary file to xml document
void DataFileParser::writeTo(wxXmlDocument& out, const EsString& session /*= EsString::s_null*/) const
{
	// use currently selected application locale when exporting data to xml
	wxASSERT( EsUtilities::getAppLocale() );
	std::locale loc( EsUtilities::getAppLocale()->GetSysName().c_str() );
	wxXmlNode* root = out.GetRoot();
	if( !root )
	{ 
		// prepare document for the first time
		root = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("data"));
		wxASSERT(root);
		wxXmlNode* globalTag = createSimpleElement(wxT("tag"), getType());
		wxASSERT(globalTag);
		root->AddChild(globalTag);
		out.SetRoot(root);
	}

	// assert document's OK
	if( !out.IsOk() )
		EsException::Throw(0, EsException::Generic, _("XML document is malformed."));
	
	// set-up data session
	wxXmlNode* sess = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("session"));
	wxASSERT(sess);
	EsString sessionId;
	if( session.empty() )
		sessionId = m_timeStamp.asString();
	else
		sessionId = session;
	sess->AddAttribute(wxT("id"), sessionId);
	addNode(root, sess);
	
	for( size_t idx = 0; idx < getMainHeaderCount(); ++idx )
	{
		const Page& mainHdr = getMainHeader(idx);
		// write header 
		wxXmlNode* hdr = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("main_header"));
		wxASSERT(hdr);
		// main header is assumed to contain single entry
		EsVariant item = mainHdr.getItem(0);
		writeItemToXml(hdr, item, loc);
		if( item.isObject() )
			hdr->AddAttribute(wxT("class"), item.asExistingObject()->getClassName());
		addNode(sess, hdr);
			
		// write linked data page contents
		wxXmlNode* nodes = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("nodes"));
		wxASSERT(nodes);
		for( size_t pageIdx = 0; pageIdx < mainHdr.getChildrenCount(); ++pageIdx )
		{
			const Page& page = mainHdr.getChild(pageIdx);
			for( long itemIdx = 0; itemIdx < page.getCount(); ++itemIdx )
			{
				wxXmlNode* node = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("node"));
				wxASSERT(node);
				item = page.getItem(itemIdx);
				if( item.isObject() )
				{
					if( !nodes->HasAttribute(wxT("class")) )
						nodes->AddAttribute(wxT("class"), item.asExistingObject()->getClassName());
				}
				writeItemToXml(node, item, loc);
				addNode(nodes, node);
			}
		}
		addNode(sess, nodes);
	}
}
