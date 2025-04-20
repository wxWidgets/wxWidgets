///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/elfversion.h
// Purpose:     Helper macro for assigning ELF version to a symbol.
// Author:      Vadim Zeitlin
// Created:     2025-04-14
// Copyright:   (c) 2025 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_ELFVERSION_H_
#define _WX_PRIVATE_ELFVERSION_H_

// This symbol is defined by configure if .symver directive is supported.
#ifdef wxHAVE_ELF_SYMVER
    // Prefer to use the attribute if it is available, as it works with LTO,
    // unlike the assembler directive.
    #ifdef __has_attribute
        #if __has_attribute(__symver__)
            #define wxELF_SYMVER(sym, symver) __attribute__((__symver__(symver)))
        #endif
    #endif

    #ifndef wxELF_SYMVER
        #define wxELF_SYMVER(sym, symver) __asm__(".symver " sym "," symver);
    #endif

    // Using multiple versions for the same symbols may be not supported, in
    // which case we omit it: this results in generating 2 default versions for
    // the same symbol, which looks wrong, but doesn't seem to cause any
    // problems.
    #ifdef wxHAVE_ELF_SYMVER_MULTIPLE
        #define wxELF_SYMVER_NON_DEFAULT(sym, ver) wxELF_SYMVER(sym, ver)
    #else
        #define wxELF_SYMVER_NON_DEFAULT(sym, ver)
    #endif

    // Our version tag depends on whether we're using Unicode or not.
    #if wxUSE_UNICODE
        #define wxMAKE_ELF_VERSION_TAG(ver) "WXU_" ver
    #else
        #define wxMAKE_ELF_VERSION_TAG(ver) "WX_" ver
    #endif

    // This macro is used to repair ABI compatibility problems with the symbols
    // versions: unfortunately, some symbols were initially added with the
    // wrong "3.2" version tag because their definitions in the version script
    // were erroneous, and, even more unfortunately, they were later corrected
    // to use a different version tag, which made the symbols with the old
    // version unavailable in the shared library. This macro allows to define
    // both the old, compatible version ("3.2") and the new ("3.2.N") one for
    // the given symbol to restore ABI compatibility with the previous releases
    // without breaking it with the release containing the corrected version.
    //
    // The parameters are the mangled symbol name (the simplest way to get is
    // probably to use nm or readelf on the object file or library) and the
    // part of the version after "WX[U]_", i.e. the version number itself.
    //
    // Note that this macro takes strings, not symbols, and that it includes
    // the trailing semicolon for consistency with the empty version below.
    #define wxELF_VERSION_COMPAT(sym, ver) \
        wxELF_SYMVER_NON_DEFAULT(sym, sym "@" wxMAKE_ELF_VERSION_TAG("3.2")) \
        wxELF_SYMVER(sym, sym "@@" wxMAKE_ELF_VERSION_TAG(ver))
#else
    #define wxELF_VERSION_COMPAT(sym, ver)
#endif

#endif // _WX_PRIVATE_ELFVERSION_H_
