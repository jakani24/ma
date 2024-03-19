#pragma warning(disable:4996)
#ifndef CONNECT_CPP
#define CONNECT_CPP

#include "connect.h"
#include "well_known.h"
#include "security.h"
#include <curl/curl.h>
#include <string>
#include <mutex>
std::mutex connect_mutex;

//this function is thread safe
int fast_send(const std::string& url, bool ignore_insecure) {
    std::lock_guard<std::mutex> lock(connect_mutex);
    thread_local const std::string url_ = url;
    thread_local const bool ignore_insecure_ = ignore_insecure;
    thread_local CURL* curl;
    thread_local CURLcode res;
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url_.c_str());
        if (ignore_insecure_)
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

static size_t write_callback_connect(void* contents, size_t size, size_t nmemb, void* userp) {
    ((std::string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}
//make this multi thread safe
int connect_to_srv(const std::string& url, char* out, int max_len, bool ignore_insecure) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_connect);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        if (ignore_insecure)
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        if (max_len > (int)readBuffer.length()) {
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

int download_file_from_srv(const std::string& url, const std::string& output_file_path, bool ignore_insecure, bool do_not_check_cyberhex_cert) {
    char* temp_path = new char[output_file_path.size() + 6];
    strcpy(temp_path, output_file_path.c_str());
    strcat(temp_path, ".temp");

    CURL* curl;
    CURLcode res;
    FILE* output_file;

    curl = curl_easy_init();
    if (!curl) {
        delete[] temp_path;
        return 1;
    }

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());

    output_file = fopen(temp_path, "wb");
    if (!output_file) {
        curl_easy_cleanup(curl);
        delete[] temp_path;
        return 2;
    }

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback_download);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output_file);
    if (ignore_insecure)
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);

    res = curl_easy_perform(curl);
    if (res != CURLE_OK) {
        fclose(output_file);
        delete[] temp_path;
        return 3;
    }

    curl_easy_cleanup(curl);
    fclose(output_file);

    if ((output_file = fopen(temp_path, "r")) == 0) {
        delete[] temp_path;
        return 4;
    }
    else {
        char buf[501];
        fscanf(output_file, "%500s", buf);
        if (strcmp(buf, "no_auth") == 0) {
            fclose(output_file);
            delete[] temp_path;
            return 5;
        }
        else if (check_cert(buf, SECRETS) == 0 or do_not_check_cyberhex_cert==true) {
            remove(output_file_path.c_str());
            fclose(output_file);
            if (rename(temp_path, output_file_path.c_str()) != 0) {
                delete[] temp_path;
                return 6;
            }
        }
        else {
            fclose(output_file);
            delete[] temp_path;
            return 7;
        }
    }

    delete[] temp_path;
    return 0;
}

std::string url_encode(const std::string& input) {
    static const char* const safe_chars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_.~";
    std::string encoded;
    for (char c : input) {
        if (std::strchr(safe_chars, c) != nullptr) {
            encoded += c;
        }
        else {
            char temp[4];
            sprintf(temp, "%%%02X", (unsigned char)c);
            encoded += temp;
        }
    }
    return encoded;
}

int upload_to_srv(const std::string& url, const std::string& filepath, bool ignore_insecure) {
    CURL* curl;
    CURLcode res;
    struct curl_httppost* formpost = nullptr;
    struct curl_httppost* lastptr = nullptr;
    struct curl_slist* headerlist = nullptr;
    static const char buf[] = "Expect:";

    curl_global_init(CURL_GLOBAL_ALL);
    curl_formadd(&formpost, &lastptr, CURLFORM_COPYNAME, "file", CURLFORM_FILE, filepath.c_str(), CURLFORM_END);

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        if (ignore_insecure)
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
        headerlist = curl_slist_append(headerlist, buf);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
        curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl_formfree(formpost);
        curl_slist_free_all(headerlist);
        if (res == CURLE_OK) {
            return 0;
        }
        else {
            return 1;
        }
    }
    return 2;
}
int send_to_pipe(const std::string& message) {
    HANDLE hPipe;
    DWORD dwRead;
    DWORD dwWritten;
    std::ofstream(MAIN_COM_PATH); // Create the pipe
    hPipe = CreateFile(TEXT(MAIN_COM_PATH),
        GENERIC_READ | GENERIC_WRITE,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        WriteFile(hPipe,
            message.c_str(),
            strlen(message.c_str()) + 1,   // = length of string + terminating '\0' !!!
            &dwWritten,
            NULL);
        CloseHandle(hPipe);
        return 0;
    }
    else {
		return 1;
	}
}
std::string read_from_pipe() {
    HANDLE hPipe;
    DWORD dwRead;
    char buffer[1000];
    hPipe = CreateFile(TEXT(MAIN_COM_PATH),
        GENERIC_READ,
        0,
        NULL,
        OPEN_EXISTING,
        0,
        NULL);
    if (hPipe != INVALID_HANDLE_VALUE)
    {
        if (ReadFile(hPipe, buffer, sizeof(buffer), &dwRead, NULL))
        {
            CloseHandle(hPipe);
            buffer[dwRead] = '\0'; // Ensure null-termination after reading
            std::ofstream(MAIN_COM_PATH); // delete the pipe
            return std::string(buffer);
        }
        else
        {
            // ReadFile failed, print error
            DWORD dwError = GetLastError();
            // Handle dwError appropriately (e.g., log, display error message)
            CloseHandle(hPipe);
            std::ofstream(MAIN_COM_PATH); // delete the pipe
            return "Error_reading_from_pipe";
        }
    }
    else
    {
        // CreateFile failed, print error
        DWORD dwError = GetLastError();
        // Handle dwError appropriately (e.g., log, display error message)
        std::ofstream(MAIN_COM_PATH); // delete the pipe
        return "Error_creating_pipe";
    }
    std::ofstream(MAIN_COM_PATH); // delete the pipe
}
#endif
