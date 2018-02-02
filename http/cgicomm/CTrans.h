#ifndef __CTRANS_H__
#define __CTRANS_H__

#include "CReqData.h"
#include <huibase.h>
#include <dbconn.h>
#include "mqobj.h"

#define SESSION_UID "SESSION_UID"
#define SESSION_TUID "SESSION_TUID"
#define SESSION_PHONE "SESSION_PHONE"


using namespace HUIBASE;
using namespace HUIBASE::MYSQL;


#define BIT_RET_FACTORY 100000000

typedef map<HSTR, CMysqlConnection*> CONNMAP;

class CReqData;

class CTrans : public CMqObj
{
public:
    CTrans(CReqData* pReqData);

    virtual int StartWork();
    void InitMysql();
    virtual void InitSession();
    
    virtual ~ CTrans();

    CMysqlConnection* GetConnection (HCSTRR strName = "");

    void ThrowWebError( const string & sErrMsg );

  protected:
    virtual int Commit() = 0;

    virtual void CheckReqVersion ();
    
    virtual void CheckIPLimit();

    void CheckParaValid (HSTRR str);

private:
    void initConnections ();

protected:    
    
    CReqData* m_pReqData;

    CMysqlConnection m_conn;

    CONNMAP m_mapConn;
    
};


 
#endif



