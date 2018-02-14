


#ifndef __HEIGHT_BLOCK_H__
#define __HEIGHT_BLOCK_H__

#include <CTrans.h>

#include <huibase.h>
#include <hlog.h>

#include <dbcomm.h>

using namespace HUIBASE;
using namespace HUIBASE::MYSQL;

class CReqData;
class CHeightBlockTrans: public CTrans {
public:
    CHeightBlockTrans(CReqData* req);

    int Commit ();

    int CommitReq ();

    int CommitWork ();

    int CommitRes ();

private:
    HSTR getBlockHeight ();
    
private:
    HSTR m_str;
};




#endif 



