/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/wxcrt.cpp
// Purpose:     wxChar CRT wrappers implementation
// Author:      Ove Kaven
// Modified by: Ron Lee, Francesco Montorsi
// Created:     09/04/99
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets copyright
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ===========================================================================
// headers, declarations, constants
// ===========================================================================

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/wxchar.h"

#define _ISOC9X_SOURCE 1 // to get vsscanf()
#define _BSD_SOURCE    1 // to still get strdup()

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef __WXWINCE__
    #include <time.h>
    #include <locale.h>
#else
    #include "wx/msw/wince/time.h"
#endif

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/hash.h"
    #include "wx/utils.h"     // for wxMin and wxMax
    #include "wx/log.h"
#endif

#if defined(__WIN32__) && defined(wxNEED_WX_CTYPE_H)
  #include <windef.h>
    #include <winbase.h>
    #include <winnls.h>
    #include <winnt.h>
#endif

#ifndef wxStrtoll
    #ifdef __WXWINCE__
        // there is no errno.h under CE apparently
        #define wxSET_ERRNO(value)
    #else
        #include <errno.h>

        #define wxSET_ERRNO(value) errno = value
    #endif
#endif

#if defined(__MWERKS__) && __MSL__ >= 0x6000
namespace std {}
using namespace std ;
#endif

#if wxUSE_WCHAR_T
size_t WXDLLEXPORT wxMB2WC(wchar_t *buf, const char *psz, size_t n)
{
  // assume that we have mbsrtowcs() too if we have wcsrtombs()
#ifdef HAVE_WCSRTOMBS
  mbstate_t mbstate;
  memset(&mbstate, 0, sizeof(mbstate_t));
#endif

  if (buf) {
    if (!n || !*psz) {
      if (n) *buf = wxT('\0');
      return 0;
    }
#ifdef HAVE_WCSRTOMBS
    return mbsrtowcs(buf, &psz, n, &mbstate);
#else
    return wxMbstowcs(buf, psz, n);
#endif
  }

  // note that we rely on common (and required by Unix98 but unfortunately not
  // C99) extension which allows to call mbs(r)towcs() with NULL output pointer
  // to just get the size of the needed buffer -- this is needed as otherwise
  // we have no idea about how much space we need and if the CRT doesn't
  // support it (the only currently known example being Metrowerks, see
  // wx/wxchar.h) we don't use its mbstowcs() at all
#ifdef HAVE_WCSRTOMBS
  return mbsrtowcs((wchar_t *) NULL, &psz, 0, &mbstate);
#else
  return wxMbstowcs((wchar_t *) NULL, psz, 0);
#endif
}

size_t WXDLLEXPORT wxWC2MB(char *buf, const wchar_t *pwz, size_t n)
{
#ifdef HAVE_WCSRTOMBS
  mbstate_t mbstate;
  memset(&mbstate, 0, sizeof(mbstate_t));
#endif

  if (buf) {
    if (!n || !*pwz) {
      // glibc2.1 chokes on null input
      if (n) *buf = '\0';
      return 0;
    }
#ifdef HAVE_WCSRTOMBS
    return wcsrtombs(buf, &pwz, n, &mbstate);
#else
    return wxWcstombs(buf, pwz, n);
#endif
  }

#ifdef HAVE_WCSRTOMBS
  return wcsrtombs((char *) NULL, &pwz, 0, &mbstate);
#else
  return wxWcstombs((char *) NULL, pwz, 0);
#endif
}
#endif // wxUSE_WCHAR_T

bool WXDLLEXPORT wxOKlibc()
{
#if wxUSE_WCHAR_T && defined(__UNIX__) && defined(__GLIBC__) && !defined(__WINE__)
  // glibc 2.0 uses UTF-8 even when it shouldn't
  wchar_t res = 0;
  if ((MB_CUR_MAX == 2) &&
      (wxMB2WC(&res, "\xdd\xa5", 1) == 1) &&
      (res==0x765)) {
    // this is UTF-8 allright, check whether that's what we want
    char *cur_locale = setlocale(LC_CTYPE, NULL);
    if ((strlen(cur_locale) < 4) ||
            (strcasecmp(cur_locale + strlen(cur_locale) - 4, "utf8")) ||
            (strcasecmp(cur_locale + strlen(cur_locale) - 5, "utf-8"))) {
      // nope, don't use libc conversion
      return false;
    }
  }
#endif
  return true;
}

// ============================================================================
// printf() functions business
// ============================================================================

// special test mode: define all functions below even if we don't really need
// them to be able to test them
#ifdef wxTEST_PRINTF
    #undef wxFprintf
    #undef wxPrintf
    #undef wxSprintf
    #undef wxVfprintf
    #undef wxVsprintf
    #undef wxVprintf
    #undef wxVsnprintf_

    #define wxNEED_WPRINTF

    int wxVfprintf( FILE *stream, const wxChar *format, va_list argptr );
#endif

#if defined(__DMC__)
    /* Digital Mars adds count to _stprintf (C99) so convert */
    #if wxUSE_UNICODE
        int wxSprintf (wchar_t * __RESTRICT s, const wchar_t * __RESTRICT format, ... )
        {
            va_list arglist;

            va_start( arglist, format );
            int iLen = swprintf ( s, -1, format, arglist );
            va_end( arglist );
            return iLen ;
        }

    #endif // wxUSE_UNICODE

#endif //__DMC__

// ----------------------------------------------------------------------------
// implement the standard IO functions for wide char if libc doesn't have them
// ----------------------------------------------------------------------------

#ifdef wxNEED_FPUTS
int wxFputs(const wchar_t *ws, FILE *stream)
{
    wxCharBuffer buf(wxConvLibc.cWC2MB(ws));
    if ( !buf )
        return -1;

    // counting the number of wide characters written isn't worth the trouble,
    // simply distinguish between ok and error
    return fputs(buf, stream) == -1 ? -1 : 0;
}
#endif // wxNEED_FPUTS

