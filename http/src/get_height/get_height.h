


#ifndef __GETHEIGHT_H__
#define __GETHEIGHT_H__

#include <CTrans.h>

#include <huibase.h>
#include <hlog.h>

#include <dbcomm.h>

using namespace HUIBASE;
using namespace HUIBASE::MYSQL;

class CReqData;
class CGetHeightCTrans: public CTrans {
public:
    CGetHeightCTrans(CReqData* req);

    int Commit ();

    int CommitReq ();

    int CommitWork ();

    int CommitRes ();

private:
    HSTR getLaikeHeight () ;
    
private:
    HSTR m_str;
};




#endif 



