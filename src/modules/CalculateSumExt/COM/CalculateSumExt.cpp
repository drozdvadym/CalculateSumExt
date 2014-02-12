//
// -*- Mode: c++; tab-width: 4; -*-
// -*- ex: ts=4 -*-
//

//
// ClassFactory.cpp    (Edited by: V. Drozd)
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

#include "CalculateSumExt.h"
#include "resource.h"

#include "CalculateSum/FileInfoLogger.h"

#include <strsafe.h>
#include <Shlwapi.h>
#pragma comment(lib, "shlwapi.lib")


extern HINSTANCE g_hInst;
extern long g_cDllRef;

#define IDM_DISPLAY 0  // The command's identifier offset

CalculateSumExt::CalculateSumExt(void)
    : m_cRef(1)
    , logFileName(L"file_inf.log")
    , m_pszMenuText(L"Calculate Information")
    , m_pszVerb("fileinfologger")
    , m_pwszVerb(L"fileinfologger")
    , m_pwszVerbCanonicalName(L"CalculateInformation")
    , m_pwszVerbHelpText(L"Save information about selected files")
{
    InterlockedIncrement(&g_cDllRef);

    // Load the bitmap for the menu item. 
    // If you want the menu item bitmap to be transparent, the color depth of 
    // the bitmap must not be greater than 8bpp.
    m_hMenuBmp = LoadImage(
        g_hInst, MAKEINTRESOURCE(IDB_OK), 
        IMAGE_BITMAP, 0, 0, LR_DEFAULTSIZE | LR_LOADTRANSPARENT
    );
}

CalculateSumExt::~CalculateSumExt(void)
{
    if (m_hMenuBmp) {
        DeleteObject(m_hMenuBmp);
        m_hMenuBmp = NULL;
    }

    InterlockedDecrement(&g_cDllRef);
}

void
CalculateSumExt::OnVerbDisplayFileNameIndepend(
    HWND hWnd,
    std::vector<std::wstring>& fileNames
    )
{
    std::wstring fullLogFileName = workDir + L'\\' + logFileName;

    std::wstring caption = L"CalculateSumExt";

    std::wstring  explanationMsg =
        L"This operation can take a lot of time. If you can wait some, press Ok";

    std::wstring  errMsg =
        L"An error occurred when calculated the files information.\n"
        L"Sorry :(\n"
        L"More detailed information will be in next version of extension\n";

    std::wstring okMsg = 
        L"You can see information about selected files in:\n" + fullLogFileName;

    bool status = true;

    try {
        int choise = MessageBox(
            hWnd, explanationMsg.c_str(), caption.c_str(), MB_OKCANCEL
        );

        if (IDOK != choise) {
            this->Release();
            return;
            /*NOTREACHED*/
        }

        FileInfoLogger fil(fileNames, fullLogFileName);
        status = fil.process();
    }
    catch (...) {
        status = false;
    }
     
    if (!status)
        MessageBox(hWnd, errMsg.c_str(), caption.c_str(), MB_OK | MB_SYSTEMMODAL | MB_ICONERROR);
    else
        MessageBox(hWnd, okMsg.c_str(), caption.c_str(), MB_OK | MB_SYSTEMMODAL | MB_ICONINFORMATION);
    
    this->Release();
}


#pragma region IUnknown

// Query to the interface the component supported.
IFACEMETHODIMP CalculateSumExt::QueryInterface(REFIID riid, void **ppv)
{
    static const QITAB qit[] = {
        QITABENT(CalculateSumExt, IContextMenu),
        QITABENT(CalculateSumExt, IShellExtInit),
        { 0 },
    };

    return QISearch(this, qit, riid, ppv);
}

// Increase the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) CalculateSumExt::AddRef()
{
    return InterlockedIncrement(&m_cRef);
}

// Decrease the reference count for an interface on an object.
IFACEMETHODIMP_(ULONG) CalculateSumExt::Release()
{
    ULONG cRef = InterlockedDecrement(&m_cRef);
    if (0 == cRef) {
        delete this;
    }

    return cRef;
}

#pragma endregion


#pragma region IShellExtInit

