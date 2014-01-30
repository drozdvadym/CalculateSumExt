//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// main.cpp	(V. Drozd)
// src/bin/testSample/src/main.cpp
//

//
// smple that using FileInfoLogger
//

//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#define BOOST_FILESYSTEM_VERSION 3
#define BOOST_FILESYSTEM_NO_DEPRECATED

#include "CalculateSum\FileInfoLogger.h"

#include <boost/filesystem.hpp>

#include <iostream>
#include <vector>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: variable definitions
//

#define LOG_FILE_NAME "file_inf.log"
static const char _s_logFileName[] = LOG_FILE_NAME;

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: declarations
//

//
// return the full filenames of all files in the specified directory
//

void getAllFileNames(const fs::path& root_path, std::vector<fs::path>& fileNames);

//
// Print usage message in stdout
//

static void _t_usage();

//
// Print argument error message in stderr
//

static void _t_args_error_occured();

//
// Print unknwn error message in stderr
//

static void _t_unknwn_error_occured();

#if WIN32 || _WIN64
# define DFLT_SEPARATOR "\\"
#else
# define DFLT_SEPARATOR "/"
#endif

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definitions
//

int main(int argc, char *argv[])
{

	if (argc != 2) {
		_t_args_error_occured();
		return 0;
		//NOTREACHED
	}

	if (!std::strcmp(argv[1], "-h")) {
		_t_usage();
		return 0;
	}

	const fs::path workDir(argv[1]);

	//Get all files names
	std::vector<fs::path> fileList;
	getAllFileNames(workDir, fileList);
	if (fileList.empty()) {
		std::cout << "There are no files in " << argv[1] << " directory" << std::endl;
		return 0;
		//NOTREACHED
	}

	std::string logFilePath(argv[1]);
	logFilePath += DFLT_SEPARATOR;
	logFilePath += _s_logFileName;


	FileInfoLogger fileLogger(fileList, fs::path(logFilePath));
	if (!fileLogger.process()) {
		_t_unknwn_error_occured();
		return (EXIT_FAILURE);
		//NOTREACHED
	}

	std::cout << L"Result saved to " << logFilePath << " file" << std::endl;

	return 0;
}

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: local definitions
//

void getAllFileNames(const fs::path& root_path, std::vector<fs::path>& fileNames)
{
	if (!fs::exists(root_path) && !fs::is_directory(root_path))
		return;

	fs::directory_iterator it(root_path);
	fs::directory_iterator endit;

	while (it != endit) {
		if (fs::is_regular_file(*it)) {
			fileNames.push_back(*it);
		}
		++it;
	}
}

static void _t_usage()
{
	 static const char  _s_usage[] =
		"Usage:\n"
		"\ttestSample [PARAMS]...\n"
		"\n"
		"testSample utility acumulate information about files in working directory "
		"and save in to [WDIR]\\" LOG_FILE_NAME " file in alphabetical order\n"
		"\n"
		" The following options are available:\n"
		"\n"
		"-h\t\tDisplay this message and exit.\n"
		"\n"
		"-w <path>\tWorking directory [WDIR].\n"
		"\n"
		"EXAMPLES:\n"
		" testSample -w ./home"
		"\n"
		"\n"
		"The testSample utility exits 0 on success, and >0 if an error occurs."
	;

	std::cout << _s_usage << std::endl;;
}

static void _t_args_error_occured()
{
	static const char errMessage[] =
		"Entered invalid arguments\n"
		"Try \"testSample -h\" for more information\n"
	;
	
	std::cerr << errMessage << std::endl;
}

static void _t_unknwn_error_occured()
{
	static const char errMessage[] =
		"An error occurred when calculated the files information.\n"
		"Sorry :(\n More detailed information will be in next version of extension"
	;

	std::cerr << errMessage << std::endl;
}

//
//
//
