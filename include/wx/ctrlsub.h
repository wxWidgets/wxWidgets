/////////////////////////////////////////////////////////////////////////////
// Name:        wx/ctrlsub.h (read: "wxConTRoL with SUBitems")
// Purpose:     wxControlWithItems interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     22.10.99
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CTRLSUB_H_BASE_
#define _WX_CTRLSUB_H_BASE_

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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
    virtual ~wxItemContainer();

    // adding items
    // ------------

    int Append(const wxString& item)
        { return DoAppend(item); }
    int Append(const wxString& item, void *clientData)
        { int n = DoAppend(item); SetClientData(n, clientData); return n; }
    int Append(const wxString& item, wxClientData *clientData)
        { int n = DoAppend(item); SetClientObject(n, clientData); return n; }

    // only for rtti needs (separate name)
    void AppendString( const wxString& item)
        { Append( item ); }

    // append several items at once to the control
    void Append(const wxArrayString& strings);

    int Insert(const wxString& item, int pos)
        { return DoInsert(item, pos); }
    int Insert(const wxString& item, int pos, void *clientData);
    int Insert(const wxString& item, int pos, wxClientData *clientData);

    // deleting items
    // --------------

    virtual void Clear() = 0;
    virtual void Delete(int n) = 0;

    // accessing strings
    // -----------------

    virtual int GetCount() const = 0;
    bool IsEmpty() const { return GetCount() == 0; }

    virtual wxString GetString(int n) const = 0;
    wxArrayString GetStrings() const;
    virtual void SetString(int n, const wxString& s) = 0;
    virtual int FindString(const wxString& s) const = 0;

    // selection
    // ---------

    virtual void Select(int n) = 0;
    virtual int GetSelection() const = 0;

    // set selection to the specified string, return false if not found
    bool SetStringSelection(const wxString& s);

    // return the selected string or empty string if none
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

#if WXWIN_COMPATIBILITY_2_2
    // compatibility - these functions are deprecated, use the new ones
    // instead
    wxDEPRECATED( int Number() const );
#endif // WXWIN_COMPATIBILITY_2_2

protected:
    virtual int DoAppend(const wxString& item) = 0;
    virtual int DoInsert(const wxString& item, int pos) = 0;

    virtual void DoSetItemClientData(int n, void* clientData) = 0;
    virtual void* DoGetItemClientData(int n) const = 0;
    virtual void DoSetItemClientObject(int n, wxClientData* clientData) = 0;
    virtual wxClientData* DoGetItemClientObject(int n) const = 0;

    // the type of the client data for the items
    wxClientDataType m_clientDataItemsType;
};

// this macro must (unfortunately) be used in any class deriving from both
// wxItemContainer and wxControl because otherwise there is ambiguity when
// calling GetClientXXX() functions -- the compiler can't choose between the
// two versions
#define wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST                    \
    void SetClientData(void *data)                                         \
        { wxControl::SetClientData(data); }                                \
    void *GetClientData() const                                            \
        { return wxControl::GetClientData(); }                             \
    void SetClientObject(wxClientData *data)                               \
        { wxControl::SetClientObject(data); }                              \
    wxClientData *GetClientObject() const                                  \
        { return wxControl::GetClientObject(); }                           \
    void SetClientData(int n, void* clientData)                            \
        { wxItemContainer::SetClientData(n, clientData); }                 \
    void* GetClientData(int n) const                                       \
        { return wxItemContainer::GetClientData(n); }                      \
    void SetClientObject(int n, wxClientData* clientData)                  \
        { wxItemContainer::SetClientObject(n, clientData); }               \
    wxClientData* GetClientObject(int n) const                             \
        { return wxItemContainer::GetClientObject(n); }

class WXDLLEXPORT wxControlWithItems : public wxControl, public wxItemContainer
{
public:
    wxControlWithItems() { }
    virtual ~wxControlWithItems();

    // we have to redefine these functions here to avoid ambiguities in classes
    // deriving from us which would arise otherwise because both base classses
    // have the methods with the same names - hopefully, a smart compiler can
    // optimize away these simple inline wrappers so we don't suffer much from
    // this
    wxCONTROL_ITEMCONTAINER_CLIENTDATAOBJECT_RECAST

    // usually the controls like list/combo boxes have their own background
    // colour
    virtual bool ShouldInheritColours() const { return false; }

protected:
    // we can't compute our best size before the items are added to the control
    // which is done after calling SetInitialBestSize() (it is called from the
    // base class ctor and the items are added in the derived class ctor), so
    // don't do anything at all here as our size will be changed later anyhow
    //
    // of course, all derived classes *must* call SetBestSize() from their
    // ctors for this to work!
    virtual void SetInitialBestSize(const wxSize& WXUNUSED(size)) { }

private:
    DECLARE_NO_COPY_CLASS(wxControlWithItems)
};

#endif // wxUSE_CONTROLS

#endif // _WX_CTRLSUB_H_BASE_

