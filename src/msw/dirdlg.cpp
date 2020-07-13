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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_DIRDLG

#if wxUSE_OLE

#include "wx/dirdlg.h"
#include "wx/modalhook.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/log.h"
    #include "wx/app.h"     // for GetComCtl32Version()
#endif

#include "wx/msw/private.h"
#include "wx/msw/wrapshl.h"
#include "wx/msw/private/comptr.h"
#include "wx/msw/private/cotaskmemptr.h"
#include "wx/dynlib.h"

#include <initguid.h>

// IFileOpenDialog implementation needs wxDynamicLibrary for
// run-time linking SHCreateItemFromParsingName(), available
// only under Windows Vista and newer.
// It also needs a compiler providing declarations and definitions
// of interfaces available in Windows Vista.
#if wxUSE_DYNLIB_CLASS && defined(__IFileOpenDialog_INTERFACE_DEFINED__)
    #define wxUSE_IFILEOPENDIALOG 1
#else
    #define wxUSE_IFILEOPENDIALOG 0
#endif

#if wxUSE_IFILEOPENDIALOG
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
// constants
// ----------------------------------------------------------------------------

#ifndef BIF_NONEWFOLDERBUTTON
    #define BIF_NONEWFOLDERBUTTON  0x0200
#endif

// ----------------------------------------------------------------------------
// wxWidgets macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_CLASS(wxDirDialog, wxDialog);

// ----------------------------------------------------------------------------
// private functions prototypes
// ----------------------------------------------------------------------------

#if wxUSE_IFILEOPENDIALOG

// helper functions for wxDirDialog::ShowIFileOpenDialog()
bool InitIFileOpenDialog(const wxString& message, const wxString& defaultPath,
                         bool multipleSelection, bool showHidden, wxCOMPtr<IFileOpenDialog>& fileDialog);
bool GetPathsFromIFileOpenDialog(const wxCOMPtr<IFileOpenDialog>& fileDialog, bool multipleSelection,
                                 wxArrayString& paths);
bool ConvertIShellItemToPath(const wxCOMPtr<IShellItem>& item, wxString& path);

#endif // #if wxUSE_IFILEOPENDIALOG

// callback used in wxDirDialog::ShowSHBrowseForFolder()
static int CALLBACK BrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lp,
                                       LPARAM pData);


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
    WXHWND hWndParent = parent ? GetHwndOf(parent) : NULL;

    m_paths.clear();

    // Use IFileDialog under new enough Windows, it's more user-friendly.
    int rc;
#if wxUSE_IFILEOPENDIALOG
    // While the new dialog is available under Vista, it may return a wrong
    // path there (see http://support.microsoft.com/kb/969885/en-us), so we
    // don't use it there by default. We could improve the version test to
    // allow its use if the comdlg32.dll version is greater than 6.0.6002.22125
    // as this means that the hotfix correcting this bug is installed.
    if ( wxGetWinVersion() > wxWinVersion_Vista )
    {
        rc = ShowIFileOpenDialog(hWndParent);
    }
    else
    {
        rc = wxID_NONE;
    }

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
    bi.pidlRoot       = NULL;
    bi.pszDisplayName = NULL;
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

// Function for obtaining folder name on Vista and newer.
//
// Returns wxID_OK on success, wxID_CANCEL if cancelled by user or wxID_NONE if
// an error occurred and we should fall back onto the old dialog.
#if wxUSE_IFILEOPENDIALOG

