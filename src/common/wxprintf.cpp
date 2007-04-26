/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/wxprintf.cpp
// Purpose:     wxWidgets wxPrintf() implementation
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

#include <string.h>

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/hash.h"
    #include "wx/utils.h"     // for wxMin and wxMax
    #include "wx/log.h"
#endif

#if defined(__MWERKS__) && __MSL__ >= 0x6000
namespace std {}
using namespace std ;
#endif


// ============================================================================
// printf() implementation
// ============================================================================

// special test mode: define all functions below even if we don't really need
// them to be able to test them
#ifdef wxTEST_PRINTF
    #undef wxVsnprintf_
#endif

// ----------------------------------------------------------------------------
// implement [v]snprintf() if the system doesn't provide a safe one
// or if the system's one does not support positional parameters
// (very useful for i18n purposes)
// ----------------------------------------------------------------------------

#if !defined(wxVsnprintf_)

#if !wxUSE_WXVSNPRINTF
    #error wxUSE_WXVSNPRINTF must be 1 if our wxVsnprintf_ is used
#endif

// wxUSE_STRUTILS says our wxVsnprintf_ implementation to use or not to
// use wxStrlen and wxStrncpy functions over one-char processing loops.
//
// Some benchmarking revealed that wxUSE_STRUTILS == 1 has the following
// effects:
// -> on Windows:
//     when in ANSI mode, this setting does not change almost anything
//     when in Unicode mode, it gives ~ 50% of slowdown !
// -> on Linux:
//     both in ANSI and Unicode mode it gives ~ 60% of speedup !
//
#if defined(WIN32) && wxUSE_UNICODE
#define wxUSE_STRUTILS      0
#else
#define wxUSE_STRUTILS      1
#endif

// some limits of our implementation
#define wxMAX_SVNPRINTF_ARGUMENTS         64
#define wxMAX_SVNPRINTF_FLAGBUFFER_LEN    32
#define wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN   512

// prefer snprintf over sprintf
#if defined(__VISUALC__) || \
        (defined(__BORLANDC__) && __BORLANDC__ >= 0x540)
    #define system_sprintf(buff, max, flags, data)      \
        ::_snprintf(buff, max, flags, data)
#elif defined(HAVE_SNPRINTF)
    #define system_sprintf(buff, max, flags, data)      \
        ::snprintf(buff, max, flags, data)
#else       // NB: at least sprintf() should always be available
    // since 'max' is not used in this case, wxVsnprintf() should always
    // ensure that 'buff' is big enough for all common needs
    // (see wxMAX_SVNPRINTF_FLAGBUFFER_LEN and wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN)
    #define system_sprintf(buff, max, flags, data)      \
        ::sprintf(buff, flags, data)

    #define SYSTEM_SPRINTF_IS_UNSAFE
#endif

// the conversion specifiers accepted by wxVsnprintf_
enum wxPrintfArgType {
    wxPAT_INVALID = -1,

    wxPAT_INT,          // %d, %i, %o, %u, %x, %X
    wxPAT_LONGINT,      // %ld, etc
#ifdef wxLongLong_t
    wxPAT_LONGLONGINT,  // %Ld, etc
#endif
    wxPAT_SIZET,        // %Zd, etc

    wxPAT_DOUBLE,       // %e, %E, %f, %g, %G
    wxPAT_LONGDOUBLE,   // %le, etc

    wxPAT_POINTER,      // %p

    wxPAT_CHAR,         // %hc  (in ANSI mode: %c, too)
    wxPAT_WCHAR,        // %lc  (in Unicode mode: %c, too)

    wxPAT_PCHAR,        // %s   (related to a char *)
    wxPAT_PWCHAR,       // %s   (related to a wchar_t *)

    wxPAT_NINT,         // %n
    wxPAT_NSHORTINT,    // %hn
    wxPAT_NLONGINT      // %ln
};

