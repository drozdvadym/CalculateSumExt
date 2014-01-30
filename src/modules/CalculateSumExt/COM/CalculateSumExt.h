//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// CalculateSumExt.h	(Edited by: V. Drozd)
// src/COM/CalculateSumExt.h
//

//
// The code sample demonstrates creating a Shell context menu handler with C++. 
//
// This context menu handler adds the menu item "Acumulate information"
// to the context menu when you right-click a file (or set of files) in the Windows Explorer. 
// Clicking the menu item save information about the file (files) into .log file
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

#include <windows.h>
#include <shlobj.h>
	//IShellExtInit and IContextMenu

#include <string>
#include <vector>

using std::wstring;
using std::vector;

///////////////////////////////////////////////////////////////////////////////
// %% BeginSection: declarations
//


class CalculateSumExt : public IShellExtInit, public IContextMenu
{
public:
    // IUnknown
    IFACEMETHODIMP QueryInterface(REFIID riid, void **ppv);
    IFACEMETHODIMP_(ULONG) AddRef();
    IFACEMETHODIMP_(ULONG) Release();

    // IShellExtInit
    IFACEMETHODIMP Initialize(LPCITEMIDLIST pidlFolder, LPDATAOBJECT pDataObj, HKEY hKeyProgID);

    // IContextMenu
    IFACEMETHODIMP QueryContextMenu(HMENU hMenu, UINT indexMenu, UINT idCmdFirst, UINT idCmdLast, UINT uFlags);
    IFACEMETHODIMP InvokeCommand(LPCMINVOKECOMMANDINFO pici);
    IFACEMETHODIMP GetCommandString(UINT_PTR idCommand, UINT uFlags, UINT *pwReserved, LPSTR pszName, UINT cchMax);
	
	CalculateSumExt(void);

protected:
	~CalculateSumExt(void);

private:
    // Reference count of component.
    long m_cRef;

    // The name of the selected file.
    wchar_t m_szSelectedFile[MAX_PATH];

	wchar_t workDir_[MAX_PATH];
	const wchar_t *log_file_name;
	
    // The names of the selected files
	vector<wstring> fileNames_;
	
    // The method that handles the "display" verb.
    void OnVerbDisplayFileName(HWND hWnd);

    PWSTR m_pszMenuText;
    HANDLE m_hMenuBmp;
    PCSTR m_pszVerb;
    PCWSTR m_pwszVerb;
    PCWSTR m_pwszVerbCanonicalName;
    PCWSTR m_pwszVerbHelpText;
};

//
//
//
