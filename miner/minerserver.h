

#ifndef __MINERSERVER_H__
#define __MINERSERVER_H__

#include "minerapp.h"
#include "block.h"

#include <hmutex.h>
#include <hmq.h>

#include <hthread.h>

using namespace HUIBASE;

class CMinerServer {
public:
    CMinerServer ();

    ~CMinerServer();

public:
    HRET Init () throw (HCException);

    HRET RunServer () throw (HCException);

private:
    HRET setupParam ();

    HRET initMq () throw (HCException);

    HRET initThreads () throw (HCException);

    HRET getBlockTemplate () throw (HCException);

    HRET putIntoBlock () throw (HCException);

    HRET calBlock () throw (HCException);

    HRET postWork () throw (HCException);

private:
    static HRET sonMiner (CMinerServer* pserver);

private:
    HSYS_T m_mq_key {0};

    HUINT m_app_count{0};

    HUINT m_mq_bk_count{0};

    HINT m_mq_valid_time{0};

    HINT m_mq_try_time{0};

    HINT m_sleep_time{0};

    HINT m_sleep_timeu{0};

    HCMq * m_pMq = nullptr;

    HSTR m_strAddr;

    HSTR m_strBlockTemp;

    CBlock m_block;

    std::vector<HCThread> m_threads;

    HINT m_nNewNonce = {0};
    uint256 m_newHash = {0};

    CConLock m_cl_task;
    CConLock m_cl_result;


};

#endif //__MINERSERVER_H__

