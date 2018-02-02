#ifndef __CGICOMM_CGI_H__
#define __CGICOMM_CGI_H__

#include <vector>
#include "cgicc/FormFile.h"
#include "cgicc/Cgicc.h"

#include "CTrans.h"
#include "CReqData.h"

#include <huibase.h>
#include <hconf.h>

using namespace HUIBASE;

#define CGI_NAME "CGI_NAME"

class CReqData;
class CCgi
{
public:
    CCgi(  );
    virtual ~ CCgi(  );

    void Run(  );
    
    virtual CTrans *MakeTransObj(CReqData* req  );
    
  protected:
    void LoadConfigure(  );
    
    int FetchHttpRequest(  );

    int GetCharset( cgicc::Cgicc * pCgi );

    void ThrowWebError( const string & sErrMsg );

    char *MakeMsgNo(  );

    string GetClientIp( const string & strRemoteAddr );
    
private:
    HCMapConf m_conf;

    CTrans *m_pTrans;

    CReqData m_reqData;

    
};



#endif
