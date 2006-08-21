/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:     wxDialog class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const wxChar) wxDialogNameStr[];

class WXDLLEXPORT wxMacToolTip ;

// Dialog boxes
class WXDLLEXPORT wxDialog : public wxDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxDialog)

public:
    wxDialog() { Init(); }

    // Constructor with a modal flag, but no window id - the old convention
    wxDialog(wxWindow *parent,
             const wxString& title, bool modal,
             int x = -1, int y= -1, int width = 500, int height = 500,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
    {
        Init();
        m_isModalStyle = modal;
        Create(parent, -1, title, wxPoint(x, y), wxSize(width, height),
               style, name);
    }

    // Constructor with no modal flag - the new convention.
    wxDialog(wxWindow *parent, wxWindowID id,
             const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
    {
        Init();
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxDialogNameStr);

    ~wxDialog();

//    virtual bool Destroy();
    virtual bool Show(bool show = true);

    void SetModal(bool flag);
    virtual bool IsModal() const;

    // For now, same as Show(TRUE) but returns return code
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);

    // mac also takes command-period as cancel
    virtual bool IsEscapeKey(const wxKeyEvent& event);

    // returns TRUE if we're in a modal loop
    bool IsModalShowing() const;

    // implementation
    // --------------

    // show modal dialog and enter modal loop
    void DoShowModal();

private:
    void Init();

    bool m_isModalStyle;
};

#endif
    // _WX_DIALOG_H_
