#include <stdafx.h>
#pragma hdrstop

#include "DataXmlConversion.h"
#include <wx/sstream.h>

// data xml to xhtml convertor service
// 
// data view colors
#define ODD_ROW_BG		wxT("#F0F0F0")
#define EVEN_ROW_BG		wxT("#FFFFFF")
#define TOC_BG				wxT("#FFF0F0")
#define SESS_BG				wxT("#F0FBFF")
#define SESS_HDR_BG		wxT("#FBFFFB")
#define SESS_DATA_BG	wxT("#FBFBFF")

// helper containers
typedef std::list< std::pair<wxString, const wxXmlNode*> > SessionNodesList;

// enumerate all session nodes in data document
static SessionNodesList enumSessions(const wxXmlNode* root)
{
	SessionNodesList sessions;
	
	// find the first session node
	wxASSERT(root);
	const wxXmlNode* node = root->GetChildren();
	// remember all session nodes for further processing
	int idx = 1;
	while( node )
	{
		if( node->GetName() == wxT("session") )
		{
			sessions.push_back( std::make_pair(wxString::Format(_("Session %d:"), idx), node) );
			++idx;
		}
		node = node->GetNext();
	}
	
	return sessions;
}

// find child xml node
static const wxXmlNode* xmlFindChildNode(const wxXmlNode* parent, const wxString& name)
{
	// find main header node
	const wxXmlNode* node = parent->GetChildren();
	while( node && node->GetName() != name )
		node = node->GetNext();
		
	return node;
}

// create and return html table element
static wxXmlNode* xhtmlCreateTable(const wxString& width = wxT("100%"), int border = 0, int cellpadding = 0, int cellspacing = 4)
{
	wxXmlNode* result = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("table"));
	wxASSERT(result);
	result->AddAttribute(wxT("width"), width);
	EsVariant var = border;
	result->AddAttribute(wxT("border"), var.asString());
	var = cellpadding;
	result->AddAttribute(wxT("cellpadding"), var.asString());
	var = cellspacing;
	result->AddAttribute(wxT("cellspacing"), var.asString());
	return result;
}

// create and return html anchor element
static wxXmlNode* xhtmlCreateAnchor(const wxString& name, const wxString& text = wxEmptyString)
{
	wxXmlNode* result = text.empty() ? new wxXmlNode(wxXML_ELEMENT_NODE, wxT("a")) : 
		DataFileParser::createSimpleElement(wxT("a"), text);
	wxASSERT(result);
	result->AddAttribute(wxT("name"), name);
	return result;
}

// create and return link to named anchor
static wxXmlNode* xhtmlCreateLink(const wxString& anchor, const wxString& text)
{
	wxXmlNode* result = DataFileParser::createSimpleElement(wxT("a"), text);
	wxASSERT(result);
	result->AddAttribute(wxT("href"), wxT("#")+anchor);
	return result;	
}

// create html document's head and body
static void xhtmlCreateHeadAndBody(wxXmlDocument& out, wxXmlNode** body)
{
	wxASSERT(out.IsOk());
	wxXmlNode* root = out.GetRoot();
	
	wxXmlNode* head = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("head"));
	wxASSERT(head);
	root->AddChild(head);
	
	*body = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("body"));
	wxASSERT(*body);
	head->SetNext(*body);
}

