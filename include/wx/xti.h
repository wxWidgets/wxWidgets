/////////////////////////////////////////////////////////////////////////////
// Name:        wx/xti.h
// Purpose:     runtime metadata information (extended class info)
// Author:      Stefan Csomor
// Modified by: Francesco Montorsi
// Created:     27/07/03
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Julian Smart
//              (c) 2003 Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_XTIH__
#define _WX_XTIH__

// We want to support properties, event sources and events sinks through
// explicit declarations, using templates and specialization to make the
// effort as painless as possible.
//
// This means we have the following domains :
//
// - Type Information for categorizing built in types as well as custom types
//   this includes information about enums, their values and names
// - Type safe value storage : a kind of wxVariant, called right now wxVariantBase
//   which will be merged with wxVariant
// - Property Information and Property Accessors providing access to a class'
//   values and exposed event delegates
// - Information about event handlers
// - extended Class Information for accessing all these

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"

#if wxUSE_EXTENDED_RTTI

// include definitions of other XTI structures
#include "wx/variantbase.h"
#include "wx/xtitypes.h"
#include "wx/xtictor.h"
#include "wx/xtiprop.h"
#include "wx/xtihandler.h"

// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxObject;
class WXDLLIMPEXP_BASE wxVariantBase;
class WXDLLIMPEXP_BASE wxVariantBaseArray;

typedef wxObject *(*wxObjectConstructorFn)(void);
typedef wxObject* (*wxVariantToObjectConverter)( wxVariantBase &data );
typedef wxVariantBase (*wxObjectToVariantConverter)( wxObject* );

class WXDLLIMPEXP_BASE wxObjectWriter;
class WXDLLIMPEXP_BASE wxObjectReaderCallback;

typedef bool (*wxObjectStreamingCallback) ( const wxObject *, wxObjectWriter *, \
                                            wxObjectReaderCallback *, wxVariantBaseArray & );

class WXDLLIMPEXP_BASE wxClassInfo
{
    friend class WXDLLIMPEXP_BASE wxPropertyInfo;
    friend class WXDLLIMPEXP_BASE wxHandlerInfo;
    friend wxObject *wxCreateDynamicObject(const wxChar *name);

public:
    wxClassInfo(const wxClassInfo **_Parents,
                const wxChar *_UnitName,
                const wxChar *_ClassName,
                int size,
                wxObjectConstructorFn ctor,
                wxPropertyInfo *_Props,
                wxHandlerInfo *_Handlers,
                wxObjectAllocatorAndCreator* _Constructor,
                const wxChar ** _ConstructorProperties,
                const int _ConstructorPropertiesCount,
                wxVariantToObjectConverter _PtrConverter1,
                wxVariantToObjectConverter _Converter2,
                wxObjectToVariantConverter _Converter3,
                wxObjectStreamingCallback _streamingCallback = NULL) :
            m_className(_ClassName),
            m_objectSize(size),
            m_objectConstructor(ctor),
            m_next(sm_first),
            m_firstProperty(_Props),
            m_firstHandler(_Handlers),
            m_parents(_Parents),
            m_unitName(_UnitName),
            m_constructor(_Constructor),
            m_constructorProperties(_ConstructorProperties),
            m_constructorPropertiesCount(_ConstructorPropertiesCount),
            m_variantOfPtrToObjectConverter(_PtrConverter1),
            m_variantToObjectConverter(_Converter2),
            m_objectToVariantConverter(_Converter3),
            m_streamingCallback(_streamingCallback)
    {
        sm_first = this;
        Register();
    }

    wxClassInfo(const wxChar *_UnitName, const wxChar *_ClassName,
                const wxClassInfo **_Parents) :
            m_className(_ClassName),
            m_objectSize(0),
            m_objectConstructor(NULL),
            m_next(sm_first),
            m_firstProperty(NULL),
            m_firstHandler(NULL),
            m_parents(_Parents),
            m_unitName(_UnitName),
            m_constructor(NULL),
            m_constructorProperties(NULL),
            m_constructorPropertiesCount(0),
            m_variantOfPtrToObjectConverter(NULL),
            m_variantToObjectConverter(NULL),
            m_objectToVariantConverter(NULL),
            m_streamingCallback(NULL)
    {
        sm_first = this;
        Register();
    }

