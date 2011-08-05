#include "stdafx.h"
#pragma hdrstop

#include "EsFwMoniker.h"
#include <wx/wfstream.h>
#include <wx/sstream.h>
#include <wx/tokenzr.h>

// firmware binary image implementation
//
void EsFirmwareBinary::reset() 
{
	m_bin.clear();
	m_binStartAddr = 0;
	m_binOffset = 0;
}

void EsFirmwareBinary::load(wxInputStream& in)
{
	// load and check magic
	size_t val;
	in.Read( &val, sizeof(val) );
	if( Magic == val )
	{
		// member values, then buffer size
		in.Read( &m_binStartAddr, sizeof(m_binStartAddr) ).
			Read( &m_binOffset, sizeof(m_binOffset) ).
			Read( &val, sizeof(val) );
		if( val > MaxLen )
			EsException::Throw(0, EsException::Generic, _("Binary size exceeds maximum allowed length."));
		m_bin.resize(val);
		in.Read( &m_bin[0], m_bin.size() );
	}
	else
		EsException::Throw(0, EsException::Generic, _("Error loading firmware binary from stream."));
}

void EsFirmwareBinary::save(wxOutputStream& out)
{
	// first, write magic number, then
	// write members, then binary buffer
	size_t val = Magic;
	out.Write( &val, sizeof(val) ).
		Write(&m_binStartAddr, sizeof(m_binStartAddr)).
		Write(&m_binOffset, sizeof(m_binOffset));
	val = m_bin.size();
	out.Write(&val, sizeof(val)).
		Write(&m_bin[0], m_bin.size());
}

// firmware moniker implementation
//
EsFirmwareMoniker::EsFirmwareMoniker() :
EsDictionary(),
m_type(NO_ID)
{
}

EsFirmwareMoniker::EsFirmwareMoniker(const EsFirmwareMoniker& other) :
EsDictionary(other),
m_type(other.m_type),
m_fwFile(other.m_fwFile),
m_img(other.m_img)
{
}

// try to load moniker from stream. existing contents will be discarded
void EsFirmwareMoniker::load(wxInputStream& in, const EsString& key /*= EsString::s_null*/ )
{
	// reset moniker contents
	clearAll();
	m_img.reset();
	m_type = NO_ID;
	m_fwFile.clear();
	
	// try load from stream
	wxStringOutputStream out;
	if( key.empty() )
		in >> out;
	else
		EsUtilities::makeFw(in, out, key);

	if( out.IsOk() )
	{
		setAsString( out.GetString() );
		// get device type from firmware configuration name
		EsString codeName = getName();
		WORD type = Devices::getTypeFromCodeName( codeName );
		if( NO_ID != type )
			m_type = type;
		else
			EsException::Throw(0, EsException::Warning, 
			_("Cannot create firmware moniker for invalid device type %d"), type);	
		// convert hex contents into binary image
		hexToBin();
	}
}

// try to load firmware file and initialize firmware moniker with it.
// firmwareFile should point to the firmware image to be loaded	
//
EsFirmwareMoniker::EsFirmwareMoniker(const EsString& firmwareFile, const EsString& key) :
EsDictionary(),	
m_type(NO_ID)
{
	wxFileName fwFile = firmwareFile;
	fwFile.Normalize();

	if( fwFile.FileExists() && fwFile.IsFileReadable() )
	{
		// load firmware configuration file
		wxFFileInputStream in( fwFile.GetFullPath() );
		if( fwFile.GetExt().Lower() == wxT("fw") )
			load(in);
		else
			load(in, key);
		
		if( isOk() )
			m_fwFile = firmwareFile;		
		else
			EsException::Throw(0, EsException::Generic, 
				_("Error loading firmware file '%s'"), firmwareFile.c_str());	
	}
	else
		EsException::Throw(0, EsException::Warning, 
			_("Cannot create firmware moniker for file '%s'. File does not exist or is not accessible for reading."), 
			firmwareFile.c_str());
}

EsFirmwareMoniker& EsFirmwareMoniker::operator= (const EsFirmwareMoniker& other)
{
	EsDictionary::operator=(other);
	m_type = other.m_type;
	m_fwFile = other.m_fwFile;
	m_img = other.m_img;

	return *this;
}

