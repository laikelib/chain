


#include <huibase.h>
#include "getheight.h"
#include <glog/logging.h>

#include "chainApp.h"



int CGetHeightService::Excute() throw (HCException) {
    LOG(INFO) << "CNewAccountService::Excute begin...";
    
    m_strTemp = so_config->GetHeight();

    LOG(INFO) << "CNewAccountService::Excute end...";
    return 0;
}





int CLkInfoService::Excute() throw (HCException) {
    LOG(INFO) << "CBlockTempService::Excute Begin...";

    m_str = so_config->LkInfo ();

    LOG(INFO) << "CBlockTempService::Excute End...";

    return 0;
}



int CLkInfoService::PackReturn (SCM_DATA* data) throw (HCException) {

    if (m_str.empty()) {

        LOG(ERROR) << "m_strTemp is null, set to 'error'";
        m_str = "error";

    }

    RESET_ODATAP(data);

    strcpy(data->odata, m_str.c_str());

    data->olen = m_str.length();

    LOG(INFO) << GetName() <<  " return: " << m_str;


    return 0;

}
