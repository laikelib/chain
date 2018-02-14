


#include <huibase.h>
#include "newaccount.h"
#include <glog/logging.h>

#include "chainApp.h"



int CNewAccountService::Excute() throw (HCException) {
    LOG(INFO) << "CNewAccountService::Excute begin...";
    
    m_strTemp = so_config->NewAccount();

    LOG(INFO) << "CNewAccountService::Excute end...";
    return 0;
}



int CGetHeightService::Excute() throw (HCException) {
    LOG(INFO) << "CNewAccountService::Excute begin...";
    
    m_strTemp = so_config->GetHeight();

    LOG(INFO) << "CNewAccountService::Excute end...";
    return 0;
}
