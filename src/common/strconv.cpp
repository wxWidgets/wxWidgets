/////////////////////////////////////////////////////////////////////////////
// Name:        strconv.cpp
// Purpose:     Unicode conversion classes
// Author:      Ove Kaaven, Robert Roebling, Vadim Zeitlin, Vaclav Slavik
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Ove Kaaven, Robert Roebling, Vadim Zeitlin, Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "strconv.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

#include <errno.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#include "wx/module.h"
#include "wx/strconv.h"

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

#if wxUSE_WCHAR_T
    WXDLLEXPORT_DATA(wxMBConv) wxConvLibc;
    WXDLLEXPORT_DATA(wxCSConv) wxConvLocal((const wxChar *)NULL);
    WXDLLEXPORT_DATA(wxCSConv) wxConvISO8859_1(_T("iso-8859-1"));
#else
    // stand-ins in absence of wchar_t
    WXDLLEXPORT_DATA(wxMBConv) wxConvLibc,
                               wxConvFile,
                               wxConvISO8859_1,
                               wxConvLocal,
                               wxConvUTF8;
#endif // wxUSE_WCHAR_T

WXDLLEXPORT_DATA(wxMBConv *) wxConvCurrent = &wxConvLibc;

class wxStrConvModule: public wxModule
{
public:
    wxStrConvModule() : wxModule() { }
    virtual bool OnInit() { return TRUE; }
    virtual void OnExit()
    {
#if wxUSE_WCHAR_T
         wxConvLocal.Clear();
         wxConvISO8859_1.Clear();
#endif
    }

    DECLARE_DYNAMIC_CLASS(wxStrConvModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxStrConvModule, wxModule)


// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if wxUSE_WCHAR_T

#ifdef __SALFORDC__
    #include <clib.h>
#endif

#ifdef HAVE_ICONV
    #include <iconv.h>
#endif

#include "wx/encconv.h"
#include "wx/fontmap.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define BSWAP_UCS4(str, len) { unsigned _c; for (_c=0; _c<len; _c++) str[_c]=wxUINT32_SWAP_ALWAYS(str[_c]); }
#define BSWAP_UTF16(str, len) { unsigned _c; for (_c=0; _c<len; _c++) str[_c]=wxUINT16_SWAP_ALWAYS(str[_c]); }

// under Unix SIZEOF_WCHAR_T is defined by configure, but under other platforms
// it might be not defined - assume the most common value
#ifndef SIZEOF_WCHAR_T
    #define SIZEOF_WCHAR_T 2
#endif // !defined(SIZEOF_WCHAR_T)

#if SIZEOF_WCHAR_T == 4
    #define WC_NAME         "UCS4"
    #define WC_BSWAP         BSWAP_UCS4
    #ifdef WORDS_BIGENDIAN
      #define WC_NAME_BEST  "UCS-4BE"
    #else
      #define WC_NAME_BEST  "UCS-4LE"
    #endif
#elif SIZEOF_WCHAR_T == 2
    #define WC_NAME         "UTF16"
    #define WC_BSWAP         BSWAP_UTF16
    #define WC_UTF16
    #ifdef WORDS_BIGENDIAN
      #define WC_NAME_BEST  "UTF-16BE"
    #else
      #define WC_NAME_BEST  "UTF-16LE"
    #endif
#else // sizeof(wchar_t) != 2 nor 4
    // I don't know what to do about this
    #error "Weird sizeof(wchar_t): please report your platform details to wx-users mailing list"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// UTF-16 en/decoding
// ----------------------------------------------------------------------------

#ifdef WC_UTF16

static size_t encode_utf16(wxUint32 input, wchar_t *output)
{
    if (input<=0xffff)
    {
        if (output) *output++ = (wchar_t) input;
        return 1;
    }
    else if (input>=0x110000)
    {
        return (size_t)-1;
    }
    else
    {
        if (output)
        {
            *output++ = (wchar_t) ((input >> 10)+0xd7c0);
            *output++ = (wchar_t) ((input&0x3ff)+0xdc00);
        }
        return 2;
    }
}

static size_t decode_utf16(const wchar_t* input, wxUint32& output)
{
    if ((*input<0xd800) || (*input>0xdfff))
    {
        output = *input;
        return 1;
    }
    else if ((input[1]<0xdc00) || (input[1]>=0xdfff))
    {
        output = *input;
        return (size_t)-1;
    }
    else
    {
        output = ((input[0] - 0xd7c0) << 10) + (input[1] - 0xdc00);
        return 2;
    }
}

#endif // WC_UTF16

// ----------------------------------------------------------------------------
// wxMBConv
// ----------------------------------------------------------------------------

#define IGNORE_LIBC 0

wxMBConv::~wxMBConv()
{
    // nothing to do here
}

size_t wxMBConv::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
#if IGNORE_LIBC
    if (buf)
    {
        for (size_t i = 0; i < strlen( psz )+1; i++)
            buf[i] = (wchar_t) psz[i];
        return strlen( psz );
    }
    else
    {
        return strlen( psz );
    }
#else
    return wxMB2WC(buf, psz, n);
#endif
}

