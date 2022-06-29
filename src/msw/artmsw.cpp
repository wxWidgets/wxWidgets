/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/artmsw.cpp
// Purpose:     stock wxArtProvider instance with native MSW stock icons
// Author:      Vaclav Slavik
// Modified by:
// Created:     2008-10-15
// Copyright:   (c) Vaclav Slavik, 2008
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ---------------------------------------------------------------------------
// headers
// ---------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/artprov.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif

#include "wx/image.h"
#include "wx/dynlib.h"
#include "wx/volume.h"
#include "wx/msw/private.h"
#include "wx/msw/wrapwin.h"
#include "wx/msw/wrapshl.h"

#ifdef SHGSI_ICON
    #define wxHAS_SHGetStockIconInfo
#endif

namespace
{

#ifdef SHDefExtractIcon
    #define MSW_SHDefExtractIcon SHDefExtractIcon
#else // !defined(SHDefExtractIcon)

// MinGW doesn't provide SHDefExtractIcon() up to at least the 5.3 version, so
// define it ourselves.
HRESULT
MSW_SHDefExtractIcon(LPCTSTR pszIconFile, int iIndex, UINT uFlags,
                     HICON *phiconLarge, HICON *phiconSmall, UINT nIconSize)
{
    typedef HRESULT
    (WINAPI *SHDefExtractIcon_t)(LPCTSTR, int, UINT, HICON*, HICON*, UINT);

    static SHDefExtractIcon_t s_SHDefExtractIcon = NULL;
    if ( !s_SHDefExtractIcon )
    {
        wxDynamicLibrary shell32(wxT("shell32.dll"));
        wxDL_INIT_FUNC_AW(s_, SHDefExtractIcon, shell32);

        if ( !s_SHDefExtractIcon )
            return E_FAIL;

        // Prevent the DLL from being unloaded while we use its function.
        // Normally it's not a problem as shell32.dll is always loaded anyhow.
        shell32.Detach();
    }

    return (*s_SHDefExtractIcon)(pszIconFile, iIndex, uFlags,
                                 phiconLarge, phiconSmall, nIconSize);
}

#endif // !defined(SHDefExtractIcon)

#ifdef wxHAS_SHGetStockIconInfo

SHSTOCKICONID MSWGetStockIconIdForArtProviderId(const wxArtID& art_id)
{
    // try to find an equivalent MSW stock icon id for wxArtID
    if ( art_id == wxART_ERROR)             return SIID_ERROR;
    else if ( art_id == wxART_QUESTION )    return SIID_HELP;
    else if ( art_id == wxART_WARNING )     return SIID_WARNING;
    else if ( art_id == wxART_INFORMATION ) return SIID_INFO;
    else if ( art_id == wxART_HELP )        return SIID_HELP;
    else if ( art_id == wxART_FOLDER )      return SIID_FOLDER;
    else if ( art_id == wxART_FOLDER_OPEN ) return SIID_FOLDEROPEN;
    else if ( art_id == wxART_DELETE )      return SIID_DELETE;
    else if ( art_id == wxART_FIND )        return SIID_FIND;
    else if ( art_id == wxART_HARDDISK )    return SIID_DRIVEFIXED;
    else if ( art_id == wxART_FLOPPY )      return SIID_DRIVE35;
    else if ( art_id == wxART_CDROM )       return SIID_DRIVECD;
    else if ( art_id == wxART_REMOVABLE )   return SIID_DRIVEREMOVE;
    else if ( art_id == wxART_PRINT )       return SIID_PRINTER;
    else if ( art_id == wxART_EXECUTABLE_FILE ) return SIID_APPLICATION;
    else if ( art_id == wxART_NORMAL_FILE ) return SIID_DOCNOASSOC;

    return SIID_INVALID;
};


// try to load SHGetStockIconInfo dynamically, so this code runs
// even on pre-Vista Windows versions
HRESULT
MSW_SHGetStockIconInfo(SHSTOCKICONID siid,
                       UINT uFlags,
                       SHSTOCKICONINFO *psii)
{
    typedef HRESULT (WINAPI *PSHGETSTOCKICONINFO)(SHSTOCKICONID, UINT, SHSTOCKICONINFO *);
    static PSHGETSTOCKICONINFO pSHGetStockIconInfo = (PSHGETSTOCKICONINFO)-1;

    if ( pSHGetStockIconInfo == (PSHGETSTOCKICONINFO)-1 )
    {
        wxDynamicLibrary shell32(wxT("shell32.dll"));

        pSHGetStockIconInfo = (PSHGETSTOCKICONINFO)shell32.RawGetSymbol( wxT("SHGetStockIconInfo") );
    }

    if ( !pSHGetStockIconInfo )
        return E_FAIL;

    return pSHGetStockIconInfo(siid, uFlags, psii);
}

#endif // #ifdef wxHAS_SHGetStockIconInfo

// Wrapper for SHDefExtractIcon().
wxBitmap
MSWGetBitmapFromIconLocation(const TCHAR* path, int index, const wxSize& size)
{
    HICON hIcon = NULL;
    if ( MSW_SHDefExtractIcon(path, index, 0, &hIcon, NULL, size.x) != S_OK )
        return wxNullBitmap;

    // Note that using "size.x" twice here is not a typo: normally size.y is
    // the same anyhow, of course, but if it isn't, the actual icon size would
    // be size.x in both directions as we only pass "x" to SHDefExtractIcon()
    // above.
    wxIcon icon;
    if ( !icon.InitFromHICON((WXHICON)hIcon, size.x, size.x) )
        return wxNullBitmap;

    return wxBitmap(icon);
}

#if !wxUSE_UNICODE

// SHSTOCKICONINFO always uses WCHAR, even in ANSI build, so we need to convert
// it to TCHAR, which is just CHAR in this case, used by the other functions.
// Provide an overload doing it as this keeps the code in the main function
// clean and this entire block (inside !wxUSE_UNICODE check) can be just
// removed when support for ANSI build is finally dropped.
wxBitmap
MSWGetBitmapFromIconLocation(const WCHAR* path, int index, const wxSize& size)
{
    return MSWGetBitmapFromIconLocation(wxString(path).mb_str(), index, size);
}

#endif // !wxUSE_UNICODE

wxBitmap
MSWGetBitmapForPath(const wxString& path, const wxSize& size, DWORD uFlags = 0)
{
    SHFILEINFO fi;
    wxZeroMemory(fi);

    uFlags |= SHGFI_USEFILEATTRIBUTES | SHGFI_ICONLOCATION;

    if ( !SHGetFileInfo(path.t_str(), FILE_ATTRIBUTE_DIRECTORY,
                        &fi, sizeof(SHFILEINFO), uFlags) )
       return wxNullBitmap;

    return MSWGetBitmapFromIconLocation(fi.szDisplayName, fi.iIcon, size);
}

#if wxUSE_FSVOLUME

wxBitmap
GetDriveBitmapForVolumeType(const wxFSVolumeKind& volKind, const wxSize& size)
{
    // get all volumes and try to find one with a matching type
    wxArrayString volumes = wxFSVolume::GetVolumes();
    for ( size_t i = 0; i < volumes.Count(); i++ )
    {
        wxFSVolume vol( volumes[i] );
        if ( vol.GetKind() == volKind )
        {
            return MSWGetBitmapForPath(volumes[i], size);
        }
    }

    return wxNullBitmap;
}

#endif // wxUSE_FSVOLUME

} // anonymous namespace

