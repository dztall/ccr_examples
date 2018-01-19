//Original source code found at : http://curl.haxx.se/libcurl/c/sepheaders.html

#if CCR_FORCE_LLVM_INTERPRETER
#error "Clang/LLVM interpreter does not support native to script callback function yet. Consider using CPP built-in compiler or turn on 'Use JIT execution' compiler option in app options menu."
#endif

/***************************************************************************
 *                                  _   _ ____  _
 *  Project                     ___| | | |  _ \| |
 *                             / __| | | | |_) | |
 *                            | (__| |_| |  _ <| |___
 *                             \___|\___/|_| \_\_____|
 *
 * Copyright (C) 1998 - 2011, Daniel Stenberg, <daniel@haxx.se>, et al.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution. The terms
 * are also available at http://curl.haxx.se/docs/copyright.html.
 *
 * You may opt to use, copy, modify, merge, publish, distribute and/or sell
 * copies of the Software, and permit persons to whom the Software is
 * furnished to do so, under the terms of the COPYING file.
 *
 * This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
 * KIND, either express or implied.
 *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

#include <curl/curl.h>

static size_t write_data(void *ptr, size_t size, size_t nmemb, void *stream)
{
	long long written = fwrite(ptr, size, nmemb, (FILE *)stream);
	return written;
}

int main()
{
	CURL *curl;

	static const char *headerfilename = "head.out";
	static const char *bodyfilename = "body.out";

	FILE *headerfile;
	FILE *bodyfile;

	curl_global_init(CURL_GLOBAL_ALL);

	/* init the curl session */
	curl = curl_easy_init();

	/* set URL to get */
	curl_easy_setopt(curl, CURLOPT_URL, "https://example.com");

	/* no progress meter please */
	curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);

	/* send all data to this function  */
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);

	/* open the files */
	headerfile = fopen(headerfilename, "wb");
	if (headerfile == NULL)
	{
		curl_easy_cleanup(curl);
		return -1;
	}
	bodyfile = fopen(bodyfilename, "wb");
	if (bodyfile == NULL)
	{
		curl_easy_cleanup(curl);
		return -1;
	}

	/* we want the headers be written to this file handle */
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, headerfile);

	/* we want the body be written to this file handle instead of stdout */
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, bodyfile);

	/* get it! */
	CURLcode res = curl_easy_perform(curl);

	/* close the header file */
	fclose(headerfile);

	/* close the body file */
	fclose(bodyfile);

	/* cleanup curl stuff */
	curl_easy_cleanup(curl);

	if (res == CURLE_OK)
		printf("See head.out and body.out file.\n");
	else
		printf("Error , '%s'\n", curl_easy_strerror(res));

	return 0;
}
