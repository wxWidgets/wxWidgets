/////////////////////////////////////////////////////////////////////////////
// Name:        wx/ctrlsub.h (read: "wxConTRoL with SUBitems")
// Purpose:     wxControlWithItems interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CTRLSUB_H_BASE_
#define _WX_CTRLSUB_H_BASE_

#ifdef __GNUG__
    #pragma interface "controlwithitems.h"
#endif

#include "wx/control.h"      // base class

// ----------------------------------------------------------------------------
// wxControlWithItems defines an interface which is implemented by all controls
// which have string subitems each of which may be selected.
//
// Examples: wxListBox, wxCheckListBox, wxChoice and wxComboBox
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxControlWithItems : public wxControl
{
public:
    wxControlWithItems() { m_clientDataItemsType = ClientData_None; }
#ifdef __WXMAC_X__
    virtual ~wxControlWithItems() {}  // Added min for Mac X
#endif

    // adding items
    // ------------

    void Append(const wxString& item)
        { DoAppend(item); }
    void Append(const wxString& item, void *clientData)
        { int n = DoAppend(item); SetClientData(n, clientData); }
    void Append(const wxString& item, wxClientData *clientData)
        { int n = DoAppend(item); SetClientObject(n, clientData); }

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

    virtual void Select(int n) = 0;
    virtual int GetSelection() const = 0;

    wxString GetStringSelection() const;

    // misc
    // ----

    // client data stuff
    void SetClientData(int n, void* clientData);
    void* GetClientData(int n) const;

    void SetClientObject(int n, wxClientData* clientData);
    wxClientData* GetClientObject(int n) const;

    bool HasClientObjectData() const
        { return m_clientDataItemsType == ClientData_Object; }
    bool HasClientUntypedData() const
        { return m_clientDataItemsType == ClientData_Void; }

    // compatibility - these functions are deprecated, use the new ones
    // instead
    int Number() const { return GetCount(); }

protected:
    virtual int DoAppend(const wxString& item) = 0;

    virtual void DoSetItemClientData(int n, void* clientData) = 0;
    virtual void* DoGetItemClientData(int n) const = 0;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData) = 0;
    virtual wxClientData* DoGetItemClientObject(int n) const = 0;

    // the type of the client data for the items
    wxClientDataType m_clientDataItemsType;
};

#endif // _WX_CTRLSUB_H_BASE_