// create table of contents from session nodes
static void xhtmlCreateTOC(const wxXmlNode& root, const SessionNodesList& sessions, wxXmlNode* parent)
{
	// anchor TOC header
	DataFileParser::addNode(parent, xhtmlCreateAnchor(wxT("TOC")));
	wxXmlNode* hdr = DataFileParser::createSimpleElement(wxT("h1"), _("Contents:"));
	DataFileParser::addNode(parent, hdr);	
	// process all session nodes to produce TOC (as table)
	wxXmlNode* toc = xhtmlCreateTable();
	int idx = 1;
	for(SessionNodesList::const_iterator cit = sessions.begin(); cit != sessions.end(); ++cit)
	{
		wxXmlNode* row = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("tr"));
		wxASSERT(row);
		// mark different rows with different background colors
		row->AddAttribute(wxT("bgcolor"), idx%2 ? ODD_ROW_BG : EVEN_ROW_BG);
		wxString sessId = (*cit).second->GetAttribute(wxT("id"), _("Non-marked session"));
		wxXmlNode* cell = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("td"));
		wxASSERT(cell);
		wxXmlNode* sessLink = xhtmlCreateLink(EsString::makeSafeFileName((*cit).first, wxT(" -")).c_str(), (*cit).first);
		wxASSERT(sessLink);
		DataFileParser::addNode(cell, sessLink);
		DataFileParser::addNode(row, cell);
		cell = DataFileParser::createSimpleElement(wxT("td"), sessId);
		wxASSERT(cell);
		DataFileParser::addNode(row, cell);
		DataFileParser::addNode(toc, row);
	}
	DataFileParser::addNode(parent, toc);
}

// create session header
static void xhtmlCreateSessionHeader(const wxXmlNode* session, wxXmlNode* parent)
{
	// find main header node
	const wxXmlNode* node = xmlFindChildNode(session, wxT("main_header"));
	
	if( node )
	{
		DataFileParser::addNode(parent, DataFileParser::createSimpleElement(wxT("h2"), _("Header:")));
		wxXmlNode* hdr = xhtmlCreateTable();
		// get header class info
		const ClassInfo* info = ClassInfo::getClassInfo( 
			node->GetAttribute(wxT("class"), wxEmptyString) );
		// handle each child node
		int idx = 1;
		node = node->GetChildren();
		while( node )
		{
			wxString prop = node->GetName();
			wxString propVal = node->GetNodeContent();
			wxASSERT( !propVal.empty() );
			if( info )
			{
				const PropertyInfo* propInfo = info->findPropertyInfo(prop);
				if( propInfo )
					prop = propInfo->getDescr().c_str();
			}
			wxXmlNode* row = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("tr"));
			wxASSERT(row);
			// mark different rows with different background colors
			row->AddAttribute(wxT("bgcolor"), idx%2 ? ODD_ROW_BG : EVEN_ROW_BG);
			wxXmlNode* cell = DataFileParser::createSimpleElement(wxT("td"), prop);
			DataFileParser::addNode( row, cell );
			cell = DataFileParser::createSimpleElement(wxT("td"), propVal);
			DataFileParser::addNode( row, cell );
			DataFileParser::addNode( hdr, row );
			node = node->GetNext();
			++idx;
		}
		DataFileParser::addNode(parent, hdr);
	}
}

// create data headers row
static void xhtmlCreateDataHeadersRow(wxXmlNode* dataTbl, const wxXmlNode* dataNode, const ClassInfo* colsInfo)
{
	wxXmlNode* hdrs = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("tr"));
	wxASSERT(hdrs);
	
	// enumerate data children nodes, extracting column titles from element names
	const wxXmlNode* field = dataNode->GetChildren();
	while( field )
	{
		wxString col = field->GetName();
		if( colsInfo )
		{
			const PropertyInfo* colInfo = colsInfo->findPropertyInfo(col);
			if( colInfo )
				col = colInfo->getDescr().c_str();
		}
		DataFileParser::addNode(hdrs, DataFileParser::createSimpleElement(wxT("th"), col));
		field	= field->GetNext();
	}
	hdrs->AddAttribute(wxT("bgcolor"), ODD_ROW_BG);
	DataFileParser::addNode(dataTbl, hdrs);
}

// create the data row
static void	xhtmlCreateDataRow(wxXmlNode* dataTbl, const wxXmlNode* dataNode, int rowIdx)
{
	wxXmlNode* row = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("tr"));
	wxASSERT(row);
	
	// enumerate data children nodes, extracting column titles from element names
	const wxXmlNode* field = dataNode->GetChildren();
	while( field )
	{
		// get field value from element
		DataFileParser::addNode(row, DataFileParser::createSimpleElement(wxT("td"), field->GetNodeContent()));
		field	= field->GetNext();
	}
	// mark different rows with different background colors
	row->AddAttribute(wxT("bgcolor"), rowIdx%2 ? ODD_ROW_BG : EVEN_ROW_BG);
	DataFileParser::addNode(dataTbl, row);
}

