#ifndef _data_file_view_h_
#define _data_file_view_h_

#include <wx/html/htmlwin.h>

// XML data file viewer
class EKOSF_SHELL_BASE_CLASS DataFileView : public wxPanel
{
public:
	DataFileView(wxWindow* parent, const wxString& fileName);
	const wxString& getFileName() const { return m_fileName; }
	// save data contents as xml, html, or CSV
	void saveXml(const wxString& dest) const;
	void saveHtml(const wxString& dest) const;
	void saveCsv(const wxString& dest) const;
	// return html contents as string
	const wxString& getHtml() const { return m_html; }
	
protected:
	// try to convert xml contents to html page
	wxString toHtml() const;

protected:
	// data members
	//
	// html viewer
	wxHtmlWindow* m_view;
	// data file name
	wxString m_fileName;
	// html contents
	wxString m_html;
	// viewed data 
	wxXmlDocument m_doc;
};

#endif // _data_file_view_h_