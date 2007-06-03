/////////////////////////////////////////////////////////////////////////////
// Name:        wx/rtti.h
// Purpose:     old RTTI macros (use XTI when possible instead)
// Author:      Julian Smart
// Modified by: Ron Lee
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Julian Smart
//              (c) 2001 Ron Lee <ron@debian.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RTTIH__
#define _WX_RTTIH__

#if !wxUSE_EXTENDED_RTTI     // XTI system is meant to replace these macros

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/memory.h"

// ----------------------------------------------------------------------------
// forward declarations
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxObject;
class WXDLLIMPEXP_BASE wxClassInfo;
class WXDLLIMPEXP_BASE wxHashTable;
class WXDLLIMPEXP_BASE wxObject;
class WXDLLIMPEXP_BASE wxPluginLibrary;
class WXDLLIMPEXP_BASE wxObjectRefData;
class WXDLLIMPEXP_BASE wxHashTable_Node;

// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

typedef wxObject *(*wxObjectConstructorFn)(void);

class WXDLLIMPEXP_BASE wxClassInfo
{
    friend class WXDLLIMPEXP_BASE wxObject;
    friend WXDLLIMPEXP_BASE wxObject *wxCreateDynamicObject(const wxChar *name);
public:
    wxClassInfo( const wxChar *className,
                 const wxClassInfo *baseInfo1,
                 const wxClassInfo *baseInfo2,
                 int size,
                 wxObjectConstructorFn ctor )
        : m_className(className)
        , m_objectSize(size)
        , m_objectConstructor(ctor)
        , m_baseInfo1(baseInfo1)
        , m_baseInfo2(baseInfo2)
        , m_next(sm_first)
        {
            sm_first = this;
            Register();
        }

    ~wxClassInfo();

    wxObject *CreateObject() const
        { return m_objectConstructor ? (*m_objectConstructor)() : 0; }
    bool IsDynamic() const { return (NULL != m_objectConstructor); }

    const wxChar       *GetClassName() const { return m_className; }
    const wxChar       *GetBaseClassName1() const
        { return m_baseInfo1 ? m_baseInfo1->GetClassName() : NULL; }
    const wxChar       *GetBaseClassName2() const
        { return m_baseInfo2 ? m_baseInfo2->GetClassName() : NULL; }
    const wxClassInfo  *GetBaseClass1() const { return m_baseInfo1; }
    const wxClassInfo  *GetBaseClass2() const { return m_baseInfo2; }
    int                 GetSize() const { return m_objectSize; }

    wxObjectConstructorFn      GetConstructor() const
        { return m_objectConstructor; }
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

    DECLARE_CLASS_INFO_ITERATORS()
private:
    const wxChar            *m_className;
    int                      m_objectSize;
    wxObjectConstructorFn    m_objectConstructor;

        // Pointers to base wxClassInfos

    const wxClassInfo       *m_baseInfo1;
    const wxClassInfo       *m_baseInfo2;

        // class info object live in a linked list:
        // pointers to its head and the next element in it

    static wxClassInfo      *sm_first;
    wxClassInfo             *m_next;

    static wxHashTable      *sm_classTable;

protected:
    // registers the class
    void Register();
    void Unregister();

    DECLARE_NO_COPY_CLASS(wxClassInfo)
};

WXDLLIMPEXP_BASE wxObject *wxCreateDynamicObject(const wxChar *name);

// ----------------------------------------------------------------------------
// Dynamic class macros
// ----------------------------------------------------------------------------

#define DECLARE_ABSTRACT_CLASS(name)                                          \
    public:                                                                   \
        static wxClassInfo ms_classInfo;                                      \
        virtual wxClassInfo *GetClassInfo() const;

#define DECLARE_DYNAMIC_CLASS_NO_ASSIGN(name)                                 \
    DECLARE_NO_ASSIGN_CLASS(name)                                             \
    DECLARE_DYNAMIC_CLASS(name)

#define DECLARE_DYNAMIC_CLASS_NO_COPY(name)                                   \
    DECLARE_NO_COPY_CLASS(name)                                               \
    DECLARE_DYNAMIC_CLASS(name)

#define DECLARE_DYNAMIC_CLASS(name)                                           \
    DECLARE_ABSTRACT_CLASS(name)                                              \
    static wxObject* wxCreateObject();

#define DECLARE_CLASS(name) DECLARE_DYNAMIC_CLASS(name)


// common part of the macros below
#define wxIMPLEMENT_CLASS_COMMON(name, basename, baseclsinfo2, func)          \
    wxClassInfo name::ms_classInfo(wxT(#name),                                \
            &basename::ms_classInfo,                                          \
            baseclsinfo2,                                                     \
            (int) sizeof(name),                                               \
            (wxObjectConstructorFn) func);                                    \
                                                                              \
    wxClassInfo *name::GetClassInfo() const                                   \
        { return &name::ms_classInfo; }

#define wxIMPLEMENT_CLASS_COMMON1(name, basename, func)                       \
    wxIMPLEMENT_CLASS_COMMON(name, basename, NULL, func)

#define wxIMPLEMENT_CLASS_COMMON2(name, basename1, basename2, func)           \
    wxIMPLEMENT_CLASS_COMMON(name, basename1, &basename2::ms_classInfo, func)

// -----------------------------------
// for concrete classes
// -----------------------------------

    // Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS(name, basename)                               \
    wxIMPLEMENT_CLASS_COMMON1(name, basename, name::wxCreateObject)           \
    wxObject* name::wxCreateObject()                                          \
        { return new name; }

    // Multiple inheritance with two base classes
#define IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2)                  \
    wxIMPLEMENT_CLASS_COMMON2(name, basename1, basename2,                     \
                              name::wxCreateObject)                           \
    wxObject* name::wxCreateObject()                                          \
        { return new name; }

// -----------------------------------
// for abstract classes
// -----------------------------------

    // Single inheritance with one base class

#define IMPLEMENT_ABSTRACT_CLASS(name, basename)                              \
    wxIMPLEMENT_CLASS_COMMON1(name, basename, NULL)

    // Multiple inheritance with two base classes

#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2)                 \
    wxIMPLEMENT_CLASS_COMMON2(name, basename1, basename2, NULL)

#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

#endif // !wxUSE_EXTENDED_RTTI
#endif // _WX_RTTIH__
