///////////////////////////////////////////////////////////////////////////////
// Name:        dynarray.h
// Purpose:     auto-resizable (i.e. dynamic) array support
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef   _DYNARRAY_H
#define   _DYNARRAY_H

#ifdef __GNUG__
#pragma interface "dynarray.h"
#endif

#include "wx/defs.h"
#include "wx/debug.h"

/** @name Dynamic arrays and lists
    @memo Arrays which grow on demand and do range checking (only in debug)
  */
//@{

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

/**
 the initial size by which an array/list grows when an element is added
 default value avoids allocate one or two bytes when the array is created
 which is rather inefficient
*/
#define   WX_ARRAY_DEFAULT_INITIAL_SIZE    (16)

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

/**
 callback compare function for quick sort
 must return negative value, 0 or positive value if pItem1 <, = or > pItem2
 */

#ifdef  __VISUALC__
  #define   CMPFUNC_CONV    _cdecl
#else   // !Visual C++
  #define   CMPFUNC_CONV
#endif  // compiler
typedef int (CMPFUNC_CONV *CMPFUNC)(const void* pItem1, const void* pItem2);

// ----------------------------------------------------------------------------
/**
 base class managing data having size of type 'long' (not used directly)

 NB: for efficiency this often used class has no virtual functions (hence no
     VTBL), even dtor is <B>not</B> virtual. If used as expected it won't
     create any problems because ARRAYs from DEFINE_ARRAY have no dtor at all,
     so it's not too important if it's not called (this happens when you cast
     "SomeArray *" as "BaseArray *" and then delete it)

  @memo Base class for template array and list classes
*/
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxBaseArray
{
public:
  /** @name ctors and dtor */
  //@{
    /// default ctor
  wxBaseArray();
    /// copy ctor
  wxBaseArray(const wxBaseArray& array);
    /// assignment operator
  wxBaseArray& operator=(const wxBaseArray& src);
    /// not virtual, see above
    /// EXCEPT for Gnu compiler to reduce warnings...
#ifdef __GNUG__
 virtual
#endif
  ~wxBaseArray();
  //@}

  /** @name memory management */
  //@{
    /// empties the list, but doesn't release memory
  void Empty() { m_uiCount = 0; }
    /// empties the list and releases memory
  void Clear();
    /// preallocates memory for given number of items
  void Alloc(size_t uiSize);
  //@}

  /** @name simple accessors */
  //@{
    /// number of elements in the array
  size_t  Count() const   { return m_uiCount;      }
  size_t  GetCount() const   { return m_uiCount;      }
    /// is it empty?
  bool  IsEmpty() const { return m_uiCount == 0; }
  //@}

protected:
  // these methods are protected because if they were public one could
  // mistakenly call one of them instead of DEFINE_ARRAY's or LIST's
  // type safe methods

  /** @name items access */
  //@{
    /// get item at position uiIndex (range checking is done in debug version)
  long& Item(size_t uiIndex) const
    { wxASSERT( uiIndex < m_uiCount ); return m_pItems[uiIndex]; }
    /// same as Item()
  long& operator[](size_t uiIndex) const { return Item(uiIndex); }
  //@}

  /** @name item management */
  //@{
    /**
      Search the element in the array, starting from the either side
      @param bFromEnd if TRUE, start from the end
      @return index of the first item matched or NOT_FOUND
      @see NOT_FOUND
     */
  int Index(long lItem, bool bFromEnd = FALSE) const;
    /// search for an item using binary search in a sorted array
  int Index(long lItem, CMPFUNC fnCompare) const;
    /// add new element at the end
  void Add(long lItem);
    /// add item assuming the array is sorted with fnCompare function
  void Add(long lItem, CMPFUNC fnCompare);
    /// add new element at given position (it becomes Item[uiIndex])
  void Insert(long lItem, size_t uiIndex);
    /// remove first item matching this value
  void Remove(long lItem);
    /// remove item by index
  void Remove(size_t uiIndex);
  //@}

  /// sort array elements using given compare function
  void Sort(CMPFUNC fnCompare);

private:
  void    Grow();     // makes array bigger if needed

  size_t    m_uiSize,   // current size of the array
          m_uiCount;  // current number of elements

  long   *m_pItems;   // pointer to data
};

// ============================================================================
// template classes
// ============================================================================

