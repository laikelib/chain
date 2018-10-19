


#include "lkinfo.h"
#include <hlog.h>


HRET CLkInfoCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = "key=val";

    HFAILED_MSG(sendAndRecv("lk_info", str), "get height sendAndRecv FAILED");

    m_res = str;

    HFUN_END;
    HRETURN_OK;
}


HSTR CLkInfoCgi::GetRes() const throw (HCException) {

    return m_res;

}



REGISTE_COMMAND(lkinfo, CLkInfoCgi);
