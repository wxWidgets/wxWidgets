/////////////////////////////////////////////////////////////////////////////
// Name:        wx/clntdata.h
// Purpose:     A mixin class for holding a wxClientData or void pointer
// Author:      Robin Dunn
// Created:     9-Oct-2001
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CLNTDATAH__
#define _WX_CLNTDATAH__

#include "wx/defs.h"
#include "wx/string.h"
#include "wx/object.h"

#include <unordered_map>

typedef int (*wxShadowObjectMethod)(void*, void*);

using wxShadowObjectMethods = std::unordered_map<wxString, wxShadowObjectMethod>;
using wxShadowObjectFields = std::unordered_map<wxString, void*>;

class WXDLLIMPEXP_BASE wxShadowObject
{
public:
    wxShadowObject() = default;

    void AddMethod( const wxString &name, wxShadowObjectMethod method )
    {
        const auto it = m_methods.find( name );
        if (it == m_methods.end())
            m_methods[ name ] = method;
        else
            it->second = method;
    }

    bool InvokeMethod( const wxString &name, void* window, void* param, int* returnValue )
    {
        const auto it = m_methods.find( name );
        if (it == m_methods.end())
            return false;
        wxShadowObjectMethod method = it->second;
        const int ret = (*method)(window, param);
        if (returnValue)
            *returnValue = ret;
        return true;
    }

    void AddField( const wxString &name, void* initialValue = nullptr )
    {
        const auto it = m_fields.find( name );
        if (it == m_fields.end())
            m_fields[ name ] = initialValue;
        else
            it->second = initialValue;
    }

    void SetField( const wxString &name, void* value )
    {
        const auto it = m_fields.find( name );
        if (it == m_fields.end())
            return;
        it->second = value;
    }

    void* GetField( const wxString &name, void *defaultValue = nullptr )
    {
        const auto it = m_fields.find( name );
        if (it == m_fields.end())
            return defaultValue;
        return it->second;
    }

private:
    wxShadowObjectMethods   m_methods;
    wxShadowObjectFields    m_fields;
};


// ----------------------------------------------------------------------------

// what kind of client data do we have?
enum wxClientDataType
{
    wxClientData_None,    // we don't know yet because we don't have it at all
    wxClientData_Object,  // our client data is typed and we own it
    wxClientData_Void     // client data is untyped and we don't own it
};

class WXDLLIMPEXP_BASE wxClientData
{
public:
    wxClientData() = default;
    virtual ~wxClientData() = default;
};

class WXDLLIMPEXP_BASE wxStringClientData : public wxClientData
{
public:
    wxStringClientData() : m_data() { }
    wxStringClientData( const wxString &data ) : m_data(data) { }
    void SetData( const wxString &data ) { m_data = data; }
    const wxString& GetData() const { return m_data; }

private:
    wxString  m_data;
};

// This class is a mixin that provides storage and management of "client
// data."  The client data stored can either be a pointer to a wxClientData
// object in which case it is managed by the container (i.e. it will delete
// the data when it's destroyed) or an untyped pointer which won't be deleted
// by the container - but not both of them
//
// NOTE:  This functionality is currently duplicated in wxEvtHandler in order
//        to avoid having more than one vtable in that class hierarchy.

class WXDLLIMPEXP_BASE wxClientDataContainer
{
public:
    wxClientDataContainer();
    virtual ~wxClientDataContainer();

    void SetClientObject( wxClientData *data ) { DoSetClientObject(data); }
    wxClientData *GetClientObject() const { return DoGetClientObject(); }

    void SetClientData( void *data ) { DoSetClientData(data); }
    void *GetClientData() const { return DoGetClientData(); }

protected:
    // The user data: either an object which will be deleted by the container
    // when it's deleted or some raw pointer which we do nothing with. Only
    // one type of data can be used with the given window, i.e. you cannot set
    // the void data and then associate the container with wxClientData or vice
    // versa.
    union
    {
        wxClientData *m_clientObject;
        void         *m_clientData;
    };

    // client data accessors
    virtual void DoSetClientObject( wxClientData *data );
    virtual wxClientData *DoGetClientObject() const;

    virtual void DoSetClientData( void *data );
    virtual void *DoGetClientData() const;

    // what kind of data do we have?
    wxClientDataType m_clientDataType;

};

// This class is a replacement for wxClientDataContainer, and unlike
// wxClientDataContainer the wxSharedClientDataContainer client data is
// copiable, so it can be copied when objects containing it are cloned.
// Like wxClientDataContainer, wxSharedClientDataContainer is a mixin
// that provides storage and management of "client data.". The client data
// is reference counted and managed by the container.
//
// NOTE:  If your class has a clone function and needs to store client data,
//        use wxSharedClientDataContainer and not wxClientDataContainer!

class WXDLLIMPEXP_BASE wxSharedClientDataContainer
{
public:
    // Provide the same functions as in wxClientDataContainer, so that objects
    // using it and this class could be used in exactly the same way.
    void SetClientObject(wxClientData *data);
    wxClientData *GetClientObject() const;
    void SetClientData(void *data);
    void *GetClientData() const;

protected:
    bool HasClientDataContainer() const { return m_data.get() != nullptr; }
    void CopyClientDataContainer(const wxSharedClientDataContainer& other)
    {
        m_data = other.m_data;
    }

private:
    class wxRefCountedClientDataContainer : public wxClientDataContainer,
                                            public wxRefCounter
    {
    };

    // Helper function that will create m_data if it is currently null
    wxClientDataContainer *GetValidClientData();

    // m_data is shared, not deep copied, when cloned. If you make changes to
    // the data in one instance of your class, you change it for all cloned
    // instances!
    wxObjectDataPtr<wxRefCountedClientDataContainer> m_data;
};

#endif // _WX_CLNTDATAH__

