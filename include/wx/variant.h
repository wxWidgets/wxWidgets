/////////////////////////////////////////////////////////////////////////////
// Name:        wx/variant.h
// Purpose:     wxVariant class, container for any type
// Author:      Julian Smart
// Modified by:
// Created:     10/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_VARIANT_H_
#define _WX_VARIANT_H_

#include "wx/defs.h"

#if wxUSE_VARIANT

#include "wx/variantbase.h"
#include "wx/object.h"
#include "wx/list.h"


// NOTE: wxVariantData is declared in variantbase.h

// wxVariant is not wxObject-derived and thus cannot simply use wxList
WX_DECLARE_LIST(void, wxListVoid);


/*
 * wxVariant can store any kind of data, but has some basic types
 * built in.
 */

class WXDLLIMPEXP_BASE wxVariant: public wxVariantBase
{
public:
    wxVariant() {}
    wxVariant(const wxVariant& variant)
        : wxVariantBase(variant) {}
    wxVariant(wxVariantData* data, const wxString& name = wxEmptyString)
        : wxVariantBase(data, name) {}

    // generic assignment
    void operator= (const wxVariant& variant);

    // Assignment using data, e.g.
    // myVariant = new wxStringVariantData("hello");
    void operator= (wxVariantData* variantData);

    bool operator== (const wxVariant& variant) const;
    bool operator!= (const wxVariant& variant) const;

    // double
    wxVariant(double val, const wxString& name = wxEmptyString);
    bool operator== (double value) const;
    bool operator!= (double value) const;
    void operator= (double value) ;
    inline operator double () const {  return GetDouble(); }
    inline double GetReal() const { return GetDouble(); }
    double GetDouble() const;

    // long
    wxVariant(long val, const wxString& name = wxEmptyString);
    wxVariant(int val, const wxString& name = wxEmptyString);
    wxVariant(short val, const wxString& name = wxEmptyString);
    bool operator== (long value) const;
    bool operator!= (long value) const;
    void operator= (long value) ;
    inline operator long () const {  return GetLong(); }
    inline long GetInteger() const { return GetLong(); }
    long GetLong() const;

    // bool
#ifdef HAVE_BOOL
    wxVariant(bool val, const wxString& name = wxEmptyString);
    bool operator== (bool value) const;
    bool operator!= (bool value) const;
    void operator= (bool value) ;
    inline operator bool () const {  return GetBool(); }
    bool GetBool() const ;
#endif

    // wxDateTime
#if wxUSE_DATETIME
    wxVariant(const wxDateTime& val, const wxString& name = wxEmptyString);
#if wxUSE_ODBC
    wxVariant(const DATE_STRUCT* valptr, const wxString& name = wxEmptyString);
    wxVariant(const TIME_STRUCT* valptr, const wxString& name = wxEmptyString);
    wxVariant(const TIMESTAMP_STRUCT* valptr, const wxString& name = wxEmptyString);
#endif
    bool operator== (const wxDateTime& value) const;
    bool operator!= (const wxDateTime& value) const;
    void operator= (const wxDateTime& value) ;
#if wxUSE_ODBC
    void operator= (const DATE_STRUCT* value) ;
    void operator= (const TIME_STRUCT* value) ;
    void operator= (const TIMESTAMP_STRUCT* value) ;
#endif
    inline operator wxDateTime () const { return GetDateTime(); }
    wxDateTime GetDateTime() const;
#endif

    // wxString
    wxVariant(const wxString& val, const wxString& name = wxEmptyString);
    wxVariant(const wxChar* val, const wxString& name = wxEmptyString); // Necessary or VC++ assumes bool!
    virtual bool operator== (const wxString& value) const;
    virtual bool operator!= (const wxString& value) const;
    virtual void operator= (const wxString& value) ;
    virtual void operator= (const wxChar* value) ; // Necessary or VC++ assumes bool!
    virtual inline operator wxString () const {  return MakeString(); }
    wxString GetString() const;

    // wxChar
    wxVariant(wxChar val, const wxString& name = wxEmptyString);
    virtual bool operator== (wxChar value) const;
    virtual bool operator!= (wxChar value) const;
    virtual void operator= (wxChar value) ;
    virtual inline operator wxChar () const { return GetChar(); }
    wxChar GetChar() const ;

    // wxArrayString
    wxVariant(const wxArrayString& val, const wxString& name = wxEmptyString);
    virtual bool operator== (const wxArrayString& value) const;
    virtual bool operator!= (const wxArrayString& value) const;
    virtual void operator= (const wxArrayString& value);
    virtual inline operator wxArrayString () const { return GetArrayString(); }
    wxArrayString GetArrayString() const;

    // void*
    wxVariant(void* ptr, const wxString& name = wxEmptyString);
    bool operator== (void* value) const;
    bool operator!= (void* value) const;
    void operator= (void* value);
    inline operator void* () const {  return GetVoidPtr(); }
    void* GetVoidPtr() const;

    // wxObject*
    wxVariant(wxObject* ptr, const wxString& name = wxEmptyString);
    virtual bool operator== (wxObject* value) const;
    virtual bool operator!= (wxObject* value) const;
    virtual void operator= (wxObject* value);
    wxObject* GetWxObjectPtr() const;

    // ------------------------------
    // list operations
    // ------------------------------

    wxVariant(const wxList& val, const wxString& name = wxEmptyString); // List of variants
    bool operator== (const wxList& value) const;
    bool operator!= (const wxList& value) const;
    void operator= (const wxList& value) ;
    // Treat a list variant as an array
    wxVariant operator[] (size_t idx) const;
    wxVariant& operator[] (size_t idx) ;
    wxListVoid& GetList() const ;

