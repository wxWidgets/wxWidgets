///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/radiobox.h
// Purpose:     wxRadioBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_RADIOBOX_H_
#define _WX_UNIV_RADIOBOX_H_

class WXDLLEXPORT wxRadioButton;

#include "wx/statbox.h"
#include "wx/dynarray.h"

WX_DEFINE_EXPORTED_ARRAY_PTR(wxRadioButton *, wxArrayRadioButtons);

// ----------------------------------------------------------------------------
// wxRadioBox: a box full of radio buttons
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRadioBox : public wxStaticBox,
                               public wxRadioBoxBase
{
public:
    // wxRadioBox construction
    wxRadioBox() { Init(); }

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString *choices = NULL,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr)
    {
        Init();

        (void)Create(parent, id, title, pos, size, n, choices,
                     majorDim, style, val, name);
    }
    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_SPECIFY_COLS,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr);

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString *choices = NULL,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_SPECIFY_COLS,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);

    virtual ~wxRadioBox();

    // implement wxRadioBox interface
    virtual void SetSelection(int n);
    virtual int GetSelection() const;

    virtual size_t GetCount() const { return m_buttons.GetCount(); }

    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& label);

    virtual bool Enable(int n, bool enable = true);
    virtual bool Show(int n, bool show = true);

    virtual bool IsItemEnabled(int n) const;
    virtual bool IsItemShown(int n) const;

    // we also override the wxControl methods to avoid virtual function hiding
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual wxString GetLabel() const;
    virtual void SetLabel(const wxString& label);

    // we inherit a version returning false from wxStaticBox, override it again
    virtual bool AcceptsFocus() const { return true; }

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

    // wxUniversal-only methods

    // another Append() version
    void Append(int n, const wxString *choices);

    // implementation only: called by wxRadioHookHandler
    void OnRadioButton(wxEvent& event);
    bool OnKeyDown(wxKeyEvent& event);

protected:
    // override the base class methods dealing with window positioning/sizing
    // as we must move/size the buttons as well
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual wxSize DoGetBestClientSize() const;

    // generate a radiobutton click event for the current item
    void SendRadioEvent();

    // common part of all ctors
    void Init();

    // calculate the max size of all buttons
    wxSize GetMaxButtonSize() const;

    // the currently selected radio button or -1
    int m_selection;

    // all radio buttons
    wxArrayRadioButtons m_buttons;

    // the event handler which is used to translate radiobutton events into
    // radiobox one
    wxEvtHandler *m_evtRadioHook;

private:
    DECLARE_DYNAMIC_CLASS(wxRadioBox)
};

#endif // _WX_UNIV_RADIOBOX_H_
