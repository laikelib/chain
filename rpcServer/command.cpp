
#include <hstr.h>
#include <hlog.h>
#include <hmq.h>
#include <hmutex.h>
#include "command.h"

#include <hipcdata.h>

#include "rpcapp.h"

using namespace HUIBASE;

HRET CCommand::sendAndRecv(HCSTRR strCmd, HSTRR str) throw (HCException) {

    static constexpr HINT CLEN = 102400;
    static char rbuf[CLEN] = {0};
    memset(rbuf, 0, CLEN);

    std::vector<HSTR> ps;
    ps.push_back(strCmd);
    ps.push_back(str);

    HUINT len = 0;
    HPSZ buf = HCIpcData::MakeBuf (ps, len);
    if (buf == nullptr) {
        LOG_ES("HCIpcData::MakeBuf make a null pointer");
        HRETURN(ILL_PT);
    }

    //LOG_NORMAL("call mq buf length: [%d]", len);

    HCMq::CMsgControl control(1, 0, len, time(nullptr));
    control.SetType (1);
    control.SetRType(899);

    bool bfailed = false;
    do {

        //LOG_NORMAL("send contro: {type:[%d], nid: [%d], bid: [%d], len: [%d]}", control.GetType(), control.GetNid(), control.GetBid(), control.GetLen());

        MUTEXHOLDER holder(m_pmq->mutex);

        HRET cb = m_pmq->mq.Send(control, buf, 0);

        HIF_NOTOK(cb) {
            LOG_ES("send message queue failed");
            bfailed = true;
            break;
        }

        control.SetType(899);
        control.SetNid(1);
        control.SetLen(CLEN);
        control.SetCTime(time(nullptr));
        cb = m_pmq->mq.Recv(control, rbuf, 15, 0);

        //LOG_NORMAL("return control: {type: [%d], nid: [%d], bid: [%d], len: [%d]}", control.GetType(), control.GetNid(), control.GetBid(), control.GetLen());

        HIF_NOTOK(cb) {

            LOG_ES("recv message queue failed");
            bfailed = true;
            break;

        }


        std::vector<HCIpcData> req_datas;
        HCIpcData::ParseDatas(rbuf, control.GetLen(), req_datas);
        //LOG_NORMAL("req_datas size: [%d]", req_datas.size());

        const HCIpcData& res = req_datas[0];

        char ss[102400] = {0};
        memcpy(ss, rbuf, control.GetLen());

        str = res.GetString();

        //LOG_NORMAL("mq return: [%s]", str.c_str());

    } while (0);

    free(buf);

    if(bfailed) {

        HRETURN(ERR_STATUS);

    }

    //HFUN_END;
    HRETURN_OK;

}


HSTR CCommand::GetRes() const throw (HCException) {

    stringstream ss;
    ss << "\"" << m_res <<"\"";

    return ss.str();
}


HPS CCommand::parseInput(HCSTRR str) const {

    HPS res;

    HVSTRS vs;
    HINT cb = HCStr::Split (str, "&", vs);

    HIF_NOTOK(cb) {

        return res;

    }

    for (size_t i = 0; i < vs.size(); ++i) {

        HSTRR ss = vs[i];
        HCStr::Trim (ss);
        HVSTRS ii;

        cb = HCStr::Split(ss, "=", ii);

        HIF_NOTOK(cb) {

            continue;

        }

        if(ii.size() < 2) {
            continue;
        }

        HSTRR skey = ii[0];
        HCStr::Trim(skey);

        HSTRR sval = ii[1];
        HCStr::Trim(sval);

        res[skey] = sval;

    }

    return res;

}



CCommand* CCommandFactory::newCommand(HCSTRR strName){

	std::map<HSTR, command_maker>::iterator fit = m_makers.find (strName);
	if (fit == m_makers.end()) {
		return nullptr;
	}

	command_maker pfun = fit->second;

	if (pfun != nullptr) {
		return pfun();
	}

	return nullptr;

}


void CCommandFactory::RegisteCommand (HCSTRR strName, command_maker maker) {

	if (m_makers.find(strName) != m_makers.end()) {
		//cout << "CCommandFactory::RegisteCommand command name [" + strName
		//	+ "] hash registered!";
		exit(-1);
	}

	m_makers.insert(std::map<HSTR, command_maker>::value_type(strName, maker) );

}
