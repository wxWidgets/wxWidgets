/////////////////////////////////////////////////////////////////////////////
// Name:        object.cpp
// Purpose:     wxObject implementation
// Author:      Julian Smart
// Modified by: Ron Lee
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Julian Smart and Markus Holzem
//              (c) 2001 Ron Lee <ron@debian.org>
// Licence:     wxWindows license
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
#endif

#include <string.h>
#include <assert.h>

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

#endif

 
wxClassInfo wxObject::sm_classwxObject( wxT("wxObject"), 0, 0,
                                        (int) sizeof(wxObject),
                                        (wxObjectConstructorFn) 0 );
wxClassInfo* wxClassInfo::sm_first = 0;
wxHashTable* wxClassInfo::sm_classTable = 0;

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
        str << "unknown object class";
}
#endif

#if defined(__WXDEBUG__) && wxUSE_MEMORY_TRACING

#ifdef new
#undef new
#endif

void *wxObject::operator new (size_t size, wxChar *fileName, int lineNum)
{
    return wxDebugAlloc(size, fileName, lineNum, TRUE);
}

#ifndef __VISAGECPP__
void wxObject::operator delete (void *buf)
{
    wxDebugFree(buf);
}
#elif __DEBUG_ALLOC__
void wxObject::operator delete (void *buf, const char *_fname, size_t _line)
{
    wxDebugFree(buf);
}
#endif

    // VC++ 6.0

#if defined(__VISUALC__) && (__VISUALC__ >= 1200)
void wxObject::operator delete(void *pData, wxChar *WXUNUSED(fileName), int WXUNUSED(lineNum))
{
    ::operator delete(pData);
}
#endif

    // Cause problems for VC++ - crashes

#if (!defined(__VISUALC__) && wxUSE_ARRAY_MEMORY_OPERATORS ) || defined(__MWERKS__)
void *wxObject::operator new[] (size_t size, wxChar *fileName, int lineNum)
{
    return wxDebugAlloc(size, fileName, lineNum, TRUE, TRUE);
}

void wxObject::operator delete[] (void *buf)
{
    wxDebugFree(buf, TRUE);
}
#endif

#endif  // __WXDEBUG__  && wxUSE_MEMORY_TRACING


// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

wxClassInfo *wxClassInfo::FindClass(const wxChar *className)
{
    for(wxClassInfo *info = sm_first; info ; info = info->m_next)
        if( wxStrcmp(info->GetClassName(), className) == 0 )
            return info;

    return 0;
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

    wxClassInfo::sm_classTable = new wxHashTable(wxKEY_STRING);

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
        if (info->GetBaseClassName1())
            info->m_baseInfo1 = (wxClassInfo *)sm_classTable->Get(info->GetBaseClassName1());
        if (info->GetBaseClassName2())
            info->m_baseInfo2 = (wxClassInfo *)sm_classTable->Get(info->GetBaseClassName2());
    }
}

void wxClassInfo::CleanUpClasses()
{
    delete wxClassInfo::sm_classTable;
    wxClassInfo::sm_classTable = 0;
}


wxObject *wxCreateDynamicObject(const wxChar *name)
{
#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
 DEBUG_PRINTF(wxObject *wxCreateDynamicObject)
#endif

    if (wxClassInfo::sm_classTable)
    {
        wxClassInfo *info = (wxClassInfo *)wxClassInfo::sm_classTable->Get(name);
        return info != 0 ? info->CreateObject() : 0;
    }
    else
    {
        for(wxClassInfo *info = wxClassInfo::sm_first; info; info = info->m_next)
            if (info->m_className && wxStrcmp(info->m_className, name) == 0)
                return info->CreateObject();
        return 0;
    }
}


// ----------------------------------------------------------------------------
// wxClassInfo
// ----------------------------------------------------------------------------

void wxObject::Ref(const wxObject& clone)
{
#if defined(__WXDEBUG__) || wxUSE_DEBUG_CONTEXT
 DEBUG_PRINTF(wxObject::Ref)
#endif

    // delete reference to old data
    UnRef();

    // reference new data
    if( clone.m_refData )
    {
        m_refData = clone.m_refData;
        ++(m_refData->m_count);
    }
}

void wxObject::UnRef()
{
    if( m_refData )
    {
        wxASSERT_MSG( m_refData->m_count > 0, _T("invalid ref data count") );

        if ( !--m_refData->m_count )
            delete m_refData;
        m_refData = 0;
    }
}


#if defined(__DARWIN__) && defined(DYLIB_INIT)

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

    __initialize_Cplusplus();
}

#endif

// vi:sts=4:sw=4:et