size_t wxMBConv::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
#if IGNORE_LIBC
    if (buf)
    {
        for (size_t i = 0; i < wxStrlen( psz )+1; i++)
            buf[i] = (char) psz[i];
        return wxStrlen( psz );
    }
    else
    {
        return wxStrlen( psz );
    }
#else
    return wxWC2MB(buf, psz, n);
#endif
}

const wxWCharBuffer wxMBConv::cMB2WC(const char *psz) const
{
    if ( psz )
    {
        // calculate the length of the buffer needed first
        size_t nLen = MB2WC(NULL, psz, 0);
        if ( nLen != (size_t)-1 )
        {
            // now do the actual conversion
            wxWCharBuffer buf(nLen);
            MB2WC(buf.data(), psz, nLen + 1); // with the trailing NUL

            return buf;
        }
    }

    wxWCharBuffer buf((wchar_t *)NULL);

    return buf;
}

const wxCharBuffer wxMBConv::cWC2MB(const wchar_t *pwz) const
{
    if ( pwz )
    {
        size_t nLen = WC2MB(NULL, pwz, 0);
        if ( nLen != (size_t)-1 )
        {
            wxCharBuffer buf(nLen);
            WC2MB(buf.data(), pwz, nLen + 1);

            return buf;
        }
    }

    wxCharBuffer buf((char *)NULL);

    return buf;
}

// ----------------------------------------------------------------------------
// standard gdk conversion
// ----------------------------------------------------------------------------

#ifdef __WXGTK12__

WXDLLEXPORT_DATA(wxMBConvGdk) wxConvGdk;

#include <gdk/gdk.h>

size_t wxMBConvGdk::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    if (buf)
    {
        return gdk_mbstowcs((GdkWChar *)buf, psz, n);
    }
    else
    {
        GdkWChar *nbuf = new GdkWChar[n=strlen(psz)];
        size_t len = gdk_mbstowcs(nbuf, psz, n);
        delete[] nbuf;
        return len;
    }
}

size_t wxMBConvGdk::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    char *mbstr = gdk_wcstombs((GdkWChar *)psz);
    size_t len = mbstr ? strlen(mbstr) : 0;
    if (buf)
    {
        if (len > n)
            len = n;
        memcpy(buf, psz, len);
        if (len < n)
            buf[len] = 0;
    }
    return len;
}

#endif // GTK > 1.0

// ----------------------------------------------------------------------------
// UTF-7
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConvUTF7) wxConvUTF7;