// create session data nodes
static void xhtmlCreateSessionDataNodes(const wxXmlNode* session, wxXmlNode* parent)
{
	// locate data nodes container
	const wxXmlNode* node = xmlFindChildNode( session, wxT("nodes") );
	
	if( node )
	{
		// data section header
		DataFileParser::addNode(parent, DataFileParser::createSimpleElement(wxT("h2"), _("Data:")));
		// get header class info
		const ClassInfo* info = ClassInfo::getClassInfo( node->GetAttribute(wxT("class"), wxEmptyString) );
		node = node->GetChildren();
		wxXmlNode* dataTbl = 0;
		if( node )
		{
			wxASSERT(node->GetName() == wxT("node"));
			// create table with column headers
			dataTbl = xhtmlCreateTable();
			xhtmlCreateDataHeadersRow(dataTbl, node, info);
		}
				
		// handle each child node
		int idx = 1;
		while( node )
		{
			wxASSERT(node->GetName() == wxT("node"));
			// create the data row
			wxASSERT(dataTbl);
			xhtmlCreateDataRow(dataTbl, node, idx);
			node = node->GetNext();
			++idx;
		}
		
		if( dataTbl )
		 DataFileParser::addNode(parent, dataTbl);		
	}
}

// create session element with contents
static void xhtmlCreateSession(SessionNodesList::const_iterator cit, wxXmlNode* parent)
{
	DataFileParser::addNode(parent, new wxXmlNode(wxXML_ELEMENT_NODE, wxT("hr")));
	wxXmlNode* cap = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("h1"));
	DataFileParser::addNode(cap, xhtmlCreateAnchor(EsString::makeSafeFileName((*cit).first, wxT(" -")).c_str(), (*cit).first));
	DataFileParser::addNode(parent, cap);
	
	// add data header
	wxXmlNode* block = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("div"));
	wxASSERT(block);
	block->AddAttribute(wxT("style"), wxT("background:") SESS_HDR_BG);
	xhtmlCreateSessionHeader( (*cit).second, block );
	DataFileParser::addNode(parent, block);
	// add data nodes
	block = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("div"));
	wxASSERT(block);	
	block->AddAttribute(wxT("style"), wxT("background:") SESS_DATA_BG);
	xhtmlCreateSessionDataNodes( (*cit).second, block );
	DataFileParser::addNode(parent, block);
}

wxString DataXmlConversion::toXhtml(const wxXmlDocument& doc)
{
	wxString result;
	
	// convert xml document to html contained in string
	if( doc.IsOk() )
	{
		wxXmlNode* root = doc.GetRoot();
		// create root element for html
		wxXmlDocument out;
		out.SetRoot( new wxXmlNode(wxXML_ELEMENT_NODE, wxT("html")) );		
		// initialize document's head
		wxXmlNode* body = 0;
		xhtmlCreateHeadAndBody(out, &body);
		// create table of contents from session nodes
		SessionNodesList sessions = enumSessions(root);
		wxXmlNode* block = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("div"));
		wxASSERT(block);
		block->AddAttribute(wxT("style"), wxT("background:") TOC_BG);
		xhtmlCreateTOC(*root, sessions, block);
		DataFileParser::addNode(body, block);
		// create session elements
		for(SessionNodesList::const_iterator cit = sessions.begin(); cit != sessions.end(); ++cit)
		{
			block = new wxXmlNode(wxXML_ELEMENT_NODE, wxT("div"));
			wxASSERT(block);
			block->AddAttribute(wxT("style"), wxT("background:") SESS_BG);			
			xhtmlCreateSession(cit, block);
			DataFileParser::addNode(body, block);
		}
		// get resulting xhtml as string
		wxStringOutputStream text;
		out.Save(text);
		result = text.GetString();
	}
	
	return result;
}

// data xml to csv conversion service
//
#define SEP			wxT(';')
#define NEWLINE	wxT("\r\n")

