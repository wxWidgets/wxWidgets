///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.h
// Purpose:     wxCheckListBox class - a listbox with checkable items
//              Note: this is an optional class.
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKLST_H_
#define _WX_CHECKLST_H_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface "checklst.h"
#endif

#include "wx/listbox.h"

class wxCheckListBox : public wxCheckListBoxBase
{
    DECLARE_DYNAMIC_CLASS(wxCheckListBox)
        
public:
    // ctors
    wxCheckListBox();
    wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int nStrings = 0, 
        const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr);

    wxCheckListBox(wxWindow *parent, wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr);

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
    
    // items may be checked
    bool IsChecked(size_t uiIndex) const;
    void Check(size_t uiIndex, bool bCheck = TRUE);

    // override base class functions
    virtual int DoAppend(const wxString& item);
    virtual int FindString(const wxString& s) const;
    virtual void SetString(int n, const wxString& s);
    virtual wxString GetString(int n) const;

    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
private:
    void DoToggleItem( int item, int x );
private:
    DECLARE_EVENT_TABLE()
};

#endif
// _WX_CHECKLST_H_
