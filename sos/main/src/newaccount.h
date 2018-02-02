

#ifndef __NEWACCOUNT_H__
#define __NEWACCOUNT_H__

#include "blocktemp.h"

class CNewAccountService : public CBlockTempService {
public:
    CNewAccountService(HCSTRR strName)
	: CBlockTempService(strName) { }

    virtual int Excute () throw (HCException);

};

class CGetHeightService : public CBlockTempService {
public:
    CGetHeightService (HCSTRR strName)
	: CBlockTempService(strName) { }


    virtual int Excute () throw (HCException);

}; 

#endif //__NEWACCOUNT_H__
