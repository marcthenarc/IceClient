#pragma once

#include <winsock.h>
#include <sstream>
#include "typedefs.h"

class TCPSocketException
{
	std::string errMsg;

public:

	TCPSocketException(char *whichFunc)
	{ 
		int err = WSAGetLastError();

		std::ostringstream oss;
		oss << "Call to " << whichFunc << "returned error " << err << ".";
		errMsg = oss.str();
	}

	std::string &GetErrorMessage() { return errMsg; };
};

class TCPWin
{
	SOCKET theSocket;
	char theBuffer[5001];
	const char *theHost;

public:

	TCPWin();
	virtual ~TCPWin();
	void Connect(const char *host, int port);
	void Request(const char *get);
	void GetHeaders(HttpHeader& http, IcyHeader& icy);
	void Close();
	void GetBuffer(char **buf, int& size);
};