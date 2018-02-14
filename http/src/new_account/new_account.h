


#ifndef __NEWACCOUNT_H__
#define __NEWACCOUNT_H__

#include <CTrans.h>

#include <huibase.h>
#include <hlog.h>

#include <dbcomm.h>

using namespace HUIBASE;
using namespace HUIBASE::MYSQL;

class CReqData;
class CNewAccountCTrans: public CTrans {
public:
    CNewAccountCTrans(CReqData* req);

    int Commit ();

    int CommitReq ();

    int CommitWork ();

    int CommitRes ();

private:
    HSTR getNewAccount () ;
    
private:
    HSTR m_str;
};




#endif 



