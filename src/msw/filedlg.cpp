/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/filedlg.cpp
// Purpose:     wxFileDialog
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


#if wxUSE_FILEDLG

#include "wx/filedlg.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcdlg.h"
    #include "wx/msw/missing.h"
    #include "wx/utils.h"
    #include "wx/msgdlg.h"
    #include "wx/filefn.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/app.h"
    #include "wx/math.h"
#endif

#include <stdlib.h>
#include <string.h>

#include "wx/dynlib.h"
#include "wx/filename.h"
#include "wx/scopedptr.h"
#include "wx/scopeguard.h"
#include "wx/tokenzr.h"
#include "wx/modalhook.h"

#include "wx/msw/private/dpiaware.h"
#include "wx/msw/private/filedialog.h"

// Note: this must be done after including the header above, as this is where
// wxUSE_IFILEOPENDIALOG is defined.
#if wxUSE_IFILEOPENDIALOG
    #include "wx/msw/wrapshl.h"

    #include "wx/msw/ole/comimpl.h"
#endif // wxUSE_IFILEOPENDIALOG

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

# define wxMAXPATH   65534

# define wxMAXFILE   1024

# define wxMAXEXT    5

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// standard dialog size for the old Windows systems where the dialog wasn't
// resizable
static wxRect gs_rectDialog(0, 0, 428, 266);

// ============================================================================
// implementation
// ============================================================================

wxIMPLEMENT_CLASS(wxFileDialog, wxFileDialogBase);

// ----------------------------------------------------------------------------

namespace
{

#if wxUSE_DYNLIB_CLASS

typedef BOOL (WINAPI *GetProcessUserModeExceptionPolicy_t)(LPDWORD);
typedef BOOL (WINAPI *SetProcessUserModeExceptionPolicy_t)(DWORD);

GetProcessUserModeExceptionPolicy_t gs_pfnGetProcessUserModeExceptionPolicy =
    (GetProcessUserModeExceptionPolicy_t) -1;

SetProcessUserModeExceptionPolicy_t gs_pfnSetProcessUserModeExceptionPolicy =
    (SetProcessUserModeExceptionPolicy_t) -1;

DWORD gs_oldExceptionPolicyFlags = 0;

bool gs_changedPolicy = false;

#endif // #if wxUSE_DYNLIB_CLASS

/*
Since Windows 7 by default (callback) exceptions aren't swallowed anymore
with native x64 applications. Exceptions can occur in a file dialog when
using the hook procedure in combination with third-party utilities.
Since Windows 7 SP1 the swallowing of exceptions can be enabled again
by using SetProcessUserModeExceptionPolicy.
*/
void ChangeExceptionPolicy()
{
#if wxUSE_DYNLIB_CLASS
    gs_changedPolicy = false;

    wxLoadedDLL dllKernel32(wxT("kernel32.dll"));

    if ( gs_pfnGetProcessUserModeExceptionPolicy
        == (GetProcessUserModeExceptionPolicy_t) -1)
    {
        wxDL_INIT_FUNC(gs_pfn, GetProcessUserModeExceptionPolicy, dllKernel32);
        wxDL_INIT_FUNC(gs_pfn, SetProcessUserModeExceptionPolicy, dllKernel32);
    }

    if ( !gs_pfnGetProcessUserModeExceptionPolicy
        || !gs_pfnSetProcessUserModeExceptionPolicy
        || !gs_pfnGetProcessUserModeExceptionPolicy(&gs_oldExceptionPolicyFlags) )
    {
        return;
    }

    if ( gs_pfnSetProcessUserModeExceptionPolicy(gs_oldExceptionPolicyFlags
        | 0x1 /* PROCESS_CALLBACK_FILTER_ENABLED */ ) )
    {
        gs_changedPolicy = true;
    }

#endif // wxUSE_DYNLIB_CLASS
}

void RestoreExceptionPolicy()
{
#if wxUSE_DYNLIB_CLASS
    if (gs_changedPolicy)
    {
        gs_changedPolicy = false;
        (void) gs_pfnSetProcessUserModeExceptionPolicy(gs_oldExceptionPolicyFlags);
    }
#endif // wxUSE_DYNLIB_CLASS
}

#if wxUSE_IFILEOPENDIALOG

// ----------------------------------------------------------------------------
// Various IFileDialog-related helpers: they're only used here for now, but if
// they're ever needed in wxDirDialog too, we should put move them to
// wx/msw/private/filedialog.h
// ----------------------------------------------------------------------------

// Register the given event handler with the dialog during its life-time.
class FileDialogEventsRegistrar
{
public:
    // The file dialog scope must be greater than that of this object.
    FileDialogEventsRegistrar(wxMSWImpl::wxIFileDialog& fileDialog,
                              IFileDialogEvents& eventsHandler)
        : m_fileDialog(fileDialog)
    {
        HRESULT hr = m_fileDialog->Advise(&eventsHandler, &m_cookie);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::Advise"), hr);
    }

