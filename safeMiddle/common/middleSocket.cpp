

#include "middleSocket.h"


CMiddleSocket::CMiddleSocket() {

}


CMiddleSocket::SOCK_RET CMiddleSocket::ReadRequest () {

	memset(m_data, 0, MIDDLE_SOCKET_LEN);
	HSYS_RET  cb = Recv (m_data, MIDDLE_SOCKET_LEN, 0);
	if (cb == 0) return SR_CLOSE;

	if (cb < 0) return SR_ERROR;

	m_len = cb;

	return SR_OK;
}

CMiddleSocket::SOCK_RET CMiddleSocket::SendResponse () {

	auto  cb = Send (m_data, m_len, 0);
	if (cb < 0) return SR_ERROR;

	return SR_OK;
}

void CMiddleSocket::SetResponse (HCPSZ sz, HSIZE len) {

	memset(m_data, 0, MIDDLE_SOCKET_LEN);
	m_len = len;
	memcpy (m_data, sz, len);
	
}


void CMiddleSocket::SetResponse(HCSTRR str) {

	assert(str.length() < MIDDLE_SOCKET_LEN);

	memset(m_data, 0, MIDDLE_SOCKET_LEN);

	m_len = str.length() + 1;

	memcpy(m_data, str.c_str(), m_len);

}



