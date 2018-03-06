

#include "accountinfo.h"
#include <hlog.h>

HRET CAccountInfoCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("addr"), INVL_PARA);
    m_str = ps["addr"];

    HRETURN_OK;


}

HRET CAccountInfoCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HFAILED_THROW(sendAndRecv("account_info", m_str));

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;
}


REGISTE_COMMAND(accountinfo, CAccountInfoCgi);
