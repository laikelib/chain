/****************************************************************************
 *
 * File Name: sign.h
 *
 * Create Author: Tom Hui, tomhui1009@yahoo.com, 8613760232170
 *
 * Create Date: Thu Jan 18 20:53 2018
 *
 * Description:
 *	
 *
 ****************************************************************************/



#ifndef __SIGN_H__
#define __SIGN_H__


#include <huibase.h>

#include "base58.h"
#include "key.h"
#include "serialize.h"

using namespace HUIBASE;

class CSign {
public:
    CSign () {}

    CSign (const CSign& sign);

    bool EmptySignature () const { return m_sign.empty(); }

    bool IsAddressSign (const CLKAddress& addr) const;

    bool Sinature (const CKey& key, const CLKAddress& addr,  const uint256& hash);

    bool VerifySign (const CLKAddress& addr, const uint256& hash) const;

    HSTR ToString () const;

    bool FromString (HCSTRR str);


    IMPLEMENT_SERIALIZE(
			READWRITE(m_pubkey);
			READWRITE(m_sign);
)

private:
    CPubKey m_pubkey;
    std::vector<unsigned char> m_sign;
};

#endif //__SIGN_H__









