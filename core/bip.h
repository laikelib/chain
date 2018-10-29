

#ifndef __BIP_H__
#define __BIP_H__

#include <huibase.h>
#include "key.h"
#include <hstr.h>

using namespace HUIBASE;

const uint32_t BIP32_HARDENED_KEY_LIMIT = 0x800002d2;


class CBip44Path {
 public:
    static constexpr HINT BIP_PART_COUNT = 6;

 public:
    CBip44Path() { }

    CBip44Path (const CBip44Path& path)
        : m_purpose (path.m_purpose), m_cointype (path.m_cointype),
        m_account(path.m_account), m_change(path.m_change),
        m_index(path.m_index) {

    }

    HRET Parse (HCSTRR strPath);

    HCSTRR GetPurpose () const { return m_purpose; }
    void SetPurpose (HCSTRR purpose) { m_purpose = purpose; }

    HCSTRR GetCointype () const { return m_cointype; }
    void SetCointype (HCSTRR cointype) { m_cointype = cointype; }

    HCSTRR GetAccount () const { return m_account; }
    void SetAccount(HCSTRR account) { m_account = account; }
    HINT GetAccountInt () const { return HCStr::stoi(m_account); }

    HCSTRR GetChange () const { return m_change; }
    void SetChange(HCSTRR change) { m_change = change; }
    HINT GetChangeInt() const { return HCStr::stoi(m_change); }

    HCSTRR GetIndex () const { return m_index; }
    void SetIndex (HCSTRR strIndex) { m_index = strIndex; }
    HINT GetIndexInt () const { return HCStr::stoi(m_index); }

 private:
    HSTR m_purpose;
    HSTR m_cointype;
    HSTR m_account;
    HSTR m_change;
    HSTR m_index;
};


HRET DecodeSeedHex (HCSTRR strSeed, CKey& key);





#endif //__BIP_H__
