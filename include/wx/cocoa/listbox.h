/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/listbox.h
// Purpose:     wxListBox class
// Author:      David Elliott
// Modified by:
// Created:     2003/03/16
// RCS-ID:      $Id:
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_LISTBOX_H__
#define __WX_COCOA_LISTBOX_H__

#include "wx/cocoa/NSTableView.h"

#include "wx/dynarray.h"

// ========================================================================
// wxListBox
// ========================================================================
class WXDLLEXPORT wxListBox: public wxListBoxBase, protected wxCocoaNSTableView
{
    DECLARE_DYNAMIC_CLASS(wxListBox)
    DECLARE_EVENT_TABLE()
    WX_DECLARE_COCOA_OWNER(NSTableView,NSControl,NSView)
// ------------------------------------------------------------------------
// initialization
// ------------------------------------------------------------------------
public:
    wxListBox() { m_cocoaItems = NULL; }
    wxListBox(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr)
    {
        Create(parent, winid,  pos, size, n, choices, style, validator, name);
    }
    wxListBox(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr)
    {
        Create(parent, winid,  pos, size, choices, style, validator, name);
    }

    bool Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            int n = 0, const wxString choices[] = NULL,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr);
    bool Create(wxWindow *parent, wxWindowID winid,
            const wxPoint& pos,
            const wxSize& size,
            const wxArrayString& choices,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxListBoxNameStr);
    virtual ~wxListBox();

// ------------------------------------------------------------------------
// Cocoa callbacks
// ------------------------------------------------------------------------
protected:
    virtual int CocoaDataSource_numberOfRows();
    virtual struct objc_object* CocoaDataSource_objectForTableColumn(
        WX_NSTableColumn tableColumn, int rowIndex);
    WX_NSMutableArray m_cocoaItems;
    wxArrayPtrVoid m_itemClientData;
    struct objc_object *m_cocoaDataSource;
// ------------------------------------------------------------------------
// Implementation
// ------------------------------------------------------------------------
public:
// pure virtuals from wxListBoxBase
    virtual bool IsSelected(int n) const;
    virtual int GetSelections(wxArrayInt& aSelections) const;
protected:
    virtual void DoInsertItems(const wxArrayString& items, int pos);
    virtual void DoSetItems(const wxArrayString& items, void **clientData);
    virtual void DoSetFirstItem(int n);
    virtual void DoSetSelection(int n, bool select);

// pure virtuals from wxItemContainer
public:
    // deleting items
    virtual void Clear();
    virtual void Delete(int n);
    // accessing strings
    virtual int GetCount() const;
    virtual wxString GetString(int n) const;
    virtual void SetString(int n, const wxString& s);
    virtual int FindString(const wxString& s) const;
    // selection
    virtual int GetSelection() const;
protected:
    virtual int DoAppend(const wxString& item);
    virtual void DoSetItemClientData(int n, void* clientData);
    virtual void* DoGetItemClientData(int n) const;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData);
    virtual wxClientData* DoGetItemClientObject(int n) const;
};

#endif // __WX_COCOA_LISTBOX_H__
