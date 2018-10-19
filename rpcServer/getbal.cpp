


#include "getbal.h"
#include <hlog.h>


HRET CGetBalCgi::Init(HCSTRR strAddr) throw (HCException) {

    HPS ps = parseInput(strAddr);

    HASSERT_RETURN(ps.count("addr"), INVL_PARA);

    m_str = ps["addr"];

    HRETURN_OK;

}



HRET CGetBalCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HFAILED_THROW(sendAndRecv("get_bal", m_str));

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;
}



HSTR CGetBalCgi::GetRes() const throw (HCException) {

    return m_res;

}


REGISTE_COMMAND(getbal, CGetBalCgi);
