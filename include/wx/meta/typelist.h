///////////////////////////////////////////////////////////////////////////////
// Name:        wx/meta/typelist.h
// Purpose:     definitions for wxTypeList.
// Author:      Ali Kettab
// Created:     2018-10-10
// Copyright:   (c) 2018 Ali Kettab
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_META_TYPELIST_H_
#define _WX_META_TYPELIST_H_

#if !defined(HAVE_VARIADIC_TEMPLATES)
  #error "Variadic templates support required."
#endif // HAVE_VARIADIC_TEMPLATES


namespace wxTypeList
{

// ----------------------------------------------------------------------------
// TList definition; defines size similar to  C++ STL containers size
// ----------------------------------------------------------------------------

template <typename... Ts>
struct TList 
{
    using Type = TList;

#if defined(HAVE_CONSTEXPR)
    // don't define this function altogether if compiler lack constexpr support.
    static constexpr size_t Size() /*noexcept*/ { return sizeof...(Ts); }
#endif
};

// ----------------------------------------------------------------------------
// Computes the length of a typelist at compile-time
// ----------------------------------------------------------------------------

// for compilers supporting constexpr keyword, the static member function Size()
// is the preferred way to get the length of the typelist at compile-time.
// if not, you have to use Sizeof<TList<...>>::value to get the same effect.
template <class TL> struct Sizeof;

template <>
struct Sizeof<TList<>>
{
    enum { value = 0 };
};

template <typename T, typename... Ts>
struct Sizeof<TList<T, Ts...>>
{
    enum { value = 1 + Sizeof<Ts...>::value };
};

// ----------------------------------------------------------------------------
// Access an element by an index
// ----------------------------------------------------------------------------

template <size_t idx, class TL>
struct TypeAt_Impl;

template <typename T, typename... Ts>
struct TypeAt_Impl <0, TList<T, Ts...>>
{
    using Type = T;
};

template <size_t idx, typename T, typename...Ts>
struct TypeAt_Impl <idx, TList<T, Ts...>>
{
    using Type = typename TypeAt_Impl<idx-1, TList<Ts...>>::Type;
};

template <size_t idx, class TL>
struct TypeAt;

template <size_t idx, typename... Ts>
struct TypeAt<idx, TList<Ts...>>
{
private:
    static_assert(sizeof...(Ts) > idx, 
        "Index out of bounds or called on empty list.");
public:
    using Type = typename TypeAt_Impl<idx, TList<Ts...>>::Type;
};

// ----------------------------------------------------------------------------
// Access to the front and the back of the list
// ----------------------------------------------------------------------------

template <class TL>
struct FirstType; 

template <typename... Ts>
struct FirstType<TList<Ts...>>
{
private:
    static_assert(sizeof...(Ts) > 0, "FirstType called on empty list.");
public:
    using Type = typename TypeAt<0, TList<Ts...>>::Type;
};

template <class TL>
struct LastType;

template <typename... Ts>
struct LastType<TList<Ts...>>
{
private:
    static_assert(sizeof...(Ts) > 0, "LastType called on empty list.");
public:
    using Type = typename TypeAt< (sizeof...(Ts)) - 1, TList<Ts...>>::Type;
};

// ----------------------------------------------------------------------------
// Push front and push back
// ----------------------------------------------------------------------------

template <typename T, class TL>
struct PrependType;

template <typename T, typename... Ts>
struct PrependType<T, TList<Ts...>>
{
    using Type = TList<T, Ts...>;
};

template <typename T, class TL>
struct AppendType;

template <typename T, typename... Ts>
struct AppendType<T, TList<Ts...>>
{
    using Type = TList<Ts..., T>;
};

// ----------------------------------------------------------------------------
// Erase an element from the list
// ----------------------------------------------------------------------------

template <typename T, class TL>
struct EraseType; 

template <typename T>
struct EraseType<T, TList<>>
{
    using Type = TList<>;
};

template <typename T, typename... Ts>
struct EraseType <T, TList<T, Ts...>>
{
    using Type = TList<Ts...>;
};

template <typename U, typename T, typename... Ts>
struct EraseType<U, TList<T, Ts...>>
{
    using Type =
        typename PrependType<T, 
            typename EraseType<U, TList<Ts...>>::Type>::Type;
};

// ----------------------------------------------------------------------------
// Erase all duplicates from the list
// ----------------------------------------------------------------------------

template <class TL>
struct EraseDuplType; 

template <>
struct EraseDuplType<TList<>>
{
    using Type = TList<>;
};

template <typename T, typename... Ts>
struct EraseDuplType<TList<T, Ts...>>
{
private:
    using RestOfList = typename EraseDuplType<TList<Ts...>>::Type;
    using NewList = typename EraseType<T, RestOfList>::Type;
public:
    using Type = typename PrependType<T, NewList>::Type;
};

} // namespace wxTypeList


#endif // _WX_META_TYPELIST_H_
