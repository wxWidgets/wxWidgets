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

WXDLLIMPEXP_DATA_CORE(extern const char) wxDialogNameStr[];

class WXDLLIMPEXP_FWD_CORE wxMacToolTip ;

// Dialog boxes
class WXDLLIMPEXP_CORE wxDialog : public wxDialogBase
{
    DECLARE_DYNAMIC_CLASS(wxDialog)

public:
    wxDialog() { Init(); }

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

    virtual ~wxDialog();

//    virtual bool Destroy();
    virtual bool Show(bool show = true);

    void SetModal(bool flag);
    virtual bool IsModal() const;

    // For now, same as Show(TRUE) but returns return code
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);

    // implementation
    // --------------

    // show modal dialog and enter modal loop
    void DoShowModal();

protected:
    // mac also takes command-period as cancel
    virtual bool IsEscapeKey(const wxKeyEvent& event);

private:
    void Init();

    bool m_isModalStyle;
};

#endif
    // _WX_DIALOG_H_
