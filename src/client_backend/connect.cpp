#pragma warning(disable:4996)
#ifndef CONNECT_CPP
#define CONNECT_CPP
#include "connect.h"
#include "well_known.h"
#include "security.h"


int fast_send(const char * url,bool ignore_insecure) {
    //send get rewuest to server, and cloe connection after maximum 1 second
    CURL* curl;
    CURLcode res;
    curl = curl_easy_init();
    if (curl) {
		curl_easy_setopt(curl, CURLOPT_URL, url);
		if(ignore_insecure==true)
			curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
		curl_easy_setopt(curl, CURLOPT_TIMEOUT, 1L);
		res = curl_easy_perform(curl);
		curl_easy_cleanup(curl);
        if (res == CURLE_OK) {
			return 0;
		}
        else {
			return res;
		}
	}
    return 1;
}
static size_t write_callback_connect(void* contents, size_t size, size_t nmemb, void* userp)
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
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_connect);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        if(ignore_insecure==true)
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (max_len > (int)strlen(readBuffer.c_str())) {
            strcpy(out, readBuffer.c_str());
            return 0;
        }
        else
            return  1; 
    }
    return 2;
}
size_t write_callback_download(void* contents, size_t size, size_t nmemb, void* userp) {
    size_t totalSize = size * nmemb;
    FILE* file = (FILE*)userp;
    if (file) {
        fwrite(contents, 1, totalSize, file);
    }
    return totalSize;
}

int download_file_from_srv(const char* url, const char* output_file_path, bool ignore_insecure) {
    //use curl to download a file from a server
    char*temp_path = new char[515];
    char* buf = new char[505];
    strcpy_s(temp_path,495, output_file_path);
    strcat_s(temp_path,505, ".temp");
    CURL* curl;
    CURLcode res;
    FILE* output_file;

    curl = curl_easy_init();
    if (!curl) {
        return 1;
    }

    // Set the URL to download
    curl_easy_setopt(curl, CURLOPT_URL, url);

    // Create a file to write the downloaded data
    output_file = fopen(temp_path, "wb");
    if (!output_file) {
        curl_easy_cleanup(curl);
        return 2;
    }

    // Set the write callback function
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_download);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);
    if (ignore_insecure == true)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    // Perform the download
    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        return 3;
    }
    // Cleanup and close the file
    curl_easy_cleanup(curl);
    fclose(output_file);
    if ((output_file = fopen(temp_path, "r")) == 0) {
        return 4;
    }
    else {
        fscanf(output_file, "%500s", buf);
        if (strcmp(buf, "no_auth") == 0) {
            fclose(output_file);
            return 5;
        }
        else if(check_cert(buf, SECRETS)==0){
            remove(output_file_path);//remove old file, so it can be overwritten
            fclose(output_file);
            if (rename(temp_path, output_file_path)!=0) {
				return 6;
            }
        }else {
        	fclose(output_file);
			return 7;
        }
    }
    delete[] buf;
    delete[] temp_path;
    return 0;
}

char* url_encode(const char* input) {
    // Allocate memory for the encoded string (worst case: every character needs encoding)
    size_t input_len = strlen(input);
    char* encoded = (char*)malloc(input_len * 3 + 1); // +1 for null terminator
    if (!encoded) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Define the characters that don't need encoding
    const char* safe_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";

    // Initialize variables for input and output string indices
    size_t i = 0; // index for input string
    size_t j = 0; // index for output string

    // Loop through each character in the input string
    while (input[i]) {
        // Check if the character is a safe character
        if (strchr(safe_chars, input[i]) != NULL) {
            encoded[j++] = input[i++]; // Copy safe character as is
        }
        else {
            // Encode non-safe character as percent-encoded representation
            sprintf(encoded + j, "%%%02X", (unsigned char)input[i]);
            j += 3; // Increment output index by 3 to account for encoding (%XX)
            i++; // Move to the next character in the input string
        }
    }

    // Null-terminate the encoded string
    encoded[j] = '\0';

    return encoded;
}
#endif