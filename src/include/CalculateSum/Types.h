//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// Types.h	(V. Drozd)
// src/CalculateSum/Types.h
//


//
// Declarations of main data types, that used in different modules
//

//
//	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
//	EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#pragma once

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

//@todo: create precompiled header!!!
#include <boost/filesystem.hpp>  

namespace fs = ::boost::filesystem;

#include <string>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: type declarations
//

struct FileInfo {
	std::string full_name;
	std::string short_name;
	std::string checksum;
	std::string creation;
	std::string human_readable_size;
	long long   size;
	bool        is_correct;

	std::string toString()
	{
		std::string retVal;

		retVal += short_name + ", ";
		retVal += "size is: " + human_readable_size + ", ";
		retVal += "created: " + creation + ", ";
		retVal += "MD5: " + checksum + "\n";

		return (retVal);
	}
};

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: functions declarations
//

// Convert a wide Unicode string to an UTF8 string
std::string utf8_encode(const std::wstring &wstr);
std::string utf8_encode(const std::string &wstr);

// Convert an UTF8 string to a wide Unicode String
std::wstring utf8_decode(const std::string &str);
std::wstring utf8_decode(const std::wstring &str);

//
//
//
