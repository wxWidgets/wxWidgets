///////////////////////////////////////////////////////////////////////////////
// Name:        wx/dynarray.h
// Purpose:     auto-resizable (i.e. dynamic) array support
// Author:      Vadim Zeitlin
// Modified by:
// Created:     12.09.97
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _DYNARRAY_H
#define   _DYNARRAY_H

#include "wx/defs.h"

#if wxUSE_STD_CONTAINERS
    #include "wx/beforestd.h"
    #include <vector>
    #include <algorithm>
    #include "wx/afterstd.h"

    #define wxDynArrayBaseTemplate std::vector
#else // !wxUSE_STD_CONTAINERS
    #include "wx/vector.h"

    #define wxDynArrayBaseTemplate wxVector
#endif // wxUSE_STD_CONTAINERS/!wxUSE_STD_CONTAINERS

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

#define _WX_ERROR_REMOVE "removing inexistent element in wxArray::Remove"

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

template<class T>
class wxArray_SortFunction
{
public:
    typedef int (wxCMPFUNC_CONV *CMPFUNC)(T* pItem1, T* pItem2);

    wxArray_SortFunction(CMPFUNC f) : m_f(f) { }
    bool operator()(const T& i1, const T& i2)
      { return m_f((T*)&i1, (T*)&i2) < 0; }
private:
    CMPFUNC m_f;
};

template<class T, typename F>
class wxSortedArray_SortFunction
{
public:
    typedef F CMPFUNC;

    wxSortedArray_SortFunction(CMPFUNC f) : m_f(f) { }
    bool operator()(const T& i1, const T& i2)
      { return m_f(i1, i2) < 0; }
private:
    CMPFUNC m_f;
};

