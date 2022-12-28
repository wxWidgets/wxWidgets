/////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxt2.h
// Purpose:     runtime metadata information (extended class info)
// Author:      Stefan Csomor
// Modified by: Francesco Montorsi
// Created:     27/07/03
// Copyright:   (c) 1997 Julian Smart
//              (c) 2003 Stefan Csomor
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XTI2H__
#define _WX_XTI2H__

// ----------------------------------------------------------------------------
// second part of xti headers, is included from object.h
// ----------------------------------------------------------------------------

#if wxUSE_EXTENDED_RTTI

// ----------------------------------------------------------------------------
// wxDynamicObject class, its instances connect to a 'super class instance'
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxDynamicObject : public wxObject
{
    friend class WXDLLIMPEXP_FWD_BASE wxDynamicClassInfo ;
public:
    // instantiates this object with an instance of its superclass
    wxDynamicObject(wxObject* superClassInstance, const wxDynamicClassInfo *info) ;
    virtual ~wxDynamicObject();

    void SetProperty (const wxChar *propertyName, const wxAny &value);
    wxAny GetProperty (const wxChar *propertyName) const ;

    // get the runtime identity of this object
    wxClassInfo *GetClassInfo() const
    {
#ifdef _MSC_VER
        return (wxClassInfo*) m_classInfo;
#else
        wxDynamicClassInfo *nonconst = const_cast<wxDynamicClassInfo *>(m_classInfo);
        return static_cast<wxClassInfo *>(nonconst);
#endif
    }

    wxObject* GetSuperClassInstance() const
    {
        return m_superClassInstance ;
    }
private :
    // removes an existing runtime-property
    void RemoveProperty( const wxChar *propertyName ) ;

    // renames an existing runtime-property
    void RenameProperty( const wxChar *oldPropertyName , const wxChar *newPropertyName ) ;

    wxObject *m_superClassInstance ;
    const wxDynamicClassInfo *m_classInfo;
    struct wxDynamicObjectInternal;
    wxDynamicObjectInternal *m_data;
};

// ----------------------------------------------------------------------------
// String conversion templates supporting older compilers
// ----------------------------------------------------------------------------

#if wxUSE_FUNC_TEMPLATE_POINTER
#  define wxTO_STRING(type) wxToStringConverter<type>
#  define wxTO_STRING_IMP(type)
#  define wxFROM_STRING(type) wxFromStringConverter<type>
#  define wxFROM_STRING_IMP(type)
#else
#  define wxTO_STRING(type) ToString##type
#  define wxTO_STRING_IMP(type) \
    inline void ToString##type( const wxAny& data, wxString &result ) \
{ wxToStringConverter<type>(data, result); }

#  define wxFROM_STRING(type) FromString##type
#  define wxFROM_STRING_IMP(type) \
    inline void FromString##type( const wxString& data, wxAny &result ) \
{ wxFromStringConverter<type>(data, result); }
#endif

#include "wx/xtiprop.h"
#include "wx/xtictor.h"

// ----------------------------------------------------------------------------
// wxIMPLEMENT class macros for concrete classes
// ----------------------------------------------------------------------------

// Single inheritance with one base class

#define _DEFAULT_CONSTRUCTOR(name)                                          \
wxObject* wxConstructorFor##name()                                          \
{ return new name; }

#define _DEFAULT_CONVERTERS(name)                                          \
wxObject* wxVariantOfPtrToObjectConverter##name ( const wxAny &data )        \
{ return data.As( (name**)nullptr ); }                    \
    wxAny wxObjectToVariantConverter##name ( wxObject *data )              \
{ return wxAny( wx_dynamic_cast(name*, data)  ); }

#define _TYPEINFO_CLASSES(n, toString, fromString )                           \
    wxClassTypeInfo s_typeInfo##n(wxT_OBJECT, &n::ms_classInfo,               \
    toString, fromString, typeid(n).name());    \
    wxClassTypeInfo s_typeInfoPtr##n(wxT_OBJECT_PTR, &n::ms_classInfo,        \
    toString, fromString, typeid(n*).name());