#if 0
static char utf7_setD[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz"
                        "0123456789'(),-./:?";
static char utf7_setO[]="!\"#$%&*;<=>@[]^_`{|}";
static char utf7_setB[]="ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                        "abcdefghijklmnopqrstuvwxyz"
                        "0123456789+/";
#endif

// TODO: write actual implementations of UTF-7 here
size_t wxMBConvUTF7::MB2WC(wchar_t * WXUNUSED(buf),
                           const char * WXUNUSED(psz),
                           size_t WXUNUSED(n)) const
{
  return 0;
}

size_t wxMBConvUTF7::WC2MB(char * WXUNUSED(buf),
                           const wchar_t * WXUNUSED(psz),
                           size_t WXUNUSED(n)) const
{
  return 0;
}

// ----------------------------------------------------------------------------
// UTF-8
// ----------------------------------------------------------------------------

WXDLLEXPORT_DATA(wxMBConvUTF8) wxConvUTF8;

static wxUint32 utf8_max[]=
    { 0x7f, 0x7ff, 0xffff, 0x1fffff, 0x3ffffff, 0x7fffffff, 0xffffffff };

size_t wxMBConvUTF8::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len = 0;

    while (*psz && ((!buf) || (len < n)))
    {
        unsigned char cc = *psz++, fc = cc;
        unsigned cnt;
        for (cnt = 0; fc & 0x80; cnt++)
            fc <<= 1;
        if (!cnt)
        {
            // plain ASCII char
            if (buf)
                *buf++ = cc;
            len++;
        }
        else
        {
            cnt--;
            if (!cnt)
            {
                // invalid UTF-8 sequence
                return (size_t)-1;
            }
            else
            {
                unsigned ocnt = cnt - 1;
                wxUint32 res = cc & (0x3f >> cnt);
                while (cnt--)
                {
                    cc = *psz++;
                    if ((cc & 0xC0) != 0x80)
                    {
                        // invalid UTF-8 sequence
                        return (size_t)-1;
                    }
                    res = (res << 6) | (cc & 0x3f);
                }
                if (res <= utf8_max[ocnt])
                {
                    // illegal UTF-8 encoding
                    return (size_t)-1;
                }
#ifdef WC_UTF16
                size_t pa = encode_utf16(res, buf);
                if (pa == (size_t)-1)
                  return (size_t)-1;
                if (buf)
                    buf += pa;
                len += pa;
#else // !WC_UTF16
                if (buf)
                    *buf++ = res;
                len++;
#endif // WC_UTF16/!WC_UTF16
            }
        }
    }
    if (buf && (len < n))
        *buf = 0;
    return len;
}

size_t wxMBConvUTF8::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len = 0;

    while (*psz && ((!buf) || (len < n)))
    {
        wxUint32 cc;
#ifdef WC_UTF16
        size_t pa = decode_utf16(psz, cc);
        psz += (pa == (size_t)-1) ? 1 : pa;
#else
        cc=(*psz++) & 0x7fffffff;
#endif
        unsigned cnt;
        for (cnt = 0; cc > utf8_max[cnt]; cnt++) {}
        if (!cnt)
        {
            // plain ASCII char
            if (buf)
                *buf++ = (char) cc;
            len++;
        }

        else
        {
            len += cnt + 1;
            if (buf)
            {
                *buf++ = (char) ((-128 >> cnt) | ((cc >> (cnt * 6)) & (0x3f >> cnt)));
                while (cnt--)
                    *buf++ = (char) (0x80 | ((cc >> (cnt * 6)) & 0x3f));
            }
        }
    }

    if (buf && (len<n)) *buf = 0;

    return len;
}

// ============================================================================
// wxCharacterSet and derived classes
// ============================================================================

// ----------------------------------------------------------------------------
// wxCharacterSet is the ABC for the classes below
// ----------------------------------------------------------------------------

class wxCharacterSet
{
public:
    wxCharacterSet(const wxChar*name) : cname(name) {}
    virtual ~wxCharacterSet() {}
    virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) = 0;
    virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) = 0;
    virtual bool usable() const = 0;
public:
    const wxChar*cname;
};

// ----------------------------------------------------------------------------
// ID_CharSet: implementation of wxCharacterSet using an existing wxMBConv
// ----------------------------------------------------------------------------

class ID_CharSet : public wxCharacterSet
{
public:
    ID_CharSet(const wxChar *name, wxMBConv *cnv)
        : wxCharacterSet(name), work(cnv) {}

    size_t MB2WC(wchar_t *buf, const char *psz, size_t n)
        { return work ? work->MB2WC(buf,psz,n) : (size_t)-1; }

    size_t WC2MB(char *buf, const wchar_t *psz, size_t n)
        { return work ? work->WC2MB(buf,psz,n) : (size_t)-1; }

    bool usable() const
        { return work!=NULL; }
public:
    wxMBConv*work;
};


// ============================================================================
// The classes doing conversion using the iconv_xxx() functions
// ============================================================================

#ifdef HAVE_ICONV

