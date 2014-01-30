//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// CalcMD5.h	(V. Drozd)
// src/modules/FileInfoLogger/src/CalcMD5.h
//

//
// Calculate MD5 sum
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

#include <openssl\md5.h>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definitions
//

#define MD5_STR_RESULT_LENGT (2 * MD5_DIGEST_LENGTH + 1)

#ifdef __cplusplus
extern "C" {
#endif

int calculateMD5(const wchar_t* fileName, char aResult[]);

#ifdef __cplusplus
}
#endif

