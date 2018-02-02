
#include <CCgi.h>
#include "get_height.h"
#include <sstream>
#include <algorithm>  
using namespace std;

using namespace HUIBASE;

CGetHeightCTrans::CGetHeightCTrans(CReqData* req)
    : CTrans(req){

}


int CGetHeightCTrans::Commit() {

    int ret = 0;
    
    try {

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

int CGetHeightCTrans::CommitReq() {
    HFUN_BEGIN;

    HFUN_END;
    return 0;
}


int CGetHeightCTrans::CommitWork() {
    HFUN_BEGIN;

    m_str = getLaikeHeight ();

    LOG_NORMAL("laikelib height: [%s]", m_str.c_str());

    CMysqlCommand qry(GetConnection("laikelib"));
    
    std::string sql;
    sql =  HSTR("update t_config set modifyTime = now(), `value` = '") + m_str + "' where pid = 1001 and `key` = 'height'";

    HINT cb = qry.Update(sql.c_str());

    HASSERT_THROW_MSG(cb > -1, "mysql update failed", DB_DISCONN);

    HFUN_END;
    return 0;
}

int CGetHeightCTrans::CommitRes() {
    HFUN_BEGIN;

    cout << "Content-Type: text/html \r\n\r\n";
    //"{\"errno\":9999,\"err_msg\":\"error in cgi\"}"
    stringstream ss;

    ss << "{\"errno\":0, \"err_msg\":\"OK\", \"result\": '" << m_str << "'";

    HSTR str;
    cout << str;

    HFUN_END;
    return 0;
}


CTrans* CCgi::MakeTransObj(CReqData* req) {

    return new CGetHeightCTrans(req);

}


HSTR CGetHeightCTrans::getLaikeHeight () {

    HASSERT_THROW_MSG(m_pMq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = "key=val";

    HASSERT_THROW_MSG(sendAndRecv("get_height", str), "send message queue failed", ERR_STATUS);

    return str;
}

















