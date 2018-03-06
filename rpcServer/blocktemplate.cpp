

#include "blocktemplate.h"
#include <hlog.h>

HRET CBlockTemplateCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("addr"), INVL_PARA);

    m_str = ps["addr"];

    HRETURN_OK;
}


HRET CBlockTemplateCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HFAILED_THROW(sendAndRecv("block_template", m_str));

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;
}


HSTR  CBlockTemplateCgi::GetRes () const throw (HCException) {

    return m_res;

}


REGISTE_COMMAND(blocktemplate, CBlockTemplateCgi);
