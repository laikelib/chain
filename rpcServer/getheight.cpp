

#include "getheight.h"
#include <hlog.h>

HRET CGetHeightCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = "key=val";

    HFAILED_MSG(sendAndRecv("get_height", str), "get height sendAndRecv FAILED");

    m_res = str;

    HFUN_END;
    HRETURN_OK;
}


HSTR  CGetHeightCgi::GetRes () const throw (HCException) {

    return m_res;

}


REGISTE_COMMAND(getheight, CGetHeightCgi);