    // ctor compatible with old RTTI system
    wxClassInfo(const wxChar *_ClassName,
                const wxClassInfo *_Parent1,
                const wxClassInfo *_Parent2,
                int size,
                wxObjectConstructorFn ctor) :
            m_className(_ClassName),
            m_objectSize(size),
            m_objectConstructor(ctor),
            m_next(sm_first),
            m_firstProperty(NULL),
            m_firstHandler(NULL),
            m_parents(NULL),
            m_unitName(NULL),
            m_constructor(NULL),
            m_constructorProperties(NULL),
            m_constructorPropertiesCount(0),
            m_variantOfPtrToObjectConverter(NULL),
            m_variantToObjectConverter(NULL),
            m_objectToVariantConverter(NULL),
            m_streamingCallback(NULL)
    {
        sm_first = this;
        m_parents[0] = _Parent1;
        m_parents[1] = _Parent2;
        m_parents[2] = NULL;
        Register();
    }

    virtual ~wxClassInfo();

    // allocates an instance of this class, this object does not have to be 
    // initialized or fully constructed as this call will be followed by a call to Create
    virtual wxObject *AllocateObject() const 
        { return m_objectConstructor ? (*m_objectConstructor)() : 0; }

    // 'old naming' for AllocateObject staying here for backward compatibility
    wxObject *CreateObject() const { return AllocateObject(); }

    // direct construction call for classes that cannot construct instances via alloc/create
    wxObject *ConstructObject(int ParamCount, wxVariantBase *Params) const;

    bool NeedsDirectConstruction() const 
        { return wx_dynamic_cast(wxObjectAllocator*, m_constructor) != NULL; }

    const wxChar       *GetClassName() const 
        { return m_className; }
    const wxChar       *GetBaseClassName1() const
        { return m_parents[0] ? m_parents[0]->GetClassName() : NULL; }
    const wxChar       *GetBaseClassName2() const
        { return (m_parents[0] && m_parents[1]) ? m_parents[1]->GetClassName() : NULL; }

    const wxClassInfo  *GetBaseClass1() const
        { return m_parents[0]; }
    const wxClassInfo  *GetBaseClass2() const
        { return m_parents[0] ? m_parents[1] : NULL; }

    const wxChar       *GetIncludeName() const 
        { return m_unitName; }
    const wxClassInfo **GetParents() const 
        { return m_parents; }
    int                 GetSize() const 
        { return m_objectSize; }
    bool                IsDynamic() const 
        { return (NULL != m_objectConstructor); }

    wxObjectConstructorFn      GetConstructor() const 
        { return m_objectConstructor; }
    const wxClassInfo         *GetNext() const 
        { return m_next; }

    // statics:

    static void                CleanUp();
    static wxClassInfo        *FindClass(const wxChar *className);
    static const wxClassInfo  *GetFirst() 
        { return sm_first; }


    // Climb upwards through inheritance hierarchy.
    // Dual inheritance is catered for.

    bool IsKindOf(const wxClassInfo *info) const;

    wxDECLARE_CLASS_INFO_ITERATORS()

    // if there is a callback registered with that class it will be called
    // before this object will be written to disk, it can veto streaming out
    // this object by returning false, if this class has not registered a
    // callback, the search will go up the inheritance tree if no callback has
    // been registered true will be returned by default
    bool BeforeWriteObject( const wxObject *obj, wxObjectWriter *streamer, \
                            wxObjectReaderCallback *persister, wxVariantBaseArray &metadata) const;

    // gets the streaming callback from this class or any superclass
    wxObjectStreamingCallback GetStreamingCallback() const;

    // returns the first property
    const wxPropertyInfo* GetFirstProperty() const 
        { return m_firstProperty; }

    // returns the first handler
    const wxHandlerInfo* GetFirstHandler() const 
        { return m_firstHandler; }

    // Call the Create upon an instance of the class, in the end the object is fully
    // initialized
    virtual bool Create (wxObject *object, int ParamCount, wxVariantBase *Params) const;

    // get number of parameters for constructor
    virtual int GetCreateParamCount() const 
        { return m_constructorPropertiesCount; }

    // get n-th constructor parameter
    virtual const wxChar* GetCreateParamName(int n) const 
        { return m_constructorProperties[n]; }

    // Runtime access to objects for simple properties (get/set) by property 
    // name and variant data
    virtual void SetProperty (wxObject *object, const wxChar *propertyName, 
                              const wxVariantBase &value) const;
    virtual wxVariantBase GetProperty (wxObject *object, const wxChar *propertyName) const;

