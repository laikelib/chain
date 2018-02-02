
#ifndef __WALLET_H__
#define __WALLET_H__

#include "db.h"
#include <list>
#include <vector>
#include <huibase.h>
#include <hstr.h>
#include <utility>

#include "block.h"
#include "key.h"
#include "keystore.h"
#include "entry.h"
#include "ifnames.h"

enum DBErrors {
    DB_LOAD_OK,
    DB_CORRUPT,
    DB_NOCRITICAL_ERROR,
    DB_TOO_NEW,
    DB_LOAD_FAIL,
    DB_NEED_REWRITE
};

class CWallet;
class CWalletDB : public CDB {
public:
    CWalletDB (HCSTRR strFileName, HCPSZ pszMode = "r+")
	: CDB(strFileName, pszMode) {}

public:
    DBErrors LoadWallet (CWallet* pWallet);

private:
    CWalletDB(const CWalletDB&);

    void operator= (const CWalletDB&);

public:
    static bool Recover (CDBEnv& dbenv, std::string strFileName, bool bOnlyKeys);
    
    static bool Recover (CDBEnv& dbenv, std::string strFileName);

    bool ReadKeyValue (CWallet* pwallet, CDataStream& dsKey, CDataStream& dsVal,
		       string& strType);
    
public:

    bool WriteKey (const CPubKey& pubKey, const CPrivKey& priKey);

    bool WriteEntry (const uint256& hash, const CEntry& entry);

};


class CWallet : public CBasicKeyStore {
public:
    using ACCOUNT_TXS = std::map<CLKAddress, std::set<uint256> >;

    using ACCOUNT_BAL = std::map<CLKAddress, HN64>;
    
public:
    CWallet ();
    
    CWallet(const string& strFileName);

    void GetTxs (const CLKAddress& addr, std::vector<CEntry>& entrys) const;

    HN64 GetAccountBal (const CLKAddress& addr) const;

    CPrivKey GetPrivate (const CPubKey& pub) const;

    void displayWallet () ;

    DBErrors LoadWallet (bool& bFirstLoad);

    CPubKey GenerateNewKey ();

    void DefaultAddress (CLKAddress& addr) const;

    void MakeCoinBase (CEntry& entry) const;

    void InitAccountBalance ();

    void SetAccountsBalance ();

    void AddIn (const CLKAddress& addr, const uint256& hash);

    void AddOut (const CLKAddress& addr, const uint256& hash);

    void AddTx (ACCOUNT_TXS& old_txs, const CLKAddress& addr, const
		       uint256& hash);

    void ChangeAccountValue (const CLKAddress& addr, HN64 val, bool isAdd = true);
    void ChangeValForTxs (const ACCOUNT_TXS& txs, bool isAdd = true);

    void ProcessBlock (const CBlock& block);
    
protected:
    void SetNull ();

    bool isWithMe (const CEntry& cen)const;

public:
    void SetWalletVersion (int nVersionIn) { m_nWalletVersion = nVersionIn; }

    bool LoadKey (const CKey& key, const CPubKey& pubKey);

    void AddEntry (const uint256& hash, const CEntry& entry);
    
private:
    string m_strWalletFileName;

    int m_nWalletVersion;

    bool m_bFileBacked = false;

    std::map<uint256, CEntry> m_entrys;
    ACCOUNT_TXS m_account_ins;
    ACCOUNT_TXS m_account_outs;
    ACCOUNT_BAL m_account_bal;
};





#endif // __WALLET_H__