// Initialize the context menu handler.
IFACEMETHODIMP
CalculateSumExt::Initialize(
    LPCITEMIDLIST pidlFolder,
    LPDATAOBJECT pDataObj,
    HKEY hKeyProgID
    )
{
    if (NULL == pDataObj)
        return E_INVALIDARG;

    HRESULT hr = E_FAIL;

    FORMATETC fe = {CF_HDROP, NULL, DVASPECT_CONTENT, -1, TYMED_HGLOBAL};
    STGMEDIUM stm;

    wchar_t _curFileName[MAX_PATH];
    wchar_t _workDirName[MAX_PATH];
    
    // The pDataObj pointer contains the objects being acted upon. In this 
    // example, we get an HDROP handle for enumerating the selected files and 
    // folders.
    do {
        if (!SUCCEEDED(pDataObj->GetData(&fe, &stm)))
            break;

        // Get an HDROP handle.
        HDROP hDrop = static_cast<HDROP>(GlobalLock(stm.hGlobal));
        if (NULL == hDrop)
            break;

        // Determine how many files are involved in this operation.
        UINT nFiles = DragQueryFile(hDrop, 0xFFFFFFFF, NULL, 0);
        
        for (UINT uFile = 0; uFile < nFiles; uFile++ ) {
            // Get the next filename.
            UINT dres = DragQueryFileW(
                hDrop, uFile, _curFileName, ARRAYSIZE(_curFileName)
            );
            
            if (!dres) continue;

            // Save workDir name
            if (!fileNames.size()) {
                StringCbCopyW(_workDirName, ARRAYSIZE(_workDirName), _curFileName);
                PathRemoveFileSpec(_workDirName);
                workDir = _workDirName;
            }
            
            //Save current file name in list
            fileNames.push_back(_curFileName);
        }

        if (fileNames.size() > 0)
            hr = S_OK;
        
        GlobalUnlock(stm.hGlobal);

        ReleaseStgMedium(&stm);

    } while (0);

    return hr;
}

#pragma endregion


#pragma region IContextMenu

//
// FUNCTION: FileContextMenuExt::QueryContextMenu
//
// PURPOSE: The Shell calls IContextMenu::QueryContextMenu to allow the 
//          context menu handler to add its menu items to the menu. It 
//          passes in the HMENU handle in the hmenu parameter. The 
//          indexMenu parameter is set to the index to be used for the 
//          first menu item that is to be added.
//

IFACEMETHODIMP
CalculateSumExt::QueryContextMenu(
    HMENU hMenu,
    UINT indexMenu,
    UINT idCmdFirst,
    UINT idCmdLast,
    UINT uFlags
    )
{
    // If uFlags include CMF_DEFAULTONLY then we should not do anything.
    if (CMF_DEFAULTONLY & uFlags)
        return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(0));

    MENUITEMINFO mii = { sizeof(mii) };
    mii.fMask = MIIM_BITMAP | MIIM_STRING | MIIM_FTYPE | MIIM_ID | MIIM_STATE;
    mii.wID = idCmdFirst + IDM_DISPLAY;
    mii.fType = MFT_STRING;
    mii.dwTypeData = m_pszMenuText;
    mii.fState = MFS_ENABLED;
    mii.hbmpItem = static_cast<HBITMAP>(m_hMenuBmp);
    if (!InsertMenuItem(hMenu, indexMenu, TRUE, &mii)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Add a separator.
    MENUITEMINFO sep = { sizeof(sep) };
    sep.fMask = MIIM_TYPE;
    sep.fType = MFT_SEPARATOR;
    if (!InsertMenuItem(hMenu, indexMenu + 1, TRUE, &sep)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }

    // Return an HRESULT value with the severity set to SEVERITY_SUCCESS. 
    // Set the code value to the offset of the largest command identifier 
    // that was assigned, plus one (1).
    return MAKE_HRESULT(SEVERITY_SUCCESS, 0, USHORT(IDM_DISPLAY + 1));
}


//
// FUNCTION: FileContextMenuExt::InvokeCommand
//
// PURPOSE: This method is called when a user clicks a menu item to tell 
//          the handler to run the associated command. The lpcmi parameter 
//          points to a structure that contains the needed information.
//

