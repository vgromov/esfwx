#ifndef _es_spec_dataset_h_
#define _es_spec_dataset_h_

// specific implementation of dataset for spectrum-oriented usage
//
class EKOSF_CHART_CLASS EsSpecDataset : public XYDataset
{
public:
	EsSpecDataset();

	// XY dataset API overrides
	virtual size_t GetSerieCount() { return 1; }
	virtual size_t GetCount(size_t serie = 0) { return m_data.size(); }
	virtual wxString GetSerieName(size_t serie)
	{
		return wxEmptyString;
	}
	virtual double GetX(size_t index, size_t serie = 0) 
	{ 
		wxASSERT(index < m_data.size());
		return static_cast<double>(index); 
	}
	virtual double GetY(size_t index, size_t serie = 0) 
	{ 
		wxASSERT(index < m_data.size());
		return m_data[index];
	}
	virtual double GetMaxY() { return m_maxY; }
	virtual double GetMinY() { return 0.; }
	virtual double GetMaxX() { return m_maxX; }
	virtual double GetMinX() { return 0.; }

	/**
	 * Set size of dataset. New dataset is cleared
	 * @param sze new size
	 */
	void setSize(size_t sze);

	/**
	 * Set y value at specified index.
	 * @param idx index of value
	 * @param y new y value
	 */
	void setY(size_t idx, double y);
	
	/**
	 * Set all values to zero.
	 * Do not change current size of dataset
	 */
	void clear();

private:
	// data members
	std::vector<double> m_data;
	double m_maxY; // maximum data value
	size_t m_maxX; // index of maximum data value
};

#endif // _es_spec_dataset_h_
