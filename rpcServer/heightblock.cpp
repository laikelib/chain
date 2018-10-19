


#include <hstr.h>
#include "heightblock.h"
#include <hlog.h>

#include <serialize.h>
#include <block.h>
#include <hcrypto.h>
#include <hadaptation.h>


HRET CHeightBlockCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("hid"), INVL_PARA);
    m_str = ps["hid"];


    HRETURN_OK;

}


HRET CHeightBlockCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    using namespace HUIBASE::CRYPTO;

    HSTR str2;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HFAILED_THROW(sendAndRecv("height_block", m_str));

    LOG_NORMAL("laikelib return: [%s]", m_str.c_str());

    if (m_str == "DROP") {
        m_res = m_str;
        HRETURN_OK;
    }

    HFAILED_MSG(HDecode(m_str, str2), "decode failed");

    HBUF hbuf;
    hstr_vs(str2, hbuf);
    CDataStream ds(hbuf.begin(), hbuf.end(), SER_NETWORK, NODE_VERSION);

    CBlock block;
    ds >> block;

    m_res = block.GetBlockJson();
    LOG_NORMAL("return block json: [%s]", m_res.c_str());

    HFUN_END;
    HRETURN_OK;

}


HSTR CHeightBlockCgi::GetRes () const throw (HCException) {

    if (m_res == "DROP") {

        return CCommand::GetRes();

    }

    return m_res;

}



REGISTE_COMMAND(heightblock, CHeightBlockCgi);
