/////////////////////////////////////////////////////////////////////////////
// Name:        object.h
// Purpose:     wxObject class, plus run-time type information macros
// Author:      Julian Smart
// Modified by: Ron Lee
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Julian Smart and Markus Holzem
//              (c) 2001 Ron Lee <ron@debian.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_OBJECTH__
#define _WX_OBJECTH__

#ifdef __GNUG__
#pragma interface "object.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"
#include "wx/memory.h"

class WXDLLEXPORT wxObject;

#if wxUSE_DYNAMIC_CLASSES

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#ifdef GetClassName
#undef GetClassName
#endif
#ifdef GetClassInfo
#undef GetClassInfo
#endif

class WXDLLEXPORT wxClassInfo;
class WXDLLEXPORT wxHashTable;

#if wxUSE_STD_IOSTREAM && (defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT)
#include "wx/ioswrap.h"
#endif


// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

typedef wxObject *(*wxObjectConstructorFn)(void);

class WXDLLEXPORT wxClassInfo
{
public:
    wxClassInfo( const wxChar *className,
                 const wxChar *baseName1,
                 const wxChar *baseName2,
                 int size,
                 wxObjectConstructorFn ctor )
        : m_className(className)
        , m_baseClassName1(baseName1)
        , m_baseClassName2(baseName2)
        , m_objectSize(size)
        , m_objectConstructor(ctor)
        , m_baseInfo1(0)
        , m_baseInfo2(0)
        , m_next(sm_first)
    { sm_first = this; }

    wxObject *CreateObject() { return m_objectConstructor ? (*m_objectConstructor)() : 0; }

    const wxChar       *GetClassName() const { return m_className; }
    const wxChar       *GetBaseClassName1() const { return m_baseClassName1; }
    const wxChar       *GetBaseClassName2() const { return m_baseClassName2; }
    const wxClassInfo  *GetBaseClass1() const { return m_baseInfo1; }
    const wxClassInfo  *GetBaseClass2() const { return m_baseInfo2; }
    int                 GetSize() const { return m_objectSize; }

    wxObjectConstructorFn      GetConstructor() const { return m_objectConstructor; }
    static const wxClassInfo  *GetFirst() { return sm_first; }
    const wxClassInfo         *GetNext() const { return m_next; }
    static wxClassInfo        *FindClass(const wxChar *className);
    
        // Climb upwards through inheritance hierarchy.
        // Dual inheritance is catered for.

    bool IsKindOf(const wxClassInfo *info) const
    {
        return info != 0 &&
               ( info == this ||
                 ( m_baseInfo1 && m_baseInfo1->IsKindOf(info) ) ||
                 ( m_baseInfo2 && m_baseInfo2->IsKindOf(info) ) );
    }

        // Initializes parent pointers and hash table for fast searching.

    static void     InitializeClasses();

        // Cleans up hash table used for fast searching.

    static void     CleanUpClasses();

public:
    const wxChar            *m_className;
    const wxChar            *m_baseClassName1;
    const wxChar            *m_baseClassName2;
    int                      m_objectSize;
    wxObjectConstructorFn    m_objectConstructor;

        // Pointers to base wxClassInfos: set in InitializeClasses

    const wxClassInfo       *m_baseInfo1;
    const wxClassInfo       *m_baseInfo2;

        // class info object live in a linked list:
        // pointers to its head and the next element in it

    static wxClassInfo      *sm_first;
    wxClassInfo             *m_next;

    static wxHashTable      *sm_classTable;
};

WXDLLEXPORT wxObject *wxCreateDynamicObject(const wxChar *name);


// ----------------------------------------------------------------------------
// Dynamic class macros
// ----------------------------------------------------------------------------

