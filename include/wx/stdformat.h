///////////////////////////////////////////////////////////////////////////////
// Name:        wx/stdformat.h
// Purpose:     Support for using wxString with C++20 std::format()
// Author:      Vadim Zeitlin
// Created:     2026-10-07
// Copyright:   (c) 2026 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STDFORMAT_H_
#define _WX_STDFORMAT_H_

#include "wx/string.h"

#if wxCHECK_CXX_STD(202002L) && wxHAS_CXX17_INCLUDE(<format>)

#include "wx/beforestd.h"
#include <format>
#include "wx/afterstd.h"

// This symbol is defined if std::format() can be used with wxString.
#ifdef __cpp_lib_format

#define wxHAS_STD_FORMAT

// Narrow strings are formatted using UTF-8 encoding.
template <>
struct std::formatter<wxString, char> : std::formatter<std::string_view, char>
{
    template <typename FormatContext>
    auto format(const wxString& s, FormatContext& ctx) const
    {
        const wxScopedCharBuffer buf = s.utf8_str();

        return std::formatter<std::string_view, char>::format
               (
                    std::string_view(buf.data(), buf.length()),
                    ctx
               );
    }
};

template <>
struct std::formatter<wxString, wchar_t> : std::formatter<std::wstring_view, wchar_t>
{
    template <typename FormatContext>
    auto format(const wxString& s, FormatContext& ctx) const
    {
        return std::formatter<std::wstring_view, wchar_t>::format
               (
                    s.ToStdWstring(),
                    ctx
               );
    }
};

#endif // __cpp_lib_format

#endif // C++20 with <format>

#endif // _WX_STDFORMAT_H_
