/////////////////////////////////////////////////////////////////////////////
// Name:        wx/palmos/radiobox.h
// Purpose:     wxRadioBox class
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by:
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RADIOBOX_H_
#define _WX_RADIOBOX_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface "radiobox.h"
#endif

class WXDLLEXPORT wxBitmap;

// ----------------------------------------------------------------------------
// wxRadioBox
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxRadioBox : public wxControl, public wxRadioBoxBase
{
public:
    wxRadioBox();

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
    virtual int GetSelection() const;
    virtual int GetCount() const;
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& label);
    virtual void Enable(int n, bool enable = TRUE);
    virtual void Show(int n, bool show = TRUE);
    virtual int GetColumnCount() const;
    virtual int GetRowCount() const;

    virtual bool Show(bool show = TRUE);
    void SetFocus();
    virtual bool Enable(bool enable = TRUE);
    void SetLabelFont(const wxFont& WXUNUSED(font)) {};
    void SetButtonFont(const wxFont& font) { SetFont(font); }

    void Command(wxCommandEvent& event);

    int GetNumberOfRowsOrCols() const { return m_noRowsOrCols; }
    void SetNumberOfRowsOrCols(int n) { m_noRowsOrCols = n; }

    // implementation only from now on
    // -------------------------------

    virtual bool MSWCommand(WXUINT param, WXWORD id);

    // FIXME: are they used? missing "Do" prefix?
    void GetSize(int *x, int *y) const;
    void GetPosition(int *x, int *y) const;

    virtual bool SetFont(const wxFont& font);

    WXLRESULT MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                WXUINT message,
                                WXWPARAM wParam, WXLPARAM lParam);
    WXHWND *GetRadioButtons() const { return m_radioButtons; }
    bool ContainsHWND(WXHWND hWnd) const;
    void SendNotificationEvent();

    // get the number of buttons per column/row
    int GetNumVer() const;
    int GetNumHor() const;

    virtual void ApplyParentThemeBackground(const wxColour& bg)
        { SetBackgroundColour(bg); }

protected:
    // we can't compute our best size before the items are added to the control
    virtual void SetInitialBestSize(const wxSize& WXUNUSED(size)) { }

    // subclass one radio button
    void SubclassRadioButton(WXHWND hWndBtn);

    // get the max size of radio buttons
    wxSize GetMaxButtonSize() const;

    // get the total size occupied by the radio box buttons
    wxSize GetTotalButtonSize(const wxSize& sizeBtn) const;

    WXHWND *          m_radioButtons;
    int               m_majorDim;
    int *             m_radioWidth;  // for bitmaps
    int *             m_radioHeight;

    int               m_noItems;
    int               m_noRowsOrCols;
    int               m_selectedButton;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual wxSize DoGetBestSize() const;

private:
    DECLARE_DYNAMIC_CLASS(wxRadioBox)
    DECLARE_NO_COPY_CLASS(wxRadioBox)
};

#endif
    // _WX_RADIOBOX_H_
