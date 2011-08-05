#ifndef _es_spec_view_h_
#define _es_spec_view_h_

#include <wx/statline.h> 
#include <wx/clrpicker.h>
#include <wx/spinctrl.h>

// spectrum view pane
//
class EKOSF_CHART_CLASS EsSpecView : public wxPanel
{
protected:
	// internal linkage handling codes
	enum LinkUpdateCode {
		startChanged,
		endChanged
	};
	
public:
	enum {
		IDC_SPEC_AUTOSCALE = wxID_HIGHEST + 8000,
		IDC_SPEC_ZOOMIN,
		IDC_SPEC_ZOOMOUT,
		IDC_SPEC_CLEAR,
		IDC_ADD_WINDOW,
		IDC_REMOVE_WINDOW,
		IDC_RENAME_WINDOW,
		// misc constatnts
		defLineWidth = 2,
		maxLineWidth = 10,
		minMaxY = 100,
		spacerWidth = 2,
	};
	
	class EKOSF_CHART_CLASS Locker
	{
	public:
		Locker(EsSpecView& view) :
		m_view(view)
		{
			++m_view.m_lock;
			if(1 == m_view.m_lock)
			{
				wxASSERT(m_view.m_ds);
				m_view.m_ds->BeginUpdate();
			}
		}
	
		~Locker()
		{
			if( 0 == --m_view.m_lock )
			{
				m_view.m_ds->EndUpdate();
				m_view.update();
			}
		}
		
	protected:
		EsSpecView& m_view;	
		
	private:
		Locker();
		Locker(const Locker&);
		Locker& operator= (const Locker&);	
	};
	friend class Locker;
	
	// spectrometer window
	class EKOSF_CHART_CLASS Window : public wxPanel
	{
	public:
		enum { 
			defOpacity = 64,
		};
		typedef boost::shared_ptr<Window> Ptr;
	
	protected:
		// hidden functionality
		Window(EsSpecView& owner);
		// internal services
		void fullRecalcWindowData();
		// re-layout this window and sidepanel which is owning it
		void updateLayout();

	public:
		virtual ~Window();	
		// serialization
		void load(wxConfigBase& cfg, const EsString& key);
		void save(wxConfigBase& cfg, const EsString& key);
		// misc public services
		void reset();
		void onPulse(size_t chnl, size_t chnlVal);
		void onNewSpecSize(size_t size);
		void updateInfo(bool full = false);
		// window properties access
		//
		// name
		EsString getName() const
		{
			return GetLabel();
		}
		void setName(const EsString& name)
		{
			SetLabel(name);
		}
		// borders change
		size_t getStart() const
		{
			return m_left;
		}
		void setStart(size_t start);
		size_t getEnd() const
		{
			return m_right;
		}
		void setEnd(size_t end);
		void setRange(size_t start, size_t end)
		{
			setStart(start);
			setEnd(end);
		}
		// lock editing
		bool getLock() const
		{
			return m_locked->GetValue();
		}
		void setLock(bool locked);
		// color change
		wxColor getColor() const
		{
			return m_cl->GetColour();
		}
		void setColor(const wxColor& cl);
		// opacity change
		BYTE getOpacity() const
		{
			return m_opacity->GetValue();
		}
		void setOpacity(BYTE opacity);
		// misc getters
		size_t getSum() const
		{
			return m_sum;
		}
		// enable|disable editing
		void enableStartEndEditing(bool enable = true);
		void enableColorEditing(bool enable = true);
		void enableOpacityEditing(bool enable = true);
		void enableLocking(bool enable = true);
		// show|hide editing controls
		void showColorEditor(bool show = true);
		void showOpacityEditor(bool show = true);
		// base class overrides
		virtual void SetLabel(const wxString& lbl);
		
	protected:
		// event handlers
		void onStartChanged(wxSpinEvent& evt);
		void onEndChanged(wxSpinEvent& evt);
		void onColorChanged(wxColourPickerEvent& evt);
		void onOpacityChanged(wxSpinEvent& evt);
		void onLockChanged(wxCommandEvent& evt);

