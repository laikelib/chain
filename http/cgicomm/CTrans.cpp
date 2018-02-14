#include "CTrans.h"

#include <huibase.h>
#include <hstr.h>

#include <dbcomm.h>

#include <libconfig.h++>

#include <hlog.h>

#include "../../safeMiddle/common/middleConfig.h"

using namespace HUIBASE;
using namespace HUIBASE::MYSQL;


CTrans::CTrans( CReqData * pReqData)
{
    m_pReqData = pReqData;

    HASSERT_THROW_MSG(m_pReqData != nullptr, "req_data is nullptr", ILL_PT);

    middle_config->SetConf(*(pReqData->GetConf()));
}

CTrans::~CTrans() {

    for (CONNMAP::iterator it = m_mapConn.begin(); it != m_mapConn.end();
	 ++it) {

	CMysqlConnection * p = it->second;

	delete p;
	
    }
}

CMysqlConnection* CTrans::GetConnection(HCSTRR strName) {

    if (strName.empty()) {

	return &m_conn;

    }

    CONNMAP::iterator fit = m_mapConn.find(strName);

    HASSERT_THROW(fit != m_mapConn.end(), INVL_PARA);

    return fit->second;

}

void CTrans::InitMysql() {

    CONN_INFO ci;
    ci.m_db = m_pReqData->GetConfValue("MYSQL_DB");
    ci.m_host = m_pReqData->GetConfValue("MYSQL_HOST");
    ci.m_user = m_pReqData->GetConfValue("MYSQL_USER");
    ci.m_pw = m_pReqData->GetConfValue("MYSQL_PW");
    ci.m_port = HCStr::stoi( m_pReqData->GetConfValue("MYSQL_PORT"));

    m_conn.SetConnInfo(ci);

    m_conn.SetUtf8();

    initConnections();
        
    HFAILED_THROW(m_conn.Connect());

}

void CTrans::ThrowWebError( const string & sErrMsg ) {

    cout << "Content-Type: text/html \r\n\r\n";
    cout << "{\"errno\":9999,\"err_msg\":\"" << sErrMsg << "\"}" << endl;
    exit( 1 );    

}


void CTrans::InitSession(  )
{
    HSTR token = m_pReqData->GetPara( "token" );
    
    if (not token.empty() )
    {

	CMysqlCommand comm(&m_conn);

	DBRes res;

	HSTR __sql = HSTR("select id, tuid, phone, token from t_users where token = '") + token + "';";
	comm.Query(HSTR("select id, tuid, phone, token from t_users where token = '") + token + "';", res);
	LOG_NORMAL("query token sql: [%s]", __sql.c_str());

	if (res.empty()) {
	    
	    //throw HCException(HERR_NO(ERR_STATUS), HTRUE, HFALSE, __LINE__, __FILE__);
	    cout << "Content-Type: text/html \r\n\r\n";
	    cout << "{\"errno\":1002,\"err_msg\":\"user token error\"}" << endl;
	    exit( 1 );    
	}

	HCParam row = res[0];
	m_pReqData->SetPara(SESSION_TUID, row["tuid"]);
	m_pReqData->SetPara(SESSION_UID, row["id"]);
	m_pReqData->SetPara(SESSION_PHONE, row["phone"]);

	LOG_NORMAL("session_uid: %s, phone: %s", row["id"].c_str(), row["phone"].c_str());
            
    }
}

int CTrans::StartWork(  )
{
    HFUN_BEGIN;
    
    //CheckReqVersion();
    
    CheckIPLimit();

    InitMysql();

    InitSession();

    CMqObj::Init();

    HFUN_END;
    
    return Commit(  );
}

void CTrans::CheckReqVersion() {

    HSTR str = m_pReqData->GetPara("version");
    HSTR cl = m_pReqData->GetPara("cl");
    HSTR agent = m_pReqData->GetEnv("ClientAgent");

    if (str.empty() || cl.empty() || str != "1.0")  {
	LOG_ES("version or useragent is empty");
	ThrowWebError("invalid argument");
    }

    LOG_NORMAL("Version: %s, client: %s, UserAgent: %s", str.c_str(), cl.c_str(), agent.c_str());
    
    if (cl != "pc" && cl != "app") {
	ThrowWebError("invalid argument");
    }


}


void CTrans::CheckParaValid(HSTRR str) {
    HFUN_BEGIN;
    
    HCStr::Trim (str);

    HCStr::Removes (str, "'");
    HCStr::Removes (str, "\"");
    HCStr::Removes (str, "\r");
    
    

    HFUN_END;
}


void CTrans::CheckIPLimit(  )
{

    LOG_NORMAL("Get Request from %s", m_pReqData->GetEnv("ClientIp").c_str());

}

void CTrans::initConnections () {
    HFUN_BEGIN;

    using namespace libconfig;

    Config cfg;

    try {
		
	cfg.readFile("../conf/db.cfg");
		
    } catch (const FileIOException& fiex) {

	LOG_ERROR("I/O error while read file. msg: [%s]", fiex.what());
	return;

    } catch (const ParseException& pex ) {

	LOG_ERROR("Parse config file failed at %s:%d--%s", pex.getFile(),
		  pex.getLine(), pex.getError());
	return ;
		
    }


    {

	const Setting& root = cfg.getRoot();
	const Setting& dbs = root["dbs"];

	for (int i = 0; i < dbs.getLength(); ++i) {

	    CONN_INFO ci;
	    dbs[i].lookupValue("dbname", ci.m_db);
	    HSTR strPort;
	    dbs[i].lookupValue("ip", ci.m_host);
	    dbs[i].lookupValue("port", strPort);
	    dbs[i].lookupValue("user", ci.m_user);
	    dbs[i].lookupValue("pass", ci.m_pw);
	    ci.m_port = HCStr::stoi(strPort);
		
	    CMysqlConnection * p = new CMysqlConnection(ci.m_db, ci.m_host, ci.m_user, ci.m_pw, ci.m_port);

	    //LOG_NORMAL("new connection db[%s] success", ci.m_db.c_str());
		
	    p->SetUtf8();
		
	    HRET cb = p->Connect ();

	    //LOG_NORMAL("connect db[%s] success", ci.m_db.c_str());
	    
	    dbs[i].lookupValue("name", strPort);
	    
	    //LOG_NORMAL("name: [%s]", strPort.c_str());
	    
	    HIF_OK(cb) {
		LOG_NORMAL("connect info: db[%s] name[%s] host[%s]", ci.m_db.c_str(), strPort.c_str(), ci.m_host.c_str());

		m_mapConn.insert (CONNMAP::value_type(strPort, p));
	    } else {
		
		LOG_ERROR("connection name [%s] connect failed", strPort.c_str());
		delete p;

	    }

	    LOG_NORMAL("connect db[%s] finished", ci.m_db.c_str());

	    
	}
		
    }
	
    

    HFUN_END;
}