    // Runtime access to objects for collection properties by property name
    virtual wxVariantBaseArray GetPropertyCollection(wxObject *object, 
                                                  const wxChar *propertyName) const;
    virtual void AddToPropertyCollection(wxObject *object, const wxChar *propertyName, 
                                         const wxVariantBase& value) const;

    // we must be able to cast variants to wxObject pointers, templates seem 
    // not to be suitable
    wxObject* VariantToInstance( wxVariantBase &data ) const
    {
        if ( data.GetTypeInfo()->GetKind() == wxT_OBJECT )
            return m_variantToObjectConverter( data );
        else
            return m_variantOfPtrToObjectConverter( data );
    }

    wxVariantBase InstanceToVariant( wxObject *object ) const 
        { return m_objectToVariantConverter( object ); }

    // find property by name
    virtual const wxPropertyInfo *FindPropertyInfo (const wxChar *PropertyName) const;

    // find handler by name
    virtual const wxHandlerInfo *FindHandlerInfo (const wxChar *handlerName) const;

    // find property by name
    virtual wxPropertyInfo *FindPropertyInfoInThisClass (const wxChar *PropertyName) const;

    // find handler by name
    virtual wxHandlerInfo *FindHandlerInfoInThisClass (const wxChar *handlerName) const;

    // puts all the properties of this class and its superclasses in the map, 
    // as long as there is not yet an entry with the same name (overriding mechanism)
    void GetProperties( wxPropertyInfoMap &map ) const;

private:
    const wxChar            *m_className;
    int                      m_objectSize;
    wxObjectConstructorFn     m_objectConstructor;

    // class info object live in a linked list:
    // pointers to its head and the next element in it

    static wxClassInfo      *sm_first;
    wxClassInfo              *m_next;

    static wxHashTable      *sm_classTable;

protected:
    wxPropertyInfo *          m_firstProperty;
    wxHandlerInfo *           m_firstHandler;

private:
    const wxClassInfo**       m_parents;
    const wxChar*             m_unitName;

    wxObjectAllocatorAndCreator*     m_constructor;
    const wxChar **           m_constructorProperties;
    const int                 m_constructorPropertiesCount;
    wxVariantToObjectConverter m_variantOfPtrToObjectConverter;
    wxVariantToObjectConverter m_variantToObjectConverter;
    wxObjectToVariantConverter m_objectToVariantConverter;
    wxObjectStreamingCallback  m_streamingCallback;

    const wxPropertyAccessor *FindAccessor (const wxChar *propertyName) const;

protected:
    // registers the class
    void Register();
    void Unregister();

    DECLARE_NO_COPY_CLASS(wxClassInfo)
};

WXDLLIMPEXP_BASE wxObject *wxCreateDynamicObject(const wxChar *name);


// ----------------------------------------------------------------------------
// wxDynamicClassInfo
// ----------------------------------------------------------------------------

// this object leads to having a pure runtime-instantiation

class WXDLLIMPEXP_BASE wxDynamicClassInfo : public wxClassInfo
{
    friend class WXDLLIMPEXP_BASE wxDynamicObject;

public:
    wxDynamicClassInfo( const wxChar *_UnitName, const wxChar *_ClassName, 
                        const wxClassInfo* superClass );
    virtual ~wxDynamicClassInfo();

    // constructs a wxDynamicObject with an instance
    virtual wxObject *AllocateObject() const;

    // Call the Create method for a class
    virtual bool Create (wxObject *object, int ParamCount, wxVariantBase *Params) const;

    // get number of parameters for constructor
    virtual int GetCreateParamCount() const;

    // get i-th constructor parameter
    virtual const wxChar* GetCreateParamName(int i) const;

    // Runtime access to objects by property name, and variant data
    virtual void SetProperty (wxObject *object, const wxChar *PropertyName, 
                              const wxVariantBase &Value) const;
    virtual wxVariantBase GetProperty (wxObject *object, const wxChar *PropertyName) const;

    // adds a property to this class at runtime
    void AddProperty( const wxChar *propertyName, const wxTypeInfo* typeInfo );

    // removes an existing runtime-property
    void RemoveProperty( const wxChar *propertyName );

    // renames an existing runtime-property
    void RenameProperty( const wxChar *oldPropertyName, const wxChar *newPropertyName );

