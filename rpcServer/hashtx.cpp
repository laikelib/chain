

#include "hashtx.h"
#include <hlog.h>

HRET CHashTxCgi::Init(HCSTRR strHash) throw (HCException) {

    HPS ps = parseInput(strHash);

    HASSERT_RETURN(ps.count("hash"), INVL_PARA);

    m_str = ps["hash"];

    HRETURN_OK;

}


HRET CHashTxCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HFAILED_THROW(sendAndRecv("hash_tx", m_str));

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;
}



HSTR CHashTxCgi::GetRes() const throw (HCException) {

    return m_res;

}


REGISTE_COMMAND(hashtx, CHashTxCgi);
