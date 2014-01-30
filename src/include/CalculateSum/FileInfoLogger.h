//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// FileInfoLogger.h	(V. Drozd)
// src/CalculateSum/FileInfoLogger.h
//

//
// Main class that provide logging information about files
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

#include "CalculateSum/Types.h"

#include <vector>
#include <string>
#include <future>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: declarations
//

class FileInfoLogger {
public:
	FileInfoLogger(std::vector<std::wstring> filePaths, std::wstring logFilePath);
	FileInfoLogger(std::vector<std::string> filePaths, std::string logFilePath);
	FileInfoLogger(std::vector<fs::path> filePaths, fs::path logFilePath);

	bool process();
private:
	void internal_init();

	bool writeResultsIntoLog(std::fstream & sfile);

	std::vector<fs::path>  filePaths_;
	fs::path               logFilePath_;

	std::vector<std::future<FileInfo>> results;
};

//
//
//
