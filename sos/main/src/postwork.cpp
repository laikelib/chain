


#include "postwork.h"
#include <glog/logging.h>

#include "chainApp.h"



int CPostWorkService::ParseInput(SCM_DATA* data) throw (HCException) {

    static constexpr unsigned int LEN_4M = 40960;

    char buf[LEN_4M] = {0};
    memcpy(buf, data->idata, data->ilen);

    LOG(INFO) << GetName() << ":ParseInput input request: [" <<
	buf << "]";

    m_str = buf;

    return 0;

}


int CPostWorkService::Excute() throw (HCException) {

    LOG(INFO) << GetName() << "::Excute begin...";

    m_str = so_config->PostWork (m_str);
    
    LOG(INFO) << GetName() << "::Excute end...";

    return 0;
    
}


int CPostWorkService::PackReturn(SCM_DATA* res) throw (HCException){

    if (m_str.empty()) {

	LOG(INFO) << "return null, set to 'error'";
	m_str = "\"error\"";
	
    }

    RESET_ODATAP(res);

    strcpy(res->odata, m_str.c_str());

    res->olen = m_str.length();

    LOG(INFO) << GetName () << "return " << m_str.length() << " bytes";
    LOG(INFO) << GetName() << " return: " << m_str;

    return 0;

}


int CHeightBlockService::Excute() throw (HCException) {

    m_str = so_config->HeightBlock(m_str);

    FILE * pf = fopen("abcd", "a+");
    fwrite(m_str.c_str(), 1, m_str.length(), pf);
    fclose(pf);

    return 0;

}


int CQueryAccount::Excute() throw (HCException) {

    m_str = so_config->QueryAccount(m_str);

    return 0;

}


int CGetTxs::Excute() throw (HCException) {

    m_str = so_config->GetTxs (m_str);

    return 0;

}


int CAccountInfo::Excute() throw (HCException) {

    m_str = so_config->AccountInfo(m_str);

    return 0;

}











