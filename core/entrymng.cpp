


#include "entrymng.h"
#include "ifnames.h"

HRET CEntryMng::AddNewEntry(const CEntry &entry) {

    ENTRY_HOLDER::iterator fit = m_entrys.find(entry);
    if (fit != m_entrys.end()) {
        HRETURN(RECREATE);
    }

    m_entrys.insert(entry);

    HRETURN_OK;
}



HRET CEntryMng::GetUnBlockedEntrys (std::vector<CEntry>& entrys) {

    int nCount = 0;

    for (ENTRY_HOLDER::iterator it = m_entrys.begin(); it != m_entrys.end() &&
             nCount < MAX_ENTRY_COUNT; ++it) {

        const CEntry& en = *it;

        entrys.push_back(en);

        ++nCount;

    }

    HRETURN_OK;

}


HRET CEntryMng::GetAllEntrys(std::vector<CEntry>& entrys) {

    entrys.clear();

    entrys.insert(entrys.end(), m_entrys.begin(), m_entrys.end());

    HRETURN_OK;
}


HRET CEntryMng::AcceptBlock(const CBlock &block) {

    const std::vector<CEntry>& entrys = block.GetEntrys();

    for (size_t i = 0; i < entrys.size(); ++i) {

        const CEntry& en = entrys[i];

        removeEntry(en);

    }

    HRETURN_OK;

}


HN64 CEntryMng::GetPoolPay(const CLKAddress &addr)  const {

    HN64 res = 0;
    for (ENTRY_HOLDER::const_iterator cit = m_entrys.cbegin(); cit != m_entrys.cend(); ++cit) {

        const CEntry& en = *cit;

        if (en.GetSender() == addr) {

            res += en.GetAmount();

        }


    }

    return res;


}


HRET CEntryMng::removeEntry(const CEntry &entry) {

    ENTRY_HOLDER::iterator fit = m_entrys.find(entry);

    HASSERT_RETURN(fit != m_entrys.end(), INDEX_OUT);

    m_entrys.erase(fit);

    HRETURN_OK;

}
