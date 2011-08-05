#include "stdafx.h"
#pragma hdrstop

#include "McuLpcProgrammer.h"
#include "lpc21isp.h"

#define STR_ACT_SYNCHING					_("Synchronizing...")
#define STR_PROGRAM_UPLOAD_ERROR	_("Could not upload program")

// C++ wrapper around ISP_ENVIRONMENT
struct IspEnv : public ISP_ENVIRONMENT
{
	enum { 
		BASIC_TMO = 100, 
		SYNC_RETRIES = 128,
	};

	IspEnv(	const EsLogIntf::Ptr& logger, const ProgressMonitorIntf::Ptr& progress, const EsFirmwareMoniker& fw ) :
	m_progress(progress), 
	m_logger(logger)
	{ 
		memset(this, 0, sizeof(ISP_ENVIRONMENT)); 
		m_img = fw.getBin();
		// align image size on 4*45 bytes

		pRxTmpBuf = RxTmpBuf;
		// Initialize ISP Environment defaults
		micro       = PHILIPS_ARM;      // Default Micro
		FileFormat  = FORMAT_HEX;       // Default File Format
		ProgramChip = TRUE;             // Default to Programming the chip
		nQuestionMarks = SYNC_RETRIES;	// How many retries to make while synching
		// assign binary contents
		BinaryContent = reinterpret_cast<BINARY*>(
			const_cast<BYTE*>(&m_img.getImage()[0]));
		BinaryLength = m_img.getImage().size();
		BinaryOffset = m_img.getBinaryOffs();
		StartAddress = m_img.getStartAddr();
	}

	ProgressMonitorIntf::Ptr m_progress; 
	EsLogIntf::Ptr m_logger;
	char RxTmpBuf[256];        // save received data to this buffer for half-duplex
	char* pRxTmpBuf;
	EsFirmwareBinary m_img;
};

// functional wrappers|stubs for lpcprog.c
static void AppWritten( ISP_ENVIRONMENT* env, int chunkSize )
{
	IspEnv* ispEnv = reinterpret_cast<IspEnv*>(env);
	if(ispEnv && ispEnv->m_progress)
		ispEnv->m_progress->setPos( ispEnv->m_progress->getPos() + chunkSize ); 
}

static void AppException(int exception_level)
{
	EsException::Throw(exception_level, EsException::Generic, wxT("exit"));
}

static int AppSyncing( ISP_ENVIRONMENT* env, int trials )
{
	IspEnv* ispEnv = reinterpret_cast<IspEnv*>(env);
	if(ispEnv && ispEnv->m_progress)
	{ 
		if( trials != -1 )
		{
			if( ispEnv->m_progress->getName() != static_cast<EsString>(STR_ACT_SYNCHING) )
				ispEnv->m_progress->startAction(STR_ACT_SYNCHING, 10);
			ispEnv->m_progress->pulse(); 
		}
		else
			ispEnv->m_progress->startAction(_("Downloading firmware..."), env->BinaryLength);		
	}

	return 1;
}
#define ResetTarget(IspEnv, ResetMode)
static void AppDebugPrintf(ISP_ENVIRONMENT* env, int level, const char *fmt, ...)
{
	va_list argptr;
	va_start(argptr, fmt);

	int len = _vscprintf( fmt, argptr );
	std::vector<char> str(len+1, 0);
	vsprintf( &str[0], fmt, argptr );
	EsString msg(str.begin(), str.end());

	if( !msg.empty() )
	{
		IspEnv* ispEnv = reinterpret_cast<IspEnv*>(env);

		if( ispEnv->m_logger && 2 < msg.size() )
		{
			EsLogIntf::Level lvl = EsLogIntf::LevelError;
			if( level < 4 )
			{
				if( level > 1  )
					lvl = EsLogIntf::LevelInfo;
				ispEnv->m_logger->append(lvl, msg);
			}
		}
	}
}
static void DumpString(ISP_ENVIRONMENT* env, int level, const void *b, size_t size, const char *prefix_string)
{
}
static void PrepareKeyboardTtySettings() {}
static void ResetKeyboardTtySettings() {}
static void SendComPort(ISP_ENVIRONMENT *env, const char *s)
{
	IspEnv* ispEnv = reinterpret_cast<IspEnv*>(env);
	EsIoChannelIntf* io = reinterpret_cast<EsIoChannelIntf*>(env->hCom);
	DWORD written;
	DWORD pending = strlen(s); 
	DWORD read;
	
	if( !io->isOpen() ) // break thread
		EsException::Throw(0, EsException::Generic, _("Cancelling device programming"));
	
	if(env->HalfDuplex == 0)
		written = io->putBytes(reinterpret_cast<const BYTE*>(s), pending, 500, 0);
	else
	{
		BYTE* rxpch = reinterpret_cast<BYTE*>(ispEnv->RxTmpBuf);
		// reset global pointer
		ispEnv->pRxTmpBuf = ispEnv->RxTmpBuf;

		// avoid buffer otherflow
		if(pending > sizeof(ispEnv->RxTmpBuf))
			pending = sizeof(ispEnv->RxTmpBuf);

		for(size_t m = 0; m < pending; m++)
		{
			if( !io->isOpen() ) // break thread
				EsException::Throw(0, EsException::Generic, _("Cancelling device programming"));
		
			written = io->putBytes( reinterpret_cast<const BYTE*>(s), 1, 500, 0);
			if((*s != '?') || (pending != 1))
			{
				do
				{
					read = io->getBytes(rxpch, 1, 0, 0);

				}while (read == 0);
			}
			s++;
			rxpch++;
		}
		*rxpch = 0;        // terminate echo string
	}
}

