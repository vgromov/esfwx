#ifndef _es_thread_h_ 
#define _es_thread_h_

// controlled detached thread wrapper class
//
class EKOSF_CORE_CLASS EsThread
{
public:
	// actual worker thread implementation (always use detached one),
	// so no direct access to the thread internals is available
	class EKOSF_CORE_CLASS Worker : public wxThread
	{
	protected:
		Worker(EsThread& This, size_t stack, size_t priority);
	public:	
		// thread worker
		virtual wxThread::ExitCode Entry();
		// internal check for stop condition, to be called from worker thread execution code
		// ms is time in milliseconds to wait for stop condition which is set elsewhere
		bool checkForStop(long ms);
	
	protected:
		EsThread& m_this;
		friend class EsThread;
	};
	friend class Worker;

public:
	EsThread(size_t stack = 0);
	virtual ~EsThread();

	// thread control services
	// 
	bool isStopping() const
	{
		return m_stopping;
	}
	// create and start thread if it's not already created
	// if run is false - suspend task upon creation
	void start(bool run = true, size_t priority = WXTHREAD_DEFAULT_PRIORITY);
	// suspend-resume
	void suspend();
	void resume();
	// signal stop 
	void stop();
	// signal stop and wait for thread to end
	void stopAndWait();
	// MT checks
	bool exists() const;

	// access the thread error conditions
	//
	long getExitCode() const;
	EsString::Array getErrorLog() const;
	
	// thread indentification
	wxThreadIdType getId() const;

protected:
	// thread worker body. to be implemented in derived classes
	virtual long worker(EsThread::Worker& thread) = 0;
	virtual void onEnterWorker() {}
	virtual void onExitWorker() {}
	// internal services
	void setId(wxThreadIdType id);
	void setResult(long error, const EsString& msg);
	void resetStopCondition();

private:
	// prohibited functionality
	EsThread(const EsThread&);
	EsThread& operator= (const EsThread&);

protected:
	// thread control
	//
	// MT access guard
	mutable wxCriticalSection m_cs;
	// stop signal semaphore
	bool m_stopping;
	wxSemaphore m_stop;
	// recently executed thread error status
	long m_errorCode;
	// requested stack size
	size_t m_stack;
	// current worker instance
	Worker* m_inst;
#pragma warning(push)
#pragma warning(disable: 4251)
	// recently executed thread error log
	EsString::Array m_errorLog;
#pragma warning(pop)
};

#endif // _es_thread_h_