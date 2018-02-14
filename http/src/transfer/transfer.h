


#ifndef __TRANSFER_H__
#define __TRANSFER_H__

#include <CTrans.h>

#include <huibase.h>
#include <hlog.h>

#include <dbcomm.h>

using namespace HUIBASE;
using namespace HUIBASE::MYSQL;

class CReqData;

class CTransferTrans: public CTrans {
public:
    CTransferTrans(CReqData* req);

    int Commit ();

    int CommitReq ();

    int CommitWork ();

    int CommitRes ();

private:
    HSTR transfer () ;
    
private:
    HSTR m_strTo;
    HSTR m_strFo;
    HDOUBLE m_dValue;
    HDOUBLE m_dFee;

    HSTR m_strPri;
    HSTR m_strPub;

    HSTR m_str;
};




#endif 