#ifdef wxNEED_PUTS
int wxPuts(const wxChar *ws)
{
    int rc = wxFputs(ws, stdout);
    if ( rc != -1 )
    {
        if ( wxFputs(L"\n", stdout) == -1 )
            return -1;

        rc++;
    }

    return rc;
}
#endif // wxNEED_PUTS

#ifdef wxNEED_PUTC
int /* not wint_t */ wxPutc(wchar_t wc, FILE *stream)
{
    wchar_t ws[2] = { wc, L'\0' };

    return wxFputs(ws, stream);
}
#endif // wxNEED_PUTC

// NB: we only implement va_list functions here, the ones taking ... are
//     defined below for wxNEED_PRINTF_CONVERSION case anyhow and we reuse
//     the definitions there to avoid duplicating them here
#ifdef wxNEED_WPRINTF

// TODO: implement the scanf() functions
int vwscanf(const wxChar *format, va_list argptr)
{
    wxFAIL_MSG( _T("TODO") );

    return -1;
}

int vswscanf(const wxChar *ws, const wxChar *format, va_list argptr)
{
    // The best we can do without proper Unicode support in glibc is to
    // convert the strings into MB representation and run ANSI version
    // of the function. This doesn't work with %c and %s because of difference
    // in size of char and wchar_t, though.

    wxCHECK_MSG( wxStrstr(format, _T("%s")) == NULL, -1,
                 _T("incomplete vswscanf implementation doesn't allow %s") );
    wxCHECK_MSG( wxStrstr(format, _T("%c")) == NULL, -1,
                 _T("incomplete vswscanf implementation doesn't allow %c") );

    va_list argcopy;
    wxVaCopy(argcopy, argptr);
    return vsscanf(wxConvLibc.cWX2MB(ws), wxConvLibc.cWX2MB(format), argcopy);
}

int vfwscanf(FILE *stream, const wxChar *format, va_list argptr)
{
    wxFAIL_MSG( _T("TODO") );

    return -1;
}

#define vswprintf wxVsnprintf_

int vfwprintf(FILE *stream, const wxChar *format, va_list argptr)
{
    wxString s;
    int rc = s.PrintfV(format, argptr);

    if ( rc != -1 )
    {
        // we can't do much better without Unicode support in libc...
        if ( fprintf(stream, "%s", (const char*)s.mb_str() ) == -1 )
            return -1;
    }

    return rc;
}

int vwprintf(const wxChar *format, va_list argptr)
{
    return wxVfprintf(stdout, format, argptr);
}

#endif // wxNEED_WPRINTF

#ifdef wxNEED_PRINTF_CONVERSION

// ----------------------------------------------------------------------------
// wxFormatConverter: class doing the "%s" -> "%ls" conversion
// ----------------------------------------------------------------------------

/*
   Here are the gory details. We want to follow the Windows/MS conventions,
   that is to have

   In ANSI mode:

   format specifier         results in
   -----------------------------------
   %c, %hc, %hC             char
   %lc, %C, %lC             wchar_t

   In Unicode mode:

   format specifier         results in
   -----------------------------------
   %hc, %C, %hC             char
   %c, %lc, %lC             wchar_t


   while on POSIX systems we have %C identical to %lc and %c always means char
   (in any mode) while %lc always means wchar_t,

   So to use native functions in order to get our semantics we must do the
   following translations in Unicode mode (nothing to do in ANSI mode):

   wxWidgets specifier      POSIX specifier
   ----------------------------------------

   %hc, %C, %hC             %c
   %c                       %lc


   And, of course, the same should be done for %s as well.
*/

class wxFormatConverter
{
public:
    wxFormatConverter(const wxChar *format);

    // notice that we only translated the string if m_fmtOrig == NULL (as set
    // by CopyAllBefore()), otherwise we should simply use the original format
    operator const wxChar *() const
        { return m_fmtOrig ? m_fmtOrig : m_fmt.c_str(); }

private:
    // copy another character to the translated format: this function does the
    // copy if we are translating but doesn't do anything at all if we don't,
    // so we don't create the translated format string at all unless we really
    // need to (i.e. InsertFmtChar() is called)
    wxChar CopyFmtChar(wxChar ch)
    {
        if ( !m_fmtOrig )
        {
            // we're translating, do copy
            m_fmt += ch;
        }
        else
        {
            // simply increase the count which should be copied by
            // CopyAllBefore() later if needed
            m_nCopied++;
        }

        return ch;
    }

    // insert an extra character
    void InsertFmtChar(wxChar ch)
    {
        if ( m_fmtOrig )
        {
            // so far we haven't translated anything yet
            CopyAllBefore();
        }

        m_fmt += ch;
    }

    void CopyAllBefore()
    {
        wxASSERT_MSG( m_fmtOrig && m_fmt.empty(), _T("logic error") );

        m_fmt = wxString(m_fmtOrig, m_nCopied);

        // we won't need it any longer
        m_fmtOrig = NULL;
    }

    static bool IsFlagChar(wxChar ch)
    {
        return ch == _T('-') || ch == _T('+') ||
               ch == _T('0') || ch == _T(' ') || ch == _T('#');
    }

    void SkipDigits(const wxChar **ptpc)
    {
        while ( **ptpc >= _T('0') && **ptpc <= _T('9') )
            CopyFmtChar(*(*ptpc)++);
    }

    // the translated format
    wxString m_fmt;

    // the original format
    const wxChar *m_fmtOrig;

    // the number of characters already copied
    size_t m_nCopied;
};

