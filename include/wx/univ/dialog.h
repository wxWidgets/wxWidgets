/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:     wxDialog class
// Author:      Vaclav Slavik
// Created:     2001/09/16
// RCS-ID:      $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_DIALOG_H_
#define _WX_UNIV_DIALOG_H_

#ifdef __GNUG__
    #pragma interface "univdialog.h"
#endif

WXDLLEXPORT_DATA(extern const wxChar*) wxDialogNameStr;
class WXDLLEXPORT wxWindowDisabler;
class WXDLLEXPORT wxEventLoop;

// Dialog boxes
class WXDLLEXPORT wxDialog : public wxDialogBase
{
public:
    wxDialog() { Init(); }

    // Constructor with a modal flag, but no window id - the old convention
    wxDialog(wxWindow *parent,
             const wxString& title, bool modal,
             int x = -1, int y= -1, int width = 500, int height = 500,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
    {
        long modalStyle = modal ? wxDIALOG_MODAL : wxDIALOG_MODELESS ;
        Init();
        Create(parent, -1, title, wxPoint(x, y), wxSize(width, height),
               style | modalStyle, name);
    }
    
    ~wxDialog();

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

    void SetModal(bool flag);
    virtual bool IsModal() const;

    // For now, same as Show(TRUE) but returns return code
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);

    // returns TRUE if we're in a modal loop
    bool IsModalShowing() const;

    bool Show(bool show = TRUE);

    // implementation only from now on
    // -------------------------------

    // event handlers
    void OnCloseWindow(wxCloseEvent& event);
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

protected:
    // common part of all ctors
    void Init();

private:
    // while we are showing a modal dialog we disable the other windows using
    // this object
    wxWindowDisabler *m_windowDisabler;
    // modal dialog runs its own event loop
    wxEventLoop *m_eventLoop;
    // is modal right now?
    bool m_isShowingModal;

    DECLARE_DYNAMIC_CLASS(wxDialog)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_UNIV_DIALOG_H_
