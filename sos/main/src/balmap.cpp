


#include "balmap.h"
#include <glog/logging.h>
#include <ifnames.h>

HN64 CBalMng::GetBalSum() const {

    HN64 res = 0;

    for (BALMAP::const_iterator it = m_mapBal.cbegin(); it != m_mapBal.cend(); ++it) {

        res += it->second;

    }

    return res;

}


HRET CBalMng::SetAddrBal(const CLKAddress &addr, HN64 bal) {

    m_mapBal[addr] = bal;

    HRETURN_OK;
}


HRET CBalMng::PostBal (const CLKAddress& addr, HN64 bal) {

    HN64 old_val = GetBal(addr);
    SetAddrBal(addr, old_val + bal);

    HRETURN_OK;

}



HRET CBalMng::DelBal (const CLKAddress& addr, HN64 bal) {

    HN64 old_val = GetBal(addr);
    SetAddrBal(addr, old_val - bal);

    HRETURN_OK;


}


HN64 CBalMng::GetBal(const CLKAddress &addr) const {

    HN64 res = 0;
    BALMAP::const_iterator fit = m_mapBal.find(addr);
    if (fit != m_mapBal.end()) {
        res = fit->second;
    }

    return res;

}


HRET CBalMng::OverNewBlock(const CBlock &block) {

    return overBlock(block);

}


HRET CBalMng::overBlock(const CBlock& block) {

    const std::vector<CEntry>& entrys = block.GetEntrys();

    for (size_t i = 0; i < entrys.size(); ++i) {

        const CEntry& en = entrys[i];

        overEntry(en);

    }


    HRETURN_OK;

}


HRET CBalMng::overEntry(const CEntry &entry) {

    const CLKAddress& receiver = entry.GetReceiver();
    PostBal(receiver, entry.GetValue());

    if ( not entry.IsBaseSign()) {

        const CLKAddress& sender = entry.GetSender();
        DelBal(sender, entry.GetAmount());

    }

    HRETURN_OK;

}



void CBalMng::Display() {

    for (BALMAP::iterator it = m_mapBal.begin(); it != m_mapBal.end(); ++it) {

        const CLKAddress& addr = it->first;
        LOG(INFO) << addr.ToString() << ":[" << it->second << "]";

    }

    LOG(INFO) << "================<INVALID VALUE>=======================";

    for (BALMAP::iterator it = m_mapBal.begin(); it != m_mapBal.end(); ++it) {

        if (it->second < 0) {

            const CLKAddress& addr = it->first;
            LOG(WARNING) << addr.ToString() << " :[" << it->second << "]";

        }

    }

    LOG(INFO) << "ALL AMOUNT: [" << GetBalSum() << "]";

}
