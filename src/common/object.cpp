/////////////////////////////////////////////////////////////////////////////
// Name:        object.cpp
// Purpose:     wxObject implementation
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "object.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/hash.h"

#include <string.h>
#include <assert.h>

#if (WXDEBUG && USE_MEMORY_TRACING) || USE_DEBUG_CONTEXT
#include "wx/memory.h"
#endif

#if WXDEBUG || USE_DEBUG_CONTEXT
  // for wxObject::Dump
  #include <iostream.h>
#endif

#if !USE_SHARED_LIBRARY
wxClassInfo wxObject::classwxObject("wxObject", NULL, NULL, sizeof(wxObject), NULL);
wxClassInfo *wxClassInfo::first = NULL;
#endif

/*
 * wxWindows root object.
 */

wxObject::wxObject(void)
{
  m_refData = NULL;
}

wxObject::~wxObject(void)
{
	UnRef();
}

/*
 * Is this object a kind of (a subclass of) 'info'?
 * E.g. is wxWindow a kind of wxObject?
 * Go from this class to superclass, taking into account
 * two possible base classes.
 */
 
bool wxObject::IsKindOf(wxClassInfo *info)
{
  wxClassInfo *thisInfo = GetClassInfo();
  if (thisInfo)
    return thisInfo->IsKindOf(info);
  else
    return FALSE;
}

#if WXDEBUG || USE_DEBUG_CONTEXT
void wxObject::Dump(ostream& str)
{
  if (GetClassInfo() && GetClassInfo()->GetClassName())
    str << GetClassInfo()->GetClassName();
  else
    str << "unknown object class";
}
#endif

#if WXDEBUG && USE_MEMORY_TRACING

#ifdef new
#undef new
#endif

void * wxObject::operator new (size_t size, char * fileName, int lineNum)
{
  return wxDebugAlloc(size, fileName, lineNum, TRUE);
}

void wxObject::operator delete (void * buf)
{
  wxDebugFree(buf);
}

// Cause problems for VC++ - crashes
#ifndef _MSC_VER
void * wxObject::operator new[] (size_t size, char * fileName, int lineNum)
{
  return wxDebugAlloc(size, fileName, lineNum, TRUE, TRUE);
}

void wxObject::operator delete[] (void * buf)
{
  wxDebugFree(buf, TRUE);
}
#endif

#endif

/*
 * Class info: provides run-time class type information.
 */

#ifdef USE_STORABLE_CLASSES

wxClassInfo::wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn fn,
     wxStorableConstructorFn stoFn )
{
  className = cName;
  baseClassName1 = baseName1;
  baseClassName2 = baseName2;

  objectSize = sz;
  objectConstructor = fn;
  storableConstructor = stoFn;
  
  next = first;
  first = this;

  baseInfo1 = NULL;
  baseInfo2 = NULL;
}
   
wxObject* wxClassInfo::CreateObject( istream &stream, char *data )
{
  if (storableConstructor)
    return (wxObject *)(*storableConstructor)( stream, data );
  else
    return NULL;
}

#else
  
wxClassInfo::wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn constr)
{
  className = cName;
  baseClassName1 = baseName1;
  baseClassName2 = baseName2;

  objectSize = sz;
  objectConstructor = constr;
  
  next = first;
  first = this;

  baseInfo1 = NULL;
  baseInfo2 = NULL;
}

#endif

wxObject *wxClassInfo::CreateObject(void)
{
  if (objectConstructor)
    return (wxObject *)(*objectConstructor)();
  else
    return NULL;
}

wxClassInfo *wxClassInfo::FindClass(char *c)
{
  wxClassInfo *p = first;
  while (p)
  {
    if (p && p->GetClassName() && strcmp(p->GetClassName(), c) == 0)
      return p;
    p = p->next;
  }
  return NULL;
}

// Climb upwards through inheritance hierarchy.
// Dual inheritance is catered for.
bool wxClassInfo::IsKindOf(wxClassInfo *info)
{
  if (info == NULL)
    return FALSE;

  // For some reason, when making/using a DLL, static data has to be included
  // in both the DLL and the application. This can lead to duplicate
  // wxClassInfo objects, so we have to test the name instead of the pointers.
#if WXMAKINGDLL
  if (GetClassName() && info->GetClassName() && (strcmp(GetClassName(), info->GetClassName()) == 0))
    return TRUE;
#else
  if (this == info)
    return TRUE;
#endif

  if (baseInfo1)
    if (baseInfo1->IsKindOf(info))
      return TRUE;

  if (baseInfo2)
    return baseInfo2->IsKindOf(info);

  return FALSE;
}

// Set pointers to base class(es) to speed up IsKindOf
void wxClassInfo::InitializeClasses(void)
{
  wxHashTable table(wxKEY_STRING);

  // Index all class infos by their class name
  wxClassInfo *info = first;
  while (info)
  {
    if (info->className)
      table.Put(info->className, (wxObject *)info);
    info = info->next;
  }

  // Set base pointers for each wxClassInfo
  info = first;
  while (info)
  {
    if (info->GetBaseClassName1())
      info->baseInfo1 = (wxClassInfo *)table.Get(info->GetBaseClassName1());
    if (info->GetBaseClassName2())
      info->baseInfo2 = (wxClassInfo *)table.Get(info->GetBaseClassName2());
    info = info->next;
  }
}

wxObject *wxCreateDynamicObject(char *name)
{
  wxClassInfo *info = wxClassInfo::first;
  while (info)
  {
    if (info->className && strcmp(info->className, name) == 0)
      return info->CreateObject();
    info = info->next;
  }
  return NULL;
}

#ifdef USE_STORABLE_CLASSES

wxObject* wxCreateStoredObject( char *name, istream &stream, char *data )
{
  wxClassInfo *info = wxClassInfo::first;
  while (info)
  {
    if (info->className && strcmp(info->className, name) == 0)
      return info->CreateObject( stream, data );
    info = info->next;
  }
  return NULL;
};

#endif

/*
 * wxObject: cloning of objects
 */

void wxObject::Ref(const wxObject& clone)
{
    // delete reference to old data
    UnRef();
    // reference new data
    if (clone.m_refData) {
        m_refData = clone.m_refData;
        ++(m_refData->m_count);
    }
}

void wxObject::UnRef(void)
{
    if (m_refData) {
        assert(m_refData->m_count > 0);
        --(m_refData->m_count);
        if (m_refData->m_count == 0)
            delete m_refData;
    }
    m_refData = NULL;
}

/*
 * wxObjectData
 */

wxObjectRefData::wxObjectRefData(void) : m_count(1)
{
}

wxObjectRefData::~wxObjectRefData(void)
{
}