#define _IMPLEMENT_DYNAMIC_CLASS(name, basename, unit, callback)                \
    _DEFAULT_CONSTRUCTOR(name)                                                  \
    _DEFAULT_CONVERTERS(name)                                                   \
    \
    const wxClassInfo* name::ms_classParents[] =                                \
{ &basename::ms_classInfo, nullptr };                                      \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxT(unit),            \
    wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) wxConstructorFor##name,   \
    name::GetPropertiesStatic, name::GetHandlersStatic, name::ms_constructor,     \
    name::ms_constructorProperties, name::ms_constructorPropertiesCount,              \
    wxVariantOfPtrToObjectConverter##name, nullptr, wxObjectToVariantConverter##name,    \
    callback);

#define _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY(name, basename, unit, callback )         \
    _DEFAULT_CONSTRUCTOR(name)                                                  \
    _DEFAULT_CONVERTERS(name)                                                   \
    void wxVariantToObjectConverter##name ( const wxAny &data, wxObjectFunctor* fn )                 \
    { name o = data.As<name>(); (*fn)( &o ); }                        \
    \
    const wxClassInfo* name::ms_classParents[] = { &basename::ms_classInfo,nullptr };  \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxT(unit),                \
    wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) wxConstructorFor##name,  \
    name::GetPropertiesStatic,name::GetHandlersStatic,name::ms_constructor,      \
    name::ms_constructorProperties, name::ms_constructorPropertiesCount,             \
    wxVariantOfPtrToObjectConverter##name, wxVariantToObjectConverter##name,         \
    wxObjectToVariantConverter##name, callback);

#define wxIMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename )                   \
    _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename, "", nullptr )              \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)                                         \
    const wxPropertyInfo *name::GetPropertiesStatic()                           \
{ return (wxPropertyInfo*) nullptr; }                                      \
    const wxHandlerInfo *name::GetHandlersStatic()                              \
{ return (wxHandlerInfo*) nullptr; }                                       \
    wxCONSTRUCTOR_DUMMY( name )

#define wxIMPLEMENT_DYNAMIC_CLASS( name, basename )                             \
    _IMPLEMENT_DYNAMIC_CLASS( name, basename, "", nullptr )                        \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)                                         \
    wxPropertyInfo *name::GetPropertiesStatic()                                 \
{ return (wxPropertyInfo*) nullptr; }                                      \
    wxHandlerInfo *name::GetHandlersStatic()                                    \
{ return (wxHandlerInfo*) nullptr; }                                       \
    wxCONSTRUCTOR_DUMMY( name )

#define wxIMPLEMENT_DYNAMIC_CLASS_XTI( name, basename, unit )                   \
    _IMPLEMENT_DYNAMIC_CLASS( name, basename, unit, nullptr )                      \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)

#define wxIMPLEMENT_DYNAMIC_CLASS_XTI_CALLBACK( name, basename, unit, callback )\
    _IMPLEMENT_DYNAMIC_CLASS( name, basename, unit, &callback )                 \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)

#define wxIMPLEMENT_DYNAMIC_CLASS_WITH_COPY_XTI( name, basename, unit )         \
    _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename, unit, nullptr  )           \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)

#define wxIMPLEMENT_DYNAMIC_CLASS_WITH_COPY_AND_STREAMERS_XTI( name, basename,  \
    unit, toString,    \
    fromString )       \
    _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename, unit, nullptr  )           \
    _TYPEINFO_CLASSES(name, toString, fromString)

// this is for classes that do not derive from wxObject, there are no creators for these

#define wxIMPLEMENT_DYNAMIC_CLASS_NO_WXOBJECT_NO_BASE_XTI( name, unit )         \
    const wxClassInfo* name::ms_classParents[] = { nullptr };                      \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxEmptyString,        \
    wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) 0,          \
    name::GetPropertiesStatic,name::GetHandlersStatic, 0, 0,        \
    0, 0, 0 );                                                          \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)

// this is for subclasses that still do not derive from wxObject

#define wxIMPLEMENT_DYNAMIC_CLASS_NO_WXOBJECT_XTI( name, basename, unit )           \
    const wxClassInfo* name::ms_classParents[] = { &basename::ms_classInfo, nullptr }; \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxEmptyString,            \
    wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) 0,              \
    name::GetPropertiesStatic,name::GetHandlersStatic, 0, 0,            \
    0, 0, 0 );                                                              \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)


