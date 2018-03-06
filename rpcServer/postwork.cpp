


#include "postwork.h"
#include <hlog.h>
#include <base64.h>

using namespace HUIBASE::CRYPTO;

HRET CPostWorkCgi::Init (HCSTRR str) throw (HCException) {

    m_str = str;

    LOG_NORMAL("work: [%s]", m_str.c_str());

    HRETURN_OK;
}


HRET CPostWorkCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    LOG_NORMAL("work: [%s]", m_str.c_str());

    HFAILED_THROW(sendAndRecv("post_work", m_str));

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;
}


HSTR CPostWorkCgi::GetRes() const throw (HCException) {

    return m_res;

}


REGISTE_COMMAND(postwork, CPostWorkCgi);