// VS: glibc 2.1.3 is broken in that iconv() conversion to/from UCS4 fails with E2BIG
//     if output buffer is _exactly_ as big as needed. Such case is (unless there's
//     yet another bug in glibc) the only case when iconv() returns with (size_t)-1
//     (which means error) and says there are 0 bytes left in the input buffer --
//     when _real_ error occurs, bytes-left-in-input buffer is non-zero. Hence,
//     this alternative test for iconv() failure.
//     [This bug does not appear in glibc 2.2.]
#if defined(__GLIBC__) && __GLIBC__ == 2 && __GLIBC_MINOR__ <= 1
#define ICONV_FAILED(cres, bufLeft) ((cres == (size_t)-1) && \
                                     (errno != E2BIG || bufLeft != 0))
#else
#define ICONV_FAILED(cres, bufLeft)  (cres == (size_t)-1)
#endif

#define ICONV_CHAR_CAST(x)  ((ICONV_CONST char **)(x))

// ----------------------------------------------------------------------------
// IC_CharSet: encapsulates an iconv character set
// ----------------------------------------------------------------------------

class IC_CharSet : public wxCharacterSet
{
public:
    IC_CharSet(const wxChar *name);
    virtual ~IC_CharSet();

    virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n);
    virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n);

    bool usable() const
        { return (m2w != (iconv_t)-1) && (w2m != (iconv_t)-1); }

protected:
    // the iconv handlers used to translate from multibyte to wide char and in
    // the other direction
    iconv_t m2w,
            w2m;

private:
    // the name (for iconv_open()) of a wide char charset - if none is
    // available on this machine, it will remain NULL
    static const char *ms_wcCharsetName;

    // true if the wide char encoding we use (i.e. ms_wcCharsetName) has
    // different endian-ness than the native one
    static bool ms_wcNeedsSwap;
};

const char *IC_CharSet::ms_wcCharsetName = NULL;
bool IC_CharSet::ms_wcNeedsSwap = FALSE;

IC_CharSet::IC_CharSet(const wxChar *name)
          : wxCharacterSet(name)
{
    // Do it the hard way
    char cname[100];
    for (size_t i = 0; i < wxStrlen(name)+1; i++)
        cname[i] = (char) name[i];

    // check for charset that represents wchar_t:
    if (ms_wcCharsetName == NULL)
    {
        ms_wcNeedsSwap = FALSE;

        // try charset with explicit bytesex info (e.g. "UCS-4LE"):
        ms_wcCharsetName = WC_NAME_BEST;
        m2w = iconv_open(ms_wcCharsetName, cname);

        if (m2w == (iconv_t)-1)
        {
            // try charset w/o bytesex info (e.g. "UCS4")
            // and check for bytesex ourselves:
            ms_wcCharsetName = WC_NAME;
            m2w = iconv_open(ms_wcCharsetName, cname);

            // last bet, try if it knows WCHAR_T pseudo-charset
            if (m2w == (iconv_t)-1)
            {
                ms_wcCharsetName = "WCHAR_T";
                m2w = iconv_open(ms_wcCharsetName, cname);
            }

            if (m2w != (iconv_t)-1)
            {
                char    buf[2], *bufPtr;
                wchar_t wbuf[2], *wbufPtr;
                size_t  insz, outsz;
                size_t  res;

                buf[0] = 'A';
                buf[1] = 0;
                wbuf[0] = 0;
                insz = 2;
                outsz = SIZEOF_WCHAR_T * 2;
                wbufPtr = wbuf;
                bufPtr = buf;

                res = iconv(m2w, ICONV_CHAR_CAST(&bufPtr), &insz,
                            (char**)&wbufPtr, &outsz);

                if (ICONV_FAILED(res, insz))
                {
                    ms_wcCharsetName = NULL;
                    wxLogLastError(wxT("iconv"));
                    wxLogError(_("Conversion to charset '%s' doesn't work."), name);
                }
                else
                {
                    ms_wcNeedsSwap = wbuf[0] != (wchar_t)buf[0];
                }
            }
            else
            {
                ms_wcCharsetName = NULL;

                // VS: we must not output an error here, since wxWindows will safely
                //     fall back to using wxEncodingConverter.
                wxLogTrace(wxT("strconv"), wxT("Impossible to convert to/from charset '%s' with iconv, falling back to wxEncodingConverter."), name);
                //wxLogError(
            }
        }
        wxLogTrace(wxT("strconv"), wxT("wchar_t charset is '%s', needs swap: %i"), ms_wcCharsetName, ms_wcNeedsSwap);
    }
    else // we already have ms_wcCharsetName
    {
        m2w = iconv_open(ms_wcCharsetName, cname);
    }

    // NB: don't ever pass NULL to iconv_open(), it may crash!
    if ( ms_wcCharsetName )
    {
        w2m = iconv_open( cname, ms_wcCharsetName);
    }
    else
    {
        w2m = (iconv_t)-1;
    }
}

