

#ifndef __NEWACCOUNT_H__
#define __NEWACCOUNT_H__

#include "blocktemp.h"

class CGetHeightService : public CBlockTempService {
public:
    CGetHeightService (HCSTRR strName)
	: CBlockTempService(strName) { }


    virtual int Excute () throw (HCException);

}; 



class CLkInfoService : public CSoServiceBase {
 public:
    CLkInfoService (HCSTRR strName)
        : CSoServiceBase (strName) { }

    int ParseInput (SCM_DATA* data) throw (HCException) { (void)data;  return 0; }

    int Excute () throw (HCException);

    int PackReturn (SCM_DATA* data) throw (HCException);

 private:
    HSTR m_str;
};




#endif //__NEWACCOUNT_H__
