/// Internal POSIX fallback implementation of GUID generator. Do not include this file in your project directly
/// It's being implicitly included from EsUUID.cpp
///

GUID EsUUID::generate()
{
  time_t seed = time(nullptr);

  EsString hex;
  size_t processed = EsString::binToHex(
    reinterpret_cast<EsBinBuffer::const_pointer>(&seed),
    sizeof(seed),
    hex
  );
  ES_ASSERT( processed > 1 );
  esU64 hash1 = hex.hashGet();

  EsBinBuffer::pointer buff = reinterpret_cast<EsBinBuffer::pointer>(&seed);
  buff[1] |= 0x80;
  std::rotate(
    buff,
    buff+1,
    buff+sizeof(seed)
  );

  processed = EsString::binToHex(
    buff,
    sizeof(seed),
    hex
  );
  ES_ASSERT( processed > 1 );
  esU64 hash2 = hex.hashGet();

  GUID uuid;
  buff = reinterpret_cast<EsBinBuffer::pointer>(&uuid);
  ES_COMPILE_TIME_ASSERT(sizeof(GUID) == 2*sizeof(esU64), _GUID_size_is_2_times_esU64_);

  memcpy(
    buff,
    &hash1,
    sizeof(esU64)
  );
  buff += sizeof(esU64);

  memcpy(
    buff,
    &hash2,
    sizeof(esU64)
  );

  return uuid;
}