    ~FileDialogEventsRegistrar()
    {
        HRESULT hr = m_fileDialog->Unadvise(m_cookie);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::Unadvise"), hr);
    }

private:
    wxMSWImpl::wxIFileDialog& m_fileDialog;
    DWORD m_cookie;

    wxDECLARE_NO_COPY_CLASS(FileDialogEventsRegistrar);
};

// Return 1-based index of the currently selected file type.
UINT FileDialogGetFileTypeIndex(IFileDialog* fileDialog)
{
    UINT nFilterIndex;
    HRESULT hr = fileDialog->GetFileTypeIndex(&nFilterIndex);
    if ( FAILED(hr) )
    {
        wxLogApiError(wxS("IFileDialog::GetFileTypeIndex"), hr);

        nFilterIndex = 0;
    }

    return nFilterIndex;
}

#endif // wxUSE_IFILEOPENDIALOG

} // unnamed namespace

// ----------------------------------------------------------------------------
// wxFileDialogMSWData: private data used by the dialog
// ----------------------------------------------------------------------------

class wxFileDialogMSWData
#if wxUSE_IFILEOPENDIALOG
    : public IFileDialogEvents
#endif // wxUSE_IFILEOPENDIALOG
{
public:
    explicit wxFileDialogMSWData(wxFileDialog* fileDialog)
#if wxUSE_IFILEOPENDIALOG
        : m_fileDialog(fileDialog)
#endif // wxUSE_IFILEOPENDIALOG
    {
        m_bMovedWindow = false;
        m_centreDir = 0;
    }

    // Hook function used by the common dialogs: it's a member of this class
    // just to allow it to call the private functions of wxFileDialog.
    static UINT_PTR APIENTRY
    HookFunction(HWND hDlg, UINT iMsg, WPARAM wParam, LPARAM lParam);

    // remember if our SetPosition() or Centre() (which requires special
    // treatment) was called
    bool m_bMovedWindow;
    int m_centreDir;        // nothing to do if 0


#if wxUSE_IFILEOPENDIALOG
    // IFileDialogEvents
    wxSTDMETHODIMP OnFileOk(IFileDialog*) wxOVERRIDE { return E_NOTIMPL; }
    wxSTDMETHODIMP OnFolderChanging(IFileDialog*, IShellItem*) wxOVERRIDE { return E_NOTIMPL; }
    wxSTDMETHODIMP OnFolderChange(IFileDialog*) wxOVERRIDE { return E_NOTIMPL; }
    wxSTDMETHODIMP OnSelectionChange(IFileDialog*) wxOVERRIDE { return E_NOTIMPL; }
    wxSTDMETHODIMP OnShareViolation(IFileDialog*, IShellItem*, FDE_SHAREVIOLATION_RESPONSE*) wxOVERRIDE { return E_NOTIMPL; }

    wxSTDMETHODIMP OnTypeChange(IFileDialog* pfd) wxOVERRIDE
    {
        m_fileDialog->MSWOnTypeChange(FileDialogGetFileTypeIndex(pfd));

        return S_OK;
    }

    wxSTDMETHODIMP OnOverwrite(IFileDialog*, IShellItem*, FDE_OVERWRITE_RESPONSE*) wxOVERRIDE { return E_NOTIMPL; }


    wxFileDialog* const m_fileDialog;

    DECLARE_IUNKNOWN_METHODS;
#endif // wxUSE_IFILEOPENDIALOG

    wxDECLARE_NO_COPY_CLASS(wxFileDialogMSWData);
};

