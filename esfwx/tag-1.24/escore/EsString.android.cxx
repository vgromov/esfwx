#if ES_COMPILER_VENDOR == ES_COMPILER_VENDOR_BORLAND

#include <Androidapi.Helpers.hpp>

//---------------------------------------------------------------------------
EsByteString EsString::jstringToByteString(JStringPtr str)
{
  const EsByteString& bstr = EsString::toUtf8(
    JStringToString(str).c_str()
  );

  return bstr;
}
//---------------------------------------------------------------------------

EsString EsString::jstringToString(JStringPtr str)
{
  return JStringToString(str).c_str();
}
//---------------------------------------------------------------------------

JStringPtr EsString::toJstring(const EsByteString& bstr, int conversion /*= EsString::UTF8*/)
{
  const EsString& tmp = EsString::fromByteString(bstr, conversion);

  return StringToJString( tmp.c_str() );
}
//---------------------------------------------------------------------------

JStringPtr EsString::toJstring(const EsString& str)
{
  return StringToJString( str.c_str() );
}
//---------------------------------------------------------------------------

#else

//---------------------------------------------------------------------------
EsByteString EsString::jstringToByteString(JStringPtr str)
{
  JNIEnv* env = EsUtilities::jniEnvGet();
  ES_ASSERT(env);

  EsByteString bstr;
  if( !str )
    return bstr;

  const jsize len = env->GetStringUTFLength(str);
  const char* chars = env->GetStringUTFChars(str, (jboolean*)0);

  bstr.assign( chars, len );

  env->ReleaseStringUTFChars(str, chars);

  return bstr;
}
//---------------------------------------------------------------------------

EsString EsString::jstringToString(JStringPtr str)
{
  return EsString::fromUtf8(
    jstringToByteString(str)
  );
}
//---------------------------------------------------------------------------

JStringPtr EsString::toJstring(const EsByteString& bstr, int conversion /*= EsString::UTF8*/)
{
  JNIEnv* env = EsUtilities::jniEnvGet();
  ES_ASSERT(env);

  EsByteString tmp;
  EsByteString::const_pointer src = bstr.c_str();

  switch(conversion)
  {
  case EsString::UTF8:
    break;
  case EsString::CP1251:
    tmp = EsStringConverter::convGet("UTF-8", "CP1251")->cToC(bstr);
    src = tmp.c_str();
    break;
  case EsString::ASCII:
    tmp = EsStringConverter::convGet("ITF-8", "ASCII")->cToC(bstr);
    src = tmp.c_str();
    break;
  default:
    EsException::Throw(esT("Encoding '%d' is not supported"), conversion);
  }

  return env->NewStringUTF(src);
}
//---------------------------------------------------------------------------

JStringPtr EsString::toJstring(const EsString& str)
{
  return toJstring(EsString::toUtf8(str), EsString::UTF8);
}
//---------------------------------------------------------------------------

#endif


