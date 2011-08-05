#include "stdafx.h"
#pragma hdrstop

#include <wx/dir.h>
#include <wx/filename.h>
#include <wx/zstream.h>
#include <wx/mstream.h>
#include <wx/sstream.h>
#include <algorithm>
#include "EsUtilities.h"

DEF_CFG_KEY(LEFT);
DEF_CFG_KEY(TOP);
DEF_CFG_KEY(WIDTH);
DEF_CFG_KEY(HEIGHT);

void EsUtilities::saveWindowPos(wxConfigBase* cfg, const wxWindow* wnd)
{
	wxASSERT(cfg);
	wxASSERT(wnd);
	
	// main form
	wxRect rect = wnd->GetRect();
	cfg->Write( CFG_LEFT_KEY, rect.GetLeft() );
	cfg->Write( CFG_TOP_KEY, rect.GetTop() );
	cfg->Write( CFG_WIDTH_KEY, rect.GetWidth() );
	cfg->Write( CFG_HEIGHT_KEY, rect.GetHeight() );
}	

void EsUtilities::loadWindowPos(wxConfigBase* cfg, wxWindow* wnd, const wxSize& minSize)
{
	wxASSERT(cfg);
	wxASSERT(wnd);
	
	// form pos & size
	wxPoint pos;
	wxSize sze;
	if( cfg->Read(CFG_LEFT_KEY, &pos.x) )
	{
		pos.x = wxMax(0, pos.x);

		if( cfg->Read(CFG_TOP_KEY, &pos.y) )
		{
			pos.y = wxMax(0, pos.y);

			if( cfg->Read(CFG_WIDTH_KEY, &sze.x) )
			{
				sze.x = wxMax(minSize.x, sze.x);

				if( cfg->Read(CFG_HEIGHT_KEY, &sze.y) )
				{
					sze.y = wxMax(minSize.y, sze.y);

					wnd->SetPosition( pos );
					wnd->SetSize( sze );
				}
			}
		}
	}
}

// standard menu bitmaps
#include "res/menu-check-checked.xpm"
#include "res/menu-check-unchecked.xpm"
#include "res/menu-radio-checked.xpm"
#include "res/menu-radio-unchecked.xpm"

// create menu item for later addition to the menu
wxMenuItem* EsUtilities::createMenuItem(wxMenu* parent, int id, const wxString& label, 
	const wxString& help, wxItemKind kind, const wxBitmap& normalChecked,
	const wxBitmap& unchecked, wxMenu* submenu /*= 0*/)
{
	wxMenuItem* result = new wxMenuItem(parent, id, label, help, kind, submenu);
	wxASSERT(result);
//	result->SetFont( ShellApp::getInstance()->getMenuFont() );
	result->SetBitmaps(normalChecked, unchecked);
	return result;
}

// normal item
wxMenuItem* EsUtilities::createNormalMenuItem(wxMenu* parent, int id, const wxString& label, 
	const wxString& help, const wxBitmap& normal/* = wxNullBitmap*/, wxMenu* submenu /*= 0*/)
{
	return EsUtilities::createMenuItem(parent, id, label, help, wxITEM_NORMAL, normal, normal, submenu);
}

// check item
wxMenuItem* EsUtilities::createCheckMenuItem(wxMenu* parent, int id, const wxString& label, 
	const wxString& help, const wxBitmap& checked/* = wxNullBitmap*/, const wxBitmap& unchecked/* = wxNullBitmap*/)
{
	return EsUtilities::createMenuItem(parent, id, label, help, wxITEM_CHECK, 
		(checked.IsNull() && unchecked.IsNull()) ? wxBitmap(menu_check_checked_xpm, wxBITMAP_TYPE_XPM) : checked, 
		(checked.IsNull() && unchecked.IsNull()) ? wxBitmap(menu_check_unchecked_xpm, wxBITMAP_TYPE_XPM) : unchecked, 0);
}
// radio item
wxMenuItem* EsUtilities::createRadioMenuItem(wxMenu* parent, int id, const wxString& label, 
	const wxString& help, const wxBitmap& checked/* = wxNullBitmap*/, const wxBitmap& unchecked/* = wxNullBitmap*/)
{
	return EsUtilities::createMenuItem(parent, id, label, help, wxITEM_CHECK/*wxITEM_RADIO*/, 
		(checked.IsNull() && unchecked.IsNull()) ? wxBitmap(menu_radio_checked_xpm, wxBITMAP_TYPE_XPM) : checked, 
		(checked.IsNull() && unchecked.IsNull()) ? wxBitmap(menu_radio_unchecked_xpm, wxBITMAP_TYPE_XPM) : unchecked, 0);
}