		// internal services
		void doSetStart(size_t end);
		void doSetEnd(size_t end);

	private:
		// prohibited functionality
		Window(const Window&);
		Window& operator= (const Window&);

	protected:
		EsSpecView& m_owner;
		// window controls
		wxStaticBoxSizer* m_box;		
		wxFlexGridSizer* m_grid;
		wxSpinCtrl* m_start;
		wxSpinCtrl* m_end;
		wxStaticText* m_lblCl;
		wxColourPickerCtrl* m_cl;
		wxStaticText* m_lblOpacity;
		wxSpinCtrl* m_opacity;
		wxCheckBox* m_locked;
		// chart range marker
		AreaDraw* m_markerAreaDraw;
		RangeMarker* m_marker;
		// marker movement recursive lock
		int m_suppressMarkerMove;
		// link handling recursion lock
		bool m_handlingLink;
		// setters recursion locks
		bool m_settingStart;
		bool m_settingEnd;
		// window info labels
		wxStaticText* m_lblSum;
		wxStaticText* m_lblMax;
		wxStaticText* m_lblMaxChnl;
		// internal data cache
		size_t m_left;
		size_t m_right;
		size_t m_sum;			// sum of pulses in window
		size_t m_max;			// maximum value in window
		size_t m_maxIdx;  // window channel where max value is
		
		friend class EsSpecView;
	};
	typedef std::vector< EsSpecView::Window::Ptr > WindowsT;

	// internal window linkage stuff
	class WindowLink
	{
	protected:
		WindowLink(Window::Ptr left, Window::Ptr right) :
		m_left(left),
		m_right(right)
		{
			wxASSERT(m_left);
			wxASSERT(m_right);
			wxASSERT(m_left != m_right);
		}
				
	public:	
		bool isParticipating(Window::Ptr wnd) const
		{
			return isParticipating(*(wnd.get()));
		}
		
		bool isParticipating(Window& wnd) const
		{
			return m_left.get() == &wnd || m_right.get() == &wnd;
		}		

		bool isParticipatingLeft(Window& wnd) const
		{
			return m_left.get() == &wnd;
		}		

		bool isParticipatingRight(Window& wnd) const
		{
			return m_right.get() == &wnd;
		}		
		
		bool operator== (const WindowLink& other) const
		{
			return m_left == other.m_left && m_right == other.m_right;
		}
	
