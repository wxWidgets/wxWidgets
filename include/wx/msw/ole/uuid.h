///////////////////////////////////////////////////////////////////////////////
// Name:        ole/uuid.h
// Purpose:     encapsulates an UUID with some added helper functions
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     11.07.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
//
// Notes:       you should link your project with RPCRT4.LIB!
///////////////////////////////////////////////////////////////////////////////

#ifndef   _OLEUUID_H
#define   _OLEUUID_H

#ifdef __GNUG__
#pragma interface "uuid.h"
#endif

// ------------------------------------------------------------------
// UUID (Universally Unique IDentifier) definition
// ------------------------------------------------------------------

// ----- taken from RPC.H
#ifndef UUID_DEFINED            // in some cases RPC.H will be already
  #ifdef  __WIN32__             // included, so avoid redefinition
    typedef struct                
    {
      unsigned long   Data1;
      unsigned short  Data2;
      unsigned short  Data3;
      unsigned char   Data4[8];
    } UUID;                     // UUID = GUID = CLSID = LIBID = IID
  #else   // WIN16
    #error "Don't know about UUIDs on this platform"
  #endif  // WIN32
#endif  // UUID_DEFINED

#ifndef GUID_DEFINED
  typedef UUID GUID;
  #define UUID_DEFINED          // prevent redefinition
#endif  // GUID_DEFINED

typedef unsigned char uchar;

// ------------------------------------------------------------------
// a class to store UUID and it's string representation
// ------------------------------------------------------------------

// uses RPC functions to create/convert Universally Unique Identifiers
class Uuid
{
private:
  UUID  m_uuid;
  uchar *m_pszUuid;   // this string is alloc'd and freed by RPC 
  char  *m_pszCForm;  // this string is allocated in Set/Create

  void  UuidToCForm();

  // function used to set initial state by all ctors
  void  Init() { m_pszUuid = NULL; m_pszCForm = NULL; }

public:
  // ctors & dtor
  Uuid()                 { Init();            }
  Uuid(const char *pc)   { Init(); Set(pc);   }
  Uuid(const UUID &uuid) { Init(); Set(uuid); }
 ~Uuid();

  // copy ctor and assignment operator needed for this class
  Uuid(const Uuid& uuid);
  Uuid& operator=(const Uuid& uuid);

  // create a brand new UUID
  void Create();

  // set value of UUID 
  bool Set(const char *pc);   // from a string, returns true if ok
  void Set(const UUID& uuid); // from another UUID (never fails)

  // accessors
  operator const UUID*() const { return &m_uuid;             }
  operator const char*() const { return (char *)(m_pszUuid); }

  // return string representation of the UUID in the C form
  // (as in DEFINE_GUID macro)
  const char *CForm() const    { return m_pszCForm;          }
};

#endif    // _OLEUUID_H