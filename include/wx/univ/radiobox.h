///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/radiobox.h
// Purpose:     wxRadioBox declaration
// Author:      Vadim Zeitlin
// Modified by:
// Created:     11.09.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_RADIOBOX_H_
#define _WX_UNIV_RADIOBOX_H_

#ifdef __GNUG__
    #pragma interface "univradiobox.h"
#endif

class WXDLLEXPORT wxRadioButton;

#include "wx/statbox.h"
#include "wx/dynarray.h"

WX_DEFINE_ARRAY(wxRadioButton *, wxArrayRadioButtons);

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

    virtual ~wxRadioBox();

    // implement wxRadioBox interface
    virtual void SetSelection(int n);
    virtual int GetSelection() const;

    virtual int GetCount() const { return m_buttons.GetCount(); }
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& label);

    virtual void Enable(int n, bool enable = TRUE);
    virtual void Show(int n, bool show = TRUE);

    // we also override the wxControl methods to avoid virtual function hiding
    virtual bool Enable(bool enable = TRUE);
    virtual bool Show(bool show = TRUE);
    virtual wxString GetLabel() const;
    virtual void SetLabel(const wxString& label);

    // wxUniversal-only methods

    // another Append() version
    void Append(int n, const wxString *choices);

    // implementation only: called by wxRadioHookHandler
    void OnRadioButton(wxEvent& event);

protected:
    // override the base class methods dealing with window positioning/sizing
    // as we must move/size the buttons as well
    virtual void DoMoveWindow(int x, int y, int width, int height);
    virtual wxSize DoGetBestClientSize() const;

    // common part of all ctors
    void Init();

    // check that the index is valid
    bool IsValid(int n) const { return n >= 0 && n < GetCount(); }

    // sets m_majorDim and calculate m_numCols and m_numRows
    void SetMajorDim(int majorDim);

    // calculate the max size of all buttons
    wxSize GetMaxButtonSize() const;

    // the currently selected radio button or -1
    int m_selection;

    // the parameters defining the button layout: majorDim meaning depends on
    // the style and is the (max) number of columns if it includes
    // wxRA_SPECIFY_COLS and is the (max) number of rows if it includes
    // wxRA_SPECIFY_ROWS - the number of rows and columns is calculated from
    // it
    int m_majorDim,
        m_numCols,
        m_numRows;

    // all radio buttons
    wxArrayRadioButtons m_buttons;

    // the event handler which is used to translate radiobutton events into
    // radiobox one
    wxEvtHandler *m_evtRadioHook;

private:
    DECLARE_DYNAMIC_CLASS(wxRadioBox)
};

#endif // _WX_UNIV_RADIOBOX_H_