// an argument passed to wxVsnprintf_
typedef union {
    int pad_int;                        //  %d, %i, %o, %u, %x, %X
    long int pad_longint;               // %ld, etc
#ifdef wxLongLong_t
    wxLongLong_t pad_longlongint;      // %Ld, etc
#endif
    size_t pad_sizet;                   // %Zd, etc

    double pad_double;                  // %e, %E, %f, %g, %G
    long double pad_longdouble;         // %le, etc

    void *pad_pointer;                  // %p

    char pad_char;                      // %hc  (in ANSI mode: %c, too)
    wchar_t pad_wchar;                  // %lc  (in Unicode mode: %c, too)

    void *pad_str;                      // %s

    int *pad_nint;                      // %n
    short int *pad_nshortint;           // %hn
    long int *pad_nlongint;             // %ln
} wxPrintfArg;


// Contains parsed data relative to a conversion specifier given to
// wxVsnprintf_ and parsed from the format string
// NOTE: in C++ there is almost no difference between struct & classes thus
//       there is no performance gain by using a struct here...
class wxPrintfConvSpec
{
public:

    // the position of the argument relative to this conversion specifier
    size_t m_pos;

    // the type of this conversion specifier
    wxPrintfArgType m_type;

    // the minimum and maximum width
    // when one of this var is set to -1 it means: use the following argument
    // in the stack as minimum/maximum width for this conversion specifier
    int m_nMinWidth, m_nMaxWidth;

    // does the argument need to the be aligned to left ?
    bool m_bAlignLeft;

    // pointer to the '%' of this conversion specifier in the format string
    // NOTE: this points somewhere in the string given to the Parse() function -
    //       it's task of the caller ensure that memory is still valid !
    const wxChar *m_pArgPos;

    // pointer to the last character of this conversion specifier in the
    // format string
    // NOTE: this points somewhere in the string given to the Parse() function -
    //       it's task of the caller ensure that memory is still valid !
    const wxChar *m_pArgEnd;

    // a little buffer where formatting flags like #+\.hlqLZ are stored by Parse()
    // for use in Process()
    // NB: even if this buffer is used only for numeric conversion specifiers and
    //     thus could be safely declared as a char[] buffer, we want it to be wxChar
    //     so that in Unicode builds we can avoid to convert its contents to Unicode
    //     chars when copying it in user's buffer.
    char m_szFlags[wxMAX_SVNPRINTF_FLAGBUFFER_LEN];


public:

    // we don't declare this as a constructor otherwise it would be called
    // automatically and we don't want this: to be optimized, wxVsnprintf_
    // calls this function only on really-used instances of this class.
    void Init();

    // Parses the first conversion specifier in the given string, which must
    // begin with a '%'. Returns false if the first '%' does not introduce a
    // (valid) conversion specifier and thus should be ignored.
    bool Parse(const wxChar *format);

    // Process this conversion specifier and puts the result in the given
    // buffer. Returns the number of characters written in 'buf' or -1 if
    // there's not enough space.
    int Process(wxChar *buf, size_t lenMax, wxPrintfArg *p, size_t written);

    // Loads the argument of this conversion specifier from given va_list.
    bool LoadArg(wxPrintfArg *p, va_list &argptr);

private:
    // An helper function of LoadArg() which is used to handle the '*' flag
    void ReplaceAsteriskWith(int w);
};

void wxPrintfConvSpec::Init()
{
    m_nMinWidth = 0;
    m_nMaxWidth = 0xFFFF;
    m_pos = 0;
    m_bAlignLeft = false;
    m_pArgPos = m_pArgEnd = NULL;
    m_type = wxPAT_INVALID;

    // this character will never be removed from m_szFlags array and
    // is important when calling sprintf() in wxPrintfConvSpec::Process() !
    m_szFlags[0] = '%';
}

