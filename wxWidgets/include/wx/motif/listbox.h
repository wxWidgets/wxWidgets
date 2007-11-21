/////////////////////////////////////////////////////////////////////////////
// Name:        wx/motif/listbox.h
// Purpose:     wxListBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOX_H_
#define _WX_LISTBOX_H_

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

    virtual ~wxListBox();

    // implementation of wxControlWithItems
    virtual unsigned int GetCount() const;
    virtual int DoAppend(const wxString& item);
    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoSetItemClientObject(unsigned int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(unsigned int n) const;
    virtual int GetSelection() const;
    virtual void Delete(unsigned int n);
    virtual int FindString(const wxString& s, bool bCase = false) const;
    virtual void Clear();
    virtual void SetString(unsigned int n, const wxString& s);
    virtual wxString GetString(unsigned int n) const;

    // implementation of wxListBoxbase
    virtual void DoSetSelection(int n, bool select);
    virtual void DoInsertItems(const wxArrayString& items, unsigned int pos);
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
    virtual void DoToggleItem(int WXUNUSED(item), int WXUNUSED(x)) {};
#endif
protected:
    virtual wxSize DoGetBestSize() const;

    unsigned int m_noItems;

    // List mapping positions->client data
    wxClientDataDictionary m_clientDataDict;
private:
    void SetSelectionPolicy();
};

#endif
// _WX_LISTBOX_H_
