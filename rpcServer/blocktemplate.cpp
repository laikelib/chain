

#include "blocktemplate.h"
#include <hlog.h>

#include <serialize.h>
#include <block.h>
#include <hcrypto.h>
#include <hadaptation.h>


HRET CBlockTemplateCgi::Init(HCSTRR str) throw (HCException) {

    HPS ps = parseInput(str);

    HASSERT_RETURN(ps.count("addr"), INVL_PARA);

    m_str = ps["addr"];

    HRETURN_OK;
}


HRET CBlockTemplateCgi::Work() throw (HCException) {
    HFUN_BEGIN;

    using namespace HUIBASE::CRYPTO;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str2;
    HFAILED_THROW(sendAndRecv("block_template", m_str));

    LOG_NORMAL("laikelib return: [%s]", m_str.c_str());

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


HSTR  CBlockTemplateCgi::GetRes () const throw (HCException) {

    return m_res;

}


REGISTE_COMMAND(blocktemplate, CBlockTemplateCgi);
