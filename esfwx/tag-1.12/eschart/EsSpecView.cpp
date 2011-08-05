#include "stdafx.h"
#pragma hdrstop

#include "EsSpecView.h"

#include <numeric>
#include <algorithm>
#include <wx/tokenzr.h>
#include <common/utils.h>

// resources
#include "res/spec-wnd-add.xpm"
#include "res/spec-wnd-rename.xpm"
#include "res/spec-wnd-del.xpm"
#include "res/spec-autoscale.xpm"
#include "res/spec-zoom-in.xpm"
#include "res/spec-zoom-out.xpm"
#include "res/spec-clear.xpm"

// shared strings
#define STR_DEFAULT_MAX_INFO							_("Max: -\nin channel: -")
#define STR_FMT_MAX_INFO									_("Max: %d\nin channel: %d")
#define STR_FMT_SUM												_("Sum: %d")
#define STR_WINDOW_INDEX									_("Window index")

DEF_CFG_KEY(RESOLUTION);
DEF_CFG_KEY(ALLOW_WINDOW_OVERLAP);
DEF_CFG_KEY(WINDOWS_CNT);
DEF_CFG_KEY(SPEC_COLOR);
DEF_CFG_KEY(SPEC_BAR_WIDTH);

// spectrometer window implementation
//
EsSpecView::Window::Window(EsSpecView& owner) :
wxPanel(owner.m_sidePanel),
m_owner(owner),
m_box(0),
m_grid(0),
m_start(0),
m_end(0),
m_lblCl(0),
m_cl(0),
m_lblOpacity(0),
m_opacity(0),
m_markerAreaDraw(0),
m_marker(0),
m_suppressMarkerMove(0),
m_handlingLink(false),
m_settingStart(false),
m_settingEnd(false),
m_locked(0),
m_lblSum(0),
m_lblMax(0),
m_left(0),
m_right(0),
m_sum(0),
m_max(0),
m_maxIdx(0)
{
	// initialize spectrometer window controls
	//
	wxSizerFlags spinCtlFlags = wxSizerFlags().Proportion(1).Expand();
	wxSizerFlags lblFlags = wxSizerFlags().Align(wxALIGN_CENTER_VERTICAL);
	wxSizerFlags lblInfoFlags = wxSizerFlags().Border().Expand();
	m_box = new wxStaticBoxSizer(wxVERTICAL, this);
	wxASSERT(m_box);
	m_grid = new wxFlexGridSizer(2);
	wxASSERT(m_grid);
	m_grid->AddGrowableCol(1, 1);
	m_box->Add(m_grid, wxSizerFlags().Expand().Border());
	// start-stop margins
	wxStaticText* lbl = new wxStaticText(this, wxID_ANY, _("Start:"));
	wxASSERT(lbl);
	m_grid->Add(lbl, lblFlags);
	m_start = new wxSpinCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(64, wxDefaultCoord));
	wxASSERT(m_start);
	m_grid->Add(m_start, spinCtlFlags);
	lbl = new wxStaticText(this, wxID_ANY, _("End:"));
	wxASSERT(lbl);
	m_grid->Add(lbl, lblFlags);
	m_end = new wxSpinCtrl(this);
	wxASSERT(m_end);
	m_grid->Add(m_end, spinCtlFlags);
	// color settings
	m_lblCl = new wxStaticText(this, wxID_ANY, _("Color:"));
	wxASSERT(m_lblCl);
	m_grid->Add(m_lblCl, lblFlags);
	m_cl = new wxColourPickerCtrl(this, wxID_ANY, *wxStockGDI::GetColour(wxStockGDI::COLOUR_GREEN));
	wxASSERT(m_cl);
	m_grid->Add(m_cl, wxSizerFlags().Proportion(1).Expand());
	// opacity settings (for area only)
	m_lblOpacity = new wxStaticText(this, wxID_ANY, _("Opacity:"));
	wxASSERT(m_lblOpacity);
	m_grid->Add(m_lblOpacity, lblFlags);
	m_opacity = new wxSpinCtrl(this);
	wxASSERT(m_opacity);
	m_opacity->SetRange(0, 255);
	m_grid->Add(m_opacity, spinCtlFlags);	
	// editing lock
	m_locked = new wxCheckBox(this, wxID_ANY, _("Window is locked"));
	wxASSERT(m_locked);
	m_box->Add(m_locked, wxSizerFlags().Expand());
	m_box->AddSpacer(EsSpecView::spacerWidth);
	// info labels
	m_lblSum = new wxStaticText(this, wxID_ANY, wxString::Format( STR_FMT_SUM, m_sum ) );
	wxASSERT(m_lblSum);
	m_box->Add(m_lblSum, lblInfoFlags);
	m_lblMax = new wxStaticText(this, wxID_ANY, STR_DEFAULT_MAX_INFO);
	wxASSERT(m_lblMax);
	m_box->Add(m_lblMax, lblInfoFlags);
	
	// create and attach range marker to the m_chart
	m_markerAreaDraw = new EsTransparentAreaDraw(getColor(), 0);
	wxASSERT(m_markerAreaDraw);
	m_marker = new RangeMarker( m_markerAreaDraw );
	wxASSERT(m_marker);
	// attach marker to m_chart
	m_owner.m_ds->AddMarker(m_marker);
	// initialize opacity to default value
	setOpacity(defOpacity);
	
	// bind events
	m_start->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onStartChanged, this);
	m_end->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onEndChanged, this);
	m_locked->Bind(wxEVT_COMMAND_CHECKBOX_CLICKED, &Window::onLockChanged, this);
	m_cl->Bind(wxEVT_COMMAND_COLOURPICKER_CHANGED, &Window::onColorChanged, this);
	m_opacity->Bind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onOpacityChanged, this);
	
	// finalize
	//
	SetSizer(m_box);
	// add itself to the sidebar & layout
	m_owner.m_sidePanel->GetSizer()->Add(this, wxSizerFlags().Expand());
	updateLayout();
	onNewSpecSize(m_owner.getSpecResolution());
}