#define  _WX_DECLARE_BASEARRAY(T, name, classexp)                   \
   typedef int (wxCMPFUNC_CONV *CMPFUN##name)(T pItem1, T pItem2);  \
   typedef wxSortedArray_SortFunction<T, CMPFUN##name> name##_Predicate; \
   _WX_DECLARE_BASEARRAY_2(T, name, name##_Predicate, classexp)

#define  _WX_DECLARE_BASEARRAY_2(T, name, predicate, classexp)      \
class name : public wxDynArrayBaseTemplate<T>                       \
{                                                                   \
  typedef predicate Predicate;                                      \
  typedef predicate::CMPFUNC SCMPFUNC;                              \
public:                                                             \
  typedef wxArray_SortFunction<T>::CMPFUNC CMPFUNC;                 \
                                                                    \
public:                                                             \
  typedef T base_type;                                              \
                                                                    \
  name() : wxDynArrayBaseTemplate<T>() { }                          \
  name(size_type n) : wxDynArrayBaseTemplate<T>(n) { }              \
  name(size_type n, const_reference v)                              \
    : wxDynArrayBaseTemplate<T>(n, v) { }                           \
  template <class InputIterator>                                    \
  name(InputIterator first, InputIterator last)                     \
    : wxDynArrayBaseTemplate<T>(first, last) { }                    \
                                                                    \
  void Empty() { clear(); }                                         \
  void Clear() { clear(); }                                         \
  void Alloc(size_t uiSize) { reserve(uiSize); }                    \
  void Shrink() { name tmp(*this); swap(tmp); }                     \
                                                                    \
  size_t GetCount() const { return size(); }                        \
  void SetCount(size_t n, T v = T()) { resize(n, v); }              \
  bool IsEmpty() const { return empty(); }                          \
  size_t Count() const { return size(); }                           \
                                                                    \
  T& Item(size_t uiIndex) const                                     \
    { wxASSERT( uiIndex < size() ); return (T&)operator[](uiIndex); }   \
  T& Last() const { return Item(size() - 1); }                      \
                                                                    \
  int Index(T item, bool bFromEnd = false) const                    \
  {                                                                 \
      if ( bFromEnd )                                               \
      {                                                             \
          const const_reverse_iterator b = rbegin(),                \
                                       e = rend();                  \
          for ( const_reverse_iterator i = b; i != e; ++i )         \
              if ( *i == item )                                     \
                  return (int)(e - i - 1);                          \
      }                                                             \
      else                                                          \
      {                                                             \
          const const_iterator b = begin(),                         \
                               e = end();                           \
          for ( const_iterator i = b; i != e; ++i )                 \
              if ( *i == item )                                     \
                  return (int)(i - b);                              \
      }                                                             \
                                                                    \
      return wxNOT_FOUND;                                           \
  }                                                                 \
  int Index(T lItem, CMPFUNC fnCompare) const                       \
  {                                                                 \
      Predicate p((SCMPFUNC)fnCompare);                             \
      const_iterator i = std::lower_bound(begin(), end(), lItem, p);\
      return i != end() && !p(lItem, *i) ? (int)(i - begin())       \
                                         : wxNOT_FOUND;             \
  }                                                                 \
  size_t IndexForInsert(T lItem, CMPFUNC fnCompare) const           \
  {                                                                 \
      Predicate p((SCMPFUNC)fnCompare);                             \
      const_iterator i = std::lower_bound(begin(), end(), lItem, p);\
      return i - begin();                                           \
  }                                                                 \
  void Add(T lItem, size_t nInsert = 1)                             \
    { insert(end(), nInsert, lItem); }                              \
  size_t Add(T lItem, CMPFUNC fnCompare)                            \
  {                                                                 \
      size_t n = IndexForInsert(lItem, fnCompare);                  \
      Insert(lItem, n);                                             \
      return n;                                                     \
  }                                                                 \
  void Insert(T lItem, size_t uiIndex, size_t nInsert = 1)          \
    { insert(begin() + uiIndex, nInsert, lItem); }                  \
  void Remove(T lItem)                                              \
  {                                                                 \
    int n = Index(lItem);                                           \
    wxCHECK_RET( n != wxNOT_FOUND, _WX_ERROR_REMOVE );              \
    RemoveAt((size_t)n);                                            \
  }                                                                 \
  void RemoveAt(size_t uiIndex, size_t nRemove = 1)                 \
    { erase(begin() + uiIndex, begin() + uiIndex + nRemove); }      \
                                                                    \
  void Sort(CMPFUNC fCmp)                                           \
  {                                                                 \
    wxArray_SortFunction<T> p(fCmp);                                \
    std::sort(begin(), end(), p);                                   \
  }                                                                 \
}

// ----------------------------------------------------------------------------
// _WX_DEFINE_TYPEARRAY: array for simple types
// ----------------------------------------------------------------------------

// in STL case we don't need the entire base arrays hack as standard container
// don't suffer from alignment/storage problems as our home-grown do
#define  _WX_DEFINE_TYPEARRAY(T, name, base, classexp)                \
    _WX_DECLARE_BASEARRAY(T, name, classexp)

#define  _WX_DEFINE_TYPEARRAY_PTR(T, name, base, classexp)         \
         _WX_DEFINE_TYPEARRAY(T, name, base, classexp)

// ----------------------------------------------------------------------------
// _WX_DEFINE_SORTED_TYPEARRAY: sorted array for simple data types
//    cannot handle types with size greater than pointer because of sorting
// ----------------------------------------------------------------------------

#define _WX_DEFINE_SORTED_TYPEARRAY_2(T, name, base, defcomp, classexp, comptype)\
wxCOMPILE_TIME_ASSERT2(sizeof(T) <= sizeof(base::base_type),          \
                       TypeTooBigToBeStoredInSorted##base,            \
                       name);                                         \
classexp name : public base                                           \
{                                                                     \
  typedef comptype SCMPFUNC;                                          \
public:                                                               \
  name(comptype fn defcomp) { m_fnCompare = fn; }                     \
                                                                      \
  name& operator=(const name& src)                                    \
    { base* temp = (base*) this;                                      \
      (*temp) = ((const base&)src);                                   \
      m_fnCompare = src.m_fnCompare;                                  \
      return *this; }                                                 \
                                                                      \
  T& operator[](size_t uiIndex) const                                 \
    { return (T&)(base::operator[](uiIndex)); }                       \
  T& Item(size_t uiIndex) const                                       \
    { return (T&)(base::operator[](uiIndex)); }                       \
  T& Last() const                                                     \
    { return (T&)(base::operator[](size() - 1)); }                    \
                                                                      \
  int Index(T lItem) const                                            \
    { return base::Index(lItem, (CMPFUNC)m_fnCompare); }              \
                                                                      \
  size_t IndexForInsert(T lItem) const                                \
    { return base::IndexForInsert(lItem, (CMPFUNC)m_fnCompare); }     \
                                                                      \
  void AddAt(T item, size_t index)                                    \
    { base::insert(begin() + index, item); }                          \
                                                                      \
  size_t Add(T lItem)                                                 \
    { return base::Add(lItem, (CMPFUNC)m_fnCompare); }                \
  void push_back(T lItem)                                             \
    { Add(lItem); }                                                   \
                                                                      \
  void RemoveAt(size_t uiIndex, size_t nRemove = 1)                   \
    { base::erase(begin() + uiIndex, begin() + uiIndex + nRemove); }  \
  void Remove(T lItem)                                                \
    { int iIndex = Index(lItem);                                      \
      wxCHECK_RET( iIndex != wxNOT_FOUND, _WX_ERROR_REMOVE );         \
      base::erase(begin() + iIndex); }                                \
                                                                      \
private:                                                              \
  comptype m_fnCompare;                                               \
}


// ----------------------------------------------------------------------------
// _WX_DECLARE_OBJARRAY: an array for pointers to type T with owning semantics
// ----------------------------------------------------------------------------

#define _WX_DECLARE_OBJARRAY(T, name, base, classexp)                    \
typedef int (CMPFUNC_CONV *CMPFUNC##T)(T **pItem1, T **pItem2);          \
classexp name : protected base                                           \
{                                                                        \
typedef int (CMPFUNC_CONV *CMPFUNC##base)(void **pItem1, void **pItem2); \
typedef base base_array;                                                 \
public:                                                                  \
  name() { }                                                             \
  name(const name& src);                                                 \
  name& operator=(const name& src);                                      \
                                                                         \
  ~name();                                                               \
                                                                         \
  void Alloc(size_t count) { base::reserve(count); }                     \
  void reserve(size_t count) { base::reserve(count); }                   \
  size_t GetCount() const { return base_array::size(); }                 \
  size_t size() const { return base_array::size(); }                     \
  bool IsEmpty() const { return base_array::empty(); }                   \
  bool empty() const { return base_array::empty(); }                     \
  size_t Count() const { return base_array::size(); }                    \
  void Shrink() { base::Shrink(); }                                      \
                                                                         \
  T& operator[](size_t uiIndex) const                                    \
    { return *(T*)base::operator[](uiIndex); }                           \
  T& Item(size_t uiIndex) const                                          \
    { return *(T*)base::operator[](uiIndex); }                           \
  T& Last() const                                                        \
    { return *(T*)(base::operator[](size() - 1)); }                      \
                                                                         \
  int Index(const T& lItem, bool bFromEnd = false) const;                \
                                                                         \
  void Add(const T& lItem, size_t nInsert = 1);                          \
  void Add(const T* pItem)                                               \
    { base::push_back((T*)pItem); }                                      \
  void push_back(const T* pItem)                                         \
    { base::push_back((T*)pItem); }                                      \
  void push_back(const T& lItem)                                         \
    { Add(lItem); }                                                      \
                                                                         \
  void Insert(const T& lItem,  size_t uiIndex, size_t nInsert = 1);      \
  void Insert(const T* pItem, size_t uiIndex)                            \
    { base::insert(begin() + uiIndex, (T*)pItem); }                      \
                                                                         \
  void Empty() { DoEmpty(); base::clear(); }                             \
  void Clear() { DoEmpty(); base::clear(); }                             \
                                                                         \
  T* Detach(size_t uiIndex)                                              \
    { T* p = (T*)base::operator[](uiIndex);                              \
      base::erase(begin() + uiIndex); return p; }                        \
  void RemoveAt(size_t uiIndex, size_t nRemove = 1);                     \
                                                                         \
  void Sort(CMPFUNC##T fCmp) { base::Sort((CMPFUNC##base)fCmp); }        \
                                                                         \
private:                                                                 \
  void DoEmpty();                                                        \
  void DoCopy(const name& src);                                          \
}

// ============================================================================
// The public macros for declaration and definition of the dynamic arrays
// ============================================================================

// Please note that for each macro WX_FOO_ARRAY we also have
// WX_FOO_EXPORTED_ARRAY and WX_FOO_USER_EXPORTED_ARRAY which are exactly the
// same except that they use an additional __declspec(dllexport) or equivalent
// under Windows if needed.
//
// The first (just EXPORTED) macros do it if wxWidgets was compiled as a DLL
// and so must be used used inside the library. The second kind (USER_EXPORTED)
// allow the user code to do it when it wants. This is needed if you have a dll
// that wants to export a wxArray daubed with your own import/export goo.
//
// Finally, you can define the macro below as something special to modify the
// arrays defined by a simple WX_FOO_ARRAY as well. By default is empty.
#define wxARRAY_DEFAULT_EXPORT

// ----------------------------------------------------------------------------
// WX_DECLARE_BASEARRAY(T, name) declare an array class named "name" containing
// the elements of type T
// ----------------------------------------------------------------------------

#define WX_DECLARE_BASEARRAY(T, name)                             \
    WX_DECLARE_USER_EXPORTED_BASEARRAY(T, name, wxARRAY_DEFAULT_EXPORT)

#define WX_DECLARE_EXPORTED_BASEARRAY(T, name)                    \
    WX_DECLARE_USER_EXPORTED_BASEARRAY(T, name, WXDLLIMPEXP_CORE)

#define WX_DECLARE_USER_EXPORTED_BASEARRAY(T, name, expmode)      \
    typedef T _wxArray##name;                                     \
    _WX_DECLARE_BASEARRAY(_wxArray##name, name, class expmode)

// ----------------------------------------------------------------------------
// WX_DEFINE_TYPEARRAY(T, name, base) define an array class named "name" deriving
// from class "base" containing the elements of type T
//
// Note that the class defined has only inline function and doesn't take any
// space at all so there is no size penalty for defining multiple array classes
// ----------------------------------------------------------------------------

#define WX_DEFINE_TYPEARRAY(T, name, base)                        \
    WX_DEFINE_TYPEARRAY_WITH_DECL(T, name, base, class wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_TYPEARRAY_PTR(T, name, base)                        \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, base, class wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_EXPORTED_TYPEARRAY(T, name, base)               \
    WX_DEFINE_TYPEARRAY_WITH_DECL(T, name, base, class WXDLLIMPEXP_CORE)

#define WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, base)               \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, base, class WXDLLIMPEXP_CORE)

#define WX_DEFINE_USER_EXPORTED_TYPEARRAY(T, name, base, expdecl) \
    WX_DEFINE_TYPEARRAY_WITH_DECL(T, name, base, class expdecl)

#define WX_DEFINE_USER_EXPORTED_TYPEARRAY_PTR(T, name, base, expdecl) \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, base, class expdecl)

#define WX_DEFINE_TYPEARRAY_WITH_DECL(T, name, base, classdecl) \
    typedef T _wxArray##name;                                   \
    _WX_DEFINE_TYPEARRAY(_wxArray##name, name, base, classdecl)

#define WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, base, classdecl) \
    typedef T _wxArray##name;                                          \
    _WX_DEFINE_TYPEARRAY_PTR(_wxArray##name, name, base, classdecl)

// ----------------------------------------------------------------------------
// WX_DEFINE_SORTED_TYPEARRAY: this is the same as the previous macro, but it
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
// is the same for all objects of a class, WX_DEFINE_SORTED_TYPEARRAY_CMP below
// is more convenient.
//
// Summary: use this class when the speed of Index() function is important, use
// the normal arrays otherwise.
// ----------------------------------------------------------------------------

// we need a macro which expands to nothing to pass correct number of
// parameters to a nested macro invocation even when we don't have anything to
// pass it
#define wxARRAY_EMPTY

#define WX_DEFINE_SORTED_TYPEARRAY(T, name, base)                         \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, base,               \
                                             wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_SORTED_EXPORTED_TYPEARRAY(T, name, base)                \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, base, WXDLLIMPEXP_CORE)

#define WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, base, expmode)  \
    typedef T _wxArray##name;                                             \
    typedef int (CMPFUNC_CONV *SCMPFUNC##name)(T pItem1, T pItem2);       \
    _WX_DEFINE_SORTED_TYPEARRAY_2(_wxArray##name, name, base,             \
                                wxARRAY_EMPTY, class expmode, SCMPFUNC##name)

// ----------------------------------------------------------------------------
// WX_DEFINE_SORTED_TYPEARRAY_CMP: exactly the same as above but the comparison
// function is provided by this macro and the objects of this class have a
// default constructor which just uses it.
//
// The arguments are: the element type, the comparison function and the array
// name
//
// NB: this is, of course, how WX_DEFINE_SORTED_TYPEARRAY() should have worked
//     from the very beginning - unfortunately I didn't think about this earlier
// ----------------------------------------------------------------------------

#define WX_DEFINE_SORTED_TYPEARRAY_CMP(T, cmpfunc, name, base)               \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, base,     \
                                                 wxARRAY_DEFAULT_EXPORT)

#define WX_DEFINE_SORTED_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, base)      \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, base,     \
                                                 WXDLLIMPEXP_CORE)

#define WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, base, \
                                                     expmode)                \
    typedef T _wxArray##name;                                                \
    typedef int (CMPFUNC_CONV *SCMPFUNC##name)(T pItem1, T pItem2);          \
    _WX_DEFINE_SORTED_TYPEARRAY_2(_wxArray##name, name, base, = cmpfunc,     \
                                class expmode, SCMPFUNC##name)

// ----------------------------------------------------------------------------
// WX_DECLARE_OBJARRAY(T, name): this macro generates a new array class
// named "name" which owns the objects of type T it contains, i.e. it will
// delete them when it is destroyed.
//
// An element is of type T*, but arguments of type T& are taken (see below!)
// and T& is returned.
//
// Don't use this for simple types such as "int" or "long"!
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
    WX_DECLARE_USER_EXPORTED_OBJARRAY(T, name, WXDLLIMPEXP_CORE)

#define WX_DECLARE_OBJARRAY_WITH_DECL(T, name, decl) \
    typedef T _wxObjArray##name;                            \
    _WX_DECLARE_OBJARRAY(_wxObjArray##name, name, wxArrayPtrVoid, decl)

#define WX_DECLARE_USER_EXPORTED_OBJARRAY(T, name, expmode) \
    WX_DECLARE_OBJARRAY_WITH_DECL(T, name, class expmode)

// WX_DEFINE_OBJARRAY is going to be redefined when arrimpl.cpp is included,
// try to provoke a human-understandable error if it used incorrectly.
//
// there is no real need for 3 different macros in the DEFINE case but do it
// anyhow for consistency
#define WX_DEFINE_OBJARRAY(name) DidYouIncludeArrimplCpp
#define WX_DEFINE_EXPORTED_OBJARRAY(name)   WX_DEFINE_OBJARRAY(name)
#define WX_DEFINE_USER_EXPORTED_OBJARRAY(name)   WX_DEFINE_OBJARRAY(name)

// ----------------------------------------------------------------------------
// Some commonly used predefined base arrays
// ----------------------------------------------------------------------------

WX_DECLARE_USER_EXPORTED_BASEARRAY(const void *, wxBaseArrayPtrVoid,
                                   WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_BASEARRAY(char, wxBaseArrayChar, WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_BASEARRAY(short, wxBaseArrayShort, WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_BASEARRAY(int, wxBaseArrayInt, WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_BASEARRAY(long, wxBaseArrayLong, WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_BASEARRAY(size_t, wxBaseArraySizeT, WXDLLIMPEXP_BASE);
WX_DECLARE_USER_EXPORTED_BASEARRAY(double, wxBaseArrayDouble, WXDLLIMPEXP_BASE);

// ----------------------------------------------------------------------------
// Convenience macros to define arrays from base arrays
// ----------------------------------------------------------------------------

#define WX_DEFINE_ARRAY(T, name)                                       \
    WX_DEFINE_TYPEARRAY(T, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_ARRAY_PTR(T, name)                                \
    WX_DEFINE_TYPEARRAY_PTR(T, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_EXPORTED_ARRAY(T, name)                              \
    WX_DEFINE_EXPORTED_TYPEARRAY(T, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_EXPORTED_ARRAY_PTR(T, name)                       \
    WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_ARRAY_WITH_DECL_PTR(T, name, decl)                \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArrayPtrVoid, decl)
#define WX_DEFINE_USER_EXPORTED_ARRAY(T, name, expmode)                \
    WX_DEFINE_TYPEARRAY_WITH_DECL(T, name, wxBaseArrayPtrVoid, wxARRAY_EMPTY expmode)
#define WX_DEFINE_USER_EXPORTED_ARRAY_PTR(T, name, expmode)         \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArrayPtrVoid, wxARRAY_EMPTY expmode)

#define WX_DEFINE_ARRAY_CHAR(T, name)                                 \
    WX_DEFINE_TYPEARRAY_PTR(T, name, wxBaseArrayChar)
#define WX_DEFINE_EXPORTED_ARRAY_CHAR(T, name)                        \
    WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, wxBaseArrayChar)
#define WX_DEFINE_USER_EXPORTED_ARRAY_CHAR(T, name, expmode)          \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArrayChar, wxARRAY_EMPTY expmode)

#define WX_DEFINE_ARRAY_SHORT(T, name)                                 \
    WX_DEFINE_TYPEARRAY_PTR(T, name, wxBaseArrayShort)
#define WX_DEFINE_EXPORTED_ARRAY_SHORT(T, name)                        \
    WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, wxBaseArrayShort)
#define WX_DEFINE_USER_EXPORTED_ARRAY_SHORT(T, name, expmode)          \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArrayShort, wxARRAY_EMPTY expmode)

#define WX_DEFINE_ARRAY_INT(T, name)                                   \
    WX_DEFINE_TYPEARRAY_PTR(T, name, wxBaseArrayInt)
#define WX_DEFINE_EXPORTED_ARRAY_INT(T, name)                          \
    WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, wxBaseArrayInt)
#define WX_DEFINE_USER_EXPORTED_ARRAY_INT(T, name, expmode)            \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArrayInt, wxARRAY_EMPTY expmode)

#define WX_DEFINE_ARRAY_LONG(T, name)                                  \
    WX_DEFINE_TYPEARRAY_PTR(T, name, wxBaseArrayLong)
#define WX_DEFINE_EXPORTED_ARRAY_LONG(T, name)                         \
    WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, wxBaseArrayLong)
#define WX_DEFINE_USER_EXPORTED_ARRAY_LONG(T, name, expmode)           \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArrayLong, wxARRAY_EMPTY expmode)

#define WX_DEFINE_ARRAY_SIZE_T(T, name)                                  \
    WX_DEFINE_TYPEARRAY_PTR(T, name, wxBaseArraySizeT)
#define WX_DEFINE_EXPORTED_ARRAY_SIZE_T(T, name)                         \
    WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, wxBaseArraySizeT)
#define WX_DEFINE_USER_EXPORTED_ARRAY_SIZE_T(T, name, expmode)           \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArraySizeT, wxARRAY_EMPTY expmode)

#define WX_DEFINE_ARRAY_DOUBLE(T, name)                                \
    WX_DEFINE_TYPEARRAY_PTR(T, name, wxBaseArrayDouble)
#define WX_DEFINE_EXPORTED_ARRAY_DOUBLE(T, name)                       \
    WX_DEFINE_EXPORTED_TYPEARRAY_PTR(T, name, wxBaseArrayDouble)
#define WX_DEFINE_USER_EXPORTED_ARRAY_DOUBLE(T, name, expmode)         \
    WX_DEFINE_TYPEARRAY_WITH_DECL_PTR(T, name, wxBaseArrayDouble, wxARRAY_EMPTY expmode)

// ----------------------------------------------------------------------------
// Convenience macros to define sorted arrays from base arrays
// ----------------------------------------------------------------------------

#define WX_DEFINE_SORTED_ARRAY(T, name)                                \
    WX_DEFINE_SORTED_TYPEARRAY(T, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY(T, name)                       \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY(T, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY(T, name, expmode)         \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, wxBaseArrayPtrVoid, wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_CHAR(T, name)                          \
    WX_DEFINE_SORTED_TYPEARRAY(T, name, wxBaseArrayChar)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CHAR(T, name)                 \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY(T, name, wxBaseArrayChar)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CHAR(T, name, expmode)   \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, wxBaseArrayChar, wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_SHORT(T, name)                          \
    WX_DEFINE_SORTED_TYPEARRAY(T, name, wxBaseArrayShort)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_SHORT(T, name)                 \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY(T, name, wxBaseArrayShort)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_SHORT(T, name, expmode)   \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, wxBaseArrayShort, wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_INT(T, name)                            \
    WX_DEFINE_SORTED_TYPEARRAY(T, name, wxBaseArrayInt)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_INT(T, name)                   \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY(T, name, wxBaseArrayInt)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_INT(T, name, expmode)     \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, wxBaseArrayInt, expmode)

#define WX_DEFINE_SORTED_ARRAY_LONG(T, name)                           \
    WX_DEFINE_SORTED_TYPEARRAY(T, name, wxBaseArrayLong)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_LONG(T, name)                  \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY(T, name, wxBaseArrayLong)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_LONG(T, name, expmode)    \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, wxBaseArrayLong, expmode)

#define WX_DEFINE_SORTED_ARRAY_SIZE_T(T, name)                           \
    WX_DEFINE_SORTED_TYPEARRAY(T, name, wxBaseArraySizeT)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_SIZE_T(T, name)                  \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY(T, name, wxBaseArraySizeT)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_SIZE_T(T, name, expmode)    \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY(T, name, wxBaseArraySizeT, wxARRAY_EMPTY expmode)

// ----------------------------------------------------------------------------
// Convenience macros to define sorted arrays from base arrays
// ----------------------------------------------------------------------------

#define WX_DEFINE_SORTED_ARRAY_CMP(T, cmpfunc, name)                   \
    WX_DEFINE_SORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CMP(T, cmpfunc, name)          \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayPtrVoid)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP(T, cmpfunc,           \
                                                     name, expmode)    \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name,     \
                                                 wxBaseArrayPtrVoid,   \
                                                 wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_CMP_CHAR(T, cmpfunc, name)             \
    WX_DEFINE_SORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayChar)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CMP_CHAR(T, cmpfunc, name)    \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayChar)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP_CHAR(T, cmpfunc,      \
                                                       name, expmode)  \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name,     \
                                                 wxBaseArrayChar,      \
                                                 wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_CMP_SHORT(T, cmpfunc, name)             \
    WX_DEFINE_SORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayShort)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CMP_SHORT(T, cmpfunc, name)    \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayShort)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP_SHORT(T, cmpfunc,     \
                                                       name, expmode)  \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name,     \
                                                 wxBaseArrayShort,     \
                                                 wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_CMP_INT(T, cmpfunc, name)               \
    WX_DEFINE_SORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayInt)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CMP_INT(T, cmpfunc, name)      \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayInt)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP_INT(T, cmpfunc,       \
                                                     name, expmode)    \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name,     \
                                                 wxBaseArrayInt,       \
                                                 wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_CMP_LONG(T, cmpfunc, name)              \
    WX_DEFINE_SORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayLong)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CMP_LONG(T, cmpfunc, name)     \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArrayLong)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP_LONG(T, cmpfunc,      \
                                                      name, expmode)   \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name,     \
                                                 wxBaseArrayLong,      \
                                                 wxARRAY_EMPTY expmode)

#define WX_DEFINE_SORTED_ARRAY_CMP_SIZE_T(T, cmpfunc, name)              \
    WX_DEFINE_SORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArraySizeT)
#define WX_DEFINE_SORTED_EXPORTED_ARRAY_CMP_SIZE_T(T, cmpfunc, name)     \
    WX_DEFINE_SORTED_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name, wxBaseArraySizeT)