// convert hex string into binary image in memory
void EsFirmwareMoniker::hexToBin(DWORD flashSizeMask)
{
	EsString hex = getValue(wxT("hex")).asString();
	m_img.reset();
	EsBinBuffer bin;
	bin.reserve(hex.size() * 2);
	wxStringTokenizer hexTokenizer(hex, wxT("\r\n :"), wxTOKEN_STRTOK );

	if( !hexTokenizer.HasMoreTokens() )
		EsException::Throw(0, EsException::Generic, _("Firmware image is empty."));

	bool binOffsDefined = false;
	DWORD realAddr = 0;

	while( hexTokenizer.HasMoreTokens() )
	{
		EsString rec = hexTokenizer.GetNextToken();
		BYTE pos = 0;
		WORD recAddr;
		BYTE recType;
		BYTE Hexvalue;
		BYTE nib;

		// record length
		BYTE recLength;
		EsString::hexToBinNibble(rec[pos++], nib);
		recLength = nib;
		recLength <<= 4;
		EsString::hexToBinNibble(rec[pos++], nib);
		recLength |= nib;

		// record address
		EsString::hexToBinNibble(rec[pos++], nib);
		recAddr = nib;
		recAddr <<= 4;
		EsString::hexToBinNibble(rec[pos++], nib);			
		recAddr |= nib;
		recAddr <<= 4;
		EsString::hexToBinNibble(rec[pos++], nib);
		recAddr |= nib;
		recAddr <<= 4;
		EsString::hexToBinNibble(rec[pos++], nib);
		recAddr |= nib;

		realAddr = realAddr - (realAddr & 0xffff) + recAddr;

		EsString::hexToBinNibble(rec[pos++], nib);
		recType = nib;
		recType <<= 4;
		EsString::hexToBinNibble(rec[pos++], nib);
		recType |= nib;

		switch(recType)
		{
		case 0x00:          // 00 - Data record
			/*
			* Binary Offset is defined as soon as first data record read
			*/
			binOffsDefined = true;
			// Memory for binary file big enough ?
			while(realAddr + recLength - m_img.getBinaryOffs() > bin.capacity())
				bin.reserve(bin.capacity()*2);

			// We need to know, what the highest address is,
			// how many bytes / sectors we must flash
			if(realAddr + recLength - m_img.getBinaryOffs() > bin.size())
				bin.resize(realAddr + recLength - m_img.getBinaryOffs(), 0);

			for(int i = 0; i < recLength; i++)
			{
				EsString::hexToBinNibble(rec[pos++], nib);
				Hexvalue = nib;
				Hexvalue <<= 4;
				EsString::hexToBinNibble(rec[pos++], nib);
				Hexvalue |= nib;

				bin[realAddr + i - m_img.getBinaryOffs()] = Hexvalue;
			}
			break;
		case 0x01:     // 01 - End of file record
			break;
		case 0x02:     // 02 - Extended segment address record
			for(int i = 0; i < recLength * 2; i++)   // double amount of nibbles
			{
				realAddr <<= 4;
				if(i == 0)
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					realAddr  = nib;
				}
				else
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					realAddr |= nib;
				}
			}
			realAddr <<= 4;
			break;
		case 0x03:     // 03 - Start segment address record
			for(int i = 0; i < recLength * 2; i++)   // double amount of nibbles
			{
				realAddr <<= 4;
				if(i == 0)
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					realAddr  = nib;
				}
				else
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					realAddr |= nib;
				}
			}
			realAddr <<= 8;
			break;
		case 0x04:     // 04 - Extended linear address record
			for(int i = 0; i < recLength * 2; i++)   // double amount of nibbles
			{
				realAddr <<= 4;
				if(i == 0)
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					realAddr  = nib;
				}
				else
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					realAddr |= nib;
				}
			}
			realAddr <<= 16;
			if (!binOffsDefined)
			{
				// set startaddress of BinaryContent
				// use of flashSizeMask to allow a memory range, not taking the first
				// [04] record as actual start-address.
				m_img.setBinaryOffs( realAddr & flashSizeMask );
			}
			else
			{
				if((realAddr & flashSizeMask) != m_img.getBinaryOffs())
					EsException::Throw(0, EsException::Generic, 
						_("New Extended Linear Address Record [04] out of memory range\nCurrent Memory starts at: 0x%08X, new Address is: 0x%08X"),
						m_img.getBinaryOffs(), realAddr);
			}
			break;
		case 0x05:     // 05 - Start linear address record
			m_img.setStartAddr(0);
			for(int i = 0; i < recLength * 2; i++)   // double amount of nibbles
			{
				m_img.setStartAddr( m_img.getStartAddr() << 4 );
				if(i == 0)
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					m_img.setStartAddr(nib);
				}
				else
				{
					EsString::hexToBinNibble(rec[pos++], nib);
					m_img.setStartAddr( m_img.getStartAddr() | nib );
				}
			}
			break;
		default:
			// we should not be here
			EsException::Throw(0, EsException::Generic, _("Unsupported hex record type encountered: [%d]."), recType);
			break;
		}

		// check length to flash for correct alignment, can happen with broken ld-scripts
		//
		if(bin.size() % 4 != 0)
			bin.resize(((bin.size() + 3)/4) * 4, 0);
	}
	
	m_img.setImage(bin);
}

// access optional firmware comment field
EsString EsFirmwareMoniker::getComment() const
{
	return getValueDef(wxT("comment"), EsString::s_null).asString();
}

