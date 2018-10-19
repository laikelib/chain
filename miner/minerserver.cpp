
#include <hlog.h>
#include "minerserver.h"
#include <hipcdata.h>

#include <http/httphelper.h>
#include <base64.h>
#include <hstr.h>
#include "../safeMiddle/common/middleConfig.h"
#include <uint256.h>
#include <bignum.h>

#include <sys/sysinfo.h>

#include <json/json.h>

#include <serialize.h>

#include <hcrypto.h>

using namespace HUIBASE::HTTP;

using namespace HUIBASE::CRYPTO;

HINT CMinerServer::siSonRun = 0;

HINT CMinerServer::siNeedCal = 0;

CMinerServer::CMinerServer() {

}


CMinerServer::~CMinerServer() {

}


HRET CMinerServer::Init() throw (HCException) {
    HFUN_BEGIN;


    /*
      > load xml config;
      > init shminfo, mq;
      > run
          test: call
     */

    HNOTOK_RETURN(setupParam());

    HNOTOK_RETURN(initThreads());

    HFUN_END;

    HRETURN_OK;
}


HRET CMinerServer::RunServer() throw (HCException) {

    LOG_NORMAL("minerserver running...");

    HNOTOK_RETURN(getBlockTemplate());

    HNOTOK_RETURN(putIntoBlock());

    needCal();

    waitForFinished();

    HNOTOK_RETURN(postWork());

    //sleep(2);

    HRETURN_OK;
}


HRET CMinerServer::SonMiner(HUINT tar) {

    HFUN_BEGIN;

    static constexpr HULL MAX_NONCE = 0xffffffff;
    HULL index = tar & 0xff;
    HULL count = (tar >> 8) & 0xff;
    HULL width = MAX_NONCE / count;
    HULL max_nonce = width * (index + 1);

    LOG_NORMAL("thread target: count[%d], index[%d], max_nonce: [%llu]",
               count, index, max_nonce);

    while (not m_bStop) {

        LOG_NORMAL("thread [%d] wait need condition", index);

        waitForNeed();

        LOG_NORMAL("thread [%d] begin cal nonce", index);

        uint256 hashTarget = 0;
        HUINT nNonce = (HUINT)width * index + 1;

        while (true) {

            CBlock block = GetBlock();

            if (block.GetNonce() != 0) {
                // if the nonce is not zero,
                // meant that we have found the best nonce.
                // so we need to get the newest task.

                LOG_NORMAL("block merkle[%s] best nonce has found",
                           block.GetMerkleRoot().ToString().c_str());

                break;

            }

            // set the nonce that last cal..
            block.SetNonce (++nNonce);

            if (hashTarget == 0) {
                // if hashTarget is not zero,
                // meant that we have got the target limit.
                LOG_NORMAL("cal target hash for nonce [%d]", index);
                hashTarget = CBigNum().SetCompact(block.GetBits()).getuint256();

            }

            uint256 new_hash = block.GetHash ();

            if (new_hash < hashTarget ) {

                // we find the best nonce.
                LOG_NORMAL("we find the best hash: [%s], target: [%s], nonce: [%u], index: [%d]",
                           new_hash.ToString().c_str(),
                           hashTarget.ToString().c_str(),
                           nNonce, index);

                // set the block;
                SetNewstNonce(nNonce);

                // signal
                finishedNeed();

                break;

            }

            if (nNonce > max_nonce) {

                LOG_NORMAL("thread [%d] cann't find the best nonce", index);
                break;

            }

        }

        hashTarget = 0;
        nNonce = 0;

    }

    HFUN_END;
    HRETURN_OK;
}


CBlock CMinerServer::GetBlock() {

    READLOCK<CRwlock> lock(m_lock);

    return m_block;

}


void CMinerServer::SetNewstNonce(HUINT nNonce) {

    WRITELOCK<CRwlock> lock(m_lock);

    m_block.SetNonce(nNonce);

}