		Window::Ptr m_left;
		Window::Ptr m_right;
		friend class EsSpecView;
	};
	typedef std::vector<WindowLink> WindowLinksT;
	friend class Window;
	typedef std::vector<double> Data;
	
public:	
	EsSpecView(wxWindow* parent, size_t minSpecRes, size_t maxSpecRes, size_t defSpecRes);
	virtual ~EsSpecView();
	// access side panel sizer. use with caution
	wxBoxSizer* getSidePanel()
	{
		return m_sideBox;
	}
	// show|hide spectrometer side panel
	void showSidePanel(bool show);
	bool sidePanelShown() const
	{
		return m_sideBox->IsShown(static_cast<size_t>(0));
	}
	// restrictions access
	size_t getMinSpecRes() const { return m_minSpecRes; }
	size_t getMaxSpecRes() const { return m_maxSpecRes; }
	size_t getDefSpecRes() const { return m_defSpecRes; }
	// spectrum stats access
	size_t getSum() const { return m_sum;	}
	size_t getMax() const { return m_max;	}
	size_t getMaxChnl() const { return m_maxIdx; }
	// spectrum data access
	size_t getPulses(size_t chnl) const;
	bool addPulse(size_t ampl);
	void setData( const EsSpecView::Data& data );
	// clear spectrometer data. if resetScale is true, 
	// reset spectrum chart scaling to default as well
	void clearSpec(bool resetScale = false);	
	// spectrometer parameter setter helpers
	void setSpecResolution(size_t sr, bool force = false);
	size_t getSpecResolution() const { return m_specResolution; }
	void setSpecColor(wxColor cl, bool force = false);
	wxColor getSpecColor() const { return m_specBrush.GetColour(); }
	void setSpecBarWidth(size_t width);
	size_t getSpecBarWidth() const { return m_barWidth; }
	// window management
	void showWindowButtons(bool show);
	bool windowButtonsShown() const
	{
		return m_windowCtlsShown;
	}
	Window::Ptr addWindow(const EsString& name);
	void renameWindow();
	void renameWindow(size_t idx, const EsString& name);
	void removeWindow();
	void removeWindow(size_t idx);	
	size_t getWindowsCnt() const
	{
		return m_windows.size();
	}
	Window::Ptr getWindowAt(size_t idx);
	const Window::Ptr getWindowAt(size_t idx) const;
	// windows linkage manipulation
	//
	// add link between left and right
	void addWindowsLink(Window::Ptr left, Window::Ptr right);
	// remove all links in which wnd is participating
	void removeLinks(Window::Ptr wnd);
	// return true if there already is window with this name
	bool isExistingWindowName(const EsString& name, bool doThrow = false) const;
	// return index of the window with specified name, wxNOT_FOUND otherwise
	int findWindowByName(const EsString& name) const;
	// return all existing window names in array
	wxArrayString getAllWindowNames() const;	
	// update spectrum stats && axes, if corresponding needXXX flags are set. 
	// Locker object calls this method automatically on unlock
	void update();
	// scale manipulation
	void setAutoScaleY();
	bool getAutoScaleY() const
	{
		return m_autoscaleY;
	}
	void setScaleY(double scale);
	double getScaleY() const;
	void setZoomOutConstraint(double constraint);
	double getZoomOutConstraint() const
	{
		return m_zoomOutConstraint;
	}
	// settings serialization
	void saveSettings(wxConfigBase& cfg) const;
	void loadSettings(wxConfigBase& cfg);
	// antialiasing
	void setAntialias(bool aa) { m_chartPanel->SetAntialias(aa); };
	bool getAntialias() const { return m_chartPanel->GetAntialias(); };
	// render to bitmap
	wxBitmap getAsBitmap(const wxSize& size) const;
	
protected:
	// internal services
	//
	void connectEvents();
	void disconnectEvents();
	// reset generic spectrum information
	void resetSpecInfo();
	// update spectrum status info
	void updateSpecInfo();
	// window link search. if not found, return wxNOT_FOUND
	int findLink(Window& wnd, bool right) const;
	// window link handling
	bool handleLink(Window& wnd, bool pre, LinkUpdateCode code, size_t val);
	void addWindowCtlButtons();
	void removeWindowCtlButtons();
	// event handlers
	//
	void onSpecCtl(wxCommandEvent& evt);
	void onUpdateUi(wxUpdateUIEvent& evt);	
	
protected:
	// nested lock counter
	long m_lock;
	// pending update flags
	bool m_needUpdateInfo;
	bool m_needAdjustAxes;
	// autoscale flags
	bool m_autoscaleY;
	// chart settings
	int m_barWidth;
	wxBrush m_specBrush;
	// spectrometer settings
	size_t m_minSpecRes;
	size_t m_maxSpecRes;
	size_t m_defSpecRes;
	size_t m_specResolution;
	// spectrometer windows and window links
	WindowsT m_windows;
	WindowLinksT m_links;
	bool m_windowCtlsShown;
	// common spectrometer data
	size_t m_sum;			// total sum of pulses
	size_t m_max;			// maximum value
	size_t m_maxIdx;  // channel where max value is
	// spectrometer side panel
	wxBoxSizer* m_sideBox;
	wxScrolledWindow* m_sidePanel;
	// zoom && windows management buttons
	wxAuiToolBar* m_specTb;
	double m_zoomOutConstraint;
	// spectrometer common info
	wxStaticText* m_lblSum;
	wxStaticText* m_lblMax;
	// spectrometer data
	EsSpecDataset* m_ds;
	NumberAxis* m_aX;
	NumberAxis* m_aY;
	// chart object instance
	Chart* m_chart;
	wxChartPanel* m_chartPanel;
};

#endif // _es_spec_view_h_
