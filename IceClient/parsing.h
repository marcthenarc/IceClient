#pragma once

#include "typedefs.h"
#include <vector>
#include <cctype>
#include <algorithm>

class Parsing
{
public:

	static bool GrabStringEntries(char *start, std::vector<Pair<char *>>& pairs);
	static bool FindHTTPResponse(std::vector <Pair<char *>>& pairs, std::string& response);
	static bool FindEntryStarting(const char *startingWith, std::vector <Pair<char *>>& pairs, std::string& entry);
	static void FindEntryWithColon(const char *withcolon, std::vector <Pair<char *>>& pairs, int&store, bool throwOnAbsent);
	static void FindEntryWithColon(const char *withcolon, std::vector <Pair<char *>>& pairs, std::string& store, bool throwOnAbsent);

	// A simple trim 
	static inline std::string trim(const std::string &s)
	{
		auto wsfront = std::find_if_not(s.begin(), s.end(), [](int c) {return std::isspace(c); });
		return std::string(wsfront, std::find_if_not(s.rbegin(), std::string::const_reverse_iterator(wsfront), [](int c) {return std::isspace(c); }).base());
	}
};


