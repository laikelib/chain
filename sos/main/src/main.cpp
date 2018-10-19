
#include <sodef.h>
#include <huibase.h>
#include <glog/logging.h>
#include "chainApp.h"

#include "blocktemp.h"
#include "postwork.h"
#include "getheight.h"
#include "newtx.h"

#include <innerso.h>

DECLARE_INIT() 
{
    
    HFAILED_THROW(so_config->Init ());

    LOG(INFO) <<"all function: [" << g_services::Instance()->GetAllService() << "]";
    
    return 0;
}


DECLARE_FUN(block_template)
{
    DeclareService<CBlockTempService>(data,"block_template");

    return 0;
}


DECLARE_FUN(post_work)
{
    
    DeclareService<CPostWorkService>(data, "post_work");

    return 0;
}


DECLARE_FUN(get_height)
{

    DeclareService<CGetHeightService>(data, "get_height");

    return 0;
}

DECLARE_FUN(height_block)
{

    DeclareService<CHeightBlockService>(data, "height_block");

    return 0;
}


DECLARE_FUN(get_bal)
{

    DeclareService<CGetBalService>(data, "get_bal");

    return 0;

}


DECLARE_FUN(hash_tx)
{
    DeclareService<CHashTxService>(data, "hash_tx");

    return 0;
}


DECLARE_FUN(lk_info)
{

    DeclareService<CLkInfoService>(data, "lk_info");

    return 0;

}


DECLARE_FUN(new_tx)
{

    DeclareService<CNewTxService>(data, "new_tx");

    return 0;

}


DECLARE_UNINIT() 
{

    (void)so_config->Shutdown();

}















