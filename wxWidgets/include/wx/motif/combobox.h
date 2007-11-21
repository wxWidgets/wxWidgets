/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/combobox.h
// Purpose:     wxComboBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMBOBOX_H_
#define _WX_COMBOBOX_H_

#include "wx/choice.h"

// Combobox item
class WXDLLEXPORT wxComboBox: public wxChoice
{
    DECLARE_DYNAMIC_CLASS(wxComboBox)

public:
    wxComboBox() { m_inSetSelection = false; }
    virtual ~wxComboBox();

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
    virtual int DoInsert(const wxString& item, unsigned int pos);
    virtual void Delete(unsigned int n);
    virtual void Clear();
    virtual int GetSelection() const ;
    virtual void SetSelection(int n);
    virtual int FindString(const wxString& s, bool bCase = false) const;
    virtual wxString GetString(unsigned int n) const ;
    virtual void SetString(unsigned int n, const wxString& s);

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

    // Implementation
    virtual void ChangeFont(bool keepOriginalSize = true);
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