EsSpecView::Window::~Window()
{
	// unbind events
	m_start->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onStartChanged, this);
	m_end->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onEndChanged, this);
	m_locked->Unbind(wxEVT_COMMAND_CHECKBOX_CLICKED, &Window::onLockChanged, this);		
	m_cl->Unbind(wxEVT_COMMAND_COLOURPICKER_CHANGED, &Window::onColorChanged, this);
	m_opacity->Unbind(wxEVT_COMMAND_SPINCTRL_UPDATED, &Window::onOpacityChanged, this);
	
	// remove marker from m_chart. it does automatic resources cleanup
	m_owner.m_ds->RemoveMarker(m_marker);
	m_marker = 0;
	m_markerAreaDraw = 0;
	
	// remove itself from the sidepanel and layout it
	m_owner.m_sidePanel->GetSizer()->Detach(this);
	m_owner.Layout();
}

// serialization
void EsSpecView::Window::load(wxConfigBase& cfg, const EsString& key)
{
	EsDictionary dic;
	dic.setAsString( cfg.Read(key, wxEmptyString) );
	EsString name = dic.getValueDef(wxT("name"), EsString::s_null).asString();
	if( !name.empty() )
		setName( name );
	setStart( dic.getValueDef(wxT("start"), 0).asULong() );
	setEnd( dic.getValueDef(wxT("end"), 0).asULong() );
	wxColor cl;
	cl.Set( dic.getValueDef(wxT("color"), wxT("green")).asString() );
	setColor( cl );
	setOpacity( dic.getValueDef(wxT("opacity"), defOpacity).asByte() );
	setLock( dic.getValueDef(wxT("locked"), false).asBool() );
}

void EsSpecView::Window::save(wxConfigBase& cfg, const EsString& key)
{
	EsDictionary dic;
	dic.setValue(wxT("name"), getName(), true);
	dic.setValue(wxT("start"), getStart(), true);
	dic.setValue(wxT("end"), getEnd(), true );
	EsString str = getColor().GetAsString();
	dic.setValue(wxT("color"), str, true );
	dic.setValue(wxT("opacity"), getOpacity(), true );
	dic.setValue(wxT("locked"), getLock(), true );
	str = dic.getAsString();
	cfg.Write(key, static_cast<wxString>(str));
}

void EsSpecView::Window::updateInfo(bool full /*= false*/)
{
	if( full )
		fullRecalcWindowData();
	
	m_lblSum->SetLabel( wxString::Format(STR_FMT_SUM, m_sum) );
	if( 0 != m_sum )
		m_lblMax->SetLabel( wxString::Format(STR_FMT_MAX_INFO, m_max, m_maxIdx) );
	else
		m_lblMax->SetLabel( STR_DEFAULT_MAX_INFO );
}

void EsSpecView::Window::reset()
{
	m_sum = 0;
	m_max = 0;
	m_maxIdx = 0;
	updateInfo();
}

void EsSpecView::Window::onPulse(size_t chnl, size_t chnlVal)
{
	if( chnl >= m_left && chnl <= m_right )
	{
		++m_sum;
		if( chnlVal > m_max )
		{
			m_max = chnlVal;
			m_maxIdx = chnl;
		}
	}
}

void EsSpecView::Window::onNewSpecSize(size_t size)
{
	reset();
	if( getEnd() >= size )
		setEnd(size-1);
	m_start->SetRange(0, size-1);
	m_end->SetRange(0, size-1);
}

void EsSpecView::Window::fullRecalcWindowData()
{
	m_sum = 0;
	m_max = 0;
	m_maxIdx = 0;
	
	for(size_t idx = m_left; idx <= m_right; ++idx)
	{
		size_t val = m_owner.m_ds->GetY(idx);
		m_sum += val;
		if( m_max < val )
		{
			m_max = val;
			m_maxIdx = idx;
		}
	}
}

