#ifndef _des_h_
#define _des_h_

// actual internal implementation of des codec
class EKOSF_CORE_CLASS DES
{
public:
	enum { 
		KEY_SIZE			=  56, 
		DATA_SIZE     =  64,
		SBUFFER_SIZE  =  48, 
		ROUNDS				=  16 };

private:
	const char *xmix(const char *, BYTE [DES::DATA_SIZE],
		const BYTE [DES::KEY_SIZE]);
	void encrypt_decrypt(BYTE [DES::DATA_SIZE], bool);
	BYTE compressed_shifted_key[DES::ROUNDS][DES::SBUFFER_SIZE];
	void initialize(const BYTE[DES::KEY_SIZE]);	

public:
	DES(const EsString& key) { setKey(key); }
	void setKey(const EsString& key);
	void process(const EsBinBuffer& src, EsBinBuffer& dest, bool encode = true);
};

#endif // _des_h_
