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

class WXDLLEXPORT wxListBox;

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

    virtual ~wxComboControl();

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

    // access the control components
    wxTextCtrl *GetText() const { return m_text; }

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

class wxComboBox : public wxComboControl, public wxComboBoxBase
{
public:
    // ctors and such
    wxComboBox() { Init(); }

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
        Init();

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

    // the wxUniversal-specific methods
    // --------------------------------

    // implement the combobox interface

    // wxTextCtrl methods
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual long GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);

    // wxControlWithItems methods
    virtual void Clear();
    virtual void Delete(int n);
    virtual int GetCount() const;
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& s);
    virtual int FindString(const wxString& s) const;
    virtual void Select(int n);
    virtual int GetSelection() const;
    void SetSelection(int n) { Select(n); }

protected:
    virtual int DoAppend(const wxString& item);
    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;

    // common part of all ctors
    void Init();

    // get the associated listbox
    wxListBox *GetLBox() const { return m_lbox; }

private:
    // the popup listbox
    wxListBox *m_lbox;

    //DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxComboBox)
};

#endif // _WX_UNIV_COMBOBOX_H_