void EsSpecView::Window::SetLabel(const wxString& lbl)
{
	// check for uniqueness
	if( !m_owner.isExistingWindowName(lbl, true) )
	{
		wxPanel::SetLabel(lbl);
		m_box->GetStaticBox()->SetLabel(lbl);
	}
}

void EsSpecView::Window::setStart(size_t start)
{
	m_settingStart = true;
	doSetStart(start);
	m_settingStart = false;
}

void EsSpecView::Window::doSetStart(size_t start)
{
	if( start < m_owner.getSpecResolution() &&
		// notify owner, for optional window link pre-handling
		m_owner.handleLink(*this, true, EsSpecView::startChanged, start) )
	{
		++m_suppressMarkerMove;
		if( m_right < start && !m_settingEnd)
			doSetEnd(start);

		m_left = start;
		m_start->SetValue(start);
		--m_suppressMarkerMove;

		if( 0 == m_suppressMarkerMove )
		{
			m_marker->SetVerticalRange(m_left+1, m_right+1);
			// if we're moving with non-zero spectrum data - do full recalc
			updateInfo(true);
		}
		// notify owner, for optional window post-link handling
		m_handlingLink = true;
		m_owner.handleLink(*this, false, EsSpecView::startChanged, start);
		m_handlingLink = false;
	}
}

void EsSpecView::Window::setEnd(size_t end)
{
	m_settingEnd = true;
	doSetEnd(end);
	m_settingEnd = false;
}

void EsSpecView::Window::doSetEnd(size_t end)
{
	if( end < m_owner.getSpecResolution() &&
		// notify owner, for optional window link pre-handling
		m_owner.handleLink(*this, true, EsSpecView::endChanged, end) )
	{
		++m_suppressMarkerMove;
		if( end < m_left && !m_settingStart )
			doSetStart(end);

		m_right = end;
		m_end->SetValue(end);
		--m_suppressMarkerMove;

		if( 0 == m_suppressMarkerMove )
		{
			m_marker->SetVerticalRange(m_left+1, m_right+1);
			// if we're moving with non-zero spectrum data - do full recalc
			updateInfo(true);
		}
		// notify owner, for optional window post-link handling
		m_handlingLink = true;
		m_owner.handleLink(*this, false, EsSpecView::endChanged, end);
		m_handlingLink = false;
	}
}

void EsSpecView::Window::setLock(bool locked)
{
	m_start->Enable(!locked);
	m_end->Enable(!locked);
	m_cl->Enable(!locked);
	m_opacity->Enable(!locked);
	m_locked->SetValue(locked);
}

void EsSpecView::Window::setColor(const wxColor& cl)
{
	m_cl->SetColour(cl);
	dynamic_cast<EsTransparentAreaDraw*>(m_markerAreaDraw)->setColor(cl);
	// signal we got to redraw marker
	m_owner.m_ds->NeedRedraw(m_marker);
}

void EsSpecView::Window::setOpacity(BYTE opacity)
{
	m_opacity->SetValue(opacity);
	dynamic_cast<EsTransparentAreaDraw*>(m_markerAreaDraw)->setOpacity(opacity);
	// signal we got to redraw marker
	m_owner.m_ds->NeedRedraw(m_marker);
}

// enable|disable editing
void EsSpecView::Window::enableStartEndEditing(bool enable/* = true*/)
{
	m_start->Enable(enable);
	m_end->Enable(enable);	
	if(enable)
		setLock(false);	
}

void EsSpecView::Window::enableColorEditing(bool enable /*= true*/)
{
	m_cl->Enable(enable);
	if(enable)
		setLock(false);	
}

void EsSpecView::Window::enableOpacityEditing(bool enable /*= true*/)
{
	m_opacity->Enable(enable);
	if(enable)
		setLock(false);	
}

void EsSpecView::Window::updateLayout()
{
	Layout();
	m_owner.m_sidePanel->FitInside();
	m_owner.Layout();
}

void EsSpecView::Window::enableLocking(bool enable /*= true*/)
{
	m_box->Show(m_locked, enable);
	if( !enable )
		setLock(false);
	updateLayout();
}

// show|hide editing controls
void EsSpecView::Window::showColorEditor(bool show /*= true*/)
{
	m_grid->Show( m_lblCl, show );
	m_grid->Show( m_cl, show );
	updateLayout();
}

void EsSpecView::Window::showOpacityEditor(bool show /*= true*/)
{
	m_grid->Show( m_lblOpacity, show );
	m_grid->Show( m_opacity, show );
	updateLayout();
}

// events
//
void EsSpecView::Window::onStartChanged(wxSpinEvent& evt)
{
	setStart(evt.GetValue());
}

void EsSpecView::Window::onEndChanged(wxSpinEvent& evt)
{
	setEnd(evt.GetValue());
}

void EsSpecView::Window::onLockChanged(wxCommandEvent& evt)
{
	setLock(evt.IsChecked());
}

void EsSpecView::Window::onColorChanged(wxColourPickerEvent& evt)
{
	setColor( evt.GetColour() );
}

void EsSpecView::Window::onOpacityChanged(wxSpinEvent& evt)
{
	setOpacity( evt.GetValue() );
}

