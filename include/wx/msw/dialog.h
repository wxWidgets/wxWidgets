/////////////////////////////////////////////////////////////////////////////
// Name:        dialog.h
// Purpose:     wxDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DIALOG_H_
#define _WX_DIALOG_H_

#ifdef __GNUG__
    #pragma interface "dialog.h"
#endif

#include "wx/panel.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxDialogNameStr;

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
        Create(parent, -1, title, wxPoint(x, y), wxSize(width, height),
               style | modalStyle, name);
    }

    // Constructor with no modal flag - the new convention.
    wxDialog(wxWindow *parent, wxWindowID id,
             const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxDialogNameStr)
    {
        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxDialogNameStr);

    ~wxDialog();

    // override some base class virtuals
    virtual bool Destroy();
    virtual bool Show(bool show);
    virtual void Iconize(bool iconize);
    virtual bool IsIconized() const;

    virtual bool IsTopLevel() const { return TRUE; }

    void SetModal(bool flag);
    virtual bool IsModal() const;

    // For now, same as Show(TRUE) but returns return code
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);

    // returns TRUE if we're in a modal loop
    bool IsModalShowing() const;

#if WXWIN_COMPATIBILITY
    bool Iconized() const { return IsIconized(); };
#endif

    // implementation only from now on
    // -------------------------------

    // event handlers
    bool OnClose();
    void OnCharHook(wxKeyEvent& event);
    void OnCloseWindow(wxCloseEvent& event);

    // Standard buttons
    void OnOK(wxCommandEvent& event);
    void OnApply(wxCommandEvent& event);
    void OnCancel(wxCommandEvent& event);

    // Responds to colour changes
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // Windows callbacks
    long MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

#if wxUSE_CTL3D
    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
#endif // wxUSE_CTL3D

protected:
    // override more base class virtuals
    virtual void DoSetClientSize(int width, int height);
    virtual void DoGetPosition(int *x, int *y) const;

    // show modal dialog and enter modal loop
    void DoShowModal();

    // common part of all ctors
    void Init();

private:
    wxWindow *m_oldFocus;

    // while we are showing a modal dialog we disable the other windows using
    // this object
    class wxWindowDisabler *m_windowDisabler;

    DECLARE_DYNAMIC_CLASS(wxDialog)
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_DIALOG_H_
