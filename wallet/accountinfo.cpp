

#include "accountinfo.h"
#include "walletmng.h"
#include "lktkey.h"

#include <hlog.h>
#include <hstr.h>
#include <hfname.h>


HRET CAccountInfoCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("addr"), INVL_PARA);
    m_strAddr = ps["addr"];
    HASSERT_RETURN(m_strAddr.length() > 20, INVL_PARA);
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitChar(m_strAddr)), INVL_PARA);

    HASSERT_RETURN(ps.count("pass"), INVL_PARA);
    m_strPass = ps["pass"];
    HASSERT_RETURN(m_strPass.length() > 8, INVL_PARA);
    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitChar(m_strPass)), INVL_PARA);

    HRETURN_OK;


}

HRET CAccountInfoCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    CAddrInfo ai;
    HFAILED_MSG(ai.Load(m_strAddr, m_strPass), "load key failed failed");

    stringstream ss;
    ss << "{\"addr\": \""<< ai.GetAddr()  <<"\", \"pub\": \""<< ai.GetPub() <<"\", \"pri\":\""<< ai.GetPri() <<"\"}";

    m_res = ss.str();

    HFUN_END;
    HRETURN_OK;
}


REGISTE_COMMAND(accountinfo, CAccountInfoCgi);
