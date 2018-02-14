

#include "CReqData.h"


void CReqData::SetPara( const string & paraName, const string & paraValue )
{
    m_formData[paraName] = paraValue;
}

HSTR CReqData::GetPara(HCSTRR paraName) {

    if ( m_formData.find( paraName ) != m_formData.end(  ) )
    {
        return m_formData[paraName];
    }
    return "";

}


void CReqData::SetCookie( const string & paraName, const string & paraValue )
{
    m_cookieData[paraName] = paraValue;
}

void CReqData::SetEnv( const string & paraName, const string & paraValue )
{
    m_envData[paraName] = paraValue;
}

string CReqData::GetEnv( const string & paraName )
{
    if ( m_envData.find( paraName ) != m_envData.end(  ) )
    {
        return m_envData[paraName];
    }
    return "";
}

void CReqData::SetWebData( const cgicc::CgiEnvironment & cgiEnv )
{
    
    if(!strcasecmp( cgiEnv.getRequestMethod(  ).c_str(  ), "post" )){
	m_strWebData = cgiEnv.getPostData(  );
    } else {
	// get
	m_strWebData = cgiEnv.getQueryString();
    }
}


HSTR CReqData::GetConfValue (HCSTR key) throw (HCException) {

    HASSERT_THROW(m_pConf != nullptr, ILL_PT);
    
    return m_pConf->GetValue (key);
    
}

void CReqData::SaveUploadFile(const cgicc::FormFile& uploadFile) {

    m_uploadFiles.push_back(uploadFile);

}

void CReqData::GetUploadFiles(std::vector<cgicc::FormFile>& files) {

    files = m_uploadFiles;

}


