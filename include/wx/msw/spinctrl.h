////////////////////////////////////////////////////////////////////////////
// Name:        msw/spinctrl.h
// Purpose:     wxSpinCtrl class declaration for Win32
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.07.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MSW_SPINCTRL_H_
#define _WX_MSW_SPINCTRL_H_

#ifdef __GNUG__
    #pragma interface "spinctrl.h"
#endif

#include "wx/spinbutt.h"    // the base class

// ----------------------------------------------------------------------------
// Under Win32, wxSpinCtrl is a wxSpinButton with a buddy (as MSDN docs call
// it) text window whose contents is automatically updated when the spin
// control is clicked.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxSpinCtrl : public wxSpinButton
{
public:
    wxSpinCtrl() { }

    wxSpinCtrl(wxWindow *parent,
               wxWindowID id = -1,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = wxSP_ARROW_KEYS,
               int min = 0, int max = 100, int initial = 0,
               const wxString& name = _T("wxSpinCtrl"))
    {
        Create(parent, id, value, pos, size, style, min, max, initial, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxSP_ARROW_KEYS,
                int min = 0, int max = 100, int initial = 0,
                const wxString& name = _T("wxSpinCtrl"));

    // a wxTextCtrl-like method (but we can't have GetValue returning wxString
    // because the base class already has one returning int!)
    void SetValue(const wxString& text);

    // implementation only from now on
    // -------------------------------

    virtual ~wxSpinCtrl();

    virtual void SetValue(int val) { wxSpinButton::SetValue(val); }
    virtual int  GetValue() const;
    virtual bool SetFont(const wxFont &font);
    virtual void SetFocus();

    virtual bool Enable(bool enable = TRUE);
    virtual bool Show(bool show = TRUE);

    // wxSpinButton doesn't accept focus, but we do
    virtual bool AcceptsFocus() const { return wxWindow::AcceptsFocus(); }

    WXFARPROC GetBuddyWndProc() const { return m_oldBuddyWndProc; }

protected:
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual wxSize DoGetBestSize() const;
    virtual void DoGetSize(int *width, int *height) const;

    // the handler for wxSpinButton events
    void OnSpinChange(wxSpinEvent& event);

    // the data for the "buddy" text ctrl
    WXHWND     m_hwndBuddy;
    WXFARPROC  m_oldBuddyWndProc;

private:
    DECLARE_DYNAMIC_CLASS(wxSpinCtrl)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_MSW_SPINCTRL_H_