// spectrometer view pane implementation
//
EsSpecView::EsSpecView(wxWindow* parent, size_t minSpecRes, size_t maxSpecRes, size_t defSpecRes) :
wxPanel(parent),
m_lock(0),
m_needUpdateInfo(false),
m_needAdjustAxes(false),
m_autoscaleY(true),
m_minSpecRes(minSpecRes),
m_maxSpecRes(maxSpecRes),
m_defSpecRes(defSpecRes),
m_sideBox(0),
m_sidePanel(0),
m_specTb(0),
m_zoomOutConstraint(65535),
m_ds(0),
m_aX(0),
m_aY(0),
m_barWidth(defLineWidth),
m_specBrush(*wxBLUE),
m_specResolution(0),
m_windowCtlsShown(false),
m_sum(0),
m_max(0),
m_maxIdx(0),
m_lblSum(0),
m_lblMax(0),
m_chart(0),
m_chartPanel(0)
{
	wxASSERT(	minSpecRes > 0 && minSpecRes <= maxSpecRes && defSpecRes >= minSpecRes && defSpecRes <= maxSpecRes );
	
	// reserve windows && links vector
	//
	m_windows.reserve(8);
	m_links.reserve(16);
	
	// initialize controls 
	//
	wxBoxSizer* contents = new wxBoxSizer(wxHORIZONTAL);
	wxASSERT(contents);
	
	// create m_chart stuff
	//
	// first step: create plot
	XYPlot *plot = new XYPlot;
	// create dynamic datasets
	m_ds = new EsSpecDataset;
	wxASSERT(m_ds);
	// set histogram renderer to it
	XYHistoRenderer* renderer = new XYHistoRenderer(m_barWidth);
	wxASSERT(renderer);
	renderer->SetBarArea(0, new FillAreaDraw(*wxStockGDI::GetPen(wxStockGDI::PEN_TRANSPARENT), m_specBrush) );
	m_ds->SetRenderer(renderer);
	// add our datasets to plot
	plot->AddDataset(m_ds);
	// create left and bottom number axes
	m_aY = new NumberAxis(AXIS_LEFT);
	wxASSERT(m_aY);
	m_aY->SetMargins(0, 0);
	m_aY->IntegerValues();
	m_aY->SetFixedBounds(0., minMaxY);
	m_aX = new NumberAxis(AXIS_BOTTOM);
	wxASSERT(m_aX);
	m_aX->SetTitle(_("Channel"));
	m_aX->IntegerValues();
	// add axes to plot
	plot->AddAxis(m_aX);
	plot->AddAxis(m_aY);	
	// link axes and dataset
	plot->LinkDataHorizontalAxis(0, 0);
	plot->LinkDataVerticalAxis(0, 0);	
	// and finally create m_chart
	m_chart = new Chart(plot);
	wxASSERT(m_chart);
	m_chartPanel = new wxChartPanel(this, wxID_ANY, m_chart );
	wxASSERT(m_chartPanel);
	m_chartPanel->SetSizeHints(320, 240);
	contents->Add(m_chartPanel, 1, wxEXPAND|wxALL, 5);
	// spectrum|windows side panel
	m_sideBox = new wxBoxSizer(wxVERTICAL);
	wxASSERT(m_sideBox);
	contents->Add(m_sideBox, 0, wxEXPAND, 5);
	// common spectrum info labels
	m_lblSum = new wxStaticText(this, wxID_ANY, wxString::Format( STR_FMT_SUM, m_sum ) );
	wxASSERT(m_lblSum);
	m_lblSum->SetSizeHints(100, -1);
	m_sideBox->Add(m_lblSum, 0, wxEXPAND);
	m_lblMax = new wxStaticText(this, wxID_ANY, STR_DEFAULT_MAX_INFO);
	wxASSERT(m_lblMax);
	m_sideBox->Add(m_lblMax, 0, wxEXPAND);
	m_sideBox->AddSpacer(EsSpecView::spacerWidth);
	// spectrum control toolbar
	wxBoxSizer* box = new wxBoxSizer(wxHORIZONTAL);
	wxASSERT(box);
	m_specTb = new wxAuiToolBar(this);
	wxASSERT(m_specTb);
	m_specTb->AddTool(IDC_SPEC_AUTOSCALE, wxEmptyString, wxBitmap(spec_autoscale_xpm, wxBITMAP_TYPE_XPM),
		_("Auto scale"), wxITEM_CHECK);
	m_specTb->AddTool(IDC_SPEC_ZOOMIN, wxEmptyString, wxBitmap(spec_zoom_in_xpm, wxBITMAP_TYPE_XPM),
		_("Zoom in"));
	m_specTb->AddTool(IDC_SPEC_ZOOMOUT, wxEmptyString, wxBitmap(spec_zoom_out_xpm, wxBITMAP_TYPE_XPM),
		_("Zoom out"));
	m_specTb->AddSeparator();
	m_specTb->AddTool(IDC_SPEC_CLEAR, wxEmptyString, wxBitmap(spec_clear_xpm, wxBITMAP_TYPE_XPM),
		_("Clear spectrum"));
	addWindowCtlButtons();
	m_specTb->Realize();
	box->Add(m_specTb, wxSizerFlags().Expand());	
	m_sideBox->Add(box, wxSizerFlags().Expand());
	// create vertically scrollable side panel
	m_sidePanel = new wxScrolledWindow(this);
	wxASSERT(m_sidePanel);
	m_sideBox->Add(m_sidePanel, 1, wxEXPAND);
	// set-up scroller
	box = new wxBoxSizer(wxVERTICAL);
	wxASSERT(box);
	m_sidePanel->SetSizer(box);
	m_sidePanel->SetScrollRate(0, 5);
	
	// set sizer with contents
	SetSizer(contents);
	Fit();
	
	// set default spectrum size
	setSpecResolution(m_defSpecRes, true);
	
	// final layout
	Layout();
	
	connectEvents();
}