#define WX_DEFINE_SORTED_USER_EXPORTED_ARRAY_CMP_SIZE_T(T, cmpfunc,      \
                                                      name, expmode)   \
    WX_DEFINE_SORTED_USER_EXPORTED_TYPEARRAY_CMP(T, cmpfunc, name,     \
                                                 wxBaseArraySizeT,     \
                                                 wxARRAY_EMPTY expmode)

// ----------------------------------------------------------------------------
// Some commonly used predefined arrays
// ----------------------------------------------------------------------------

WX_DEFINE_USER_EXPORTED_ARRAY_SHORT(short, wxArrayShort, class WXDLLIMPEXP_BASE);
WX_DEFINE_USER_EXPORTED_ARRAY_INT(int, wxArrayInt, class WXDLLIMPEXP_BASE);
WX_DEFINE_USER_EXPORTED_ARRAY_DOUBLE(double, wxArrayDouble, class WXDLLIMPEXP_BASE);
WX_DEFINE_USER_EXPORTED_ARRAY_LONG(long, wxArrayLong, class WXDLLIMPEXP_BASE);
WX_DEFINE_USER_EXPORTED_ARRAY_PTR(void *, wxArrayPtrVoid, class WXDLLIMPEXP_BASE);

// -----------------------------------------------------------------------------
// convenience macros
// -----------------------------------------------------------------------------

