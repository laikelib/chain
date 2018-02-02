
#include <sodef.h>
#include <string>
#include <cstring>
#include <memory.h>

static int g_n = 100;

DECLARE_INIT() 
{
	g_n = 200;

	return 0;
}

//key=value&key=value
DECLARE_FUN(fun1)
{
	++g_n;

	memset(data->odata, 0, SCM_MAX_LEN);
	static const char* sz = "this is fun1";
	strcpy(data->odata, sz);

	return 0;
}

DECLARE_FUN(fun2)
{
	++g_n;

	memset(data->odata, 0, SCM_MAX_LEN);
	static const char* sz = "this is fun2";
	strcpy(data->odata, sz);

	return 0;
}




DECLARE_UNINIT() 
{
	g_n = 0;

    int fd = open("abc", O_CREAT | O_RDWR, 0644);

    ssize_t cb = write(fd, "end...", 6);

    (void)cb;

    close(fd);
}