HRET CMinerServer::setupParam() {

    m_strRpcIp = middle_config->GetValue("rpc_ip");

    m_iRpcPort = middle_config->GetInt ("rpc_port", 10088);

    LOG_NORMAL("rpc ip: [%s] port [%d]", m_strRpcIp.c_str(), m_iRpcPort);

    m_strAddr = middle_config->GetValue("bf_addr");

    LOG_NORMAL("benifit address: [%s]", m_strAddr.c_str());

    HRETURN_OK;

}


HRET CMinerServer::initThreads() throw (HCException) {
    HFUN_BEGIN;

    auto nprocs = get_nprocs() - 2;
    if (nprocs <= 0) {
        nprocs = 1;
    }
    LOG_NORMAL("operate system process number: [%d]", nprocs);

    SArg _arg;
    _arg.p = this;
    m_threads.resize(nprocs);

    HUINT tar = 0;

    for (int i = 0; i < nprocs; ++i) {

        tar = ((HUINT) nprocs) << 8;

        CThread& thread = m_threads[i];

        tar += i;
        _arg.tar = tar;

        HASSERT_THROW_MSG(HIS_OK(thread.Create(threadMiner, (void*)&_arg)), "create miner thread failed", SYS_FAILED);

        waitChildRun();

    }

    HFUN_END;
    HRETURN_OK;
}


HRET CMinerServer::getBlockTemplate() throw (HCException) {
    HFUN_BEGIN;

    HSTR strUrl = HCStr::Format("http://%s:%d/blocktemplate",
                                m_strRpcIp.c_str(), m_iRpcPort);

    //LOG_NORMAL("postwork url: [%s]", strUrl.c_str());

    HSTR strData = HCStr::Format("addr=%s", m_strAddr.c_str());
    LOG_NORMAL("getblock request: [%s]", strData.c_str());

    HRET cb = HERR_NO(OK);

    HSTR call_res;

    do {

        LOG_NORMAL("call [%s]", strUrl.c_str());

        cb = HttpPost(strUrl, strData, call_res);

    } while (HNOT_OK(cb));

    LOG_NORMAL("http blocktemplate return: [%s]", call_res.c_str());

    Json::Reader reader;
    Json::Value root;
    HASSERT_THROW_MSG(reader.parse(call_res, root), "json parse blocktemplate return FAILED", SRC_FAIL);

    Json::FastWriter fw;
    m_strBlockTemp = fw.write(root["result"]);

    LOG_NORMAL("block json: %s", m_strBlockTemp.c_str());

    HFUN_END;
    HRETURN_OK;
}



HRET CMinerServer::putIntoBlock() throw (HCException) {
    HFUN_BEGIN;

    LOG_NORMAL("mq return: [%s]", m_strBlockTemp.c_str());

    m_block.FromJson(m_strBlockTemp);

    LOG_NORMAL("new block json: [%s]", m_block.ToJsonString().c_str());

    LOG_NORMAL("new block hash: [%s]", m_block.GetHash().ToString().c_str());

    HFUN_END;
    HRETURN_OK;

}


HRET CMinerServer::calBlock() throw (HCException) {
    HFUN_BEGIN;

    uint256 hashTarget = CBigNum().SetCompact(m_block.GetBits()).getuint256();

    uint256 hash = 0;

    LOG_NORMAL("target hash: [%s]", hashTarget.ToString().c_str());

    do {

	hash = m_block.TryHash ();

	//LOG_NORMAL("try [%d] hash: [%s]", m_block.GetNonce(), m_block.GetHash().ToString().c_str());

    } while (hash > hashTarget);

    LOG_NORMAL("found a solution, nonce: [%d], hash: [%s], target: [%s]", m_block.GetNonce(), hash.ToString().c_str(), hashTarget.ToString().c_str());

    LOG_NORMAL("now block string: %s", m_block.ToJsonString().c_str());

    HFUN_END;
    HRETURN_OK;
}



