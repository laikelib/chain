
#include <sodef.h>
#include <huibase.h>
#include <glog/logging.h>
#include "chainApp.h"

DECLARE_INIT() 
{	
	HFAILED_THROW(so_config->Init ());

	

	return 0;
}


DECLARE_FUN(fun1)
{

	(void)data;
	return 0;
}


DECLARE_FUN(fun2)
{
	(void)data;

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