BEGIN_IID_TABLE(wxFileDialogMSWData)
    ADD_IID(Unknown)
    ADD_IID(FileDialogEvents)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxFileDialogMSWData)

// ----------------------------------------------------------------------------
// hook function for moving the dialog
// ----------------------------------------------------------------------------

UINT_PTR APIENTRY
wxFileDialogMSWData::HookFunction(HWND      hDlg,
                                  UINT      iMsg,
                                  WPARAM    WXUNUSED(wParam),
                                  LPARAM    lParam)
{
    switch ( iMsg )
    {
        case WM_INITDIALOG:
            {
                OPENFILENAME* ofn = reinterpret_cast<OPENFILENAME *>(lParam);
                reinterpret_cast<wxFileDialog *>(ofn->lCustData)
                    ->MSWOnInitDialogHook((WXHWND)hDlg);
            }
            break;

        case WM_NOTIFY:
            {
                NMHDR* const pNM = reinterpret_cast<NMHDR*>(lParam);
                if ( pNM->code > CDN_LAST && pNM->code <= CDN_FIRST )
                {
                    const OPENFILENAME&
                        ofn = *reinterpret_cast<OFNOTIFY*>(lParam)->lpOFN;
                    wxFileDialog* const
                        dialog = reinterpret_cast<wxFileDialog*>(ofn.lCustData);

                    switch ( pNM->code )
                    {
                        case CDN_INITDONE:
                            dialog->MSWOnInitDone((WXHWND)hDlg);
                            break;

                        case CDN_SELCHANGE:
                            dialog->MSWOnSelChange((WXHWND)hDlg);
                            break;

                        case CDN_TYPECHANGE:
                            dialog->MSWOnTypeChange(ofn.nFilterIndex);
                            break;
                    }
                }
            }
            break;

        case WM_DESTROY:
            // reuse the position used for the dialog the next time by default
            //
            // NB: at least under Windows 2003 this is useless as after the
            //     first time it's shown the dialog always remembers its size
            //     and position itself and ignores any later SetWindowPos calls
            wxCopyRECTToRect(wxGetWindowRect(::GetParent(hDlg)), gs_rectDialog);
            break;
    }

    // do the default processing
    return 0;
}

// ----------------------------------------------------------------------------
// wxFileDialog
// ----------------------------------------------------------------------------

wxFileDialog::wxFileDialog(wxWindow *parent,
                           const wxString& message,
                           const wxString& defaultDir,
                           const wxString& defaultFileName,
                           const wxString& wildCard,
                           long style,
                           const wxPoint& pos,
                           const wxSize& sz,
                           const wxString& name)
            : wxFileDialogBase(parent, message, defaultDir, defaultFileName,
                               wildCard, style, pos, sz, name)

{
    // NB: all style checks are done by wxFileDialogBase::Create

    m_data = NULL;

    // Must set to zero, otherwise the wx routines won't size the window
    // the second time you call the file dialog, because it thinks it is
    // already at the requested size.. (when centering)
    gs_rectDialog.x =
    gs_rectDialog.y = 0;
}

wxFileDialog::~wxFileDialog()
{
#if wxUSE_IFILEOPENDIALOG
    if ( m_data )
        m_data->Release();
#else // !wxUSE_IFILEOPENDIALOG
    delete m_data;
#endif // wxUSE_IFILEOPENDIALOG
}

wxFileDialogMSWData& wxFileDialog::MSWData()
{
    if ( !m_data )
    {
        m_data = new wxFileDialogMSWData(this);

#if wxUSE_IFILEOPENDIALOG
        // Make sure it stays alive while we are.
        m_data->AddRef();
#endif // wxUSE_IFILEOPENDIALOG
    }

    return *m_data;
}

