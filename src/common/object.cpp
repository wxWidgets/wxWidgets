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

#ifndef WX_PRECOMP
#include "wx/hash.h"
#ifdef wxUSE_SERIAL
#include "wx/objstrm.h"
#include "wx/serbase.h"
#endif
#endif

#include <string.h>
#include <assert.h>

#if (WXDEBUG && wxUSE_MEMORY_TRACING) || wxUSE_DEBUG_CONTEXT
#include "wx/memory.h"
#endif

#if WXDEBUG || wxUSE_DEBUG_CONTEXT
  // for wxObject::Dump
  #include <iostream.h>
#endif

#if !USE_SHARED_LIBRARY
wxClassInfo wxObject::sm_classwxObject((char *) "wxObject", (char *) NULL, (char *) NULL, (int ) sizeof(wxObject), (wxObjectConstructorFn) NULL);
wxClassInfo* wxClassInfo::sm_first = (wxClassInfo *) NULL;
wxHashTable* wxClassInfo::sm_classTable = (wxHashTable*) NULL;
#endif

/*
 * wxWindows root object.
 */

wxObject::wxObject(void)
{
  m_refData = (wxObjectRefData *) NULL;
#ifdef wxUSE_SERIAL
  m_serialObj = (wxObject_Serialize *)NULL;
#endif
}

wxObject::~wxObject(void)
{
	UnRef();
#ifdef wxUSE_SERIAL
	if (m_serialObj)
	  delete m_serialObj;
#endif
}

/*
 * Is this object a kind of (a subclass of) 'info'?
 * E.g. is wxWindow a kind of wxObject?
 * Go from this class to superclass, taking into account
 * two possible base classes.
 */
 
bool wxObject::IsKindOf(wxClassInfo *info) const
{
  wxClassInfo *thisInfo = GetClassInfo();
  if (thisInfo)
    return thisInfo->IsKindOf(info);
  else
    return FALSE;
}

#if WXDEBUG || wxUSE_DEBUG_CONTEXT
void wxObject::Dump(ostream& str)
{
  if (GetClassInfo() && GetClassInfo()->GetClassName())
    str << GetClassInfo()->GetClassName();
  else
    str << "unknown object class";
}
#endif

#if WXDEBUG && wxUSE_MEMORY_TRACING

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

// VC++ 6.0
#if _MSC_VER >= 1200
void operator delete(void* pData, char* /* fileName */, int /* lineNum */)
{
 ::operator delete(pData);
}
#endif

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

wxClassInfo::wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn constr)
{
  m_className = cName;
  m_baseClassName1 = baseName1;
  m_baseClassName2 = baseName2;

  m_objectSize = sz;
  m_objectConstructor = constr;
  
  m_next = sm_first;
  sm_first = this;

  m_baseInfo1 = (wxClassInfo *) NULL;
  m_baseInfo2 = (wxClassInfo *) NULL;
}

wxObject *wxClassInfo::CreateObject(void)
{
  if (m_objectConstructor)
    return (wxObject *)(*m_objectConstructor)();
  else
    return (wxObject *) NULL;
}

wxClassInfo *wxClassInfo::FindClass(char *c)
{
  wxClassInfo *p = sm_first;
  while (p)
  {
    if (p && p->GetClassName() && strcmp(p->GetClassName(), c) == 0)
      return p;
    p = p->m_next;
  }
  return (wxClassInfo *) NULL;
}

// Climb upwards through inheritance hierarchy.
// Dual inheritance is catered for.
bool wxClassInfo::IsKindOf(wxClassInfo *info) const
{
  if (info == NULL)
    return FALSE;

  // For some reason, when making/using a DLL, static data has to be included
  // in both the DLL and the application. This can lead to duplicate
  // wxClassInfo objects, so we have to test the name instead of the pointers.
  // PROBABLY NO LONGER TRUE now I've done DLL creation right.
/*
#if WXMAKINGDLL
  if (GetClassName() && info->GetClassName() && (strcmp(GetClassName(), info->GetClassName()) == 0))
    return TRUE;
#else
*/
  if (this == info)
    return TRUE;

  if (m_baseInfo1)
    if (m_baseInfo1->IsKindOf(info))
      return TRUE;

  if (m_baseInfo2)
    return m_baseInfo2->IsKindOf(info);

  return FALSE;
}

