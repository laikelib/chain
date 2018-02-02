
#ifndef __BASE58_H__
#define __BASE58_H__

#include "key.h"
#include "hash.h"
#include "bignum.h"

static const char* pszBase58 = "123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

inline std::string EncodeBase58(const unsigned char* pbegin, const unsigned char* pend)
{
    CAutoBN_CTX pctx;
    CBigNum bn58 = 58;
    CBigNum bn0 = 0;

    // Convert big endian data to little endian
    // Extra zero at the end make sure bignum will interpret as a positive number
    std::vector<unsigned char> vchTmp(pend-pbegin+1, 0);
    reverse_copy(pbegin, pend, vchTmp.begin());

    // Convert little endian data to bignum
    CBigNum bn;
    bn.setvch(vchTmp);

    // Convert bignum to std::string
    std::string str;
    // Expected size increase from base58 conversion is approximately 137%
    // use 138% to be safe
    str.reserve((pend - pbegin) * 138 / 100 + 1);
    CBigNum dv;
    CBigNum rem;
    while (bn > bn0)
    {
        if (!BN_div(&dv, &rem, &bn, &bn58, pctx))
            throw bignum_error("EncodeBase58 : BN_div failed");
        bn = dv;
        unsigned int c = rem.getulong();
        str += pszBase58[c];
    }

    // Leading zeroes encoded as base58 zeros
    for (const unsigned char* p = pbegin; p < pend && *p == 0; p++)
        str += pszBase58[0];

    // Convert little endian std::string to big endian
    reverse(str.begin(), str.end());
    return str;
}


inline std::string EncodeBase58(const std::vector<unsigned char>& vch)
{
    return EncodeBase58(&vch[0], &vch[0] + vch.size());
}

// Decode a base58-encoded string psz into byte vector vchRet
// returns true if decoding is successful
inline bool DecodeBase58(const char* psz, std::vector<unsigned char>& vchRet)
{
    CAutoBN_CTX pctx;
    vchRet.clear();
    CBigNum bn58 = 58;
    CBigNum bn = 0;
    CBigNum bnChar;
    while (isspace(*psz))
        psz++;

    // Convert big endian string to bignum
    for (const char* p = psz; *p; p++)
    {
        const char* p1 = strchr(pszBase58, *p);
        if (p1 == NULL)
        {
            while (isspace(*p))
                p++;
            if (*p != '\0')
                return false;
            break;
        }
        bnChar.setulong(p1 - pszBase58);
        if (!BN_mul(&bn, &bn, &bn58, pctx))
            throw bignum_error("DecodeBase58 : BN_mul failed");
        bn += bnChar;
    }

    // Get bignum as little endian data
    std::vector<unsigned char> vchTmp = bn.getvch();

    // Trim off sign byte if present
    if (vchTmp.size() >= 2 && vchTmp.end()[-1] == 0 && vchTmp.end()[-2] >= 0x80)
        vchTmp.erase(vchTmp.end()-1);

    // Restore leading zeros
    int nLeadingZeros = 0;
    for (const char* p = psz; *p == pszBase58[0]; p++)
        nLeadingZeros++;
    vchRet.assign(nLeadingZeros + vchTmp.size(), 0);

    // Convert little endian data to big endian
    reverse_copy(vchTmp.begin(), vchTmp.end(), vchRet.end() - vchTmp.size());
    return true;
}


// Decode a base58-encoded string str into byte vector vchRet
// returns true if decoding is successful
inline bool DecodeBase58(const std::string& str, std::vector<unsigned char>& vchRet)
{
    return DecodeBase58(str.c_str(), vchRet);
}


class CBase58Data
{
protected:
	
    // the actually encoded data
    //typedef std::vector<unsigned char, zero_after_free_allocator<unsigned char> > vector_uchar;
    typedef std::vector<unsigned char> vector_uchar;
    vector_uchar vchData;

    CBase58Data()
    {
        vchData.clear();
    }

    CBase58Data(const CBase58Data& data) :
    vchData(data.vchData) {}

    void SetData(const void* pdata, size_t nSize)
    {
        vchData.resize(nSize);
        if (!vchData.empty())
            memcpy(&vchData[0], pdata, nSize);
    }

    void SetData(const unsigned char *pbegin, const unsigned char *pend)
    {
        SetData((void*)pbegin, pend - pbegin);
    }

public:
    bool IsEmpty () const { return vchData.empty(); }
    
    void SetEmpty() { vchData.clear(); }
    
    bool SetString(const char* psz)
    {
        std::vector<unsigned char> vchTemp;
        DecodeBase58(psz, vchTemp);
		
        vchData.resize(vchTemp.size());
        if (!vchData.empty()) {
            memcpy(&vchData[0], &vchTemp[0], vchData.size());
        }
		
        OPENSSL_cleanse(&vchTemp[0], vchData.size());
        return true;
    }

    bool SetString(const std::string& str)
    {
        return SetString(str.c_str());
    }

    std::string ToString() const
    {
	std::vector<unsigned char> vch;
        vch.insert(vch.end(), vchData.begin(), vchData.end());
        return EncodeBase58(vch);
    }

    int CompareTo(const CBase58Data& b58) const
    {
        if (vchData < b58.vchData)   return -1;
        if (vchData > b58.vchData)   return  1;
        return 0;
    }

    /*
    unsigned int GetSerializeSize(int, int) const
    {
        return vchData.size() + sizeof(size_t);
    }

    template<typename Stream>
    void Serialize(Stream& s, int, int) const
    {
	size_t len = vchData.size();
	s.write((char*)&len, sizeof(len));
	s.write((char*)&vchData[0], vchData.size());
        //s.write((char*)pn, sizeof(pn));
    }

    template<typename Stream>
    void Unserialize(Stream& s, int, int)
    {
	size_t len = 0;
	s.read((char*)&len, sizeof(len));
	vchData.resize(len);
	s.read((char*)&vchData[0], len);
       //s.read((char*)pn, sizeof(pn));
       }*/

    IMPLEMENT_SERIALIZE(
			READWRITE(vchData);
)
    

    bool operator==(const CBase58Data& b58) const { return CompareTo(b58) == 0; }
    bool operator<=(const CBase58Data& b58) const { return CompareTo(b58) <= 0; }
    bool operator>=(const CBase58Data& b58) const { return CompareTo(b58) >= 0; }
    bool operator< (const CBase58Data& b58) const { return CompareTo(b58) <  0; }
    bool operator> (const CBase58Data& b58) const { return CompareTo(b58) >  0; }
};

class CLKAddress : public CBase58Data
{
public:

    bool IsValid() const
    {
        bool fCorrectSize = vchData.size() == 20;

        return fCorrectSize;
    }

    CLKAddress()
    {
    }

    CLKAddress (const CLKAddress& address):
    CBase58Data(address) {}
    

    CLKAddress(const std::string& strAddress)
    {
        SetString(strAddress);
    }

    CLKAddress(const char* pszAddress)
    {
        SetString(pszAddress);
    }

    
    bool Set(const CKeyID& id) {

	SetData(&id, 20);

	return true;
	
    }


    bool GetKeyID(CKeyID &keyID) const {
        if (!IsValid())
            return false;
        uint160 id;
        memcpy(&id, &vchData[0], 20);
        keyID = CKeyID(id);
        return true;
    }

};


#endif //__BASE58_H__