bool wxPrintfConvSpec::Parse(const wxChar *format)
{
    bool done = false;

    // temporary parse data
    size_t flagofs = 1;
    bool in_prec,       // true if we found the dot in some previous iteration
         prec_dot;      // true if the dot has been already added to m_szFlags
    int ilen = 0;

    m_bAlignLeft = in_prec = prec_dot = false;
    m_pArgPos = m_pArgEnd = format;
    do
    {
#define CHECK_PREC \
        if (in_prec && !prec_dot) \
        { \
            m_szFlags[flagofs++] = '.'; \
            prec_dot = true; \
        }

        // what follows '%'?
        const wxChar ch = *(++m_pArgEnd);
        switch ( ch )
        {
            case wxT('\0'):
                return false;       // not really an argument

            case wxT('%'):
                return false;       // not really an argument

            case wxT('#'):
            case wxT('0'):
            case wxT(' '):
            case wxT('+'):
            case wxT('\''):
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                break;

            case wxT('-'):
                CHECK_PREC
                m_bAlignLeft = true;
                m_szFlags[flagofs++] = char(ch);
                break;

            case wxT('.'):
                CHECK_PREC
                in_prec = true;
                prec_dot = false;
                m_nMaxWidth = 0;
                // dot will be auto-added to m_szFlags if non-negative
                // number follows
                break;

            case wxT('h'):
                ilen = -1;
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                break;

            case wxT('l'):
                // NB: it's safe to use flagofs-1 as flagofs always start from 1
                if (m_szFlags[flagofs-1] == 'l')       // 'll' modifier is the same as 'L' or 'q'
                    ilen = 2;
                else
                ilen = 1;
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                break;

            case wxT('q'):
            case wxT('L'):
                ilen = 2;
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                break;
#ifdef __WXMSW__
            // under Windows we support the special '%I64' notation as longlong
            // integer conversion specifier for MSVC compatibility
            // (it behaves exactly as '%lli' or '%Li' or '%qi')
            case wxT('I'):
                if (*(m_pArgEnd+1) != wxT('6') ||
                    *(m_pArgEnd+2) != wxT('4'))
                    return false;       // bad format

                m_pArgEnd++;
                m_pArgEnd++;

                ilen = 2;
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                m_szFlags[flagofs++] = '6';
                m_szFlags[flagofs++] = '4';
                break;
#endif      // __WXMSW__

            case wxT('Z'):
                ilen = 3;
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                break;

            case wxT('*'):
                if (in_prec)
                {
                    CHECK_PREC

                    // tell Process() to use the next argument
                    // in the stack as maxwidth...
                    m_nMaxWidth = -1;
                }
                else
                {
                    // tell Process() to use the next argument
                    // in the stack as minwidth...
                    m_nMinWidth = -1;
                }

                // save the * in our formatting buffer...
                // will be replaced later by Process()
                m_szFlags[flagofs++] = char(ch);
                break;

            case wxT('1'): case wxT('2'): case wxT('3'):
            case wxT('4'): case wxT('5'): case wxT('6'):
            case wxT('7'): case wxT('8'): case wxT('9'):
                {
                    int len = 0;
                    CHECK_PREC
                    while ( (*m_pArgEnd >= wxT('0')) &&
                            (*m_pArgEnd <= wxT('9')) )
                    {
                        m_szFlags[flagofs++] = char(*m_pArgEnd);
                        len = len*10 + (*m_pArgEnd - wxT('0'));
                        m_pArgEnd++;
                    }

                    if (in_prec)
                        m_nMaxWidth = len;
                    else
                        m_nMinWidth = len;

                    m_pArgEnd--; // the main loop pre-increments n again
                }
                break;

            case wxT('$'):      // a positional parameter (e.g. %2$s) ?
                {
                    if (m_nMinWidth <= 0)
                        break;      // ignore this formatting flag as no
                                    // numbers are preceding it

                    // remove from m_szFlags all digits previously added
                    do {
                        flagofs--;
                    } while (m_szFlags[flagofs] >= '1' &&
                             m_szFlags[flagofs] <= '9');

                    // re-adjust the offset making it point to the
                    // next free char of m_szFlags
                    flagofs++;

                    m_pos = m_nMinWidth;
                    m_nMinWidth = 0;
                }
                break;

            case wxT('d'):
            case wxT('i'):
            case wxT('o'):
            case wxT('u'):
            case wxT('x'):
            case wxT('X'):
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                m_szFlags[flagofs] = '\0';
                if (ilen == 0)
                    m_type = wxPAT_INT;
                else if (ilen == -1)
                    // NB: 'short int' value passed through '...'
                    //      is promoted to 'int', so we have to get
                    //      an int from stack even if we need a short
                    m_type = wxPAT_INT;
                else if (ilen == 1)
                    m_type = wxPAT_LONGINT;
                else if (ilen == 2)
#ifdef wxLongLong_t
                    m_type = wxPAT_LONGLONGINT;
#else // !wxLongLong_t
                    m_type = wxPAT_LONGINT;
#endif // wxLongLong_t/!wxLongLong_t
                else if (ilen == 3)
                    m_type = wxPAT_SIZET;
                done = true;
                break;

            case wxT('e'):
            case wxT('E'):
            case wxT('f'):
            case wxT('g'):
            case wxT('G'):
                CHECK_PREC
                m_szFlags[flagofs++] = char(ch);
                m_szFlags[flagofs] = '\0';
                if (ilen == 2)
                    m_type = wxPAT_LONGDOUBLE;
                else
                    m_type = wxPAT_DOUBLE;
                done = true;
                break;

            case wxT('p'):
                m_type = wxPAT_POINTER;
                m_szFlags[flagofs++] = char(ch);
                m_szFlags[flagofs] = '\0';
                done = true;
                break;

            case wxT('c'):
                if (ilen == -1)
                {
                    // in Unicode mode %hc == ANSI character
                    // and in ANSI mode, %hc == %c == ANSI...
                    m_type = wxPAT_CHAR;
                }
                else if (ilen == 1)
                {
                    // in ANSI mode %lc == Unicode character
                    // and in Unicode mode, %lc == %c == Unicode...
                    m_type = wxPAT_WCHAR;
                }
                else
                {
#if wxUSE_UNICODE
                    // in Unicode mode, %c == Unicode character
                    m_type = wxPAT_WCHAR;
#else
                    // in ANSI mode, %c == ANSI character
                    m_type = wxPAT_CHAR;
#endif
                }
                done = true;
                break;

            case wxT('s'):
                if (ilen == -1)
                {
                    // Unicode mode wx extension: we'll let %hs mean non-Unicode
                    // strings (when in ANSI mode, %s == %hs == ANSI string)
                    m_type = wxPAT_PCHAR;
                }
                else if (ilen == 1)
                {
                    // in Unicode mode, %ls == %s == Unicode string
                    // in ANSI mode, %ls == Unicode string
                    m_type = wxPAT_PWCHAR;
                }
                else
                {
#if wxUSE_UNICODE
                    m_type = wxPAT_PWCHAR;
#else
                    m_type = wxPAT_PCHAR;
#endif
                }
                done = true;
                break;

            case wxT('n'):
                if (ilen == 0)
                    m_type = wxPAT_NINT;
                else if (ilen == -1)
                    m_type = wxPAT_NSHORTINT;
                else if (ilen >= 1)
                    m_type = wxPAT_NLONGINT;
                done = true;
                break;

            default:
                // bad format, don't consider this an argument;
                // leave it unchanged
                return false;
        }

        if (flagofs == wxMAX_SVNPRINTF_FLAGBUFFER_LEN)
        {
            wxLogDebug(wxT("Too many flags specified for a single conversion specifier!"));
            return false;
        }
    }
    while (!done);

    return true;        // parsing was successful
}


