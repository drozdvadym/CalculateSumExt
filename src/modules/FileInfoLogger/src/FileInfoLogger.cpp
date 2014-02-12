//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// FileInfoLogger.cpp    (V. Drozd)
// src/modules/FileInfoLogger/src/FileInfoLogger.cpp
//

//
// Main class that provide logging information about files
//

//
// @todo: 
// 1. add normal error handling
//

//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
//  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
//  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#define _CRT_SECURE_NO_WARNINGS

#include "CalculateSum/FileInfoLogger.h"
#include "FileInfoExtractor.h"

#include "ThreadPool.h"

#include <algorithm>

#include <sstream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: public function member definitions
//

FileInfoLogger::FileInfoLogger(std::vector<std::wstring>& filePaths, std::wstring& logFilePath)
    : file_paths(filePaths.begin(), filePaths.end())
    , log_file_path(logFilePath)
{
    internalInit();
}

FileInfoLogger::FileInfoLogger(std::vector<std::string>& filePaths, std::string& logFilePath)
    : file_paths(filePaths.begin(), filePaths.end())
    , log_file_path(logFilePath)
{
    internalInit();
}

FileInfoLogger::FileInfoLogger(std::vector<fs::path>& filePaths, fs::path& logFilePath)
    : file_paths(filePaths.begin(), filePaths.end())
    , log_file_path(logFilePath)
{
    internalInit();
}

bool FileInfoLogger::process()
{
    // wrap the main function
    auto binded_fn = std::bind(&FileInfoLogger::writeResultsIntoLog, this);
    auto task = std::packaged_task<bool()>(binded_fn);

    std::future<bool> result = task.get_future();

    //And run main task in thread
    std::thread(std::move(task)).detach();

    //Create thread pool with optimal size for logger
    ThreadPool pool(std::max(1U, std::thread::hardware_concurrency() - 1));

    //Add tasks for calculating file information
    for (size_t i = 0; i < file_paths.size(); ++i) {
        fs::path &cpath = file_paths[i];
        results[i] = pool.addTask(
            [this, &cpath, i]() { return infoExtractorWrapper(cpath, i); }
        );
    }

    //Wait for result
    //That mean all task is done or an error occurred
    result.wait();

    bool status = result.get();
    
    //false == status -> error occurred and we must clear task queue
    if (!status)
        pool.clearTaskQueue();

    return (status);
    
}
///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: private function member definitions
//

void FileInfoLogger::internalInit()
{
    //Fisrt of all check if fNames containts logFilePath_
    fs::path cpath(log_file_path);
    auto finded = std::find_if(
        file_paths.begin(),
        file_paths.end(),
        [&cpath](const fs::path& thisPath) {
            return fs::equivalent(thisPath, cpath);
        }
    );
    if (finded != file_paths.end()) {
        file_paths.erase(finded);
    }

    //Delete all directory paths
    file_paths.erase(
        std::remove_if(
            file_paths.begin(),
            file_paths.end(),
            [](const fs::path& thisPath) { return fs::is_directory(thisPath); }
        ),
        file_paths.end()
    );

    //Sort file list in alphabetical order
    std::sort(file_paths.begin(), file_paths.end());

    //Allocate memory for results
    results.resize(file_paths.size());
}

void inline updateOffsets(std::vector<long>& offsets, int pos, long val)
{
    long diff = (!pos ? offsets[0] : offsets[pos] - offsets[pos - 1]) - val;
    
    for (size_t cp = pos; cp < offsets.size(); cp++) {
        offsets[cp] -= diff;
    }
}

bool FileInfoLogger::writeResultsIntoLog()
{
    //Open (create) logFile
    std::fstream file(log_file_path.c_str(), std::ios::out | std::ios::in | std::ios::trunc);
    if (!file.is_open()) {
        return false;
        //NOTREACHED
    }

    std::string res;

    std::vector<bool> visited(file_paths.size());
    std::vector<long> offsets(file_paths.size());
    size_t visitedCount = 0;

    is_extract_done = false;

    while (visitedCount != file_paths.size()) {
        //Wait for one work is done
        {
            std::unique_lock<std::mutex> lock(extract_done_mutex);

            while (!is_extract_done)
                extract_done_condition.wait(lock);

            //and wait for result
            auto idx = idx_extract_done_task;
            while (!results[idx]._Is_ready());
        }
        is_extract_done = false;

        for (size_t i = 0; i < file_paths.size(); i++) {
            
            if (visited[i] || !results[i]._Is_ready()) continue;

            FileInfo finfo = results[i].get();
            
            //@todo: Need correct error handling
            if (!finfo.is_correct) {
                return false;
                //NOTREACHED
            }

            res = finfo.toString();
            // Rewrite the file
            {
                std::stringstream buffer;

                //Go to position for insert new line
                file.seekp(offsets[i], std::ios_base::beg);

                //Read and save in string stream all text that must be rewrited
                buffer << file.rdbuf();

                //Write new data in file
                file.seekp(offsets[i], std::ios_base::beg);
                file << res.c_str();

                //Paste saved data from string stream
                file << buffer.str().c_str();
                updateOffsets(offsets, i, res.length() + 1);
            }

            visited[i] = true;
            visitedCount++;
        }
    }

    file.close();
    return true;
}

FileInfo FileInfoLogger::infoExtractorWrapper(fs::path& fpath, size_t idx)
{
    FileInfo retVal = FileInfoExtract(fpath);

    {
        std::unique_lock<std::mutex> lock(extract_done_mutex);
        is_extract_done = true;
        idx_extract_done_task = idx;
        extract_done_condition.notify_one(); //Wake up main thread
    }

    return (retVal);
}

//
//
//
