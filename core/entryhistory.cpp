


#include "entryhistory.h"
#include <glog/logging.h>

CEntryIndex::CEntryIndex()
    :CEntry(), m_hashBlock(0), m_nHeight(0), m_nBlockedTime(0), m_pIndex(nullptr) {
    m_pHashEntry = nullptr;
}


CEntryIndex::CEntryIndex(const CEntry& entry, CBlockIndex* pIndex)
    :CEntry(entry), m_hashBlock(pIndex->GetHash()), m_nHeight(pIndex->GetHeight()), m_nBlockedTime(pIndex->GetTime()), m_pIndex(pIndex) {

    m_pHashEntry = nullptr;

}



HSTR CEntryIndex::ToString() const {

    std::stringstream ss;

    ss << "{ \"txhash\": \"" << GetEntryHash().ToString()
       << "\", \"type\": " << GetType() << ", \"createTime\":"
       << GetCreateTime() << ", \"sender\": \"" << GetSender().ToString()
       << "\", \"receiver\":\"" << GetReceiver().ToString()
       << "\", \"sign\":\"" << GetSign().ToString() << "\",\"value\":"
       << GetValue() << ", \"fee\":" << GetFee()
       << ", \"blockhassh\": \"" << m_hashBlock.ToString()
       << "\", \"height\": " << GetBlockHeight()
       << ", \"blockedTime\": " << GetBlockedTime() << "}";

    return ss.str();
}



CEntryHistory::~CEntryHistory () {

    for (ENTRY_HISTORY::iterator it = m_history.begin(); it != m_history.end(); ++it) {

        CEntryIndex* p = it->second;
        if (nullptr != p) {
            // unblocked entry is nullptr;
            delete p;
        }

    }

    m_history.clear();

}


HRET CEntryHistory::OverIndex(CBlockIndex *pIndex) {

    CBlock block;
    block.ReadFromDisk(pIndex);

    return OverBlock(&block, pIndex);

}


HRET CEntryHistory::NewTxHash(const uint256 &hash) {

    ENTRY_HISTORY::iterator fit = m_history.find(hash);
    HASSERT_RETURN(fit == m_history.end(), RECREATE);

    m_history[hash] = nullptr;

    HRETURN_OK;

}


HRET CEntryHistory::OverBlock(const CBlock* pbk, CBlockIndex* pIndex) {

    HRET res = HERR_NO(OK);
    const std::vector<CEntry>& entrys = pbk->GetEntrys ();

    for (size_t i = 0; i < entrys.size(); ++i) {

        const CEntry& cen = entrys[i];

        CEntryIndex* p = new CEntryIndex(cen, pIndex);

        uint256 hash = p->GetEntryHash();
        m_history[hash] = p;

    }

    return res;

}



HRET CEntryHistory::AddNewEntry(CEntryIndex *index) {

    uint256 hash = index->GetEntryHash();
    ENTRY_HISTORY::iterator fit = m_history.find(hash);
    if (fit != m_history.end()) {

        LOG(WARNING) << "entry[" << hash.ToString() << "]["<< index->GetBlockHeight() <<"]==["<< fit->second->GetBlockHeight() <<"] is re-exists";
        HRETURN(RECREATE);

    }

    ENTRY_HISTORY::iterator rit = m_history.insert(make_pair(hash, index)).first;

    index->SetEntryHashP(&(rit->first));

    HRETURN_OK;

}


const CEntryIndex* CEntryHistory::GetEntryIndex(const uint256 &hash) const {

    ENTRY_HISTORY::const_iterator fit = m_history.find(hash);
    if (fit != m_history.cend()) {

        return fit->second;

    }

    return nullptr;
}


HBOOL CEntryHistory::IsRepeate(const uint256 &hash) const {

    ENTRY_HISTORY::const_iterator fit = m_history.find(hash);

    HRET_BOOL (fit == m_history.end());

}
