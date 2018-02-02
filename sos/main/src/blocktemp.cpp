

#include <huibase.h>
#include "blocktemp.h"
#include <glog/logging.h>

#include "chainApp.h"

int CBlockTempService::ParseInput(SCM_DATA* req) throw (HCException){

    (void) req;
    return 0;

}


int CBlockTempService::Excute() throw (HCException){
    LOG(INFO) << "CBlockTempService::Excute Begin...";
    
    m_strTemp = so_config->GetBlockTemplate ();

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








