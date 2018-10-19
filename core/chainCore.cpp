
#include "chainCore.h"
#include <glog/logging.h>
#include <hstr.h>
#include <hfname.h>
#include <huibase.h>

using namespace HUIBASE;

CChainCore::~CChainCore () {}


HRET CChainCore::Init () {

	HNOTOK_RETURN(initFirst ());

	HNOTOK_RETURN(initDic ());

	HNOTOK_RETURN(initConf ());

	HNOTOK_RETURN(initLog ());	

	HNOTOK_RETURN(initLast ());

	testInit();

	HRETURN_OK;

}


HRET CChainCore::Shutdown() {

    HRETURN_OK;
}


HRET CChainCore::InitChain () {


	HRETURN_OK;
}


HSTR CChainCore::GetValue (HCSTRR strKey) {

	IF_FALSE(m_conf.IsHere(strKey)) {
		return "";
	}

	return m_conf.GetValue(strKey);

}


HSTR CChainCore::GetValue (HCSTRR strKey, HCSTRR def) {
	return m_conf.GetValue(strKey, def);
}


void CChainCore::SetParm(HCSTRR strKey, HCSTRR strValue) {
	m_conf.SetValue(strKey, strValue);
}


// check the default direction.
HRET CChainCore::initDic () {

    //int fd = open("tem_log", O_CREAT | O_RDWR, 0666);

    HCDic curDic = HCDic::GetCurrentDict();
    HCDic soDic = curDic;
    soDic.GoToParent ();
    m_conf.SetValue(SO_DIR, soDic.GetName());
    //ssize_t wcb = write(fd, soDic.GetName().c_str(), soDic.GetName().length());
    //wcb = write(fd, "\n", 1);
    
    HCDic binDic = soDic;
    binDic.Append("bin");
    m_conf.SetValue(SO_BIN_DIR, binDic.GetName());

    //wcb = write(fd, binDic.GetName().c_str(), binDic.GetName().length());
    //wcb = write(fd, "\n", 1);

    IF_FALSE(HCDic::IsExactExists(binDic)) {

	HNOTOK_RETURN(HCDic::CreateDic(binDic.GetName()));

    }

    HCDic confDic = soDic;
    confDic.Append("conf");
    m_conf.SetValue(SO_CONF_DIR, confDic.GetName());

    //wcb = write(fd, confDic.GetName().c_str(), confDic.GetName().length());
    //wcb = write(fd, "\n", 1);

    IF_FALSE(HCDic::IsExactExists(confDic)) {

	HNOTOK_RETURN(HCDic::CreateDic(confDic.GetName()));
	
    }

    HCDic logDic = soDic;
    logDic.Append("log");
    m_conf.SetValue(SO_LOG_DIR, logDic.GetName());

    //wcb = write(fd, logDic.GetName().c_str(), logDic.GetName().length());
    //wcb = write(fd, "\n", 1);

    IF_FALSE(HCDic::IsExactExists(logDic)) {
	
	HNOTOK_RETURN(HCDic::CreateDic(logDic.GetName()));

    }

    HCDic dataDic = soDic;
    dataDic.Append ("data");
    m_conf.SetValue (SO_DATA_DIR, dataDic.GetName ());

    //wcb = write(fd, dataDic.GetName().c_str(), dataDic.GetName().length());
    //wcb = write(fd, "\n", 1);

    IF_FALSE (HCDic::IsExactExists (dataDic)) {

	HNOTOK_RETURN(HCDic::CreateDic (dataDic.GetName()));
	    
    }

    //(void)wcb;
    //close(fd);
    HRETURN_OK;

}



// load the default conf.
HRET CChainCore::initConf () {

    //int fd = open("tem_log2", O_CREAT | O_RDWR, 0666);
    //ssize_t wcb = write(fd, "initConf", strlen("initConf"));
    //wcb = write(fd, "\n", 1);

    HCDic confDic (m_conf.GetValue(SO_CONF_DIR));
    confDic.Append("conf");

    HNOTOK_RETURN(m_conf.LoadConfFile(confDic.GetName()));

    IF_FALSE (m_conf.IsHere(CONF_APP_NAME)) {
	HRETURN(ERR_STATUS);	
    }

	
   IF_FALSE (m_conf.IsHere(CONF_LOG_NAME)) {
       m_conf.SetValue(CONF_LOG_NAME, m_conf.GetValue(CONF_APP_NAME));
   }

   //(void) wcb;
   //close(fd);
			
   HRETURN_OK;

}


HRET CChainCore::initLog () {

    int fd = open("tem_log3", O_CREAT | O_RDWR, 0666);
    ssize_t wcb = write(fd, "initlog", strlen("initlog"));
    wcb = write(fd, "\n", 1);

    // Initialize Google's logging library.
    google::InitGoogleLogging(m_conf.GetValue(CONF_LOG_NAME).c_str());
    FLAGS_log_dir = m_conf.GetValue(SO_LOG_DIR);

    wcb = write(fd, m_conf.GetValue(SO_LOG_DIR).c_str(), m_conf.GetValue(SO_CONF_DIR).length());
    wcb = write(fd, "\n", 1);
    wcb = write(fd, m_conf.GetValue(CONF_LOG_NAME).c_str(), m_conf.GetValue(CONF_LOG_NAME).length());
    wcb = write(fd, "\n", 1);
    
    FLAGS_stderrthreshold = 3;    // 3: FATAL
    FLAGS_max_log_size    = 100;  // max log file size 100 MB
    FLAGS_logbuflevel     = -1;   // don't buffer logs
    FLAGS_stop_logging_if_full_disk = true;

    LOG(INFO) << "google logging init success";
    LOG(ERROR) << "google logging error test";
    LOG(WARNING) << "gooogle logging warrning test";
    
    (void)wcb;
    close(fd);

    HRETURN_OK;
	
}


void CChainCore::testInit() {

	LOG(INFO) << SO_BIN_DIR  << ": " << m_conf.GetValue(SO_BIN_DIR);

	LOG(INFO) << SO_CONF_DIR  << ": " << m_conf.GetValue(SO_CONF_DIR);

	LOG(INFO) << SO_LOG_DIR  << ": " << m_conf.GetValue(SO_LOG_DIR);

	LOG(INFO) << CONF_APP_NAME  << ": " << m_conf.GetValue(CONF_APP_NAME);

	LOG(INFO) << CONF_LOG_NAME  << ": " << m_conf.GetValue(CONF_LOG_NAME);

}











