//Original source code found at : http://stackoverflow.com/questions/1636333/download-file-using-libcurl-in-c-c

#if CCR_FORCE_LLVM_INTERPRETER
#error "Clang/LLVM interpreter does not support native to script callback function yet. Consider using CPP built-in compiler or turn on 'Use JIT execution' compiler option in app options menu."
#endif

#include <stdio.h>
#include <curl/curl.h>
#ifdef __CCR__
#include <ccr.h>
#endif

#define REMOTE_URL "https://www.example.com"
#define LOCAL_FILE_NAME "example.txt"

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

int progress_callback(void *clientp, double dltotal, double dlnow, double ultotal, double ulnow)
{
#ifdef __CCR__
	ccrClearTextOutput();
#endif
	printf("Total of file : %6.0f B | Downloaded :  %6.0f B\n", dltotal, dlnow);
	return 0;
}

int main()
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	char url[512] = REMOTE_URL;

	printf("URL ( %s ) ? ", url);
	scanf("%s", url);

	char outfilename[FILENAME_MAX] = LOCAL_FILE_NAME;

	printf("File name ( %s ) ? ", outfilename);
	scanf("%s", outfilename);

	curl = curl_easy_init();
	if (curl)
	{
		fp = fopen(outfilename, "wb");

		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
		curl_easy_setopt(curl, CURLOPT_PROGRESSFUNCTION, progress_callback);
		curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 0L);

		res = curl_easy_perform(curl);

		//Always cleanup.
		curl_easy_cleanup(curl);

		fclose(fp);

		if (res == CURLE_OK)
			printf("Remote resource \"%s\" fetched and saved at : \"%s\".\n", url, outfilename);
		else
			printf("Error , '%s'\n", curl_easy_strerror(res));
	}
	else
		printf("Error on curl_easy_init()\n");

	return 0;
}
