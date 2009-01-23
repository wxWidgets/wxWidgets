/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/combobox.h
// Purpose:     wxComboBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#include "wx/containr.h"
#include "wx/choice.h"

WXDLLIMPEXP_DATA_CORE(extern const char) wxComboBoxNameStr[];

// forward declaration of private implementation classes

class wxComboBoxText;
class wxComboBoxChoice;

// Combobox item
class WXDLLIMPEXP_CORE wxComboBox : public wxControl, public wxComboBoxBase
{
    DECLARE_DYNAMIC_CLASS(wxComboBox)

 public:
    virtual ~wxComboBox();

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);

    // callback functions
    virtual void DelegateTextChanged( const wxString& value );
    virtual void DelegateChoice( const wxString& value );

    wxComboBox() { Init(); }

    wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr)
    {
        Init();
        Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }

    wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr)
    {
        Init();
        Create(parent, id, value, pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr);

    bool Create(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr);

    virtual int GetSelection() const;
    virtual void SetSelection(int n);
    virtual int FindString(const wxString& s, bool bCase = false) const;
    virtual wxString GetString(unsigned int n) const;
    virtual wxString GetStringSelection() const;
    virtual void SetString(unsigned int n, const wxString& s);

    // Text field functions
    virtual void SetValue(const wxString& value);
    virtual wxString GetValue() const;
    virtual void WriteText(const wxString& text);
    virtual void GetSelection(long *from, long *to) const;

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const;
    virtual wxTextPos GetLastPosition() const;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);
    virtual bool IsEditable() const;

    virtual unsigned int GetCount() const;

    virtual void Undo();
    virtual void Redo();
    virtual void SelectAll();

    virtual bool CanCopy() const;
    virtual bool CanCut() const;
    virtual bool CanPaste() const;
    virtual bool CanUndo() const;
    virtual bool CanRedo() const;

    virtual wxClientDataType GetClientDataType() const;

    // osx specific event handling common for all osx-ports
    
    virtual bool        OSXHandleClicked( double timestampsec );

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

    WX_DECLARE_CONTROL_CONTAINER();

protected:
    // common part of all ctors
    void Init();

    // List functions
    virtual void DoDeleteOneItem(unsigned int n);
    virtual void DoClear();

    // override the base class virtuals involved in geometry calculations
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    virtual int DoInsertItems(const wxArrayStringsAdapter& items,
                              unsigned int pos,
                              void **clientData, wxClientDataType type);

    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void * DoGetItemClientData(unsigned int n) const;

    virtual void SetClientDataType(wxClientDataType clientDataItemsType);

    // the subcontrols
    wxComboBoxText*     m_text;
    wxComboBoxChoice*   m_choice;

    DECLARE_EVENT_TABLE()
};

#endif // _WX_COMBOBOX_H_
