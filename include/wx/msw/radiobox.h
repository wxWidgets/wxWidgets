/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/radiobox.h
// Purpose:     wxRadioBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_
#define _WX_RADIOBOX_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "radiobox.h"
#endif

#include "wx/statbox.h"

class WXDLLEXPORT wxSubwindows;

// ----------------------------------------------------------------------------
// wxRadioBox
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRadioBox : public wxStaticBox, public wxRadioBoxBase
{
public:
    wxRadioBox() { Init(); }

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = NULL,
               int majorDim = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr)
    {
        Init();

        (void)Create(parent, id, title, pos, size, n, choices, majorDim,
                     style, val, name);
    }

    wxRadioBox(wxWindow *parent,
               wxWindowID id,
               const wxString& title,
               const wxPoint& pos,
               const wxSize& size,
               const wxArrayString& choices,
               int majorDim = 0,
               long style = wxRA_HORIZONTAL,
               const wxValidator& val = wxDefaultValidator,
               const wxString& name = wxRadioBoxNameStr)
    {
        Init();

        (void)Create(parent, id, title, pos, size, choices, majorDim,
                     style, val, name);
    }

    ~wxRadioBox();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0, const wxString choices[] = NULL,
                int majorDim = 0,
                long style = wxRA_HORIZONTAL,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                const wxArrayString& choices,
                int majorDim = 0,
                long style = wxRA_HORIZONTAL,
                const wxValidator& val = wxDefaultValidator,
                const wxString& name = wxRadioBoxNameStr);

    // implement the radiobox interface
    virtual void SetSelection(int n);
    virtual int GetSelection() const { return m_selectedButton; }
    virtual int GetCount() const;
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& label);
    virtual void Enable(int n, bool enable = true);
    virtual void Show(int n, bool show = true);
    virtual int GetColumnCount() const { return GetNumHor(); }
    virtual int GetRowCount() const { return GetNumVer(); }

    // override some base class methods
    virtual bool Show(bool show = true);
    virtual bool Enable(bool enable = true);
    virtual void SetFocus();
    virtual bool SetFont(const wxFont& font);
    virtual bool ContainsHWND(WXHWND hWnd) const;

    void SetLabelFont(const wxFont& WXUNUSED(font)) {}
    void SetButtonFont(const wxFont& font) { SetFont(font); }


    // implementation only from now on
    // -------------------------------

    virtual bool MSWCommand(WXUINT param, WXWORD id);
    void Command(wxCommandEvent& event);

    void SendNotificationEvent();

    // get the number of buttons per column/row
    int GetNumVer() const;
    int GetNumHor() const;

protected:
    // common part of all ctors
    void Init();

    // we can't compute our best size before the items are added to the control
    virtual void SetInitialBestSize(const wxSize& WXUNUSED(size)) { }

    // subclass one radio button
    void SubclassRadioButton(WXHWND hWndBtn);

    // get the max size of radio buttons
    wxSize GetMaxButtonSize() const;

    // get the total size occupied by the radio box buttons
    wxSize GetTotalButtonSize(const wxSize& sizeBtn) const;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual wxSize DoGetBestSize() const;


    // the buttons we contain
    wxSubwindows *m_radioButtons;

    // array of widths and heights of the buttons, may be wxDefaultCoord if the
    // corresponding quantity should be computed
    int *m_radioWidth;
    int *m_radioHeight;

    // the number of elements in major dimension (i.e. number of columns if
    // wxRA_SPECIFY_COLS or the number of rows if wxRA_SPECIFY_ROWS)
    int m_majorDim;

    // currently selected button or wxNOT_FOUND if none
    int m_selectedButton;

private:
    DECLARE_DYNAMIC_CLASS(wxRadioBox)
    DECLARE_NO_COPY_CLASS(wxRadioBox)
};

#endif
    // _WX_RADIOBOX_H_
