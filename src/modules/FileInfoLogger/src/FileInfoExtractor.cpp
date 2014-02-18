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

FileInfo FileInfoExtract(fs::path& filePath)
{
    FileInfo finfo;
    boost::system::error_code ec;
    
    finfo.is_correct = false;

    do {
		finfo.full_name = filePath.string();
		finfo.short_name = filePath.filename().string();

        finfo.size = fs::file_size(filePath, ec);
        if (ec) {
            break;
            //NOTREACHED
        }

        finfo.creation = getTimeCreation(filePath, ec);
        if (ec) {
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
