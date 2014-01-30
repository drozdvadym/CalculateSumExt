//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// FileInfoLogger.cpp	(V. Drozd)
// src/modules/FileInfoLogger/src/FileInfoLogger.cpp
//

//
// Main class that provide logging information about files
//

//
// @todo: 
// 1) add normal error handling
// 2) add normal thread handling
//    2.1) if one thread error occurred -> stop all another
//

//
//	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
//	EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
//	WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#define _CRT_SECURE_NO_WARNINGS

#include "CalculateSum/FileInfoLogger.h"
#include "FileInfoExtractor.h"

#include "ThreadPool.h"
#include <chrono>

#include <algorithm>

#include <sstream>
#include <fstream>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definitions
//


FileInfoLogger::FileInfoLogger(std::vector<std::wstring> filePaths, std::wstring logFilePath) :
	filePaths_(filePaths.begin(), filePaths.end()),
	logFilePath_(logFilePath)
{
	internal_init();
}

FileInfoLogger::FileInfoLogger(std::vector<std::string> filePaths, std::string logFilePath) :
	filePaths_(filePaths.begin(), filePaths.end()),
	logFilePath_(logFilePath)
{
	internal_init();
}

FileInfoLogger::FileInfoLogger(std::vector<fs::path> filePaths, fs::path logFilePath) :
	filePaths_(filePaths.begin(), filePaths.end()),
	logFilePath_(logFilePath)
{
	internal_init();
}

void FileInfoLogger::internal_init()
{
	//Fisrt of all check if fNames containts fullLogFileName
	fs::path cpath(logFilePath_);
	auto finded = std::find_if(
		filePaths_.begin(),
		filePaths_.end(),
		[cpath](const fs::path& thisPath) {
			return fs::equivalent(thisPath, cpath);
		}
	);
	if (finded != filePaths_.end()) {
		filePaths_.erase(finded);
	}

	//Delete all dirs
	filePaths_.erase(
		std::remove_if(
			filePaths_.begin(),
			filePaths_.end(),
			[](const fs::path& thisPath) { return fs::is_directory(thisPath); }
		),
		filePaths_.end()
	);

	//And sort file list in alphabetical order
	std::sort(filePaths_.begin(), filePaths_.end());

	//Allocate memory for results
	results.resize(filePaths_.size());
}

void updateOffsets(std::vector<long> & offsets, int pos, long val)
{
	long diff = (!pos ? offsets[0] : offsets[pos] - offsets[pos - 1]) - val;
	
	for (size_t cp = pos; cp < offsets.size(); cp++)
		offsets[cp] -= diff;
}

bool FileInfoLogger::writeResultsIntoLog(std::fstream & sfile)
{
	std::string res;

	std::vector<bool> visited(filePaths_.size());
	std::vector<long> offsets(filePaths_.size());
	size_t visitedCount = 0;

	std::chrono::microseconds dura = std::chrono::microseconds(20);
	
	while (visitedCount != filePaths_.size()) {
		for (size_t i = 0; i < filePaths_.size(); i++) {
			
			if (visited[i]) continue;

			if (!results[i]._Is_ready()) continue;

			FileInfo finfo = results[i].get();
			
			//Need correct error handling
			if (!finfo.is_correct) {
				return false;
				//NOTREACHED
			}

			res = finfo.toString();

			// Rewrite the file
			//Awesome :)
			{
				std::stringstream buffer;

				//Go to position for insert new line
				sfile.seekp(offsets[i], std::ios_base::beg);

				//Read and save in string stream all text that must be rewrited
				buffer << sfile.rdbuf();

				//Write new data in file
				sfile.seekp(offsets[i], std::ios_base::beg);
				sfile << res.c_str();

				//Paste saved data from string stream
				sfile << buffer.str().c_str();
				updateOffsets(offsets, i, res.length() + 1);
			}

			visited[i] = true;
			visitedCount++;
		}
		//Wait some little time 
		std::this_thread::sleep_for(dura);

		//@todo: need correct thread handling!!!

		//Every next cycle we sleep longer
		dura += std::chrono::microseconds(20);
	}

	return true;
}

// Minimum 1 thread for writeResultsIntoLog and 1 for FileInfoExtract
const auto min_thread_needed = 2U;

bool FileInfoLogger::process()
{
	//Create logFile
	std::fstream file(logFilePath_.c_str(), std::ios::out | std::ios::in | std::ios::trunc);
	if (!file.is_open()) {
		return false;
		//NOTREACHED
	}
	
	//Calculate optimal pool size
	size_t thread_count = std::max(min_thread_needed, std::thread::hardware_concurrency());

	ThreadPool pool(thread_count);
	
	std::future<bool> state = pool.enqueue(
		&FileInfoLogger::writeResultsIntoLog, std::ref(*this), std::ref(file)
	);

	for (size_t i = 0; i < filePaths_.size(); ++i) {
		fs::path cpath = filePaths_[i];
		results[i] = pool.enqueue(
			[cpath]() {
				return FileInfoExtract(cpath);
			}
		);
	}

	pool.wait();

	file.close();

	return state.get();
}

//
//
//
