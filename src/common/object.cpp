/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/object.cpp
// Purpose:     wxObject implementation
// Author:      Julian Smart
// Modified by: Ron Lee
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart
//              (c) 2001 Ron Lee <ron@debian.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "object.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/hash.h"
    #include "wx/object.h"
#endif

#include <string.h>

#if (defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
#include "wx/memory.h"
#endif

#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
    // for wxObject::Dump
    #include "wx/ioswrap.h"

    #if defined(__VISAGECPP__)
        #define DEBUG_PRINTF(NAME) { static int raz=0; \
            printf( #NAME " %i\n",raz); fflush(stdout); raz++; }
    #else
        #define DEBUG_PRINTF(NAME)
    #endif
#endif // __WXDEBUG__ || wxUSE_DEBUG_CONTEXT

// we must disable optimizations for VC.NET because otherwise its too eager
// linker discards wxClassInfo objects in release build thus breaking many,
// many things
#if defined __VISUALC__ && __VISUALC__ >= 1300
    #pragma optimize("", off)
#endif

wxClassInfo wxObject::sm_classwxObject( wxT("wxObject"), 0, 0,
                                        (int) sizeof(wxObject),
                                        (wxObjectConstructorFn) 0 );

// restore optimizations
#if defined __VISUALC__ && __VISUALC__ >= 1300
    #pragma optimize("", on)
#endif

wxClassInfo* wxClassInfo::sm_first = NULL;
wxHashTable* wxClassInfo::sm_classTable = NULL;

// These are here so we can avoid 'always true/false' warnings
// by referring to these instead of TRUE/FALSE
const bool wxTrue = TRUE;
const bool wxFalse = FALSE;

// Is this object a kind of (a subclass of) 'info'?
// E.g. is wxWindow a kind of wxObject?
// Go from this class to superclass, taking into account
// two possible base classes.
bool wxObject::IsKindOf(wxClassInfo *info) const
{
    wxClassInfo *thisInfo = GetClassInfo();
    return (thisInfo) ? thisInfo->IsKindOf(info) : FALSE ;
}

#if wxUSE_STD_IOSTREAM && (defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT)
void wxObject::Dump(wxSTD ostream& str)
{
    if (GetClassInfo() && GetClassInfo()->GetClassName())
        str << GetClassInfo()->GetClassName();
    else
        str << _T("unknown object class");
}
#endif


#if defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING && defined( new )
	#undef new
#endif


#ifdef _WX_WANT_NEW_SIZET_WXCHAR_INT
void *wxObject::operator new ( size_t size, const wxChar *fileName, int lineNum )
{
    return wxDebugAlloc(size, (wxChar*) fileName, lineNum, TRUE);
}
#endif

#ifdef _WX_WANT_DELETE_VOID
void wxObject::operator delete ( void *buf )
{
    wxDebugFree(buf);
}
#endif

#ifdef _WX_WANT_DELETE_VOID_CONSTCHAR_SIZET
void wxObject::operator delete ( void *buf, const char *_fname, size_t _line )
{
    wxDebugFree(buf);
}
#endif

#ifdef _WX_WANT_DELETE_VOID_WXCHAR_INT
void wxObject::operator delete ( void *buf, const wxChar *WXUNUSED(fileName), int WXUNUSED(lineNum) )
{
     wxDebugFree(buf);
}
#endif

#ifdef _WX_WANT_ARRAY_NEW_SIZET_WXCHAR_INT
void *wxObject::operator new[] ( size_t size, const wxChar* fileName, int lineNum )
{
    return wxDebugAlloc(size, (wxChar*) fileName, lineNum, TRUE, TRUE);
}
#endif

#ifdef _WX_WANT_ARRAY_DELETE_VOID
void wxObject::operator delete[] ( void *buf )
{
    wxDebugFree(buf, TRUE);
}
#endif

#ifdef _WX_WANT_ARRAY_DELETE_VOID_WXCHAR_INT
void wxObject::operator delete[] (void * buf, const wxChar*  WXUNUSED(fileName), int WXUNUSED(lineNum) )
{
    wxDebugFree(buf, TRUE);
}
#endif


// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

wxClassInfo::~wxClassInfo()
{
    // remove this object from the linked list of all class infos: if we don't
    // do it, loading/unloading a DLL containing static wxClassInfo objects is
    // not going to work
    if ( this == sm_first )
    {
        sm_first = m_next;
    }
    else
    {
        wxClassInfo *info = sm_first;
        while (info)
        {
            if ( info->m_next == this )
            {
                info->m_next = m_next;
                break;
            }

            info = info->m_next;
        }
    }
}

wxClassInfo *wxClassInfo::FindClass(const wxChar *className)
{
    if ( sm_classTable )
    {
        return (wxClassInfo *)wxClassInfo::sm_classTable->Get(className);
    }
    else
    {
        for ( wxClassInfo *info = sm_first; info ; info = info->m_next )
        {
            if ( wxStrcmp(info->GetClassName(), className) == 0 )
                return info;
        }

        return NULL;
    }
}

// a tiny InitializeClasses() helper
/* static */
inline wxClassInfo *wxClassInfo::GetBaseByName(const wxChar *name)
{
    if ( !name )
        return NULL;

    wxClassInfo *classInfo = (wxClassInfo *)sm_classTable->Get(name);

#ifdef __WXDEBUG__
    // this must be fixed, other things will work wrongly later if you get this
    if ( !classInfo )
    {
        wxFAIL_MSG( wxString::Format
                    (
                        _T("base class '%s' is unknown to wxWindows RTTI"),
                        name
                    ) );
    }
#endif // __WXDEBUG__

    return classInfo;
}

// Set pointers to base class(es) to speed up IsKindOf
void wxClassInfo::InitializeClasses()
{
    // using IMPLEMENT_DYNAMIC_CLASS() macro twice (which may happen if you
    // link any object module twice mistakenly) will break this function
    // because it will enter an infinite loop and eventually die with "out of
    // memory" - as this is quite hard to detect if you're unaware of this,
    // try to do some checks here

#ifdef __WXDEBUG__
    static const size_t nMaxClasses = 10000;    // more than we'll ever have
    size_t nClass = 0;
#endif

    sm_classTable = new wxHashTable(wxKEY_STRING);

        // Index all class infos by their class name

    wxClassInfo *info;
    for(info = sm_first; info; info = info->m_next)
    {
        if (info->m_className)
        {
            wxASSERT_MSG( ++nClass < nMaxClasses,
                          _T("an infinite loop detected - have you used IMPLEMENT_DYNAMIC_CLASS() twice (may be by linking some object module(s) twice)?") );
            sm_classTable->Put(info->m_className, (wxObject *)info);
        }
    }

        // Set base pointers for each wxClassInfo

    for(info = sm_first; info; info = info->m_next)
    {
        info->m_baseInfo1 = GetBaseByName(info->GetBaseClassName1());
        info->m_baseInfo2 = GetBaseByName(info->GetBaseClassName2());
    }
}

void wxClassInfo::CleanUpClasses()
{
    delete wxClassInfo::sm_classTable;
    wxClassInfo::sm_classTable = NULL;
}


wxObject *wxCreateDynamicObject(const wxChar *name)
{
#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
    DEBUG_PRINTF(wxObject *wxCreateDynamicObject)
#endif

    if ( wxClassInfo::sm_classTable )
    {
        wxClassInfo *info = (wxClassInfo *)wxClassInfo::sm_classTable->Get(name);
        return info ? info->CreateObject() : NULL;
    }
    else // no sm_classTable yet
    {
        for ( wxClassInfo *info = wxClassInfo::sm_first;
              info;
              info = info->m_next )
        {
            if (info->m_className && wxStrcmp(info->m_className, name) == 0)
                return info->CreateObject();
        }

        return NULL;
    }
}


// ----------------------------------------------------------------------------
// wxObject
// ----------------------------------------------------------------------------

// Initialize ref data from another object (needed for copy constructor and
// assignment operator)
void wxObject::InitFrom(const wxObject& other)
{
    m_refData = other.m_refData;
    if ( m_refData )
        m_refData->m_count++;
}

void wxObject::Ref(const wxObject& clone)
{
#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
    DEBUG_PRINTF(wxObject::Ref)
#endif

    // nothing to be done
    if (m_refData == clone.m_refData)
        return;

    // delete reference to old data
    UnRef();

    // reference new data
    if ( clone.m_refData )
    {
        m_refData = clone.m_refData;
        ++(m_refData->m_count);
    }
}

void wxObject::UnRef()
{
    if ( m_refData )
    {
        wxASSERT_MSG( m_refData->m_count > 0, _T("invalid ref data count") );

        if ( !--m_refData->m_count )
            delete m_refData;
        m_refData = NULL;
    }
}

void wxObject::AllocExclusive()
{
    if ( !m_refData )
    {
        m_refData = CreateRefData();
    }
    else if ( m_refData->GetRefCount() > 1 )
    {
        // note that ref is not going to be destroyed in this case
        const wxObjectRefData* ref = m_refData;
        UnRef();

        // ... so we can still access it
        m_refData = CloneRefData(ref);
    }
    //else: ref count is 1, we are exclusive owners of m_refData anyhow

    wxASSERT_MSG( m_refData && m_refData->GetRefCount() == 1,
                  _T("wxObject::AllocExclusive() failed.") );
}

wxObjectRefData *wxObject::CreateRefData() const
{
    // if you use AllocExclusive() you must override this method
    wxFAIL_MSG( _T("CreateRefData() must be overridden if called!") );

    return NULL;
}

wxObjectRefData *
wxObject::CloneRefData(const wxObjectRefData * WXUNUSED(data)) const
{
    // if you use AllocExclusive() you must override this method
    wxFAIL_MSG( _T("CloneRefData() must be overridden if called!") );

    return NULL;
}

// ----------------------------------------------------------------------------
// misc
// ----------------------------------------------------------------------------

#if defined(__DARWIN__) && defined(WXMAKINGDLL)

extern "C" {
    void __initialize_Cplusplus(void);
    void wxWindowsDylibInit(void);
};

// Dynamic shared library (dylib) initialization routine
//   required to initialize static C++ objects bacause of lazy dynamic linking
//   http://developer.apple.com/techpubs/macosx/Essentials/
//          SystemOverview/Frameworks/Dynamic_Shared_Libraries.html

void wxWindowsDylibInit()
{
    // The function __initialize_Cplusplus() must be called from the shared
    // library initialization routine to cause the static C++ objects in
    // the library to be initialized (reference number 2441683).

    // This only seems to be necessary if the library initialization routine
    // needs to use the static C++ objects
    __initialize_Cplusplus();
}

#endif

// vi:sts=4:sw=4:et
