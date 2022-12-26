/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/dirdlg.cpp
// Purpose:     wxDirDialog
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dirdlg.h"
#include "wx/modalhook.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/log.h"
    #include "wx/app.h"     // for GetComCtl32Version()
#endif

#include "wx/msw/private/filedialog.h"

#if wxUSE_IFILEOPENDIALOG

#include <initguid.h>

#include "wx/msw/private/cotaskmemptr.h"
#include "wx/dynlib.h"

// IFileDialog related declarations missing from some compilers headers.

#if defined(__VISUALC__)
// Always define this GUID, we might still not have it in the actual uuid.lib,
// even when IShellItem interface is defined in the headers.
// This happens with at least VC7 used with its original (i.e. not updated) SDK.
// clang complains about multiple definitions, so only define it unconditionally
// when using a Visual C compiler.
DEFINE_GUID(IID_IShellItem,
    0x43826D1E, 0xE718, 0x42EE, 0xBC, 0x55, 0xA1, 0xE2, 0x61, 0xC3, 0x7B, 0xFE);
#endif

#endif // wxUSE_IFILEOPENDIALOG

// ----------------------------------------------------------------------------
// private functions prototypes
// ----------------------------------------------------------------------------

#if wxUSE_IFILEOPENDIALOG

namespace
{

// helper functions for wxDirDialog::ShowIFileOpenDialog()
bool GetPathsFromIFileOpenDialog(IFileOpenDialog* fileDialog, wxArrayString& paths);
bool GetPathFromIFileDialog(IFileDialog* fileDialog, wxString& path);

} // anonymous namespace

#endif // #if wxUSE_IFILEOPENDIALOG

// Note that parts of this file related to IFileDialog are still compiled even
// when wxUSE_DIRDLG == 0 because they're used by wxUSE_FILEDLG too.
#if wxUSE_DIRDLG

// callback used in wxDirDialog::ShowSHBrowseForFolder()
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp,
                                       LPARAM pData);

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#ifndef BIF_NONEWFOLDERBUTTON
    #define BIF_NONEWFOLDERBUTTON  0x0200
#endif

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxDirDialog, wxDialog);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxDirDialog
// ----------------------------------------------------------------------------

wxDirDialog::wxDirDialog(wxWindow *parent,
                         const wxString& message,
                         const wxString& defaultPath,
                         long style,
                         const wxPoint& WXUNUSED(pos),
                         const wxSize& WXUNUSED(size),
                         const wxString& WXUNUSED(name))
{
    m_message = message;
    m_parent = parent;

    wxASSERT_MSG( !( (style & wxDD_MULTIPLE) && (style & wxDD_CHANGE_DIR) ),
                  "wxDD_CHANGE_DIR can't be used together with wxDD_MULTIPLE" );

    SetWindowStyle(style);
    SetPath(defaultPath);
}

void wxDirDialog::SetPath(const wxString& path)
{
    m_path = path;

    // SHBrowseForFolder doesn't like '/'s nor the trailing backslashes
    m_path.Replace(wxT("/"), wxT("\\"));

    while ( !m_path.empty() && (*(m_path.end() - 1) == wxT('\\')) )
    {
        m_path.erase(m_path.length() - 1);
    }

    // but the root drive should have a trailing slash (again, this is just
    // the way the native dialog works)
    if ( !m_path.empty() && (*(m_path.end() - 1) == wxT(':')) )
    {
        m_path += wxT('\\');
    }
}

int wxDirDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindow* const parent = GetParentForModalDialog();
    WXHWND hWndParent = parent ? GetHwndOf(parent) : nullptr;

    wxWindowDisabler disableOthers(this, parent);

    m_paths.clear();

    // Use IFileDialog under new enough Windows, it's more user-friendly.
    int rc;
#if wxUSE_IFILEOPENDIALOG
    rc = ShowIFileOpenDialog(hWndParent);

    if ( rc == wxID_NONE )
#endif // wxUSE_IFILEOPENDIALOG
    {
        rc = ShowSHBrowseForFolder(hWndParent);
    }

    // change current working directory if asked so
    if ( rc == wxID_OK && HasFlag(wxDD_CHANGE_DIR) )
        wxSetWorkingDirectory(m_path);

    return rc;
}

