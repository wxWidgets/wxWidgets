///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/combobox.h
// Purpose:     the universal combobox
// Author:      Vadim Zeitlin
// Modified by:
// Created:     30.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_COMBOBOX_H_
#define _WX_UNIV_COMBOBOX_H_

#ifdef __GNUG__
    #pragma implementation "univcombobox.h"
#endif

// ----------------------------------------------------------------------------
// the actions supported by this control
// ----------------------------------------------------------------------------

// all actions of single line text controls are supported

// popup/dismiss the choice window
#define wxACTION_COMBOBOX_POPUP     _T("popup")
#define wxACTION_COMBOBOX_DISMISS   _T("dismiss")

// choose the next/prev/specified (by numArg) item
#define wxACTION_COMBOBOX_SELECT_NEXT _T("next")
#define wxACTION_COMBOBOX_SELECT_PREV _T("prev")
#define wxACTION_COMBOBOX_SELECT      _T("select")

// ----------------------------------------------------------------------------
// wxComboControl: a combination of a (single line) text control with a button
// opening a popup window which contains the control from which the user can
// choose the value directly.
// ----------------------------------------------------------------------------

class wxComboControl : public wxControl
{
public:
    // construction
    wxComboControl()
    {
        Init();
    }

    wxComboControl(wxWindow *parent,
                   wxWindowID id,
                   const wxString& value = wxEmptyString,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxComboBoxNameStr)
    {
        Init();

        (void)Create(parent, id, value, pos, size, style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    // a combo control needs a control for popup window it displays
    void SetPopupControl(wxControl *control);
    wxControl *GetPopupControl() const { return m_ctrlPopup; }

    // operations
    void ShowPopup();
    void HidePopup();

    // implementation only from now on
    // -------------------------------

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = TRUE);
    virtual bool Show(bool show = TRUE);

protected:
    // override the base class virtuals involved into geometry calculations
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    // event handlers
    void OnButton(wxCommandEvent& event);

    // common part of all ctors
    void Init();

private:
    // the text control all the time and the one we popup when m_btn is pressed
    wxTextCtrl *m_text;
    wxButton *m_btn;
    wxControl *m_ctrlPopup;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxComboBox: a combination of text control and a listbox
// ----------------------------------------------------------------------------

class wxComboBox : public wxComboListBoxBase
{
public:
    // ctors and such
    wxComboBox(wxWindow *parent,
               wxWindowID id,
               const wxString& value = wxEmptyString,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0,
               const wxString *choices = (const wxString *) NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxComboBoxNameStr)
    {
        (void)Create(parent, id, value, pos, size, n, choices,
                     style, validator, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& value = wxEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                int n = 0,
                const wxString choices[] = (const wxString *) NULL,
                long style = 0,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxComboBoxNameStr);

    // implement the combobox interface

    // wxControlWithItems methods
    virtual void Clear();
    virtual void Delete(int n);

    virtual int GetCount() const { return (int)m_strings.GetCount(); }
    virtual wxString GetString(int n) const { return m_strings[n]; }
    virtual void SetString(int n, const wxString& s);
    virtual int FindString(const wxString& s) const
        { return m_strings.Index(s); }

    virtual bool IsSelected(int n) const
        { return m_selections.Index(n) != wxNOT_FOUND; }
    virtual void SetSelection(int n, bool select = TRUE);
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

protected:
    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);

    virtual void DoSetFirstItem(int n);

    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;

public:
    // the wxUniversal-specific methods
    // --------------------------------

    // override the wxControl virtual methods
    virtual bool PerformAction(const wxControlAction& action,
                               long numArg = 0l,
                               const wxString& strArg = wxEmptyString);

protected:
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoDraw(wxControlRenderer *renderer);
    virtual wxBorder GetDefaultBorder() const;

    virtual wxString GetInputHandlerType() const;

    // common part of all ctors
    void Init();

    // event handlers
    void OnSize(wxSizeEvent& event);

    // common part of Clear() and DoSetItems(): clears everything
    virtual void DoClear();

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxComboBox)
};

#endif // _WX_UNIV_COMBOBOX_H_
