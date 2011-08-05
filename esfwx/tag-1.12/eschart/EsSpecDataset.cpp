#include "stdafx.h"
#pragma hdrstop

#include "EsSpecDataset.h"

EsSpecDataset::EsSpecDataset() : 
XYDataset(),
m_data(1),
m_maxY(0.),
m_maxX(0)
{}

void EsSpecDataset::setSize(size_t sze)
{
	if( sze > 0 && sze != m_data.size() )
	{
		m_data.resize(sze);
		clear();
	}
}

void EsSpecDataset::setY(size_t idx, double y)
{
	if(idx < m_data.size()) 
	{
		m_data[idx] = y;
		// update minimax
		if( m_maxY < y )
		{
			m_maxY = y;
			m_maxX = idx;
		}
		DatasetChanged();
	}
}

void EsSpecDataset::clear()
{
	std::fill( m_data.begin(), m_data.end(), 0. );
	m_maxY = 0.;
	m_maxX = 0;
	DatasetChanged();
}