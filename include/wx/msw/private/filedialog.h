///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/private/filedialog.h
// Purpose:     IFileDialog-related functions
// Author:      Vadim Zeitlin
// Created:     2022-05-15
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_PRIVATE_FILEDIALOG_H_
#define _WX_MSW_PRIVATE_FILEDIALOG_H_

#include "wx/msw/private.h"
#include "wx/msw/wrapshl.h"

// We want to use IFileDialog if either wxDirDialog or wxFileDialog are used.
//
// Using IFileOpenDialog requires a compiler providing declarations and
// definitions of interfaces available in Windows Vista.
// And it needs OLE support to actually use these interfaces.
#if (wxUSE_DIRDLG || wxUSE_FILEDLG) && wxUSE_OLE && \
    defined(__IFileOpenDialog_INTERFACE_DEFINED__)
    #define wxUSE_IFILEOPENDIALOG 1
#else
    #define wxUSE_IFILEOPENDIALOG 0
#endif

#if wxUSE_IFILEOPENDIALOG

#include "wx/msw/private/comptr.h"

namespace wxMSWImpl
{

// For historical reasons, this class is defined in src/msw/dirdlg.cpp.
class wxIFileDialog
{
public:
    // Return true if we can use IFileDialog with an owner: this is
    // unfortunately not always the case, as IFileDialog requires using
    // apartment threading model in this case.
    static bool CanBeUsedWithAnOwner();

    // Create the dialog of the specified type.
    //
    // CLSID must be either CLSID_FileOpenDialog or CLSID_FileSaveDialog.
    //
    // Use IsOk() to check if the dialog was created successfully.
    explicit wxIFileDialog(const CLSID& clsid);

    // If this returns false, the dialog can't be used at all.
    bool IsOk() const { return m_fileDialog.Get() != nullptr; }

    // Set the dialog title.
    void SetTitle(const wxString& title);

    // Set the initial path to show in the dialog.
    void SetInitialPath(const wxString& path);

    // Add a shortcut.
    void AddPlace(const wxString& path, FDAP fdap);

    // Show the file dialog with the given parent window and options.
    //
    // Returns the selected path, or paths, in the provided output parameters,
    // depending on whether FOS_ALLOWMULTISELECT is part of the options.
    //
    // The return value is wxID_OK if any paths were returned, wxID_CANCEL if the
    // dialog was cancelled.
    int
    Show(HWND owner, int options, wxArrayString* pathsOut, wxString* pathOut);

    // Behave as IFileDialog.
    IFileDialog* Get() const { return m_fileDialog.Get(); }
    IFileDialog* operator->() const { return m_fileDialog.Get(); }

private:
    wxCOMPtr<IFileDialog> m_fileDialog;
};

// Initialize an IShellItem object with the given path.
HRESULT InitShellItemFromPath(wxCOMPtr<IShellItem>& item, const wxString& path);

// Extract the filesystem path corresponding to the given shell item.
HRESULT GetFSPathFromShellItem(const wxCOMPtr<IShellItem>& item, wxString& path);

} // namespace wxMSWImpl

#endif // wxUSE_IFILEOPENDIALOG

#endif // _WX_MSW_PRIVATE_FILEDIALOG_H_
