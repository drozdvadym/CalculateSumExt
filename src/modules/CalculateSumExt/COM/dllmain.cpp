//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// dllmain.cpp (V. Drozd)
// src/COM/dllmain.cpp
//

//
// The file implements DllMain, and the DllGetClassObject, DllCanUnloadNow, 
// DllRegisterServer, DllUnregisterServer functions that are necessary for a COM DLL. 
//
// DllGetClassObject invokes the class factory defined in ClassFactory.h/cpp and 
// queries to the specific interface.
//
// DllCanUnloadNow checks if we can unload the component from the memory.
//
// DllRegisterServer registers the COM server and the context menu handler in 
// the registry by invoking the helper functions defined in Reg.h/cpp. The 
// context menu handler is associated with the .cpp file class.
//
// DllUnregisterServer unregisters the COM server and the context menu handler. 
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

#define COM_LIB_MODULE

#include "ClassFactory.h"
#include "Reg.h"

#include <Guiddef.h>


// {C26734F0-2436-4CC1-AE16-37F18D96E67B}
static const GUID CLSID_CalculateSumExt = {
        0xc26734f0, 0x2436, 0x4cc1, {
            0xae, 0x16, 0x37, 0xf1, 0x8d, 0x96, 0xe6, 0x7b
        } 
};

HINSTANCE g_hInst   = NULL;
long      g_cDllRef = 0;

const WCHAR *_s_file_ext[] = {
    L"*",
    L"Directory"
};

BOOL APIENTRY
DllMain(
    HMODULE hModule,
    DWORD dwReason,
    LPVOID lpReserved
    )
{
    switch (dwReason) {
        case DLL_PROCESS_ATTACH:
            // Hold the instance of this DLL module, we will use it to get the 
            // path of the DLL to register the component.
            g_hInst = hModule;
            DisableThreadLibraryCalls(hModule);
            break;
        case DLL_THREAD_ATTACH:
        case DLL_THREAD_DETACH:
        case DLL_PROCESS_DETACH:
            break;
    }

    return TRUE;
}

//
// FUNCTION: DllGetClassObject
//
// PURPOSE: Create the class factory and query to the specific interface.
//
// PARAMETERS:
//  * rclsid - The CLSID that will associate the correct data and code.
//  * riid - A reference to the identifier of the interface that the caller 
//    is to use to communicate with the class object.
//  * ppv - The address of a pointer variable that receives the interface 
//    pointer requested in riid. Upon successful return, *ppv contains the 
//    requested interface pointer. If an error occurs, the interface pointer 
//    is NULL. 
//

STDAPI DllGetClassObject(REFCLSID rclsid, REFIID riid, void **ppv)
{
    HRESULT hr = CLASS_E_CLASSNOTAVAILABLE;

    if (IsEqualCLSID(CLSID_CalculateSumExt, rclsid)) {
        hr = E_OUTOFMEMORY;

        ClassFactory *pClassFactory = new ClassFactory();
        if (pClassFactory)  {
            hr = pClassFactory->QueryInterface(riid, ppv);
            pClassFactory->Release();
        }
    }

    return hr;
}


//
// FUNCTION: DllCanUnloadNow
//
// PURPOSE: Check if we can unload the component from the memory.
//
// NOTE: The component can be unloaded from the memory when its reference 
// count is zero (i.e. nobody is still using the component).
// 

STDAPI DllCanUnloadNow(void)
{
    return g_cDllRef > 0 ? S_FALSE : S_OK;
}


//
// FUNCTION: DllRegisterServer
//
// PURPOSE: Register the COM server and the context menu handler.
// 

STDAPI DllRegisterServer(void)
{
    HRESULT hr;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Register the component.
    hr = RegisterInprocServer(
        szModule,
        CLSID_CalculateSumExt,
        L"CalculateSum.CalculateSumExt Class", 
        L"Apartment"
    );
    if (SUCCEEDED(hr)) {
        // Register the context menu handler.
        for (int i = 0; i < 2; i++) {
            hr = RegisterShellExtContextMenuHandler(
                _s_file_ext[i], CLSID_CalculateSumExt,
                L"CalculateSum.CalculateSumExt"
            );
        }
    }

    return hr;
}


//
// FUNCTION: DllUnregisterServer
//
// PURPOSE: Unregister the COM server and the context menu handler.
// 

STDAPI DllUnregisterServer(void)
{
    HRESULT hr = S_OK;

    wchar_t szModule[MAX_PATH];
    if (GetModuleFileName(g_hInst, szModule, ARRAYSIZE(szModule)) == 0) {
        hr = HRESULT_FROM_WIN32(GetLastError());
        return hr;
    }

    // Unregister the component.
    hr = UnregisterInprocServer(CLSID_CalculateSumExt);
    if (SUCCEEDED(hr)) {
        // Unregister the context menu handler.
        for (int i = 0; i < 2; i++) {
            hr = UnregisterShellExtContextMenuHandler(
                _s_file_ext[i], CLSID_CalculateSumExt
            );
        }
    }

    return hr;
}

//
//
//
