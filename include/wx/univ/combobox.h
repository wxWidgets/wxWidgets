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

/*
   A few words about all the classes defined in this file are probably in
   order: why do we need extra wxComboControl and wxComboPopup classes?

   This is because a traditional combobox is a combination of a text control
   (with a button allowing to open the pop down list) with a listbox and
   wxComboBox class is exactly such control, however we want to also have other
   combinations - in fact, we want to allow anything at all to be used as pop
   down list, not just a wxListBox.

   So we define a base wxComboControl which can use any control as pop down
   list and wxComboBox deriving from it which implements the standard wxWindows
   combobox API. wxComboControl needs to be told somehow which control to use
   and this is done by SetPopupControl(). However, we need something more than
   just a wxControl in this method as, for example, we need to call
   SetSelection("initial text value") and wxControl doesn't have such method.
   So we also need a wxComboPopup which is just a very simple interface which
   must be implemented by a control to be usable as a popup.

   We couldn't derive wxComboPopup from wxControl as this would make it
   impossible to have a class deriving from both wxListBx and from it, so
   instead it is just a mix-in.
 */

#ifndef _WX_UNIV_COMBOBOX_H_
#define _WX_UNIV_COMBOBOX_H_

#ifdef __GNUG__
    #pragma implementation "univcombobox.h"
#endif

class WXDLLEXPORT wxComboControl;
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
// wxComboPopup is the interface which must be implemented by a control to be
// used as a popup by wxComboControl
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxComboPopup
{
public:
    wxComboPopup(wxComboControl *combo) { m_combo = combo; }

    // we must have an associated control which is subclassed by the combobox
    virtual wxControl *GetControl() = 0;

    // called before showing the control to set the initial selection - notice
    // that the text passed to this method might not correspond to any valid
    // item (if the user edited it directly), in which case the method should
    // just return FALSE but not emit any errors
    virtual bool SetSelection(const wxString& value) = 0;

    // called immediately after the control is shown
    virtual void OnShow() = 0;

protected:
    wxComboControl *m_combo;
};

// ----------------------------------------------------------------------------
// wxComboControl: a combination of a (single line) text control with a button
// opening a popup window which contains the control from which the user can
// choose the value directly.
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxComboControl : public wxControl
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
    void SetPopupControl(wxComboPopup *popup);
    wxComboPopup *GetPopupControl() const { return m_popup; }

    // operations
    void ShowPopup();
    void HidePopup();

    // implementation only from now on
    // -------------------------------

    // notifications from wxComboPopup (shouldn't be called by anybody else)

    // called when the user selects something in the popup: this normally hides
    // the popup and sets the text to the new value
    virtual void OnSelect(const wxString& value);

    // called when the user dismisses the popup
    virtual void OnDismiss();

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = TRUE);
    virtual bool Show(bool show = TRUE);

protected:
    // override the base class virtuals involved into geometry calculations
    virtual wxSize DoGetBestClientSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    // event handlers
    void OnButton(wxCommandEvent& event);

    // common part of all ctors
    void Init();

    // access the control components
    wxTextCtrl *GetText() const { return m_text; }

    // remove the combobox event handler from m_popup (ok to call if m_popup ==
    // NULL)
    void RemoveEventHandler();

private:
    // the text control and button we show all the time
    wxTextCtrl *m_text;
    wxButton *m_btn;

    // the popup control
    wxComboPopup *m_popup;

    // the height of the combobox popup as calculated in Create()
    wxCoord m_heightPopup;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxComboBox: a combination of text control and a listbox
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxComboBox : public wxComboControl, public wxComboBoxBase
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


    virtual ~wxComboBox();

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