static void SerialTimeoutTick(ISP_ENVIRONMENT *IspEnvironment)
{
	if (IspEnvironment->serial_timeout_count <= 1)
	{
		IspEnvironment->serial_timeout_count = 0;
	}
	else
	{
		IspEnvironment->serial_timeout_count--;
		Sleep(1);
	}
}

static void SerialTimeoutSet(ISP_ENVIRONMENT *IspEnvironment, unsigned timeout_milliseconds)
{
	IspEnvironment->serial_timeout_count = timeout_milliseconds;
}

static int SerialTimeoutCheck(ISP_ENVIRONMENT *IspEnvironment)
{
	if(IspEnvironment->serial_timeout_count == 0)
		return 1;
		
	return 0;
}

static void ReceiveComPortBlock(ISP_ENVIRONMENT* env,
																void *answer, unsigned long max_size,
																unsigned long *real_size)
{
	IspEnv* ispEnv = reinterpret_cast<IspEnv*>(env);
	EsIoChannelIntf* io = reinterpret_cast<EsIoChannelIntf*>(env->hCom);

	if( !io->isOpen() ) // break thread
		EsException::Throw(0, EsException::Generic, _("Cancelling device programming"));

	if(env->HalfDuplex == 0)
		*real_size = io->getBytes(reinterpret_cast<BYTE*>(answer), max_size, 0, 0);
	else
	{
		*real_size = strlen(ispEnv->pRxTmpBuf);
		if(*real_size)
		{
			if(max_size >= *real_size)
			{
				strncpy((char*)answer, ispEnv->pRxTmpBuf, *real_size);
				ispEnv->RxTmpBuf[0] = 0;
				// reset global pointer
				ispEnv->pRxTmpBuf = ispEnv->RxTmpBuf;
			}
			else
			{
				strncpy((char*) answer, ispEnv->pRxTmpBuf, max_size);
				*real_size = max_size;
				// increment global pointer
				ispEnv->pRxTmpBuf += max_size;
			}
		}
		else
			*real_size = io->getBytes(reinterpret_cast<BYTE*>(answer), max_size, 0, 0);
	}

	if(*real_size == 0)
	{
		SerialTimeoutTick(env);
	}
}

static void ReceiveComPort(ISP_ENVIRONMENT* env,
													 const char *Ans, unsigned long MaxSize,
													 unsigned long *RealSize, unsigned long WantedNr0x0A,
													 unsigned timeOutMilliseconds)
{
	unsigned long tmp_realsize = 0;
	unsigned long nr_of_0x0A = 0;
	int eof = 0;
	unsigned long p;
	unsigned char *Answer = (unsigned char*) Ans;
	(*RealSize) = 0;
	
	SerialTimeoutSet(env, timeOutMilliseconds);

	do
	{
		ReceiveComPortBlock(env, Answer + (*RealSize), MaxSize - 1 - (*RealSize), &tmp_realsize);

		if (tmp_realsize != 0)
		{
			for (p = (*RealSize); p < (*RealSize) + tmp_realsize; p++)
			{
				if (Answer[p] == 0x0a)
				{
					nr_of_0x0A++;
				}
				else if (((signed char) Answer[p]) < 0)
				{
					eof = 1;
				}
			}

			(*RealSize) += tmp_realsize;
		}

	} while (((*RealSize) < MaxSize) && !SerialTimeoutCheck(env) && (nr_of_0x0A < WantedNr0x0A) && !eof);

	Answer[(*RealSize)] = 0;
}

