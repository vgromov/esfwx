#ifndef  _es_math_array_view_h_
#define  _es_math_array_view_h_

/// array view class template
///
template <typename ArrayT>
class EsMathArrayView
{
// todo: add its own dirty flag and minimax
public:
	typedef typename ArrayT::value_type 		value_type;
	typedef typename ArrayT::pointer 	 			pointer;
	typedef typename ArrayT::const_pointer	const_pointer;

public:
	EsMathArrayView();
	EsMathArrayView(ArrayT& subj, size_t offs, size_t size);

	/// validity check
	bool isOk() const;

	/// emptiness check
	bool empty() const { return 0 == m_size; }

	/// view offset
	size_t offs() const { return m_offs; }
	void offsSet(size_t offs) { m_offs = offs; }

	/// view size
	size_t size() const { return m_size; }
	void sizeSet(size_t size) { m_size = size; }

	/// attach to subject
	void attach(ArrayT& subj, size_t offs, size_t size);

	/// subject item indexed access
	value_type itemGet(size_t idx) const;
	void itemSet(size_t idx, value_type val);

protected:
	ArrayT* m_subj; // NB! weak reference to the viewed subject
	size_t m_offs;
	size_t m_size;
};

//---------------------------------------------------------------------------
/// array view implementation
///
template <typename ArrayT>
EsMathArrayView<ArrayT>::EsMathArrayView() :
m_subj(0),
m_offs(0),
m_size(0)
{}

template <typename ArrayT>
EsMathArrayView<ArrayT>::EsMathArrayView(ArrayT& subj, size_t offs, size_t size) :
m_subj(0),
m_offs(0),
m_size(0)
{
	attach(subj, offs, size);
}

/// validity check
template <typename ArrayT>
bool EsMathArrayView<ArrayT>::isOk() const
{
	return m_subj && m_offs < m_subj->countGet() && m_size <= m_subj->countGet() &&
		m_offs+m_size <= m_subj->countGet(); // NB! error-prone
}

/// attach to subject
template <typename ArrayT>
void EsMathArrayView<ArrayT>::attach(ArrayT& subj, size_t offs, size_t size)
{
	m_subj = 0;
	if( size <= subj.countGet() &&
			offs < subj.countGet() &&
			size+offs <= subj.countGet()	)
	{
		m_subj = &subj;
		m_size = size;
		m_offs = offs;
	}
}

/// subject item indexed access
template <typename ArrayT>
typename EsMathArrayView<ArrayT>::value_type EsMathArrayView<ArrayT>::itemGet(size_t idx) const
{
	ES_ASSERT(isOk());
	ES_ASSERT(idx < m_size);
	return m_subj->itemGet(m_offs+idx);
}

template <typename ArrayT>
void EsMathArrayView<ArrayT>::itemSet(size_t idx, typename EsMathArrayView<ArrayT>::value_type val)
{
	ES_ASSERT(isOk());
	ES_ASSERT(idx < m_size);
	m_subj->itemSet(m_offs+idx, val);
}

#endif // _es_math_array_view_h_