EsSpecView::~EsSpecView()
{
	disconnectEvents();
}

void EsSpecView::connectEvents()
{
	// setup control events
	Bind(wxEVT_COMMAND_MENU_SELECTED, &EsSpecView::onSpecCtl, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);	
	Bind(wxEVT_UPDATE_UI, &EsSpecView::onUpdateUi, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);		
}

void EsSpecView::disconnectEvents()
{
	// clean control events
	Unbind(wxEVT_COMMAND_MENU_SELECTED, &EsSpecView::onSpecCtl, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);	
	Unbind(wxEVT_UPDATE_UI, &EsSpecView::onUpdateUi, this, IDC_SPEC_AUTOSCALE, IDC_RENAME_WINDOW);		
}

// internal services
//
void EsSpecView::resetSpecInfo()
{
	// reset accumulated values
	m_sum = 0;
	m_max = 0;
	m_maxIdx = 0;			
	// update spectrum status info
	updateSpecInfo();
}

void EsSpecView::setSpecResolution(size_t res, bool force /*= false*/)
{
	if( m_specResolution != res || force )
	{
		m_specResolution = res;
		Locker lock(*this);
		m_ds->clear();
		size_t size = getSpecResolution();
		// vector series have x in range from 0 to size-1, inclusive
		m_aX->SetFixedBounds(0, size-1);
		// reset vertical axis
		m_aY->SetFixedBounds(0., minMaxY);
		m_ds->setSize(size);
		// reset generic info
		resetSpecInfo();
		// reset and adjust all windows
		for(size_t idx = 0; idx < m_windows.size(); ++idx)
		{
			Window::Ptr p = m_windows[idx];
			p->onNewSpecSize(size);
		}
		// set-up pending updage flags
		m_needUpdateInfo = true;
		m_needAdjustAxes = true;
	}
}

void EsSpecView::setSpecColor(wxColour cl, bool force /*= false*/)
{
	if( m_specBrush.GetColour() != cl || force )
	{
		Locker lock(*this);
		m_specBrush.SetColour(cl);
		dynamic_cast<XYHistoRenderer*>(m_ds->GetRenderer())->SetBarArea(0, 
			new FillAreaDraw(*wxStockGDI::GetPen(wxStockGDI::PEN_TRANSPARENT), m_specBrush) );
	}
}

void EsSpecView::setSpecBarWidth(size_t width)
{
	if( m_barWidth != width )
	{
		Locker lock(*this);
		m_barWidth = width;
		XYHistoRenderer* renderer = new XYHistoRenderer( m_barWidth );
		wxASSERT(renderer);
		renderer->SetBarArea(0, new FillAreaDraw(*wxStockGDI::GetPen(wxStockGDI::PEN_TRANSPARENT), m_specBrush));
		m_ds->SetRenderer( renderer );
	}
}

void EsSpecView::setData( const EsSpecView::Data& data )
{
	Locker lock(*this);
	
	if( m_ds->GetCount() != data.size() )
		setSpecResolution(data.size());
	else
		m_ds->clear();

	m_sum = 0;
	for(size_t idx = 0; idx < m_ds->GetCount(); ++idx)
	{
		m_ds->setY(idx, data[idx]);
		m_sum += data[idx];
	}
	// update main minimax and window minimax stuff
	m_needUpdateInfo = true;
	m_needAdjustAxes = true;	
	
	m_max = m_ds->GetMaxY();
	m_maxIdx = m_ds->GetMaxX();
}

void EsSpecView::addWindowCtlButtons()
{
	if( !m_windowCtlsShown )
	{
		m_specTb->AddSeparator();
		m_specTb->AddTool(IDC_ADD_WINDOW, wxEmptyString, wxBitmap(spec_wnd_add_xpm, wxBITMAP_TYPE_XPM),
			_("Add spectrometer window"));
		m_specTb->AddTool(IDC_RENAME_WINDOW, wxEmptyString, wxBitmap(spec_wnd_rename_xpm, wxBITMAP_TYPE_XPM),
			_("Rename spectrometer window"));
		m_specTb->AddTool(IDC_REMOVE_WINDOW, wxEmptyString, wxBitmap(spec_wnd_del_xpm, wxBITMAP_TYPE_XPM),
			_("Remove spectrometer window"));
		m_specTb->Realize();
		m_windowCtlsShown = true;
	}
}

