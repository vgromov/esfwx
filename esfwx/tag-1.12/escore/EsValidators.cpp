#include "stdafx.h"
#pragma hdrstop

#include "EsValidators.h"

const EsDoubleValidator::Boundary EsDoubleValidator::s_emptyBoundary;

IMPLEMENT_DYNAMIC_CLASS(EsDoubleValidator, wxValidator)

BEGIN_EVENT_TABLE(EsDoubleValidator, wxValidator)
  EVT_CHAR			(EsDoubleValidator::onChar)
	EVT_TEXT			(wxID_ANY, EsDoubleValidator::onTextUpdate)
	EVT_KILL_FOCUS(EsDoubleValidator::onKillFocus)
END_EVENT_TABLE()

EsDoubleValidator::EsDoubleValidator(wxString fmt, const Boundary& min, const Boundary& max, double* val) :
wxValidator(),
m_fmt(fmt),
m_min(min),
m_max(max),
m_extVar(val),
m_valid(true),
m_validateOnExit(false),
m_skipKillFocus(false)
{
}

EsDoubleValidator::EsDoubleValidator(const EsDoubleValidator& src) :
wxValidator(),
m_fmt(src.m_fmt),
m_min(src.m_min),
m_max(src.m_max),
m_extVar(src.m_extVar),
m_valid(src.m_valid),
m_validateOnExit(src.m_validateOnExit),
m_skipKillFocus(src.m_skipKillFocus)
{
	Copy(src);
}

bool EsDoubleValidator::checkValidator() const
{
  wxCHECK_MSG(m_validatorWindow, false,
		_("Validator is not associated with control"));

	return true;
}

bool EsDoubleValidator::Validate(wxWindow *parent)
{
  if( !checkValidator() )
    return false;

	// check if value is convertable to float
	double val;
	wxString valStr;
	if( m_validatorWindow->IsKindOf(CLASSINFO(wxComboBox)) )
		valStr = wxDynamicCast(m_validatorWindow, wxComboBox)->GetValue();
	else
		valStr = m_validatorWindow->GetLabel();
  bool result = valStr.ToDouble( &val );
	wxString err;
	if( result )
	{
		// check range
		if( m_min.m_ok && m_max.m_ok )
		{
			if( val < m_min.m_val || val > m_max.m_val )
			{
				err = wxString::Format( _("Value must be in range [%f, %f]"), m_min.m_val, m_max.m_val );
				result = false;
			}
		}
		else if( m_min.m_ok )
		{
			if( val < m_min.m_val )
			{
				err = wxString::Format( _("Value must be greater than or equal to %f"), m_min.m_val );
				result = false;
			}
		}
		else if( m_max.m_ok )
		{
			if( val > m_max.m_val )
			{
				err = wxString::Format( _("Value must be less than or equal to %f"), m_max.m_val );
				result = false;
			}
		}
	}
	else
	{
		if( valStr.empty() )
			valStr = _("Empty string");

		err = wxString::Format( _("\"%s\" is not a valid floating point number"), valStr.t_str() );
	}
	
	m_valid = result; // set internal validator status
	if( !result )
	{
		EsUtilities::showErrorFmt( _("Validation error"), _("Validation failed for %s\n%s"), m_validatorWindow->GetName().t_str(),
			err.t_str() );
		m_validatorWindow->SetFocus();
		m_validatorWindow->SetFocusFromKbd();
	}

	return result;
}

bool EsDoubleValidator::TransferToWindow()
{
  if( !checkValidator() )
    return false;

	if( m_extVar )
		m_validatorWindow->SetLabel( wxString::Format( m_fmt, *m_extVar) );

	return true;
}

bool EsDoubleValidator::TransferFromWindow()
{
  if( !checkValidator() )
    return false;

	if( m_extVar )
	{
		return m_validatorWindow->GetLabel().ToDouble( m_extVar );
	}
	else
		return true;
}

void EsDoubleValidator::updateEditorText()
{
	// set label to the formatted string if internal validator state is valid
	double val;
	if( m_valid && m_validatorWindow->GetLabel().ToDouble(&val) )
		m_validatorWindow->SetLabel( wxString::Format( m_fmt, val ) );
}

void EsDoubleValidator::setFormat(wxString fmt)
{
	if( m_fmt != fmt )
	{
		m_fmt = fmt;
		if( m_validatorWindow && m_validatorWindow != wxWindow::FindFocus() )
			updateEditorText();
	}
}

void EsDoubleValidator::onChar(wxKeyEvent &evt)
{
  int keyCode = evt.GetKeyCode();
	if( !(keyCode < WXK_SPACE || keyCode == WXK_DELETE || keyCode > WXK_START) )
		m_prevStr = m_validatorWindow->GetLabel();
	
  evt.Skip();
}

void EsDoubleValidator::onTextUpdate(wxCommandEvent &evt)
{
	if( evt.GetEventObject() == m_validatorWindow )
	{  
		static const wxChar c_decSep(
#if wxUSE_INTL
      wxLocale::GetInfo(wxLOCALE_DECIMAL_POINT, wxLOCALE_CAT_NUMBER)[0]
#else
      wxT('.')
#endif
      );
		static wxRegEx s_regex( wxString(wxT("^[+-]?[0-9]*[")) + c_decSep + wxT("]?(?:[0-9]+)?(?:e[+-]?[0-9]{0,2})?$"), 
			wxRE_ADVANCED|wxRE_ICASE|wxRE_NOSUB );
		wxASSERT( s_regex.IsValid() );

		if( !s_regex.Matches( evt.GetString() ) )
		{
			m_valid = false;
			m_validatorWindow->SetLabel( m_prevStr );
			if( !wxValidator::IsSilent() )
				wxBell();		
		}
		else
		{
			m_valid = true;
			evt.Skip();
		}
	}
	else
		evt.Skip();
}

void EsDoubleValidator::onKillFocus(wxFocusEvent& evt)
{
	if( !m_skipKillFocus )
	{
		m_skipKillFocus	= true;
		updateEditorText();
		if( m_validatorWindow && evt.GetEventObject() == m_validatorWindow && m_validateOnExit )
			// prevent endless re-entry
			Validate( wxTheApp->GetTopWindow() );
		m_skipKillFocus = false;
	}

	evt.Skip();
}