#define DECLARE_DYNAMIC_CLASS(name)           \
 public:                                      \
  static wxClassInfo sm_class##name;          \
  virtual wxClassInfo *GetClassInfo() const   \
   { return &name::sm_class##name; }

#define DECLARE_ABSTRACT_CLASS(name) DECLARE_DYNAMIC_CLASS(name)
#define DECLARE_CLASS(name) DECLARE_DYNAMIC_CLASS(name)

// -----------------------------------
// for concrete classes
// -----------------------------------

    // Single inheritance with one base class

#define IMPLEMENT_DYNAMIC_CLASS(name, basename)                 \
 wxObject* WXDLLEXPORT_CTORFN wxConstructorFor##name()          \
  { return new name; }                                          \
 wxClassInfo name::sm_class##name(wxT(#name), wxT(#basename),   \
            0, (int) sizeof(name),                              \
            (wxObjectConstructorFn) wxConstructorFor##name);

    // Multiple inheritance with two base classes

#define IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2)    \
 wxObject* WXDLLEXPORT_CTORFN wxConstructorFor##name()          \
  { return new name; }                                          \
 wxClassInfo name::sm_class##name(wxT(#name), wxT(#basename1),  \
            wxT(#basename2), (int) sizeof(name),                \
            (wxObjectConstructorFn) wxConstructorFor##name);

// -----------------------------------
// for abstract classes
// -----------------------------------

    // Single inheritance with one base class

#define IMPLEMENT_ABSTRACT_CLASS(name, basename)                \
 wxClassInfo name::sm_class##name(wxT(#name), wxT(#basename),   \
            0, (int) sizeof(name), (wxObjectConstructorFn) 0);

    // Multiple inheritance with two base classes

#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2)   \
 wxClassInfo name::sm_class##name(wxT(#name), wxT(#basename1),  \
            wxT(#basename2), (int) sizeof(name),                \
            (wxObjectConstructorFn) 0);

#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

// -----------------------------------
// for pluggable classes
// -----------------------------------

    // NOTE: this should probably be the very first statement
    //       in the class declaration so wxPluginSentinel is
    //       the first member initialised and the last destroyed.

// _DECLARE_DL_SENTINEL(name) wxPluginSentinel m_pluginsentinel;

#if wxUSE_NESTED_CLASSES

#define _DECLARE_DL_SENTINEL(name, exportdecl)  \
class exportdecl name##PluginSentinel {         \
private:                                        \
    static const wxString sm_className;         \
public:                                         \
    name##PluginSentinel();                     \
    ~##name##PluginSentinel();                  \
};                                              \
name##PluginSentinel  m_pluginsentinel;

#define _IMPLEMENT_DL_SENTINEL(name)                                \
 const wxString name::name##PluginSentinel::sm_className(#name);    \
 name::name##PluginSentinel::name##PluginSentinel() {               \
    wxDLManifestEntry *e = (wxDLManifestEntry*) wxDLManifestEntry::ms_classes.Get(#name);   \
    if( e != 0 ) { e->RefObj(); }                                      \
 }                                                                  \
 name::name##PluginSentinel::~##name##PluginSentinel() {            \
    wxDLManifestEntry *e = (wxDLManifestEntry*) wxDLManifestEntry::ms_classes.Get(#name);   \
    if( e != 0 ) { e->UnrefObj(); }                                 \
 }
#else

#define _DECLARE_DL_SENTINEL(name)
#define _IMPLEMENT_DL_SENTINEL(name)

#endif  // wxUSE_NESTED_CLASSES

#define DECLARE_PLUGGABLE_CLASS(name) \
 DECLARE_DYNAMIC_CLASS(name) _DECLARE_DL_SENTINEL(name, WXDLLEXPORT)
#define DECLARE_ABSTRACT_PLUGGABLE_CLASS(name)  \
 DECLARE_ABSTRACT_CLASS(name) _DECLARE_DL_SENTINEL(name, WXDLLEXPORT)

#define DECLARE_USER_EXPORTED_PLUGGABLE_CLASS(name, usergoo) \
 DECLARE_DYNAMIC_CLASS(name) _DECLARE_DL_SENTINEL(name, usergoo)