IC_CharSet::~IC_CharSet()
{
    if ( m2w != (iconv_t)-1 )
        iconv_close(m2w);
    if ( w2m != (iconv_t)-1 )
        iconv_close(w2m);
}

size_t IC_CharSet::MB2WC(wchar_t *buf, const char *psz, size_t n)
{
    size_t inbuf = strlen(psz);
    size_t outbuf = n * SIZEOF_WCHAR_T;
    size_t res, cres;
    // VS: Use these instead of psz, buf because iconv() modifies its arguments:
    wchar_t *bufPtr = buf;
    const char *pszPtr = psz;

    if (buf)
    {
        // have destination buffer, convert there
        cres = iconv(m2w,
                     ICONV_CHAR_CAST(&pszPtr), &inbuf,
                     (char**)&bufPtr, &outbuf);
        res = n - (outbuf / SIZEOF_WCHAR_T);

        if (ms_wcNeedsSwap)
        {
            // convert to native endianness
            WC_BSWAP(buf /* _not_ bufPtr */, res)
        }

        // NB: iconv was given only strlen(psz) characters on input, and so
        //     it couldn't convert the trailing zero. Let's do it ourselves
        //     if there's some room left for it in the output buffer.
        if (res < n)
            buf[res] = 0;
    }
    else
    {
        // no destination buffer... convert using temp buffer
        // to calculate destination buffer requirement
        wchar_t tbuf[8];
        res = 0;
        do {
            bufPtr = tbuf;
            outbuf = 8*SIZEOF_WCHAR_T;

            cres = iconv(m2w,
                         ICONV_CHAR_CAST(&pszPtr), &inbuf,
                         (char**)&bufPtr, &outbuf );

            res += 8-(outbuf/SIZEOF_WCHAR_T);
        } while ((cres==(size_t)-1) && (errno==E2BIG));
    }

    if (ICONV_FAILED(cres, inbuf))
    {
        //VS: it is ok if iconv fails, hence trace only
        wxLogTrace(wxT("strconv"), wxT("iconv failed: %s"), wxSysErrorMsg(wxSysErrorCode()));
        return (size_t)-1;
    }

    return res;
}

size_t IC_CharSet::WC2MB(char *buf, const wchar_t *psz, size_t n)
{
    size_t inbuf = wxWcslen(psz) * SIZEOF_WCHAR_T;
    size_t outbuf = n;
    size_t res, cres;

    wchar_t *tmpbuf = 0;

    if (ms_wcNeedsSwap)
    {
        // need to copy to temp buffer to switch endianness
        // this absolutely doesn't rock!
        // (no, doing WC_BSWAP twice on the original buffer won't help, as it
        //  could be in read-only memory, or be accessed in some other thread)
        tmpbuf=(wchar_t*)malloc((inbuf+1)*SIZEOF_WCHAR_T);
        memcpy(tmpbuf,psz,(inbuf+1)*SIZEOF_WCHAR_T);
        WC_BSWAP(tmpbuf, inbuf)
        psz=tmpbuf;
    }

    if (buf)
    {
        // have destination buffer, convert there
        cres = iconv( w2m, ICONV_CHAR_CAST(&psz), &inbuf, &buf, &outbuf );

        res = n-outbuf;

        // NB: iconv was given only wcslen(psz) characters on input, and so
        //     it couldn't convert the trailing zero. Let's do it ourselves
        //     if there's some room left for it in the output buffer.
        if (res < n)
            buf[0] = 0;
    }
    else
    {
        // no destination buffer... convert using temp buffer
        // to calculate destination buffer requirement
        char tbuf[16];
        res = 0;
        do {
            buf = tbuf; outbuf = 16;

            cres = iconv( w2m, ICONV_CHAR_CAST(&psz), &inbuf, &buf, &outbuf );

            res += 16 - outbuf;
        } while ((cres==(size_t)-1) && (errno==E2BIG));
    }

    if (ms_wcNeedsSwap)
    {
        free(tmpbuf);
    }

    if (ICONV_FAILED(cres, inbuf))
    {
        //VS: it is ok if iconv fails, hence trace only
        wxLogTrace(wxT("strconv"), wxT("iconv failed: %s"), wxSysErrorMsg(wxSysErrorCode()));
        return (size_t)-1;
    }

    return res;
}