int wxDirDialog::ShowSHBrowseForFolder(WXHWND owner)
{
    BROWSEINFO bi;
    bi.hwndOwner      = owner;
    bi.pidlRoot       = nullptr;
    bi.pszDisplayName = nullptr;
    bi.lpszTitle      = m_message.c_str();
    bi.ulFlags        = BIF_RETURNONLYFSDIRS | BIF_STATUSTEXT;
    bi.lpfn           = BrowseCallbackProc;
    bi.lParam         = wxMSW_CONV_LPARAM(m_path); // param for the callback

    static const int verComCtl32 = wxApp::GetComCtl32Version();

    // we always add the edit box (it doesn't hurt anybody, does it?)
    bi.ulFlags |= BIF_EDITBOX;

    // to have the "New Folder" button we must use the "new" dialog style which
    // is also the only way to have a resizable dialog
    //
    const bool needNewDir = !HasFlag(wxDD_DIR_MUST_EXIST);
    if ( needNewDir || HasFlag(wxRESIZE_BORDER) )
    {
        if (needNewDir)
        {
            bi.ulFlags |= BIF_NEWDIALOGSTYLE;
        }
        else
        {
            // Versions < 600 doesn't support BIF_NONEWFOLDERBUTTON
            // The only way to get rid of the Make New Folder button is use
            // the old dialog style which doesn't have the button thus we
            // simply don't set the New Dialog Style for such comctl versions.
            if (verComCtl32 >= 600)
            {
                bi.ulFlags |= BIF_NEWDIALOGSTYLE;
                bi.ulFlags |= BIF_NONEWFOLDERBUTTON;
            }
        }
    }

    // do show the dialog
    wxItemIdList pidl(SHBrowseForFolder(&bi));

    wxItemIdList::Free(const_cast<LPITEMIDLIST>(bi.pidlRoot));

    if ( !pidl )
    {
        // Cancel button pressed
        return wxID_CANCEL;
    }

    m_path = pidl.GetPath();

    return m_path.empty() ? wxID_CANCEL : wxID_OK;
}

// Function for obtaining folder name using IFileDialog.
//
// Returns wxID_OK on success, wxID_CANCEL if cancelled by user or wxID_NONE if
// an error occurred and we should fall back onto the old dialog.
#if wxUSE_IFILEOPENDIALOG

int wxDirDialog::ShowIFileOpenDialog(WXHWND owner)
{
    wxMSWImpl::wxIFileDialog fileDialog(CLSID_FileOpenDialog);
    if ( !fileDialog.IsOk() )
        return wxID_NONE;

    fileDialog.SetTitle(m_message);
    if ( !m_path.empty() )
        fileDialog.SetInitialPath(m_path);

    // We currently always use FOS_NOCHANGEDIR even if wxDD_CHANGE_DIR was
    // specified because we change the directory ourselves in this case.
    int options = FOS_PICKFOLDERS | FOS_NOCHANGEDIR;
    if ( HasFlag(wxDD_MULTIPLE) )
        options |= FOS_ALLOWMULTISELECT;
    if ( HasFlag(wxDD_SHOW_HIDDEN) )
        options |= FOS_FORCESHOWHIDDEN;

    return fileDialog.Show(owner, options, &m_paths, &m_path);
}

#endif // wxUSE_IFILEOPENDIALOG

#endif // wxUSE_DIRDLG

#if wxUSE_IFILEOPENDIALOG

// ----------------------------------------------------------------------------
// Helper functions used by wxDirDialog and wxFileDialog.
// ----------------------------------------------------------------------------

namespace wxMSWImpl
{

wxIFileDialog::wxIFileDialog(const CLSID& clsid)
{
    HRESULT hr = ::CoCreateInstance
                 (
                    clsid,
                    nullptr, // no outer IUnknown
                    CLSCTX_INPROC_SERVER,
                    wxIID_PPV_ARGS(IFileDialog, &m_fileDialog)
                 );
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("CoCreateInstance(CLSID_FileOpenDialog)"), hr);
    }
}

