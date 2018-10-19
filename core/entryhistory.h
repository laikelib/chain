



#ifndef __ENTRY_HISTORY_H__
#define __ENTRY_HISTORY_H__


#include <huibase.h>
#include <unordered_map>
#include "lktmap.h"
#include "entry.h"
#include "blockindex.h"
#include "block.h"

using namespace HUIBASE;

class CEntryIndex : public CEntry {
 public:
    CEntryIndex();

    CEntryIndex(const CEntry& entry, CBlockIndex* pIndex);

    virtual ~CEntryIndex() { };

 public:
    const uint256& GetBlockHash() const { return m_hashBlock; }

    HUINT GetBlockHeight () const { return m_nHeight; }

    HUINT GetBlockedTime () const { return m_nBlockedTime; }

    void SetBlockHash (const uint256& hash) { m_hashBlock = hash; }

    void SetHeight (HUINT nHeight) { m_nHeight = nHeight; }

    void SetBlockedTime (HUINT nTime) { m_nBlockedTime = nTime; }

    const CBlockIndex* GetIndex() const { return m_pIndex; }

    void SetIndex(CBlockIndex* pIndex) { m_pIndex = pIndex; }

    const uint256* GetEntryHashP() const { return m_pHashEntry; }

    void SetEntryHashP (const uint256* p) { m_pHashEntry = p; }

    HSTR ToString () const;

 private:
    uint256 m_hashBlock = 0;
    HUINT m_nHeight = 0;
    HUINT m_nBlockedTime = 0;
    CBlockIndex* m_pIndex = nullptr;
    const uint256* m_pHashEntry = nullptr;
};

class CEntryHistory{
 public:
    using ENTRY_HISTORY = std::unordered_map<uint256, CEntryIndex*, hash256_hash, hash256_equal>;

 public:
    CEntryHistory () { }

    virtual ~CEntryHistory ();

    HRET OverIndex(CBlockIndex* pIndex);

    HRET NewTxHash (const uint256& hash);

    HRET OverBlock (const CBlock* pbk, CBlockIndex* pIndex);

    HRET AddNewEntry (CEntryIndex* index);

    HUINT GetEntrySum () const { return m_history.size(); }

    const CEntryIndex* GetEntryIndex (const uint256& hash) const;

    HBOOL IsRepeate (const uint256& hash) const;

 private:
    ENTRY_HISTORY m_history;

};






#endif //__ENTRY_HISTORY_H__
