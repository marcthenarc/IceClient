#include "stdafx.h"
#include "typedefs.h"

std::ostream & operator << (std::ostream &os, HttpHeader& h)
{
	os << "Cache-Control: " << h.CacheControl
		<< "\r\nContentType: " << h.ContentType
		<< "\r\nExpires: " << h.Expires
		<< "\r\nPragma: " << h.Pragma
		<< "\r\nServer: " << h.Server
		<< "\r\n";

	return os;
}

std::ostream & operator << (std::ostream &os, IcyHeader& i)
{
	os << "br: " << i.br
		<< "\r\ngenre: " << i.genre
		<< "\r\nmetaint: " << i.metaint
		<< "\r\nname: " << i.name
		<< "\r\npub: " << i.pub
		<< "\r\nurl: " << i.url;

	int z = 1;

	for (auto &n : i.notices)
	{
		os << "\r\nnotice" << z++ << ": " << n;
	}

	os << "\r\n";

	return os;
}