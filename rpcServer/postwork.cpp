


#include "postwork.h"
#include <hlog.h>
#include <base64.h>

#include <hadaptation.h>
#include <hcrypto.h>
#include <block.h>
#include <serialize.h>

#include "rpcapp.h"
#include "syncmng.h"

using namespace HUIBASE::CRYPTO;

HRET CPostWorkCgi::Init (HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("work"), INVL_PARA);
    m_str = ps["work"];

    LOG_NORMAL("work: [%s]", m_str.c_str());

    m_src = m_str;

    HRETURN_OK;
}


HRET CPostWorkCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    LOG_NORMAL("work: [%s]", m_str.c_str());

    HFAILED_THROW(sendAndRecv("post_work", m_str));

    /*
    if (m_str != "DROP") {

        HSTR dst;
        HFAILED_MSG(HDecode(m_src, dst), "decode src failed");

        HBUF hbuf;
        hstr_vs(dst, hbuf);
        CDataStream ds(hbuf.begin(), hbuf.end(), SER_NETWORK, NODE_VERSION);

        CBlock new_bl;
        ds >> new_bl;

        m_pApp->NewBl (new_bl);

        }*/

    m_res = m_str;

    HFUN_END;
    HRETURN_OK;
}

/*
HSTR CPostWorkCgi::GetRes() const throw (HCException) {

    return m_res;

    }*/


REGISTE_COMMAND(postwork, CPostWorkCgi);
