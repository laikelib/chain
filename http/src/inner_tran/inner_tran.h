


#ifndef __INNER_TRAN_H__
#define __INNER_TRAN_H__

#include <CTrans.h>

#include <huibase.h>
#include <hlog.h>

#include <dbcomm.h>

using namespace HUIBASE;
using namespace HUIBASE::MYSQL;

class CReqData;

class CInnerTranCTrans: public CTrans {
public:
    CInnerTranCTrans(CReqData* req);

    int Commit ();

    int CommitReq ();

    int CommitWork ();

    int CommitRes ();

private:
    HSTR innerTran () ;
    
private:
    HSTR m_strFo;
    HSTR m_strTo;
    HDOUBLE m_dValue;
    HDOUBLE m_dFee;

    HSTR m_str;
};




#endif 