wxFormatConverter::wxFormatConverter(const wxChar *format)
{
    m_fmtOrig = format;
    m_nCopied = 0;

    while ( *format )
    {
        if ( CopyFmtChar(*format++) == _T('%') )
        {
            // skip any flags
            while ( IsFlagChar(*format) )
                CopyFmtChar(*format++);

            // and possible width
            if ( *format == _T('*') )
                CopyFmtChar(*format++);
            else
                SkipDigits(&format);

            // precision?
            if ( *format == _T('.') )
            {
                CopyFmtChar(*format++);
                if ( *format == _T('*') )
                    CopyFmtChar(*format++);
                else
                    SkipDigits(&format);
            }

            // next we can have a size modifier
            enum
            {
                Default,
                Short,
                Long
            } size;

            switch ( *format )
            {
                case _T('h'):
                    size = Short;
                    format++;
                    break;

                case _T('l'):
                    // "ll" has a different meaning!
                    if ( format[1] != _T('l') )
                    {
                        size = Long;
                        format++;
                        break;
                    }
                    //else: fall through

                default:
                    size = Default;
            }

            // and finally we should have the type
            switch ( *format )
            {
                case _T('C'):
                case _T('S'):
                    // %C and %hC -> %c and %lC -> %lc
                    if ( size == Long )
                        CopyFmtChar(_T('l'));

                    InsertFmtChar(*format++ == _T('C') ? _T('c') : _T('s'));
                    break;

                case _T('c'):
                case _T('s'):
                    // %c -> %lc but %hc stays %hc and %lc is still %lc
                    if ( size == Default)
                        InsertFmtChar(_T('l'));
                    // fall through

                default:
                    // nothing special to do
                    if ( size != Default )
                        CopyFmtChar(*(format - 1));
                    CopyFmtChar(*format++);
            }
        }
    }
}

#else // !wxNEED_PRINTF_CONVERSION
    // no conversion necessary
    #define wxFormatConverter(x) (x)
#endif // wxNEED_PRINTF_CONVERSION/!wxNEED_PRINTF_CONVERSION

#ifdef __WXDEBUG__
// For testing the format converter
wxString wxConvertFormat(const wxChar *format)
{
    return wxString(wxFormatConverter(format));
}
#endif

// ----------------------------------------------------------------------------
// wxPrintf(), wxScanf() and relatives
// ----------------------------------------------------------------------------

#if defined(wxNEED_PRINTF_CONVERSION) || defined(wxNEED_WPRINTF)

int wxCRT_Scanf( const wxChar *format, ... )
{
    va_list argptr;
    va_start(argptr, format);

    int ret = vwscanf(wxFormatConverter(format), argptr );

    va_end(argptr);

    return ret;
}

int wxCRT_Sscanf( const wxChar *str, const wxChar *format, ... )
{
    va_list argptr;
    va_start(argptr, format);

    int ret = vswscanf( str, wxFormatConverter(format), argptr );

    va_end(argptr);

    return ret;
}

int wxCRT_Fscanf( FILE *stream, const wxChar *format, ... )
{
    va_list argptr;
    va_start(argptr, format);
    int ret = vfwscanf(stream, wxFormatConverter(format), argptr);

    va_end(argptr);

    return ret;
}

int wxCRT_Printf( const wxChar *format, ... )
{
    va_list argptr;
    va_start(argptr, format);

    int ret = vwprintf( wxFormatConverter(format), argptr );

    va_end(argptr);

    return ret;
}

int wxCRT_Fprintf( FILE *stream, const wxChar *format, ... )
{
    va_list argptr;
    va_start( argptr, format );

    int ret = vfwprintf( stream, wxFormatConverter(format), argptr );

    va_end(argptr);

    return ret;
}

int wxCRT_Vsscanf( const wxChar *str, const wxChar *format, va_list argptr )
{
    return vswscanf( str, wxFormatConverter(format), argptr );
}

int wxCRT_Vfprintf( FILE *stream, const wxChar *format, va_list argptr )
{
    return vfwprintf( stream, wxFormatConverter(format), argptr );
}

int wxCRT_Vprintf( const wxChar *format, va_list argptr )
{
    return vwprintf( wxFormatConverter(format), argptr );
}

#ifndef wxCRT_Vsnprintf
int wxCRT_Vsnprintf( wxChar *str, size_t size, const wxChar *format, va_list argptr )
{
    return vswprintf( str, size, wxFormatConverter(format), argptr );
}
#endif // wxCRT_Vsnprintf

int wxCRT_Vsprintf( wxChar *str, const wxChar *format, va_list argptr )
{
    // same as for wxSprintf()
    return vswprintf(str, INT_MAX / 4, wxFormatConverter(format), argptr);
}

#endif // wxNEED_PRINTF_CONVERSION


// ----------------------------------------------------------------------------
// wrappers to printf and scanf function families
// ----------------------------------------------------------------------------

