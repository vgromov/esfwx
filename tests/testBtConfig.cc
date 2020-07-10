// BT SENA module configuration test
//

#define BT_BUFF_LEN       64
static char s_btBuff[BT_BUFF_LEN];

enum {
  atFail = -1,        ///< Command|response could not be received (may be due to timeout)
  atOk,               ///< Command was properly sent|Response line waas read OK
  atNotEnoughBuff,    ///< Response buffer was overflown
};

typedef struct {
  // order
  esU32 order;
  // year
  esU16 year;
  // month
  esU8 month;
  // day of month
  esU8 day;
  // version
  esU8 verMajor;
  esU8 verMinor;
  // reserved block
  esU8 reserved[54];

} FwInfo;

const FwInfo c_fwInfo = {
  // order
  0,
  // year
  2014,
  // month
  7,
  // day
  16,
  // version
  2,
  0,
  // reserved block follows - leave as is
};

int atCommandSend(const EsChannelIoIntf::Ptr& chnl, const char* cmd, esU32 cmdLen,
  const char* term, esU32 termLen)
{
  // substitute the default command terminator
  if( 0 == term || termLen != 1 )
  {
    term = "\r";
    termLen = 1;
  }

  if( 2 == chnl->bytesPut((const esU8*)"AT", 2, 0) )
  {
    esBL ok = TRUE;
    if( cmd &&
        cmdLen )
      ok = cmdLen == chnl->bytesPut((const esU8*)cmd, cmdLen, 0);

    if( ok &&
        termLen == chnl->bytesPut((const esU8*)term, termLen, 0) )
      return atOk;
  }

  return atFail;
}
//---------------------------------------------------------------------------

int atCommandRepeat(const EsChannelIoIntf::Ptr& chnl)
{
  if( 2 == chnl->bytesPut((const esU8*)"A/", 2, 0) )
    return atOk;

  return atFail;
}
//---------------------------------------------------------------------------

int atResponseLineRead(const EsChannelIoIntf::Ptr& chnl, char* out, esU32* outLen,
  const char* sep, esU32 sepLen, esU32 tmo)
{
  if( chnl && out && outLen && *outLen > 0 )
  {
    char* pos = out;
    char* end = out + *outLen;
    esU32 sepPos = 0;
    char c;

    *outLen = 0;

    // substitute default line separator
    if( 0 == sep )
    {
      sep = "\r\n";
      sepLen = 2;
    }

    // read incoming chars one by one
    while( 1 == chnl->bytesGet((esU8*)&c, 1, tmo) )
    {
      if( sep[sepPos] == c )
        ++sepPos;
      else
      {
        // break with 'not enough buffer' code
        if( pos == end )
        {
          *outLen = pos-out;
          return atNotEnoughBuff;
        }
        // reset separator pos, just in case
        sepPos = 0;
        // copy char to out
        *pos++ = c;
      }

      // we've receive complete separator,
      // check if our buffer is not empty. if not - we're done
      // otherwise - just skip to the next incoming char,
      // not copying anything to out buffer
      if( sepPos >= sepLen )
      {
        sepPos = 0;

        if( pos != out )
        {
          *outLen = pos-out;
          return atOk;
        }
        else
          continue;
      }
    }
  }

  return atFail;
}
//---------------------------------------------------------------------------

static esBL btSendCommandAndReadOk(const EsChannelIoIntf::Ptr& chnl, const char* cmd, esU32 len)
{
  char buff[2];
  esU32 respLen = 2;

  return atOk == atCommandSend(chnl, cmd, len, 0, 0) &&
    atOk == atResponseLineRead(chnl, buff, &respLen, 0, 0, 500) &&
    0 == strncmp(buff, "OK", 2);
}

// Check if there is live BT on channel.
// Request simple AT ping: AT->OK
static esBL btLiveCheck(const EsChannelIoIntf::Ptr& chnl)
{
  return btSendCommandAndReadOk(chnl, 0, 0);
}

static esBL btSetHighRateAndLiveCheck(const EsChannelIoIntf::Ptr& chnl)
{
  chnl->close();
  EsRateCtlIntf::Ptr rc = chnl;
  rc->rateSet(921600);
  chnl->open();
  return btLiveCheck(chnl);
}

// Try to find and configure BT module over HCI
void btConfigure(const EsChannelIoIntf::Ptr& chnl)
{
  esBL configured = FALSE;

  chnl->close();
  EsRateCtlIntf::Ptr rc = chnl;
  rc->rateSet(9600);
  chnl->open();
  if( btLiveCheck(chnl) )
  {
    configured =
    // set-up BT for high rate IO
    btSendCommandAndReadOk(chnl, "+UARTCONFIG,921600,N,1,0", 24) &&
    // Reboot device after BT mode set-up
    atOk == atCommandSend(chnl, "Z", 1, 0, 0);

    if( configured )
    {
      EsThread::sleep(200); // Wait ~200ms
      configured = btSetHighRateAndLiveCheck(chnl);
    }
  }
  else
    configured = btSetHighRateAndLiveCheck(chnl);

  if( configured )
  {
    esU8 u;
    int nameCmdLen = snprintf(
      s_btBuff,
      BT_BUFF_LEN,
      "+BTNAME=\"LUPIN %0.4d-%0.2d-%0.2d %0.6lu\"",
      (int)c_fwInfo.year,
      (int)c_fwInfo.month,
      (int)c_fwInfo.day,
      c_fwInfo.order
    );

    // Cancel all potentially pending operations
    atCommandSend(chnl, "+BTCANCEL", 9, 0, 0);
    while( 1 == chnl->bytesGet(&u, 1, 1) );

    configured =
    // Disable escape sequence
    btSendCommandAndReadOk(chnl, "S11=0", 5) &&
    // Clear data buffer on disconnect
    btSendCommandAndReadOk(chnl, "S12=1", 5) &&
    // Configure UART for thoroughput
    btSendCommandAndReadOk(chnl, "S3=0", 4) &&
    // Set up BT device name to our serial string
    btSendCommandAndReadOk(chnl, s_btBuff, nameCmdLen) &&
    // Set up encryption pin key
    btSendCommandAndReadOk(chnl, "+BTKEY=\"pha-01-light\"", 14) &&
    // Set up encryption and authentication
    btSendCommandAndReadOk(chnl, "+BTSEC=1,1", 10);

    if( configured )
    {
      // Finally, set up mode 3
      configured = atOk == atCommandSend(chnl, "+BTMODE,3", 9, 0, 0);
      while( 1 == chnl->bytesGet(&u, 1, 1) );

      // Reboot device after BT mode set-up
      btSendCommandAndReadOk(chnl, "Z", 1);
    }

    if( configured )
      EsThread::sleep(200); // Wait ~200ms
  }
}

TEST(EsBtConfigTest, Basic) {

  EsChannelIoIntf::Ptr io = EsChannelIoFactory::channelCreate(esT("EsChannelIoUart"));
  EsReflectedClassIntf::Ptr rc = io;
  rc->propertySet(esT("port"), esT("\\\\.\\COM46"));

  btConfigure(io);
}
