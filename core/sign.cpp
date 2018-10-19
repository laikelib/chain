

#include "sign.h"
#include "util.h"

#include <huibase.h>
#include <hstr.h>

using namespace HUIBASE;


CSign::CSign(const CSign& sign)
    : m_pubkey(sign.m_pubkey), m_sign(sign.m_sign) {

}


bool CSign::IsAddressSign(const CLKAddress& addr) const {

    CLKAddress sign_addr;
    sign_addr.Set(m_pubkey.GetID());
    
    return sign_addr == addr;
}


bool CSign::Sinature(const CKey &key, const CLKAddress& addr,  const uint256 &hash ) {

    if (not key.IsValid() || not addr.IsValid()) {
	return false;
    }

    CPubKey pubkey = key.GetPubKey();
    CLKAddress key_addr;
    key_addr.Set(pubkey.GetID());

    if ( not (key_addr == addr)) {
	return false;
    }

    m_sign.clear();

    if (not key.Sign(hash, m_sign)) {
	return false;
    }
    
    m_pubkey = key.GetPubKey();

    return true;
}


bool CSign::VerifySign(const CLKAddress &addr, const uint256& hash) const {

    if (not addr.IsValid() || not m_pubkey.IsValid()) {
	return false;
    }

    if (not IsAddressSign(addr)) {
	return false;
    }

    return m_pubkey.Verify(hash, m_sign);

}


HSTR CSign::ToString() const {

    string strpub = EncodeBase64(m_pubkey.begin(), m_pubkey.size());

    string strsign = EncodeBase64(&m_sign[0], m_sign.size());

    return strpub + "||" + strsign;

}

bool CSign::FromString(HCSTRR str) {

    if (str == "||") {
	return false;
    }

    HVSTRS vs;
    (void) HCStr::Split(str, "||", vs);

    if (vs.size() != 2) {
	return false;
    }

    if (vs[0].length() < 3 || vs[1].length() < 3) {
	return false;
    }

    HCSTRR str_pub = vs[0];
    std::vector<unsigned char> pub = DecodeBase64(str_pub.c_str());

    m_pubkey.Set(pub.begin(), pub.end());

    HCSTRR str_sign = vs[1];
    m_sign = DecodeBase64(str_sign.c_str());

    return true;
}
