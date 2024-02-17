#pragma warning(disable:4996)
#ifndef CONNECT_CPP
#define CONNECT_CPP
#include "connect.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

int connect_to_srv(const char*url,char*out,int max_len, bool ignore_insecure) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        if(ignore_insecure==true)
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (max_len > (int)strlen(readBuffer.c_str())) {
            strcpy(out, readBuffer.c_str());
            return res;
        }
        else
            return  1; 
    }
    return 2;
}
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    FILE* file = (FILE*)userp;
    if (file) {
        fwrite(contents, 1, totalSize, file);
    }
    return totalSize;
}

int download_file_from_srv(const char* url, const char* output_file_path) {
    //use curl to download a file from a server

    CURL* curl;
    CURLcode res;
    FILE* output_file;
    char*buf=new char[55];
    curl = curl_easy_init();
    if (!curl) {
        return 1;
    }

    // Set the URL to download
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Create a file to write the downloaded data
    output_file = fopen(output_file_path, "wb");
    if (!output_file) {
        curl_easy_cleanup(curl);
        return 2;
    }

    // Set the write callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);

    // Perform the download
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        return 3;
    }
    // Cleanup and close the file
    curl_easy_cleanup(curl);
    fclose(output_file);
    if ((output_file = fopen(output_file_path, "r")) == 0) {
        return 4;
    }
    else {
        fscanf(output_file, "%50s", buf);
        if (strcmp(buf, "no_auth") == 0) {
            fclose(output_file);
            return 5;
        }
        fclose(output_file);
    }
    delete[] buf;
    return 0;
}


int call_srv(const char*server_url,const char*get_request,const char*additional) {
    CURL* curl;
	CURLcode res;
	std::string readBuffer;
    char*url=new char[1000];
    url[0] = '\0';
  
	curl = curl_easy_init();
    if (curl) {
        char* output = curl_easy_escape(curl, additional, strlen(additional));
        sprintf_s(url, 995, "%s%s%s", server_url, get_request,output);
		curl_easy_setopt(curl, CURLOPT_URL, url);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
		res = curl_easy_perform(curl);
        curl_free(output);
		curl_easy_cleanup(curl);
        delete[] url;
		return res;
	}
    delete[] url;
	return 0;
}
#endif