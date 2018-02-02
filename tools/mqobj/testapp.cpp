

#include "testapp.h"
#include "testserver.h"
#include "../safeMiddle/common/middleConfig.h"

#include <hlog.h>
#include <hfname.h>

CTestApp::CTestApp(HINT argc, HCHAR* argv[])
    : HCHapp(argc, argv){

    m_pServer = new CTestServer();
    CHECK_NEWPOINT(m_pServer);

}


CTestApp::~CTestApp() {

    HDELP(m_pServer);

}


HBOOL CTestApp::Run() {

    while (HIS_TRUE(IsRunning())) {

	try {

	    m_pServer->RunServer ();


	} catch(HCException& ex) {


	}

    }

    LOG_WS("MinerApp is over");

    return HTRUE;
}


void CTestApp::init() {

    middle_config->SetConf(m_conf);

    HSTR strSosCfg = middle_config->GetValue("admin_conf");
    LOG_NORMAL("sos config: [%s]", strSosCfg.c_str());

    HFAILED_THROW(middle_config->LoadSoConfig());

    middle_config->ShowSoConfig();

    HFAILED_THROW(m_pServer->Init());
    
}