static wxString toCsvString(const wxString& src)
{
	wxString result = src;
	if( wxString::npos != result.find(SEP) )
		result = EsString::toString(result, EsString::StrQuote|EsString::StrNoCEscape|EsString::StrKeepSideBlanks).c_str();
	
	return result;	
}

static wxString csvCreateSessionHeader( SessionNodesList::const_iterator cit )
{
	const wxXmlNode* session = (*cit).second;
	wxASSERT(session);
	wxString result;
	
	// find main header node
	const wxXmlNode* node = xmlFindChildNode(session, wxT("main_header") );
	if( node )
	{
		result << toCsvString( cit->first.c_str() ) << SEP <<
			toCsvString( 
				session->GetAttribute(wxT("id"), wxEmptyString)
		) << NEWLINE;
		
		// get header class info
		const ClassInfo* info = ClassInfo::getClassInfo( 
			node->GetAttribute(wxT("class"), wxEmptyString)
		);
		// handle each child node
		node = node->GetChildren();
		while( node )
		{
			wxString prop = node->GetName();
			wxString propVal = node->GetNodeContent();
			wxASSERT( !propVal.empty() );
			if( info )
			{
				const PropertyInfo* propInfo = info->findPropertyInfo(prop);
				if( propInfo )
					prop = propInfo->getDescr().c_str();
			}
						
			result << toCsvString(prop) << SEP <<
				toCsvString(propVal) << NEWLINE;
			
			node = node->GetNext();
		}
	}	
	
	return result;
}

static wxString csvCreateDataHeadersRow(const wxXmlNode* dataNode, const ClassInfo* colsInfo)
{
	wxASSERT(dataNode);
	wxString result;
	// enumerate data children nodes, extracting column titles from element names
	const wxXmlNode* field = dataNode->GetChildren();
	while( field )
	{
		wxString col = field->GetName();
		if( colsInfo )
		{
			const PropertyInfo* colInfo = colsInfo->findPropertyInfo(col);
			if( colInfo )
				col = colInfo->getDescr().c_str();
		}
		if( !result.empty() )
			result += SEP;
		result += toCsvString(col);
		field	= field->GetNext();
	}
	if( !result.empty() )
		result += NEWLINE;
	
	return result;
}

static wxString csvCreateDataRow(const wxXmlNode* dataNode)
{
	wxString result;
	
	const wxXmlNode* field = dataNode->GetChildren();
	while( field )
	{
		// get field value from element
		if( !result.empty() )
			result += SEP;
		// quote value if it contains space or comma
		result += toCsvString( field->GetNodeContent() );
		field	= field->GetNext();
	}
	if( !result.empty() )
		result += NEWLINE;
	
	return result;
}

static wxString csvCreateSessionDataNodes( const wxXmlNode* session )
{
	wxASSERT(session);
	wxString result;
	
	// locate data nodes container
	const wxXmlNode* node = xmlFindChildNode( session, wxT("nodes") );
	
	if( node )
	{
		// get header class info
		const ClassInfo* info = ClassInfo::getClassInfo( 
			node->GetAttribute(wxT("class"), wxEmptyString)
		);
		node = node->GetChildren();
		wxXmlNode* dataTbl = 0;
		if( node )
		{
			wxASSERT(node->GetName() == wxT("node"));
			result = csvCreateDataHeadersRow(node, info);
		}
				
		// handle each child node
		while( node )
		{
			wxASSERT(node->GetName() == wxT("node"));
			result += csvCreateDataRow(node);
			node = node->GetNext();
		}
	}	
	
	return result;
}

static wxString csvCreateSession( SessionNodesList::const_iterator cit )
{
	wxString result =	csvCreateSessionHeader( cit );
	result += csvCreateSessionDataNodes( (*cit).second );
	return result;
}

wxString DataXmlConversion::toCsv(const wxXmlDocument& doc)
{
	wxString result;
	
	SessionNodesList sessions = enumSessions( doc.GetRoot() );
	// create session elements
	for(SessionNodesList::const_iterator cit = sessions.begin(); cit != sessions.end(); ++cit)
		result += csvCreateSession(cit);
	return result;
}