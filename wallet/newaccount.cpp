

#include "newaccount.h"
#include <hlog.h>
#include <json/json.h>

#include "walletmng.h"

HRET CNewAccountCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("pass"), INVL_PARA);
    m_strPass = ps["pass"];

    HASSERT_RETURN(m_strPass.length() > 8, INVL_PARA);

    HASSERT_RETURN(HIS_TRUE(HCStr::IsDigitChar(m_strPass)), INVL_PARA);

    HRETURN_OK;

}


HRET CNewAccountCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    HSTR strNewAddr = g_manage->NewAccount(m_strPass);

    LOG_NORMAL("addr: [%s]", strNewAddr.c_str());

    m_res = HSTR("\"") +  strNewAddr + "\"";

    HFUN_END;
    HRETURN_OK;

}



REGISTE_COMMAND(newaccount, CNewAccountCgi);
