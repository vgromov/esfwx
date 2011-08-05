#ifndef _es_fw_moniker_h_
#define _es_fw_moniker_h_

// forward decl
class EsFirmwareMoniker;

// binary firmware image
class MCUPROG_CLASS EsFirmwareBinary
{
public:
	enum {
		Magic = 0x0B140A16,
		MaxLen = 2000000,
		};
		
public:		
	EsFirmwareBinary() { reset(); }
	void reset(); 
	void load(wxInputStream& in);
	void save(wxOutputStream& out);

	const EsBinBuffer& getImage() const { return m_bin; }
	size_t getStartAddr() const { return m_binStartAddr; }
	size_t getBinaryOffs() const { return m_binOffset; }

protected:
	// friend-only services
	//
	void setImage(const EsBinBuffer& bin) { m_bin = bin; }
	void setStartAddr(size_t addr) { m_binStartAddr = addr; }
	void setBinaryOffs(size_t offs) { m_binOffset = offs; }

protected:
	EsBinBuffer m_bin;
	size_t m_binStartAddr;	
	size_t m_binOffset;
	
	friend class EsFirmwareMoniker;
};

// firmware file moniker class
//
class MCUPROG_CLASS EsFirmwareMoniker : public EsDictionary
{
public:
	EsFirmwareMoniker();
	// try to load firmware file and initialize firmware moniker with it.
	// firmwareFile should point to the firmware image to be loaded	
	//
	EsFirmwareMoniker(const EsString& firmwareFile, const EsString& key);
	// copy constructor. firmware contents get copied by value
	EsFirmwareMoniker(const EsFirmwareMoniker& other);
	// assignment operator. firmware contents get copied by value
	EsFirmwareMoniker& operator= (const EsFirmwareMoniker& other);
	// try to load moniker from stream. existing contents will be discarded
	void load(wxInputStream& in, const EsString& key = EsString::s_null );
	// simple validity checker
	bool isOk() const { return m_type != NO_ID && getName() == Devices::getCodeName(m_type) && !m_img.getImage().empty(); }
	// access optional firmware comment field
	EsString getComment() const;
	// access firmware version info
	VerInfo getVerInfo() const;
	// access device type & fw file
	WORD getType() const { return m_type; }
	EsString getFile() const { return m_fwFile; }
	// access crp and patch values
	void setCrp(bool crp);
	void setYear(long year);
	void setNum(long num);
	void setUid(const EsString& uid);
	void setFuncMask(long mask);
	bool getCrp() const;
	long getYear() const;
	long getNum() const;
	EsString getUid() const;
	long getFuncMask() const;
	// return copy of firmware binary image, with
	// all patches and crp applied
	EsFirmwareBinary getBin() const;
	// return firmware identification string
	EsString getIdString() const;

protected:
	// convert hex string into binary image in memory
	void hexToBin(DWORD flashSizeMask = 0xFFC00000);

protected:
	// data members
	//
	// type of device for which moniker is created
	WORD m_type;
	// firmware file name
	EsString m_fwFile;
	// binary firmware image. 
	EsFirmwareBinary m_img;
};

#endif // _es_fw_moniker_h_