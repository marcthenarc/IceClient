#include "stdafx.h"
#include "parsing.h"

// Takes a block of text and parses it to identify all SOLs and EOLs and pairs them until a 0xd 0xa 0xd 0xa sequence is found.
// Returns false if the sequence is not found.
bool Parsing::GrabStringEntries(char *start, std::vector <Pair<char *>>& pairs)
{
	while (1)
	{
		char *end = strchr(start, '\n') + 1;

		if (!end)
			return false;

		pairs.push_back({ start, end });

		// The last statement should end by 0xd 0xa 0xd 0xa
		if (end - start == 2)
			break;

		// New start.
		start = end;
	}

	return true;
}

// Within a group a SOL and EOL pairs, find a given string entry starting which a matching string.
// Erase items as they are found.
// Return true if we found a match, false if not.
bool Parsing::FindEntryStarting(const char *startingWith, std::vector <Pair<char *>>& pairs, std::string &entry)
{
	auto it = pairs.begin(), end = pairs.end();

	for (; it != end; it++)
	{
		if (std::string(it->left, it->right - it->left).find(startingWith) == 0)
		{
			entry = std::string(it->left, it->right - it->left);
			pairs.erase(it);
			return true;
		}
	}

	return false;
}

// Takes a block of text and parses it to identify all SOLs and EOLs and pairs them until a 0xd 0xa 0xd 0xa sequence is found.
// Returns false if the sequence is not found.
bool Parsing::FindHTTPResponse(std::vector <Pair<char *>>& pairs, std::string& response)
{
	auto it = pairs.begin(), end = pairs.end();

	for (; it != end; it++)
	{
		if (std::string(it->left, it->right - it->left).find("HTTP") == 0)
		{
			response = std::string(it->left, it->right - it->left);
			pairs.erase(it);
			return true;
		}
	}

	return false;
}

// Within a group a SOL and EOL pairs, find a given string entry starting
// which a matching string that has a colon at the end and keep the right part into the store.
// Erase items as they are found.
// Return true if a value is stored.
void Parsing::FindEntryWithColon(const char *withcolon, std::vector <Pair<char *>>& pairs, int& store, bool throwOnAbsent)
{
	auto it = pairs.begin(), end = pairs.end();

	for (; it != end; it++)
	{
		if (std::string(it->left, it->right - it->left).find(withcolon) == 0)
		{
			char* start = it->left + strlen(withcolon) + 1;
			store = atoi(trim(std::string(start, it->right - start)).c_str());
			pairs.erase(it);
			return;
		}
	}

	if (throwOnAbsent)
		throw std::string("Missing required entry ") + std::string(withcolon);
}

// Within a group a SOL and EOL pairs, find a given string entry starting
// which a matching string that has a colon at the end and keep the right part into the store.
// Erase items as they are found.
// Return true if a value is stored.
void Parsing::FindEntryWithColon(const char *withcolon, std::vector <Pair<char *>>& pairs, std::string& store, bool throwOnAbsent)
{
	auto it = pairs.begin(), end = pairs.end();

	for (; it != end; it++)
	{
		if (std::string(it->left, it->right - it->left).find(withcolon) == 0)
		{
			char* start = it->left + strlen(withcolon) + 1;
			store = trim(std::string(start, it->right - start));
			pairs.erase(it);
			return;
		}
	}

	if (throwOnAbsent)
		throw std::string("Missing required entry ") + std::string(withcolon);
}