    // as a handler to this class at runtime
    void AddHandler( const wxChar *handlerName, wxObjectEventFunction address, 
                     const wxClassInfo* eventClassInfo );

    // removes an existing runtime-handler
    void RemoveHandler( const wxChar *handlerName );

    // renames an existing runtime-handler
    void RenameHandler( const wxChar *oldHandlerName, const wxChar *newHandlerName );

private:
    struct wxDynamicClassInfoInternal;
    wxDynamicClassInfoInternal* m_data;
};


// ----------------------------------------------------------------------------
// wxDECLARE class macros
// ----------------------------------------------------------------------------

#define _DECLARE_DYNAMIC_CLASS(name)                        \
    public:                                                 \
        static wxClassInfo ms_classInfo;                    \
        static const wxClassInfo* ms_classParents[];        \
        static wxPropertyInfo* GetPropertiesStatic();       \
        static wxHandlerInfo* GetHandlersStatic();          \
        static wxClassInfo *GetClassInfoStatic()            \
            { return &name::ms_classInfo; }                 \
        virtual wxClassInfo *GetClassInfo() const           \
            { return &name::ms_classInfo; }

#define wxDECLARE_DYNAMIC_CLASS(name)                       \
    static wxObjectAllocatorAndCreator* ms_constructor;           \
    static const wxChar * ms_constructorProperties[];       \
    static const int ms_constructorPropertiesCount;         \
    _DECLARE_DYNAMIC_CLASS(name)

#define wxDECLARE_DYNAMIC_CLASS_NO_ASSIGN(name)             \
    wxDECLARE_NO_ASSIGN_CLASS(name)                         \
    wxDECLARE_DYNAMIC_CLASS(name)

#define wxDECLARE_DYNAMIC_CLASS_NO_COPY(name)               \
    wxDECLARE_NO_COPY_CLASS(name)                           \
    wxDECLARE_DYNAMIC_CLASS(name)

#define wxDECLARE_ABSTRACT_CLASS(name)    _DECLARE_DYNAMIC_CLASS(name)
#define wxCLASSINFO(name)                 (&name::ms_classInfo)


// ----------------------------------------------------------------------------
// wxIMPLEMENT class macros for concrete classes
// ----------------------------------------------------------------------------

// Single inheritance with one base class

#define _TYPEINFO_CLASSES(n, toString, fromString )                           \
    wxClassTypeInfo s_typeInfo##n(wxT_OBJECT, &n::ms_classInfo,               \
                                  toString, fromString, typeid(n).name());    \
    wxClassTypeInfo s_typeInfoPtr##n(wxT_OBJECT_PTR, &n::ms_classInfo,        \
                                     toString, fromString, typeid(n*).name());

#define _IMPLEMENT_DYNAMIC_CLASS(name, basename, unit, callback)                \
    wxObject* wxConstructorFor##name()                                          \
        { return new name; }                                                    \
    wxObject* wxVariantOfPtrToObjectConverter##name ( wxVariantBase &data )        \
        { return data.wxTEMPLATED_MEMBER_CALL(Get, name*); }                    \
    wxVariantBase wxObjectToVariantConverter##name ( wxObject *data )              \
        { return wxVariantBase( wx_dynamic_cast(name*, data)  ); }                 \
                                                                                \
    const wxClassInfo* name::ms_classParents[] =                                \
        { &basename::ms_classInfo, NULL };                                      \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxT(unit),            \
        wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) wxConstructorFor##name,   \
        name::GetPropertiesStatic(), name::GetHandlersStatic(), name::ms_constructor,     \
        name::ms_constructorProperties, name::ms_constructorPropertiesCount,              \
        wxVariantOfPtrToObjectConverter##name, NULL, wxObjectToVariantConverter##name,    \
        callback);

#define _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY(name, basename, unit, callback )         \
    wxObject* wxConstructorFor##name()                                              \
        { return new name; }                                                        \
    wxObject* wxVariantToObjectConverter##name ( wxVariantBase &data )                 \
        { return &data.wxTEMPLATED_MEMBER_CALL(Get, name); }                        \
    wxObject* wxVariantOfPtrToObjectConverter##name ( wxVariantBase &data )            \
        { return data.wxTEMPLATED_MEMBER_CALL(Get, name*); }                        \
    wxVariantBase wxObjectToVariantConverter##name ( wxObject *data )                  \
        { return wxVariantBase( wx_dynamic_cast(name*, data)  ); }                     \
                                                                                    \
    const wxClassInfo* name::ms_classParents[] = { &basename::ms_classInfo,NULL };  \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxT(unit),                \
        wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) wxConstructorFor##name,  \
        name::GetPropertiesStatic(),name::GetHandlersStatic(),name::ms_constructor,      \
        name::ms_constructorProperties, name::ms_constructorPropertiesCount,             \
        wxVariantOfPtrToObjectConverter##name, wxVariantToObjectConverter##name,         \
        wxObjectToVariantConverter##name, callback);

