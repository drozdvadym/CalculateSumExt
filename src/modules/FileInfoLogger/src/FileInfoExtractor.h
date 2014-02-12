//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// FileInfoExtractor.h (V. Drozd)
// src/modules/FileInfoLogger/src/FileInfoExtractor.h
//

//
// Provides functionality to fetch main information about the file
//

//
// THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
// EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
// WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
//

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#pragma once

#include "CalculateSum/Types.h"


///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: declarations
//

FileInfo FileInfoExtract(fs::path& filePath);

//
//
//
