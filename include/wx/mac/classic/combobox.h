/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "combobox.h"
#endif

#include "wx/choice.h"

WXDLLEXPORT_DATA(extern const wxChar*) wxComboBoxNameStr;

// Combobox item
class WXDLLEXPORT wxComboBox : public wxControl, public wxComboBoxBase
{
  DECLARE_DYNAMIC_CLASS(wxComboBox)

 public:
    inline wxComboBox() {}
    virtual ~wxComboBox();
    // override the base class virtuals involved in geometry calculations
    virtual wxSize DoGetBestSize() const;
    virtual void DoMoveWindow(int x, int y, int width, int height);

    // forward these functions to all subcontrols
    virtual bool Enable(bool enable = true);
    virtual bool Show(bool show = true);
    virtual void SetFocus();

    // callback functions
    virtual void DelegateTextChanged( const wxString& value );
    virtual void DelegateChoice( const wxString& value );

    inline wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr)
    {
    Create(parent, id, value, pos, size, n, choices, style, validator, name);
    }
    inline wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value,
           const wxPoint& pos,
           const wxSize& size,
           const wxArrayString& choices,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxComboBoxNameStr)
    {
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

    // List functions
    virtual void Delete(int n);
    virtual void Clear();

    virtual int GetSelection() const ;
    virtual void SetSelection(int n);
    virtual void Select(int n) { SetSelection(n) ; }
    virtual int FindString(const wxString& s) const;
    virtual wxString GetString(int n) const ;
    virtual wxString GetStringSelection() const ;
    virtual void SetString(int n, const wxString& s) ;

    // Text field functions
    virtual wxString GetValue() const ;
    virtual void SetValue(const wxString& value);

    // Clipboard operations
    virtual void Copy();
    virtual void Cut();
    virtual void Paste();
    virtual void SetInsertionPoint(long pos);
    virtual void SetInsertionPointEnd();
    virtual long GetInsertionPoint() const ;
    virtual wxTextPos GetLastPosition() const ;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);
    virtual int GetCount() const { return m_choice->GetCount() ; }

    virtual bool IsEditable() const ;

    virtual void Undo() ;
    virtual void Redo() ;
    virtual void SelectAll() ;

    virtual bool CanCopy() const ;
    virtual bool CanCut() const ;
    virtual bool CanPaste() const ;
    virtual bool CanUndo() const ;
    virtual bool CanRedo() const ;

    void MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool mouseStillDown ) ;

    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

protected:
    virtual int DoAppend(const wxString& item) ;
    virtual int DoInsert(const wxString& item, int pos) ;

    virtual void DoSetItemClientData(int n, void* clientData) ;
    virtual void* DoGetItemClientData(int n) const ;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData) ;
    virtual wxClientData* DoGetItemClientObject(int n) const ;

    void FreeData();

    // the subcontrols
    wxTextCtrl*     m_text;
    wxChoice*       m_choice;
};

#endif
    // _WX_COMBOBOX_H_
