#include "stdafx.h"
#include "tcpwin.h"
#include <windows.h>
#include <winsock.h>
#include "parsing.h"
#include <fstream>

#define PARSE_FOR_ENTRY(x,y,z) Parsing::FindEntryWithColon(x, pairs, y, z)

TCPWin::TCPWin()
{
	WORD sockVersion;
	WSADATA wsaData;

	sockVersion = MAKEWORD(1, 1);

	// Initialize Winsock as before
	WSAStartup(sockVersion, &wsaData);
}

TCPWin::~TCPWin()
{
	WSACleanup();
}

void TCPWin::Connect(const char *host, int port)
{
	theHost = host;

	int nret;

	// Store information about the server
	LPHOSTENT hostEntry;

	hostEntry = gethostbyname(host);	// Specifying the server by its name;
																// another option: gethostbyaddr()
	if (!hostEntry)
		throw TCPSocketException("gethostbyname()");

	// Create the socket
	theSocket = socket(AF_INET,			// Go over TCP/IP
		SOCK_STREAM,			// This is a stream-oriented socket
		IPPROTO_TCP);		// Use TCP rather than UDP

	if (theSocket == INVALID_SOCKET)
		throw TCPSocketException("socket()");

	// Fill a SOCKADDR_IN struct with address information

	SOCKADDR_IN serverInfo;
	serverInfo.sin_family = AF_INET;

	// At this point, we've successfully retrieved vital information about the server,
	// including its hostname, aliases, and IP addresses.  Wait; how could a single
	// computer have multiple addresses, and exactly what is the following line doing?

	// See the explanation below.
	serverInfo.sin_addr = *((LPIN_ADDR)*hostEntry->h_addr_list);
	serverInfo.sin_port = htons(port);		// Change to network-byte order and
											// insert into port field

											// Connect to the server
	nret = connect(theSocket,
		(LPSOCKADDR)& serverInfo,
		sizeof(struct sockaddr));

	if (nret == SOCKET_ERROR)
		throw TCPSocketException("connect()");

	// Successfully connected!
}

void TCPWin::Request(const char *get)
{
	// Send
	std::ostringstream oss;

	oss << "GET /" << get << " HTTP/1.1\r\n"
	<< "Host: " << theHost << "\r\n"
	<< "User-Agent: WinampMPEG/5.0\r\n"
	<< "Accept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,*/*;q=0.5\r\n"
	<< "Accept-Language: en-US\r\n"
	<< "Accept-Encoding: gzip, deflate\r\n"
	<< "Icy-MetaData:1\r\n"
	<< "Connection: keep-alive\r\n\r\n";

	std::string str = oss.str();

	if (send(theSocket, str.c_str(), (int)str.size(), 0) == SOCKET_ERROR)
		throw TCPSocketException("send()");

	int len = recv(theSocket, theBuffer, 5000, 0);

	if (len == SOCKET_ERROR)
		throw TCPSocketException("recv()");

	theBuffer[len] = 0;
}

void TCPWin::GetHeaders(HttpHeader& http, IcyHeader& icy)
{
	if (theBuffer)
	{
		std::vector<Pair<char *>> pairs;

		if (!Parsing::GrabStringEntries(theBuffer, pairs))
			throw std::string("Incorrect or incomplete header format");

		// Check for a "200 OK response from the server"
		std::string response;

		if (!Parsing::FindHTTPResponse(pairs, response))
			throw("No HTTP response in header.");

		if (response.find("200 OK") == std::string::npos)
			throw (std::string("Request denied.  Response was : ") + response);

		PARSE_FOR_ENTRY("Content-Type", http.ContentType, true);
		PARSE_FOR_ENTRY("Server", http.Server, true);
		PARSE_FOR_ENTRY("Cache-Control", http.CacheControl, true);
		PARSE_FOR_ENTRY("Pragma", http.Pragma, false);
		PARSE_FOR_ENTRY("Expires", http.Expires, false);

		PARSE_FOR_ENTRY("icy-br", icy.br, true);
		PARSE_FOR_ENTRY("icy-genre", icy.genre, true);
		PARSE_FOR_ENTRY("icy-name", icy.name, true);
		PARSE_FOR_ENTRY("icy-pub", icy.pub, true);
		PARSE_FOR_ENTRY("icy-url", icy.url, true);
		PARSE_FOR_ENTRY("icy-metaint", icy.metaint, false);

		std::string notice;

		while (Parsing::FindEntryStarting("icy-notice", pairs, notice))
		{
			// We don't really care about the numbering of the notices.
			// Just append them.
			size_t pos = notice.find(":");

			if (pos != std::string::npos)
				icy.notices.push_back(Parsing::trim(notice.substr(pos + 1)));
		}
	}
}

void TCPWin::GetBuffer(char **buf, int& size)
{
	size = recv(theSocket, theBuffer, 5000, 0);

	if (size == SOCKET_ERROR)
		throw TCPSocketException("recv()");

	*buf = theBuffer;
}

void TCPWin::Close()
{
	if (theSocket != INVALID_SOCKET)
	{
		closesocket(theSocket);
		theSocket = INVALID_SOCKET;
	}
}