#define DECLARE_USER_EXPORTED_ABSTRACT_PLUGGABLE_CLASS(name, usergoo)  \
 DECLARE_ABSTRACT_CLASS(name) _DECLARE_DL_SENTINEL(name, usergoo)

#define IMPLEMENT_PLUGGABLE_CLASS(name, basename) \
 IMPLEMENT_DYNAMIC_CLASS(name, basename) _IMPLEMENT_DL_SENTINEL(name)
#define IMPLEMENT_PLUGGABLE_CLASS2(name, basename1, basename2)  \
 IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2) _IMPLEMENT_DL_SENTINEL(name)
#define IMPLEMENT_ABSTRACT_PLUGGABLE_CLASS(name, basename) \
 IMPLEMENT_ABSTRACT_CLASS(name, basename) _IMPLEMENT_DL_SENTINEL(name)
#define IMPLEMENT_ABSTRACT_PLUGGABLE_CLASS2(name, basename1, basename2)  \
 IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2) _IMPLEMENT_DL_SENTINEL(name)

#define IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS(name, basename) \
 IMPLEMENT_PLUGGABLE_CLASS(name, basename)
#define IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS2(name, basename1, basename2)  \
 IMPLEMENT_PLUGGABLE_CLASS2(name, basename1, basename2)
#define IMPLEMENT_USER_EXPORTED_ABSTRACT_PLUGGABLE_CLASS(name, basename) \
 IMPLEMENT_ABSTRACT_PLUGGABLE_CLASS(name, basename)
#define IMPLEMENT_USER_EXPORTED_ABSTRACT_PLUGGABLE_CLASS2(name, basename1, basename2)  \
 IMPLEMENT_ABSTRACT_PLUGGABLE_CLASS2(name, basename1, basename2)


#define CLASSINFO(name) (&name::sm_class##name)

#else // !wxUSE_DYNAMIC_CLASSES

    // No dynamic class system: so stub out the macros

#define DECLARE_DYNAMIC_CLASS(name)
#define DECLARE_ABSTRACT_CLASS(name)
#define DECLARE_CLASS(name)
#define IMPLEMENT_DYNAMIC_CLASS(name, basename)
#define IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2)
#define IMPLEMENT_ABSTRACT_CLASS(name, basename)
#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2)
#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

#define DECLARE_PLUGGABLE_CLASS(name)
#define DECLARE_ABSTRACT_PLUGGABLE_CLASS(name)
#define IMPLEMENT_PLUGGABLE_CLASS(name, basename)
#define IMPLEMENT_PLUGGABLE_CLASS2(name, basename1, basename2)
#define IMPLEMENT_ABSTRACT_PLUGGABLE_CLASS(name, basename)
#define IMPLEMENT_ABSTRACT_PLUGGABLE_CLASS2(name, basename1, basename2)

#define DECLARE_USER_EXPORTED_PLUGGABLE_CLASS(name, usergoo)
#define DECLARE_USER_EXPORTED_ABSTRACT_PLUGGABLE_CLASS(name, usergoo)
#define IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS(name, basename)
#define IMPLEMENT_USER_EXPORTED_PLUGGABLE_CLASS2(name, basename1, basename2)
#define IMPLEMENT_USER_EXPORTED_ABSTRACT_PLUGGABLE_CLASS(name, basename)
#define IMPLEMENT_USER_EXPORTED_ABSTRACT_PLUGGABLE_CLASS2(name, basename1, basename2)

#endif // wxUSE_DYNAMIC_CLASSES


#define wxIS_KIND_OF(obj, className) obj->IsKindOf(&className::sm_class##className)

    // Just seems a bit nicer-looking (pretend it's not a macro)

#define wxIsKindOf(obj, className) obj->IsKindOf(&className::sm_class##className)

    // to be replaced by dynamic_cast<> in the future

