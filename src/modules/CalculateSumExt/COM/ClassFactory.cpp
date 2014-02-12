//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// ClassFactory.cpp    (V. Drozd)
// src/COM/ClassFactory.cpp
//

//
// The file implements the class factory for the CalculateSumExt COM class. 
//

/*
 *  See http://www.microsoft.com/opensource/licenses.mspx#Ms-PL.
 *  All other rights reserved.
 *
 *  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY KIND,
 *  EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE IMPLIED
 *  WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR PURPOSE.
 */

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: includes
//

#include "ClassFactory.h"
#include "CalculateSumExt.h"
#include <new>
#include <Shlwapi.h>

#pragma comment(lib, "shlwapi.lib") // use linker option

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: definitions
//

extern long g_cDllRef;

ClassFactory::ClassFactory() : m_cRef(1)
{
    InterlockedIncrement(&g_cDllRef);
}

ClassFactory::~ClassFactory()
{
    InterlockedDecrement(&g_cDllRef);
}

//
// IUnknown
//

IFACEMETHODIMP ClassFactory::QueryInterface(REFIID riid, void** ppv)
{
    *ppv = 0;

    if (riid == IID_IUnknown || riid == IID_IClassFactory)
       *ppv = static_cast<IClassFactory *>( this );
   
    if (*ppv) {
        AddRef();
        return( S_OK );
    }

    return (E_NOINTERFACE);
}


IFACEMETHODIMP_(ULONG) ClassFactory::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

IFACEMETHODIMP_(ULONG) ClassFactory::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef) {
        delete this;
    }

    return cRef;
}


// 
// IClassFactory
//

IFACEMETHODIMP ClassFactory::CreateInstance(IUnknown *pUnkOuter, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_NOAGGREGATION;

    // pUnkOuter is used for aggregation. We do not support it in the sample.
    if (pUnkOuter == NULL) {
        hr = E_OUTOFMEMORY;

        // Create the COM component.
        CalculateSumExt *pExt = new (std::nothrow) CalculateSumExt();
        if (pExt) {
            // Query the specified interface.
            hr = pExt->QueryInterface(riid, ppv);
            pExt->Release();
        }
    }

    return hr;
}

IFACEMETHODIMP ClassFactory::LockServer(BOOL fLock)
{
    if (fLock) {
        InterlockedIncrement(&g_cDllRef);
    }
    else {
        InterlockedDecrement(&g_cDllRef);
    }
    return S_OK;
}

//
//
//