#define wxIMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename )                   \
    _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename, "", NULL )              \
    _TYPEINFO_CLASSES(name, NULL, NULL)                                         \
    const wxPropertyInfo *name::GetPropertiesStatic()                           \
        { return (wxPropertyInfo*) NULL; }                                      \
    const wxHandlerInfo *name::GetHandlersStatic()                              \
        { return (wxHandlerInfo*) NULL; }                                       \
    wxCONSTRUCTOR_DUMMY( name )

#define wxIMPLEMENT_DYNAMIC_CLASS( name, basename )                             \
    _IMPLEMENT_DYNAMIC_CLASS( name, basename, "", NULL )                        \
    _TYPEINFO_CLASSES(name, NULL, NULL)                                         \
    wxPropertyInfo *name::GetPropertiesStatic()                                 \
        { return (wxPropertyInfo*) NULL; }                                      \
    wxHandlerInfo *name::GetHandlersStatic()                                    \
        { return (wxHandlerInfo*) NULL; }                                       \
    wxCONSTRUCTOR_DUMMY( name )

#define wxIMPLEMENT_DYNAMIC_CLASS_XTI( name, basename, unit )                   \
    _IMPLEMENT_DYNAMIC_CLASS( name, basename, unit, NULL )                      \
    _TYPEINFO_CLASSES(name, NULL, NULL)

#define wxIMPLEMENT_DYNAMIC_CLASS_XTI_CALLBACK( name, basename, unit, callback )\
    _IMPLEMENT_DYNAMIC_CLASS( name, basename, unit, &callback )                 \
    _TYPEINFO_CLASSES(name, NULL, NULL)

#define wxIMPLEMENT_DYNAMIC_CLASS_WITH_COPY_XTI( name, basename, unit )         \
    _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename, unit, NULL  )           \
    _TYPEINFO_CLASSES(name, NULL, NULL)

#define wxIMPLEMENT_DYNAMIC_CLASS_WITH_COPY_AND_STREAMERS_XTI( name, basename,  \
                                                             unit, toString,    \
                                                             fromString )       \
    _IMPLEMENT_DYNAMIC_CLASS_WITH_COPY( name, basename, unit, NULL  )           \
    _TYPEINFO_CLASSES(name, toString, fromString)

// this is for classes that do not derive from wxObject, there are no creators for these

#define wxIMPLEMENT_DYNAMIC_CLASS_NO_WXOBJECT_NO_BASE_XTI( name, unit )         \
    const wxClassInfo* name::ms_classParents[] = { NULL };                      \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxEmptyString,        \
            wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) 0,          \
            name::GetPropertiesStatic(),name::GetHandlersStatic(), 0, 0,        \
            0, 0, 0 );                                                          \
    _TYPEINFO_CLASSES(name, NULL, NULL)

// this is for subclasses that still do not derive from wxObject

#define wxIMPLEMENT_DYNAMIC_CLASS_NO_WXOBJECT_XTI( name, basename, unit )           \
    const wxClassInfo* name::ms_classParents[] = { &basename::ms_classInfo, NULL }; \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxEmptyString,            \
            wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) 0,              \
            name::GetPropertiesStatic(),name::GetHandlersStatic(), 0, 0,            \
            0, 0, 0 );                                                              \
    _TYPEINFO_CLASSES(name, NULL, NULL)


// Multiple inheritance with two base classes

