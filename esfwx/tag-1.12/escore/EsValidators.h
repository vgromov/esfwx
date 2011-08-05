#ifndef _core_validators_h_
#define _core_validators_h_

// misc validator extensions
//
// numeric validator class
class EKOSF_CORE_CLASS EsDoubleValidator : public wxValidator
{
	DECLARE_DYNAMIC_CLASS(EsDoubleValidator)

public:
	class EKOSF_CORE_CLASS Boundary
	{
	public:
		Boundary() : m_ok(false), m_val(0) {}
		Boundary(double val) : m_ok(true), m_val(val) {}

	protected:
		bool m_ok;
		double m_val;

		friend class EsDoubleValidator;
	};
	static const Boundary s_emptyBoundary;

public:
	EsDoubleValidator(wxString fmt = wxT("%f"), const Boundary& min = s_emptyBoundary, 
		const Boundary& max = s_emptyBoundary, double* val = 0);
	EsDoubleValidator(const EsDoubleValidator& src);
	
	bool getValidateOnExit() const { return m_validateOnExit; }
	void setValidateOnExit(bool validate) { m_validateOnExit = validate; }

	wxString getFormat() const { return m_fmt; }
	void setFormat(wxString fmt);

	// overridden validator interface
	virtual wxObject* Clone() const { return new EsDoubleValidator(*this); }
  virtual bool Validate(wxWindow *parent);
  virtual bool TransferToWindow();
  virtual bool TransferFromWindow();

protected:
	bool checkValidator() const;
	void updateEditorText();
	// numeric char filter
  void onChar(wxKeyEvent &evt);
	void onTextUpdate(wxCommandEvent& evt);
	void onKillFocus(wxFocusEvent& evt);

DECLARE_EVENT_TABLE()

protected:
	double* m_extVar;
	Boundary m_min;
	Boundary m_max;
	wxString m_fmt;
	wxString m_prevStr;
	bool m_valid;
	bool m_validateOnExit;
	bool m_skipKillFocus;

private:
	EsDoubleValidator& operator= (const EsDoubleValidator&);
};

#endif // _core_validators_h_