// create language selection menu
wxMenu* EsUtilities::createLanguageSelectionMenu(const EsString::Array& langs, int startId)	
{
	wxMenu* result = new wxMenu;
	wxASSERT(result);
	
	// create language selection menu
	for(size_t idx = 0; idx < langs.size(); ++idx)
	{
		const wxLanguageInfo* info = wxLocale::FindLanguageInfo( langs[idx].c_str() );
		wxASSERT(info);
		result->Append( 
			EsUtilities::createCheckMenuItem(result, startId + info->Language, 
			info->Description, wxString::Format( _("Select '%s' language"), info->Description.t_str())) );
	}
	
	return result;
}

// try to create and initialize locale given language ID. If everything is ok
// pointer to the newly created and initialized locale is returned, NULL otherwise
wxLocale* EsUtilities::initLocale(int languageId)
{
	wxLocale* result = 0;
	try
	{
		result = new wxLocale;
		if( result && !result->Init( languageId/*, wxLOCALE_CONV_ENCODING*/ ) )
		{
			delete result;
			result = 0;
			EsUtilities::showErrorFmt( wxT("Unsupported language."), 
				wxT("Language '%s' is not supported"), wxLocale::GetLanguageName(languageId).t_str() );
		}
	}
	catch(...)
	{
		delete result;
		result = 0;
	}
	
	return result;
}			

// return top level window given the current one
wxWindow* EsUtilities::getTopLevelFor(wxWindow* wnd)
{
	while( wnd )
	{
		if( wnd->IsTopLevel() )
			return wnd;
		
		wnd = wnd->GetParent();
	}
	
	return 0;
}

// GUI adjustments
//
static void setMenuFont(wxMenu* menu, const wxFont& fnt)
{
	wxASSERT(menu);
	wxMenuItemList& items = menu->GetMenuItems();
	for( size_t idx = 0; idx < items.size(); ++idx )
	{
		wxMenuItem* item = items[idx];
		wxASSERT(item);
		if( !item->IsSeparator() )
		{
			menu->Remove(item);
			item->SetFont(fnt);
			menu->Insert(idx, item);
		}
		if( item->IsSubMenu() )
			setMenuFont(item->GetSubMenu(), fnt);
	}
}

void EsUtilities::applyMenuFont(wxMenuBar* mnu, const wxFont& fnt)
{
	if( mnu )
	{
		wxWindowUpdateLocker lock(mnu);
		for(size_t idx = 0; idx < mnu->GetMenuCount(); ++idx)
		{
			wxMenu* menu = mnu->GetMenu(idx);
			setMenuFont(menu, fnt);
		}
	}
}

static void doApplyCtlsFont(wxWindow* wnd, const wxFont& fnt)
{
	wxASSERT(wnd);
	wxWindowList& children = wnd->GetChildren();
	wxSize sze;
	int height = fnt.GetPixelSize().y;
	for( size_t idx = 0; idx < children.size(); ++idx )
	{
		wxWindow* child = children[idx];
		wxASSERT(child);
		doApplyCtlsFont(child, fnt);
		child->SetFont(fnt);
		sze = child->GetClientSize();
		sze.y = wxMax(sze.y, height);
		child->SetClientSize(sze);		
	}
	wnd->SetFont(fnt);
	sze = wnd->GetClientSize();
	sze.y = wxMax(sze.y, height);
	wnd->SetClientSize(sze);		
}

void EsUtilities::applyCtlsFont(wxWindow* wnd, const wxFont& fnt)
{
	if( wnd )
	{
		wxWindowUpdateLocker lock(wnd);
		doApplyCtlsFont(wnd, fnt);
		wnd->Layout();
		wnd->Refresh();
	}
}

// global data MT guard
static wxCriticalSection s_cs;

// access globally set-up application logger
static EsLogIntf* s_logger; 
EsLogIntf* EsUtilities::getAppLogger()
{
	wxCriticalSectionLocker lock(s_cs);
	return s_logger;
}

void EsUtilities::setAppLogger(EsLogIntf* intf) 
{
	wxCriticalSectionLocker lock(s_cs);
	s_logger = intf;
}