#define wxDynamicCast(obj, className) \
 (className *) wxCheckDynamicCast((wxObject*)(obj), &className::sm_class##className)

    // The 'this' pointer is always true, so use this version
    // to cast the this pointer and avoid compiler warnings.

#define wxDynamicCastThis(className) \
 (IsKindOf(&className::sm_class##className) ? (className *)(this) : (className *)0)

#define wxConstCast(obj, className) ((className *)(obj))


#ifdef __WXDEBUG__
inline void wxCheckCast(void *ptr)
{
    wxASSERT_MSG( ptr, _T("wxStaticCast() used incorrectly") );
}
#define wxStaticCast(obj, className) \
 (wxCheckCast(wxDynamicCast(obj, className)), ((className *)(obj)))

#else  // !__WXDEBUG__
#define wxStaticCast(obj, className) ((className *)(obj))

#endif  // __WXDEBUG__


    // Unfortunately Borland seems to need this include.

#if wxUSE_STD_IOSTREAM                                \
    && (defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT)  \
    && defined(__BORLANDC__)
#if wxUSE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif
#endif


// ----------------------------------------------------------------------------
// wxObject
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxObjectRefData;

class WXDLLEXPORT wxObject
{
DECLARE_ABSTRACT_CLASS(wxObject)

public:
    wxObject() : m_refData(0) {}
    virtual ~wxObject() { UnRef(); }

    bool IsKindOf(wxClassInfo *info) const;

#if defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING
    void *operator new (size_t size, wxChar *fileName = 0, int lineNum = 0);

#ifndef __VISAGECPP__
    void operator delete (void * buf);
#elif __DEBUG_ALLOC__
    void operator delete (void *buf, const char *_fname, size_t _line);
#endif

    // VC++ 6.0

#if defined(__VISUALC__) && (__VISUALC__ >= 1200)
    void operator delete(void *buf, wxChar*, int);
#endif

    // Causes problems for VC++

#if wxUSE_ARRAY_MEMORY_OPERATORS && !defined(__VISUALC__) && !defined( __MWERKS__)
    void *operator new[] (size_t size, wxChar *fileName = 0, int lineNum = 0);
    void operator delete[] (void *buf);
#endif

#ifdef __MWERKS__
    void *operator new[] (size_t size, wxChar *fileName , int lineNum = 0);
    void *operator new[] (size_t size) { return operator new[] ( size, 0, 0 ) ; }
    void operator delete[] (void *buf);
#endif

#endif // Debug & memory tracing


#if wxUSE_STD_IOSTREAM && (defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT)
    virtual void Dump(wxSTD ostream& str);
#endif

        // make a 'clone' of the object
  
    void Ref(const wxObject& clone);

        // destroy a reference
  
    void UnRef();

    inline wxObjectRefData *GetRefData() const { return m_refData; }
    inline void SetRefData(wxObjectRefData *data) { m_refData = data; }

protected:
    wxObjectRefData *m_refData;
};


class WXDLLEXPORT wxObjectRefData
{
    friend class wxObject;

public:
    wxObjectRefData() : m_count(1) {}
    virtual ~wxObjectRefData() {}

    inline int GetRefCount() const { return m_count; }
private:
    int m_count;
};


inline wxObject *wxCheckDynamicCast(wxObject *obj, wxClassInfo *classInfo)
{
    return obj && obj->GetClassInfo()->IsKindOf(classInfo) ? obj : 0;
}

#ifdef __WXDEBUG__
#ifndef WXDEBUG_NEW
#define WXDEBUG_NEW new(__TFILE__,__LINE__)
#endif
#else
#define WXDEBUG_NEW new
#endif

    // Redefine new to be the debugging version. This doesn't
    // work with all compilers, in which case you need to
    // use WXDEBUG_NEW explicitly if you wish to use the debugging version.

#if defined(__WXDEBUG__) && wxUSE_GLOBAL_MEMORY_OPERATORS && wxUSE_DEBUG_NEW_ALWAYS
#define new new(__TFILE__,__LINE__)
#endif

#endif  // _WX_OBJECTH__

// vi:sts=4:sw=4:et
