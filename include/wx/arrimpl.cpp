///////////////////////////////////////////////////////////////////////////////
// Name:        wx/arrimpl.cpp
// Purpose:     helper file for implementation of dynamic lists
// Author:      Vadim Zeitlin
// Modified by:
// Created:     16.10.97
// RCS-ID:      $Id$
// Copyright:   (c) 1997 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

/*****************************************************************************
 * Purpose: implements methods of "template" class declared in               *
 *          DECLARE_OBJARRAY macro and which couldn't be implemented inline  *
 *          (because they need the full definition of type T in scope)       *
 *                                                                           *
 * Usage:   1) #include dynarray.h                                           *
 *          2) WX_DECLARE_OBJARRAY                                           *
 *          3) #include arrimpl.cpp                                          *
 *          4) WX_DEFINE_OBJARRAY                                            *
 *****************************************************************************/

// needed to resolve the conflict between global T and macro parameter T

#define _WX_ERROR_REMOVE2(x)     wxT("bad index in ") wxT(#x) wxT("::RemoveAt()")

// macro implements remaining (not inline) methods of template list
// (it's private to this file)
#undef  _DEFINE_OBJARRAY
#define _DEFINE_OBJARRAY(T, name)                                             \
name::~name()                                                                 \
{                                                                             \
  Empty();                                                                    \
}                                                                             \
                                                                              \
void name::DoCopy(const name& src)                                            \
{                                                                             \
  for ( size_t ui = 0; ui < src.Count(); ui++ )                               \
    Add(src[ui]);                                                             \
}                                                                             \
                                                                              \
name& name::operator=(const name& src)                                        \
{                                                                             \
  Empty();                                                                    \
  DoCopy(src);                                                                \
                                                                              \
  return *this;                                                               \
}                                                                             \
                                                                              \
name::name(const name& src) : wxArrayPtrVoid()                                \
{                                                                             \
  DoCopy(src);                                                                \
}                                                                             \
                                                                              \
void name::DoEmpty()                                                          \
{                                                                             \
  for ( size_t ui = 0; ui < Count(); ui++ )                                   \
    delete (T*)wxBaseArrayPtrVoid::Item(ui);                                  \
}                                                                             \
                                                                              \
void name::RemoveAt(size_t uiIndex, size_t nRemove)                           \
{                                                                             \
  wxCHECK_RET( uiIndex < Count(), _WX_ERROR_REMOVE2(name) );                  \
                                                                              \
  for (size_t i = 0; i < nRemove; i++ )                                       \
    delete (T*)wxBaseArrayPtrVoid::Item(uiIndex + i);                         \
                                                                              \
  wxBaseArrayPtrVoid::RemoveAt(uiIndex, nRemove);                             \
}                                                                             \
                                                                              \
void name::Add(const T& item, size_t nInsert)                                 \
{                                                                             \
  if (nInsert == 0)                                                           \
    return;                                                                   \
  T* pItem = new T(item);                                                     \
  size_t nOldSize = GetCount();                                               \
  if ( pItem != NULL )                                                        \
    wxBaseArrayPtrVoid::Add(pItem, nInsert);                                  \
  for (size_t i = 1; i < nInsert; i++)                                        \
    wxBaseArrayPtrVoid::Item(nOldSize + i) = new T(item);                     \
}                                                                             \
                                                                              \
void name::Insert(const T& item, size_t uiIndex, size_t nInsert)              \
{                                                                             \
  if (nInsert == 0)                                                           \
    return;                                                                   \
  T* pItem = new T(item);                                                     \
  if ( pItem != NULL )                                                        \
    wxBaseArrayPtrVoid::Insert(pItem, uiIndex, nInsert);                      \
  for (size_t i = 1; i < nInsert; i++)                                        \
    wxBaseArrayPtrVoid::Item(uiIndex + i) = new T(item);                      \
}                                                                             \
                                                                              \
int name::Index(const T& Item, bool bFromEnd) const                           \
{                                                                             \
  if ( bFromEnd ) {                                                           \
    if ( Count() > 0 ) {                                                      \
      size_t ui = Count() - 1;                                                \
      do {                                                                    \
        if ( (T*)wxBaseArrayPtrVoid::Item(ui) == &Item )                      \
          return ui;                                                          \
        ui--;                                                                 \
      }                                                                       \
      while ( ui != 0 );                                                      \
    }                                                                         \
  }                                                                           \
  else {                                                                      \
    for( size_t ui = 0; ui < Count(); ui++ ) {                                \
      if( (T*)wxBaseArrayPtrVoid::Item(ui) == &Item )                         \
        return ui;                                                            \
    }                                                                         \
  }                                                                           \
                                                                              \
  return wxNOT_FOUND;                                                         \
}

// redefine the macro so that now it will generate the class implementation
// old value would provoke a compile-time error if this file is not included
#undef  WX_DEFINE_OBJARRAY
#define WX_DEFINE_OBJARRAY(name) _DEFINE_OBJARRAY(_wxObjArray##name, name)