#endif // HAVE_ICONV

// ============================================================================
// Win32 conversion classes
// ============================================================================

#if defined(__WIN32__) && !defined(__WXMICROWIN__) && !defined(__WXUNIVERSAL__)

extern long wxCharsetToCodepage(const wxChar *charset); // from utils.cpp

class CP_CharSet : public wxCharacterSet
{
public:
    CP_CharSet(const wxChar* name)
        : wxCharacterSet(name)
        {
            m_CodePage = wxCharsetToCodepage(name);
        }

    size_t MB2WC(wchar_t *buf, const char *psz, size_t n)
    {
        const size_t len = ::MultiByteToWideChar
                             (
                                m_CodePage,     // code page
                                0,              // flags (none)
                                psz,            // input string
                                -1,             // its length (NUL-terminated)
                                buf,            // output string
                                buf ? n : 0     // size of output buffer
                             );

        // note that it returns # of written chars for buf != NULL and *size*
        // of the needed buffer for buf == NULL
        return len ? (buf ? len : len - 1) : (size_t)-1;
    }

    size_t WC2MB(char *buf, const wchar_t *psz, size_t n)
    {
        const size_t len = ::WideCharToMultiByte
                             (
                                m_CodePage,     // code page
                                0,              // flags (none)
                                psz,            // input string
                                -1,             // it is (wide) NUL-terminated
                                buf,            // output buffer
                                buf ? n : 0,    // and its size
                                NULL,           // default "replacement" char
                                NULL            // [out] was it used?
                             );

        // see the comment above!
        return len ? (buf ? len : len - 1) : (size_t)-1;
    }

    bool usable() const
        { return m_CodePage != -1; }

public:
    long m_CodePage;
};
#endif // defined(__WIN32__) && !defined(__WXMICROWIN__) && !defined(__WXUNIVERSAL__)

// ============================================================================
// wxEncodingConverter based conversion classes
// ============================================================================

#if wxUSE_FONTMAP

class EC_CharSet : public wxCharacterSet
{
public:
    // temporarily just use wxEncodingConverter stuff,
    // so that it works while a better implementation is built
    EC_CharSet(const wxChar* name) : wxCharacterSet(name),
                                     enc(wxFONTENCODING_SYSTEM)
    {
        if (name)
            enc = wxFontMapper::Get()->CharsetToEncoding(name, FALSE);

        m_ok = m2w.Init(enc, wxFONTENCODING_UNICODE) &&
               w2m.Init(wxFONTENCODING_UNICODE, enc);
    }

    size_t MB2WC(wchar_t *buf, const char *psz, size_t WXUNUSED(n))
    {
        size_t inbuf = strlen(psz);
        if (buf)
            m2w.Convert(psz,buf);
        return inbuf;
    }

    size_t WC2MB(char *buf, const wchar_t *psz, size_t WXUNUSED(n))
    {
        const size_t inbuf = wxWcslen(psz);
        if (buf)
            w2m.Convert(psz,buf);

        return inbuf;
    }

    bool usable() const { return m_ok; }

public:
    wxFontEncoding enc;
    wxEncodingConverter m2w, w2m;

    // were we initialized successfully?
    bool m_ok;
};

#endif // wxUSE_FONTMAP

// ----------------------------------------------------------------------------
// the function creating the wxCharacterSet for the specified charset on the
// current system, trying all possibilities
// ----------------------------------------------------------------------------

