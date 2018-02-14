
#include <CCgi.h>
#include "account_info.h"
#include <sstream>
#include <algorithm>


using namespace std;

using namespace HUIBASE;

CAccountInfoTrans::CAccountInfoTrans(CReqData* req)
    : CTrans(req){

    
}


int CAccountInfoTrans::Commit() {

    int ret = 0;

    SetRKey (103);
    
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

int CAccountInfoTrans::CommitReq() {
    HFUN_BEGIN;

    m_str = m_pReqData->GetPara("addr");
    CheckParaValid(m_str);

    HFUN_END;
    return 0;
}


int CAccountInfoTrans::CommitWork() {
    HFUN_BEGIN;

    m_str = getAccountInfo();

    LOG_NORMAL("account info return: [%s]", m_str.c_str());


    HFUN_END;
    return 0;
}

int CAccountInfoTrans::CommitRes() {
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

    return new CAccountInfoTrans(req);

}

HSTR CAccountInfoTrans::getAccountInfo() {

    HASSERT_THROW_MSG(m_pMq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = m_str;

    
    HFAILED_THROW(sendAndRecv("account_info", str));

    return str;
}

