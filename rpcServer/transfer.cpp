

#include <hstr.h>
#include "transfer.h"
#include <hlog.h>

#include <hadaptation.h>
#include <hcrypto.h>
#include <entry.h>

#include <serialize.h>

#include "rpcapp.h"
#include "syncmng.h"

HRET CTransferCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("entry"), INVL_PARA);
    m_str = ps["entry"];
    LOG_NORMAL("entry: [%s]", m_str.c_str());

    m_src = m_str;

    HRETURN_OK;
}



HRET CTransferCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    using namespace HUIBASE::CRYPTO;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HFAILED_THROW(sendAndRecv("new_tx", m_str));

    LOG_NORMAL("laikelib new_tx return [%s]", m_str.c_str());

    if (m_str != "DROP") {

        HSTR dst;
        HFAILED_MSG(HDecode(m_src, dst), "decode src failed");

        HBUF hbuf;
        hstr_vs(dst, hbuf);
        CDataStream ds(hbuf.begin(), hbuf.end(), SER_NETWORK, NODE_VERSION);

        CEntry new_en;
        ds >> new_en;

        LOG_NORMAL("need to broadcast new tx [%s]", new_en.ToJsonString().c_str());

        m_pApp->GetSyncMng()->NewEn (new_en);

    }

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;

}


REGISTE_COMMAND(transfer, CTransferCgi);
