
#include <sodef.h>
#include <huibase.h>
#include <glog/logging.h>
#include "chainApp.h"

#include "blocktemp.h"
#include "postwork.h"
#include "newaccount.h"

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


DECLARE_FUN(new_account)
{

    DeclareService<CNewAccountService>(data, "new_account");

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


DECLARE_FUN(query_account)
{

    DeclareService<CQueryAccount>(data, "query_account");

    return 0;
}

DECLARE_FUN(get_txs)
{

    DeclareService<CGetTxs>(data, "get_txs");

    return 0;
}


DECLARE_UNINIT() 
{
    int fd = open("abc", O_CREAT | O_RDWR, 0644);

    ssize_t cb = write(fd, "end...", 6);

    (void)cb;

    close(fd);


    (void)so_config->Shutdown();

}















