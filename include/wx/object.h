/////////////////////////////////////////////////////////////////////////////
// Name:        object.h
// Purpose:     wxObject class, plus run-time type information macros
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __OBJECTH__
#define __OBJECTH__

#ifdef __GNUG__
#pragma interface "object.h"
#endif

#include "wx/defs.h"

class WXDLLEXPORT wxObject;

#if USE_DYNAMIC_CLASSES

#ifdef __GNUWIN32__
#ifdef GetClassName
#undef GetClassName
#endif
#endif

class WXDLLEXPORT wxClassInfo;

class WXDLLIMPORT istream;
class WXDLLIMPORT ostream;

/*
 * Dynamic object system declarations
 */

typedef wxObject * (*wxObjectConstructorFn) (void);

#ifdef USE_STORABLE_CLASSES
typedef wxObject* (*wxStorableConstructorFn) ( istream &stream, char *data );
#endif

class WXDLLEXPORT wxClassInfo
{
 public:
   char *className;
   char *baseClassName1;
   char *baseClassName2;
   int objectSize;
   wxObjectConstructorFn objectConstructor;
   
#ifdef USE_STORABLE_CLASSES
  wxStorableConstructorFn storableConstructor;
#endif

   // Pointers to base wxClassInfos: set in InitializeClasses
   // called from wx_main.cc
   wxClassInfo *baseInfo1;
   wxClassInfo *baseInfo2;

   static wxClassInfo *first;
   wxClassInfo *next;

#ifdef USE_STORABLE_CLASSES
   wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn fn,
     wxStorableConstructorFn stoFn = NULL );
#else
   wxClassInfo(char *cName, char *baseName1, char *baseName2, int sz, wxObjectConstructorFn fn);
#endif

   wxObject *CreateObject(void);
   
#ifdef USE_STORABLE_CLASSES
   wxObject* CreateObject( istream &stream, char *data );
#endif

   inline char *GetClassName(void) const { return className; }
   inline char *GetBaseClassName1(void) const { return baseClassName1; }
   inline char *GetBaseClassName2(void) const { return baseClassName2; }
   inline int GetSize(void) const { return objectSize; }
   bool IsKindOf(wxClassInfo *info);

   static wxClassInfo *FindClass(char *c);
   // Initializes parent pointers for fast searching.
   static void InitializeClasses(void);
};

wxObject* WXDLLEXPORT wxCreateDynamicObject(char *name);

#ifdef USE_STORABLE_CLASSES
wxObject* WXDLLEXPORT wxCreateStoredObject( char *name, istream &stream, char *data );
#endif

#define DECLARE_DYNAMIC_CLASS(name) \
 public:\
  static wxClassInfo class##name;\
  wxClassInfo *GetClassInfo() \
   { return &name::class##name; }

#define DECLARE_ABSTRACT_CLASS(name) DECLARE_DYNAMIC_CLASS(name)
#define DECLARE_CLASS(name) DECLARE_DYNAMIC_CLASS(name)

#ifdef USE_STORABLE_CLASSES
#define DECLARE_STORABLECLASS(name) DECLARE_DYNAMIC_CLASS(name)
#endif

//////
////// for concrete classes
//////

// Single inheritance with one base class
#define IMPLEMENT_DYNAMIC_CLASS(name, basename) \
wxObject* WXDLLEXPORT_CTORFN wxConstructorFor##name(void) \
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), wxConstructorFor##name);

// Multiple inheritance with two base classes
#define IMPLEMENT_DYNAMIC_CLASS2(name, basename1, basename2) \
wxObject* WXDLLEXPORT_CTORFN wxConstructorFor##name(void) \
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename1, #basename2, sizeof(name), wxConstructorFor##name);

//////
////// for storable classes
//////

#ifdef USE_STORABLE_CLASSES

#define IMPLEMENT_STORABLE_CLASS(name, basename) \
wxObject* WXDLLEXPORT_CTORFN wxStorableConstructorFor##name( istream* stream, char* data )\
	{ return new name( stream, data ); }\
wxObject* WXDLLEXPORT_CTORFN wxConstructorFor##name(void)\
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), wxConstructorFor##name,\
   wxStorableConstructorFor##name );

#define IMPLEMENT_STORABLE_CLASS2(name, basename1, basename2) \
wxObject* WXDLLEXPORT_CTORFN wxStorableConstructorFor##name( istream* stream, char* data )\
	{ return new name( stream, data ); }\
wxObject* WXDLLEXPORT_CTORFN wxConstructorFor##name(void)\
   { return new name; }\
 wxClassInfo name::class##name(#name, #basename1, basename2, sizeof(name), wxConstructorFor##name,\
   wxStorableConstructorFor##name );

#endif
 
//////
////// for abstract classes
//////

// Single inheritance with one base class
#define IMPLEMENT_ABSTRACT_CLASS(name, basename) \
 wxClassInfo name::class##name(#name, #basename, NULL, sizeof(name), NULL);

// Multiple inheritance with two base classes
#define IMPLEMENT_ABSTRACT_CLASS2(name, basename1, basename2) \
 wxClassInfo name::class##name(#name, #basename1, #basename2, sizeof(name), NULL);

#define IMPLEMENT_CLASS IMPLEMENT_ABSTRACT_CLASS
#define IMPLEMENT_CLASS2 IMPLEMENT_ABSTRACT_CLASS2

#define CLASSINFO(name) (&name::class##name)

#else

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

#endif

#define IS_KIND_OF(obj, className) obj->IsKindOf(&className::class##name)

// Unfortunately Borland seems to need this include.
#ifdef __BORLANDC__
#if USE_IOSTREAMH
#include <iostream.h>
#else
#include <iostream>
#endif
#endif

class WXDLLEXPORT wxObjectRefData;

class WXDLLEXPORT wxObject
{
 public:

  // This is equivalent to using the macro DECLARE_ABSTRACT_CLASS
  static wxClassInfo classwxObject;

  wxObject(void);
  virtual ~wxObject(void);

  virtual wxClassInfo *GetClassInfo(void) { return &classwxObject; }

  bool IsKindOf(wxClassInfo *info);

#if DEBUG && USE_MEMORY_TRACING
  void * operator new (size_t size, char * fileName = NULL, int lineNum = 0);
  void operator delete (void * buf);
  
  // Cause problems for VC++
#ifndef _MSC_VER
  void * operator new[] (size_t size, char * fileName = NULL, int lineNum = 0);
  void operator delete[] (void * buf);
#endif

#endif

#if DEBUG || USE_DEBUG_CONTEXT
  virtual void Dump(ostream& str);
#endif

#ifdef USE_STORABLE_CLASSES
  virtual void StoreObject( ostream &WXUNUSED(stream) ) {};
#endif

  // make a 'clone' of the object
  void Ref(const wxObject& clone);

  // destroy a reference
  void UnRef(void);

  inline wxObjectRefData *GetRefData(void) const { return m_refData; }
  inline void SetRefData(wxObjectRefData *data) { m_refData = data; }

protected:
  wxObjectRefData *m_refData;
};

/*
 * wxObjectData
 */

class WXDLLEXPORT wxObjectRefData {

    friend class wxObject;

public:
    wxObjectRefData(void);
    virtual ~wxObjectRefData(void);

    inline int GetRefCount(void) const { return m_count; }
private:
    int m_count;
};

#if DEBUG && USE_GLOBAL_MEMORY_OPERATORS
#ifndef WXDEBUG_NEW
#define WXDEBUG_NEW new(__FILE__,__LINE__)
#endif
#define new WXDEBUG_NEW
#endif

#endif
    // __OBJECTH__


