

#include "newaccount.h"
#include <hlog.h>
#include <json/json.h>

HRET CNewAccountCgi::Work() throw (HCException) {

    HFUN_BEGIN;

    HASSERT_THROW_MSG(m_pmq != nullptr, "message qeueu is not initialize", ILL_PT);

    HSTR str = "key=val";

    HFAILED_MSG(sendAndRecv("new_account", str), "get height sendAndRecv FAILED");

    LOG_NORMAL("blockchain newaccount return: [%s]", str.c_str());

    m_res = str;

    /*
    Json::Reader reader;
    Json::Value root;

    HASSERT_THROW_MSG(reader.parse(m_res, root), "parse json failed", ERR_STATUS);

    HASSERT_THROW_MSG(root["ret_code"] == "0", "block chain newaccount return error", ERR_STATUS);

    m_res = root["result"].asString();*/

    HFUN_END;
    HRETURN_OK;

}



REGISTE_COMMAND(newaccount, CNewAccountCgi);
