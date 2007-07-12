/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/xvariant.h
// Purpose:     wxxVariant - to be merged with wxVariant
// Author:      Stefan Csomor
// Modified by: Francesco Montorsi
// Created:     27/07/03
// RCS-ID:      $Id: xti.h 47299 2007-07-10 15:58:27Z FM $
// Copyright:   (c) 1997 Julian Smart
//              (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _XVARIANT_H_
#define _XVARIANT_H_

// ----------------------------------------------------------------------------
// wxxVariant as typesafe data holder
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxxVariantData
{
public:
    virtual ~wxxVariantData() {}

    // return a heap allocated duplicate
    virtual wxxVariantData* Clone() const = 0;

    // returns the type info of the contentc
    virtual const wxTypeInfo* GetTypeInfo() const = 0;
};

template<typename T> class wxxVariantDataT : public wxxVariantData
{
public:
    wxxVariantDataT(const T& d) : m_data(d) {}
    virtual ~wxxVariantDataT() {}

    // get a ref to the stored data
    T & Get() { return m_data; }

    // get a const ref to the stored data
    const T & Get() const { return m_data; }

    // set the data
    void Set(const T& d) { m_data =  d; }

    // return a heap allocated duplicate
    virtual wxxVariantData* Clone() const { return new wxxVariantDataT<T>( Get() ); }

    // returns the type info of the contentc
    virtual const wxTypeInfo* GetTypeInfo() const { return wxGetTypeInfo( (T*) NULL ); }

private:
    T m_data;
};

class WXDLLIMPEXP_BASE wxxVariant
{
public:
    wxxVariant() { m_data = NULL; }
    wxxVariant( wxxVariantData* data, const wxString& name = wxEmptyString ) 
        : m_data(data), m_name(name) {}
    wxxVariant( const wxxVariant &d ) 
        { 
            if ( d.m_data ) 
                m_data = d.m_data->Clone(); 
            else 
                m_data = NULL; 
            m_name = d.m_name; 
        }

    template<typename T> wxxVariant( const T& data, const wxString& name = wxEmptyString ) :
            m_data(new wxxVariantDataT<T>(data) ), m_name(name) {}

    ~wxxVariant() { delete m_data; }

    // get a ref to the stored data
    template<typename T> T& Get(wxTEMPLATED_MEMBER_FIX(T))
    {
        wxxVariantDataT<T> *dataptr = 
            dynamic_cast<wxxVariantDataT<T>*> (m_data);
        wxASSERT_MSG( dataptr, 
            wxString::Format(wxT("Cast to %s not possible"), typeid(T).name()) );
        return dataptr->Get();
    }

    // get a ref to the stored data
    template<typename T> const T& Get(wxTEMPLATED_MEMBER_FIX(T)) const
    {
        const wxxVariantDataT<T> *dataptr = 
            dynamic_cast<const wxxVariantDataT<T>*> (m_data);
        wxASSERT_MSG( dataptr, 
            wxString::Format(wxT("Cast to %s not possible"), typeid(T).name()) );
        return dataptr->Get();
    }

    bool IsEmpty() const { return m_data == NULL; }

    template<typename T> bool HasData(wxTEMPLATED_MEMBER_FIX(T)) const
    {
        const wxxVariantDataT<T> *dataptr = 
            dynamic_cast<const wxxVariantDataT<T>*> (m_data);
        return dataptr != NULL;
    }

    // stores the data
    template<typename T> void Set(const T& data) const
    {
        delete m_data;
        m_data = new wxxVariantDataT<T>(data);
    }

    wxxVariant& operator=(const wxxVariant &d)
    {
        delete m_data;
        m_data = d.m_data ? d.m_data->Clone() : NULL;
        m_name = d.m_name;
        return *this;
    }

    // gets the stored data casted to a wxObject*, returning NULL if cast is not possible
    wxObject* GetAsObject();

    // get the typeinfo of the stored object
    const wxTypeInfo* GetTypeInfo() const 
    { 
        if (!m_data)
            return NULL;
        return m_data->GetTypeInfo(); 
    }

    // returns this value as string
    wxString GetAsString() const
    {
        if (!GetTypeInfo())
            return wxEmptyString;
        wxString s;
        GetTypeInfo()->ConvertToString( *this, s );
        return s;
    }

    const wxString& GetName() const { return m_name; }

private:
    wxxVariantData* m_data;
    wxString m_name;
};

#include "wx/dynarray.h"

WX_DECLARE_OBJARRAY_WITH_DECL(wxxVariant, wxxVariantArray, class WXDLLIMPEXP_BASE);

// templated streaming, every type must have their specialization for these methods

template<typename T>
void wxStringReadValue( const wxString &s, T &data );

template<typename T>
void wxStringWriteValue( wxString &s, const T &data);

template<typename T>
void wxToStringConverter( const wxxVariant &v, wxString &s wxTEMPLATED_FUNCTION_FIX(T)) \
    { wxStringWriteValue( s, v.wxTEMPLATED_MEMBER_CALL(Get, T) ); }

template<typename T>
void wxFromStringConverter( const wxString &s, wxxVariant &v wxTEMPLATED_FUNCTION_FIX(T)) \
    { T d; wxStringReadValue( s, d ); v = wxxVariant(d); }

#endif