int wxDirDialog::ShowIFileOpenDialog(WXHWND owner)
{
    HRESULT hr = S_OK;
    wxCOMPtr<IFileOpenDialog> fileDialog;

    if ( !InitIFileOpenDialog(m_message, m_path, HasFlag(wxDD_MULTIPLE),
                              HasFlag(wxDD_SHOW_HIDDEN), fileDialog) )
    {
        return wxID_NONE; // Failed to initialize the dialog
    }

    hr = fileDialog->Show(owner);
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
    else if ( GetPathsFromIFileOpenDialog(fileDialog, HasFlag(wxDD_MULTIPLE),
                                          m_paths) )
    {
        if ( !HasFlag(wxDD_MULTIPLE) )
        {
            m_path = m_paths.Last();
        }

        return wxID_OK;
    }

    // Failed to show the dialog or obtain the selected folders(s)
    wxLogSysError(_("Couldn't obtain folder name"), hr);
    return wxID_CANCEL;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// helper function for wxDirDialog::ShowIFileOpenDialog()
bool InitIFileOpenDialog(const wxString& message, const wxString& defaultPath,
                         bool multipleSelection, bool showHidden,
                         wxCOMPtr<IFileOpenDialog>& fileDialog)
{
    HRESULT hr = S_OK;
    wxCOMPtr<IFileOpenDialog> dlg;
    // allow to select only a file system folder, do not change the CWD
    long options = FOS_PICKFOLDERS | FOS_FORCEFILESYSTEM | FOS_NOCHANGEDIR;

    hr = ::CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER,
                            wxIID_PPV_ARGS(IFileOpenDialog, &dlg));
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("CoCreateInstance(CLSID_FileOpenDialog)"), hr);
        return false;
    }

    if ( multipleSelection )
        options |= FOS_ALLOWMULTISELECT;
    if ( showHidden )
        options |= FOS_FORCESHOWHIDDEN;

    hr = dlg->SetOptions(options);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IFileOpenDialog::SetOptions"), hr);
        return false;
    }

    hr = dlg->SetTitle(message.wc_str());
    if ( FAILED(hr) )
    {
        // This error is not serious, let's just log it and continue even
        // without the title set.
        wxLogApiError(wxS("IFileOpenDialog::SetTitle"), hr);
    }

    // set the initial path
    if ( !defaultPath.empty() )
    {
        // We need to link SHCreateItemFromParsingName() dynamically as it's
        // not available on pre-Vista systems.
        typedef HRESULT
        (WINAPI *SHCreateItemFromParsingName_t)(PCWSTR,
                                                IBindCtx*,
                                                REFIID,
                                                void**);

        SHCreateItemFromParsingName_t pfnSHCreateItemFromParsingName = NULL;
        wxDynamicLibrary dllShell32;
        if ( dllShell32.Load(wxS("shell32.dll"), wxDL_VERBATIM | wxDL_QUIET) )
        {
            wxDL_INIT_FUNC(pfn, SHCreateItemFromParsingName, dllShell32);
        }

        if ( !pfnSHCreateItemFromParsingName )
        {
            wxLogLastError(wxS("SHCreateItemFromParsingName() not found"));
            return false;
        }

        wxCOMPtr<IShellItem> folder;
        hr = pfnSHCreateItemFromParsingName(defaultPath.wc_str(),
                                            NULL,
                                            wxIID_PPV_ARGS(IShellItem,
                                                           &folder));

        // Failing to parse the folder name is not really an error, we'll just
        // ignore the initial directory in this case, but we should still show
        // the dialog.
        if ( FAILED(hr) )
        {
            if ( hr != HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND) )
            {
                wxLogApiError(wxS("SHCreateItemFromParsingName"), hr);
                return false;
            }
        }
        else // The folder was parsed correctly.
        {
            hr = dlg->SetFolder(folder);
            if ( FAILED(hr) )
            {
                wxLogApiError(wxS("IFileOpenDialog::SetFolder"), hr);
                return false;
            }
        }
    }

    fileDialog = dlg;
    return true;
}

// helper function for wxDirDialog::ShowIFileOpenDialog()
bool GetPathsFromIFileOpenDialog(const wxCOMPtr<IFileOpenDialog>& fileDialog, bool multipleSelection,
                                 wxArrayString& paths)
{
    HRESULT hr = S_OK;
    wxString path;
    wxArrayString tempPaths;

    if ( multipleSelection )
    {
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

            if ( !ConvertIShellItemToPath(item, path) )
            {
                // again, just fail
                tempPaths.clear();
                break;
            }

            tempPaths.push_back(path);
        }

    }
    else // single selection
    {
        wxCOMPtr<IShellItem> item;

        hr = fileDialog->GetResult(&item);
        if ( FAILED(hr) )
        {
            wxLogApiError(wxS("IFileOpenDialog::GetResult"), hr);
            return false;
        }

        if ( !ConvertIShellItemToPath(item, path) )
        {
            return false;
        }

        tempPaths.push_back(path);
    }

    if ( tempPaths.empty() )
        return false; // there was en error

    paths = tempPaths;
    return true;
}

// helper function for wxDirDialog::ShowIFileOpenDialog()
bool ConvertIShellItemToPath(const wxCOMPtr<IShellItem>& item, wxString& path)
{
    wxCoTaskMemPtr<WCHAR> pOLEPath;
    const HRESULT hr = item->GetDisplayName(SIGDN_FILESYSPATH, &pOLEPath);

    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IShellItem::GetDisplayName"), hr);
        return false;
    }

    path = pOLEPath;

    return true;
}

#endif // wxUSE_IFILEOPENDIALOG

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

#endif // compiler/platform on which the code here compiles

#endif // wxUSE_DIRDLG
