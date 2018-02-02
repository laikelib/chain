

#ifndef __CREQ_DATA_H__
#define __CREQ_DATA_H__



#include <cgicc/FormFile.h>
#include <cgicc/CgiEnvironment.h>
#include <limits>

#include <huibase.h>
#include <hdict.h>
#include <hconf.h>

#include <cgicc/FormFile.h>
#include <cgicc/CgiEnvironment.h>

using namespace HUIBASE;


class CReqData
{
public:
    CReqData(  )
    {
    }


    ~CReqData(  )
    {
    }

    

    string GetEnv( const string & paraName );

    void SetPara( const string & paraName, const string & paraValue );

    HSTR GetPara (HCSTRR paraName);

    void SetCookie( const string & paraName, const string & paraValue );

    void SetEnv( const string & paraName, const string & paraValue );

    void SetWebData( const cgicc::CgiEnvironment & cgiEnv );

    HCSTRR GetWebData () const { return m_strWebData; }

    void SetConf (HCMapConf * pConf) { m_pConf = pConf; }

    HSTR GetConfValue (HCSTR key) throw (HCException);

    void SaveUploadFile (const cgicc::FormFile& uploadFile);

    void GetUploadFiles (std::vector<cgicc::FormFile>& files);

    const HCMapConf* GetConf () const { return m_pConf; }
    
private:

    HCParam m_formData;
    HCParam m_envData;
    
    HCParam m_cookieData;
    
    string m_strWebData;

    HCMapConf *m_pConf = nullptr;

    std::vector<cgicc::FormFile> m_uploadFiles;
    
};


 
#endif








