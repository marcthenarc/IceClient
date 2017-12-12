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

const char *streamTitle = "StreamTitle";

char* binaryArraySearch(char *haystack, size_t sizeOfHaystack, const char *needle, size_t sizeOfNeeddle)
{
    for (size_t i = 0; i < sizeOfHaystack; i++)
    {
        if (haystack[i] != *needle)
            continue;

        bool pass = true;
        int keep = i;

        for (size_t j = 0; j < sizeOfNeeddle; i++, j++)
        {
            if (i >= sizeOfHaystack)
                break;

            if (haystack[i] != needle[j])
            {
                pass = false;
                break;
            }
        }

        if (pass)
            return &haystack[keep];
    }
    
    return nullptr;
}

int main()
{
	TCPWin tcp;

	HttpHeader http;
	IcyHeader icy;

	try
	{
        tcp.Connect("<Your stream>", 80);
        tcp.Request("<mountpoint>");
        tcp.GetHeaders(http, icy);

		FILE *fp = fopen("E:\\out.mp3", "wb");

		if (!fp)
			throw("Could not create mp3 output file");

        // HACK - Some streams start correctly, perfectly timed to reach the meta at length
        // metaint, but others don't.  The only way I found to correct this is to check for
        // the first "StreamTitle" message and infer the meta jump from there.
        bool checkTitle = true;

		for (;;)
		{
			char *buf;
			int size;

			tcp.GetBuffer(&buf, size);

            // Anything we get, we add it to the buffer.
            memcpy(ptr, buf, size);

            // Advance pointer to buffer.
            ptr += size;

            if (checkTitle)
            {
                // Check for the meta title in the data accumulated so far.
                char *meta = binaryArraySearch(dump, ptr - dump, streamTitle, strlen(streamTitle));

                if (!meta)
                    continue;

                // Print out the title: this will be our stating point.
                std::cout << 0 << ": " << meta << std::endl;

                // Get the meta length from the previous character and advance.
                size_t len = (unsigned char)*(meta - 1) * 16;
                meta += len;
                
                // Move remaining of data to the beginning of the dump buffer.
                len = (size_t)(ptr - meta);
                memmove(dump, meta, len);

                // Set the pointer at the end of the moved data.
                ptr = dump + len;

                // End of check.
                checkTitle = false;
            }

            // If we have enough data to recover the meta-info ...
            if (ptr - dump > icy.metaint + 4096)
            {
                // Dump sound
                fwrite(dump, icy.metaint, 1, fp);

                // Set pointer to meta-info at the sound block's end
                char *meta = dump + icy.metaint;

                // Take the meta-info and multiply by 16
                size_t len = (unsigned char)*meta * 16;

                // Advance
                meta++;

                // If there is meta-data (0 is none)
                if (len)
                {
                    // Take the string and advance pointer to the end of it.
                    std::cout << total << ": " << meta << std::endl;
                    meta += len;
                }

                // Keep the total.  This can be used to approximate TOC for your dump.
                total += icy.metaint;

                // What is the length of the remaining data ?
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

