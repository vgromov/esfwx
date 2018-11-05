//---------------------------------------------------------------------------

EsByteString EsString::cfStringRefToByteString(CFStringRef cfstr)
{
  EsByteString bstr;

  if( cfstr )
  {
    ulong required = CFStringGetMaximumSizeForEncoding(
      CFStringGetLength(cfstr),
      kCFStringEncodingUTF8
    );
    bstr.resize(required);

    if( CFStringGetCString(cfstr, &bstr[0], required, kCFStringEncodingUTF8) )
    {
      size_t pos = bstr.find('\0');
      bstr.erase( pos );
    }
    else
      bstr.clear();
  }

  return bstr;
}
//---------------------------------------------------------------------------

EsString EsString::cfStringRefToString(CFStringRef cfstr)
{
  return EsString::fromUtf8(
    cfStringRefToByteString(cfstr)
  );
}
//---------------------------------------------------------------------------

CFStringRef EsString::toCfStringRef(const EsByteString& bstr, int conversion /*= EsString::UTF8*/)
{
  CFStringEncoding enc;
  EsByteString tmp;
  EsByteString::const_pointer src = bstr.c_str();

  switch(conversion)
  {
  case EsString::UTF8:
    enc = kCFStringEncodingUTF8;
    break;
  case EsString::CP1251:
    {
      tmp = EsStringConverter::convGet("UTF-8", "CP1251")->cToC(bstr);
      src = tmp.c_str();
    }
    enc = kCFStringEncodingUTF8;
    break;
  case EsString::ASCII:
    enc = kCFStringEncodingASCII;
    break;
  default:
    EsException::Throw(esT("Encoding '%d' is not supported"), conversion);
  }

  return CFStringCreateWithCString(
    NULL,
    src,
    enc
  );
}
//---------------------------------------------------------------------------

CFStringRef EsString::toCfStringRef(const EsString& str)
{
  return toCfStringRef(EsString::toUtf8(str), EsString::UTF8);
}
//---------------------------------------------------------------------------


