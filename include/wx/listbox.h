///////////////////////////////////////////////////////////////////////////////
// Name:        wx/listbox.h
// Purpose:     wxListBox class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:   	wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOX_H_BASE_
#define _WX_LISTBOX_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma interface "listboxbase.h"
#endif

#include "wx/defs.h"

#if wxUSE_LISTBOX

#include "wx/control.h"         // base class

// forward declarations are enough here
class WXDLLEXPORT wxArrayInt;
class WXDLLEXPORT wxArrayString;

// ----------------------------------------------------------------------------
// global data
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(extern const wxChar*) wxListBoxNameStr;

// ----------------------------------------------------------------------------
// wxListBox interface is defined by the class wxListBoxBase
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxListBoxBase : public wxControl
{
public:
    // ctor
    wxListBoxBase() { m_clientDataItemsType = ClientData_None; }

    // adding items
    // ------------

    void Append(const wxString& item)
        { DoAppend(item); }
    void Append(const wxString& item, void *clientData)
        { int n = DoAppend(item); SetClientData(n, clientData); }
    void Append(const wxString& item, wxClientData *clientData)
        { int n = DoAppend(item); SetClientObject(n, clientData); }

    void Insert(const wxString& item, int pos)
        { DoInsert(item, pos); }
    void Insert(const wxString& item, int pos, void *clientData)
        { DoInsert(item, pos); SetClientData(pos, clientData); }
    void Insert(const wxString& item, int pos, wxClientData *clientData)
        { DoInsert(item, pos); SetClientObject(pos, clientData); }

    void InsertItems(int nItems, const wxString *items, int pos);
    void InsertItems(const wxArrayString& items, int pos)
        { DoInsertItems(items, pos); }

    void Set(int n, const wxString* items, void **clientData = NULL);
    void Set(const wxArrayString& items, void **clientData = NULL)
        { DoSetItems(items, clientData); }

    // deleting items
    // --------------

    virtual void Clear() = 0;
    virtual void Delete(int n) = 0;

    // accessing strings
    // -----------------

    virtual int GetCount() const = 0;
    virtual wxString GetString(int n) const = 0;
    virtual void SetString(int n, const wxString& s) = 0;
    virtual int FindString(const wxString& s) const = 0;

    // selection
    // ---------

    virtual bool IsSelected(int n) const = 0;
    virtual void SetSelection(int n, bool select = TRUE) = 0;
    void Select(int n) { SetSelection(n, TRUE); }
    void Deselect(int n) { SetSelection(n, FALSE); }

    virtual int GetSelection() const = 0;
    virtual int GetSelections(wxArrayInt& aSelections) const = 0;

    bool SetStringSelection(const wxString& s, bool select = TRUE);
    wxString GetStringSelection() const;

    // misc
    // ----

    // client data stuff
    void SetClientData( int n, void* clientData );
    void* GetClientData( int n ) const;

    void SetClientObject( int n, wxClientData* clientData );
    wxClientData* GetClientObject( int n ) const;

    // Set the specified item at the first visible item or scroll to max
    // range.
    void SetFirstItem(int n) { DoSetFirstItem(n); }
    void SetFirstItem(const wxString& s);

    // emulate selecting or deselecting (depending on event.GetExtraLong())
    // the item event.GetInt() from the control
    virtual void Command(wxCommandEvent &event);

    // compatibility - these functions are deprecated, use the new ones
    // instead
    bool Selected(int n) const { return IsSelected(n); }
    int Number() const { return GetCount(); }

protected:
    // NB: due to wxGTK implementation details, DoInsert() is implemented
    //     using DoInsertItems() and not the other way round
    void DoInsert(const wxString& item, int pos)
        { InsertItems(1, &item, pos); }

    // to be implemented in derived classes
    virtual int DoAppend(const wxString& item) = 0;
    virtual void DoInsertItems(const wxArrayString& items, int pos) = 0;
    virtual void DoSetItems(const wxArrayString& items, void **clientData) = 0;

    virtual void DoSetFirstItem(int n) = 0;

    virtual void DoSetClientData(int n, void* clientData) = 0;
    virtual void* DoGetClientData(int n) const = 0;
    virtual void DoSetClientObject(int n, wxClientData* clientData) = 0;
    virtual wxClientData* DoGetClientObject(int n) const = 0;

    // the above pure virtuals hide these virtuals in wxWindowBase
    virtual void DoSetClientData(void* clientData )
        { wxWindowBase::DoSetClientData(clientData); };
    virtual void* DoGetClientData() const
        { return(wxWindowBase::DoGetClientData()); };
    virtual void DoSetClientObject( wxClientData* clientData )
        { wxWindowBase::DoSetClientObject(clientData); };
    virtual wxClientData* DoGetClientObject() const
        { return(wxWindowBase::DoGetClientObject()); };

    // the type of the client data for the items
    wxClientDataType m_clientDataItemsType;
};

// ----------------------------------------------------------------------------
// include the platform-specific class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/listbox.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/listbox.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/listbox.h"
#elif defined(__WXQT__)
    #include "wx/qt/listbox.h"
#elif defined(__WXMAC__)
    #include "wx/mac/listbox.h"
#elif defined(__WXPM__)
    #include "wx/os2/listbox.h"
#elif defined(__WXSTUBS__)
    #include "wx/stubs/listbox.h"
#endif

#endif // wxUSE_LISTBOX

#endif
    // _WX_LISTBOX_H_BASE_