int wxIFileDialog::Show(HWND owner, int options,
                        wxArrayString* pathsOut, wxString* pathOut)
{
    wxCHECK_MSG( m_fileDialog, wxID_NONE, wxS("shouldn't be called") );

    HRESULT hr;

    // allow to select only a file system object
    hr = m_fileDialog->SetOptions(options | FOS_FORCEFILESYSTEM);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IFileDialog::SetOptions"), hr);
        return false;
    }

    hr = m_fileDialog->Show(owner);
    if ( FAILED(hr) )
    {
        if ( hr == HRESULT_FROM_WIN32(ERROR_CANCELLED) )
        {
            return wxID_CANCEL; // the user cancelled the dialog
        }
        else
        {
            wxLogApiError(wxS("IFileDialog::Show"), hr);
        }
    }
    else if ( options & FOS_ALLOWMULTISELECT )
    {
        wxCOMPtr<IFileOpenDialog> fileOpenDialog;
        hr = m_fileDialog->QueryInterface(wxIID_PPV_ARGS(IFileOpenDialog, &fileOpenDialog));
        if ( SUCCEEDED(hr) )
        {
            if ( GetPathsFromIFileOpenDialog(fileOpenDialog, *pathsOut) )
                return wxID_OK;
        }
        else
        {
            wxLogApiError(wxS("IFileDialog::QI(IFileOpenDialog)"), hr);
        }
    }
    else // Single selection only, path output parameter must be non-null.
    {
        if ( GetPathFromIFileDialog(m_fileDialog, *pathOut) )
            return wxID_OK;
    }

    // Failed to show the dialog or obtain the selected folders(s)
    wxLogSysError(_("Couldn't obtain folder name"), hr);
    return wxID_CANCEL;
}

void wxIFileDialog::SetTitle(const wxString& message)
{
    HRESULT hr = m_fileDialog->SetTitle(message.wc_str());
    if ( FAILED(hr) )
    {
        // This error is not serious, let's just log it and continue even
        // without the title set.
        wxLogApiError(wxS("IFileDialog::SetTitle"), hr);
    }
}

HRESULT InitShellItemFromPath(wxCOMPtr<IShellItem>& item, const wxString& path)
{
    HRESULT hr;

    // SHCreateItemFromParsingName() doesn't support slashes, so if the path
    // uses them, replace them with the backslashes.
    wxString pathBS;
    const wxString* pathWithoutSlashes;
    if ( path.find('/') != wxString::npos )
    {
        pathBS = path;
        pathBS.Replace("/", "\\", true);

        pathWithoutSlashes = &pathBS;
    }
    else // Just use the original path without copying.
    {
        pathWithoutSlashes = &path;
    }

    hr = ::SHCreateItemFromParsingName
         (
            pathWithoutSlashes->wc_str(),
            nullptr,
            wxIID_PPV_ARGS(IShellItem, &item)
         );
    if ( FAILED(hr) )
    {
        wxLogApiError
        (
            wxString::Format(wxS("SHCreateItemFromParsingName(\"%s\")"),
                             *pathWithoutSlashes),
            hr
        );
    }

    return hr;
}

void wxIFileDialog::SetInitialPath(const wxString& defaultPath)
{
    wxCOMPtr<IShellItem> folder;

    HRESULT hr = InitShellItemFromPath(folder, defaultPath);

    // Failing to parse the folder name is not really an error, e.g. it might
    // not exist, so we'll just ignore the initial directory in this case.
    if ( SUCCEEDED(hr) )
    {
        hr = m_fileDialog->SetFolder(folder);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::SetFolder"), hr);
    }
}

void wxIFileDialog::AddPlace(const wxString& path, FDAP fdap)
{
    wxCOMPtr<IShellItem> place;

    HRESULT hr = InitShellItemFromPath(place, path);

    // Don't bother with doing anything else if we couldn't parse the path
    // (debug message about failing to do it was already logged).
    if ( FAILED(hr) )
        return;

    hr = m_fileDialog->AddPlace(place, fdap);
    if ( FAILED(hr) )
    {
        wxLogApiError
        (
            wxString::Format(wxS("IFileDialog::AddPlace(\"%s\")"), path), hr
        );
    }
}

} // namespace wxMSWImpl

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

