#include "CCgi.h"
#include "cgicc/CgiEnvironment.h"

#include <hprocess.h>
#include <hfname.h>
#include <hlog.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


using namespace cgicc;


CCgi::CCgi(  )
{
    m_pTrans = NULL;

}

CCgi::~CCgi(  )
{
    if ( m_pTrans != NULL )
    {
        delete m_pTrans;
    }
}


void CCgi::ThrowWebError( const string & sErrMsg )
{

    cout << "Content-Type: text/html \r\n\r\n";
    cout << "{\"errno\":9999,\"err_msg\":\"" << sErrMsg << "\"}" << endl;
    exit( 1 );
}

void CCgi::LoadConfigure(  )
{
    m_conf.SetValue("conf", "../conf/conf");
    if (HNOT_OK(m_conf.LoadConfFile(m_conf.GetValue("conf")))) {
	exit(-1);
    }

    HSTR strName;
    if (HNOT_OK(HCProcess::GetProcName(strName))) {
	exit(-1);
    }

    m_conf.SetValue(CGI_NAME, strName);

    //HSTR strLogFileName = HSTR("../log/") + strName + ".log";
    HCFileName fname(strName);
    strName = fname.JustFileName ();

    strName = HSTR("../log/") + strName;
    
    InitLog( strName.c_str());


}

void CCgi::Run(  )
{

    LoadConfigure(  );

    m_reqData.SetConf(&m_conf);
    
    FetchHttpRequest(  );

    m_pTrans = MakeTransObj(&m_reqData);
    if (not m_pTrans )
    {
	ThrowWebError("tran is null");
	exit(1);
    }

    m_pTrans->StartWork();
}


int CCgi::GetCharset( cgicc::Cgicc * pCgi )
{
    const cgicc::CgiEnvironment & cgiEvn = pCgi->getEnvironment(  );

    if ( !strcasecmp( cgiEvn.getRequestMethod(  ).c_str(  ), "post" ) )
    {
        if ( cgiEvn.getPostData(  ).find( "input_charset=GBK" ) !=
             string::npos )
        {
            return 1;
        }
        return 0;
    }
    else
    {
        if ( cgiEvn.getQueryString(  ).find( "input_charset=GBK" ) !=
             string::npos )
        {
            return 1;
        }
        return 0;
    }
}


int CCgi::FetchHttpRequest(  )
{

    Cgicc formData;
    const_form_iterator iter;
    int iCharset = GetCharset( &formData );

    for ( iter = formData.getElements(  ).begin(  );
          iter != formData.getElements(  ).end(  ); ++iter )
    {

	m_reqData.SetPara( iter->getName(  ), iter->getValue(  ) );

    }

    const CgiEnvironment & env = formData.getEnvironment(  );
    
    m_reqData.SetWebData( env );

    /*LOG_NORMAL("method: %s", env.getRequestMethod().c_str());
    LOG_NORMAL("get_data: %s", env.getQueryString().c_str());
    LOG_NORMAL("data: %s", m_reqData.GetWebData().c_str());
    LOG_NORMAL("romote ip: %s", env.getRemoteAddr(  ).c_str() );
    LOG_NORMAL("userAgent: %s", env.getUserAgent().c_str());*/

    m_reqData.SetEnv( "ClientIp", GetClientIp( env.getRemoteAddr(  ) ) );
    m_reqData.SetEnv( "ClientAgent", env.getUserAgent(  ) );
    m_reqData.SetEnv( "RequestMethod", env.getRequestMethod(  ) );
    m_reqData.SetEnv( "CgiName", env.getScriptName(  ) );
    m_reqData.SetEnv( "referer", env.getReferrer(  ) );
    m_reqData.SetEnv( "ServerIp",
                      getenv( "SERVER_ADDR" ) ==
                      NULL ? "" : getenv( "SERVER_ADDR" ) );
    const vector < HTTPCookie > &cks = env.getCookieList(  );

    for ( int i = 0; i < cks.size(  ); i++ )
    {
        m_reqData.SetCookie( cks[i].getName(  ), cks[i].getValue(  ) );
    }

    for (cgicc::const_file_iterator cfi = formData.getFiles().begin();
	 cfi != formData.getFiles().end(); ++cfi) {

	m_reqData.SaveUploadFile(*cfi);
	
    }


    return 0;
}

char *CCgi::MakeMsgNo(  )
{
    static char cMsgNo[32] = { 0 };
    char *pLocalIp = getenv( "SERVER_ADDR" );

    if ( pLocalIp == NULL )
    {
	throw HCException(HERR_NO(ILL_PT), HTRUE, HFALSE, __LINE__, __FILE__);
    }

    in_addr_t lIp = inet_addr( pLocalIp );
    time_t t = time( NULL );
    pid_t iPid = getpid(  );

    snprintf( cMsgNo, sizeof( cMsgNo ) - 1, "510%08x%010u%05u", lIp, t, iPid );
    return cMsgNo;
}

string CCgi::GetClientIp( const string & strRemoteAddr )
{
   
    /*char *p = getenv( "HTTP_X_FORWARDED_FOR" );

    if ( p == NULL )
    {
        return strRemoteAddr;
    }
    else
    {
        CStrVector strVec;

        Tools::StrToVector( strVec, p, "," );
        return strVec[0];
	}*/
    return strRemoteAddr;
    
}
    

