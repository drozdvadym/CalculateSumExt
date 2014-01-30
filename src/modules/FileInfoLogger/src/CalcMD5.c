//
// -*- Mode: c; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// CalcMD5.c	(V. Drozd)
// src/modules/FileInfoLogger/src/CalcMD5.cpp
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

#define _CRT_SECURE_NO_WARNINGS

#include "CalcMD5.h"

#include <stdio.h>
#include <string.h>


///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definitions
//


#ifdef _WIN32
#define my_wfopen(file, filename, param) \
	file = _wfopen(filename, param);
#endif

#ifdef __linux__
const wchar_t *ss = filename;

int i = (int)wcsrtombs(NULL, &ss, 0, NULL);
if (i<0)
	return NULL;
char *d = (char *)malloc(i + 1);
wcsrtombs(d, &filename, i, NULL);
d[i] = 0;

m_WorkFile = fopen(d, param);
#endif

#define BUF_SIZE 2048

int calculateMD5(const wchar_t* fileName, char aResult[])
{
	unsigned char c[MD5_DIGEST_LENGTH];
	size_t bytes;
	int i;
	_wfopen()
	FILE *inFile = _wfopen(fileName, "rb");
	if (NULL == inFile) {
		return 0;
		/*NOTREACHED*/
	}

	MD5_CTX mdContext;
	long long sz, sz2;
	unsigned char data[BUF_SIZE];


	MD5_Init(&mdContext);
	while ((bytes = fread(data, 1, BUF_SIZE, inFile)))
		MD5_Update(&mdContext, data, bytes);
	MD5_Final(c, &mdContext);

	//Forming result string
	{
		char tmpRes[2];
		for (i = 0; i < MD5_DIGEST_LENGTH; i++) {
			sprintf(tmpRes, "%02x", c[i]);
			aResult[2 * i] = tmpRes[0];
			aResult[2 * i + 1] = tmpRes[1];
		}
		aResult[MD5_STR_RESULT_LENGT - 1] = '\0';
	}

	fclose(inFile);
	
	return 0;
}

//
//
//

