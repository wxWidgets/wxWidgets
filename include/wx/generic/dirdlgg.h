/////////////////////////////////////////////////////////////////////////////
// Name:        dirdlgg.h
// Purpose:     wxGenericDirCtrl class
//              Builds on wxDirCtrl class written by Robert Roebling for the
//              wxFile application, modified by Harm van der Heijden.
//              Further modified for Windows.
// Author:      Robert Roebling, Harm van der Heijden, Julian Smart et al
// Modified by:
// Created:     21/3/2000
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling, Harm van der Heijden, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIRDLGG_H_
#define _WX_DIRDLGG_H_

#ifdef __GNUG__
#pragma interface "dirdlgg.h"
#endif

#include "wx/defs.h"

#if wxUSE_DIRDLG

#include "wx/dialog.h"
class WXDLLEXPORT wxGenericDirCtrl;
class WXDLLEXPORT wxTextCtrl;
class WXDLLEXPORT wxTreeEvent;

//-----------------------------------------------------------------------------
// wxGenericDirDialog
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxGenericDirDialog: public wxDialog
{
public:
    wxGenericDirDialog(): wxDialog() {}
    wxGenericDirDialog(wxWindow* parent, const wxString& title,
                       const wxString& defaultPath = wxEmptyString,
                       long style = wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& sz = wxSize(450, 550),
                       const wxString& name = _T("dialog"));

    //// Accessors
    inline void SetMessage(const wxString& message) { m_message = message; }
    void SetPath(const wxString& path);
    inline void SetStyle(long style) { m_dialogStyle = style; }

    inline wxString GetMessage(void) const { return m_message; }
    wxString GetPath(void) const;
    inline long GetStyle(void) const { return m_dialogStyle; }

    wxTextCtrl* GetInputCtrl() const { return m_input; }

    //// Overrides
    int ShowModal();

protected:
    //// Event handlers
    void OnCloseWindow(wxCloseEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnTreeSelected(wxTreeEvent &event);
    void OnTreeKeyDown(wxTreeEvent &event);
    void OnNew(wxCommandEvent& event);
    void OnGoHome(wxCommandEvent& event);
    void OnShowHidden(wxCommandEvent& event);

    wxString          m_message;
    long              m_dialogStyle;
    wxString          m_path;
    wxGenericDirCtrl* m_dirCtrl;
    wxTextCtrl*       m_input;

    DECLARE_EVENT_TABLE()
};

#if !defined(__WXMSW__) && !defined(__WXMAC__)
    #define wxDirDialog wxGenericDirDialog
#endif

#endif // wxUSE_DIRDLG

#endif // _WX_DIRDLGG_H_