    // Return the number of elements in a list
    size_t GetCount() const;

    // Make empty list
    void NullList();

    // Append to list
    void Append(const wxVariant& value);

    // Insert at front of list
    void Insert(const wxVariant& value);

    // Returns true if the variant is a member of the list
    bool Member(const wxVariant& value) const;

    // Deletes the nth element of the list
    bool Delete(size_t item);

    // Clear list
    void ClearList();

public:
    // Type conversion
    bool Convert(long* value) const;
    bool Convert(bool* value) const;
    bool Convert(double* value) const;
    bool Convert(wxString* value) const;
    bool Convert(wxChar* value) const;
#if wxUSE_DATETIME
    bool Convert(wxDateTime* value) const;
#endif // wxUSE_DATETIME
};

#define DECLARE_VARIANT_OBJECT(classname) \
    DECLARE_VARIANT_OBJECT_EXPORTED(classname, wxEMPTY_PARAMETER_VALUE)

#define DECLARE_VARIANT_OBJECT_EXPORTED(classname,expdecl) \
expdecl classname& operator << ( classname &object, const wxVariant &variant ); \
expdecl wxVariant& operator << ( wxVariant &variant, const classname &object );

#define IMPLEMENT_VARIANT_OBJECT(classname) \
    IMPLEMENT_VARIANT_OBJECT_EXPORTED(classname, wxEMPTY_PARAMETER_VALUE)

#define IMPLEMENT_VARIANT_OBJECT_EXPORTED_NO_EQ(classname,expdecl) \
class classname##VariantData: public wxVariantData \
{ \
public:\
    classname##VariantData() {} \
    classname##VariantData( const classname &value ) { m_value = value; } \
\
    classname &GetValue() { return m_value; } \
\
    virtual bool Eq(wxVariantData& data) const; \
\
    virtual const wxTypeInfo *GetTypeInfo() const; \
    virtual wxString GetType() const; \
    virtual wxClassInfo* GetValueClassInfo(); \
\
protected:\
    classname m_value; \
};\
\
const wxTypeInfo* classname##VariantData::GetTypeInfo() const\
{\
    /* no idea if this is ok */ \
    return wxTypeInfo::FindType(GetType());\
}\
\
wxString classname##VariantData::GetType() const\
{\
    return m_value.GetClassInfo()->GetClassName();\
}\
\
wxClassInfo* classname##VariantData::GetValueClassInfo()\
{\
    return m_value.GetClassInfo();\
}\
\
expdecl classname& operator << ( classname &value, const wxVariant &variant )\
{\
    /*wxASSERT( wxIsKindOf( variant.GetData(), classname##VariantData ) );*/ \
    \
    classname##VariantData *data = (classname##VariantData*) variant.GetData();\
    value = data->GetValue();\
    return value;\
}\
\
expdecl wxVariant& operator << ( wxVariant &variant, const classname &value )\
{\
    classname##VariantData *data = new classname##VariantData( value );\
    variant.SetData( data );\
    return variant;\
}

// implements a wxVariantData-derived class using for the Eq() method the
// operator== which must have been provided by "classname"
#define IMPLEMENT_VARIANT_OBJECT_EXPORTED(classname,expdecl) \
IMPLEMENT_VARIANT_OBJECT_EXPORTED_NO_EQ(classname,wxEMPTY_PARAMETER_VALUE expdecl) \
\
bool classname##VariantData::Eq(wxVariantData& data) const \
{\
    /*wxASSERT( wxIsKindOf((&data), classname##VariantData) );*/ \
\
    classname##VariantData & otherData = (classname##VariantData &) data;\
\
    return otherData.m_value == m_value;\
}\


// implements a wxVariantData-derived class using for the Eq() method a shallow
// comparison (through wxObject::IsSameAs function)
#define IMPLEMENT_VARIANT_OBJECT_SHALLOWCMP(classname) \
    IMPLEMENT_VARIANT_OBJECT_EXPORTED_SHALLOWCMP(classname, wxEMPTY_PARAMETER_VALUE)
#define IMPLEMENT_VARIANT_OBJECT_EXPORTED_SHALLOWCMP(classname,expdecl) \
IMPLEMENT_VARIANT_OBJECT_EXPORTED_NO_EQ(classname,wxEMPTY_PARAMETER_VALUE expdecl) \
\
bool classname##VariantData::Eq(wxVariantData& data) const \
{\
    /*wxASSERT( wxIsKindOf((&data), classname##VariantData) );*/  \
\
    classname##VariantData & otherData = (classname##VariantData &) data;\
\
    return (otherData.m_value.IsSameAs(m_value));\
}\


// Since we want type safety wxVariant we need to fetch and dynamic_cast
// in a seemingly safe way so the compiler can check, so we define
// a dynamic_cast /wxDynamicCast analogue.
// FIXME: needs rename to e.g. wxGET_VARIANT_CAST() being a macro!
#define wxGetVariantCast(var,classname) \
    ((classname*)(var.IsValueKindOf(&classname::ms_classInfo) ?\
                  var.GetWxObjectPtr() : NULL));

#include "wx/dynarray.h"
WX_DECLARE_OBJARRAY_WITH_DECL(wxVariant, wxVariantArray, class WXDLLIMPEXP_BASE);

// default objects:

extern wxVariant WXDLLIMPEXP_BASE wxNullVariant;


#endif // wxUSE_VARIANT

#endif // _WX_VARIANT_H_