// access globally set-up application locale
static wxLocale* s_loc;
wxLocale* EsUtilities::getAppLocale()
{	
	wxCriticalSectionLocker lock(s_cs);
	return s_loc;
}

void EsUtilities::setAppLocale(wxLocale* loc)
{
	wxCriticalSectionLocker lock(s_cs);	
	s_loc = loc;
}

// return readable asl string
EsString EsUtilities::getAslString(int asl)
{
	EsString result;
	
	switch(asl)
	{
	case AslNone: // no user is logged on
		result = _("None");
		break;
	case AslGuest:
		result = _("Guest");
		break;
	case AslUser:
		result = _("User");
		break;
	case AslSuperuser:
		result = _("Superuser");
		break;
	case AslAdministrator:
		result = _("Administrator");
		break;
	case AslDeveloper:
		result = _("Developer");
		break;
	default:
		result = _("Invalid role value");
		break;	
	}
	
	return result;
}

// framework-wide logging support
void EsUtilities::log(EsLogIntf::Level lvl, const EsString& msg, bool notify)
{
	// log error
	EsLogIntf* logger = EsUtilities::getAppLogger();
	if( logger )
		logger->append(lvl, msg);
}

void EsUtilities::logError(const EsString& msg, bool notify/* = false*/)
{
	log(EsLogIntf::LevelError, msg, notify);
}

void EsUtilities::logWarning(const EsString& msg, bool notify/* = false*/)
{
	log(EsLogIntf::LevelWarning, msg, notify);
}

// copy wxArrayString	to string array and vice versa
EsString::Array EsUtilities::fromArrayString(const wxArrayString& src)
{
	EsString::Array result;
	result.reserve(src.size());
	for(size_t idx = 0; idx < src.size(); ++idx)
		result.push_back(src[idx]);
	return result;
}

wxArrayString EsUtilities::toArrayString(const EsString::Array& src)
{
	wxArrayString result;
	result.reserve(src.size());
	for(size_t idx = 0; idx < src.size(); ++idx)
		result.push_back(src[idx].c_str());
	return result;
}

// our own message dialog implementation (use centralized font settings)
int EsUtilities::showMessage(const EsString& msg, const EsString& caption, long lStyle, const wxPoint& pos)
{
	wxMessageDialog dlg(wxTheApp->GetTopWindow(), msg, caption, lStyle, pos);
	return dlg.ShowModal();
}

// show modal error message box
void EsUtilities::showError(const EsString& msg, const EsString& caption/* = _("Error")*/)
{
	EsUtilities::showMessage(msg, caption, wxCENTRE|wxOK|wxICON_ERROR);
}

void EsUtilities::showErrorFmt(const EsString& caption, const wxChar* fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);

	EsString msg = EsString::formatV(fmt, argptr);
	EsUtilities::showError(msg, caption);
}

// datetime conversions
wxDateTime EsUtilities::DATETIME2wxDateTime(const DATETIME& dt)
{
	int y, mn, d, h, m, s, ms;
	dtDecomposeDateTime(&dt, &y, 0, &mn, &d, &h, &m, &s, &ms);
	// NB. wxDateTime uses 0-based month values
	return wxDateTime( d, static_cast<wxDateTime::Month>(mn-1), y, h, m, s, ms);
}

DATETIME EsUtilities::wxDateTime2DATETIME(const wxDateTime& dt)
{
	DATETIME result = 0;
	wxDateTime::Tm tm = dt.GetTm();
	// NB. wxDateTime uses 0-based month values
	if( !dtComposeDateTime(&result, tm.year, static_cast<int>(tm.mon)+1, tm.mday, tm.hour, tm.min, tm.sec, tm.msec) )
		EsException::Throw(0, EsException::Generic,
			_("Error composing DATETIME from:\nyear: %d, month: %d, day: %d,\nhour: %d, minute: %d, second: %d, millisecond: %d"),
		tm.year, tm.mon, tm.mday, tm.hour, tm.min, tm.sec, tm.msec);

	return result;
}

// extract only time part from wxDateTime
DATETIME EsUtilities::wxDateTimeTime2DATETIME(const wxDateTime& dt)
{
	DATETIME result = 0;
	wxDateTime::Tm tm = dt.GetTm();
	// NB. wxDateTime uses 0-based month values
	if( !dtComposeDateTime(&result, DT_YEAR0, 1, 1, tm.hour, tm.min, tm.sec, tm.msec) )
		EsException::Throw(0, EsException::Generic,
			_("Error composing time-only DATETIME from:\nhour: %d, minute: %d, second: %d, millisecond: %d"),
		tm.hour, tm.min, tm.sec, tm.msec);

	return result;
}