// access firmware version info
VerInfo EsFirmwareMoniker::getVerInfo() const
{
	VerInfo result;
	result.major = 0;
	result.minor = 0;

	EsVariant val = getValue(wxT("ver"));
	wxASSERT(val.getCount() == 2);
	wxASSERT(val.getItem(1).asLong() == VerInfo_SZE);
	if( val.getItem(0).asULong() + VerInfo_SZE > m_img.m_bin.size() )
		EsException::Throw(0, EsException::Generic, _("Error extracting version info from firmware image."));

	memcpy(&result, &m_img.m_bin[0] + val.getItem(0).asLong(), VerInfo_SZE);

	return result;
}

// access crp and pathes
void EsFirmwareMoniker::setCrp(bool crp)
{
	setValue(wxT("crp"), crp, true);
}

void EsFirmwareMoniker::setYear(long year)
{
	EsVariant val = getValue(wxT("patches"));
	EsVariant valYear = val.getItem(0);
	valYear.setItem(2, year);
	val.setItem(0, valYear);
	setValue(wxT("patches"), val);
}

void EsFirmwareMoniker::setNum(long num)
{
	EsVariant val = getValue(wxT("patches"));
	EsVariant valNum = val.getItem(1);
	valNum.setItem(2, num);
	val.setItem(1, valNum);
	setValue(wxT("patches"), val);
}

void EsFirmwareMoniker::setUid(const EsString& uid)
{
	EsVariant val = getValue(wxT("patches"));
	EsVariant valUid = val.getItem(2);
	valUid.setItem(2, uid);
	val.setItem(2, valUid);
	setValue(wxT("patches"), val);
}

void EsFirmwareMoniker::setFuncMask(long mask)
{
	EsVariant val = getValue(wxT("patches"));
	EsVariant valMask = val.getItem(3);
	valMask.setItem(2, mask);
	val.setItem(3, valMask);
	setValue(wxT("patches"), val);
}

bool EsFirmwareMoniker::getCrp() const
{
	return getValueDef(wxT("crp"), false).asBool();
}

long EsFirmwareMoniker::getYear() const
{
	EsVariant val = getValue(wxT("patches"));
	return val.getItem(0).getItem(2).asLong();
}

long EsFirmwareMoniker::getNum() const
{
	EsVariant val = getValue(wxT("patches"));
	return val.getItem(1).getItem(2).asLong();
}

EsString EsFirmwareMoniker::getUid() const
{
	EsVariant val = getValue(wxT("patches"));
	return val.getItem(2).getItem(2).asString();
}

long EsFirmwareMoniker::getFuncMask() const
{	
	EsVariant val = getValue(wxT("patches"));
	return val.getItem(3).getItem(2).asLong();
}

EsFirmwareBinary EsFirmwareMoniker::getBin() const
{
	EsFirmwareBinary result = m_img;

	// now we may apply patches to binary image
	EsVariant patches = getValue(wxT("patches"));
	wxASSERT( 0 != patches.getCount() );
	for( int idx = 0; idx < patches.getCount(); ++idx )
	{
		EsVariant patch = patches.getItem(idx);
		wxASSERT( 3 == patch.getCount() );
		EsVariant val = patch.getItem(2);
		// check if patch fits inside binary image
		if( patch.getItem(0).asULong()+patch.getItem(1).asULong() > result.m_bin.size() )
			EsException::Throw(0, EsException::Generic, _("Firmware patch does not fit image"));
		// apply patch
		if( val.getType() == EsVariant::VAR_INT )
		{
			unsigned long data = val.asULong();
			memcpy(&result.m_bin[0]+patch.getItem(0).asLong(), &data,	patch.getItem(1).asLong());
		}
		else if( val.getType() == EsVariant::VAR_STRING )
		{
			std::string str = val.asString().toAscii();
			if( str.size() == patch.getItem(1).asULong() )
				memcpy(&result.m_bin[0]+patch.getItem(0).asLong(), str.c_str(), patch.getItem(1).asLong());
			else
				EsException::Throw(0, EsException::Generic, _("String patch length does not match"));
		}
		else
			EsException::Throw(0, EsException::Generic, _("Unsupported patch value type"));
	}

	return result;
}

// return firmware identification string
EsString EsFirmwareMoniker::getIdString() const
{	
	EsString result;
	
	if( isOk() )
	{
		EsString path = getFile();
		if( !path.empty() )
		{
			wxFileName file = wxFileName::FileName(path);
			result = EsString::format( wxT("'%s' - %s"), 
				file.GetName().t_str(),
				Devices::getDescrLong( getType() ).c_str());
		}
		else
		{
			// firmware moniker wasn't created from file. use firmware 
			// type, serial number, and version to collate IdString
			VerInfo v = getVerInfo();
			result = EsString::format( wxT("%s %0.5d-%0.2d %d.%d"),
				Devices::getDescrLong( getType() ).c_str(),
				getNum(), getYear()%100, v.major, v.minor);
		}
	}
	
	return result;
}