namespace
{

// helper function for wxDirDialog::ShowIFileOpenDialog()
bool GetPathsFromIFileOpenDialog(IFileOpenDialog* fileDialog, wxArrayString& paths)
{
    HRESULT hr = S_OK;
    wxString path;
    wxArrayString tempPaths;

    wxCOMPtr<IShellItemArray> itemArray;

    hr = fileDialog->GetResults(&itemArray);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IShellItemArray::GetResults"), hr);
        return false;
    }

    DWORD count = 0;

    hr = itemArray->GetCount(&count);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IShellItemArray::GetCount"), hr);
        return false;
    }

    for ( DWORD i = 0; i < count; ++i )
    {
        wxCOMPtr<IShellItem> item;

        hr = itemArray->GetItemAt(i, &item);
        if ( FAILED(hr) )
        {
            // do not attempt to retrieve any other items
            // and just fail
            wxLogApiError(wxS("IShellItemArray::GetItem"), hr);
            tempPaths.clear();
            break;
        }

        hr = wxMSWImpl::GetFSPathFromShellItem(item, path);
        if ( FAILED(hr) )
        {
            // again, just fail
            tempPaths.clear();
            break;
        }

        tempPaths.push_back(path);
    }

    if ( tempPaths.empty() )
        return false; // there was en error

    paths = tempPaths;
    return true;
}

bool GetPathFromIFileDialog(IFileDialog* fileDialog, wxString& path)
{
    wxCOMPtr<IShellItem> item;

    HRESULT hr = fileDialog->GetResult(&item);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IFileDialog::GetResult"), hr);
        return false;
    }

    hr = wxMSWImpl::GetFSPathFromShellItem(item, path);
    if ( FAILED(hr) )
    {
        return false;
    }

    return true;
}

} // anonymous namespace

HRESULT
wxMSWImpl::GetFSPathFromShellItem(const wxCOMPtr<IShellItem>& item, wxString& path)
{
    wxCoTaskMemPtr<WCHAR> pOLEPath;
    const HRESULT hr = item->GetDisplayName(SIGDN_FILESYSPATH, &pOLEPath);

    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IShellItem::GetDisplayName"), hr);
        return hr;
    }

    path = pOLEPath;

    return S_OK;
}

#endif // wxUSE_IFILEOPENDIALOG

#if wxUSE_DIRDLG

// callback used in wxDirDialog::ShowSHBrowseForFolder()
static int CALLBACK
BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp, LPARAM pData)
{
    switch(uMsg)
    {
        case BFFM_INITIALIZED:
            // sent immediately after initialisation and so we may set the
            // initial selection here
            //
            // wParam = TRUE => lParam is a string and not a PIDL
            ::SendMessage(hwnd, BFFM_SETSELECTION, TRUE, pData);
            break;

        case BFFM_SELCHANGED:
            // note that this doesn't work with the new style UI (MSDN doesn't
            // say anything about it, but the comments in shlobj.h do!) but we
            // still execute this code in case it starts working again with the
            // "new new UI" (or would it be "NewUIEx" according to tradition?)
            {
                // Set the status window to the currently selected path.
                wxString strDir;
                if ( SHGetPathFromIDList((LPITEMIDLIST)lp,
                                         wxStringBuffer(strDir, MAX_PATH)) )
                {
                    // NB: this shouldn't be necessary with the new style box
                    //     (which is resizable), but as for now it doesn't work
                    //     anyhow (see the comment above) no harm in doing it

                    // need to truncate or it displays incorrectly
                    static const size_t maxChars = 37;
                    if ( strDir.length() > maxChars )
                    {
                        strDir = strDir.Right(maxChars);
                        strDir = wxString(wxT("...")) + strDir;
                    }

                    SendMessage(hwnd, BFFM_SETSTATUSTEXT,
                                0, wxMSW_CONV_LPARAM(strDir));
                }
            }
            break;

        //case BFFM_VALIDATEFAILED: -- might be used to provide custom message
        //                             if the user types in invalid dir name
    }

    return 0;
}

#endif // wxUSE_DIRDLG
