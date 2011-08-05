#include <stdafx.h>
#pragma hdrstop

#include "EsThread.h"

EsThread::EsThread(size_t stack /*= 0*/) :
m_stopping(false),
m_stop(0, 1),
m_errorCode(0),
m_stack(stack),
m_inst(0)
{
}

EsThread::~EsThread()
{
	stopAndWait();
}

// thread control services
// 
void EsThread::resetStopCondition()
{
	// reset stop condition
	m_stopping = false;
	m_stop.TryWait();
}

// create and start thread if it's not already created
void EsThread::start( bool run, size_t priority )
{
	if( m_inst )
		EsException::Throw(0, EsException::Generic, _("Thread is already executing"));
	else if( !m_stopping )
	{
		// re-initialize semaphores && misc members
		//
		// reset error
		m_errorCode = 0;
		// reset error log
		m_errorLog.clear();
		// create and run worker thread
		m_inst = new Worker(*this, m_stack, priority);
		if( !run )
			m_inst->Pause();
	}
	else
		EsException::Throw(0, EsException::Generic, _("Thread is in stopping stage and could not be re-started until after termination"));
}

// suspend-resume
void EsThread::suspend()
{
	if( m_inst && 
			!m_inst->IsPaused() )
		m_inst->Pause();
}

void EsThread::resume()
{
	if( m_inst &&
			m_inst->IsPaused() )
		m_inst->Resume();
}

void EsThread::stop()
{
	if( m_inst )
	{
		m_stopping = true;
		m_stop.Post();
	}
}

// signal stop and wait for thread to end
void EsThread::stopAndWait()
{
	if( m_inst )
	{
		m_stopping = true;
		m_stop.Post();
		// also, ask thread to delete
		m_inst->Delete();
		// finally, delete and zero-out thread instance
		wxDELETE(m_inst);
		// reset stop condition
		resetStopCondition();
	}
}

// MT checks
bool EsThread::exists() const
{
	return 0 != m_inst;
}

// access the thread error conditions
//
long EsThread::getExitCode() const
{
	wxCriticalSectionLocker lock(m_cs);
	return m_errorCode;
}

EsString::Array EsThread::getErrorLog() const
{
	wxCriticalSectionLocker lock(m_cs);
	return m_errorLog;
}

// thread indentification
wxThreadIdType EsThread::getId() const
{
	if( m_inst )
		return m_inst->GetId();
	
	return -1;
}

void EsThread::setResult(long error, const EsString& msg)
{
	wxCriticalSectionLocker lock(m_cs);
	m_errorCode = error;
	if( !msg.empty() )
		m_errorLog.push_back(msg);
}

// actual worker thread implementation (always use detached one),
// so no direct access to the thread internals is available
//	
// ctor
EsThread::Worker::Worker(EsThread& This, size_t stack, size_t priority) : 
wxThread(wxTHREAD_JOINABLE),
m_this(This)
{
	// create & run thread
	if( wxTHREAD_NO_ERROR != Create(stack) )
		EsException::Throw(0, EsException::Generic, _("Cannot create worker thread"));
	SetPriority(priority);
	if( wxTHREAD_NO_ERROR != Run() )
		EsException::Throw(0, EsException::Generic, _("Cannot launch worker thread"));
}

// internal check for stop condition, to be called from worker thread execution code
// ms is time in milliseconds to wait for stop condition to be set elsewhere
bool EsThread::Worker::checkForStop(long ms)
{
	bool result = TestDestroy();
	if( !result &&
			wxSEMA_NO_ERROR == m_this.m_stop.WaitTimeout(ms) )
	{
		m_this.m_stop.Post(); // allow multiple stop re-query
		result = true;
	}
			
	return result;
}

// generic worker
wxThread::ExitCode EsThread::Worker::Entry()
{
	long error = 0;
	EsString errorMsg;
	try
	{
		m_this.onEnterWorker();
		error = m_this.worker( *this );
	}
	catch( const EsException& ex )
	{
		error = ex.getCode();
		errorMsg = ex.getMsg();
	}
	catch( const std::exception& stdex )
	{
		error = -1;
		errorMsg = EsString::fromUtf8(stdex.what());
	}
	catch(...)
	{
		error = -1;
		errorMsg = _("Unknown exception occured in worker thread");
	}
	m_this.setResult(error, errorMsg);	
	
	try
	{
		m_this.onExitWorker();
	}
	catch( const EsException& ex )
	{
		error = ex.getCode();
		errorMsg = ex.getMsg();
		m_this.setResult(error, errorMsg);		
	}
	catch( const std::exception& stdex )
	{
		error = -1;
		errorMsg = EsString::fromUtf8(stdex.what());
		m_this.setResult(error, errorMsg);		
	}
	catch(...)
	{
		error = -1;
		errorMsg = _("Unknown exception occured upon exiting worker thread");
		m_this.setResult(error, errorMsg);
	}
	
	m_this.m_stop.Post();	

	return reinterpret_cast<wxThread::ExitCode>(error);
}