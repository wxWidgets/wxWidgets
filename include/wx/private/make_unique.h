///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/make_unique.h
// Purpose:     Provide implementation of std::make_unique for C++11
// Author:      Vadim Zeitlin
// Created:     2025-12-10
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_MAKE_UNIQUE_H_
#define _WX_PRIVATE_MAKE_UNIQUE_H_

#include <memory>

#if !defined(_MSC_VER) && __cplusplus < 201402L
    #include <cstddef>      // for std::size_t
    #include <type_traits>  // for std::remove_extent
    #include <utility>      // for std::forward()

    namespace wxPrivate
    {
      // This helper is used to select the appropriate overload below, as in
      // N3656 which had originally specified std::make_unique.
      template <typename T>
      struct MakeUniqueRet { typedef std::unique_ptr<T> SingleObject; };

      template <typename T>
      struct MakeUniqueRet<T[]> { typedef std::unique_ptr<T[]> UnknownBound; };

      template <typename T, std::size_t N>
      struct MakeUniqueRet<T[N]> { typedef void KnownBound; };
    }

    namespace std
    {
      template <typename T, typename... Args>
      typename wxPrivate::MakeUniqueRet<T>::SingleObject
      make_unique(Args&&... args)
      {
          return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
      }

      template <typename T>
      typename wxPrivate::MakeUniqueRet<T>::UnknownBound
      make_unique(std::size_t n)
      {
          typedef typename std::remove_extent<T>::type U;
          return std::unique_ptr<T>(new U[n]());
      }

      // Creating arrays of known bound is not allowed.
      template <typename T, typename... Args>
      typename wxPrivate::MakeUniqueRet<T>::KnownBound
      make_unique(Args&&...) = delete;
    }
#endif

#endif // _WX_PRIVATE_MAKE_UNIQUE_H_
