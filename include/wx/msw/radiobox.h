/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
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

#ifdef __GNUG__
    #pragma interface "radiobox.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxRadioBoxNameStr;

class WXDLLEXPORT wxBitmap;

class WXDLLEXPORT wxRadioBox : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxRadioBox)

public:
    wxRadioBox();

    wxRadioBox(wxWindow *parent, wxWindowID id, const wxString& title,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            int majorDim = 0, long style = wxRA_HORIZONTAL,
            const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr)
    {
        Create(parent, id, title, pos, size, n, choices, majorDim, style, val, name);
    }

    ~wxRadioBox();

    bool Create(wxWindow *parent, wxWindowID id, const wxString& title,
            const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            int majorDim = 0, long style = wxRA_HORIZONTAL,
            const wxValidator& val = wxDefaultValidator, const wxString& name = wxRadioBoxNameStr);

    virtual bool MSWCommand(WXUINT param, WXWORD id);

    int FindString(const wxString& s) const;
    void SetSelection(int N);
    int GetSelection() const;
    wxString GetString(int N) const;

    void GetSize(int *x, int *y) const;
    void GetPosition(int *x, int *y) const;

    void SetLabel(int item, const wxString& label);
    void SetLabel(int item, wxBitmap *bitmap);
    wxString GetLabel(int item) const;
    bool Show(bool show);
    void SetFocus();
    bool Enable(bool enable);
    void Enable(int item, bool enable);
    void Show(int item, bool show);
    void SetLabelFont(const wxFont& WXUNUSED(font)) {};
    void SetButtonFont(const wxFont& font) { SetFont(font); }

    virtual wxString GetStringSelection() const;
    virtual bool SetStringSelection(const wxString& s);
    virtual int Number() const { return m_noItems; };
    void Command(wxCommandEvent& event);

    int GetNumberOfRowsOrCols() const { return m_noRowsOrCols; }
    void SetNumberOfRowsOrCols(int n) { m_noRowsOrCols = n; }

    // implementation only from now on
    // -------------------------------

    virtual bool SetFont(const wxFont& font);

    long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
    virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
            WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
    WXHWND *GetRadioButtons() const { return m_radioButtons; }
    bool ContainsHWND(WXHWND hWnd) const;
    void SendNotificationEvent();

    // get the number of buttons per column/row
    int GetNumVer() const;
    int GetNumHor() const;

    // compatibility ctor
#if WXWIN_COMPATIBILITY
    wxRadioBox(wxWindow *parent, wxFunction func, const char *title,
            int x = -1, int y = -1, int width = -1, int height = -1,
            int n = 0, char **choices = NULL,
            int majorDim = 0, long style = wxRA_HORIZONTAL, const char *name = wxRadioBoxNameStr);
#endif // WXWIN_COMPATIBILITY

protected:
    void SubclassRadioButton(WXHWND hWndBtn);

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
};

#endif
    // _WX_RADIOBOX_H_
