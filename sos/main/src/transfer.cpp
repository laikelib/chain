

#include "transfer.h"
#include <glog/logging.h>
#include "chainApp.h"

#include <json/json.h>

int CTransfer::ParseInput(SCM_DATA* data) throw (HCException) {

    static constexpr unsigned int LEN_4M = 40960;

    char buf[LEN_4M] = {0};

    memcpy(buf, data->idata, data->ilen);

    LOG(INFO) << GetName() << ": ParseInput input request [" << buf << "]";

    Json::Reader reader;
    Json::Value root;

    HSTR str = buf;
    HASSERT_THROW_MSG(reader.parse(str, root), "parse input json failed", INVL_PARA);

    m_dValue = root["value"].asDouble();

    m_dFee = root["fee"].asDouble();

    m_strSender = root["sender"].asString();

    m_strReceiver = root["receiver"].asString();

    m_strPublic = root["public"].asString();

    m_strPrivate = root["private"].asString();

    LOG(INFO) << "value: " << m_dValue << ", fee: " << m_dFee
	      << ", sender: " << m_strSender << ", receiver: " << m_strReceiver
	      << ", public: " << m_strPublic 
	      << ", private: " << m_strPrivate;
    
    return 0;

}


int CTransfer::Excute() throw (HCException) {

    m_result = so_config->Transfer (m_dValue, m_dFee, m_strSender,
				    m_strReceiver, m_strPublic,  m_strPrivate);

    return 0;    

}


int CTransfer::PackReturn(SCM_DATA* data) throw (HCException) {


    if (m_result.empty()) {

	LOG(INFO) << "return null, set to '{\"errno\":505, \"errmsg\":\"unknown exception\", result: \"error\"}'";
	m_result = "{\"errno\":505, \"errmsg\":\"unknown exception\", result: \"error\"}";
	
    }

    RESET_ODATAP(data);

    strcpy(data->odata, m_result.c_str());
    data->olen = m_result.length();

    LOG(INFO) << GetName() << " return: " << m_result;

    return 0;

}


int CInnerTransfer::ParseInput(SCM_DATA* data) throw (HCException) {

    static constexpr unsigned int LEN_4M = 40960;

    char buf[LEN_4M] = {0};

    memcpy(buf, data->idata, data->ilen);

    LOG(INFO) << GetName() << ": ParseInput input request [" << buf << "]";

    Json::Reader reader;
    Json::Value root;

    HSTR str = buf;
    HASSERT_THROW_MSG(reader.parse(str, root), "parse input json failed", INVL_PARA);

    m_dValue = root["value"].asDouble();

    m_dFee = root["fee"].asDouble();

    m_strSender = root["sender"].asString();

    m_strReceiver = root["receiver"].asString();

    LOG(INFO) << "value: " << m_dValue << ", fee: " << m_dFee << ",sender: " << m_strSender
	      << ", receiver: " << m_strReceiver;
    
    return 0;

}


int CInnerTransfer::Excute() throw (HCException) {
    
    m_result = so_config->InnerTransfer(m_dValue, m_dFee, m_strSender,  m_strReceiver);

    return 0;

}


int CInnerTransfer::PackReturn(SCM_DATA* data) throw (HCException) {

    if (m_result.empty()) {

	LOG(INFO) << "return null, set to '{\"errno\":505, \"errmsg\":\"unknown exception\", result: \"error\"}'";
	m_result = "{\"errno\":505, \"errmsg\":\"unknown exception\", result: \"error\"}";
	
    }

    RESET_ODATAP(data);

    strcpy(data->odata, m_result.c_str());
    data->olen = m_result.length();

    LOG(INFO) << GetName() << " return: " << m_result;

    return 0;

}
