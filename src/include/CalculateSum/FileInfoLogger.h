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
#include <mutex>
#include <condition_variable>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: declarations
//

class FileInfoLogger {
public:
	FileInfoLogger(std::vector<std::wstring>& filePaths, std::wstring& logFilePath);
	FileInfoLogger(std::vector<std::string>& filePaths,  std::string& logFilePath);
	FileInfoLogger(std::vector<fs::path>& filePaths,     fs::path& logFilePath);

	bool process();
private:
    //deprecate copy constructor and assigment operator
    FileInfoLogger(const FileInfoLogger&);
    FileInfoLogger& operator=(const FileInfoLogger&);

    void internalInit();
    bool writeResultsIntoLog();
    FileInfo infoExtractorWrapper(fs::path& fpath, const size_t taskIdx);


    std::vector<fs::path>  file_paths;
    fs::path               log_file_path;

    //Vector with all results for FileInfoExtract
    std::vector<std::future<FileInfo>> results;

    //For synchronization of threads @{
    bool    is_extract_done;
    size_t  idx_extract_done_task;

    mutable std::mutex              extract_done_mutex;
    mutable std::condition_variable extract_done_condition;
    //@}
};

//
//
//
