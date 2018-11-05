// This file must not be included in project directly
// It's implicitly included in EsDateTime.cpp
//

static esDT internalNow()
{
	esDT _now;

  timeval t;
  gettimeofday(&t, NULL);
  tm _tm;
  _tm = *localtime(&t.tv_sec);
 	dtComposeDateTime(&_now, _tm.tm_year+1900, _tm.tm_mon+1, _tm.tm_mday, _tm.tm_hour, _tm.tm_min, _tm.tm_sec, t.tv_usec/1000);

	return _now;
}

// UTC bias
EsTimeSpan EsDateTime::bias()
{
  EsTimeSpan ts;


  return ts;
}

bool EsDateTime::isDstActive()
{

  return false;
}
