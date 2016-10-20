// IceClient.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "tcpwin.h"
#include "typedefs.h"
#include <iostream>
#include <fstream>

char dump[50000];
char *ptr = dump;
__int64 total = 0;

int main()
{
	TCPWin tcp;

	HttpHeader http;
	IcyHeader icy;

	try
	{
		tcp.Connect("stream-tx3.radioparadise.com", 80);
		tcp.Request("mp3-192");
		tcp.GetHeaders(http, icy);

		FILE *fp = fopen("E:\\out.mp3", "wb");

		if (!fp)
			throw("Could not create mp3 output file");

		int count = 0;

		for (;;)
		{
			char *buf;
			int size;

			tcp.GetBuffer(&buf, size);

			// Anything we get, we add it to the buffer.
			memcpy(ptr, buf, size);

			// Advance poiner to buffer.
			ptr += size;

			// If we have enough data to recover the meta-info ...
			if (ptr - dump > icy.metaint + 4080)
			{
				// Dump sound
				fwrite(dump, icy.metaint, 1, fp);

				// Set pointer to meta-info at the sound block's end
				char *meta = dump + icy.metaint;

				// Take the meta-info and multiply by 16
				__int64 len = (unsigned char)*meta * 16;

				// Advance
				meta++;

				// If there is meta-data (0 is none)
				if (len)
				{
					// Take the string and advance pointer to the end of it.
					std::cout << total << ": " << std::string(meta, len) << std::endl;
					meta += len;
				}

				total += icy.metaint;

				// What is the length of the remaining data (reusing len here)
				len = ptr - meta;

				// Move remaining data to the beginning of the dump buffer.
				memmove(dump, meta, len);

				// Set the pointer at the end of the moved data.
				ptr = dump + len;
			}
		}

		tcp.Close();
	}
	catch (TCPSocketException e)
	{
		std::cerr << e.GetErrorMessage() << std::endl;
	}
	catch (const std::string &e)
	{
		std::cerr << e << std::endl;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
	}
	catch (...)
	{
		std::cerr << "Unknown exception" << std::endl;
	}

	system("pause");

	return 1;
}

