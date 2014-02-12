//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// FileInfoExtractor.cpp (V. Drozd)
// src/modules/FileInfoLogger/src/FileInfoExtractor.cpp
//

//
// Provides functionality to get main information about the file
//

//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#define _CRT_SECURE_NO_WARNINGS

#include "FileInfoExtractor.h"
#include "openssl/md5.h"

#include <ctime>
#include <fstream>
#include <iomanip>

#ifdef _WIN32
# include <Windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: local function declaration
//

std::string getTimeCreation(fs::path&, boost::system::error_code&);
std::string getFileMD5(fs::path&);
std::string getHumanReadableSize(long long);
std::string byteToHexStr(unsigned char);

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definitions
//

#define _array_size(arr) sizeof(arr) / sizeof(arr[0])

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring& wstr)
{
#ifdef _WIN32
    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL
    );
    std::string retVal(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8, 0, &wstr[0], (int)wstr.size(), &retVal[0], size_needed, NULL, NULL
    );
#else
    //@todo: Write correct decoding for UNIX and LINUX like system
    std::string retVal(wstr.begin(), wstr.end());
#endif
    return (retVal);
}


// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string& str)
{
#ifdef _WIN32
    int size_needed = MultiByteToWideChar(
        CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0
    );
    std::wstring retVal(size_needed, 0);
    MultiByteToWideChar(
        CP_UTF8, 0, &str[0], (int)str.size(), &retVal[0], size_needed
    );
#else
    //@todo: Write correct decoding for UNIX and LINUX like system
    std::wstring retVal(str.begin(), str.end());
#endif
    return (retVal);
}

FileInfo FileInfoExtract(fs::path& filePath)
{
    FileInfo finfo;
    boost::system::error_code ec;
    
    finfo.is_correct = false;

    do {
#ifdef BOOST_WINDOWS_API
        finfo.full_name = utf8_encode(filePath.c_str());
        finfo.short_name = utf8_encode(filePath.filename().c_str());
#else
        finfo.full_name = filePath.c_str();
        finfo.short_name = filePath.filename().c_str();
#endif

        finfo.size = fs::file_size(filePath, ec);
        if (!!ec) {
            break;
            //NOTREACHED
        }

        finfo.creation = getTimeCreation(filePath, ec);
        if (!!ec) {
            break;
            //NOTREACHED
        }

        finfo.checksum = getFileMD5(filePath);
        if (finfo.checksum.empty()) {
            break;
            //NOTREACHED
        }
        
        finfo.human_readable_size = getHumanReadableSize(finfo.size);

        finfo.is_correct = true;

    } while (0);

    return (finfo);
}

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: local function definitions
//

std::string getTimeCreation(fs::path& filePath, boost::system::error_code& ec)
{
    std::string retVal;

    std::time_t time;
    std::tm *tminfo;

    time = fs::last_write_time(filePath, ec);
    if (!!ec) {
        return retVal;
        //NOTREACHED
    }

    tminfo = std::localtime(&time);

    retVal += std::to_string(tminfo->tm_mday) + "/";
    retVal += std::to_string(tminfo->tm_mon + 1) + "/";
    retVal += std::to_string(tminfo->tm_year + 1900);

    return (retVal);
}

std::string getFileMD5(fs::path& filePath)
{
    std::string retVal;

    static const int BUF_SIZE = 4096;
    unsigned char data[BUF_SIZE];
    unsigned char MD5res[MD5_DIGEST_LENGTH];

    std::ifstream file(filePath.c_str(), std::ios::binary);

    if (!file.is_open()) {
        return retVal;
        /*NOTREACHED*/
    }

    MD5_CTX mdContext;
    MD5_Init(&mdContext);

    while (file.read((char *)data, BUF_SIZE))
        MD5_Update(&mdContext, data, BUF_SIZE);

    MD5_Update(&mdContext, data, file.gcount());
    MD5_Final(MD5res, &mdContext);

    file.close();

    for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++)
        retVal += byteToHexStr(MD5res[i]);

    return (retVal);
}

std::string getHumanReadableSize(long long fileSize)
{
    static const auto _SIZE_TB = 1024LL * 1024LL * 1024LL * 1024LL;
    static const char *sizesPrefix[] = { " Tera ", " Giga ", " Mega ", " Kilo ", " " };

    if (!fileSize) {
        return std::string("0 bytes");
        //NOTREACHED
    }

    std::string retVal;

    long long curSize;
    long long delim = _SIZE_TB;

    for (size_t i = 0; i < _array_size(sizesPrefix); i++, delim /= 1024LL) {
        curSize = fileSize / delim;
        fileSize %= delim;
        
        if (curSize) {
            retVal += std::to_string(curSize) + sizesPrefix[i];
            retVal += (1 == curSize) ? "byte " : "bytes ";
        }
    }
    //Remove last space symbol
    retVal.erase(retVal.end() - 1);

    return (retVal);
}

std::string byteToHexStr(unsigned char ch)
{
    std::string retVal(2, 0);

    std::sprintf(&retVal[0], "%02x", ch);

    return (retVal);
}

//
//
//
