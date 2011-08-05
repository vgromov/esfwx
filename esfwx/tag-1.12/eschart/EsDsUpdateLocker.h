#ifndef _es_ds_update_locker_h_
#define _es_ds_update_locker_h_

// dataset update locker helper
class EsDsUpdateLocker
{
public:
	EsDsUpdateLocker(Dataset* ds) :
	m_ds(ds)
	{
		wxASSERT(m_ds);
		m_ds->BeginUpdate();
	}
	
	~EsDsUpdateLocker()
	{
		m_ds->EndUpdate();
	}

protected:
	Dataset* m_ds;
	
private:
	EsDsUpdateLocker();
	EsDsUpdateLocker(const EsDsUpdateLocker&);
	EsDsUpdateLocker& operator= (const EsDsUpdateLocker&);
};

#endif