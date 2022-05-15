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
// IFileOpenDialog implementation needs wxDynamicLibrary for
// run-time linking SHCreateItemFromParsingName(), available
// only under Windows Vista and newer.
// It also needs a compiler providing declarations and definitions
// of interfaces available in Windows Vista.
// And it needs OLE support to actually use these interfaces.
#if (wxUSE_DIRDLG || wxUSE_FILEDLG) && wxUSE_DYNLIB_CLASS && wxUSE_OLE && \
    defined(__IFileOpenDialog_INTERFACE_DEFINED__)
    #define wxUSE_IFILEOPENDIALOG 1
#else
    #define wxUSE_IFILEOPENDIALOG 0
#endif

#if wxUSE_IFILEOPENDIALOG

#include "wx/msw/private/comptr.h"

namespace wxMSWImpl
{

class wxIFileDialog
{
public:
    wxIFileDialog() { }

    // Show the file dialog with the given parent window and options.
    //
    // Returns the selected path, or paths, in the provided output parameters,
    // depending on whether FOS_ALLOWMULTISELECT is part of the options.
    //
    // The return value is wxID_OK if any paths were returned, wxID_CANCEL if the
    // dialog was cancelled or wxID_NONE if creating it failed, in which case
    // fileDialog is null -- otherwise it contains the pointer to the dialog which
    // can be used to retrieve more information from it.
    //
    // For historical reasons, this function is defined in src/msw/dirdlg.cpp.
    int
    Show(HWND owner,
         const CLSID& clsid,
         int options,
         const wxString& message,
         const wxString& defaultValue,
         wxArrayString* pathsOut,
         wxString* pathOut);

    // Behave as IFileDialog.
    IFileDialog* operator->() const { return m_fileDialog.Get(); }

private:
    wxCOMPtr<IFileDialog> m_fileDialog;
};

} // namespace wxMSWImpl

#endif // wxUSE_IFILEOPENDIALOG

#endif // _WX_MSW_PRIVATE_FILEDIALOG_H_
