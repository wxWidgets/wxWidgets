/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "combobox.h"
#endif

#include "wx/choice.h"

// Combobox item
class WXDLLEXPORT wxComboBox: public wxChoice
{
    DECLARE_DYNAMIC_CLASS(wxComboBox)
        
public:
    wxComboBox() { m_inSetSelection = false; }
    ~wxComboBox();
    
    inline wxComboBox(wxWindow *parent, wxWindowID id,
        const wxString& value = wxEmptyString,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxComboBoxNameStr)
    {
        m_inSetSelection = false;
        Create(parent, id, value, pos, size, n, choices,
               style, validator, name);
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
        m_inSetSelection = false;
        Create(parent, id, value, pos, size, choices,
               style, validator, name);
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
    
    // implementation of wxControlWithItems
    virtual int DoAppend(const wxString& item);
    virtual int DoInsert(const wxString& item, int pos);
    virtual void Delete(int n);
    virtual void Clear();
    virtual int GetSelection() const ;
    virtual void SetSelection(int n);
    virtual int FindString(const wxString& s) const;
    virtual wxString GetString(int n) const ;
    virtual void SetString(int n, const wxString& s);

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
    virtual long GetLastPosition() const ;
    virtual void Replace(long from, long to, const wxString& value);
    virtual void Remove(long from, long to);
    virtual void SetSelection(long from, long to);
    virtual void SetEditable(bool editable);
    
    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = TRUE);
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    WXWidget GetTopWidget() const { return m_mainWidget; }
    WXWidget GetMainWidget() const { return m_mainWidget; }

    virtual wxSize DoGetBestSize() const;
protected:
    virtual void DoSetSize(int x, int y,
        int width, int height,
        int sizeFlags = wxSIZE_AUTO);
private:
    // only implemented for native combo box
    void AdjustDropDownListSize();

    // implementation detail, should really be private
public:
    bool m_inSetSelection;
};

#endif
// _WX_COMBOBOX_H_
