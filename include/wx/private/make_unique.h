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
    #include <utility>    // for std::forward()

    namespace std
    {
      template <typename T, typename... Args>
      std::unique_ptr<T> make_unique(Args&&... args)
      {
          return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
      }
    }
#endif

#endif // _WX_PRIVATE_MAKE_UNIQUE_H_
