//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// ClassFactory.h	(Edited by: V. Drozd)
// src/COM/ClassFactory.h
//

//
// The file declares the class factory for the CalculateSumExt COM class.
//

/*
 *	This source is subject to the Microsoft Public License.
 *	See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 *	All other rights reserved.
 *
 *	THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND, 
 *	EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED 
 *	WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#pragma once

#include <unknwn.h> 
	//IClassFactory
#include <windows.h>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: declarations
//

class ClassFactory : public IClassFactory
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IClassFactory
    IFACEMETHODIMP CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv);
    IFACEMETHODIMP LockServer(BOOL fLock);

    ClassFactory();

protected:
    ~ClassFactory();

private:
    long m_cRef;
};

//
//
//
