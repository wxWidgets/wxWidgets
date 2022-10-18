/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/combobox.h
// Purpose:     wxComboBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#include "wx/containr.h"
#include "wx/choice.h"
#include "wx/textctrl.h"

WX_DEFINE_ARRAY( char * , wxComboBoxDataArray ) ;

// forward declaration of private implementation classes

class wxComboBoxText;
class wxComboBoxChoice;
class wxComboWidgetImpl;

// Combobox item
class WXDLLIMPEXP_CORE wxComboBox :
    public wxWindowWithItems<
                wxControl,
                wxComboBoxBase>
{
    wxDECLARE_DYNAMIC_CLASS(wxComboBox);

 public:
    virtual ~wxComboBox();

    // callback functions
    virtual void DelegateTextChanged( const wxString& value );
    virtual void DelegateChoice( const wxString& value );

    wxComboBox() { }

    wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = nullptr,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxComboBoxNameStr))
    {
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }

    wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxComboBoxNameStr))
    {
        Create(parent, id, value, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = nullptr,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxASCII_STR(wxComboBoxNameStr));

    virtual int GetSelection() const override;
    virtual void GetSelection(long *from, long *to) const override;
    virtual void SetSelection(int n) override;
    virtual void SetSelection(long from, long to) override;
    virtual int FindString(const wxString& s, bool bCase = false) const override;
    virtual wxString GetString(unsigned int n) const override;
    virtual wxString GetStringSelection() const override;
    virtual void SetString(unsigned int n, const wxString& s) override;

    virtual unsigned int GetCount() const override;

    virtual void SetValue(const wxString& value) override;
// these methods are provided by wxTextEntry for the native impl.

#if wxOSX_USE_COCOA
    virtual void Popup() override;
    virtual void Dismiss() override;
#endif // wxOSX_USE_COCOA


    virtual const wxTextEntry* WXGetTextEntry() const override { return this; }

    // osx specific event handling common for all osx-ports

    virtual bool OSXHandleClicked(double timestampsec) override;

#if wxOSX_USE_COCOA
    wxComboWidgetImpl* GetComboPeer() const;
#endif
protected:
    // List functions
    virtual void DoDeleteOneItem(unsigned int n) override;
    virtual void DoClear() override;

    // wxTextEntry functions
    virtual wxWindow *GetEditableWindow() override { return this; }

    // override the base class virtuals involved in geometry calculations
    virtual wxSize DoGetBestSize() const override;

    virtual int DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type) override;

    virtual void DoSetItemClientData(unsigned int n, void* clientData) override;
    virtual void * DoGetItemClientData(unsigned int n) const override;


    virtual void EnableTextChangedEvents(bool enable) override;

    // callbacks
    void OnChar(wxKeyEvent& event); // Process 'enter' if required
    void OnKeyDown(wxKeyEvent& event); // Process clipboard shortcuts

    // the subcontrols
    wxComboBoxText*     m_text;
    wxComboBoxChoice*   m_choice;

    wxComboBoxDataArray m_datas;

private:
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_COMBOBOX_H_