void EsSpecView::removeWindowCtlButtons()
{
	if( m_windowCtlsShown )
	{
		m_specTb->DeleteTool(IDC_REMOVE_WINDOW);
		m_specTb->DeleteTool(IDC_RENAME_WINDOW);
		m_specTb->DeleteTool(IDC_ADD_WINDOW);
		m_specTb->DeleteByIndex(m_specTb->GetToolCount()-1);	// remove left-over separator
		m_windowCtlsShown = false;
	}
}

// show|hide spectrometer windows management buttons
void EsSpecView::showWindowButtons(bool show)
{
	if( show )	
		addWindowCtlButtons();
	else
		removeWindowCtlButtons();
}

// show|hide spectrometer side panel
void EsSpecView::showSidePanel(bool show)
{
	wxASSERT(m_sideBox);
	m_sideBox->Show(show);
	
	Layout();
}

// return index of the window with specified name, wxNOT_FOUND otherwise
int EsSpecView::findWindowByName(const EsString& name) const
{
	for(size_t idx = 0; idx < m_windows.size(); ++idx)
		if( name.compare( m_windows[idx]->GetLabel().t_str() ) == 0 )
			return idx;
		
	return wxNOT_FOUND;
}

bool EsSpecView::isExistingWindowName(const EsString& name, bool doThrow) const
{
	bool result = findWindowByName(name) != wxNOT_FOUND;
	
	if( result && doThrow )
		EsException::Throw(0, EsException::Generic, _("Window with name '%s' already exists"), name.c_str());
	
	return result;
}

EsSpecView::Window::Ptr EsSpecView::getWindowAt(size_t idx)
{
	EsNumericCheck::checkRangeInteger(0, m_windows.size()-1, idx, STR_WINDOW_INDEX);
	return m_windows[idx];
}

const EsSpecView::Window::Ptr EsSpecView::getWindowAt(size_t idx) const
{
	EsNumericCheck::checkRangeInteger(0, m_windows.size()-1, idx, STR_WINDOW_INDEX);		
	return m_windows[idx];
}

EsSpecView::Window::Ptr EsSpecView::addWindow(const EsString& name)
{
	EsSpecView::Window::Ptr result;
	EsString nm = name;
	if( nm.empty() )
		nm = EsString::format( _("Window %d"), m_windows.size() );
	if( !isExistingWindowName(nm, true) )
	{
		result.reset( new Window(*this) );
		if( result.get() )
		{
			m_windows.push_back( result );
			result->SetLabel( nm );
		}
	}

	return result;	
}

// participating functor
struct IsParticipating
{
	IsParticipating(EsSpecView::Window::Ptr wnd) :
	m_wnd(wnd)
	{}
	
	bool operator() (const EsSpecView::WindowLink& link) const
	{
		return link.isParticipating(m_wnd);
	}

	EsSpecView::Window::Ptr m_wnd;
};

// add link between left and right
void EsSpecView::addWindowsLink(Window::Ptr left, Window::Ptr right)
{
	m_links.push_back( WindowLink(left, right) );
}

// remove all links in which wnd is participating
void EsSpecView::removeLinks(Window::Ptr wnd)
{
	// remove all links in which this window is participating
	std::remove_if( m_links.begin(), m_links.end(), IsParticipating(wnd) );
}

void EsSpecView::removeWindow(size_t idx)
{
	EsNumericCheck::checkRangeInteger(0, m_windows.size()-1, idx, STR_WINDOW_INDEX);
	Window::Ptr wnd = m_windows[idx];
	// first, remove all links in which this window is participating
	removeLinks(wnd);
	// last, remove window from collection
	m_windows.erase( m_windows.begin()+idx );
}

void EsSpecView::renameWindow(size_t idx, const EsString& name)
{
	EsNumericCheck::checkRangeInteger(0, m_windows.size()-1, idx, STR_WINDOW_INDEX);
	// check for existing name with throw
	isExistingWindowName(name, true);
	// remove trailing non-printables, check for empty name
	EsString nm = EsString::trim(name, wxT(" \t\n\r"));
	if( !nm.empty() )
		m_windows[idx]->SetLabel(nm);
	else
		EsException::Throw(0, EsException::Generic, _("Window name must not be neither empty, nor containing only non-printable symbols"));
}

void EsSpecView::clearSpec(bool resetScale /*= false*/)
{
	Locker lock(*this);
	m_ds->clear();
	if( resetScale )
		m_aY->SetFixedBounds(0., minMaxY);
	// reset generic info
	resetSpecInfo();
	// reset all windows
	for(size_t idx = 0; idx < m_windows.size(); ++idx)
		m_windows[idx]->reset();	
	m_needUpdateInfo = true;	
	m_needAdjustAxes = true;
}

