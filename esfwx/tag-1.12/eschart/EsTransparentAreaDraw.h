#ifndef _es_transparent_area_draw_h_
#define _es_transparent_area_draw_h_

// range draw with thransparency
//
class EKOSF_CHART_CLASS EsTransparentAreaDraw : public AreaDraw
{
public:
	EsTransparentAreaDraw(wxColor cl, BYTE opacity);
	virtual ~EsTransparentAreaDraw() {}
	// base class override
	virtual void Draw(wxDC& dc, wxRect rc);
	
	// properties access
	//
	void setColor(wxColor cl)
	{
		if( m_color != cl )
		{
			m_color = cl;
			m_pen.SetColour(cl);
			m_updateImg = true;
		}
	}
	wxColor getColor() const
	{
		return m_color;
	}
	void setOpacity(BYTE opacity)
	{
		if( m_opacity != opacity )
		{
			m_opacity = opacity;
			m_updateImg = true;			
		}
	}
	BYTE getOpacity() const
	{
		return m_opacity;
	}
	
private:
	EsTransparentAreaDraw();
	EsTransparentAreaDraw(const EsTransparentAreaDraw&);
	EsTransparentAreaDraw& operator= (const EsTransparentAreaDraw&);	
		
protected:
	wxColor m_color;
	BYTE m_opacity;	
	bool m_updateImg;
	// cached values
	wxRect m_rcOld;
	wxImage m_img;
	std::vector<wxImage::RGBValue> m_imgData;
	std::vector<BYTE> m_imgAlpha;
	wxPen m_pen;
};

#endif // _es_transparent_area_draw_h_
