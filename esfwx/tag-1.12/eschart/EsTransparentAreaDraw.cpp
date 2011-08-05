#include "stdafx.h"
#pragma hdrstop

#include "EsTransparentAreaDraw.h"

EsTransparentAreaDraw::EsTransparentAreaDraw(wxColor cl, BYTE opacity) :
AreaDraw(),
m_color(cl),
m_opacity(opacity),
m_updateImg(true),
m_pen(cl)
{}

void EsTransparentAreaDraw::Draw(wxDC& dc, wxRect rc)
{
	// 1 - create bitmap with transparency, if needed
	if( m_rcOld != rc || m_updateImg )
	{
		m_img.Destroy();
		if( rc.GetWidth() > 2 )
		{
			size_t w = rc.GetWidth()-2;
			size_t h = rc.GetHeight();
			size_t pixelCnt = w*h;
			if( pixelCnt )
			{
				m_imgData.resize(pixelCnt);
				m_imgAlpha.resize(pixelCnt);
				wxImage::RGBValue rgb(m_color.Red(), m_color.Green(), m_color.Blue());
				for(size_t idx = 0; idx < pixelCnt; ++idx)
				{
					m_imgData[idx] = rgb;
					m_imgAlpha[idx] = m_opacity;
				}
				m_img.Create(w, h, reinterpret_cast<BYTE*>(&m_imgData[0]), &m_imgAlpha[0], true);
			}
		}
		m_rcOld = rc;
		m_updateImg = false;
	}

	if( m_img.IsOk() )
	{
		wxBitmap bm( m_img );
		dc.DrawBitmap( bm, rc.GetLeft()+1, rc.GetTop());
	}

	dc.SetPen(m_pen);
	dc.DrawLine(rc.GetLeftTop(), rc.GetLeftBottom());
	if( 0 != rc.GetWidth() )
		dc.DrawLine(rc.GetRightTop(), rc.GetRightBottom());
	dc.SetPen(wxNullPen);
}	