// prepend all element of one array to another one; e.g. if first array contains
// elements X,Y,Z and the second contains A,B,C (in those orders), then the
// first array will be result as A,B,C,X,Y,Z
#define WX_PREPEND_ARRAY(array, other)                                        \
    {                                                                         \
        size_t wxAAcnt = (other).size();                                      \
        (array).reserve(wxAAcnt);                                             \
        for ( size_t wxAAn = 0; wxAAn < wxAAcnt; wxAAn++ )                    \
        {                                                                     \
            (array).Insert((other)[wxAAn], wxAAn);                            \
        }                                                                     \
    }

// append all element of one array to another one
#define WX_APPEND_ARRAY(array, other)                                         \
    {                                                                         \
        size_t wxAAcnt = (other).size();                                      \
        (array).reserve(wxAAcnt);                                             \
        for ( size_t wxAAn = 0; wxAAn < wxAAcnt; wxAAn++ )                    \
        {                                                                     \
            (array).push_back((other)[wxAAn]);                                \
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
        size_t wxAAcnt = (array).size();                                      \
        for ( size_t wxAAn = 0; wxAAn < wxAAcnt; wxAAn++ )                    \
        {                                                                     \
            delete (array)[wxAAn];                                            \
        }                                                                     \
                                                                              \
        (array).clear();                                                      \
    }

#endif // _DYNARRAY_H
