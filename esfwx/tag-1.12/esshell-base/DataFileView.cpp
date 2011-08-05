#include <stdafx.h>
#pragma hdrstop

#include "DataFileView.h"
#include <escore/DataXmlConversion.h>
#include <wx/wfstream.h>

// data view panel
DataFileView::DataFileView(wxWindow* parent, const wxString& fileName) :
wxPanel(parent),
m_view(0),
m_fileName(fileName)
{
	wxBoxSizer* contents;
	contents = new wxBoxSizer( wxVERTICAL );
	m_view = new wxHtmlWindow( this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxHW_SCROLLBAR_AUTO );
	contents->Add( m_view, 1, wxEXPAND, 5 );
	SetSizer( contents );
	Layout();

	bool ok = m_doc.Load(fileName);
	if( ok )
	{
		m_html = toHtml();
		ok = m_view->SetPage( m_html );
	}
}

// xml contents-to-html convertor
wxString DataFileView::toHtml() const
{
	return DataXmlConversion::toXhtml(m_doc);
}

// save data contents as xml or html
void DataFileView::saveXml(const wxString& dest) const
{
	m_doc.Save(dest);	
}

void DataFileView::saveHtml(const wxString& dest) const
{
	wxFile html( dest, wxFile::write );
	html.Write( m_html );
}

void DataFileView::saveCsv(const wxString& dest) const
{
	wxFile csv( dest, wxFile::write );
	csv.Write( DataXmlConversion::toCsv(m_doc) );	
}