#if !wxUSE_UTF8_LOCALE_ONLY
int wxDoSprintfWchar(char *str, const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsprintf(str, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
int wxDoSprintfUtf8(char *str, const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsprintf(str, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // wxUSE_UNICODE_UTF8

#if wxUSE_UNICODE

#if !wxUSE_UTF8_LOCALE_ONLY
int wxDoSprintfWchar(wchar_t *str, const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsprintf(str, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
int wxDoSprintfUtf8(wchar_t *str, const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsprintf(str, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // wxUSE_UNICODE_UTF8

#endif // wxUSE_UNICODE

#if !wxUSE_UTF8_LOCALE_ONLY
int wxDoSnprintfWchar(char *str, size_t size, const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsnprintf(str, size, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
int wxDoSnprintfUtf8(char *str, size_t size, const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsnprintf(str, size, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // wxUSE_UNICODE_UTF8

#if wxUSE_UNICODE

#if !wxUSE_UTF8_LOCALE_ONLY
int wxDoSnprintfWchar(wchar_t *str, size_t size, const wxChar *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsnprintf(str, size, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
int wxDoSnprintfUtf8(wchar_t *str, size_t size, const char *format, ...)
{
    va_list argptr;
    va_start(argptr, format);

    int rv = wxVsnprintf(str, size, format, argptr);

    va_end(argptr);
    return rv;
}
#endif // wxUSE_UNICODE_UTF8

#endif // wxUSE_UNICODE


#ifdef HAVE_BROKEN_VSNPRINTF_DECL
    #define vsnprintf wx_fixed_vsnprintf
#endif

#if wxUSE_UNICODE

#if !wxUSE_UTF8_LOCALE_ONLY
static int ConvertStringToBuf(const wxString& s, char *out, size_t outsize)
{
    const wxWX2WCbuf buf = s.wc_str();

    size_t len = wxConvLibc.FromWChar(out, outsize, buf);
    if ( len != wxCONV_FAILED )
        return len-1;
    else
        return wxConvLibc.FromWChar(NULL, 0, buf);
}
#endif // !wxUSE_UTF8_LOCALE_ONLY

#if wxUSE_UNICODE_UTF8
static int ConvertStringToBuf(const wxString& s, wchar_t *out, size_t outsize)
{
    const wxWX2WCbuf buf(s.wc_str());
    size_t len = wxWcslen(buf);
    if ( outsize > len )
        memcpy(out, buf, (len+1) * sizeof(wchar_t));
    // else: not enough space
    return len;
}
#endif // wxUSE_UNICODE_UTF8

template<typename T>
static size_t PrintfViaString(T *out, size_t outsize,
                              const wxString& format, va_list argptr)
{
    va_list argcopy;
    wxVaCopy(argcopy, argptr);

    wxString s;
    s.PrintfV(format, argcopy);

    return ConvertStringToBuf(s, out, outsize);
}
#endif // wxUSE_UNICODE

int wxVsprintf(char *str, const wxString& format, va_list argptr)
{
    va_list argcopy;
    wxVaCopy(argcopy, argptr);

#if wxUSE_UTF8_LOCALE_ONLY
    return vsprintf(str, format.wx_str(), argcopy);
#else
    #if wxUSE_UNICODE_UTF8
    if ( wxLocaleIsUtf8 )
        return vsprintf(str, format.wx_str(), argcopy);
    else
    #endif
    #if wxUSE_UNICODE
    return PrintfViaString(str, wxNO_LEN, format, argcopy);
    #else
    return wxCRT_Vsprintf(str, format, argcopy);
    #endif
#endif
}

#if wxUSE_UNICODE
int wxVsprintf(wchar_t *str, const wxString& format, va_list argptr)
{
    va_list argcopy;
    wxVaCopy(argcopy, argptr);

#if wxUSE_UNICODE_WCHAR
    return wxCRT_Vsprintf(str, format, argcopy);
#else // wxUSE_UNICODE_UTF8
    #if !wxUSE_UTF8_LOCALE_ONLY
    if ( !wxLocaleIsUtf8 )
        return wxCRT_Vsprintf(str, format, argcopy);
    else
    #endif
        return PrintfViaString(str, wxNO_LEN, format, argcopy);
#endif // wxUSE_UNICODE_UTF8
}
#endif // wxUSE_UNICODE

int wxVsnprintf(char *str, size_t size, const wxString& format, va_list argptr)
{
    int rv;
    va_list argcopy;
    wxVaCopy(argcopy, argptr);

#if wxUSE_UTF8_LOCALE_ONLY
    rv = vsnprintf(str, size, format.wx_str(), argcopy);
#else
    #if wxUSE_UNICODE_UTF8
    if ( wxLocaleIsUtf8 )
        rv = vsnprintf(str, size, format.wx_str(), argcopy);
    else
    #endif
    #if wxUSE_UNICODE
    {
        // NB: if this code is called, then wxString::PrintV() would use the
        //     wchar_t* version of wxVsnprintf(), so it's safe to use PrintV()
        //     from here
        rv = PrintfViaString(str, size, format, argcopy);
    }
    #else
    rv = wxCRT_Vsnprintf(str, size, format, argcopy);
    #endif
#endif

    // VsnprintfTestCase reveals that glibc's implementation of vswprintf
    // doesn't nul terminate on truncation.
    str[size - 1] = 0;

    return rv;
}

#if wxUSE_UNICODE
int wxVsnprintf(wchar_t *str, size_t size, const wxString& format, va_list argptr)
{
    int rv;
    va_list argcopy;
    wxVaCopy(argcopy, argptr);

#if wxUSE_UNICODE_WCHAR
    rv = wxCRT_Vsnprintf(str, size, format, argcopy);
#else // wxUSE_UNICODE_UTF8
    #if !wxUSE_UTF8_LOCALE_ONLY
    if ( !wxLocaleIsUtf8 )
        rv = wxCRT_Vsnprintf(str, size, format, argcopy);
    else
    #endif
    {
        // NB: if this code is called, then wxString::PrintV() would use the
        //     char* version of wxVsnprintf(), so it's safe to use PrintV()
        //     from here
        rv = PrintfViaString(str, size, format, argcopy);
    }
#endif // wxUSE_UNICODE_UTF8

    // VsnprintfTestCase reveals that glibc's implementation of vswprintf
    // doesn't nul terminate on truncation.
    str[size - 1] = 0;

    return rv;
}
#endif // wxUSE_UNICODE

#if wxUSE_WCHAR_T

// ----------------------------------------------------------------------------
// ctype.h stuff (currently unused)
// ----------------------------------------------------------------------------

#if defined(__WIN32__) && defined(wxNEED_WX_CTYPE_H)
inline WORD wxMSW_ctype(wxChar ch)
{
  WORD ret;
  GetStringTypeEx(LOCALE_USER_DEFAULT, CT_CTYPE1, &ch, 1, &ret);
  return ret;
}

WXDLLEXPORT int wxIsalnum(wxChar ch) { return IsCharAlphaNumeric(ch); }
WXDLLEXPORT int wxIsalpha(wxChar ch) { return IsCharAlpha(ch); }
WXDLLEXPORT int wxIscntrl(wxChar ch) { return wxMSW_ctype(ch) & C1_CNTRL; }
WXDLLEXPORT int wxIsdigit(wxChar ch) { return wxMSW_ctype(ch) & C1_DIGIT; }
WXDLLEXPORT int wxIsgraph(wxChar ch) { return wxMSW_ctype(ch) & (C1_DIGIT|C1_PUNCT|C1_ALPHA); }
WXDLLEXPORT int wxIslower(wxChar ch) { return IsCharLower(ch); }
WXDLLEXPORT int wxIsprint(wxChar ch) { return wxMSW_ctype(ch) & (C1_DIGIT|C1_SPACE|C1_PUNCT|C1_ALPHA); }
WXDLLEXPORT int wxIspunct(wxChar ch) { return wxMSW_ctype(ch) & C1_PUNCT; }
WXDLLEXPORT int wxIsspace(wxChar ch) { return wxMSW_ctype(ch) & C1_SPACE; }
WXDLLEXPORT int wxIsupper(wxChar ch) { return IsCharUpper(ch); }
WXDLLEXPORT int wxIsxdigit(wxChar ch) { return wxMSW_ctype(ch) & C1_XDIGIT; }
WXDLLEXPORT int wxTolower(wxChar ch) { return (wxChar)CharLower((LPTSTR)(ch)); }
WXDLLEXPORT int wxToupper(wxChar ch) { return (wxChar)CharUpper((LPTSTR)(ch)); }
#endif

#ifdef wxNEED_WX_MBSTOWCS

WXDLLEXPORT size_t wxMbstowcs (wchar_t * out, const char * in, size_t outlen)
{
    if (!out)
    {
        size_t outsize = 0;
        while(*in++)
            outsize++;
        return outsize;
    }

    const char* origin = in;

    while (outlen-- && *in)
    {
        *out++ = (wchar_t) *in++;
    }

    *out = '\0';

    return in - origin;
}

WXDLLEXPORT size_t wxWcstombs (char * out, const wchar_t * in, size_t outlen)
{
    if (!out)
    {
        size_t outsize = 0;
        while(*in++)
            outsize++;
        return outsize;
    }

    const wchar_t* origin = in;

    while (outlen-- && *in)
    {
        *out++ = (char) *in++;
    }

    *out = '\0';

    return in - origin;
}

#endif // wxNEED_WX_MBSTOWCS

#if defined(wxNEED_WX_CTYPE_H)

#include <CoreFoundation/CoreFoundation.h>

#define cfalnumset CFCharacterSetGetPredefined(kCFCharacterSetAlphaNumeric)
#define cfalphaset CFCharacterSetGetPredefined(kCFCharacterSetLetter)
#define cfcntrlset CFCharacterSetGetPredefined(kCFCharacterSetControl)
#define cfdigitset CFCharacterSetGetPredefined(kCFCharacterSetDecimalDigit)
//CFCharacterSetRef cfgraphset = kCFCharacterSetControl && !' '
#define cflowerset CFCharacterSetGetPredefined(kCFCharacterSetLowercaseLetter)
//CFCharacterSetRef cfprintset = !kCFCharacterSetControl
#define cfpunctset CFCharacterSetGetPredefined(kCFCharacterSetPunctuation)
#define cfspaceset CFCharacterSetGetPredefined(kCFCharacterSetWhitespaceAndNewline)
#define cfupperset CFCharacterSetGetPredefined(kCFCharacterSetUppercaseLetter)

WXDLLEXPORT int wxIsalnum(wxChar ch) { return CFCharacterSetIsCharacterMember(cfalnumset, ch); }
WXDLLEXPORT int wxIsalpha(wxChar ch) { return CFCharacterSetIsCharacterMember(cfalphaset, ch); }
WXDLLEXPORT int wxIscntrl(wxChar ch) { return CFCharacterSetIsCharacterMember(cfcntrlset, ch); }
WXDLLEXPORT int wxIsdigit(wxChar ch) { return CFCharacterSetIsCharacterMember(cfdigitset, ch); }
WXDLLEXPORT int wxIsgraph(wxChar ch) { return !CFCharacterSetIsCharacterMember(cfcntrlset, ch) && ch != ' '; }
WXDLLEXPORT int wxIslower(wxChar ch) { return CFCharacterSetIsCharacterMember(cflowerset, ch); }
WXDLLEXPORT int wxIsprint(wxChar ch) { return !CFCharacterSetIsCharacterMember(cfcntrlset, ch); }
WXDLLEXPORT int wxIspunct(wxChar ch) { return CFCharacterSetIsCharacterMember(cfpunctset, ch); }
WXDLLEXPORT int wxIsspace(wxChar ch) { return CFCharacterSetIsCharacterMember(cfspaceset, ch); }
WXDLLEXPORT int wxIsupper(wxChar ch) { return CFCharacterSetIsCharacterMember(cfupperset, ch); }
WXDLLEXPORT int wxIsxdigit(wxChar ch) { return wxIsdigit(ch) || (ch>='a' && ch<='f') || (ch>='A' && ch<='F'); }
WXDLLEXPORT int wxTolower(wxChar ch) { return (wxChar)tolower((char)(ch)); }
WXDLLEXPORT int wxToupper(wxChar ch) { return (wxChar)toupper((char)(ch)); }

#endif  // wxNEED_WX_CTYPE_H

#ifndef wxStrdupA

WXDLLEXPORT char *wxStrdupA(const char *s)
{
    return strcpy((char *)malloc(strlen(s) + 1), s);
}

#endif // wxStrdupA

#ifndef wxStrdupW

WXDLLEXPORT wchar_t * wxStrdupW(const wchar_t *pwz)
{
  size_t size = (wxWcslen(pwz) + 1) * sizeof(wchar_t);
  wchar_t *ret = (wchar_t *) malloc(size);
  memcpy(ret, pwz, size);
  return ret;
}

#endif // wxStrdupW

#ifndef wxStricmp
int WXDLLEXPORT wxStricmp(const wxChar *psz1, const wxChar *psz2)
{
  register wxChar c1, c2;
  do {
    c1 = wxTolower(*psz1++);
    c2 = wxTolower(*psz2++);
  } while ( c1 && (c1 == c2) );
  return c1 - c2;
}
#endif

#ifndef wxStricmp
int WXDLLEXPORT wxStrnicmp(const wxChar *s1, const wxChar *s2, size_t n)
{
  // initialize the variables just to suppress stupid gcc warning
  register wxChar c1 = 0, c2 = 0;
  while (n && ((c1 = wxTolower(*s1)) == (c2 = wxTolower(*s2)) ) && c1) n--, s1++, s2++;
  if (n) {
    if (c1 < c2) return -1;
    if (c1 > c2) return 1;
  }
  return 0;
}
#endif

#ifndef wxSetlocale_
wxWCharBuffer wxSetlocale_(int category, const wxChar *locale)
{
    char *localeOld = setlocale(category, wxConvLibc.cWX2MB(locale));

    return wxWCharBuffer(wxConvLibc.cMB2WC(localeOld));
}

wxWCharBuffer wxSetlocale(int category, const wxChar *locale)
{
    wxWCharBuffer rv = wxSetlocale_(category, locale);
    if ( rv )
        wxUpdateLocaleIsUtf8();
    return rv;
}
#else // defined(wxSetlocale_)
wxChar *wxSetlocale(int category, const wxChar *locale)
{
    wxChar *rv = wxSetlocale_(category, locale);
    if ( rv )
        wxUpdateLocaleIsUtf8();
    return rv;
}
#endif // wxSetlocale_ defined or not

#if wxUSE_WCHAR_T && !defined(HAVE_WCSLEN)
WXDLLEXPORT size_t wxWcslen(const wchar_t *s)
{
    size_t n = 0;
    while ( *s++ )
        n++;

    return n;
}
#endif

// ----------------------------------------------------------------------------
// string.h functions
// ----------------------------------------------------------------------------

#ifdef wxNEED_WX_STRING_H

// RN:  These need to be c externed for the regex lib
#ifdef __cplusplus
extern "C" {
#endif

WXDLLEXPORT wxChar * wxStrcat(wxChar *dest, const wxChar *src)
{
  wxChar *ret = dest;
  while (*dest) dest++;
  while ((*dest++ = *src++));
  return ret;
}

WXDLLEXPORT const wxChar * wxStrchr(const wxChar *s, wxChar c)
{
    // be careful here as the terminating NUL makes part of the string
    while ( *s != c )
    {
        if ( !*s++ )
            return NULL;
    }

    return s;
}

WXDLLEXPORT int wxStrcmp(const wxChar *s1, const wxChar *s2)
{
  while ((*s1 == *s2) && *s1) s1++, s2++;
  if ((wxUChar)*s1 < (wxUChar)*s2) return -1;
  if ((wxUChar)*s1 > (wxUChar)*s2) return 1;
  return 0;
}

WXDLLEXPORT wxChar * wxStrcpy(wxChar *dest, const wxChar *src)
{
  wxChar *ret = dest;
  while ((*dest++ = *src++));
  return ret;
}

WXDLLEXPORT size_t wxStrlen_(const wxChar *s)
{
    size_t n = 0;
    while ( *s++ )
        n++;

    return n;
}


WXDLLEXPORT wxChar * wxStrncat(wxChar *dest, const wxChar *src, size_t n)
{
  wxChar *ret = dest;
  while (*dest) dest++;
  while (n && (*dest++ = *src++)) n--;
  return ret;
}

WXDLLEXPORT int wxStrncmp(const wxChar *s1, const wxChar *s2, size_t n)
{
  while (n && (*s1 == *s2) && *s1) n--, s1++, s2++;
  if (n) {
    if ((wxUChar)*s1 < (wxUChar)*s2) return -1;
    if ((wxUChar)*s1 > (wxUChar)*s2) return 1;
  }
  return 0;
}

WXDLLEXPORT wxChar * wxStrncpy(wxChar *dest, const wxChar *src, size_t n)
{
  wxChar *ret = dest;
  while (n && (*dest++ = *src++)) n--;
  while (n) *dest++=0, n--; // the docs specify padding with zeroes
  return ret;
}

WXDLLEXPORT const wxChar * wxStrpbrk(const wxChar *s, const wxChar *accept)
{
  while (*s && !wxStrchr(accept, *s))
      s++;

  return *s ? s : NULL;
}

WXDLLEXPORT const wxChar * wxStrrchr(const wxChar *s, wxChar c)
{
    const wxChar *ret = NULL;
    do
    {
        if ( *s == c )
            ret = s;
        s++;
    }
    while ( *s );

    return ret;
}

WXDLLEXPORT size_t wxStrspn(const wxChar *s, const wxChar *accept)
{
  size_t len = 0;
  while (wxStrchr(accept, *s++)) len++;
  return len;
}

WXDLLEXPORT const wxChar *wxStrstr(const wxChar *haystack, const wxChar *needle)
{
    wxASSERT_MSG( needle != NULL, _T("NULL argument in wxStrstr") );

    // VZ: this is not exactly the most efficient string search algorithm...

    const size_t len = wxStrlen(needle);

    while ( const wxChar *fnd = wxStrchr(haystack, *needle) )
    {
        if ( !wxStrncmp(fnd, needle, len) )
            return fnd;

        haystack = fnd + 1;
    }

    return NULL;
}

#ifdef __cplusplus
}
#endif

WXDLLEXPORT double wxStrtod(const wxChar *nptr, wxChar **endptr)
{
  const wxChar *start = nptr;

  // FIXME: only correct for C locale
  while (wxIsspace(*nptr)) nptr++;
  if (*nptr == wxT('+') || *nptr == wxT('-')) nptr++;
  while (wxIsdigit(*nptr)) nptr++;
  if (*nptr == wxT('.')) {
    nptr++;
    while (wxIsdigit(*nptr)) nptr++;
  }
  if (*nptr == wxT('E') || *nptr == wxT('e')) {
    nptr++;
    if (*nptr == wxT('+') || *nptr == wxT('-')) nptr++;
    while (wxIsdigit(*nptr)) nptr++;
  }

  wxString data(nptr, nptr-start);
  wxWX2MBbuf dat = data.mb_str(wxConvLibc);
  char *rdat = wxMBSTRINGCAST dat;
  double ret = strtod(dat, &rdat);

  if (endptr) *endptr = (wxChar *)(start + (rdat - (const char *)dat));

  return ret;
}

WXDLLEXPORT long int wxStrtol(const wxChar *nptr, wxChar **endptr, int base)
{
  const wxChar *start = nptr;

  // FIXME: only correct for C locale
  while (wxIsspace(*nptr)) nptr++;
  if (*nptr == wxT('+') || *nptr == wxT('-')) nptr++;
  if (((base == 0) || (base == 16)) &&
      (nptr[0] == wxT('0') && nptr[1] == wxT('x'))) {
    nptr += 2;
    base = 16;
  }
  else if ((base == 0) && (nptr[0] == wxT('0'))) base = 8;
  else if (base == 0) base = 10;

  while ((wxIsdigit(*nptr) && (*nptr - wxT('0') < base)) ||
         (wxIsalpha(*nptr) && (wxToupper(*nptr) - wxT('A') + 10 < base))) nptr++;

  wxString data(start, nptr-start);
  wxWX2MBbuf dat = data.mb_str(wxConvLibc);
  char *rdat = wxMBSTRINGCAST dat;
  long int ret = strtol(dat, &rdat, base);

  if (endptr) *endptr = (wxChar *)(start + (rdat - (const char *)dat));

  return ret;
}

WXDLLEXPORT unsigned long int wxStrtoul(const wxChar *nptr, wxChar **endptr, int base)
{
    return (unsigned long int) wxStrtol(nptr, endptr, base);
}

#endif // wxNEED_WX_STRING_H

#ifdef wxNEED_WX_STDIO_H
WXDLLEXPORT FILE * wxFopen(const wxChar *path, const wxChar *mode)
{
    char mode_buffer[10];
    for (size_t i = 0; i < wxStrlen(mode)+1; i++)
       mode_buffer[i] = (char) mode[i];

    return fopen( wxConvFile.cWX2MB(path), mode_buffer );
}

WXDLLEXPORT FILE * wxFreopen(const wxChar *path, const wxChar *mode, FILE *stream)
{
    char mode_buffer[10];
    for (size_t i = 0; i < wxStrlen(mode)+1; i++)
       mode_buffer[i] = (char) mode[i];

    return freopen( wxConvFile.cWX2MB(path), mode_buffer, stream );
}

WXDLLEXPORT int wxRemove(const wxChar *path)
{
    return remove( wxConvFile.cWX2MB(path) );
}

WXDLLEXPORT int wxRename(const wxChar *oldpath, const wxChar *newpath)
{
    return rename( wxConvFile.cWX2MB(oldpath), wxConvFile.cWX2MB(newpath) );
}
#endif

#ifndef wxAtof
double   WXDLLEXPORT wxAtof(const wxChar *psz)
{
#ifdef __WXWINCE__
    double d;
    wxString str(psz);
    if (str.ToDouble(& d))
        return d;

    return 0.0;
#else
    return atof(wxConvLibc.cWX2MB(psz));
#endif
}
#endif

#ifdef wxNEED_WX_STDLIB_H
int      WXDLLEXPORT wxAtoi(const wxChar *psz)
{
  return atoi(wxConvLibc.cWX2MB(psz));
}

long     WXDLLEXPORT wxAtol(const wxChar *psz)
{
  return atol(wxConvLibc.cWX2MB(psz));
}

wxChar * WXDLLEXPORT wxGetenv(const wxChar *name)
{
#if wxUSE_UNICODE
    // NB: buffer returned by getenv() is allowed to be overwritten next
    //     time getenv() is called, so it is OK to use static string
    //     buffer to hold the data.
    static wxWCharBuffer value((wxChar*)NULL);
    value = wxConvLibc.cMB2WX(getenv(wxConvLibc.cWX2MB(name)));
    return value.data();
#else
    return getenv(name);
#endif
}

int WXDLLEXPORT wxSystem(const wxChar *psz)
{
    return system(wxConvLibc.cWX2MB(psz));
}

#endif // wxNEED_WX_STDLIB_H

#ifdef wxNEED_WX_TIME_H
WXDLLEXPORT size_t
wxStrftime(wxChar *s, size_t maxsize, const wxChar *fmt, const struct tm *tm)
{
    if ( !maxsize )
        return 0;

    wxCharBuffer buf(maxsize);

    wxCharBuffer bufFmt(wxConvLibc.cWX2MB(fmt));
    if ( !bufFmt )
        return 0;

    size_t ret = strftime(buf.data(), maxsize, bufFmt, tm);
    if  ( !ret )
        return 0;

    wxWCharBuffer wbuf = wxConvLibc.cMB2WX(buf);
    if ( !wbuf )
        return 0;

    wxStrncpy(s, wbuf, maxsize);
    return wxStrlen(s);
}
#endif // wxNEED_WX_TIME_H

#ifndef wxCtime
WXDLLEXPORT wxChar *wxCtime(const time_t *timep)
{
    // normally the string is 26 chars but give one more in case some broken
    // DOS compiler decides to use "\r\n" instead of "\n" at the end
    static wxChar buf[27];

    // ctime() is guaranteed to return a string containing only ASCII
    // characters, as its format is always the same for any locale
    wxStrncpy(buf, wxString::FromAscii(ctime(timep)), WXSIZEOF(buf));
    buf[WXSIZEOF(buf) - 1] = _T('\0');

    return buf;
}
#endif // wxCtime

#endif // wxUSE_WCHAR_T

#ifndef wxStrtoll
static wxULongLong_t wxStrtoullBase(const wxChar* nptr, wxChar** endptr, int base, wxChar* sign)
{
    wxULongLong_t sum = 0;
    wxString wxstr(nptr);
    wxString::const_iterator i = wxstr.begin();
    wxString::const_iterator end = wxstr.end();

    // Skip spaces
    while ( i != end && wxIsspace(*i) ) i++;

    // Starts with sign?
    *sign = wxT(' ');
    if ( i != end )
    {
        wxChar c = *i;
        if ( c == wxT('+') || c == wxT('-') )
        {
            *sign = c;
            i++;
        }
    }

    // Starts with 0x?
    if ( i != end && *i == wxT('0') )
    {
        i++;
        if ( i != end )
        {
            if ( *i == wxT('x') && (base == 16 || base == 0) )
            {
                base = 16;
                i++;
            }
            else
            {
                if ( endptr )
                    *endptr = (wxChar*) nptr;
                wxSET_ERRNO(EINVAL);
                return sum;
            }
        }
        else
            i--;
    }

    if ( base == 0 )
        base = 10;

    for ( ; i != end; i++ )
    {
        unsigned int n;

        wxChar c = *i;
        if ( c >= wxT('0') )
        {
            if ( c <= wxT('9') )
                n = c - wxT('0');
            else
                n = wxTolower(c) - wxT('a') + 10;
        }
        else
            break;

        if ( n >= (unsigned int)base )
            // Invalid character (for this base)
            break;

        wxULongLong_t prevsum = sum;
        sum = (sum * base) + n;

        if ( sum < prevsum )
        {
            wxSET_ERRNO(ERANGE);
            break;
        }
    }

    if ( endptr )
    {
        const wxChar& endref = *i;
        *endptr = &(wxChar&)endref;
    }

    return sum;
}

wxULongLong_t wxStrtoull(const wxChar* nptr, wxChar** endptr, int base)
{
    wxChar sign;
    wxULongLong_t uval = wxStrtoullBase(nptr, endptr, base, &sign);

    if ( sign == wxT('-') )
    {
        wxSET_ERRNO(ERANGE);
        uval = 0;
    }

    return uval;
}

wxLongLong_t wxStrtoll(const wxChar* nptr, wxChar** endptr, int base)
{
    wxChar sign;
    wxULongLong_t uval = wxStrtoullBase(nptr, endptr, base, &sign);
    wxLongLong_t val = 0;

    if ( sign == wxT('-') )
    {
        if ( uval <= wxULL(wxINT64_MAX+1) )
        {
            if ( uval == wxULL(wxINT64_MAX+1))
                val = -((wxLongLong_t)wxINT64_MAX) - 1;
            else
                val = -((wxLongLong_t)uval);
        }
        else
        {
            wxSET_ERRNO(ERANGE);
        }
    }
    else if ( uval <= wxINT64_MAX )
    {
        val = uval;
    }
    else
    {
        wxSET_ERRNO(ERANGE);
    }

    return val;
}
#endif // wxStrtoll

// ----------------------------------------------------------------------------
// functions which we may need even if !wxUSE_WCHAR_T
// ----------------------------------------------------------------------------

#ifndef wxStrtok

WXDLLEXPORT wxChar * wxStrtok(wxChar *psz, const wxChar *delim, wxChar **save_ptr)
{
    if (!psz)
    {
        psz = *save_ptr;
        if ( !psz )
            return NULL;
    }

    psz += wxStrspn(psz, delim);
    if (!*psz)
    {
        *save_ptr = (wxChar *)NULL;
        return (wxChar *)NULL;
    }

    wxChar *ret = psz;
    psz = wxStrpbrk(psz, delim);
    if (!psz)
    {
        *save_ptr = (wxChar*)NULL;
    }
    else
    {
        *psz = wxT('\0');
        *save_ptr = psz + 1;
    }

    return ret;
}

#endif // wxStrtok

// ----------------------------------------------------------------------------
// missing C RTL functions
// ----------------------------------------------------------------------------

#ifdef wxNEED_STRDUP

char *strdup(const char *s)
{
    char *dest = (char*) malloc( strlen( s ) + 1 ) ;
    if ( dest )
        strcpy( dest , s ) ;
    return dest ;
}
#endif // wxNEED_STRDUP

#if defined(__WXWINCE__) && (_WIN32_WCE <= 211)

void *calloc( size_t num, size_t size )
{
    void** ptr = (void **)malloc(num * size);
    memset( ptr, 0, num * size);
    return ptr;
}

#endif // __WXWINCE__ <= 211

#ifdef __WXWINCE__

int wxRemove(const wxChar *path)
{
    return ::DeleteFile(path) == 0;
}

#endif


// ----------------------------------------------------------------------------
// wxLocaleIsUtf8
// ----------------------------------------------------------------------------

#if wxUSE_UNICODE_UTF8

#if !wxUSE_UTF8_LOCALE_ONLY
bool wxLocaleIsUtf8 = false; // the safer setting if not known
#endif

static bool wxIsLocaleUtf8()
{
    // NB: we intentionally don't use wxLocale::GetSystemEncodingName(),
    //     because a) it may be unavailable in some builds and b) has slightly
    //     different semantics (default locale instead of current)

#if defined(HAVE_LANGINFO_H) && defined(CODESET)
    // GNU libc provides current character set this way (this conforms to
    // Unix98)
    const char *charset = nl_langinfo(CODESET);
    if ( charset )
    {
        // "UTF-8" is used by modern glibc versions, but test other variants
        // as well, just in case:
        if ( strcmp(charset, "UTF-8") == 0 ||
             strcmp(charset, "utf-8") == 0 ||
             strcmp(charset, "UTF8") == 0 ||
             strcmp(charset, "utf8") == 0 )
        {
            return true;
        }
    }
#endif

    // check if we're running under the "C" locale: it is 7bit subset
    // of UTF-8, so it can be safely used with the UTF-8 build:
    const char *lc_ctype = setlocale(LC_CTYPE, NULL);
    if ( lc_ctype &&
         (strcmp(lc_ctype, "C") == 0 || strcmp(lc_ctype, "POSIX") == 0) )
    {
        return true;
    }

    // we don't know what charset libc is using, so assume the worst
    // to be safe:
    return false;
}

void wxUpdateLocaleIsUtf8()
{
#if wxUSE_UTF8_LOCALE_ONLY
    if ( !wxIsLocaleUtf8() )
    {
        wxLogFatalError(_T("This program requires UTF-8 locale to run."));
    }
#else // !wxUSE_UTF8_LOCALE_ONLY
    wxLocaleIsUtf8 = wxIsLocaleUtf8();
#endif
}

#endif // wxUSE_UTF8_LOCALE_ONLY
