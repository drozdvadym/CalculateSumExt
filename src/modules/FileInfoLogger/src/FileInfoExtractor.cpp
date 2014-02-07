//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// FileInfoExtractor.cpp	(V. Drozd)
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
#include "openssl\md5.h"

#include <ctime>
#include <fstream>
#include <iomanip>

#ifdef _WIN32
# include <Windows.h>
#endif

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: local function declaration
//

std::string getTimeCreation(fs::path, boost::system::error_code);
std::string getFileMD5(fs::path);
std::string getHumanReadableSize(long long);
std::string byte_to_hex_string(unsigned char);

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definitions
//

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr)
{
#ifdef _WIN32
	int size_needed = WideCharToMultiByte(
		CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL
	);
	std::string retVal(size_needed, 0);
	WideCharToMultiByte(
		CP_UTF8, 0, &wstr[0], (int)wstr.size(), &retVal[0], size_needed, NULL, NULL
	);
#else //@todo: Write correct decoding for UNIX and LINUX like system
	std::string retVal(wstr.begin(), wstr.end());
#endif
	return (retVal);
}

//if we have std::string we do nothing
std::string utf8_encode(const std::string &str)
{
	return (str);
}

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str)
{
#ifdef _WIN32
	int size_needed = MultiByteToWideChar(
		CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0
	);
	std::wstring retVal(size_needed, 0);
	MultiByteToWideChar(
		CP_UTF8, 0, &str[0], (int)str.size(), &retVal[0], size_needed
	);
#else //@todo: Write correct decoding for UNIX and LINUX like system
	std::wstring retVal(str.begin(), str.end());
#endif
	return (retVal);
}

//if we have std::wstring we do nothing
std::wstring utf8_decode(const std::wstring &wstr)
{
	return (wstr);
}

FileInfo FileInfoExtract(fs::path filePath)
{
	FileInfo finfo;
	boost::system::error_code ec;
	
	finfo.is_correct = false;

	do {
		
		finfo.full_name = utf8_encode(filePath.c_str());
		finfo.short_name = utf8_encode(filePath.filename().c_str());

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

std::string getTimeCreation(fs::path filePath, boost::system::error_code ec)
{
	std::string retval;

	std::time_t time;
	std::tm *tminfo;

	time = fs::last_write_time(filePath, ec);
	if (!!ec) {
		return retval;
		//NOTREACHED
	}

	tminfo = std::localtime(&time);

	retval += std::to_string(tminfo->tm_mday) + "/";
	retval += std::to_string(tminfo->tm_mon + 1) + "/";
	retval += std::to_string(tminfo->tm_year + 1900);

	return (retval);
}

std::string getFileMD5(fs::path filePath)
{
	std::string retval;

	//calculate optimal BUF_SIZE due to file_size ???
	const int BUF_SIZE = 2048;
	unsigned char data[BUF_SIZE];
	unsigned char c[MD5_DIGEST_LENGTH];

	std::ifstream file(filePath.c_str(), std::ios::binary);
	MD5_CTX mdContext;

	if (!file.is_open()) {
		return retval;
		/*NOTREACHED*/
	}

	MD5_Init(&mdContext);

	while (file.read((char *)data, BUF_SIZE))
		MD5_Update(&mdContext, data, BUF_SIZE);

	MD5_Update(&mdContext, data, file.gcount());
	MD5_Final(c, &mdContext);

	file.close();

	for (size_t i = 0; i < MD5_DIGEST_LENGTH; i++) {
		retval += byte_to_hex_string(c[i]);
	}

	return (retval);
}

std::string getHumanReadableSize(long long fileSize)
{

	if (!fileSize) return std::string("0 bytes");

	static const auto _SIZE_TB = 1024LL * 1024LL * 1024LL * 1024LL;

	std::string retval;

	long long curSize;
	char *sizesPrefix[] = { " Tera ", " Giga ", " Mega ", " Kilo ", " " };

	long long curDelim = _SIZE_TB;
	for (size_t i = 0; i < 5; i++, curDelim /= 1024LL) {
		curSize = fileSize / curDelim;
		fileSize %= curDelim;
		
		if (curSize) {
			retval += std::to_string(curSize) + sizesPrefix[i];
			retval += (1 == curSize) ? "byte " : "bytes ";
		}
	}
	//Remove last space symbol
	retval.erase(retval.end() - 1);

	return (retval);
}

std::string byte_to_hex_string(unsigned char ch)
{
	char tmpRes[3];
	std::sprintf(tmpRes, "%02x", ch);

	return std::string(tmpRes);
}

//
//
//
