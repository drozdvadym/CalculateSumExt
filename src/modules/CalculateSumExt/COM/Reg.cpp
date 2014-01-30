//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// Reg.cpp (V. Drozd)
// src/COM/Reg.cpp
//

//
// The file implements the reusable helper functions to register and unregister 
// in-process COM components and shell context menu handlers in the registry.
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

#include "Reg.h"
#include <strsafe.h>

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: local function Declarations
//

#pragma region Registry Helper Functions Declarations

//
//   FUNCTION: SetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function creates a HKCR registry key and sets the specified 
//   registry value.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not 
//     exist, the function will create the registry key.
//   * pszValueName - specifies the registry value to be set. If pszValueName 
//     is NULL, the function will set the default value.
//   * pszData - specifies the string data of the registry value.
//
//   RETURN VALUE: 
//   If the function succeeds, it returns S_OK. Otherwise, it returns an 
//   HRESULT error code.
// 
HRESULT
SetHKCRRegistryKeyAndValue(
	PCWSTR pszSubKey,
	PCWSTR pszValueName, 
    PCWSTR pszData
	);

//
//   FUNCTION: GetHKCRRegistryKeyAndValue
//
//   PURPOSE: The function opens a HKCR registry key and gets the data for the 
//   specified registry value name.
//
//   PARAMETERS:
//   * pszSubKey - specifies the registry key under HKCR. If the key does not 
//     exist, the function returns an error.
//   * pszValueName - specifies the registry value to be retrieved. If 
//     pszValueName is NULL, the function will get the default value.
//   * pszData - a pointer to a buffer that receives the value's string data.
//   * cbData - specifies the size of the buffer in bytes.
//
//   RETURN VALUE:
//   If the function succeeds, it returns S_OK. Otherwise, it returns an 
//   HRESULT error code. For example, if the specified registry key does not 
//   exist or the data for the specified value name was not set, the function 
//   returns COR_E_FILENOTFOUND (0x80070002).
//

HRESULT
GetHKCRRegistryKeyAndValue(
	PCWSTR pszSubKey,
	PCWSTR pszValueName, 
    PWSTR pszData,
	DWORD cbData
	);

#pragma endregion

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: function definitions
//

HRESULT
RegisterInprocServer(
	PCWSTR pszModule,
	const CLSID& clsid, 
    PCWSTR pszFriendlyName,
	PCWSTR pszThreadModel
	)
{
    if (pszModule == NULL || pszThreadModel == NULL)
        return E_INVALIDARG;

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

	do {
		// Create the HKCR\CLSID\{<CLSID>} key.
		hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
		if (!SUCCEEDED(hr))
			break;
	
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);
		if (!SUCCEEDED(hr))
			break;

        // Create the HKCR\CLSID\{<CLSID>}\InprocServer32 key.
		hr = StringCchPrintf(
			szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s\\InprocServer32", szCLSID
		);
		if (!SUCCEEDED(hr))
			break;

        // Set the default value of the InprocServer32 key to the 
        // path of the COM module.
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszModule);
		if (!SUCCEEDED(hr))
			break;

        hr = SetHKCRRegistryKeyAndValue(
			szSubkey, L"ThreadingModel", pszThreadModel
		);

	} while (0);

    return hr;
}

HRESULT UnregisterInprocServer(const CLSID& clsid)
{
    HRESULT hr = S_OK;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // Delete the HKCR\CLSID\{<CLSID>} key.
    hr = StringCchPrintf(szSubkey, ARRAYSIZE(szSubkey), L"CLSID\\%s", szCLSID);
    if (SUCCEEDED(hr)) {
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));
    }

    return hr;
}


