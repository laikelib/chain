

#ifndef __MQOBJ_H__
#define __MQOBJ_H__


#include "block.h"

#include <hmq.h>


using namespace HUIBASE;

class CMqObj {
public:
    CMqObj ();

    ~CMqObj();

public:
    HRET Init () throw (HCException);

    HRET sendAndRecv (HCSTRR strCmd, HSTRR str) throw (HCException);

    void SetRKey (HINT nKey) { m_rkey = nKey; }

private:
    HRET setupParam ();
    
    HRET initMq () throw (HCException);

private:
    HSYS_T m_mq_key {0};

    HUINT m_app_count{0};

    HUINT m_mq_bk_count{0};

    HINT m_mq_valid_time{0};

    HINT m_mq_try_time{0};

    HINT m_sleep_time{0};

    HINT m_sleep_timeu{0};

protected:
    HINT m_rkey = 0;
    HCMq * m_pMq = nullptr;

};

#endif //__MINERSERVER_H__

