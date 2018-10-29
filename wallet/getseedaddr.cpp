

#include "getseedaddr.h"
#include <hlog.h>
#include "walletmng.h"

HRET CGetSeedAddrCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("seed"), INVL_PARA);
    m_strSeed = ps["seed"];
    HASSERT_RETURN(m_strSeed.length() > 32, INVL_PARA);

    HASSERT_RETURN(ps.count("path"), INVL_PARA);
    m_strPath = ps["path"];

    LOG_NORMAL("input seed(HEX): [%s] Bip44 Path: [%s]", m_strSeed.c_str(), m_strPath.c_str());

    HRETURN_OK;
}



HRET CGetSeedAddrCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    HNOTOK_RETURN(m_path.Parse(m_strPath));

    HFAILED_MSG(DecodeSeedHex(m_strSeed, m_key), "Decode seed failed");

    HASSERT_THROW_MSG(m_key.IsValid(), "key is invalid", INVL_PARA);

    m_pubKey = m_key.GetPubKey();

    CExtKey masterkey;
    masterkey.SetSeed(m_key.begin(), m_key.size());

    // derive laikelib bip.
    CExtKey lkkey;
    masterkey.Derive(lkkey, BIP32_HARDENED_KEY_LIMIT | m_path.GetAccountInt());

    CExtKey changeKey;
    lkkey.Derive(changeKey, BIP32_HARDENED_KEY_LIMIT | m_path.GetChangeInt());

    // NOT DO WHILE
    CExtKey indexkey;
    changeKey.Derive(indexkey, m_path.GetIndexInt() | BIP32_HARDENED_KEY_LIMIT);

    m_indexKey = indexkey.key;

    m_strAddr = g_manage->SetNewAccount(m_indexKey, "laikelib");

    LOG_NORMAL("addr: [%s]", m_strAddr.c_str());

    m_res = HSTR("\"") +  m_strAddr + "\"";

    HFUN_END;
    HRETURN_OK;
}



REGISTE_COMMAND(getseedaddr, CGetSeedAddrCgi);