// get longest string width in pixels, using wnd visual settings
int EsUtilities::getLongestStringWidth(const EsString::Array& strings, const wxWindow& wnd, const wxFont& fnt)
{
	int result = 0;

	for( size_t idx = 0; idx < strings.size(); ++idx )
	{
		int w, h;
		if( !fnt.IsNull() )
			wnd.GetTextExtent(strings[idx].c_str(), &w, &h, 0, 0, &fnt);
		else
			wnd.GetTextExtent(strings[idx].c_str(), &w, &h);
		result = wxMax(result, w);
	}

	return result;
}

// get languages installed under the specified directory. only use non-empty language folders containing mo files with specified base name
//
// internal mo file traverser, accumulates found languages into m_languages array linked externally
class MoTraverser : public wxDirTraverser
{
public:
	MoTraverser(EsString::Array& languages, const EsString& baseMoName) : 
	m_languages(languages),
	m_baseMoName(baseMoName)
	{
		wxASSERT(s_langDirRegex.IsValid());
	}

	virtual wxDirTraverseResult OnFile(const wxString& WXUNUSED(filename))
	{
		return wxDIR_CONTINUE;
	}

	virtual wxDirTraverseResult OnDir(const wxString& dirname)
	{
		wxFileName dir(dirname);
		if( s_langDirRegex.Matches( dir.GetName() ) )
		{
			wxFileName file(dirname, m_baseMoName + wxT(".mo"));
			if( file.FileExists() )
			{
				const wxLanguageInfo* info = wxLocale::FindLanguageInfo( dir.GetName() );
				if( info && (m_languages.end() == std::find( m_languages.begin(), m_languages.end(), static_cast<EsString>(info->Description) )) )
					m_languages.push_back( info->Description );
			}
		}

		return wxDIR_CONTINUE;
	}

private:
	EsString::Array& m_languages;
	const EsString& m_baseMoName;
	static const wxRegEx s_langDirRegex;
};

const wxRegEx MoTraverser::s_langDirRegex(wxT("^[a-z]{2}(_[A-Z]{2}){0,1}$"), wxRE_ADVANCED|wxRE_NOSUB);

EsString::Array EsUtilities::getInstalledLanguages(const EsString& languagesRoot, const EsString& baseMoName)
{
	EsString::Array result;

	// iterate all sub-folders of languagesRoot
	wxDir dir(languagesRoot.c_str());
	dir.Traverse(MoTraverser(result, baseMoName), wxEmptyString, wxDIR_DIRS);
	
	// always support english
	const wxLanguageInfo* info = wxLocale::FindLanguageInfo(wxT("en"));
	wxASSERT(info);
	if( result.end() == std::find(result.begin(), result.end(), info->Description.c_str()) )
		result.push_back(info->Description);

	return result;
}

// handle language selection event
int EsUtilities::handleLangSelectionEvent(int curLangId, int startLangMenuId, wxCommandEvent& evt)
{
	// find language id for clicked language
	int langId = evt.GetId()-startLangMenuId;
	if( langId != curLangId )
	{
		EsUtilities::showMessage(_("User Interface Language is changed. It will not take place until after application is restarted."), 
			_("User Interface Language Changed"), wxOK|wxICON_INFORMATION|wxCENTER);
	}
	
	return langId;
}

// convert GUID to string
EsString EsUtilities::GUIDtoStr(const GUID& guid)
{
	EsString key;
	EsString::binToHex(reinterpret_cast<const BYTE*>(&guid), sizeof(GUID), key);
	return key;
}

// firmware processing (compression+encryption)
void EsUtilities::makeFwe(wxInputStream& in, wxOutputStream& out, const EsString& key)
{
	EsBinBuffer src, dest;
	wxMemoryOutputStream mOut;
	wxZlibOutputStream zOut(mOut);
	in >> zOut;
	zOut.Close();
	wxStreamBuffer* buff = mOut.GetOutputStreamBuffer();
	src.assign(reinterpret_cast<EsBinBuffer::const_pointer>(buff->GetBufferStart()), 
		reinterpret_cast<EsBinBuffer::const_pointer>(buff->GetBufferEnd()));
	DES des(key);
	des.process(src, dest);
	wxMemoryInputStream mIn(&dest[0], dest.size());
	mIn >> out;
}