// ----------------------------------------------------------------------------
// wxWindowsArtProvider
// ----------------------------------------------------------------------------

class wxWindowsArtProvider : public wxArtProvider
{
protected:
    virtual wxBitmap CreateBitmap(const wxArtID& id, const wxArtClient& client,
                                  const wxSize& size) wxOVERRIDE;
};

wxBitmap wxWindowsArtProvider::CreateBitmap(const wxArtID& id,
                                            const wxArtClient& client,
                                            const wxSize& size)
{
    wxBitmap bitmap;

    const wxSize
        sizeNeeded = size.IsFullySpecified()
                        ? size
                        : wxArtProvider::GetNativeSizeHint(client);

#ifdef wxHAS_SHGetStockIconInfo
    // first try to use SHGetStockIconInfo, available only on Vista and higher
    SHSTOCKICONID stockIconId = MSWGetStockIconIdForArtProviderId( id );
    if ( stockIconId != SIID_INVALID )
    {
        WinStruct<SHSTOCKICONINFO> sii;

        UINT uFlags = SHGSI_ICONLOCATION | SHGSI_SYSICONINDEX;

        HRESULT res = MSW_SHGetStockIconInfo(stockIconId, uFlags, &sii);
        if ( res == S_OK )
        {
            bitmap = MSWGetBitmapFromIconLocation(sii.szPath, sii.iIcon,
                                                  sizeNeeded);
            if ( bitmap.IsOk() )
                return bitmap;
        }
    }
#endif // wxHAS_SHGetStockIconInfo


#if wxUSE_FSVOLUME
    // now try SHGetFileInfo
    wxFSVolumeKind volKind = wxFS_VOL_OTHER;
    if ( id == wxART_HARDDISK )
        volKind = wxFS_VOL_DISK;
    else if ( id == wxART_FLOPPY )
        volKind = wxFS_VOL_FLOPPY;
    else if ( id == wxART_CDROM )
        volKind = wxFS_VOL_CDROM;

    if ( volKind != wxFS_VOL_OTHER )
    {
        bitmap = GetDriveBitmapForVolumeType(volKind, sizeNeeded);
        if ( bitmap.IsOk() )
            return bitmap;
    }
#endif // wxUSE_FSVOLUME

    // notice that the directory used here doesn't need to exist
    if ( id == wxART_FOLDER )
        bitmap = MSWGetBitmapForPath("C:\\wxdummydir\\", sizeNeeded);
    else if ( id == wxART_FOLDER_OPEN )
        bitmap = MSWGetBitmapForPath("C:\\wxdummydir\\", sizeNeeded, SHGFI_OPENICON );

    if ( !bitmap.IsOk() )
    {
        // handle message box icons specially (wxIcon ctor treat these names
        // as special cases via wxICOResourceHandler::LoadIcon):
        const char *name = NULL;
        if ( id == wxART_ERROR )
            name = "wxICON_ERROR";
        else if ( id == wxART_INFORMATION )
            name = "wxICON_INFORMATION";
        else if ( id == wxART_WARNING )
            name = "wxICON_WARNING";
        else if ( id == wxART_QUESTION )
            name = "wxICON_QUESTION";

        if ( name )
            return wxIcon(name);
    }

    // for anything else, fall back to generic provider:
    return bitmap;
}

