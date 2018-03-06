

#ifndef __MINERSERVER_H__
#define __MINERSERVER_H__

#include "minerapp.h"
#include "block.h"


#include <hmutex.h>

#include <hmutex.h>
#include <thread.h>
#include <hconlock.h>

using namespace HUIBASE;

class CMinerServer {
public:
    typedef struct {
        CMinerServer* p;
        HUINT tar;
    } SArg;
public:
    CMinerServer ();

    ~CMinerServer();

public:
    HRET Init () throw (HCException);

    HRET RunServer () throw (HCException);

    HRET SonMiner (HUINT tar);

    CBlock GetBlock ();

    void SetNewstNonce (HUINT nNonce);

private:
    HRET setupParam ();

    HRET initThreads () throw (HCException);

    HRET getBlockTemplate () throw (HCException);

    HRET putIntoBlock () throw (HCException);

    HRET calBlock () throw (HCException);

    HRET postWork () throw (HCException);

private:
    static void* threadMiner (void* arg);

    static HINT siSonRun;

    static HINT siNeedCal;

    void waitChildRun ();

    void childRun ();

    void waitForNeed ();

    void finishedNeed ();

    void waitForFinished ();

    void needCal ();

private:
    volatile bool m_bStop = false;

    HSTR m_strRpcIp;

    HINT m_iRpcPort{0};

    HINT m_nNewNonce = {0};

    uint256 m_newHash = {0};

    HINT m_nPorc = {1};

    HSTR m_strAddr;

    HSTR m_strBlockTemp;

    CBlock m_block;

    HUINT m_nNonce;

    std::vector<CThread> m_threads;

    CRwlock m_lock;

    CConLock m_cl_task;
    CConLock m_cl_result;


};

#endif //__MINERSERVER_H__