IFACEMETHODIMP
CalculateSumExt::InvokeCommand(LPCMINVOKECOMMANDINFO pici)
{
    BOOL fUnicode = FALSE;
    BOOL isRun = FALSE;

    // Determine which structure is being passed in, CMINVOKECOMMANDINFO or 
    // CMINVOKECOMMANDINFOEX based on the cbSize member of lpcmi. Although 
    // the lpcmi parameter is declared in Shlobj.h as a CMINVOKECOMMANDINFO 
    // structure, in practice it often points to a CMINVOKECOMMANDINFOEX 
    // structure. This struct is an extended version of CMINVOKECOMMANDINFO 
    // and has additional members that allow Unicode strings to be passed.
    if (pici->cbSize == sizeof(CMINVOKECOMMANDINFOEX)) {
        if (pici->fMask & CMIC_MASK_UNICODE)
            fUnicode = TRUE;
    }

    // Determines whether the command is identified by its offset or verb.
    // There are two ways to identify commands:
    // 
    //   1) The command's verb string 
    //   2) The command's identifier offset
    // 
    // If the high-order word of lpcmi->lpVerb (for the ANSI case) or 
    // lpcmi->lpVerbW (for the Unicode case) is nonzero, lpVerb or lpVerbW 
    // holds a verb string. If the high-order word is zero, the command 
    // offset is in the low-order word of lpcmi->lpVerb.

    // For the ANSI case, if the high-order word is not zero, the command's 
    // verb string is in lpcmi->lpVerb. 
    if (!fUnicode && HIWORD(pici->lpVerb)) {
        // Is the verb supported by this context menu extension?
        if (StrCmpIA(pici->lpVerb, m_pszVerb) == 0) {
            isRun = TRUE;
        }
        else {
            // If the verb is not recognized by the context menu handler, it 
            // must return E_FAIL to allow it to be passed on to the other 
            // context menu handlers that might implement that verb.
            return E_FAIL;
        }
    }

    // For the Unicode case, if the high-order word is not zero, the 
    // command's verb string is in lpcmi->lpVerbW. 
    else if (fUnicode && HIWORD(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW)) {
        // Is the verb supported by this context menu extension?
        if (StrCmpIW(((CMINVOKECOMMANDINFOEX*)pici)->lpVerbW, m_pwszVerb) == 0)
            isRun = TRUE;
        else
            return E_FAIL;
    }

    // If the command cannot be identified through the verb string, then 
    // check the identifier offset.
    else {
        // Is the command identifier offset supported by this context menu 
        // extension?
        if (LOWORD(pici->lpVerb) == IDM_DISPLAY)
            isRun = TRUE;
        else
            return E_FAIL;
    }


    if (isRun) {

        HWND hwnd = NULL;

        this->AddRef();
        std::thread(
            &CalculateSumExt::OnVerbDisplayFileNameIndepend,
            std::ref(*this), std::ref(hwnd), std::ref(fileNames)
        ).detach();

    }

    return S_OK;
}

//
// FUNCTION: CFileContextMenuExt::GetCommandString
//
// PURPOSE: If a user highlights one of the items added by a context menu 
//          handler, the handler's IContextMenu::GetCommandString method is 
//          called to request a Help text string that will be displayed on 
//          the Windows Explorer status bar. This method can also be called 
//          to request the verb string that is assigned to a command. 
//          Either ANSI or Unicode verb strings can be requested. This 
//          example only implements support for the Unicode values of 
//          uFlags, because only those have been used in Windows Explorer 
//          since Windows 2000.
//

IFACEMETHODIMP
CalculateSumExt::GetCommandString(
    UINT_PTR idCommand, 
    UINT uFlags,
    UINT *pwReserved,
    LPSTR pszName,
    UINT cchMax
    )
{
    HRESULT hr = E_INVALIDARG;

    if (idCommand != IDM_DISPLAY) {
        return hr;
        /*NOTREACHED*/
    }

    switch (uFlags) {
        case GCS_HELPTEXTW:
            // Only useful for pre-Vista versions of Windows that have a 
            // Status bar.
            hr = StringCchCopy(
                reinterpret_cast<PWSTR>(pszName),
                cchMax, m_pwszVerbHelpText
            );
            break;

        case GCS_VERBW:
            // GCS_VERBW is an optional feature that enables a caller to 
            // discover the canonical name for the verb passed in through 
            // idCommand.
            hr = StringCchCopy(
                reinterpret_cast<PWSTR>(pszName),
                cchMax, m_pwszVerbCanonicalName
            );
            break;

        default:
            hr = S_OK;
    }

    // If the command (idCommand) is not supported by this context menu 
    // extension handler, return E_INVALIDARG.

    return hr;
}

#pragma endregion

//
//
//