// spectrum data access
size_t EsSpecView::getPulses(size_t chnl) const
{
	EsNumericCheck::checkRangeInteger(0, m_specResolution-1, chnl, _("Channel"));
	return static_cast<size_t>(m_ds->GetY(chnl, 0));
}

bool EsSpecView::addPulse(size_t ampl)
{
	if( ampl < m_specResolution )
	{
		size_t newVal = getPulses(ampl)+1;
		m_ds->setY(ampl, newVal);
		++m_sum;
		if(	newVal > m_max )
		{	
			m_max = newVal;
			m_maxIdx = ampl;
		}
		
		// pass received pulse to spectrometer windows
		for( size_t idx = 0; idx < m_windows.size(); ++idx )
			m_windows[idx]->onPulse( ampl, newVal );		
		
		m_needUpdateInfo = true;
		m_needAdjustAxes = true;
		
		return true;
	}
	
	return false;
}

// update spectrum info
void EsSpecView::updateSpecInfo()
{
	m_lblSum->SetLabel( wxString::Format(STR_FMT_SUM, m_sum) );
	if( 0 != m_sum )
		m_lblMax->SetLabel( wxString::Format(STR_FMT_MAX_INFO, m_max, m_maxIdx) );
	else
		m_lblMax->SetLabel( STR_DEFAULT_MAX_INFO );
}

wxArrayString EsSpecView::getAllWindowNames() const
{
	wxArrayString result;
	result.reserve(32);
	for(size_t idx = 0; idx < m_windows.size(); ++idx)
		result.push_back( m_windows[idx]->GetLabel() );
			
	return result;
}

void EsSpecView::update()
{
	// update spectrum && windows info
	if( m_needUpdateInfo )
	{
		updateSpecInfo();
		for(size_t idx = 0; idx < m_windows.size(); ++idx)
			m_windows[idx]->updateInfo(true);
		m_needUpdateInfo = false;
	}
	
	if( m_needAdjustAxes )
	{	
		// start autoscaling with default scale
		if( m_autoscaleY )
			m_aY->SetFixedBounds(0., minMaxY);
	
		// adjust axes, if needed
		if( m_max > minMaxY && m_autoscaleY )
		{
			double newMax = (m_max / minMaxY + (m_max % minMaxY ? 1 : 0)) * minMaxY;
			m_aY->SetFixedBounds(0., newMax);
		}
		
		m_needAdjustAxes = false;
	}
}

void EsSpecView::setAutoScaleY()
{
	if( !m_autoscaleY )
	{
		m_autoscaleY = true;
		m_needAdjustAxes = true;
		update();		
	}
}

void EsSpecView::setScaleY(double scale)
{
	if( scale < minMaxY )
		scale = minMaxY;
		
	m_autoscaleY = false;
	m_needAdjustAxes = true;		
	m_aY->SetFixedBounds(0., scale);
	update();
}

double EsSpecView::getScaleY() const
{
	return m_aY->getMaxBound();
}

void EsSpecView::setZoomOutConstraint(double constraint)
{
	if( constraint <= minMaxY )
	 constraint = minMaxY;
	
	m_zoomOutConstraint = constraint;
}

// window link search. if not found, return wxNOT_FOUND
int EsSpecView::findLink(Window& wnd, bool right) const
{
	for( size_t idx = 0; idx < m_links.size(); ++idx )
	{
		if( right )
		{
			if( m_links[idx].isParticipatingRight(wnd) )
				return idx;
		}
		else
		{
			if( m_links[idx].isParticipatingLeft(wnd) )
				return idx;
		}
	}
	
	return wxNOT_FOUND;
}

// window link handling
bool EsSpecView::handleLink(Window& wnd, bool pre, LinkUpdateCode code, size_t val)
{
	int idx;
	if( pre )
	{
		//// check if we actually may change window border
		//if( startChanged == code )
		//	// find link where this window is right
		//	idx = findLink(wnd, true);
		//else
		//	// find link where this window is left
		//	idx = findLink(wnd, false);
		//
		//if( wxNOT_FOUND != idx )
		//{
		//}
	}
	else
	{
		// update other window
		if( startChanged == code )
			// find link where this window is right
			idx = findLink(wnd, true);			
		else
			// find link where this window is left
			idx = findLink(wnd, false);
		
		if( wxNOT_FOUND != idx )
		{
			const WindowLink& link = m_links[idx];
			if( startChanged == code )
			{
				// change left window's right border. avoid recursion
				if( !link.m_left->m_handlingLink )
					link.m_left->setEnd(val);
			}
			else
			{
				// change right window's left border
				if( !link.m_right->m_handlingLink )
					link.m_right->setStart(val);
			}
		}
	}
	
	return true;	
}

