
#include <CCgi.h>
#include "transfer.h"
#include <sstream>
#include <algorithm>  

using namespace std;

using namespace HUIBASE;

CTransferTrans::CTransferTrans(CReqData* req)
    : CTrans(req){

}


int CTransferTrans::Commit() {

    int ret = 0;
    
    try {

	SetRKey (108);

	ret = CommitReq();

	ret = CommitWork();

	ret = CommitRes();

    } catch (const HCException& ex) {

	LOG_ERROR("Get an exception: %s", ex.what());
	
	cout << "Content-Type: text/html \r\n\r\n";
	cout << "{\"errno\":9999,\"err_msg\":\"error in cgi\"}" << endl;
	
    }

    
    return 0;

}

int CTransferTrans::CommitReq() {
    HFUN_BEGIN;

    HSTR str;

    str = m_pReqData->GetPara("value");
    CheckParaValid(str);

    HASSERT_THROW_MSG(HIS_TRUE(HCStr::IsDigitAndDos(str)), "value must to be digit", INVL_PARA);

    m_dValue = HCStr::stod(str);

    str = m_pReqData->GetPara("fee");
    CheckParaValid(str);

    HASSERT_THROW_MSG(HIS_TRUE(HCStr::IsDigitAndDos(str)), "fee must to be digit", INVL_PARA);

    m_dFee = HCStr::stod(str);

    m_strTo = m_pReqData->GetPara("tos");
    CheckParaValid(m_strTo);

    m_strFo = m_pReqData->GetPara("fos");
    CheckParaValid(m_strFo);

    m_strPub = m_pReqData->GetPara("pub");
    CheckParaValid(m_strPub);

    m_strPri = m_pReqData->GetPara("pri");
    CheckParaValid(m_strPri);

    HFUN_END;
    return 0;
}


int CTransferTrans::CommitWork() {
    HFUN_BEGIN;

    m_str = transfer();

    LOG_NORMAL("get new account: [%s]", m_str.c_str());

    /*
    CMysqlCommand qry(GetConnection("laikelib"));
    
    std::string sql;
    sql =  HSTR("update t_config set modifyTime = now(), `value` = '") + m_str + "' where pid = 1001 and `key` = 'height'";

    HINT cb = qry.Update(sql.c_str());

    HASSERT_THROW_MSG(cb > -1, "mysql update failed", DB_DISCONN);
    */

    HFUN_END;
    return 0;
}

int CTransferTrans::CommitRes() {
    HFUN_BEGIN;

    //cout << "Content-Type: text/html \r\n\r\n";
    cout << "Content-Type: text/plain \r\n";
    cout << "Access-Control-Allow-Origin: * \r\n";
    cout << "Access-Control-Allow-Method: POST,GET \r\n\r\n";
    //"{\"errno\":9999,\"err_msg\":\"error in cgi\"}"
    stringstream ss;

    ss << "{\"errno\":0, \"err_msg\":\"OK\", \"result\": " << m_str << "}";

    HSTR str = ss.str();
    cout << str;

    HFUN_END;
    return 0;
}


CTrans* CCgi::MakeTransObj(CReqData* req) {

    return new CTransferTrans(req);

}


HSTR CTransferTrans::transfer() {

    HASSERT_THROW_MSG(m_pMq != nullptr, "message qeueu is not initialize", ILL_PT);

    stringstream ss;
    ss << "{\"value\":" << m_dValue << ", \"fee\":" << m_dFee << ", \"sender\": \"" << m_strFo <<  "\",\"receiver\":\"" << m_strTo << "\", \"public\":\""<< m_strPub <<"\", \"private\":\"" << m_strPri << "\"}";
	//HSTR str = HCStr::Format("{\"value\": %f, \"fee\": %f, \"sender\": \"%s\",  \"receiver\":\"%s\", \"public\":\"%s\", \"private\":\"%s\"}",
	//			     m_dValue, m_dFee, m_strFo.c_str(),
	//		     m_strTo.c_str(), m_strPub.c_str(), m_strPri.c_str());
    HSTR str = ss.str();
    LOG_NORMAL("sent mq: [%s]", str.c_str());
    
    HFAILED_THROW(sendAndRecv("transfer", str));

    return str;
}

