// Set pointers to base class(es) to speed up IsKindOf
void wxClassInfo::InitializeClasses(void)
{
  wxClassInfo::sm_classTable = new wxHashTable(wxKEY_STRING);

  // Index all class infos by their class name
  wxClassInfo *info = sm_first;
  while (info)
  {
    if (info->m_className)
      sm_classTable->Put(info->m_className, (wxObject *)info);
    info = info->m_next;
  }

  // Set base pointers for each wxClassInfo
  info = sm_first;
  while (info)
  {
    if (info->GetBaseClassName1())
      info->m_baseInfo1 = (wxClassInfo *)sm_classTable->Get(info->GetBaseClassName1());
    if (info->GetBaseClassName2())
      info->m_baseInfo2 = (wxClassInfo *)sm_classTable->Get(info->GetBaseClassName2());
    info = info->m_next;
  }
}

void wxClassInfo::CleanUpClasses(void)
{
    delete wxClassInfo::sm_classTable;
    wxClassInfo::sm_classTable = NULL;
}

wxObject *wxCreateDynamicObject(const char *name)
{
    if (wxClassInfo::sm_classTable)
    {
        wxClassInfo *info = (wxClassInfo *)wxClassInfo::sm_classTable->Get(name);
        if (!info)
            return (wxObject *)NULL;

        return info->CreateObject();
    }
    else
    {
        wxClassInfo *info = wxClassInfo::sm_first;
        while (info)
        {
            if (info->m_className && strcmp(info->m_className, name) == 0)
                return info->CreateObject();
            info = info->m_next;
        }
        return (wxObject*) NULL;
    }
    return (wxObject*) NULL;
}

#ifdef wxUSE_SERIAL

#include "wx/serbase.h"
#include "wx/dynlib.h"
#include "wx/msgdlg.h"

wxObject* wxCreateStoredObject( wxInputStream &stream )
{
  wxObjectInputStream obj_s(stream);
  return obj_s.LoadObject();
};

void wxObject::StoreObject( wxObjectOutputStream& stream )
{
  wxString obj_name = wxString(GetClassInfo()->GetClassName()) + "_Serialize";
  wxLibrary *lib = wxTheLibraries.LoadLibrary("wxserial");

  if (!lib) {
    wxMessageBox("Can't load wxSerial dynamic library.", "Alert !");
    return;
  }
  if (!m_serialObj) {
    m_serialObj = (WXSERIAL(wxObject) *)lib->CreateObject( obj_name );

    if (!m_serialObj) {
      wxString message;

      message.Printf("Can't find the serialization object (%s) for the object %s",
                     WXSTRINGCAST obj_name,
                     WXSTRINGCAST GetClassInfo()->GetClassName());
      wxMessageBox(message, "Alert !");
      return;
    }
    m_serialObj->SetObject(this);
  }

  m_serialObj->StoreObject(stream);
}

void wxObject::LoadObject( wxObjectInputStream& stream )
{
  wxString obj_name = wxString(GetClassInfo()->GetClassName()) + "_Serialize";
  wxLibrary *lib = wxTheLibraries.LoadLibrary("wxserial");

  if (!m_serialObj) {
    m_serialObj = (WXSERIAL(wxObject) *)lib->CreateObject( obj_name );

    if (!m_serialObj) {
      wxString message;

      message.Printf("Can't find the serialization object (%s) for the object %s",
                     WXSTRINGCAST obj_name,
                     WXSTRINGCAST GetClassInfo()->GetClassName());
      wxMessageBox(message, "Alert !");
      return;
    }
    m_serialObj->SetObject(this);
  }

  m_serialObj->LoadObject(stream);
}

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
    m_refData = (wxObjectRefData *) NULL;
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

// These are here so we can avoid 'always true/false' warnings
// by referring to these instead of TRUE/FALSE
const bool wxTrue = TRUE;
const bool wxFalse = FALSE;