void EsSpecView::renameWindow()
{
	wxSingleChoiceDialog dlg(this, _("Select window to rename:"), _("Rename window"), getAllWindowNames());
	if( dlg.ShowModal() == wxID_OK )
	{
		int sel = dlg.GetSelection();
		if( sel > wxNOT_FOUND )
		{
			EsString name = m_windows[sel]->GetLabel();
			wxTextEntryDialog te(this, _("Enter new window name:"), _("Enter new name"), name);
			if( te.ShowModal() == wxID_OK )
			{
				name = te.GetValue();
				renameWindow(sel, name);
			}
		}
	}
}

void EsSpecView::removeWindow()
{
	wxSingleChoiceDialog dlg(this, _("Select window to remove:"), _("Remove window"), getAllWindowNames());
	if( dlg.ShowModal() == wxID_OK )
	{
		int sel = dlg.GetSelection();
		if( sel > wxNOT_FOUND )
			removeWindow(sel);
	}
}

// events
//
void EsSpecView::onSpecCtl(wxCommandEvent& evt)
{
	switch( evt.GetId() )
	{
	case IDC_SPEC_AUTOSCALE:
		if( !getAutoScaleY() )
			setAutoScaleY();
		else
			setScaleY( getScaleY() );
		break;
	case IDC_SPEC_ZOOMIN:
		setScaleY( getScaleY() - 100. );
		break;
	case IDC_SPEC_ZOOMOUT:
		setScaleY( getScaleY() + 100. );
		break;
	case IDC_SPEC_CLEAR:
		clearSpec();
		break;
	case IDC_ADD_WINDOW:
		addWindow(EsString::s_null);
		break;
	case IDC_REMOVE_WINDOW:
		removeWindow();
		break;
	case IDC_RENAME_WINDOW:
		renameWindow();
		break;	
	}
}

void EsSpecView::onUpdateUi(wxUpdateUIEvent& evt)
{
	switch( evt.GetId() )
	{
	case IDC_SPEC_AUTOSCALE:
		evt.Check( getAutoScaleY() );
		break;
	case IDC_SPEC_ZOOMIN:
		evt.Enable( getScaleY() > EsSpecView::minMaxY );
		break;
	case IDC_SPEC_ZOOMOUT:
		evt.Enable( getScaleY() < getZoomOutConstraint() );
		break;
	case IDC_SPEC_CLEAR:
		evt.Enable( 0 != getSum() );
		break;
	case IDC_RENAME_WINDOW:
	case IDC_REMOVE_WINDOW:
		evt.Enable( !m_windows.empty() );
		break;
	}
}

void EsSpecView::saveSettings(wxConfigBase &cfg) const
{
	// save existing path
	wxString oldPath = cfg.GetPath();
	// set new path	
	cfg.SetPath(oldPath + wxT("/EsSpecView"));
	// save spectrometer windows
	cfg.Write(CFG_WINDOWS_CNT_KEY, static_cast<long>(m_windows.size()));
	for( size_t idx = 0; idx < m_windows.size(); ++idx )
		m_windows[idx]->save(cfg, EsString::format(wxT("WINDOW_%d"), idx));
	cfg.Write(CFG_RESOLUTION_KEY, static_cast<long>(m_specResolution));
	
	// m_chart settings
	cfg.Write(CFG_SPEC_COLOR_KEY, m_specBrush.GetColour().GetAsString());
	cfg.Write(CFG_SPEC_BAR_WIDTH_KEY, m_barWidth);
	// restore saved path
	cfg.SetPath(oldPath);
}

void EsSpecView::loadSettings(wxConfigBase &cfg)
{
	// save existing path
	wxString oldPath = cfg.GetPath();
	// set new path	
	cfg.SetPath(oldPath + wxT("/EsSpecView"));
	// load spectrometer windows
	long cnt = cfg.ReadLong(CFG_WINDOWS_CNT_KEY, 0);
	if( cnt < 0 )
		cnt = 0;
	for( long idx = 0; idx < cnt; ++idx )
	{
		Window::Ptr p = addWindow(EsString::s_null);
		p->load(cfg, EsString::format(wxT("WINDOW_%d"), idx));
	}
	// spectrometer settings
	long res = cfg.ReadLong(CFG_RESOLUTION_KEY, m_defSpecRes);
	// validate resolution
	if( res < static_cast<long>(m_minSpecRes) || res > static_cast<long>(m_maxSpecRes) )
		res = m_defSpecRes;
	setSpecResolution(res, true);
	// m_chart settings
	wxColor col( cfg.Read(CFG_SPEC_COLOR_KEY, wxT("blue")) );
	setSpecColor(col, true);
	setSpecBarWidth( cfg.ReadLong(CFG_SPEC_BAR_WIDTH_KEY, defLineWidth) );
	// restore saved path
	cfg.SetPath(oldPath);
}

wxBitmap EsSpecView::getAsBitmap(const wxSize& size) const
{
	wxMemoryDC mdc;
	wxBitmap bmp(size, 32);
	wxASSERT(bmp.IsOk());
	mdc.SelectObject(bmp);
	
	wxASSERT(m_chart);
	m_chart->Draw(mdc, wxRect(size));
	mdc.SelectObject(wxNullBitmap);
	return bmp; 
}
