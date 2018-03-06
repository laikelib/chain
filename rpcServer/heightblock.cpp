


#include <hstr.h>
#include "heightblock.h"
#include <hlog.h>


HRET CHeightBlockCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("hid"), INVL_PARA);
    m_str = ps["hid"];


    HRETURN_OK;

}


HRET CHeightBlockCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HFAILED_THROW(sendAndRecv("height_block", m_str));

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;

}


HSTR CHeightBlockCgi::GetRes () const throw (HCException) {

    return m_res;

}



REGISTE_COMMAND(heightblock, CHeightBlockCgi);
