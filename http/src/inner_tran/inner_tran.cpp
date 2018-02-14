
#include <CCgi.h>
#include "inner_tran.h"
#include <sstream>
#include <algorithm>  

using namespace std;

using namespace HUIBASE;

CInnerTranCTrans::CInnerTranCTrans(CReqData* req)
    : CTrans(req){

}


int CInnerTranCTrans::Commit() {

    int ret = 0;
    
    try {

	SetRKey (106);

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

int CInnerTranCTrans::CommitReq() {
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
    CheckParaValid(m_strTo);    

    HFUN_END;
    return 0;
}


int CInnerTranCTrans::CommitWork() {
    HFUN_BEGIN;

    m_str = innerTran();

    LOG_NORMAL("new transaction: [%s]", m_str.c_str());

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

int CInnerTranCTrans::CommitRes() {
    HFUN_BEGIN;

    cout << "Content-Type: text/html \r\n\r\n";
    //"{\"errno\":9999,\"err_msg\":\"error in cgi\"}"
    stringstream ss;

    ss << "{\"errno\": 0, \"err_msg\":\"OK\", \"result\": " << m_str << "}";

    HSTR str = ss.str();
    cout << str;

    HFUN_END;
    return 0;
}


CTrans* CCgi::MakeTransObj(CReqData* req) {

    return new CInnerTranCTrans(req);

}


HSTR CInnerTranCTrans::innerTran() {

    HASSERT_THROW_MSG(m_pMq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = HCStr::Format("{\"value\": %f, \"fee\": %f, \"sender\": \"%s\", \"receiver\":\"%s\"}",
			     m_dValue, m_dFee, m_strFo.c_str(),  m_strTo.c_str());

    LOG_NORMAL("sent mq: [%s]", str.c_str());
    
    HFAILED_THROW(sendAndRecv("inner_transfer", str));

    return str;
}


