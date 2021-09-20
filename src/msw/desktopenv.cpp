///////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/desktopenv.cpp
// Purpose:     implementation of wxDesktopEnvBase
// Author:      Igor Korot
// Created:     2021-09-05
// Copyright:   (c) 2021 wxWidgets development team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "shlobj.h"
#include "wx/log.h"
#include "wx/desktopenv.h"
#include "wx/msw/private.h"

// ============================================================================
// implementation
// ============================================================================

/* static */
bool wxDesktopEnv::MoveToRecycleBin(const wxString &path)
{
    bool res = wxDesktopEnvBase::MoveToRecycleBin( path );
    if( res )
    {
        SHFILEOPSTRUCT fileOp;
        wxZeroMemory( fileOp );
        fileOp.hwnd = NULL;
        fileOp.wFunc = FO_DELETE;
        wxString temp = path + '\0';
        fileOp.pFrom = temp.t_str();
        fileOp.pTo = NULL;
        fileOp.fFlags = FOF_ALLOWUNDO | FOF_NOERRORUI | FOF_NOCONFIRMATION | FOF_SILENT;
        res = SHFileOperation( &fileOp );
        if( res != 0 )
            wxLogError( _( "Failed to move '%s' to Recycle Bin" ), path );
        else
            res = true;
    }
    return res;
}

/*static*/
bool wxDesktopEnv::GetFilesInRecycleBin(std::vector<wxString> &paths)
{
    bool res = false;
    LPITEMIDLIST pidl = NULL;
    LPITEMIDLIST pidlRecycleBin	= NULL;
    IShellFolder2 *m_pFolder2;
    LPSHELLFOLDER pDesktop = NULL;
    STRRET strRet;
    TCHAR szPath[MAX_PATH], szTemp[MAX_PATH];
    wxString lpszName;
    LPENUMIDLIST penumFiles;
    SHELLDETAILS sd;
    SHFILEINFO fi;

    if( ( SUCCEEDED( SHGetDesktopFolder( &pDesktop ) ) ) &&
        ( SUCCEEDED( SHGetFolderLocation( NULL, CSIDL_BITBUCKET, NULL, NULL, &pidlRecycleBin ) ) ) )
    {
        if( SUCCEEDED( pDesktop->BindToObject( pidlRecycleBin, NULL, IID_IShellFolder2, (LPVOID *)&m_pFolder2 ) ) )
        {
            if( S_OK == pDesktop->GetDisplayNameOf( pidlRecycleBin, SHGDN_NORMAL, &strRet ) )
            {
                ZeroMemory( szPath, sizeof( szPath ) );
                switch( strRet.uType )
                {
                case STRRET_CSTR:
                    _tcscpy( szPath, strRet.pOleStr );
                    break;
                case STRRET_OFFSET:
                    break;
                case STRRET_WSTR:
                    memcpy( szPath, strRet.pOleStr, MAX_PATH );
                    CoTaskMemFree( strRet.pOleStr );
                    break;
                }
            }
            if( SUCCEEDED( m_pFolder2->EnumObjects( NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS| SHCONTF_INCLUDEHIDDEN, &penumFiles ) ) )
            {
                int iSubItem = 0;
                while( penumFiles->Next( 1, &pidl, NULL ) != S_FALSE )
                {
                    ::ZeroMemory( &fi, sizeof( fi ) );
                    if( SUCCEEDED( SHGetFileInfo( (LPCWSTR) pidl, 0, &fi, sizeof( fi ), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_PIDL ) ) )
                    {
                        if( SUCCEEDED( m_pFolder2->GetDetailsOf( pidl, iSubItem, &sd ) ) )
                        {
                            switch( sd.str.uType )
                            {
                            case STRRET_CSTR:
                                _tcscpy( szTemp, sd.str.pOleStr );
                                break;
                            case STRRET_OFFSET:
                                break;
                            case STRRET_WSTR:
                                memcpy( szPath, sd.str.pOleStr, MAX_PATH );
                                CoTaskMemFree( sd.str.pOleStr );
                                break;
                            }
                            wxString tmp( szPath );
                            paths.push_back( tmp );
                            res = true;
                        }
                    }
                }
            }
        }
    }
    return res;
}