// Multiple inheritance with two base classes

#define _IMPLEMENT_DYNAMIC_CLASS2(name, basename, basename2, unit, callback)         \
    _DEFAULT_CONSTRUCTOR(name)                                                  \
    _DEFAULT_CONVERTERS(name)                                                   \
    \
    const wxClassInfo* name::ms_classParents[] =                                     \
{ &basename::ms_classInfo,&basename2::ms_classInfo, nullptr };                  \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxT(unit),                 \
    wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) wxConstructorFor##name, \
    name::GetPropertiesStatic,name::GetHandlersStatic,name::ms_constructor,     \
    name::ms_constructorProperties, name::ms_constructorPropertiesCount,            \
    wxVariantOfPtrToObjectConverter##name, nullptr, wxObjectToVariantConverter##name,  \
    callback);

#define wxIMPLEMENT_DYNAMIC_CLASS2( name, basename, basename2)                      \
    _IMPLEMENT_DYNAMIC_CLASS2( name, basename, basename2, "", nullptr)                 \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)                                             \
    wxPropertyInfo *name::GetPropertiesStatic() { return (wxPropertyInfo*) nullptr; }  \
    wxHandlerInfo *name::GetHandlersStatic() { return (wxHandlerInfo*) nullptr; }      \
    wxCONSTRUCTOR_DUMMY( name )

#define wxIMPLEMENT_DYNAMIC_CLASS2_XTI( name, basename, basename2, unit) \
    _IMPLEMENT_DYNAMIC_CLASS2( name, basename, basename2, unit, nullptr)    \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)



// ----------------------------------------------------------------------------
// wxIMPLEMENT class macros for abstract classes
// ----------------------------------------------------------------------------

// Single inheritance with one base class

#define _IMPLEMENT_ABSTRACT_CLASS(name, basename)                               \
    _DEFAULT_CONVERTERS(name)                                                   \
    \
    const wxClassInfo* name::ms_classParents[] =                                \
{ &basename::ms_classInfo,nullptr };                                       \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxEmptyString,        \
    wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) 0,              \
    name::GetPropertiesStatic,name::GetHandlersStatic, 0, 0,            \
    0, wxVariantOfPtrToObjectConverter##name,0, \
    wxObjectToVariantConverter##name);                                         \
    _TYPEINFO_CLASSES(name, nullptr, nullptr)

#define wxIMPLEMENT_ABSTRACT_CLASS( name, basename )                            \
    _IMPLEMENT_ABSTRACT_CLASS( name, basename )                                 \
    wxHandlerInfo *name::GetHandlersStatic() { return (wxHandlerInfo*) nullptr; }  \
    wxPropertyInfo *name::GetPropertiesStatic() { return (wxPropertyInfo*) nullptr; }

// Multiple inheritance with two base classes

#define wxIMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2)                 \
    wxClassInfo name::ms_classInfo(wxT(#name), wxT(#basename1),                 \
    wxT(#basename2), (int) sizeof(name),         \
    (wxObjectConstructorFn) 0);

// templated streaming, every type that can be converted to wxString
// must have their specialization for these methods

template<typename T>
void wxStringReadValue( const wxString &s, T &data );

template<typename T>
void wxStringWriteValue( wxString &s, const T &data);

template<typename T>
void wxToStringConverter( const wxAny &v, wxString &s )
{ wxStringWriteValue(s, v.As<T>()); }

template<typename T>
void wxFromStringConverter( const wxString &s, wxAny &v)
{ T d; wxStringReadValue(s, d); v = wxAny(d); }

// --------------------------------------------------------------------------
// Collection Support
// --------------------------------------------------------------------------

template<typename iter, typename collection_t > void wxListCollectionToAnyList(
                                                                               const collection_t& coll, wxAnyList &value )
{
    for ( iter current = coll.GetFirst(); current;
         current = current->GetNext() )
    {
        value.Append( new wxAny(current->GetData()) );
    }
}

template<typename collection_t> void wxArrayCollectionToVariantArray(
                                                                     const collection_t& coll, wxAnyList &value )
{
    for( size_t i = 0; i < coll.GetCount(); i++ )
    {
        value.Append( new wxAny(coll[i]) );
    }
}

#endif

#endif // _WX_XTIH2__
