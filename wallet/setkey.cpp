


#include "setkey.h"
#include <hlog.h>
#include <json/json.h>
#include "walletmng.h"

HRET CSetKeyCgi::Init(HCSTRR str) throw (HCException) {

    LOG_NORMAL("input string: [%s]", str.c_str());

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("addr"), INVL_PARA);
    m_strAddr = ps["addr"];
    HASSERT_RETURN(m_strAddr.length() > 8, INVL_PARA);

    HASSERT_RETURN(ps.count("pub"), INVL_PARA);
    m_strPub = ps["pub"];
    HASSERT_RETURN(m_strPub.length() > 8, INVL_PARA);

    HASSERT_RETURN(ps.count("pri"), INVL_PARA);
    m_strPri = ps["pri"];
    HASSERT_RETURN(m_strPri.length() > 8, INVL_PARA);

    HASSERT_RETURN(ps.count("pass"), INVL_PARA);
    m_strPass = ps["pass"];
    HASSERT_RETURN(m_strPass.length() > 8, INVL_PARA);

    HRETURN_OK;
}


HRET CSetKeyCgi::Work() throw (HCException) {

    HSTR strNewAddr = g_manage->SetAccount(m_strAddr, m_strPub, m_strPri, m_strPass);

    LOG_NORMAL("addr: [%s]", strNewAddr.c_str());

    m_res = HSTR("\"") +  strNewAddr + "\"";

    HRETURN_OK;
}


REGISTE_COMMAND(setkey, CSetKeyCgi);