/* static */
bool wxDesktopEnv::RestoreFromRecycleBin(const wxString &path)
{
    bool res = false;
    LPITEMIDLIST pidl = NULL;
    LPITEMIDLIST pidlRecycleBin	= NULL;
    IShellFolder2 *m_pFolder2;
    LPSHELLFOLDER pDesktop = NULL;
    STRRET strRet;
    TCHAR szPath[MAX_PATH], szTemp[MAX_PATH];
    wxString lpszName;
    LPENUMIDLIST penumFiles;
    SHELLDETAILS sd;
    SHFILEINFO fi;
    LPCONTEXTMENU pCtxMenu = NULL;

    if( ( SUCCEEDED( SHGetDesktopFolder( &pDesktop ) ) ) &&
        ( SUCCEEDED( SHGetFolderLocation( NULL, CSIDL_BITBUCKET, NULL, NULL, &pidlRecycleBin ) ) ) )
    {
        if( SUCCEEDED( pDesktop->BindToObject( pidlRecycleBin, NULL, IID_IShellFolder2, (LPVOID *)&m_pFolder2 ) ) )
        {
            if( S_OK == pDesktop->GetDisplayNameOf( pidlRecycleBin, SHGDN_NORMAL, &strRet ) )
            {
                ZeroMemory( szPath, sizeof( szPath ) );
                switch( strRet.uType )
                {
                case STRRET_CSTR:
                    _tcscpy( szPath, strRet.pOleStr );
                    break;
                case STRRET_OFFSET:
                    break;
                case STRRET_WSTR:
                    memcpy( szPath, strRet.pOleStr, MAX_PATH );
                    CoTaskMemFree( strRet.pOleStr );
                    break;
                }
            }
            if( SUCCEEDED( m_pFolder2->EnumObjects( NULL, SHCONTF_FOLDERS | SHCONTF_NONFOLDERS| SHCONTF_INCLUDEHIDDEN, &penumFiles ) ) )
            {
                int iSubItem = 0;
                while( penumFiles->Next( 1, &pidl, NULL ) != S_FALSE )
                {
                    ::ZeroMemory( &fi, sizeof( fi ) );
                    if( SUCCEEDED( SHGetFileInfo( (LPCWSTR) pidl, 0, &fi, sizeof( fi ), SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_PIDL ) ) )
                    {
                        if( SUCCEEDED( m_pFolder2->GetDetailsOf( pidl, iSubItem, &sd ) ) )
                        {
                            switch( sd.str.uType )
                            {
                            case STRRET_CSTR:
                                _tcscpy( szTemp, sd.str.pOleStr );
                                break;
                            case STRRET_OFFSET:
                                break;
                            case STRRET_WSTR:
                                memcpy( szPath, sd.str.pOleStr, MAX_PATH );
                                CoTaskMemFree( sd.str.pOleStr );
                                break;
                            }
                            wxString tmp( szPath );
                            if( path == tmp )
                            {
                                if( SUCCEEDED( m_pFolder2->GetUIObjectOf( NULL, 1, (LPCITEMIDLIST *) &pidl, IID_IContextMenu, NULL, (LPVOID *) &pCtxMenu ) ) )
                                {
                                    UINT uiID = UINT (-1);
                                    UINT uiCommand = 0;
                                    UINT uiMenuFirst = 1;
                                    UINT uiMenuLast = 0x00007FFF;
                                    HMENU hmenuCtx;
                                    int iMenuPos = 0;
                                    int iMenuMax = 0;
                                    TCHAR szMenuItem[128];
                                    TCHAR szTrace[512];
                                    char verb[MAX_PATH] ;

                                    hmenuCtx = CreatePopupMenu();
                                    if( SUCCEEDED( pCtxMenu->QueryContextMenu( hmenuCtx, 0, uiMenuFirst, uiMenuLast, CMF_NORMAL ) ) )
                                    {
                                        iMenuMax = GetMenuItemCount( hmenuCtx );
                                        bool found = false;
                                        for( iMenuPos = 0 ; iMenuPos < iMenuMax && !found; iMenuPos++ )
                                        {
                                            GetMenuString( hmenuCtx, iMenuPos, szMenuItem, sizeof( szMenuItem ), MF_BYPOSITION );
                                            uiID = GetMenuItemID( hmenuCtx, iMenuPos );
                                            if( ( uiID == -1 ) || ( uiID == 0 ) )
                                            {
                                            }
                                            else
                                            {
                                                HRESULT hr = pCtxMenu->GetCommandString( uiID - 1, GCS_VERBA, NULL, verb, sizeof( verb ) );
                                                if( FAILED( hr ) )
                                                {
                                                    verb[0] = TCHAR( '\0' );
                                                }
                                                else
                                                {
                                                    wxString command( verb );
                                                    if( command == "undelete" )
                                                    {
                                                        found = true;
                                                        uiCommand = uiID - 1;
                                                        CMINVOKECOMMANDINFO cmi;
                                                        ::ZeroMemory( &cmi, sizeof( CMINVOKECOMMANDINFO ) );
                                                        cmi.cbSize			= sizeof( CMINVOKECOMMANDINFO );
                                                        cmi.fMask			= CMIC_MASK_FLAG_NO_UI;
                                                        cmi.hwnd			= NULL;
                                                        cmi.lpParameters	= NULL;
                                                        cmi.lpDirectory		= NULL;
                                                        cmi.lpVerb			= (LPSTR) MAKEINTRESOURCE( uiCommand );
                                                        cmi.nShow			= SW_SHOWNORMAL;
                                                        cmi.dwHotKey		= NULL;
                                                        cmi.hIcon			= NULL;
                                                        hr = pCtxMenu->InvokeCommand( &cmi );
                                                        if( SUCCEEDED( hr ) )
                                                        {
                                                            res = TRUE;
                                                        }
                                                    }
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    if( pidlRecycleBin )
    {
        CoTaskMemFree( pidlRecycleBin );
    }
    if( pDesktop )
    {
        pDesktop->Release();
    }
    return res;
}
