
#include <CCgi.h>
#include "height_block.h"
#include <sstream>
#include <algorithm>


using namespace std;

using namespace HUIBASE;

CHeightBlockTrans::CHeightBlockTrans(CReqData* req)
    : CTrans(req){

}


int CHeightBlockTrans::Commit() {

    int ret = 0;
    
    try {

	SetRKey (105);

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

int CHeightBlockTrans::CommitReq() {
    HFUN_BEGIN;

    m_str = m_pReqData->GetPara("hid");
    CheckParaValid(m_str);

    HASSERT_THROW_MSG(HIS_TRUE(HCStr::IsDigit(m_str)), "height must be digit", INVL_PARA);

    HFUN_END;
    return 0;
}


int CHeightBlockTrans::CommitWork() {
    HFUN_BEGIN;

    m_str = getBlockHeight();

    LOG_NORMAL("block: [%s]", m_str.c_str());


    HFUN_END;
    return 0;
}

int CHeightBlockTrans::CommitRes() {
    HFUN_BEGIN;

    cout << "Content-Type: text/html \r\n\r\n";
    //"{\"errno\":9999,\"err_msg\":\"error in cgi\"}"

    stringstream ss;
    if (m_str == "error") {

	LOG_ERROR("get block failed, return:[%s]", m_str.c_str());

	ss << "{\"errno\":505, \"err_msg\":\"error in blockchain\"}";

    } else {

	ss << "{\"errno\":0, \"err_msg\":\"OK\", \"result\": " << m_str << "}";

    }

    HSTR str = ss.str();
    cout << str;

    HFUN_END;
    return 0;
}


CTrans* CCgi::MakeTransObj(CReqData* req) {

    return new CHeightBlockTrans(req);

}

HSTR CHeightBlockTrans::getBlockHeight () {

    HASSERT_THROW_MSG(m_pMq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = m_str;
    
    HFAILED_THROW(sendAndRecv("height_block", str));

    return str;
}

