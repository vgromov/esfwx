#ifndef _Mcu_Lpc_Programmer_h_
#define _Mcu_Lpc_Programmer_h_

class McuLpc2kProgrammer : public McuProgrammer
{
protected:
	// friends only functionality
	McuLpc2kProgrammer() : McuProgrammer(LPC2000) {}

public:
	// interface methods
	virtual long program(const EsIoChannelIntf::Ptr& chnl, const EsFirmwareMoniker& fw,
		const EsLogIntf::Ptr& logger = EsLogIntf::Ptr(), const ProgressMonitorIntf::Ptr& progressMonitor = ProgressMonitorIntf::Ptr());
	// return programmer- specific string interpretation of error code
	virtual EsString getErrorString(long err) const;

	friend class McuProgrammer;
};

#endif // _Mcu_Lpc_Programmer_h_