void wxPrintfConvSpec::ReplaceAsteriskWith(int width)
{
    char temp[wxMAX_SVNPRINTF_FLAGBUFFER_LEN];

    // find the first * in our flag buffer
    char *pwidth = strchr(m_szFlags, '*');
    wxCHECK_RET(pwidth, _T("field width must be specified"));

    // save what follows the * (the +1 is to skip the asterisk itself!)
    strcpy(temp, pwidth+1);
    if (width < 0)
    {
        pwidth[0] = wxT('-');
        pwidth++;
    }

    // replace * with the actual integer given as width
#ifndef SYSTEM_SPRINTF_IS_UNSAFE
    int maxlen = (m_szFlags + wxMAX_SVNPRINTF_FLAGBUFFER_LEN - pwidth) /
                        sizeof(*m_szFlags);
#endif
    int offset = system_sprintf(pwidth, maxlen, "%d", abs(width));

    // restore after the expanded * what was following it
    strcpy(pwidth+offset, temp);
}

bool wxPrintfConvSpec::LoadArg(wxPrintfArg *p, va_list &argptr)
{
    // did the '*' width/precision specifier was used ?
    if (m_nMaxWidth == -1)
    {
        // take the maxwidth specifier from the stack
        m_nMaxWidth = va_arg(argptr, int);
        if (m_nMaxWidth < 0)
            m_nMaxWidth = 0;
        else
            ReplaceAsteriskWith(m_nMaxWidth);
    }

    if (m_nMinWidth == -1)
    {
        // take the minwidth specifier from the stack
        m_nMinWidth = va_arg(argptr, int);

        ReplaceAsteriskWith(m_nMinWidth);
        if (m_nMinWidth < 0)
        {
            m_bAlignLeft = !m_bAlignLeft;
            m_nMinWidth = -m_nMinWidth;
        }
    }

    switch (m_type) {
        case wxPAT_INT:
            p->pad_int = va_arg(argptr, int);
            break;
        case wxPAT_LONGINT:
            p->pad_longint = va_arg(argptr, long int);
            break;
#ifdef wxLongLong_t
        case wxPAT_LONGLONGINT:
            p->pad_longlongint = va_arg(argptr, wxLongLong_t);
            break;
#endif // wxLongLong_t
        case wxPAT_SIZET:
            p->pad_sizet = va_arg(argptr, size_t);
            break;
        case wxPAT_DOUBLE:
            p->pad_double = va_arg(argptr, double);
            break;
        case wxPAT_LONGDOUBLE:
            p->pad_longdouble = va_arg(argptr, long double);
            break;
        case wxPAT_POINTER:
            p->pad_pointer = va_arg(argptr, void *);
            break;

        case wxPAT_CHAR:
            p->pad_char = (char)va_arg(argptr, int);  // char is promoted to int when passed through '...'
            break;
        case wxPAT_WCHAR:
            p->pad_wchar = (wchar_t)va_arg(argptr, int);  // char is promoted to int when passed through '...'
            break;

        case wxPAT_PCHAR:
        case wxPAT_PWCHAR:
            p->pad_str = va_arg(argptr, void *);
            break;

        case wxPAT_NINT:
            p->pad_nint = va_arg(argptr, int *);
            break;
        case wxPAT_NSHORTINT:
            p->pad_nshortint = va_arg(argptr, short int *);
            break;
        case wxPAT_NLONGINT:
            p->pad_nlongint = va_arg(argptr, long int *);
            break;

        case wxPAT_INVALID:
        default:
            return false;
    }

    return true;    // loading was successful
}

