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

#if wxUSE_CONTROLS

#include "wx/control.h"      // base class

// ----------------------------------------------------------------------------
// wxItemContainer defines an interface which is implemented by all controls
// which have string subitems each of which may be selected.
//
// Examples: wxListBox, wxCheckListBox, wxChoice and wxComboBox (which
// implements an extended interface deriving from this one)
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxItemContainer
{
public:
    wxItemContainer() { m_clientDataItemsType = wxClientData_None; }

    // adding items
    // ------------

    int Append(const wxString& item)
        { return DoAppend(item); }
    int Append(const wxString& item, void *clientData)
        { int n = DoAppend(item); SetClientData(n, clientData); return n; }
    int Append(const wxString& item, wxClientData *clientData)
        { int n = DoAppend(item); SetClientObject(n, clientData); return n; }

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
        { return m_clientDataItemsType == wxClientData_Object; }
    bool HasClientUntypedData() const
        { return m_clientDataItemsType == wxClientData_Void; }

#if WXWIN_COMPATIBILITY_2
    // compatibility - these functions are deprecated, use the new ones
    // instead
    int Number() const { return GetCount(); }
#endif // WXWIN_COMPATIBILITY_2

protected:
    virtual int DoAppend(const wxString& item) = 0;

    virtual void DoSetItemClientData(int n, void* clientData) = 0;
    virtual void* DoGetItemClientData(int n) const = 0;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData) = 0;
    virtual wxClientData* DoGetItemClientObject(int n) const = 0;

    // the type of the client data for the items
    wxClientDataType m_clientDataItemsType;
};

class WXDLLEXPORT wxControlWithItems : public wxControl, public wxItemContainer
{
public:
    // we have to redefine these functions here to avoid ambiguities in classes
    // deriving from us which would arise otherwise because both base classses
    // have the methods with the same names - hopefully, a smart compiler can
    // optimize away these simple inline wrappers so we don't suffer much from
    // this

    void SetClientData(void *data)
    {
        wxControl::SetClientData(data);
    }

    void *GetClientData() const
    {
        return wxControl::GetClientData();
    }

    void SetClientObject(wxClientData *data)
    {
        wxControl::SetClientObject(data);
    }

    wxClientData *GetClientObject() const
    {
        return wxControl::GetClientObject();
    }

    void SetClientData(int n, void* clientData)
    {
        wxItemContainer::SetClientData(n, clientData);
    }

    void* GetClientData(int n) const
    {
        return wxItemContainer::GetClientData(n);
    }

    void SetClientObject(int n, wxClientData* clientData)
    {
        wxItemContainer::SetClientObject(n, clientData);
    }

    wxClientData* GetClientObject(int n) const
    {
        return wxItemContainer::GetClientObject(n);
    }
};

#endif // wxUSE_CONTROLS

#endif // _WX_CTRLSUB_H_BASE_