HRET CMinerServer::postWork() throw (HCException) {
    HFUN_BEGIN;

    using namespace HUIBASE::CRYPTO;

    HSTR strUrl = HCStr::Format("http://%s:%d/postwork",
                                m_strRpcIp.c_str(), m_iRpcPort);

    CDataStream ds(SER_NETWORK, NODE_VERSION);
    ds << m_block;

    HSTR strWork;
    HEncode(ds.str(), strWork);

    HSTR strData = HCStr::Format("work=%s", strWork.c_str());
    LOG_NORMAL("post work request: [%s]", strData.c_str());

    HRET cb = HERR_NO(OK);

    HSTR call_res;

    do {

        LOG_NORMAL("call [%s]", strUrl.c_str());

        cb = HttpPost(strUrl, strData, call_res);

    } while (HNOT_OK(cb));

    LOG_NORMAL("http postwork return: [%s]", call_res.c_str());

    Json::Reader reader;
    Json::Value root;
    HASSERT_THROW_MSG(reader.parse(call_res, root), "json parse post work return FAILED", SRC_FAIL);

    Json::FastWriter fw;
    m_strBlockTemp = fw.write(root["result"]);

    // TODO: we need to check the postwork return string.
    LOG_NORMAL("post work: [%s]", m_strBlockTemp.c_str());

    HFUN_END;
    HRETURN_OK;

}

void* CMinerServer::threadMiner(void* arg) {

    HFUN_BEGIN;

    SArg *p = (SArg*)arg;

    p->p->childRun();

    p->p->SonMiner(p->tar);

    HFUN_END;

    return nullptr;
}


void CMinerServer::waitChildRun() {
    HFUN_BEGIN;

    HFAILED_MSG(m_cl_task.Lock(), "wait child run lock failed");

    while (siSonRun == 0) {

        HFAILED_MSG(m_cl_task.Wait(), "wait child run wait condition failed");

    }

    siSonRun = 0;

    HFAILED_MSG(m_cl_task.UnLock(), "wait child run wait unlock failed");

    HFUN_END;
}

void CMinerServer::childRun() {
    HFUN_BEGIN;

    HFAILED_MSG(m_cl_task.Lock(), "child run lock failed");

    siSonRun = 1;

    HFAILED_MSG(m_cl_task.Broatcast(), "child run signal condition failed");


    HFAILED_MSG(m_cl_task.UnLock (), "child run unlock failed");

    HFUN_END;
}


void CMinerServer::waitForNeed() {

    HFAILED_MSG(m_cl_result.Lock(), "wait for need lock failed");

    while (siNeedCal == 0) {

        HFAILED_MSG(m_cl_result.Wait(), "wait for need wait condition failed");

    }

    HFAILED_MSG(m_cl_result.UnLock(), "wait for need unlock failed");

}


void CMinerServer::finishedNeed() {

    HFAILED_MSG(m_cl_result.Lock(), "finished need lock failed");

    siNeedCal = 0;

    HFAILED_MSG(m_cl_result.Broatcast(), "finished need signal condition failed");

    HFAILED_MSG(m_cl_result.UnLock(), "finished need unlock failed");
}

void CMinerServer::waitForFinished() {

    HFAILED_MSG(m_cl_result.Lock(), "wait for finished lock failed");

    while (siNeedCal != 0) {

        HFAILED_MSG(m_cl_result.Wait(), "wait for finished wait condition failed");

    }

    HFAILED_MSG(m_cl_result.UnLock(), "wait for finished unlock failed");

}

void CMinerServer::needCal() {

    HFAILED_MSG(m_cl_result.Lock(), "need cal lock failed");

    siNeedCal = 1;

    HFAILED_MSG(m_cl_result.Broatcast(), "need cal signal condition failed");

    HFAILED_MSG(m_cl_result.UnLock(), "need cal unlock failed");
}