// 3-rd party lpc programming code directly included
#include "lpcprog.c"

// program mcu over chnl using firmware moniker
long McuLpc2kProgrammer::program(const EsIoChannelIntf::Ptr& chnl, const EsFirmwareMoniker& fw,
		const EsLogIntf::Ptr& logger /*= EsLogIntf::Ptr()*/, const ProgressMonitorIntf::Ptr& progressMonitor /*= ProgressMonitorIntf::Ptr()*/)
{
	IspEnv env(logger, progressMonitor, fw);
	// erase entire flash before programming ?
	env.WipeDevice = fw.getValueDef(wxT("wipe"), false).asBool();
	// verify device after programming ?
	env.Verify = fw.getValueDef(wxT("verify"), false).asBool();
	// use half-duplex mode ?
	env.HalfDuplex = fw.getValueDef(wxT("half_duplex"), false).asBool();
	// apply crp to binary image
	if( fw.getCrp() )
	{
		size_t crpAddr = 0x01FC;
		DWORD crp = 0x87654321;
		if( crpAddr + 4 > env.BinaryLength )
			EsException::Throw(0, EsException::Generic, _("CRP block does not fit firmware image"));
		memcpy(env.BinaryContent+crpAddr, &crp, 4);
	}

	// target oscillator frequency in khz
	long osc = fw.getValue(wxT("osc")).asLong();
	if( osc < 100000 && osc > 0 )
	{	
		std::string oscStr = EsString::fromLong(osc).toAscii();
		strncpy(env.StringOscillator, oscStr.c_str(), sizeof(env.StringOscillator));
	}
	else
		EsException::Throw(0, EsException::Generic, _("Wrong oscillator frequency value: %d"), osc);

	// set-up environent & do programming
	env.hCom = reinterpret_cast<HANDLE>(chnl.get());
	return PhilipsDownload(&env);
}

// return programmer- specific string interpretation of error code
EsString McuLpc2kProgrammer::getErrorString(long err) const
{
	if( err == 0 )
		return EsString::s_null;
		
	if( err <= USER_ABORT_SYNC )
	{
		switch( err )
		{
		case NO_ANSWER_WDT:
			return _("No answer on WDT command");
		case NO_ANSWER_QM:
			return _("No answer on QM command");
		case NO_ANSWER_SYNC:
			return _("No answer on SYNC command");
		case NO_ANSWER_OSC:
			return _("No answer on OSC command");
		case ERROR_WRITE_DATA:
			return _("Could not write data");		
		case ERROR_WRITE_CRC:
		case ERROR_WRITE_CRC2:
			return _("Could not write CRC");			
		case PROGRAM_TOO_LARGE:
			return _("Program is too large");
		case USER_ABORT_SYNC:
			return _("User aborted synchronization");
		default:
			return STR_PROGRAM_UPLOAD_ERROR;
		}
	}
	else if( err < WRONG_ANSWER_PREP )
		return EsString::format( _("Unlock error, %d"), err-UNLOCK_ERROR );
	else if( err < WRONG_ANSWER_ERAS )
		return EsString::format( _("Wrong answer to PREP command, %d"), err-WRONG_ANSWER_PREP );
	else if( err < WRONG_ANSWER_WRIT )
		return EsString::format( _("Wrong answer to ERAS command, %d"), err-WRONG_ANSWER_ERAS );
	else if( err < WRONG_ANSWER_PREP2 )
		return EsString::format( _("Wrong answer to WRIT command, %d"), err-WRONG_ANSWER_WRIT );	
	else if( err < WRONG_ANSWER_COPY )
		return EsString::format( _("Wrong answer to PREP2 command, %d"), err-WRONG_ANSWER_PREP2 );	
	else if( err < FAILED_RUN )
		return EsString::format( _("Wrong answer to COPY command, %d"), err-WRONG_ANSWER_COPY );	
	else
		return STR_PROGRAM_UPLOAD_ERROR;
}
