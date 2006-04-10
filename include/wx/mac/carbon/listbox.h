/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/carbon/listbox.h
// Purpose:     wxListBox class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOX_H_
#define _WX_LISTBOX_H_

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------
#include  "wx/dynarray.h"
#include  "wx/arrstr.h"

// forward decl for GetSelections()
class wxArrayInt;

// forward decl for GetPeer() 
class wxMacListControl;

// List box item

WX_DEFINE_ARRAY( char* , wxListDataArray );

// ----------------------------------------------------------------------------
// List box control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListBox : public wxListBoxBase
{
public:
    // ctors and such
    wxListBox();

    wxListBox(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0, const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr)
    {
        Create(parent, id, pos, size, n, choices, style, validator, name);
    }

    wxListBox(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr)
    {
        Create(parent, id, pos, size, choices, style, validator, name);
    }

    bool Create(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize,
        int n = 0,
        const wxString choices[] = NULL,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr);

    bool Create(
        wxWindow *parent,
        wxWindowID id,
        const wxPoint& pos,
        const wxSize& size,
        const wxArrayString& choices,
        long style = 0,
        const wxValidator& validator = wxDefaultValidator,
        const wxString& name = wxListBoxNameStr);

    virtual ~wxListBox();

    // implement base class pure virtuals
    virtual void Refresh(bool eraseBack = true, const wxRect *rect = NULL);
    virtual void Clear();
    virtual void Delete(unsigned int n);

    virtual unsigned int GetCount() const;
    virtual wxString GetString(unsigned int n) const;
    virtual void SetString(unsigned int n, const wxString& s);
    virtual int FindString(const wxString& s, bool bCase = false) const;

    virtual bool IsSelected(int n) const;
    virtual int GetSelection() const;
    virtual int GetSelections(wxArrayInt& aSelections) const;

    // wxCheckListBox support
    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    wxMacListControl * GetPeer() const { return (wxMacListControl*)m_peer; }

    wxArrayString m_stringArray;
    wxListDataArray m_dataArray;
    
protected:
    // common creation for all databrowser list implementations
    wxMacListControl* CreateMacListControl(const wxPoint& pos, const wxSize& size, long style);
    
    // internal storage for line n has changed, issue a redraw
    void MacUpdateLine(int n);
    
    virtual void DoSetSelection(int n, bool select);
    virtual int DoAppend(const wxString& item);
    virtual void DoInsertItems(const wxArrayString& items, unsigned int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoSetFirstItem(int n);
    virtual void DoSetItemClientData(unsigned int n, void* clientData);
    virtual void* DoGetItemClientData(unsigned int n) const;
    virtual void DoSetItemClientObject(unsigned int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(unsigned int n) const;
    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);
    virtual int DoListHitTest(const wxPoint& point) const;
    virtual wxSize DoGetBestSize() const;

    // free memory (common part of Clear() and dtor)
    // prevent collision with some BSD definitions of macro Free()
    void FreeData();

    unsigned int m_noItems;

private:
    DECLARE_DYNAMIC_CLASS(wxListBox)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_LISTBOX_H_
