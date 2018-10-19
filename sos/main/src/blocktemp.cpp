

#include <huibase.h>
#include "blocktemp.h"
#include <glog/logging.h>

#include "chainApp.h"

int CBlockTempService::ParseInput(SCM_DATA* data) throw (HCException){

    static constexpr unsigned int LEN_4M = 40960;

    char buf[LEN_4M] = {0};
    memcpy(buf, data->idata, data->ilen);

    LOG(INFO) << GetName() << ":ParseInput input request: [" <<
        buf << "]";

    m_strIn = buf;

    return 0;

}


int CBlockTempService::Excute() throw (HCException){
    LOG(INFO) << "CBlockTempService::Excute Begin...";

    m_strTemp = so_config->GetBlockTemplate (m_strIn);

    LOG(INFO) << "CBlockTempService::Excute End...";

    return 0;
}


int CBlockTempService::PackReturn(SCM_DATA* res) throw (HCException){

    if (m_strTemp.empty()) {

        LOG(ERROR) << "m_strTemp is null, set to 'error'";
        m_strTemp = "error";

    }

    RESET_ODATAP(res);

    strcpy(res->odata, m_strTemp.c_str());

    res->olen = m_strTemp.length();

    LOG(INFO) << GetName() <<  " return: " << m_strTemp;

    return 0;

}



int CGetBalService::Excute() throw (HCException) {
    LOG(INFO) << "CGetBalService::Excute Begin...";

    m_strTemp = so_config->GetBal(m_strIn);

    LOG(INFO) << "CGetBalService::Excute End...";

    return 0;
}



int CHashTxService::Excute() throw (HCException) {
    LOG(INFO) << __FUNCTION__ << " Begin...";

    try {
        m_strTemp = so_config->HashTx(m_strIn);
    } catch (const HCException& ex) {
        m_strTemp = "\"DROP\"";
    }

    LOG(INFO) << __FUNCTION__ << " End ...";
    return 0;
}