int wxPrintfConvSpec::Process(wxChar *buf, size_t lenMax, wxPrintfArg *p, size_t written)
{
    // buffer to avoid dynamic memory allocation each time for small strings;
    // note that this buffer is used only to hold results of number formatting,
    // %s directly writes user's string in buf, without using szScratch
    char szScratch[wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN];
    size_t lenScratch = 0, lenCur = 0;

#define APPEND_CH(ch) \
                { \
                    if ( lenCur == lenMax ) \
                        return -1; \
                    \
                    buf[lenCur++] = ch; \
                }

    switch ( m_type )
    {
        case wxPAT_INT:
            lenScratch = system_sprintf(szScratch, wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN, m_szFlags, p->pad_int);
            break;

        case wxPAT_LONGINT:
            lenScratch = system_sprintf(szScratch, wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN, m_szFlags, p->pad_longint);
            break;

#ifdef wxLongLong_t
        case wxPAT_LONGLONGINT:
            lenScratch = system_sprintf(szScratch, wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN, m_szFlags, p->pad_longlongint);
            break;
#endif // SIZEOF_LONG_LONG

        case wxPAT_SIZET:
            lenScratch = system_sprintf(szScratch, wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN, m_szFlags, p->pad_sizet);
            break;

        case wxPAT_LONGDOUBLE:
            lenScratch = system_sprintf(szScratch, wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN, m_szFlags, p->pad_longdouble);
            break;

        case wxPAT_DOUBLE:
            lenScratch = system_sprintf(szScratch, wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN, m_szFlags, p->pad_double);
            break;

        case wxPAT_POINTER:
            lenScratch = system_sprintf(szScratch, wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN, m_szFlags, p->pad_pointer);
            break;

        case wxPAT_CHAR:
        case wxPAT_WCHAR:
            {
                wxChar val =
#if wxUSE_UNICODE
                    p->pad_wchar;

                if (m_type == wxPAT_CHAR)
                {
                    // user passed a character explicitely indicated as ANSI...
                    const char buf[2] = { p->pad_char, 0 };
                    val = wxString(buf, wxConvLibc)[0u];

                    //wprintf(L"converting ANSI=>Unicode");   // for debug
                }
#else
                    p->pad_char;

#if wxUSE_WCHAR_T
                if (m_type == wxPAT_WCHAR)
                {
                    // user passed a character explicitely indicated as Unicode...
                    const wchar_t buf[2] = { p->pad_wchar, 0 };
                    val = wxString(buf, wxConvLibc)[0u];

                    //printf("converting Unicode=>ANSI");   // for debug
                }
#endif
#endif

                size_t i;

                if (!m_bAlignLeft)
                    for (i = 1; i < (size_t)m_nMinWidth; i++)
                        APPEND_CH(_T(' '));

                APPEND_CH(val);

                if (m_bAlignLeft)
                    for (i = 1; i < (size_t)m_nMinWidth; i++)
                        APPEND_CH(_T(' '));
            }
            break;

        case wxPAT_PCHAR:
        case wxPAT_PWCHAR:
            {
                wxArgNormalizedString arg(p->pad_str);
                wxString s = arg;

                if ( !arg.IsValid() && m_nMaxWidth >= 6 )
                    s = wxT("(null)");

                // at this point we are sure that m_nMaxWidth is positive or
                // null (see top of wxPrintfConvSpec::LoadArg)
                int len = wxMin((unsigned int)m_nMaxWidth, s.length());

                int i;

                if (!m_bAlignLeft)
                {
                    for (i = len; i < m_nMinWidth; i++)
                        APPEND_CH(_T(' '));
                }

#if wxUSE_STRUTILS
                len = wxMin((unsigned int)len, lenMax-lenCur);
    #if wxUSE_UNICODE // FIXME-UTF8
                wxStrncpy(buf+lenCur, s.wc_str(), len);
    #else
                wxStrncpy(buf+lenCur, s.mb_str(), len);
    #endif
                lenCur += len;
#else
                wxString::const_iterator end = s.begin() + len;
                for (wxString::const_iterator j = s.begin(); j != end; ++j)
                    APPEND_CH(*j);
#endif

                if (m_bAlignLeft)
                {
                    for (i = len; i < m_nMinWidth; i++)
                        APPEND_CH(_T(' '));
                }
            }
            break;

        case wxPAT_NINT:
            *p->pad_nint = written;
            break;

        case wxPAT_NSHORTINT:
            *p->pad_nshortint = (short int)written;
            break;

        case wxPAT_NLONGINT:
            *p->pad_nlongint = written;
            break;

        case wxPAT_INVALID:
        default:
            return -1;
    }

    // if we used system's sprintf() then we now need to append the s_szScratch
    // buffer to the given one...
    switch (m_type)
    {
        case wxPAT_INT:
        case wxPAT_LONGINT:
#ifdef wxLongLong_t
        case wxPAT_LONGLONGINT:
#endif
        case wxPAT_SIZET:
        case wxPAT_LONGDOUBLE:
        case wxPAT_DOUBLE:
        case wxPAT_POINTER:
            wxASSERT(lenScratch < wxMAX_SVNPRINTF_SCRATCHBUFFER_LEN);
#if !wxUSE_UNICODE
            {
                if (lenMax < lenScratch)
                {
                    // fill output buffer and then return -1
                    wxStrncpy(buf, szScratch, lenMax);
                    return -1;
                }
                wxStrncpy(buf, szScratch, lenScratch);
                lenCur += lenScratch;
            }
#else
            {
                // Copy the char scratch to the wide output. This requires
                // conversion, but we can optimise by making use of the fact
                // that we are formatting numbers, this should mean only 7-bit
                // ascii characters are involved.
                wxChar *bufptr = buf;
                const wxChar *bufend = buf + lenMax;
                const char *scratchptr = szScratch;

                // Simply copy each char to a wxChar, stopping on the first
                // null or non-ascii byte. Checking '(signed char)*scratchptr
                // > 0' is an extra optimisation over '*scratchptr != 0 &&
                // isascii(*scratchptr)', though it assumes signed char is
                // 8-bit 2 complement.
                while ((signed char)*scratchptr > 0 && bufptr != bufend)
                    *bufptr++ = *scratchptr++;

                if (bufptr == bufend)
                    return -1;

                lenCur += bufptr - buf;

                // check if the loop stopped on a non-ascii char, if yes then
                // fall back to wxMB2WX
                if (*scratchptr)
                {
                    size_t len = wxMB2WX(bufptr, scratchptr, bufend - bufptr);

                    if (len && len != (size_t)(-1))
                        if (bufptr[len - 1])
                            return -1;
                        else
                            lenCur += len;
                }
            }
#endif
            break;

        default:
            break;      // all other cases were completed previously
    }

    return lenCur;
}