#define _IMPLEMENT_DYNAMIC_CLASS2(name, basename, basename2, unit, callback)         \
    wxObject* wxConstructorFor##name() { return new name; }                          \
    wxObject* wxVariantOfPtrToObjectConverter##name ( wxVariantBase &data )             \
        { return data.wxTEMPLATED_MEMBER_CALL(Get, name*); }                         \
    wxVariantBase wxObjectToVariantConverter##name ( wxObject *data )                   \
        { return wxVariantBase( wx_dynamic_cast(name*, data)  ); }                      \
                                                                                     \
    const wxClassInfo* name::ms_classParents[] =                                     \
        { &basename::ms_classInfo,&basename2::ms_classInfo, NULL };                  \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxT(unit),                 \
        wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) wxConstructorFor##name, \
        name::GetPropertiesStatic(),name::GetHandlersStatic(),name::ms_constructor,     \
        name::ms_constructorProperties, name::ms_constructorPropertiesCount,            \
        wxVariantOfPtrToObjectConverter##name, NULL, wxObjectToVariantConverter##name,  \
        callback);

#define wxIMPLEMENT_DYNAMIC_CLASS2( name, basename, basename2)                      \
    _IMPLEMENT_DYNAMIC_CLASS2( name, basename, basename2, "", NULL)                 \
    _TYPEINFO_CLASSES(name, NULL, NULL)                                             \
    wxPropertyInfo *name::GetPropertiesStatic() { return (wxPropertyInfo*) NULL; }  \
    wxHandlerInfo *name::GetHandlersStatic() { return (wxHandlerInfo*) NULL; }      \
    wxCONSTRUCTOR_DUMMY( name )

#define wxIMPLEMENT_DYNAMIC_CLASS2_XTI( name, basename, basename2, unit) \
    _IMPLEMENT_DYNAMIC_CLASS2( name, basename, basename2, unit, NULL)    \
    _TYPEINFO_CLASSES(name, NULL, NULL)



// ----------------------------------------------------------------------------
// wxIMPLEMENT class macros for abstract classes
// ----------------------------------------------------------------------------

// Single inheritance with one base class

#define _IMPLEMENT_ABSTRACT_CLASS(name, basename)                               \
    wxObject* wxVariantToObjectConverter##name ( wxVariantBase &data )             \
        { return data.wxTEMPLATED_MEMBER_CALL(Get, name*); }                    \
    wxObject* wxVariantOfPtrToObjectConverter##name ( wxVariantBase &data )        \
        { return data.wxTEMPLATED_MEMBER_CALL(Get, name*); }                    \
    wxVariantBase wxObjectToVariantConverter##name ( wxObject *data )              \
        { return wxVariantBase( wx_dynamic_cast(name*, data)  ); }                 \
                                                                                \
    const wxClassInfo* name::ms_classParents[] =                                \
        { &basename::ms_classInfo,NULL };                                       \
    wxClassInfo name::ms_classInfo(name::ms_classParents, wxEmptyString,        \
        wxT(#name), (int) sizeof(name), (wxObjectConstructorFn) 0,              \
        name::GetPropertiesStatic(),name::GetHandlersStatic(), 0, 0,            \
        0, wxVariantOfPtrToObjectConverter##name,wxVariantToObjectConverter##name, \
        wxObjectToVariantConverter##name);                                         \
    _TYPEINFO_CLASSES(name, NULL, NULL)

#define wxIMPLEMENT_ABSTRACT_CLASS( name, basename )                            \
    _IMPLEMENT_ABSTRACT_CLASS( name, basename )                                 \
    wxHandlerInfo *name::GetHandlersStatic() { return (wxHandlerInfo*) NULL; }  \
    wxPropertyInfo *name::GetPropertiesStatic() { return (wxPropertyInfo*) NULL; }

// Multiple inheritance with two base classes

#define wxIMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2)                 \
    wxClassInfo name::ms_classInfo(wxT(#name), wxT(#basename1),                 \
                                   wxT(#basename2), (int) sizeof(name),         \
                                   (wxObjectConstructorFn) 0);



// --------------------------------------------------------------------------
// Collection Support
// --------------------------------------------------------------------------

template<typename iter, typename collection_t > void wxListCollectionToVariantArray( 
    const collection_t& coll, wxVariantBaseArray &value )
{
    iter current = coll.GetFirst();
    while (current)
    {
        value.Add( new wxVariantBase(current->GetData()) );
        current = current->GetNext();
    }
}

template<typename collection_t> void wxArrayCollectionToVariantArray( 
    const collection_t& coll, wxVariantBaseArray &value )
{
    for( size_t i = 0; i < coll.GetCount(); i++ )
    {
        value.Add( new wxVariantBase(coll[i]) );
    }
}

#endif  // wxUSE_EXTENDED_RTTI
#endif // _WX_XTIH__
