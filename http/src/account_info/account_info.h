


#ifndef __ACCOUNT_INFO_H__
#define __ACCOUNT_INFO_H__

#include <CTrans.h>

#include <huibase.h>
#include <hlog.h>

#include <dbcomm.h>

using namespace HUIBASE;
using namespace HUIBASE::MYSQL;

class CReqData;
class CAccountInfoTrans: public CTrans {
public:
    CAccountInfoTrans(CReqData* req);

    int Commit ();

    int CommitReq ();

    int CommitWork ();

    int CommitRes ();

private:
    HSTR getAccountInfo ();
    
private:
    HSTR m_str;
};




#endif 



