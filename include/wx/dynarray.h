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

/*
  This header defines the dynamic arrays and object arrays (i.e. arrays which
  own their elements). Dynamic means that the arrays grow automatically as
  needed.

  These macros are ugly (especially if you look in the sources ;-), but they
  allow us to define "template" classes without actually using templates and so
  this works with all compilers (and may be also much faster to compile even
  with a compiler which does support templates). The arrays defined with these
  macros are type-safe.

  Range checking is performed in debug build for both arrays and objarrays but
  not in release build - so using an invalid index will just lead to a crash
  then.

  Note about memory usage: arrays never shrink automatically (although you may
  use Shrink() function explicitly), they only grow, so loading 10 millions in
  an array only to delete them 2 lines below might be a bad idea if the array
  object is not going to be destroyed soon. However, as it does free memory
  when destroyed, it is ok if the array is a local variable.
 */

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

/*
   The initial size by which an array grows when an element is added default
   value avoids allocate one or two bytes when the array is created which is
   rather inefficient
*/
#define WX_ARRAY_DEFAULT_INITIAL_SIZE    (16)

// ----------------------------------------------------------------------------
// types
// ----------------------------------------------------------------------------

/*
    Callback compare function for quick sort.
    
    It must return negative value, 0 or positive value if the first item is
    less than, equal to or greater than the second one.
 */
extern "C"
{
typedef int (wxCMPFUNC_CONV *CMPFUNC)(const void* pItem1, const void* pItem2);
}

// ----------------------------------------------------------------------------
// Base class managing data having size of type 'long' (not used directly)
//
// NB: for efficiency this often used class has no virtual functions (hence no
//     virtual table), even dtor is *not* virtual. If used as expected it
//     won't create any problems because ARRAYs from DEFINE_ARRAY have no dtor
//     at all, so it's not too important if it's not called (this happens when
//     you cast "SomeArray *" as "BaseArray *" and then delete it)
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
  ~wxBaseArray();
  //@}

  /** @name memory management */
  //@{
    /// empties the array, but doesn't release memory
  void Empty() { m_nCount = 0; }
    /// empties the array and releases memory
  void Clear();
    /// preallocates memory for given number of items
  void Alloc(size_t uiSize);
    /// minimizes the memory used by the array (frees unused memory)
  void Shrink();
  //@}

  /** @name simple accessors */
  //@{
    /// number of elements in the array
  size_t  GetCount() const { return m_nCount; }
    /// is it empty?
  bool  IsEmpty() const { return m_nCount == 0; }
    /// this version is obsolete, use GetCount()
  size_t  Count() const { return m_nCount; }
  //@}

protected:
  // these methods are protected because if they were public one could
  // mistakenly call one of them instead of DEFINE_ARRAY's or OBJARRAY's
  // type safe methods

  /** @name items access */
  //@{
    /// get item at position uiIndex (range checking is done in debug version)
  long& Item(size_t uiIndex) const
    { wxASSERT( uiIndex < m_nCount ); return m_pItems[uiIndex]; }
    /// same as Item()
  long& operator[](size_t uiIndex) const { return Item(uiIndex); }
  //@}

  /** @name item management */
  //@{
    /**
      Search the element in the array, starting from the either side
      @param bFromEnd if TRUE, start from the end
      @return index of the first item matched or wxNOT_FOUND
      @see wxNOT_FOUND
     */
  int Index(long lItem, bool bFromEnd = FALSE) const;
    /// search for an item using binary search in a sorted array
  int Index(long lItem, CMPFUNC fnCompare) const;
    /// search for a place to insert the element into a sorted array
  size_t IndexForInsert(long lItem, CMPFUNC fnCompare) const;
    /// add new element at the end
  void Add(long lItem);
    /// add item assuming the array is sorted with fnCompare function
  void Add(long lItem, CMPFUNC fnCompare);
    /// add new element at given position (it becomes Item[uiIndex])
  void Insert(long lItem, size_t uiIndex);
    /// remove first item matching this value
  void Remove(long lItem);
    /// remove item by index
  void RemoveAt(size_t uiIndex);
  //@}

  /// sort array elements using given compare function
  void Sort(CMPFUNC fnCompare);