// ----------------------------------------------------------------------------
// wxArtProvider::InitNativeProvider()
// ----------------------------------------------------------------------------

/*static*/ void wxArtProvider::InitNativeProvider()
{
    PushBack(new wxWindowsArtProvider);
}

// ----------------------------------------------------------------------------
// wxArtProvider::GetNativeSizeHint()
// ----------------------------------------------------------------------------

/*static*/
wxSize wxArtProvider::GetNativeDIPSizeHint(const wxArtClient& client)
{
    if ( client == wxART_TOOLBAR )
    {
        return wxSize(24, 24);
    }
    else if ( client == wxART_MENU )
    {
        return wxSize(16, 16);
    }
    else if ( client == wxART_FRAME_ICON )
    {
        return wxSize(::GetSystemMetrics(SM_CXSMICON),
                      ::GetSystemMetrics(SM_CYSMICON));
    }
    else if ( client == wxART_CMN_DIALOG ||
              client == wxART_MESSAGE_BOX )
    {
        return wxSize(::GetSystemMetrics(SM_CXICON),
                      ::GetSystemMetrics(SM_CYICON));
    }
    else if (client == wxART_BUTTON)
    {
        return wxSize(16, 16);
    }
    else if (client == wxART_LIST)
    {
        return wxSize(16, 16);
    }

    return wxDefaultSize;
}
