#pragma once

#include <string>
#include <vector>

typedef struct HttpHeader_t
{
	std::string ContentType;
	std::string Server;
	std::string CacheControl;
	std::string Pragma;
	std::string Expires;
} HttpHeader;

typedef struct IcyHeader_t
{
	int br;
	std::string genre;
	std::string name;
	std::vector<std::string> notices;
	int pub;
	std::string url;
	int metaint;
} IcyHeader;

template <class T>
class Pair
{
public:

	T left, right;

	Pair(T l, T r) : left(l), right(r) { }
};

std::ostream & operator << (std::ostream &os, HttpHeader& http);
std::ostream & operator << (std::ostream &os, IcyHeader& http);