

#include <huibase.h>
#include "newtx.h"
#include <glog/logging.h>
#include "chainApp.h"



int CNewTxService::ParseInput(SCM_DATA* data) throw (HCException){

    static constexpr unsigned int LEN_64M = 40960 * 16;

    char buf[LEN_64M] = {0};
    memcpy(buf, data->idata, data->ilen);

    LOG(INFO) << GetName() << ":ParseInput input request: [" << buf << "]";

    m_str = buf;

    return 0;

}


int CNewTxService::Excute() throw (HCException){
    LOG(INFO) << __FUNCTION__ << " Begin...";

    m_str = so_config->newTx (m_str);

    LOG(INFO) << __FUNCTION__ << " End...";

    return 0;
}


int CNewTxService::PackReturn(SCM_DATA* res) throw (HCException){

    if (m_str.empty()) {

        LOG(ERROR) << "output string is null, set to 'error'";
        m_str = "DROP";

    }

    RESET_ODATAP(res);

    strcpy(res->odata, m_str.c_str());

    res->olen = m_str.length();

    LOG(INFO) << GetName() <<  " return: " << m_str;

    return 0;

}
