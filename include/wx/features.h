/**
*  Name:        wx/features.h
*  Purpose:     test macros for the features which might be available in some
*               wxWidgets ports but not others
*  Author:      Vadim Zeitlin
*  Modified by: Ryan Norton (Converted to C)
*  Created:     18.03.02
*  Copyright:   (c) 2002 Vadim Zeitlin <vadim@wxwidgets.org>
*  Licence:     wxWindows licence
*/

/* THIS IS A C FILE, DON'T USE C++ FEATURES (IN PARTICULAR COMMENTS) IN IT */

#ifndef _WX_FEATURES_H_
#define _WX_FEATURES_H_

/* always defined now, kept only for compatibility. */
#define wxHAS_RADIO_MENU_ITEMS

/*  the raw keyboard codes are generated under wxGTK and wxMSW only */
#if defined(__WXGTK__) || defined(__WXMSW__) || defined(__WXMAC__) \
    || defined(__WXDFB__)
    #define wxHAS_RAW_KEY_CODES
#else
    #undef wxHAS_RAW_KEY_CODES
#endif

/*  taskbar is implemented in the major ports */
#if defined(__WXMSW__) \
    || defined(__WXGTK__) || defined(__WXX11__) \
    || defined(__WXOSX_MAC__) || defined(__WXQT__)
    #define wxHAS_TASK_BAR_ICON
#else
    #undef wxUSE_TASKBARICON
    #define wxUSE_TASKBARICON 0
    #undef wxHAS_TASK_BAR_ICON
#endif

/*  wxIconLocation appeared in the middle of 2.5.0 so it's handy to have a */
/*  separate define for it */
#define wxHAS_ICON_LOCATION

/*  same for wxCrashReport */
#ifdef __WXMSW__
    #define wxHAS_CRASH_REPORT
#else
    #undef wxHAS_CRASH_REPORT
#endif

/*  wxRE_ADVANCED is always defined now and kept for compatibility only. */
#define wxHAS_REGEX_ADVANCED

/* Pango-based ports and wxDFB use UTF-8 for text and font encodings
 * internally and so their fonts can handle any encodings: */
#if wxUSE_PANGO || defined(__WXDFB__)
    #define wxHAS_UTF8_FONTS
#endif

/* This is defined when the underlying toolkit handles tab traversal natively.
   Otherwise we implement it ourselves in wxControlContainer. */
#if defined(__WXGTK__) || defined(__WXQT__)
    #define wxHAS_NATIVE_TAB_TRAVERSAL
#endif

/* This is defined when the compiler provides some type of extended locale
   functions.  Otherwise, we implement them ourselves to only support the
   'C' locale */
#if defined(HAVE_LOCALE_T) || defined(__VISUALC__)
    #define wxHAS_XLOCALE_SUPPORT
#else
    #undef wxHAS_XLOCALE_SUPPORT
#endif

/* Direct access to bitmap data is not implemented in all ports yet */
#if defined(__WXGTK__) || defined(__WXMAC__) || defined(__WXDFB__) || \
        defined(__WXMSW__) || defined(__WXQT__)

    /*
       HP aCC for PA-RISC can't deal with templates in wx/rawbmp.h.
     */
    #if !(defined(__HP_aCC) && defined(__hppa))
        #define wxHAS_RAW_BITMAP
    #endif
#endif

/* also define deprecated synonym which exists for compatibility only */
#ifdef wxHAS_RAW_BITMAP
    #define wxHAVE_RAW_BITMAP
#endif

/*
    While it should be possible to implement SVG rasterizing without raw bitmap
    support using wxDC::DrawSpline(), currently we don't do it and so FromSVG()
    is only available in the ports providing raw bitmap access.
 */
#if defined(wxHAS_RAW_BITMAP) && wxUSE_NANOSVG
    #define wxHAS_SVG
#endif


// Previously this symbol wasn't defined for all compilers as Bind() couldn't
// be implemented for some of them (notably MSVC 6), but this is not the case
// any more and Bind() is always implemented when using any currently supported
// compiler, so this symbol exists purely for compatibility.
#define wxHAS_EVENT_BIND


/*
    Some platforms use DPI-independent pixels, i.e. pixels actually scale with
    DPI and a 100px-wide window has the same apparent size on the display in
    normal and high (i.e. 2x, or 200% scaling) DPI, while others always use
    physical pixels and a window must be 200px wide to have the same apparent
    size in high DPI as in normal DPI.
 */
#if defined(__WXGTK3__) || defined(__WXMAC__)
    #define wxHAS_DPI_INDEPENDENT_PIXELS

    // This is an older synonym kept only for compatibility
    #define wxHAVE_DPI_INDEPENDENT_PIXELS
#endif

#endif /*  _WX_FEATURES_H_ */

