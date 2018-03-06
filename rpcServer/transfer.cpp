

#include <hstr.h>
#include "transfer.h"
#include <hlog.h>


HRET CTransferCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HSTR str2;

    HASSERT_RETURN(ps.count("value"), INVL_PARA);
    str2 = ps["value"];
    m_dVal = HCStr::stod(str2);

    HASSERT_RETURN(ps.count("fee"), INVL_PARA);
    str2 = ps["fee"];
    m_dFee = HCStr::stod(str2);

    HASSERT_RETURN(ps.count("tos"), INVL_PARA);
    m_strTo = ps["tos"];

    HASSERT_RETURN(ps.count("fos"), INVL_PARA);
    m_strFo = ps["fos"];

    HASSERT_RETURN(ps.count("pub"), INVL_PARA);
    m_strPub = ps["pub"];

    HASSERT_RETURN(ps.count("pri"), INVL_PARA);
    m_strPri = ps["pri"];


    HRETURN_OK;
}



HRET CTransferCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    stringstream ss;
    ss << "{\"value\":" << m_dVal << ", \"fee\":" << m_dFee << ", \"sender\": \"" << m_strFo <<  "\",\"receiver\":\"" << m_strTo << "\", \"public\":\""<< m_strPub <<"\", \"private\":\"" << m_strPri << "\"}";

    HSTR str = ss.str();
    LOG_NORMAL("sent mq: [%s]", str.c_str());

    HFAILED_THROW(sendAndRecv("transfer", str));

    m_res = str;

    HFUN_END;
    HRETURN_OK;

}


HSTR CTransferCgi::GetRes () const throw (HCException) {

    return m_res;

}


REGISTE_COMMAND(transfer, CTransferCgi);