HRESULT
RegisterShellExtContextMenuHandler(
    PCWSTR pszFileType,
	const CLSID& clsid,
	PCWSTR pszFriendlyName
	)
{
    if (pszFileType == NULL)
        return E_INVALIDARG;

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // If pszFileType starts with '.', try to read the default value of the 
    // HKCR\<File Type> key which contains the ProgID to which the file type 
    // is linked.
    if (*pszFileType == L'.') {
        wchar_t szDefaultVal[260];
        hr = GetHKCRRegistryKeyAndValue(
			pszFileType, NULL, szDefaultVal, sizeof(szDefaultVal)
		);

        // If the key exists and its default value is not empty, use the 
        // ProgID as the file type.
        if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0') {
            pszFileType = szDefaultVal;
        }
    }

    // Create the key HKCR\<File Type>\shellex\ContextMenuHandlers\{<CLSID>}
    hr = StringCchPrintf(
		szSubkey,
		ARRAYSIZE(szSubkey), 
        L"%s\\shellex\\ContextMenuHandlers\\%s",
		pszFileType,
		szCLSID
	);

    if (SUCCEEDED(hr)) {
        // Set the default value of the key.
        hr = SetHKCRRegistryKeyAndValue(szSubkey, NULL, pszFriendlyName);
    }

    return hr;
}


HRESULT
UnregisterShellExtContextMenuHandler(
    PCWSTR pszFileType,
	const CLSID& clsid
	)
{
    if (pszFileType == NULL)
        return E_INVALIDARG;

    HRESULT hr;

    wchar_t szCLSID[MAX_PATH];
    StringFromGUID2(clsid, szCLSID, ARRAYSIZE(szCLSID));

    wchar_t szSubkey[MAX_PATH];

    // If pszFileType starts with '.', try to read the default value of the 
    // HKCR\<File Type> key which contains the ProgID to which the file type 
    // is linked.
    if (*pszFileType == L'.') {
        wchar_t szDefaultVal[260];
        hr = GetHKCRRegistryKeyAndValue(pszFileType, NULL, szDefaultVal, 
            sizeof(szDefaultVal));

        // If the key exists and its default value is not empty, use the 
        // ProgID as the file type.
        if (SUCCEEDED(hr) && szDefaultVal[0] != L'\0')
            pszFileType = szDefaultVal;
    }

    // Remove the HKCR\<File Type>\shellex\ContextMenuHandlers\{<CLSID>} key.
    hr = StringCchPrintf(
		szSubkey,
		ARRAYSIZE(szSubkey), 
        L"%s\\shellex\\ContextMenuHandlers\\%s",
		pszFileType,
		szCLSID
	);

    if (SUCCEEDED(hr))
        hr = HRESULT_FROM_WIN32(RegDeleteTree(HKEY_CLASSES_ROOT, szSubkey));

    return hr;
}

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: local function definitions
//

#pragma region Registry Helper Functions Definitions

HRESULT
SetHKCRRegistryKeyAndValue(
	PCWSTR pszSubKey,
	PCWSTR pszValueName, 
    PCWSTR pszData
	)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Creates the specified registry key.
	// If the key already exists, the function opens it. 
    hr = HRESULT_FROM_WIN32(
		RegCreateKeyEx(
			HKEY_CLASSES_ROOT,
			pszSubKey,
			0,
			NULL,
			REG_OPTION_NON_VOLATILE,
			KEY_WRITE,
			NULL,
			&hKey,
			NULL
		)
	);

    if (SUCCEEDED(hr)) {
        if (pszData != NULL) {
            // Set the specified value of the key.
            DWORD cbData = lstrlen(pszData) * sizeof(*pszData);
            
			hr = HRESULT_FROM_WIN32(
				RegSetValueEx(
					hKey,
					pszValueName,
					0, 
					REG_SZ,
					reinterpret_cast<const BYTE *>(pszData),
					cbData
				)
			);
        }

        RegCloseKey(hKey);
    }

    return hr;
}

HRESULT
GetHKCRRegistryKeyAndValue(
	PCWSTR pszSubKey,
	PCWSTR pszValueName, 
    PWSTR pszData,
	DWORD cbData
	)
{
    HRESULT hr;
    HKEY hKey = NULL;

    // Try to open the specified registry key. 
    hr = HRESULT_FROM_WIN32(
		RegOpenKeyEx(
			HKEY_CLASSES_ROOT,
			pszSubKey,
			0, 
			KEY_READ,
			&hKey
		)
	);

    if (SUCCEEDED(hr)) {
        // Get the data for the specified value name.
        hr = HRESULT_FROM_WIN32(
			RegQueryValueEx(
				hKey,
				pszValueName,
				NULL, 
				NULL,
				reinterpret_cast<LPBYTE>(pszData),
				&cbData
			)
		);

        RegCloseKey(hKey);
    }

    return hr;
}

#pragma endregion

//
//
//
