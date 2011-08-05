#ifndef _es_utilities_h_
#define _es_utilities_h_
 
// misc utility classes and procedures
//
#ifndef ES_CONST_ARRAY_COUNT
#	define ES_CONST_ARRAY_COUNT(a)	static_cast<size_t>(sizeof(a)/sizeof(a[0]))
#endif

namespace EsUtilities
{
// windows position serialization
EKOSF_CORE_FUNC void saveWindowPos(wxConfigBase* cfg, const wxWindow* wnd);
EKOSF_CORE_FUNC void loadWindowPos(wxConfigBase* cfg, wxWindow* wnd, const wxSize& minSize);

// GUI adjustments
EKOSF_CORE_FUNC void applyMenuFont(wxMenuBar* mnu, const wxFont& fnt);
EKOSF_CORE_FUNC void applyCtlsFont(wxWindow* wnd, const wxFont& fnt);

// return top level window given the current one
EKOSF_CORE_FUNC wxWindow* getTopLevelFor(wxWindow* wnd);

// generic
EKOSF_CORE_FUNC wxMenuItem* createMenuItem(wxMenu* parent, int id, const wxString& label, const wxString& help, 
	wxItemKind kind, const wxBitmap& normalChecked,	const wxBitmap& unchecked, wxMenu* submenu);
// normal	
EKOSF_CORE_FUNC wxMenuItem* createNormalMenuItem(wxMenu* parent, int id, const wxString& label, 
	const wxString& help, const wxBitmap& normal = wxNullBitmap, wxMenu* submenu = 0);
// check item
EKOSF_CORE_FUNC wxMenuItem* createCheckMenuItem(wxMenu* parent, int id, const wxString& label, 
	const wxString& help, const wxBitmap& checked = wxNullBitmap, const wxBitmap& unchecked = wxNullBitmap);
// radio item
EKOSF_CORE_FUNC wxMenuItem* createRadioMenuItem(wxMenu* parent, int id, const wxString& label, 
	const wxString& help, const wxBitmap& checked = wxNullBitmap, const wxBitmap& unchecked = wxNullBitmap);
// create language selection menu
EKOSF_CORE_FUNC wxMenu* createLanguageSelectionMenu(const EsString::Array& langs, int startId);	
// handle language selection event
EKOSF_CORE_FUNC int handleLangSelectionEvent(int curLangId, int startLangMenuId, wxCommandEvent& evt);

// try to create and initialize locale given language ID. If everything is ok
// pointer to the newly created and initialized locale is returned, NULL otherwise
EKOSF_CORE_FUNC wxLocale* initLocale(int languageId);
	
// access globally set-up application logger
EKOSF_CORE_FUNC EsLogIntf* getAppLogger();
EKOSF_CORE_FUNC void setAppLogger(EsLogIntf* intf);

// access globally set-up application locale
EKOSF_CORE_FUNC wxLocale* getAppLocale();
EKOSF_CORE_FUNC void setAppLocale(wxLocale* loc);

// return readable asl string
EKOSF_CORE_FUNC EsString getAslString(int asl);

// create menu item for later addition to the menu
//

// framework-wide logging support
EKOSF_CORE_FUNC void log(EsLogIntf::Level lvl, const EsString& msg, bool notify);
EKOSF_CORE_FUNC void logError(const EsString& msg, bool notify = false);
EKOSF_CORE_FUNC void logWarning(const EsString& msg, bool notify = false);

// copy wxArrayString	to string array and vice versa
EKOSF_CORE_FUNC EsString::Array fromArrayString(const wxArrayString& src);
EKOSF_CORE_FUNC wxArrayString toArrayString(const EsString::Array& src);

// our own message dialog implementation (use centralized font settings)
EKOSF_CORE_FUNC int showMessage(const EsString& msg, const EsString& caption, long lStyle = wxOK|wxCANCEL, const wxPoint& pos = wxDefaultPosition);

// show modal error message box
EKOSF_CORE_FUNC void showError(const EsString& msg, const EsString& caption = _("Error") );
EKOSF_CORE_FUNC void showErrorFmt(const EsString& caption, const wxChar* fmt, ...);

// datetime conversions
EKOSF_CORE_FUNC wxDateTime DATETIME2wxDateTime(const DATETIME& dt);
EKOSF_CORE_FUNC DATETIME wxDateTime2DATETIME(const wxDateTime& dt);
// extract only time part from wxDateTime
EKOSF_CORE_FUNC DATETIME wxDateTimeTime2DATETIME(const wxDateTime& dt);

// get longest string width in pixels, using wnd visual settings
EKOSF_CORE_FUNC int getLongestStringWidth(const EsString::Array& strings, const wxWindow& wnd, const wxFont& fnt = wxNullFont);

// get languages installed under the specified directory. only use non-empty language folders containing mo files with specified base name
EKOSF_CORE_FUNC EsString::Array getInstalledLanguages(const EsString& languagesRoot, const EsString& baseMoName);

// convert GUID to string
EKOSF_CORE_FUNC EsString GUIDtoStr(const GUID& guid);

// firmware processing (compression+encryption)
EKOSF_CORE_FUNC void makeFwe(wxInputStream& in, wxOutputStream& out, const EsString& key);
EKOSF_CORE_FUNC void makeFw(wxInputStream& in, wxOutputStream& out, const EsString& key);

// string codec based on makeFwe|makeFw
EKOSF_CORE_FUNC EsString sToE(const EsString& s, const EsString& key);
EKOSF_CORE_FUNC EsString eToS(const EsString& e, const EsString& key);

// acquire explanation string for remote FsResult value
EKOSF_CORE_FUNC EsString getFsResultString(FsResult fr);

// throw remote file system exception
EKOSF_CORE_FUNC bool checkFsResult(FsResult fr, bool doThrow = false);

// rounding double to int type
template <typename T>
T roundTo(double d)
{
	if( d >= 0.0 )
		return static_cast<T>(d + 0.5);
	else
		return static_cast<T>(d - 0.5);
}

inline double round0(double val)
{
	if( val >= 0.0 )
		return floor(val + 0.5);
	else
		return ceil(val - 0.5);
}

}; // namespace EsUtilities

#endif // _es_utilities_h_