static wxCharacterSet *wxGetCharacterSet(const wxChar *name)
{
    // check for the special case of ASCII charset
#if wxUSE_FONTMAP
    if ( wxFontMapper::Get()->CharsetToEncoding(name) == wxFONTENCODING_DEFAULT )
#else // wxUSE_FONTMAP
    if ( !name )
#endif // wxUSE_FONTMAP/!wxUSE_FONTMAP
    {
        // don't convert at all
        return NULL;
    }

    // the test above must have taken care of this case
    wxCHECK_MSG( name, NULL, _T("NULL name must be wxFONTENCODING_DEFAULT") );

    wxCharacterSet *cset;

    if ( wxStricmp(name, wxT("UTF8")) == 0 || wxStricmp(name, wxT("UTF-8")) == 0)
    {
        cset = new ID_CharSet(name, &wxConvUTF8);
    }
    else
    {
#ifdef HAVE_ICONV
        cset = new IC_CharSet(name);
#else // !HAVE_ICONV
        cset = NULL;
#endif // HAVE_ICONV/!HAVE_ICONV
    }

    // it can only be NULL in this case
#ifndef HAVE_ICONV
    if ( cset )
#endif // !HAVE_ICONV
    {
        if ( cset->usable() )
            return cset;

        delete cset;
        cset = NULL;
    }

#if defined(__WIN32__) && !defined(__WXMICROWIN__) && !defined(__WXUNIVERSAL__)
    cset = new CP_CharSet(name);
    if ( cset->usable() )
        return cset;

    delete cset;
    cset = NULL;
#endif // defined(__WIN32__) && !defined(__WXMICROWIN__) && !defined(__WXUNIVERSAL__)

#if wxUSE_FONTMAP
    cset = new EC_CharSet(name);
    if ( cset->usable() )
        return cset;

    delete cset;
    cset = NULL;
#endif // wxUSE_FONTMAP

    wxLogError(_("Cannot convert from encoding '%s'!"), name);

    return NULL;
}

// ============================================================================
// wxCSConv implementation
// ============================================================================

wxCSConv::wxCSConv(const wxChar *charset)
{
    m_name = (wxChar *)NULL;
    m_cset = (wxCharacterSet *) NULL;
    m_deferred = TRUE;

    SetName(charset);
}

wxCSConv::~wxCSConv()
{
    Clear();
}

wxCSConv::wxCSConv(const wxCSConv& conv)
    : wxMBConv()
{
    Clear();
    SetName(conv.m_name);
}

wxCSConv& wxCSConv::operator=(const wxCSConv& conv)
{
    Clear();
    SetName(conv.m_name);
    return *this;
}

void wxCSConv::Clear()
{
    if (m_name)
        free(m_name);
    if (m_cset)
        delete m_cset;
    m_name = NULL;
    m_cset = NULL;
}

void wxCSConv::SetName(const wxChar *charset)
{
    if (charset)
    {
        m_name = wxStrdup(charset);
        m_deferred = TRUE;
    }
}

void wxCSConv::LoadNow()
{
    if ( m_deferred )
    {
        // it would probably be better to make GetSystemEncodingName() always
        // available (i.e. even when wxUSE_INTL == 0)?
#if wxUSE_INTL
        if ( !m_name )
        {
            wxString name = wxLocale::GetSystemEncodingName();
            if ( !name.empty() )
            {
                SetName(name);
            }
        }
#endif // wxUSE_INTL

        // wxGetCharacterSet() complains about NULL name
        m_cset = m_name ? wxGetCharacterSet(m_name) : NULL;
        m_deferred = FALSE;
    }
}

size_t wxCSConv::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    ((wxCSConv *)this)->LoadNow(); // discard constness

    if (m_cset)
        return m_cset->MB2WC(buf, psz, n);

    // latin-1 (direct)
    size_t len = strlen(psz);

    if (buf)
    {
        for (size_t c = 0; c <= len; c++)
            buf[c] = (unsigned char)(psz[c]);
    }

    return len;
}

size_t wxCSConv::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    ((wxCSConv *)this)->LoadNow(); // discard constness

    if (m_cset)
        return m_cset->WC2MB(buf, psz, n);

    // latin-1 (direct)
    const size_t len = wxWcslen(psz);
    if (buf)
    {
        for (size_t c = 0; c <= len; c++)
            buf[c] = (psz[c] > 0xff) ? '?' : psz[c];
    }

    return len;
}

#endif // wxUSE_WCHAR_T