void EsUtilities::makeFw(wxInputStream& in, wxOutputStream& out, const EsString& key)
{
	EsBinBuffer src, dest;
	wxMemoryOutputStream mOut;			
	in >> mOut;
	wxStreamBuffer* buff = mOut.GetOutputStreamBuffer();
	src.assign(reinterpret_cast<EsBinBuffer::const_pointer>(buff->GetBufferStart()), 
		reinterpret_cast<EsBinBuffer::const_pointer>(buff->GetBufferEnd()));
	DES des(key);
	des.process(src, dest, false);
	wxMemoryInputStream mIn(&dest[0], dest.size());
	wxZlibInputStream zIn(mIn);			
	zIn >> out;
}

EsString EsUtilities::sToE(const EsString& s, const EsString& key)
{
	if( !s.empty() )
	{
		wxStringInputStream in(s);
		wxMemoryOutputStream out;
		EsUtilities::makeFwe(in, out, key);
		wxStreamBuffer* buff = out.GetOutputStreamBuffer();
		wxASSERT(buff);
		EsBinBuffer bin( reinterpret_cast<EsBinBuffer::pointer>(buff->GetBufferStart()), 
			reinterpret_cast<EsBinBuffer::pointer>(buff->GetBufferEnd()) );
		return EsString::binToHex(bin);
	}
	
	return EsString::s_null;
}

EsString EsUtilities::eToS(const EsString& e, const EsString& key)
{
	if( !e.empty() )
	{
		wxASSERT( 0 == e.size() % 2 );
		EsBinBuffer bin( e.size()/2 );
		EsString::hexToBin(e.c_str(), e.size(), &bin[0], bin.size());
		wxMemoryInputStream in(&bin[0], bin.size());
		wxStringOutputStream out;
		EsUtilities::makeFw(in, out, key);
		return out.GetString();
	}
	
	return EsString::s_null;
}

// acquire explanation string for FsResult value
// copypaste from <common/file_sys/t-ffs/src/ff.h>
typedef enum {
	FR_OK = 0,			/* 0 */
	FR_DISK_ERR,		/* 1 */
	FR_INT_ERR,			/* 2 */
	FR_NOT_READY,		/* 3 */
	FR_NO_FILE,			/* 4 */
	FR_NO_PATH,			/* 5 */
	FR_INVALID_NAME,	/* 6 */
	FR_DENIED,			/* 7 */
	FR_EXIST,				/* 8 */
	FR_INVALID_OBJECT,	/* 9 */
	FR_WRITE_PROTECTED,	/* 10 */
	FR_INVALID_DRIVE,	/* 11 */
	FR_NOT_ENABLED,	/* 12 */
	FR_NO_FILESYSTEM,	/* 13 */
	FR_MKFS_ABORTED,/* 14 */
	FR_TIMEOUT			/* 15 */
} FRESULT;

EsString EsUtilities::getFsResultString(FsResult fr)
{
	switch( fr )
	{
	case FR_OK:
		return _("No Error");
	case FR_DISK_ERR:
		return _("Disk Error");
	case FR_INT_ERR:
		return _("Internal Error");
	case FR_NOT_READY:
		return _("Disk drive is not ready|no media in drive");
	case FR_NO_FILE:
		return _("Cannot find file|directory");
	case FR_NO_PATH:
		return _("Cannot find path");
	case FR_INVALID_NAME:
		return _("File|path name is invalid");
	case FR_DENIED:
		return _("The file or directory has read-only attribute, or the directory is not empty");
	case FR_EXIST:
		return _("Object already exists");
	case FR_INVALID_OBJECT:
		return _("File system object is invalid");
	case FR_WRITE_PROTECTED:
		return _("Disk is write protected");
	case FR_INVALID_DRIVE:
		return _("The drive number is invalid");
	case FR_NOT_ENABLED:
		return _("The logical drive has no work area");
	case FR_NO_FILESYSTEM:
		return _("No file system is found");
	case FR_MKFS_ABORTED:
		return _("Make file system call failed");
	case FR_TIMEOUT:
		return _("File operation timed-out");
	default:
		return EsString::format(_("Unknown file system result value: %d"), fr);
	}
}

// throw remote file system exception
bool EsUtilities::checkFsResult(FsResult fr, bool doThrow)
{
	bool result = 0 == fr;
	if( !result && doThrow )
		EsException::Throw(0, EsException::Generic, getFsResultString(fr).c_str());
		
	return result;
}
