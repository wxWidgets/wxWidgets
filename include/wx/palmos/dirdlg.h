/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/dirdlg.h
// Purpose:     wxDirDialog class
// Author:      William Osborne
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id: 
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLG_H_
#define _WX_DIRDLG_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "dirdlg.h"
#endif

class WXDLLEXPORT wxDirDialog : public wxDialog
{
public:
    wxDirDialog(wxWindow *parent,
                const wxString& message = wxDirSelectorPromptStr,
                const wxString& defaultPath = wxEmptyString,
                long style = 0,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                const wxString& name = wxDirDialogNameStr);

    void SetMessage(const wxString& message) { m_message = message; }
    void SetPath(const wxString& path);
    void SetStyle(long style) { SetWindowStyle(style); }

    wxString GetMessage() const { return m_message; }
    wxString GetPath() const { return m_path; }
    long GetStyle() const { return GetWindowStyle(); }

    virtual int ShowModal();

protected:
    wxString    m_message;
    wxString    m_path;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxDirDialog)
};

#endif
    // _WX_DIRDLG_H_