// Copy chars from source to dest converting '%%' to '%'. Takes at most maxIn
// chars from source and write at most outMax chars to dest, returns the
// number of chars actually written. Does not treat null specially.
//
static int wxCopyStrWithPercents(
        size_t maxOut,
        wxChar *dest,
        size_t maxIn,
        const wxChar *source)
{
    size_t written = 0;

    if (maxIn == 0)
        return 0;

    size_t i;
    for ( i = 0; i < maxIn-1 && written < maxOut; source++, i++)
    {
        dest[written++] = *source;
        if (*(source+1) == wxT('%'))
        {
            // skip this additional '%' character
            source++;
            i++;
        }
    }

    if (i < maxIn && written < maxOut)
        // copy last character inconditionally
        dest[written++] = *source;

    return written;
}

int WXDLLEXPORT wxVsnprintf_(wxChar *buf, size_t lenMax,
                             const wxChar *format, va_list argptr)
{
    // useful for debugging, to understand if we are really using this function
    // rather than the system implementation
#if 0
    wprintf(L"Using wxVsnprintf_\n");
#endif

    // required memory:
    wxPrintfConvSpec arg[wxMAX_SVNPRINTF_ARGUMENTS];
    wxPrintfArg argdata[wxMAX_SVNPRINTF_ARGUMENTS];
    wxPrintfConvSpec *pspec[wxMAX_SVNPRINTF_ARGUMENTS] = { NULL };

    size_t i;

    // number of characters in the buffer so far, must be less than lenMax
    size_t lenCur = 0;

    size_t nargs = 0;
    const wxChar *toparse = format;

    // parse the format string
    bool posarg_present = false, nonposarg_present = false;
    for (; *toparse != wxT('\0'); toparse++)
    {
        if (*toparse == wxT('%') )
        {
            arg[nargs].Init();

            // let's see if this is a (valid) conversion specifier...
            if (arg[nargs].Parse(toparse))
            {
                // ...yes it is
                wxPrintfConvSpec *current = &arg[nargs];

                // make toparse point to the end of this specifier
                toparse = current->m_pArgEnd;

                if (current->m_pos > 0)
                {
                    // the positionals start from number 1... adjust the index
                    current->m_pos--;
                    posarg_present = true;
                }
                else
                {
                    // not a positional argument...
                    current->m_pos = nargs;
                    nonposarg_present = true;
                }

                // this conversion specifier is tied to the pos-th argument...
                pspec[current->m_pos] = current;
                nargs++;

                if (nargs == wxMAX_SVNPRINTF_ARGUMENTS)
                {
                    wxLogDebug(wxT("A single call to wxVsnprintf() has more than %d arguments; ")
                               wxT("ignoring all remaining arguments."), wxMAX_SVNPRINTF_ARGUMENTS);
                    break;  // cannot handle any additional conv spec
                }
            }
            else
            {
                // it's safe to look in the next character of toparse as at worst
                // we'll hit its \0
                if (*(toparse+1) == wxT('%'))
                    toparse++;      // the Parse() returned false because we've found a %%
            }
        }
    }

    if (posarg_present && nonposarg_present)
    {
        buf[0] = 0;
        return -1;      // format strings with both positional and
    }                   // non-positional conversion specifier are unsupported !!

    // on platforms where va_list is an array type, it is necessary to make a
    // copy to be able to pass it to LoadArg as a reference.
    bool ok = true;
    va_list ap;
    wxVaCopy(ap, argptr);

    // now load arguments from stack
    for (i=0; i < nargs && ok; i++)
    {
        // !pspec[i] means that the user forgot a positional parameter (e.g. %$1s %$3s);
        // LoadArg == false means that wxPrintfConvSpec::Parse failed to set the
        // conversion specifier 'type' to a valid value...
        ok = pspec[i] && pspec[i]->LoadArg(&argdata[i], ap);
    }

    va_end(ap);

    // something failed while loading arguments from the variable list...
    // (e.g. the user repeated twice the same positional argument)
    if (!ok)
    {
        buf[0] = 0;
        return -1;
    }

    // finally, process each conversion specifier with its own argument
    toparse = format;
    for (i=0; i < nargs; i++)
    {
        // copy in the output buffer the portion of the format string between
        // last specifier and the current one
        size_t tocopy = ( arg[i].m_pArgPos - toparse );

        lenCur += wxCopyStrWithPercents(lenMax - lenCur, buf + lenCur,
                                        tocopy, toparse);
        if (lenCur == lenMax)
        {
            buf[lenMax - 1] = 0;
            return lenMax+1;      // not enough space in the output buffer !
        }

        // process this specifier directly in the output buffer
        int n = arg[i].Process(buf+lenCur, lenMax - lenCur, &argdata[arg[i].m_pos], lenCur);
        if (n == -1)
        {
            buf[lenMax-1] = wxT('\0');  // be sure to always NUL-terminate the string
            return lenMax+1;      // not enough space in the output buffer !
        }
        lenCur += n;

        // the +1 is because wxPrintfConvSpec::m_pArgEnd points to the last character
        // of the format specifier, but we are not interested to it...
        toparse = arg[i].m_pArgEnd + 1;
    }

    // copy portion of the format string after last specifier
    // NOTE: toparse is pointing to the character just after the last processed
    //       conversion specifier
    // NOTE2: the +1 is because we want to copy also the '\0'
    size_t tocopy = wxStrlen(format) + 1  - ( toparse - format ) ;

    lenCur += wxCopyStrWithPercents(lenMax - lenCur, buf + lenCur, 
                                    tocopy, toparse) - 1;
    if (buf[lenCur])
    {
        buf[lenCur] = 0;
        return lenMax+1;     // not enough space in the output buffer !
    }

    // Don't do:
    //      wxASSERT(lenCur == wxStrlen(buf));
    // in fact if we embedded NULLs in the output buffer (using %c with a '\0')
    // such check would fail

    return lenCur;
}

#undef APPEND_CH
#undef CHECK_PREC

#else    // wxVsnprintf_ is defined

#if wxUSE_WXVSNPRINTF
    #error wxUSE_WXVSNPRINTF must be 0 if our wxVsnprintf_ is not used
#endif

#endif // !wxVsnprintf_
