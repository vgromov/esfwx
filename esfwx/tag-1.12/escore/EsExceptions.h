#ifndef _exceptions_h_
#define _exceptions_h_

// generic exception class for ekosfera shell
class EKOSF_CORE_CLASS EsException
{
public:
	enum Type	{
		Critical,
		Generic,
		Warning,
	};
	
protected:
	EsException(int code = 0, Type type = Generic, const EsString& msg = EsString::s_null) : 
		m_code(code), m_type(type), m_msg(msg) {}

public:	
	int getCode() const { return m_code; }
	Type getType() const { return m_type; }	
	const EsString& getMsg() const { return m_msg; }
	
	// static services
	static void Throw(int code, Type type, EsString::const_pointer fmt, ...);
	// standard exceptions
	static void ThrowNoValue();
	static void ThrowCannotIndexItem();
	static void ThrowNotSupportedForThisType();
	static void ThrowDivisionByZero();
	static void ThrowOverflowInOperation(const EsString& op);
	static void ThrowUnderflowInOperation(const EsString& op);
	static void ThrowObjectTypeIsIncompatibleWith(const EsString& className);
	static void ThrowUserDoesNotExist(const EsString& name);
	static void ThrowUserAlreadyExists(const EsString& name);
	static void ThrowAddingUsersWithAslIsNotAllowed(AppSecurityLevel asl);
	static void ThrowPasswordNotValid();
	static void ThrowUsersMayOnlyChangeThemselves();
	static void ThrowFinalObjectMayNotBeModified(const EsString& typeName);
	static void ThrowPodObjectMayNotContainFields(const EsString& typeName);
	static void ThrowFieldAlreadyDeclared(const EsString& fieldName, const EsString& typeName);
	static void ThrowAttributeDoesNotExist(const EsString& attribute, const EsString& typeName);
	
protected:
	Type m_type;
	int	m_code;		
	EsString m_msg;
};

// helper class for numeric checks
//
class EKOSF_CORE_CLASS EsNumericCheck
{
public:
	static void checkRangeFloat(double min, double max, double val, const EsString& name);
	static inline void checkRangeFloat(double min, double max, double val) { checkRangeFloat(min, max, val, EsString::s_null); }
	static void checkRangeInteger(long min, long max, long val, const EsString& name);
	static inline void checkRangeInteger(long min, long max, long val) { checkRangeInteger(min, max, val, EsString::s_null); }

private:
	EsNumericCheck();
	EsNumericCheck(const EsNumericCheck&);
	EsNumericCheck& operator= (const EsNumericCheck&);
};

#endif // _exceptions_h_