#ifndef _Mcu_Programmer_h_
#define _Mcu_Programmer_h_

// McuProgrammer interface && concrete impl. factory
class MCUPROG_CLASS McuProgrammer
{
public:
	enum Type {
		// supported programmers impl.
		//
		// NXP chips
		LPC2000,

	};

public:
	// common interface
	virtual ~McuProgrammer() {}
	// program mcu over chnl using firmware moniker. return 0 if device was programmed successfully
	// otherwise, device-dependent error code is returned
	virtual long program(const EsIoChannelIntf::Ptr& chnl, const EsFirmwareMoniker& fw,
		const EsLogIntf::Ptr& logger = EsLogIntf::Ptr(), const ProgressMonitorIntf::Ptr& progressMonitor = ProgressMonitorIntf::Ptr()) = 0;
	// return programmer- specific string interpretation of error code
	virtual EsString getErrorString(long err) const = 0;
	// return type of platform supported by programmer interface
	McuProgrammer::Type getType() const { return m_type; }
	// static services
	//
	// impl. factory
	static McuProgrammer* create(McuProgrammer::Type type);

protected:
	// functionality for derived classes only
	McuProgrammer(McuProgrammer::Type type) : m_type(type) {}
	
protected:
	// data members
	McuProgrammer::Type m_type;

private:
	// disallowed functionality
	McuProgrammer(const McuProgrammer&);
	McuProgrammer& operator =(const McuProgrammer&);
};

#endif // _Mcu_Programmer_h_