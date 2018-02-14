
#include <CCgi.h>
#include "new_account.h"
#include <sstream>
#include <algorithm>  
using namespace std;

using namespace HUIBASE;

CNewAccountCTrans::CNewAccountCTrans(CReqData* req)
    : CTrans(req){

}


int CNewAccountCTrans::Commit() {

    int ret = 0;
    
    try {

	SetRKey (107);

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

int CNewAccountCTrans::CommitReq() {
    HFUN_BEGIN;

    HFUN_END;
    return 0;
}


int CNewAccountCTrans::CommitWork() {
    HFUN_BEGIN;

    m_str = getNewAccount();

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

int CNewAccountCTrans::CommitRes() {
    HFUN_BEGIN;

    cout << "Content-Type: text/html \r\n\r\n";
    //"{\"errno\":9999,\"err_msg\":\"error in cgi\"}"
    stringstream ss;

    ss << "{\"errno\":0, \"err_msg\":\"OK\", \"result\": \"" << m_str << "\"}";

    HSTR str = ss.str();
    cout << str;

    HFUN_END;
    return 0;
}


CTrans* CCgi::MakeTransObj(CReqData* req) {

    return new CNewAccountCTrans(req);

}


HSTR CNewAccountCTrans::getNewAccount() {

    HASSERT_THROW_MSG(m_pMq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = "key=val";

    
    HFAILED_THROW(sendAndRecv("new_account", str));

    return str;
}

