private:
  void    Grow();     // makes array bigger if needed

  size_t  m_nSize,    // current size of the array
          m_nCount;   // current number of elements

  long   *m_pItems;   // pointer to data
};

// ============================================================================
// The private helper macros containing the core of the array classes
// ============================================================================

// Implementation notes:
//
// JACS: Salford C++ doesn't like 'var->operator=' syntax, as in:
//          { ((wxBaseArray *)this)->operator=((const wxBaseArray&)src);
//       so using a temporary variable instead.
//
// The classes need a (even trivial) ~name() to link under Mac X
//
// _WX_ERROR_REMOVE is needed to resolve the name conflict between the wxT()
// macor and T typedef: we can't use wxT() inside WX_DEFINE_ARRAY!

#define _WX_ERROR_REMOVE wxT("removing inexisting element in wxArray::Remove")

// ----------------------------------------------------------------------------
// _WX_DEFINE_ARRAY: array for simple types
// ----------------------------------------------------------------------------

#define  _WX_DEFINE_ARRAY(T, name, classexp)                        \
wxCOMPILE_TIME_ASSERT(sizeof(T) <= sizeof(long),                    \
                      TypeIsTooBigToBeStoredInWxArray);             \
typedef int (CMPFUNC_CONV *CMPFUNC##T)(T *pItem1, T *pItem2);       \
classexp name : public wxBaseArray                                  \
{                                                                   \
public:                                                             \
  name() { }                                                        \
  ~name() { }                                                       \
                                                                    \
  name& operator=(const name& src)                                  \
    { wxBaseArray* temp = (wxBaseArray*) this;                      \
      (*temp) = ((const wxBaseArray&)src);                          \
      return *this; }                                               \
                                                                    \
  T& operator[](size_t uiIndex) const                               \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Item(size_t uiIndex) const                                     \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Last() const                                                   \
    { return (T&)(wxBaseArray::Item(Count() - 1)); }                \
                                                                    \
  int Index(T Item, bool bFromEnd = FALSE) const                    \
    { return wxBaseArray::Index((long)Item, bFromEnd); }            \
                                                                    \
  void Add(T Item)                                                  \
    { wxBaseArray::Add((long)Item); }                               \
  void Insert(T Item, size_t uiIndex)                               \
    { wxBaseArray::Insert((long)Item, uiIndex) ; }                  \
                                                                    \
  void RemoveAt(size_t uiIndex) { wxBaseArray::RemoveAt(uiIndex); } \
  void Remove(T Item)                                               \
    { int iIndex = Index(Item);                                     \
      wxCHECK2_MSG( iIndex != wxNOT_FOUND, return,                  \
         _WX_ERROR_REMOVE);                                         \
      wxBaseArray::RemoveAt((size_t)iIndex); }                      \
                                                                    \
  void Sort(CMPFUNC##T fCmp) { wxBaseArray::Sort((CMPFUNC)fCmp); }  \
}

// ----------------------------------------------------------------------------
// _WX_DEFINE_SORTED_ARRAY: sorted array for simple data types
// ----------------------------------------------------------------------------

#define _WX_DEFINE_SORTED_ARRAY(T, name, defcomp, classexp)         \
wxCOMPILE_TIME_ASSERT(sizeof(T) <= sizeof(long),                    \
                      TypeIsTooBigToBeStoredInWxArray);             \
typedef int (CMPFUNC_CONV *SCMPFUNC##T)(T pItem1, T pItem2);        \
classexp name : public wxBaseArray                                  \
{                                                                   \
public:                                                             \
  name(SCMPFUNC##T fn defcomp) { m_fnCompare = fn; }                \
                                                                    \
  name& operator=(const name& src)                                  \
    { wxBaseArray* temp = (wxBaseArray*) this;                      \
      (*temp) = ((const wxBaseArray&)src);                          \
      m_fnCompare = src.m_fnCompare;                                \
      return *this; }                                               \
                                                                    \
  T& operator[](size_t uiIndex) const                               \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Item(size_t uiIndex) const                                     \
    { return (T&)(wxBaseArray::Item(uiIndex)); }                    \
  T& Last() const                                                   \
    { return (T&)(wxBaseArray::Item(Count() - 1)); }                \
                                                                    \
  int Index(T Item) const                                           \
    { return wxBaseArray::Index((long)Item, (CMPFUNC)m_fnCompare); }\
                                                                    \
  size_t IndexForInsert(T Item) const                               \
    { return wxBaseArray::IndexForInsert((long)Item,                \
                                         (CMPFUNC)m_fnCompare); }   \
                                                                    \
  void AddAt(T item, size_t index)                                  \
    { wxBaseArray::Insert((long)item, index); }                     \
                                                                    \
  void Add(T Item)                                                  \
    { wxBaseArray::Add((long)Item, (CMPFUNC)m_fnCompare); }         \
                                                                    \
  void RemoveAt(size_t uiIndex) { wxBaseArray::RemoveAt(uiIndex); } \
  void Remove(T Item)                                               \
    { int iIndex = Index(Item);                                     \
      wxCHECK2_MSG( iIndex != wxNOT_FOUND, return,                  \
        _WX_ERROR_REMOVE );                                         \
      wxBaseArray::RemoveAt((size_t)iIndex); }                      \
                                                                    \
private:                                                            \
  SCMPFUNC##T m_fnCompare;                                          \
}

// ----------------------------------------------------------------------------
// _WX_DECLARE_OBJARRAY: an array for pointers to type T with owning semantics
// ----------------------------------------------------------------------------

#define _WX_DECLARE_OBJARRAY(T, name, classexp)                     \
typedef int (CMPFUNC_CONV *CMPFUNC##T)(T** pItem1, T** pItem2);     \
classexp name : public wxBaseArray                                  \
{                                                                   \
public:                                                             \
  name() { }                                                        \
  name(const name& src);                                            \
  name& operator=(const name& src);                                 \
                                                                    \
  ~name();                                                          \
                                                                    \
  T& operator[](size_t uiIndex) const                               \
    { return *(T*)wxBaseArray::Item(uiIndex); }                     \
  T& Item(size_t uiIndex) const                                     \
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
  void Insert(const T& Item,  size_t uiIndex);                      \
  void Insert(const T* pItem, size_t uiIndex)                       \
    { wxBaseArray::Insert((long)pItem, uiIndex); }                  \
                                                                    \
  void Empty() { DoEmpty(); wxBaseArray::Empty(); }                 \
  void Clear() { DoEmpty(); wxBaseArray::Clear(); }                 \
                                                                    \
  T* Detach(size_t uiIndex)                                         \
    { T* p = (T*)wxBaseArray::Item(uiIndex);                        \
      wxBaseArray::RemoveAt(uiIndex); return p; }                   \
  void RemoveAt(size_t uiIndex);                                    \
                                                                    \
  void Sort(CMPFUNC##T fCmp) { wxBaseArray::Sort((CMPFUNC)fCmp); }  \
                                                                    \
private:                                                            \
  void DoEmpty();                                                   \
  void DoCopy(const name& src);                                     \
}

// ============================================================================
// The public macros for declaration and definition of the dynamic arrays
// ============================================================================

// Please note that for each macro WX_FOO_ARRAY we also have
// WX_FOO_EXPORTED_ARRAY and WX_FOO_USER_EXPORTED_ARRAY which are exactly the
// same except that they use an additional __declspec(dllexport) or equivalent
// under Windows if needed.
//
// The first (just EXPORTED) macros do it if wxWindows was compiled as a DLL
// and so must be used used inside the library. The second kind (USER_EXPORTED)
// allow the user code to do it when it wants. This is needed if you have a dll
// that wants to export a wxArray daubed with your own import/export goo.
//
// Finally, you can define the macro below as something special to modify the
// arrays defined by a simple WX_FOO_ARRAY as well. By default is is empty.
#define wxARRAY_DEFAULT_EXPORT

// ----------------------------------------------------------------------------
// WX_DEFINE_ARRAY(T, name) define an array class named "name" containing the
// elements of simple type T such that sizeof(T) <= sizeof(long)
//
// Note that the class defined has only inline function and doesn't take any
// space at all so there is no size penalty for defining multiple array classes
// ----------------------------------------------------------------------------

#define WX_DEFINE_ARRAY(T, name)                                \
    WX_DEFINE_USER_EXPORTED_ARRAY(T, name, wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_EXPORTED_ARRAY(T, name)                       \
    WX_DEFINE_USER_EXPORTED_ARRAY(T, name, WXDLLEXPORT)

#define WX_DEFINE_USER_EXPORTED_ARRAY(T, name, expmode)         \
    typedef T _wxArray##name;                                   \
    _WX_DEFINE_ARRAY(_wxArray##name, name, class expmode)

// ----------------------------------------------------------------------------
// WX_DEFINE_SORTED_ARRAY: this is the same as the previous macro, but it
// defines a sorted array.
//
// Differences:
//  1) it must be given a COMPARE function in ctor which takes 2 items of type
//     T* and should return -1, 0 or +1 if the first one is less/greater
//     than/equal to the second one.
//  2) the Add() method inserts the item in such was that the array is always
//     sorted (it uses the COMPARE function)
//  3) it has no Sort() method because it's always sorted
//  4) Index() method is much faster (the sorted arrays use binary search
//     instead of linear one), but Add() is slower.
//  5) there is no Insert() method because you can't insert an item into the
//     given position in a sorted array but there is IndexForInsert()/AddAt()
//     pair which may be used to optimize a common operation of "insert only if
//     not found"
//
// Note that you have to specify the comparison function when creating the
// objects of this array type. If, as in 99% of cases, the comparison function
// is the same for all objects of a class, WX_DEFINE_SORTED_ARRAY_CMP below is
// more convenient.
//
// Summary: use this class when the speed of Index() function is important, use
// the normal arrays otherwise.
// ----------------------------------------------------------------------------

#define wxARRAY_EMPTY_CMP

#define WX_DEFINE_SORTED_ARRAY(T, name)                         \
    WX_DEFINE_SORTED_USER_EXPORTED_ARRAY(T, name, wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_SORTED_EXPORTED_ARRAY(T, name)                \
    WX_DEFINE_SORTED_USER_EXPORTED_ARRAY(T, name, WXDLLEXPORT)

#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY(T, name, expmode)  \
    typedef T _wxArray##name;                                   \
    _WX_DEFINE_SORTED_ARRAY(_wxArray##name, name, wxARRAY_EMPTY_CMP, class expmode)

// ----------------------------------------------------------------------------
// WX_DEFINE_SORTED_ARRAY_CMP: exactly the same as above but the comparison
// function is provided by this macro and the objects of this class have a
// default constructor which just uses it.
//
// The arguments are: the element type, the comparison function and the array
// name
//
// NB: this is, of course, how WX_DEFINE_SORTED_ARRAY() should have worked from
//     the very beginning - unfortunately I didn't think about this earlier :-(
// ----------------------------------------------------------------------------

#define WX_DEFINE_SORTED_ARRAY_CMP(T, cmpfunc, name)                         \
    WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP(T, cmpfunc, name,               \
                                             wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CMP(T, cmpfunc, name)                \
    WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP(T, cmpfunc, name, WXDLLEXPORT)

#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP(T, cmpfunc, name, expmode)  \
    typedef T _wxArray##name;                                                \
    _WX_DEFINE_SORTED_ARRAY(_wxArray##name, name, = cmpfunc, class expmode)

// ----------------------------------------------------------------------------
// WX_DECLARE_OBJARRAY(T, name): this macro generates a new array class
// named "name" which owns the objects of type T it contains, i.e. it will
// delete them when it is destroyed.
//
// An element is of type T*, but arguments of type T& are taken (see below!)
// and T& is returned.
//
// Don't use this for simple types such as "int" or "long"!
// You _may_ use it for "double" but it's awfully inefficient.
//
// Note on Add/Insert functions:
//  1) function(T*) gives the object to the array, i.e. it will delete the
//     object when it's removed or in the array's dtor
//  2) function(T&) will create a copy of the object and work with it
//
// Also:
//  1) Remove() will delete the object after removing it from the array
//  2) Detach() just removes the object from the array (returning pointer to it)
//
// NB1: Base type T should have an accessible copy ctor if Add(T&) is used
// NB2: Never ever cast a array to it's base type: as dtor is not virtual
//      and so you risk having at least the memory leaks and probably worse
//
// Some functions of this class are not inline, so it takes some space to
// define new class from this template even if you don't use it - which is not
// the case for the simple (non-object) array classes
//
//
// To use an objarray class you must
//      #include "dynarray.h"
//      WX_DECLARE_OBJARRAY(element_type, list_class_name)
//      #include "arrimpl.cpp"
//      WX_DEFINE_OBJARRAY(list_class_name) // name must be the same as above!
//
// This is necessary because at the moment of DEFINE_OBJARRAY class parsing the
// element_type must be fully defined (i.e. forward declaration is not
// enough), while WX_DECLARE_OBJARRAY may be done anywhere. The separation of
// two allows to break cicrcular dependencies with classes which have member
// variables of objarray type.
// ----------------------------------------------------------------------------

#define WX_DECLARE_OBJARRAY(T, name)                        \
    WX_DECLARE_USER_EXPORTED_OBJARRAY(T, name, wxARRAY_DEFAULT_EXPORT)

#define WX_DECLARE_EXPORTED_OBJARRAY(T, name)               \
    WX_DECLARE_USER_EXPORTED_OBJARRAY(T, name, WXDLLEXPORT)

#define WX_DECLARE_USER_EXPORTED_OBJARRAY(T, name, expmode) \
    typedef T _wxObjArray##name;                            \
    _WX_DECLARE_OBJARRAY(_wxObjArray##name, name, class expmode)

// WX_DEFINE_OBJARRAY is going to be redefined when arrimpl.cpp is included,
// try to provoke a human-understandable error if it used incorrectly.
//
// there is no real need for 3 different macros in the DEFINE case but do it
// anyhow for consistency
#define WX_DEFINE_OBJARRAY(name) DidYouIncludeArrimplCpp
#define WX_DEFINE_EXPORTED_OBJARRAY(name)   WX_DEFINE_OBJARRAY(name)
#define WX_DEFINE_USER_EXPORTED_OBJARRAY(name)   WX_DEFINE_OBJARRAY(name)

// ----------------------------------------------------------------------------
// Some commonly used predefined arrays
// ----------------------------------------------------------------------------

WX_DEFINE_EXPORTED_ARRAY(int, wxArrayInt);
WX_DEFINE_EXPORTED_ARRAY(long, wxArrayLong);
WX_DEFINE_EXPORTED_ARRAY(void *, wxArrayPtrVoid);

// -----------------------------------------------------------------------------
// convenience macros
// -----------------------------------------------------------------------------

// append all element of one array to another one
#define WX_APPEND_ARRAY(array, other)                                         \
    {                                                                         \
        size_t count = (other).Count();                                       \
        for ( size_t n = 0; n < count; n++ )                                  \
        {                                                                     \
            (array).Add((other)[n]);                                          \
        }                                                                     \
    }

// delete all array elements
//
// NB: the class declaration of the array elements must be visible from the
//     place where you use this macro, otherwise the proper destructor may not
//     be called (a decent compiler should give a warning about it, but don't
//     count on it)!
#define WX_CLEAR_ARRAY(array)                                                 \
    {                                                                         \
        size_t count = (array).Count();                                       \
        for ( size_t n = 0; n < count; n++ )                                  \
        {                                                                     \
            delete (array)[n];                                                \
        }                                                                     \
                                                                              \
        (array).Empty();                                                      \
    }

#endif // _DYNARRAY_H