void wxFileDialog::GetPaths(wxArrayString& paths) const
{
    paths.Empty();

    wxString dir(m_dir);
    if ( m_dir.empty() || m_dir.Last() != wxT('\\') )
        dir += wxT('\\');

    size_t count = m_fileNames.GetCount();
    for ( size_t n = 0; n < count; n++ )
    {
        if (wxFileName(m_fileNames[n]).IsAbsolute())
            paths.Add(m_fileNames[n]);
        else
            paths.Add(dir + m_fileNames[n]);
    }
}

void wxFileDialog::GetFilenames(wxArrayString& files) const
{
    files = m_fileNames;
}

void wxFileDialog::DoGetPosition(int *x, int *y) const
{
    if ( x )
        *x = gs_rectDialog.x;
    if ( y )
        *y = gs_rectDialog.y;
}

void wxFileDialog::DoGetSize(int *width, int *height) const
{
    if ( width )
        *width = gs_rectDialog.width;
    if ( height )
        *height = gs_rectDialog.height;
}

void wxFileDialog::DoMoveWindow(int x, int y, int WXUNUSED(w), int WXUNUSED(h))
{
    gs_rectDialog.x = x;
    gs_rectDialog.y = y;

    // our HWND is only set when we're called from MSWOnInitDone(), test if
    // this is the case
    HWND hwnd = GetHwnd();
    if ( hwnd )
    {
        // size of the dialog can't be changed because the controls are not
        // laid out correctly then
       ::SetWindowPos(hwnd, HWND_TOP, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
    else // just remember that we were requested to move the window
    {
        wxFileDialogMSWData& data = MSWData();
        data.m_bMovedWindow = true;

        // if Centre() had been called before, it shouldn't be taken into
        // account now
        data.m_centreDir = 0;
    }
}

void wxFileDialog::DoCentre(int dir)
{
    wxFileDialogMSWData& data = MSWData();

    data.m_centreDir = dir;
    data.m_bMovedWindow = true;

    // it's unnecessary to do anything else at this stage as we'll redo it in
    // MSWOnInitDone() anyhow
}

void wxFileDialog::MSWOnInitDone(WXHWND hDlg)
{
    // note the dialog is the parent window: hDlg is a child of it when
    // OFN_EXPLORER is used
    HWND hFileDlg = ::GetParent((HWND)hDlg);

    // set HWND so that our DoMoveWindow() works correctly
    TempHWNDSetter set(this, (WXHWND)hFileDlg);

    if ( m_data && m_data->m_centreDir )
    {
        // now we have the real dialog size, remember it
        RECT rect;
        GetWindowRect(hFileDlg, &rect);
        gs_rectDialog = wxRectFromRECT(rect);

        // and position the window correctly: notice that we must use the base
        // class version as our own doesn't do anything except setting flags
        wxFileDialogBase::DoCentre(m_data->m_centreDir);
    }
    else // need to just move it to the correct place
    {
        SetPosition(gs_rectDialog.GetPosition());
    }

    // Call selection change handler so that update handler will be
    // called once with no selection.
    MSWOnSelChange(hDlg);
}

void wxFileDialog::MSWOnSelChange(WXHWND hDlg)
{
    TCHAR buf[MAX_PATH];
    LRESULT len = SendMessage(::GetParent(hDlg), CDM_GETFILEPATH,
                              MAX_PATH, reinterpret_cast<LPARAM>(buf));

    if ( len > 0 )
        m_currentlySelectedFilename = buf;
    else
        m_currentlySelectedFilename.clear();

    UpdateExtraControlUI();
}

void wxFileDialog::MSWOnTypeChange(int nFilterIndex)
{
    // Filter indices are 1-based, while we want to use 0-based index, as
    // usual. However the input index can apparently also be 0 in some
    // circumstances, so take care before decrementing it.
    m_currentlySelectedFilterIndex = nFilterIndex ? nFilterIndex - 1 : 0;

    UpdateExtraControlUI();
}

// helper used below in ShowCommFileDialog(): style is used to determine
// whether to show the "Save file" dialog (if it contains wxFD_SAVE bit) or
// "Open file" one; returns true on success or false on failure in which case
// err is filled with the CDERR_XXX constant
static bool DoShowCommFileDialog(OPENFILENAME *of, long style, DWORD *err)
{
    // Extra controls do not handle per-monitor DPI, fall back to system DPI
    // so entire file-dialog is resized.
    wxScopedPtr<wxMSWImpl::AutoSystemDpiAware> dpiAwareness;
    if ( of->Flags & OFN_ENABLEHOOK )
        dpiAwareness.reset(new wxMSWImpl::AutoSystemDpiAware());

    if ( style & wxFD_SAVE ? GetSaveFileName(of) : GetOpenFileName(of) )
        return true;

    if ( err )
    {
        *err = CommDlgExtendedError();
    }

    return false;
}

void wxFileDialog::MSWOnInitDialogHook(WXHWND hwnd)
{
    TempHWNDSetter set(this, hwnd);

    CreateExtraControl();
}

int wxFileDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindow* const parent = GetParentForModalDialog(m_parent, GetWindowStyle());
    WXHWND hWndParent = parent ? GetHwndOf(parent) : NULL;

    wxWindowDisabler disableOthers(this, parent);

    /*
        We need to use the old style dialog in order to use a hook function
        which allows us to position it or use custom controls in it but, if
        possible, we prefer to use the new style one instead.
    */
#if wxUSE_IFILEOPENDIALOG
    if ( (!m_data || !m_data->m_bMovedWindow) && !HasExtraControlCreator() )
    {
        const int rc = ShowIFileDialog(hWndParent);
        if ( rc != wxID_NONE )
            return rc;
        //else: Failed to use IFileDialog, fall back to the traditional one.
    }
#endif // wxUSE_IFILEOPENDIALOG

    return ShowCommFileDialog(hWndParent);
}

int wxFileDialog::ShowCommFileDialog(WXHWND hWndParent)
{
    static wxChar fileNameBuffer [ wxMAXPATH ];           // the file-name
    wxChar        titleBuffer    [ wxMAXFILE+1+wxMAXEXT ];  // the file-name, without path

    *fileNameBuffer = wxT('\0');
    *titleBuffer    = wxT('\0');

    long msw_flags = OFN_HIDEREADONLY;

    if ( HasFdFlag(wxFD_NO_FOLLOW) )
        msw_flags |= OFN_NODEREFERENCELINKS;

    if ( HasFdFlag(wxFD_FILE_MUST_EXIST) )
        msw_flags |= OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if ( HasFlag(wxFD_SHOW_HIDDEN) )
        msw_flags |= OFN_FORCESHOWHIDDEN;
    /*
        If the window has been moved the programmer is probably
        trying to center or position it.  Thus we set the callback
        or hook function so that we can actually adjust the position.
        Without moving or centering the dlg, it will just stay
        in the upper left of the frame, it does not center
        automatically.
    */
    if ((m_data && m_data->m_bMovedWindow) || HasExtraControlCreator()) // we need these flags.
    {
        ChangeExceptionPolicy();
        msw_flags |= OFN_EXPLORER|OFN_ENABLEHOOK;
        msw_flags |= OFN_ENABLESIZING;
    }

    wxON_BLOCK_EXIT0(RestoreExceptionPolicy);

    if ( HasFdFlag(wxFD_MULTIPLE) )
    {
        // OFN_EXPLORER must always be specified with OFN_ALLOWMULTISELECT
        msw_flags |= OFN_EXPLORER | OFN_ALLOWMULTISELECT;
    }

    // if wxFD_CHANGE_DIR flag is not given we shouldn't change the CWD which the
    // standard dialog does by default (notice that under NT it does it anyhow,
    // OFN_NOCHANGEDIR or not, see below)
    if ( !HasFdFlag(wxFD_CHANGE_DIR) )
    {
        msw_flags |= OFN_NOCHANGEDIR;
    }

    if ( HasFdFlag(wxFD_OVERWRITE_PROMPT) )
    {
        msw_flags |= OFN_OVERWRITEPROMPT;
    }

    OPENFILENAME of;
    wxZeroMemory(of);

    of.lStructSize       = sizeof(OPENFILENAME);
    of.hwndOwner         = hWndParent;
    of.lpstrTitle        = m_message.t_str();
    of.lpstrFileTitle    = titleBuffer;
    of.nMaxFileTitle     = wxMAXFILE + 1 + wxMAXEXT;

    GlobalPtr hgbl;
    if ( HasExtraControlCreator() )
    {
        msw_flags |= OFN_ENABLETEMPLATEHANDLE;

        hgbl.Init(256, GMEM_ZEROINIT);
        GlobalPtrLock hgblLock(hgbl);
        LPDLGTEMPLATE lpdt = static_cast<LPDLGTEMPLATE>(hgblLock.Get());

        // Define a dialog box.

        lpdt->style = DS_CONTROL | WS_CHILD | WS_CLIPSIBLINGS;
        lpdt->cdit = 0;         // Number of controls
        lpdt->x = 0;
        lpdt->y = 0;

        // convert the size of the extra controls to the dialog units
        const wxSize extraSize = GetExtraControlSize();
        const LONG baseUnits = ::GetDialogBaseUnits();
        lpdt->cx = ::MulDiv(extraSize.x, 4, LOWORD(baseUnits));
        lpdt->cy = ::MulDiv(extraSize.y, 8, HIWORD(baseUnits));

        // after the DLGTEMPLATE there are 3 additional WORDs for dialog menu,
        // class and title, all three set to zeros.

        of.hInstance = (HINSTANCE)lpdt;
    }

    // Convert forward slashes to backslashes (file selector doesn't like
    // forward slashes) and also squeeze multiple consecutive slashes into one
    // as it doesn't like two backslashes in a row neither

    wxString  dir;
    size_t    i, len = m_dir.length();
    dir.reserve(len);
    for ( i = 0; i < len; i++ )
    {
        wxChar ch = m_dir[i];
        switch ( ch )
        {
            case wxT('/'):
                // convert to backslash
                ch = wxT('\\');
                wxFALLTHROUGH;

            case wxT('\\'):
                while ( i < len - 1 )
                {
                    wxChar chNext = m_dir[i + 1];
                    if ( chNext != wxT('\\') && chNext != wxT('/') )
                        break;

                    // ignore the next one, unless it is at the start of a UNC path
                    if (i > 0)
                        i++;
                    else
                        break;
                }
                wxFALLTHROUGH;

            default:
                // normal char
                dir += ch;
        }
    }

    of.lpstrInitialDir   = dir.c_str();

    of.Flags             = msw_flags;
    of.lpfnHook          = wxFileDialogMSWData::HookFunction;
    of.lCustData         = (LPARAM)this;

    wxArrayString wildDescriptions, wildFilters;

    size_t items = wxParseCommonDialogsFilter(m_wildCard, wildDescriptions, wildFilters);

    wxASSERT_MSG( items > 0 , wxT("empty wildcard list") );

    wxString filterBuffer;

    for (i = 0; i < items ; i++)
    {
        filterBuffer += wildDescriptions[i];
        filterBuffer += wxT("|");
        filterBuffer += wildFilters[i];
        filterBuffer += wxT("|");
    }

    // Replace | with \0
    for (i = 0; i < filterBuffer.length(); i++ ) {
        if ( filterBuffer.GetChar(i) == wxT('|') ) {
            filterBuffer[i] = wxT('\0');
        }
    }

    of.lpstrFilter  = filterBuffer.t_str();
    of.nFilterIndex = m_filterIndex + 1;
    m_currentlySelectedFilterIndex = m_filterIndex;

    //=== Setting defaultFileName >>=========================================

    wxStrlcpy(fileNameBuffer, m_fileName.c_str(), WXSIZEOF(fileNameBuffer));

    of.lpstrFile = fileNameBuffer;  // holds returned filename
    of.nMaxFile  = wxMAXPATH;

    // we must set the default extension because otherwise Windows would check
    // for the existing of a wrong file with wxFD_OVERWRITE_PROMPT (i.e. if the
    // user types "foo" and the default extension is ".bar" we should force it
    // to check for "foo.bar" existence and not "foo")
    wxString defextBuffer; // we need it to be alive until GetSaveFileName()!
    if (HasFdFlag(wxFD_SAVE))
    {
        const wxChar* extension = filterBuffer.t_str();
        int maxFilter = (int)(of.nFilterIndex*2L) - 1;

        for( int j = 0; j < maxFilter; j++ )           // get extension
            extension = extension + wxStrlen( extension ) + 1;

        // use dummy name a to avoid assert in AppendExtension
        defextBuffer = AppendExtension(wxT("a"), extension);
        if (defextBuffer.StartsWith(wxT("a.")))
        {
            defextBuffer = defextBuffer.Mid(2); // remove "a."
            of.lpstrDefExt = defextBuffer.c_str();
        }
    }

    // Create a temporary struct to restore the CWD when we exit this function
    // store off before the standard windows dialog can possibly change it
    struct CwdRestore
    {
        wxString value;
        ~CwdRestore()
        {
            if (!value.empty())
                wxSetWorkingDirectory(value);
        }
    } cwdOrig;

    // GetOpenFileName will always change the current working directory
    // (according to MSDN) because the flag OFN_NOCHANGEDIR has no effect.
    // If the user did not specify wxFD_CHANGE_DIR let's restore the
    // current working directory to what it was before the dialog was shown.
    if (msw_flags & OFN_NOCHANGEDIR)
        cwdOrig.value = wxGetCwd();

    //== Execute FileDialog >>=================================================

    DWORD errCode;
    bool success = DoShowCommFileDialog(&of, m_windowStyle, &errCode);

    if ( !success &&
            errCode == FNERR_INVALIDFILENAME &&
                of.lpstrFile[0] )
    {
        // this can happen if the default file name is invalid, try without it
        // now
        of.lpstrFile[0] = wxT('\0');
        success = DoShowCommFileDialog(&of, m_windowStyle, &errCode);
    }

    if ( !success )
    {
        // common dialog failed - why?
        if ( errCode != 0 )
        {
            wxLogError(_("File dialog failed with error code %0lx."), errCode);
        }
        //else: it was just cancelled

        return wxID_CANCEL;
    }

    m_fileNames.Empty();

    if ( ( HasFdFlag(wxFD_MULTIPLE) ) &&
         ( fileNameBuffer[of.nFileOffset-1] == wxT('\0') )
       )
    {
        m_dir = fileNameBuffer;
        i = of.nFileOffset;
        m_fileName = &fileNameBuffer[i];
        m_fileNames.Add(m_fileName);
        i += m_fileName.length() + 1;

        while (fileNameBuffer[i] != wxT('\0'))
        {
            m_fileNames.Add(&fileNameBuffer[i]);
            i += wxStrlen(&fileNameBuffer[i]) + 1;
        }

        m_path = m_dir;
        if ( m_dir.Last() != wxT('\\') )
            m_path += wxT('\\');

        m_path += m_fileName;
        m_filterIndex = (int)of.nFilterIndex - 1;
    }
    else
    {
        //=== Adding the correct extension >>=================================

        m_filterIndex = (int)of.nFilterIndex - 1;

        if ( !of.nFileExtension || fileNameBuffer[of.nFileExtension] == wxT('\0') )
        {
            // User has typed a filename without an extension:
            const wxChar* extension = filterBuffer.t_str();
            int   maxFilter = (int)(of.nFilterIndex*2L) - 1;

            for( int j = 0; j < maxFilter; j++ )           // get extension
                extension = extension + wxStrlen( extension ) + 1;

            m_fileName = AppendExtension(fileNameBuffer, extension);
            wxStrlcpy(fileNameBuffer, m_fileName.c_str(), WXSIZEOF(fileNameBuffer));
        }

        m_path = fileNameBuffer;
        m_fileName = wxFileNameFromPath(fileNameBuffer);
        m_fileNames.Add(m_fileName);
        m_dir = wxPathOnly(fileNameBuffer);
    }

    return wxID_OK;

}

#if wxUSE_IFILEOPENDIALOG

int wxFileDialog::ShowIFileDialog(WXHWND hWndParent)
{
    // Create the dialog.
    wxMSWImpl::wxIFileDialog
        fileDialog(HasFlag(wxFD_SAVE) ? CLSID_FileSaveDialog
                                      : CLSID_FileOpenDialog);

    if ( !fileDialog.IsOk() )
        return wxID_NONE;

    // Register our event handler with the dialog.
    wxFileDialogMSWData& data = MSWData();

    FileDialogEventsRegistrar registerEvents(fileDialog, data);

    // Configure the dialog before showing it.
    fileDialog.SetTitle(m_message);

    HRESULT hr;

    wxArrayString wildDescriptions, wildFilters;
    const UINT nWildcards = wxParseCommonDialogsFilter(m_wildCard,
                                                       wildDescriptions,
                                                       wildFilters);
    if ( nWildcards )
    {
        wxVector<COMDLG_FILTERSPEC> filterSpecs(nWildcards);
        for ( UINT n = 0; n < nWildcards; ++n )
        {
            filterSpecs[n].pszName = wildDescriptions[n].wc_str();
            filterSpecs[n].pszSpec = wildFilters[n].wc_str();
        }

        hr = fileDialog->SetFileTypes(nWildcards, &filterSpecs[0]);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::SetFileTypes"), hr);

        hr = fileDialog->SetFileTypeIndex(m_filterIndex + 1);
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::SetFileTypeIndex"), hr);
    }

    if ( !m_dir.empty() )
    {
        fileDialog.SetInitialPath(m_dir);
    }

    if ( !m_fileName.empty() )
    {
        hr = fileDialog->SetFileName(m_fileName.wc_str());
        if ( FAILED(hr) )
            wxLogApiError(wxS("IFileDialog::SetDefaultExtension"), hr);
    }


    // We never set the following flags currently:
    //
    //  - FOS_STRICTFILETYPES
    //  - FOS_NOVALIDATE
    //  - FOS_CREATEPROMPT
    //  - FOS_SHAREAWARE
    //  - FOS_NOREADONLYRETURN
    //  - FOS_NOTESTFILECREATE
    //  - FOS_OKBUTTONNEEDSINTERACTION
    //  - FOS_DONTADDTORECENT
    //  - FOS_DEFAULTNOMINIMODE
    //  - FOS_FORCEPREVIEWPANEON
    //
    // We might want to add wxFD_XXX equivalents for some of them in the future.
    int options = 0;
    if ( HasFlag(wxFD_OVERWRITE_PROMPT) )
        options |= FOS_OVERWRITEPROMPT;
    if ( !HasFlag(wxFD_CHANGE_DIR) )
        options |= FOS_NOCHANGEDIR;
    if ( HasFlag(wxFD_FILE_MUST_EXIST) )
        options |= FOS_PATHMUSTEXIST | FOS_FILEMUSTEXIST;
    if ( HasFlag(wxFD_MULTIPLE) )
        options |= FOS_ALLOWMULTISELECT;
    if ( HasFlag(wxFD_SHOW_HIDDEN) )
        options |= FOS_FORCESHOWHIDDEN;
    if ( HasFlag(wxFD_NO_FOLLOW) )
        options |= FOS_NODEREFERENCELINKS;

    // Finally do show the dialog.
    const int rc = fileDialog.Show(hWndParent, options, &m_fileNames, &m_path);
    if ( rc == wxID_OK )
    {
        // As with the common dialog, the index is 1-based here, but don't make
        // it negative if we somehow failed to retrieve it at all.
        m_filterIndex = FileDialogGetFileTypeIndex(fileDialog.Get());
        if ( m_filterIndex > 0 )
            m_filterIndex--;

        if ( HasFlag(wxFD_MULTIPLE) )
        {
            // This shouldn't the case, but check to be absolutely sure.
            if ( !m_fileNames.empty() )
                m_dir = wxFileName(m_fileNames[0]).GetPath();
        }
        else // Single selected file is in m_path.
        {
            // Append the extension if necessary.
            m_path = AppendExtension(m_path, wildFilters[m_filterIndex]);

            const wxFileName fn(m_path);
            m_dir = fn.GetPath();
            m_fileName = fn.GetFullName();

            // For compatibility, our GetFilenames() must also return the same
            // file, so put it into the array too.
            m_fileNames.Clear();
            m_fileNames.Add(m_fileName);
        }
    }

    return rc;
}

#endif // wxUSE_IFILEOPENDIALOG

#endif // wxUSE_FILEDLG
