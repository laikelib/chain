
#ifndef __MIDDLESOCKET_H__
#define __MIDDLESOCKET_H__

#include <huibase.h>
#include <hsocket.h>

using namespace HUIBASE;


class CMiddleSocket : public HCTcpSocket {
public:
	enum SOCK_RET {
		SR_OK,
		SR_CLOSE,
		SR_ERROR,
	};

public:
	CMiddleSocket() ;

public:
	HTIME GetAccessTime () const { return m_access; }
	void SetAccessTime (HTIME t) { m_access = t; }

	SOCK_RET ReadRequest ();

	SOCK_RET SendResponse ();

	void SetResponse (HCPSZ sz, HSIZE len);

	void SetResponse(HCSTRR str);

	HCPSZ GetData () { return m_data; }

private:
	static const HUINT MIDDLE_SOCKET_LEN = 4096;
	HTIME m_access {0};

	HUINT m_len{0};
	HCHAR m_data[MIDDLE_SOCKET_LEN]{0};

};

#endif //__MIDDLESOCKET_H__


