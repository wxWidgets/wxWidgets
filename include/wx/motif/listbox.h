/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:     wxListBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOX_H_
#define _WX_LISTBOX_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "listbox.h"
#endif

#include "wx/ctrlsub.h"
#include "wx/clntdata.h"

// forward decl for GetSelections()
class WXDLLEXPORT wxArrayInt;

// List box item
class WXDLLEXPORT wxListBox: public wxListBoxBase
{
    DECLARE_DYNAMIC_CLASS(wxListBox)
        
public:
    wxListBox();
    wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    
    wxListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr)
    {
        Create(parent, id, pos, size, choices, style, validator, name);
    }
    
    bool Create(wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr);
    
    bool Create(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr);
    
    ~wxListBox();
    
    // implementation of wxControlWithItems
    virtual int GetCount() const;
    virtual int DoAppend(const wxString& item);
    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;
    virtual int GetSelection() const;
    virtual void Delete(int n);
    virtual int FindString(const wxString& s) const;
    virtual void Clear();
    virtual void SetString(int n, const wxString& s);
    virtual wxString GetString(int n) const;

    // implementation of wxListBoxbase
    virtual void SetSelection(int n, bool select = true);
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoSetFirstItem(int n);
    virtual int GetSelections(wxArrayInt& aSelections) const;
    virtual bool IsSelected(int n) const;

    // For single or multiple choice list item
    void Command(wxCommandEvent& event);
    
    // Implementation
    virtual void ChangeBackgroundColour();
    virtual void ChangeForegroundColour();
    WXWidget GetTopWidget() const;

#if wxUSE_CHECKLISTBOX
    virtual void DoToggleItem(int item, int x) {};
#endif
protected:
    virtual wxSize DoGetBestSize() const;

    int       m_noItems;
    
    // List mapping positions->client data
    wxClientDataDictionary m_clientDataDict;
private:
    void SetSelectionPolicy();
};

#endif
// _WX_LISTBOX_H_