// ----------------------------------------------------------------------------
// This macro generates a new array class. It is intended for storage of simple
// types of sizeof()<=sizeof(long) or pointers if sizeof(pointer)<=sizeof(long)
//
// NB: it has only inline functions => takes no space at all
// ----------------------------------------------------------------------------
#define  _WX_DEFINE_ARRAY(T, name)                                  \
typedef int (CMPFUNC_CONV *CMPFUNC##T)(T *pItem1, T *pItem2);       \
class WXDLLEXPORTLOCAL name : public wxBaseArray                                     \
{                                                                   \
public:                                                             \
  name()                                                            \
    { wxASSERT( sizeof(T) <= sizeof(long) ); }                      \
                                                                    \
  name& operator=(const name& src)                                  \
    { ((wxBaseArray *)this)->operator=((const wxBaseArray&)src);    \
      return *this; }                                               \
                                                                    \
  T& operator[](size_t uiIndex) const                                 \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Item(size_t uiIndex) const                                       \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Last() const                                                   \
    { return (T&)(wxBaseArray::Item(Count() - 1)); }                \
                                                                    \
  int Index(T Item, bool bFromEnd = FALSE) const                    \
    { return wxBaseArray::Index((long)Item, bFromEnd); }            \
                                                                    \
  void Add(T Item)                                                  \
    { wxBaseArray::Add((long)Item); }                               \
  void Insert(T Item, size_t uiIndex)                                 \
    { wxBaseArray::Insert((long)Item, uiIndex) ; }                  \
                                                                    \
  void Remove(size_t uiIndex) { wxBaseArray::Remove(uiIndex); }       \
  void Remove(T Item)                                               \
    { int iIndex = Index(Item);                                     \
      wxCHECK2_MSG( iIndex != NOT_FOUND, return,                    \
        "removing inexisting element in wxArray::Remove" );         \
      wxBaseArray::Remove((size_t)iIndex); }                          \
                                                                    \
  void Sort(CMPFUNC##T fCmp) { wxBaseArray::Sort((CMPFUNC)fCmp); }  \
}

// ----------------------------------------------------------------------------
// This is the same as the previous macro, but it defines a sorted array.
// Differences:
//  1) it must be given a COMPARE function in ctor which takes 2 items of type
//     T* and should return -1, 0 or +1 if the first one is less/greater
//     than/equal to the second one.
//  2) the Add() method inserts the item in such was that the array is always
//     sorted (it uses the COMPARE function)
//  3) it has no Sort() method because it's always sorted
//  4) Index() method is much faster (the sorted arrays use binary search
//     instead of linear one), but Add() is slower.
//
// Summary: use this class when the speed of Index() function is important, use
// the normal arrays otherwise.
//
// NB: it has only inline functions => takes no space at all
// ----------------------------------------------------------------------------
#define  _WX_DEFINE_SORTED_ARRAY(T, name)                           \
typedef int (CMPFUNC_CONV *SCMPFUNC##T)(T pItem1, T pItem2);        \
class WXDLLEXPORTLOCAL  name : public wxBaseArray                                     \
{                                                                   \
public:                                                             \
  name(SCMPFUNC##T fn)                                              \
    { wxASSERT( sizeof(T) <= sizeof(long) ); m_fnCompare = fn; }    \
                                                                    \
  name& operator=(const name& src)                                  \
    { ((wxBaseArray *)this)->operator=((const wxBaseArray&)src);    \
      m_fnCompare = src.m_fnCompare;                                \
      return *this; }                                               \
                                                                    \
  T& operator[](size_t uiIndex) const                                 \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Item(size_t uiIndex) const                                       \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Last() const                                                   \
    { return (T&)(wxBaseArray::Item(Count() - 1)); }                \
                                                                    \
  int Index(T Item) const                                           \
    { return wxBaseArray::Index((long)Item, (CMPFUNC)m_fnCompare); }\
                                                                    \
  void Add(T Item)                                                  \
    { wxBaseArray::Add((long)Item, (CMPFUNC)m_fnCompare); }         \
                                                                    \
  void Remove(size_t uiIndex) { wxBaseArray::Remove(uiIndex); }       \
  void Remove(T Item)                                               \
    { int iIndex = Index(Item);                                     \
      wxCHECK2_MSG( iIndex != NOT_FOUND, return,                    \
        "removing inexisting element in wxArray::Remove" );         \
      wxBaseArray::Remove((size_t)iIndex); }                          \
                                                                    \
private:                                                            \
  SCMPFUNC##T m_fnCompare;                                          \
}

// ----------------------------------------------------------------------------
// see WX_DECLARE_LIST and WX_DEFINE_LIST
// ----------------------------------------------------------------------------
#define _WX_DECLARE_LIST(T, name)                                   \
typedef int (CMPFUNC_CONV *CMPFUNC##T)(T** pItem1, T** pItem2);     \
class WXDLLEXPORTLOCAL  name : public wxBaseArray                                     \
{                                                                   \
public:                                                             \
  name() { }                                                        \
  name(const name& src);                                            \
  name& operator=(const name& src);                                 \
                                                                    \
  ~name();                                                          \
                                                                    \
  T& operator[](size_t uiIndex) const                                 \
    { return *(T*)wxBaseArray::Item(uiIndex); }                     \
  T& Item(size_t uiIndex) const                                       \
    { return *(T*)wxBaseArray::Item(uiIndex); }                     \
  T& Last() const                                                   \
    { return *(T*)(wxBaseArray::Item(Count() - 1)); }               \
                                                                    \
  int Index(const T& Item, bool bFromEnd = FALSE) const;            \
                                                                    \
  void Add(const T& Item);                                          \
  void Add(const T* pItem)                                          \
    { wxBaseArray::Add((long)pItem); }                              \
                                                                    \
  void Insert(const T& Item,  size_t uiIndex);                        \
  void Insert(const T* pItem, size_t uiIndex)                         \
    { wxBaseArray::Insert((long)pItem, uiIndex); }                  \
                                                                    \
  void Empty();                                                     \
                                                                    \
  T*   Detach(size_t uiIndex)                                         \
    { T* p = (T*)wxBaseArray::Item(uiIndex);                        \
      wxBaseArray::Remove(uiIndex); return p; }                     \
  void Remove(size_t uiIndex);                                        \
                                                                    \
  void Sort(CMPFUNC##T fCmp) { wxBaseArray::Sort((CMPFUNC)fCmp); }  \
                                                                    \
private:                                                            \
  void DoCopy(const name& src);                                     \
}

// ----------------------------------------------------------------------------
/** @name Macros for definition of dynamic arrays and lists

  These macros are ugly (especially if you look in the sources ;-), but they
  allow us to define 'template' classes without actually using templates.
  <BR>
  <BR>
  Range checking is performed in debug build for both arrays and lists. Type
  checking is done at compile-time. Warning: arrays <I>never</I> shrink, they
  only grow, so loading 10 millions in an array only to delete them 2 lines
  below is <I>not</I> recommended. However, it does free memory when it's
  destroyed, so if you destroy array also, it's ok.
  */
// ----------------------------------------------------------------------------

//@{
  /**
   This macro generates a new array class. It is intended for storage of simple
   types of sizeof()<=sizeof(long) or pointers if sizeof(pointer)<=sizeof(long)
   <BR>
   NB: it has only inline functions => takes no space at all
   <BR>

   @memo declare and define array class 'name' containing elements of type 'T'
  */
#define WX_DEFINE_ARRAY(T, name)  typedef T _A##name;                        \
                                  _WX_DEFINE_ARRAY(_A##name, name)

  /**
   This macro does the same as WX_DEFINE_ARRAY except that the array will be
   sorted with the specified compare function.
   */
#define WX_DEFINE_SORTED_ARRAY(T, name)  typedef T _A##name;                 \
                                  _WX_DEFINE_SORTED_ARRAY(_A##name, name)

  /**
   This macro generates a new list class which owns the objects it contains,
   i.e. it will delete them when it is destroyed. An element is of type T*,
   but arguments of type T& are taken (see below!) and T& is returned.
   <BR>
   Don't use this for simple types such as "int" or "long"!
   You _may_ use it for "double" but it's awfully inefficient.
   <BR>
   <BR>
   Note on Add/Insert functions:
   <BR>
    1) function(T*) gives the object to the list, i.e. it will delete the
       object when it's removed or in the list's dtor
   <BR>
    2) function(T&) will create a copy of the object and work with it
   <BR>
   <BR>
   Also:
   <BR>
    1) Remove() will delete the object after removing it from the list
   <BR>
    2) Detach() just removes the object from the list (returning pointer to it)
   <BR>
   <BR>
   NB1: Base type T should have an accessible copy ctor  if  Add(T&) is used,
   <BR>
   NB2: Never ever cast a list to it's base type: as dtor is <B>not</B> virtual
        it will provoke memory leaks
   <BR>
   <BR>
   some functions of this class are not inline, so it takes some space to
   define new class from this template.

   @memo declare list class 'name' containing elements of type 'T'
  */
#define WX_DECLARE_LIST(T, name)  typedef T _L##name;                 \
                                  _WX_DECLARE_LIST(_L##name, name)
  /**
    To use a list class you must
    <ll>
    <li>#include "dynarray.h"
    <li>DECLARE_LIST(element_type, list_class_name)
    <li>#include "listimpl.cpp"
    <li>DEFINE_LIST(list_class_name)   // same as above!
    </ll>
    <BR><BR>
    This is necessary because at the moment of DEFINE_LIST class element_type
    must be fully defined (i.e. forward declaration is not enough), while
    DECLARE_LIST may be done anywhere. The separation of two allows to break
    cicrcular dependencies with classes which have member variables of list
    type.

    @memo define (must include listimpl.cpp!) list class 'name'
   */
#define WX_DEFINE_LIST(name)       "don't forget to include listimpl.cpp!"
//@}

// ----------------------------------------------------------------------------
/** @name Some commonly used predefined arrays */
// # overhead if not used?
// ----------------------------------------------------------------------------

#define WXDLLEXPORTLOCAL WXDLLEXPORT

//@{
  /** @name ArrayInt */
WX_DEFINE_ARRAY(int,    wxArrayInt);
  /** @name ArrayLong */
WX_DEFINE_ARRAY(long,   wxArrayLong);
  /** @name ArrayPtrVoid */
WX_DEFINE_ARRAY(void *, wxArrayPtrVoid);
//@}

//@}

#undef  WXDLLEXPORTLOCAL
#define WXDLLEXPORTLOCAL

#endif // _DYNARRAY_H

