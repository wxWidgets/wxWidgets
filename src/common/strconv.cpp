/////////////////////////////////////////////////////////////////////////////
// Name:        strconv.cpp
// Purpose:     Unicode conversion classes
// Author:      Ove Kaaven, Robert Roebling, Vadim Zeitlin, Vaclav Slavik,
//              Ryan Norton, Fredrik Roubert (UTF7)
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Ove Kaaven, Robert Roebling, Vaclav Slavik
//              (c) 2000-2003 Vadim Zeitlin
//              (c) 2004 Ryan Norton, Fredrik Roubert
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
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

#include "wx/strconv.h"

#if wxUSE_WCHAR_T

#ifdef __WXMSW__
    #include "wx/msw/private.h"
#endif

#ifdef __WINDOWS__
    #include "wx/msw/missing.h"
#endif

#ifndef __WXWINCE__
#include <errno.h>
#endif

#include <ctype.h>
#include <string.h>
#include <stdlib.h>

#if defined(__WIN32__) && !defined(__WXMICROWIN__)
    #define wxHAVE_WIN32_MB2WC
#endif // __WIN32__ but !__WXMICROWIN__

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __SALFORDC__
    #include <clib.h>
#endif

#ifdef HAVE_ICONV
    #include <iconv.h>
#endif

#include "wx/encconv.h"
#include "wx/fontmap.h"
#include "wx/utils.h"

#ifdef __WXMAC__
#include <ATSUnicode.h>
#include <TextCommon.h>
#include <TextEncodingConverter.h>

#include  "wx/mac/private.h"  // includes mac headers
#endif
// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

#define BSWAP_UCS4(str, len) { unsigned _c; for (_c=0; _c<len; _c++) str[_c]=wxUINT32_SWAP_ALWAYS(str[_c]); }
#define BSWAP_UTF16(str, len) { unsigned _c; for (_c=0; _c<len; _c++) str[_c]=wxUINT16_SWAP_ALWAYS(str[_c]); }

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
    // does this ever happen?
    #error "Unknown sizeof(wchar_t): please report this to wx-dev@lists.wxwindows.org"
#endif

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// UTF-16 en/decoding to/from UCS-4
// ----------------------------------------------------------------------------


static size_t encode_utf16(wxUint32 input, wxUint16 *output)
{
    if (input<=0xffff)
    {
        if (output)
            *output = (wxUint16) input;
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
            *output++ = (wxUint16) ((input >> 10)+0xd7c0);
            *output = (wxUint16) ((input&0x3ff)+0xdc00);
        }
        return 2;
    }
}

static size_t decode_utf16(const wxUint16* input, wxUint32& output)
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


// ----------------------------------------------------------------------------
// wxMBConv
// ----------------------------------------------------------------------------

wxMBConv::~wxMBConv()
{
    // nothing to do here (necessary for Darwin linking probably)
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
            nLen = MB2WC(buf.data(), psz, nLen + 1); // with the trailing NULL
            if ( nLen != (size_t)-1 )
            {
                return buf;
            }
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
            wxCharBuffer buf(nLen+3);       // space for a wxUint32 trailing zero
            nLen = WC2MB(buf.data(), pwz, nLen + 4);
            if ( nLen != (size_t)-1 )
            {
                return buf;
            }
        }
    }

    wxCharBuffer buf((char *)NULL);

    return buf;
}

const wxWCharBuffer wxMBConv::cMB2WC(const char *szString, size_t nStringLen, size_t* pOutSize) const
{
    wxASSERT(pOutSize != NULL);

    const char* szEnd = szString + nStringLen + 1;
    const char* szPos = szString;
    const char* szStart = szPos;

    size_t nActualLength = 0;
    size_t nCurrentSize = nStringLen; //try normal size first (should never resize?)

    wxWCharBuffer theBuffer(nCurrentSize);

    //Convert the string until the length() is reached, continuing the
    //loop every time a null character is reached
    while(szPos != szEnd)
    {
        wxASSERT(szPos < szEnd); //something is _really_ screwed up if this rings true

        //Get the length of the current (sub)string
        size_t nLen = MB2WC(NULL, szPos, 0);

        //Invalid conversion?
        if( nLen == (size_t)-1 )
        {
            *pOutSize = 0;
            theBuffer.data()[0u] = wxT('\0');
            return theBuffer;
        }


        //Increase the actual length (+1 for current null character)
        nActualLength += nLen + 1;

        //if buffer too big, realloc the buffer
        if (nActualLength > (nCurrentSize+1))
        {
            wxWCharBuffer theNewBuffer(nCurrentSize << 1);
            memcpy(theNewBuffer.data(), theBuffer.data(), nCurrentSize * sizeof(wchar_t));
            theBuffer = theNewBuffer;
            nCurrentSize <<= 1;
        }

        //Convert the current (sub)string
        if ( MB2WC(&theBuffer.data()[szPos - szStart], szPos, nLen + 1) == (size_t)-1 )
        {
            *pOutSize = 0;
            theBuffer.data()[0u] = wxT('\0');
            return theBuffer;
        }

        //Increment to next (sub)string
        //Note that we have to use strlen here instead of nLen
        //here because XX2XX gives us the size of the output buffer,
        //not neccessarly the length of the string
        szPos += strlen(szPos) + 1;
    }

    //success - return actual length and the buffer
    *pOutSize = nActualLength;
    return theBuffer;  
}

const wxCharBuffer wxMBConv::cWC2MB(const wchar_t *szString, size_t nStringLen, size_t* pOutSize) const
{
    wxASSERT(pOutSize != NULL);

    const wchar_t* szEnd = szString + nStringLen + 1;
    const wchar_t* szPos = szString;
    const wchar_t* szStart = szPos;

    size_t nActualLength = 0;
    size_t nCurrentSize = nStringLen << 2; //try * 4 first

    wxCharBuffer theBuffer(nCurrentSize);

    //Convert the string until the length() is reached, continuing the
    //loop every time a null character is reached
    while(szPos != szEnd)
    {
        wxASSERT(szPos < szEnd); //something is _really_ screwed up if this rings true

        //Get the length of the current (sub)string
        size_t nLen = WC2MB(NULL, szPos, 0);

        //Invalid conversion?
        if( nLen == (size_t)-1 )
        {
            *pOutSize = 0;
            theBuffer.data()[0u] = wxT('\0');
            return theBuffer;
        }

        //Increase the actual length (+1 for current null character)
        nActualLength += nLen + 1;
        
        //if buffer too big, realloc the buffer
        if (nActualLength > (nCurrentSize+1))
        {
            wxCharBuffer theNewBuffer(nCurrentSize << 1);
            memcpy(theNewBuffer.data(), theBuffer.data(), nCurrentSize);
            theBuffer = theNewBuffer;
            nCurrentSize <<= 1;
        }

        //Convert the current (sub)string
        if(WC2MB(&theBuffer.data()[szPos - szStart], szPos, nLen + 1) == (size_t)-1 )
        {
            *pOutSize = 0;
            theBuffer.data()[0u] = wxT('\0');
            return theBuffer;
        }

        //Increment to next (sub)string
        //Note that we have to use wxWcslen here instead of nLen
        //here because XX2XX gives us the size of the output buffer,
        //not neccessarly the length of the string
        szPos += wxWcslen(szPos) + 1;
    }

    //success - return actual length and the buffer
    *pOutSize = nActualLength;
    return theBuffer;  
}

// ----------------------------------------------------------------------------
// wxMBConvLibc
// ----------------------------------------------------------------------------

size_t wxMBConvLibc::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    return wxMB2WC(buf, psz, n);
}

size_t wxMBConvLibc::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    return wxWC2MB(buf, psz, n);
}
// ----------------------------------------------------------------------------
// UTF-7 
// ----------------------------------------------------------------------------

// Implementation (C) 2004 Fredrik Roubert

//
// BASE64 decoding table
//
static const unsigned char utf7unb64[] =
{
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0x3e, 0xff, 0xff, 0xff, 0x3f,
    0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3a, 0x3b,
    0x3c, 0x3d, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
    0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e,
    0x0f, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
    0x17, 0x18, 0x19, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0x1a, 0x1b, 0x1c, 0x1d, 0x1e, 0x1f, 0x20,
    0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
    0x29, 0x2a, 0x2b, 0x2c, 0x2d, 0x2e, 0x2f, 0x30,
    0x31, 0x32, 0x33, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff
};

size_t wxMBConvUTF7::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{

    size_t len = 0;

    while (*psz && ((!buf) || (len < n)))
    {
        unsigned char cc = *psz++;
        if (cc != '+')
        {
            // plain ASCII char
            if (buf)
                *buf++ = cc;
            len++;
        }
        else if (*psz == '-')
        {
            // encoded plus sign
            if (buf)
                *buf++ = cc;
            len++;
            psz++;
        }
        else
        {
            // BASE64 encoded string
            bool lsb;
            unsigned char c;
            unsigned int d, l;
            for (lsb = false, d = 0, l = 0;
                (cc = utf7unb64[(unsigned char)*psz]) != 0xff; psz++)
            {
                d <<= 6;
                d += cc;
                for (l += 6; l >= 8; lsb = !lsb)
                {
                    c = (d >> (l -= 8)) % 256;
                    if (lsb)
                    {
                        if (buf)
                            *buf++ |= c;
                        len ++;
                    }
                    else
                        if (buf)
                            *buf = c << 8;
                }
            }
            if (*psz == '-')
                psz++;
        }
    }
    if (buf && (len < n))
        *buf = 0;
    return len;
}

//
// BASE64 encoding table
//
static const unsigned char utf7enb64[] =
{
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H',
    'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P',
    'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X',
    'Y', 'Z', 'a', 'b', 'c', 'd', 'e', 'f',
    'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n',
    'o', 'p', 'q', 'r', 's', 't', 'u', 'v',
    'w', 'x', 'y', 'z', '0', '1', '2', '3',
    '4', '5', '6', '7', '8', '9', '+', '/'
};

//
// UTF-7 encoding table
//
// 0 - Set D (directly encoded characters)
// 1 - Set O (optional direct characters)
// 2 - whitespace characters (optional)
// 3 - special characters
//
static const unsigned char utf7encode[128] =
{
    3, 3, 3, 3, 3, 3, 3, 3, 3, 2, 2, 3, 3, 2, 3, 3,
    3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3,
    2, 1, 1, 1, 1, 1, 1, 0, 0, 0, 1, 3, 0, 0, 0, 3,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 0,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 3, 1, 1, 1,
    1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 3
};

size_t wxMBConvUTF7::WC2MB(char *buf, const wchar_t 
*psz, size_t n) const
{


    size_t len = 0;

    while (*psz && ((!buf) || (len < n)))
    {
        wchar_t cc = *psz++;
        if (cc < 0x80 && utf7encode[cc] < 1)
        {
            // plain ASCII char
            if (buf)
                *buf++ = (char)cc;
            len++;
        }
#ifndef WC_UTF16
        else if (((wxUint16)cc) > 0xffff)
	    {
            // no surrogate pair generation (yet?)
            return (size_t)-1;
        }
#endif
        else
        {
            if (buf)
                *buf++ = '+';
            len++;
            if (cc != '+')
            {
                // BASE64 encode string
                unsigned int lsb, d, l;
                for (d = 0, l = 0;; psz++)
                {
                    for (lsb = 0; lsb < 2; lsb ++)
                    {
                        d <<= 8;
                        d += lsb ? cc & 0xff : (cc & 0xff00) >> 8;

                        for (l += 8; l >= 6; )
                        {
                            l -= 6;
                            if (buf)
                                *buf++ = utf7enb64[(d >> l) % 64];
                            len++;
                        }
                    }
                    cc = *psz;
                    if (!(cc) || (cc < 0x80 && utf7encode[cc] < 1))
                        break;
                }
                if (l != 0)
                {
                    if (buf)
                        *buf++ = utf7enb64[((d % 16) << (6 - l)) % 64];
                    len++;
                }
            }
            if (buf)
                *buf++ = '-';
            len++;
        }
    }
    if (buf && (len < n))
        *buf = 0;
    return len;
}

// ----------------------------------------------------------------------------
// UTF-8
// ----------------------------------------------------------------------------

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
                // cast is ok because wchar_t == wxUuint16 if WC_UTF16
                size_t pa = encode_utf16(res, (wxUint16 *)buf);
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
        // cast is ok for WC_UTF16
        size_t pa = decode_utf16((const wxUint16 *)psz, cc);
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




// ----------------------------------------------------------------------------
// UTF-16
// ----------------------------------------------------------------------------

#ifdef WORDS_BIGENDIAN
    #define wxMBConvUTF16straight wxMBConvUTF16BE
    #define wxMBConvUTF16swap     wxMBConvUTF16LE
#else
    #define wxMBConvUTF16swap     wxMBConvUTF16BE
    #define wxMBConvUTF16straight wxMBConvUTF16LE
#endif


#ifdef WC_UTF16

// copy 16bit MB to 16bit String
size_t wxMBConvUTF16straight::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint16*)psz && (!buf || len < n))
    {
        if (buf)
            *buf++ = *(wxUint16*)psz;
        len++;

        psz += sizeof(wxUint16);
    }
    if (buf && len<n)   *buf=0;

    return len;
}


// copy 16bit String to 16bit MB
size_t wxMBConvUTF16straight::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        if (buf)
        {
            *(wxUint16*)buf = *psz;
            buf += sizeof(wxUint16);
        }
        len += sizeof(wxUint16);
        psz++;
    }
    if (buf && len<=n-sizeof(wxUint16))   *(wxUint16*)buf=0;

    return len;
}


// swap 16bit MB to 16bit String
size_t wxMBConvUTF16swap::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint16*)psz && (!buf || len < n))
    {
        if (buf)
        {
            ((char *)buf)[0] = psz[1];
            ((char *)buf)[1] = psz[0];
            buf++;
        }
        len++;
        psz += sizeof(wxUint16);
    }
    if (buf && len<n)   *buf=0;

    return len;
}


// swap 16bit MB to 16bit String
size_t wxMBConvUTF16swap::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        if (buf)
        {
            *buf++ = ((char*)psz)[1];
            *buf++ = ((char*)psz)[0];
        }
        len += sizeof(wxUint16);
        psz++;
    }
    if (buf && len<=n-sizeof(wxUint16))   *(wxUint16*)buf=0;

    return len;
}


#else // WC_UTF16


// copy 16bit MB to 32bit String
size_t wxMBConvUTF16straight::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint16*)psz && (!buf || len < n))
    {
        wxUint32 cc;
        size_t pa=decode_utf16((wxUint16*)psz, cc);
        if (pa == (size_t)-1)
            return pa;

        if (buf)
            *buf++ = cc;
        len++;
        psz += pa * sizeof(wxUint16);
    }
    if (buf && len<n)   *buf=0;

    return len;
}


// copy 32bit String to 16bit MB
size_t wxMBConvUTF16straight::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        wxUint16 cc[2];
        size_t pa=encode_utf16(*psz, cc);

        if (pa == (size_t)-1)
            return pa;

        if (buf)
        {
            *(wxUint16*)buf = cc[0];
            buf += sizeof(wxUint16);
            if (pa > 1)
            {
                *(wxUint16*)buf = cc[1];
                buf += sizeof(wxUint16);
            }
        }

        len += pa*sizeof(wxUint16);
        psz++;
    }
    if (buf && len<=n-sizeof(wxUint16))   *(wxUint16*)buf=0;

    return len;
}


// swap 16bit MB to 32bit String
size_t wxMBConvUTF16swap::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint16*)psz && (!buf || len < n))
    {
        wxUint32 cc;
        char tmp[4];
        tmp[0]=psz[1];  tmp[1]=psz[0];
        tmp[2]=psz[3];  tmp[3]=psz[2];

        size_t pa=decode_utf16((wxUint16*)tmp, cc);
        if (pa == (size_t)-1)
            return pa;

        if (buf)
            *buf++ = cc;

        len++;
        psz += pa * sizeof(wxUint16);
    }
    if (buf && len<n)   *buf=0;

    return len;
}


// swap 32bit String to 16bit MB
size_t wxMBConvUTF16swap::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        wxUint16 cc[2];
        size_t pa=encode_utf16(*psz, cc);

        if (pa == (size_t)-1)
            return pa;

        if (buf)
        {
            *buf++ = ((char*)cc)[1];
            *buf++ = ((char*)cc)[0];
            if (pa > 1)
            {
                *buf++ = ((char*)cc)[3];
                *buf++ = ((char*)cc)[2];
            }
        }

        len += pa*sizeof(wxUint16);
        psz++;
    }
    if (buf && len<=n-sizeof(wxUint16))   *(wxUint16*)buf=0;

    return len;
}

#endif // WC_UTF16


// ----------------------------------------------------------------------------
// UTF-32
// ----------------------------------------------------------------------------

#ifdef WORDS_BIGENDIAN
#define wxMBConvUTF32straight  wxMBConvUTF32BE
#define wxMBConvUTF32swap      wxMBConvUTF32LE
#else
#define wxMBConvUTF32swap      wxMBConvUTF32BE
#define wxMBConvUTF32straight  wxMBConvUTF32LE
#endif


WXDLLIMPEXP_DATA_BASE(wxMBConvUTF32LE) wxConvUTF32LE;
WXDLLIMPEXP_DATA_BASE(wxMBConvUTF32BE) wxConvUTF32BE;


#ifdef WC_UTF16

// copy 32bit MB to 16bit String
size_t wxMBConvUTF32straight::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint32*)psz && (!buf || len < n))
    {
        wxUint16 cc[2];

        size_t pa=encode_utf16(*(wxUint32*)psz, cc);
        if (pa == (size_t)-1)
            return pa;

        if (buf)
        {
            *buf++ = cc[0];
            if (pa > 1)
                *buf++ = cc[1];
        }
        len += pa;
        psz += sizeof(wxUint32);
    }
    if (buf && len<n)   *buf=0;

    return len;
}


// copy 16bit String to 32bit MB
size_t wxMBConvUTF32straight::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        wxUint32 cc;

        // cast is ok for WC_UTF16
        size_t pa = decode_utf16((const wxUint16 *)psz, cc);
        if (pa == (size_t)-1)
            return pa;

        if (buf)
        {
            *(wxUint32*)buf = cc;
            buf += sizeof(wxUint32);
        }
        len += sizeof(wxUint32);
        psz += pa;
    }

    if (buf && len<=n-sizeof(wxUint32))
        *(wxUint32*)buf=0;

    return len;
}



// swap 32bit MB to 16bit String
size_t wxMBConvUTF32swap::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint32*)psz && (!buf || len < n))
    {
        char tmp[4];
        tmp[0] = psz[3];   tmp[1] = psz[2];
        tmp[2] = psz[1];   tmp[3] = psz[0];


        wxUint16 cc[2];

        size_t pa=encode_utf16(*(wxUint32*)tmp, cc);
        if (pa == (size_t)-1)
            return pa;

        if (buf)
        {
            *buf++ = cc[0];
            if (pa > 1)
                *buf++ = cc[1];
        }
        len += pa;
        psz += sizeof(wxUint32);
    }

    if (buf && len<n)
        *buf=0;

    return len;
}


// swap 16bit String to 32bit MB
size_t wxMBConvUTF32swap::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        char cc[4];

        // cast is ok for WC_UTF16
        size_t pa=decode_utf16((const wxUint16 *)psz, *(wxUint32*)cc);
        if (pa == (size_t)-1)
            return pa;

        if (buf)
        {
            *buf++ = cc[3];
            *buf++ = cc[2];
            *buf++ = cc[1];
            *buf++ = cc[0];
        }
        len += sizeof(wxUint32);
        psz += pa;
    }

    if (buf && len<=n-sizeof(wxUint32))
        *(wxUint32*)buf=0;

    return len;
}

#else // WC_UTF16


// copy 32bit MB to 32bit String
size_t wxMBConvUTF32straight::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint32*)psz && (!buf || len < n))
    {
        if (buf)
            *buf++ = *(wxUint32*)psz;
        len++;
        psz += sizeof(wxUint32);
    }

    if (buf && len<n)
        *buf=0;

    return len;
}


// copy 32bit String to 32bit MB
size_t wxMBConvUTF32straight::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        if (buf)
        {
            *(wxUint32*)buf = *psz;
            buf += sizeof(wxUint32);
        }

        len += sizeof(wxUint32);
        psz++;
    }

    if (buf && len<=n-sizeof(wxUint32))
        *(wxUint32*)buf=0;

    return len;
}


// swap 32bit MB to 32bit String
size_t wxMBConvUTF32swap::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    size_t len=0;

    while (*(wxUint32*)psz && (!buf || len < n))
    {
        if (buf)
        {
            ((char *)buf)[0] = psz[3];
            ((char *)buf)[1] = psz[2];
            ((char *)buf)[2] = psz[1];
            ((char *)buf)[3] = psz[0];
            buf++;
        }
        len++;
        psz += sizeof(wxUint32);
    }

    if (buf && len<n)
        *buf=0;

    return len;
}


// swap 32bit String to 32bit MB
size_t wxMBConvUTF32swap::WC2MB(char *buf, const wchar_t *psz, size_t n) const
{
    size_t len=0;

    while (*psz && (!buf || len < n))
    {
        if (buf)
        {
            *buf++ = ((char *)psz)[3];
            *buf++ = ((char *)psz)[2];
            *buf++ = ((char *)psz)[1];
            *buf++ = ((char *)psz)[0];
        }
        len += sizeof(wxUint32);
        psz++;
    }

    if (buf && len<=n-sizeof(wxUint32))
        *(wxUint32*)buf=0;

    return len;
}


#endif // WC_UTF16


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
// wxMBConv_iconv: encapsulates an iconv character set
// ----------------------------------------------------------------------------

class wxMBConv_iconv : public wxMBConv
{
public:
    wxMBConv_iconv(const wxChar *name);
    virtual ~wxMBConv_iconv();

    virtual size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const;
    virtual size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const;

    bool IsOk() const
        { return (m2w != (iconv_t)-1) && (w2m != (iconv_t)-1); }

protected:
    // the iconv handlers used to translate from multibyte to wide char and in
    // the other direction
    iconv_t m2w,
            w2m;

private:
    // the name (for iconv_open()) of a wide char charset -- if none is
    // available on this machine, it will remain NULL
    static const char *ms_wcCharsetName;

    // true if the wide char encoding we use (i.e. ms_wcCharsetName) has
    // different endian-ness than the native one
    static bool ms_wcNeedsSwap;
};

const char *wxMBConv_iconv::ms_wcCharsetName = NULL;
bool wxMBConv_iconv::ms_wcNeedsSwap = false;

wxMBConv_iconv::wxMBConv_iconv(const wxChar *name)
{
    // Do it the hard way
    char cname[100];
    for (size_t i = 0; i < wxStrlen(name)+1; i++)
        cname[i] = (char) name[i];

    // check for charset that represents wchar_t:
    if (ms_wcCharsetName == NULL)
    {
        ms_wcNeedsSwap = false;

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

                // VS: we must not output an error here, since wxWidgets will safely
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

wxMBConv_iconv::~wxMBConv_iconv()
{
    if ( m2w != (iconv_t)-1 )
        iconv_close(m2w);
    if ( w2m != (iconv_t)-1 )
        iconv_close(w2m);
}

size_t wxMBConv_iconv::MB2WC(wchar_t *buf, const char *psz, size_t n) const
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

size_t wxMBConv_iconv::WC2MB(char *buf, const wchar_t *psz, size_t n) const
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

#ifdef wxHAVE_WIN32_MB2WC

// from utils.cpp
#if wxUSE_FONTMAP
extern WXDLLIMPEXP_BASE long wxCharsetToCodepage(const wxChar *charset);
extern WXDLLIMPEXP_BASE long wxEncodingToCodepage(wxFontEncoding encoding);
#endif

class wxMBConv_win32 : public wxMBConv
{
public:
    wxMBConv_win32()
    {
        m_CodePage = CP_ACP;
    }

#if wxUSE_FONTMAP
    wxMBConv_win32(const wxChar* name)
    {
        m_CodePage = wxCharsetToCodepage(name);
    }

    wxMBConv_win32(wxFontEncoding encoding)
    {
        m_CodePage = wxEncodingToCodepage(encoding);
    }
#endif

    size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const
    {
        // note that we have to use MB_ERR_INVALID_CHARS flag as it without it
        // the behaviour is not compatible with the Unix version (using iconv)
        // and break the library itself, e.g. wxTextInputStream::NextChar()
        // wouldn't work if reading an incomplete MB char didn't result in an
        // error
        const size_t len = ::MultiByteToWideChar
                             (
                                m_CodePage,     // code page
                                MB_ERR_INVALID_CHARS, // flags: fall on error
                                psz,            // input string
                                -1,             // its length (NUL-terminated)
                                buf,            // output string
                                buf ? n : 0     // size of output buffer
                             );

        // note that it returns count of written chars for buf != NULL and size
        // of the needed buffer for buf == NULL so in either case the length of
        // the string (which never includes the terminating NUL) is one less
        return len ? len - 1 : (size_t)-1;
    }

    size_t WC2MB(char *buf, const wchar_t *pwz, size_t n) const
    {
        /*
            we have a problem here: by default, WideCharToMultiByte() may
            replace characters unrepresentable in the target code page with bad
            quality approximations such as turning "1/2" symbol (U+00BD) into
            "1" for the code pages which don't have it and we, obviously, want
            to avoid this at any price

            the trouble is that this function does it _silently_, i.e. it won't
            even tell us whether it did or not... Win98/2000 and higher provide
            WC_NO_BEST_FIT_CHARS but it doesn't work for the older systems and
            we have to resort to a round trip, i.e. check that converting back
            results in the same string -- this is, of course, expensive but
            otherwise we simply can't be sure to not garble the data.
         */

        // determine if we can rely on WC_NO_BEST_FIT_CHARS: according to MSDN
        // it doesn't work with CJK encodings (which we test for rather roughly
        // here...) nor with UTF-7/8 nor, of course, with Windows versions not
        // supporting it
        BOOL usedDef wxDUMMY_INITIALIZE(false);
        BOOL *pUsedDef;
        int flags;
        if ( CanUseNoBestFit() && m_CodePage < 50000 )
        {
            // it's our lucky day
            flags = WC_NO_BEST_FIT_CHARS;
            pUsedDef = &usedDef;
        }
        else // old system or unsupported encoding
        {
            flags = 0;
            pUsedDef = NULL;
        }

        const size_t len = ::WideCharToMultiByte
                             (
                                m_CodePage,     // code page
                                flags,          // either none or no best fit
                                pwz,            // input string
                                -1,             // it is (wide) NUL-terminated
                                buf,            // output buffer
                                buf ? n : 0,    // and its size
                                NULL,           // default "replacement" char
                                pUsedDef        // [out] was it used?
                             );

        if ( !len )
        {
            // function totally failed
            return (size_t)-1;
        }

        // if we were really converting, check if we succeeded
        if ( buf )
        {
            if ( flags )
            {
                // check if the conversion failed, i.e. if any replacements
                // were done
                if ( usedDef )
                    return (size_t)-1;
            }
            else // we must resort to double tripping...
            {
                wxWCharBuffer wcBuf(n);
                if ( MB2WC(wcBuf.data(), buf, n) == (size_t)-1 ||
                        wcscmp(wcBuf, pwz) != 0 )
                {
                    // we didn't obtain the same thing we started from, hence
                    // the conversion was lossy and we consider that it failed
                    return (size_t)-1;
                }
            }
        }

        // see the comment above for the reason of "len - 1"
        return len - 1;
    }

    bool IsOk() const { return m_CodePage != -1; }

private:
    static bool CanUseNoBestFit()
    {
        static int s_isWin98Or2k = -1;

        if ( s_isWin98Or2k == -1 )
        {
            int verMaj, verMin;
            switch ( wxGetOsVersion(&verMaj, &verMin) )
            {
                case wxWIN95:
                    s_isWin98Or2k = verMaj >= 4 && verMin >= 10;
                    break;

                case wxWINDOWS_NT:
                    s_isWin98Or2k = verMaj >= 5;
                    break;

                default:
                    // unknown, be conseravtive by default
                    s_isWin98Or2k = 0;
            }

            wxASSERT_MSG( s_isWin98Or2k != -1, _T("should be set above") );
        }

        return s_isWin98Or2k == 1;
    }

    long m_CodePage;
};

#endif // wxHAVE_WIN32_MB2WC

// ============================================================================
// Cocoa conversion classes
// ============================================================================

#if defined(__WXCOCOA__)

// RN:  There is no UTF-32 support in either Core Foundation or
// Cocoa.  Strangely enough, internally Core Foundation uses
// UTF 32 internally quite a bit - its just not public (yet).

#include <CoreFoundation/CFString.h>
#include <CoreFoundation/CFStringEncodingExt.h>

CFStringEncoding wxCFStringEncFromFontEnc(wxFontEncoding encoding)
{
    CFStringEncoding enc = kCFStringEncodingInvalidId ;
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        enc = CFStringGetSystemEncoding();
    }
    else switch( encoding)
    {
        case wxFONTENCODING_ISO8859_1 :
            enc = kCFStringEncodingISOLatin1 ;
            break ;
        case wxFONTENCODING_ISO8859_2 :
            enc = kCFStringEncodingISOLatin2;
            break ;
        case wxFONTENCODING_ISO8859_3 :
            enc = kCFStringEncodingISOLatin3 ;
            break ;
        case wxFONTENCODING_ISO8859_4 :
            enc = kCFStringEncodingISOLatin4;
            break ;
        case wxFONTENCODING_ISO8859_5 :
            enc = kCFStringEncodingISOLatinCyrillic;
            break ;
        case wxFONTENCODING_ISO8859_6 :
            enc = kCFStringEncodingISOLatinArabic;
            break ;
        case wxFONTENCODING_ISO8859_7 :
            enc = kCFStringEncodingISOLatinGreek;
            break ;
        case wxFONTENCODING_ISO8859_8 :
            enc = kCFStringEncodingISOLatinHebrew;
            break ;
        case wxFONTENCODING_ISO8859_9 :
            enc = kCFStringEncodingISOLatin5;
            break ;
        case wxFONTENCODING_ISO8859_10 :
            enc = kCFStringEncodingISOLatin6;
            break ;
        case wxFONTENCODING_ISO8859_11 :
            enc = kCFStringEncodingISOLatinThai;
            break ;
        case wxFONTENCODING_ISO8859_13 :
            enc = kCFStringEncodingISOLatin7;
            break ;
        case wxFONTENCODING_ISO8859_14 :
            enc = kCFStringEncodingISOLatin8;
            break ;
        case wxFONTENCODING_ISO8859_15 :
            enc = kCFStringEncodingISOLatin9;
            break ;

        case wxFONTENCODING_KOI8 :
            enc = kCFStringEncodingKOI8_R;
            break ;
        case wxFONTENCODING_ALTERNATIVE : // MS-DOS CP866
            enc = kCFStringEncodingDOSRussian;
            break ;

//      case wxFONTENCODING_BULGARIAN :
//          enc = ;
//          break ;

        case wxFONTENCODING_CP437 :
            enc =kCFStringEncodingDOSLatinUS ;
            break ;
        case wxFONTENCODING_CP850 :
            enc = kCFStringEncodingDOSLatin1;
            break ;
        case wxFONTENCODING_CP852 :
            enc = kCFStringEncodingDOSLatin2;
            break ;
        case wxFONTENCODING_CP855 :
            enc = kCFStringEncodingDOSCyrillic;
            break ;
        case wxFONTENCODING_CP866 :
            enc =kCFStringEncodingDOSRussian ;
            break ;
        case wxFONTENCODING_CP874 :
            enc = kCFStringEncodingDOSThai;
            break ;
        case wxFONTENCODING_CP932 :
            enc = kCFStringEncodingDOSJapanese;
            break ;
        case wxFONTENCODING_CP936 :
            enc =kCFStringEncodingDOSChineseSimplif ;
            break ;
        case wxFONTENCODING_CP949 :
            enc = kCFStringEncodingDOSKorean;
            break ;
        case wxFONTENCODING_CP950 :
            enc = kCFStringEncodingDOSChineseTrad;
            break ;
        case wxFONTENCODING_CP1250 :
            enc = kCFStringEncodingWindowsLatin2;
            break ;
        case wxFONTENCODING_CP1251 :
            enc =kCFStringEncodingWindowsCyrillic ;
            break ;
        case wxFONTENCODING_CP1252 :
            enc =kCFStringEncodingWindowsLatin1 ;
            break ;
        case wxFONTENCODING_CP1253 :
            enc = kCFStringEncodingWindowsGreek;
            break ;
        case wxFONTENCODING_CP1254 :
            enc = kCFStringEncodingWindowsLatin5;
            break ;
        case wxFONTENCODING_CP1255 :
            enc =kCFStringEncodingWindowsHebrew ;
            break ;
        case wxFONTENCODING_CP1256 :
            enc =kCFStringEncodingWindowsArabic ;
            break ;
        case wxFONTENCODING_CP1257 :
            enc = kCFStringEncodingWindowsBalticRim;
            break ;
//   This only really encodes to UTF7 (if that) evidently
//        case wxFONTENCODING_UTF7 :
//            enc = kCFStringEncodingNonLossyASCII ;
//            break ;
        case wxFONTENCODING_UTF8 :
            enc = kCFStringEncodingUTF8 ;
            break ;
        case wxFONTENCODING_EUC_JP :
            enc = kCFStringEncodingEUC_JP;
            break ;
        case wxFONTENCODING_UTF16 :
            enc = kCFStringEncodingUnicode ;
            break ;
        case wxFONTENCODING_MACROMAN :
            enc = kCFStringEncodingMacRoman ;
            break ;
        case wxFONTENCODING_MACJAPANESE :
            enc = kCFStringEncodingMacJapanese ;
            break ;
        case wxFONTENCODING_MACCHINESETRAD :
            enc = kCFStringEncodingMacChineseTrad ;
            break ;
        case wxFONTENCODING_MACKOREAN :
            enc = kCFStringEncodingMacKorean ;
            break ;
        case wxFONTENCODING_MACARABIC :
            enc = kCFStringEncodingMacArabic ;
            break ;
        case wxFONTENCODING_MACHEBREW :
            enc = kCFStringEncodingMacHebrew ;
            break ;
        case wxFONTENCODING_MACGREEK :
            enc = kCFStringEncodingMacGreek ;
            break ;
        case wxFONTENCODING_MACCYRILLIC :
            enc = kCFStringEncodingMacCyrillic ;
            break ;
        case wxFONTENCODING_MACDEVANAGARI :
            enc = kCFStringEncodingMacDevanagari ;
            break ;
        case wxFONTENCODING_MACGURMUKHI :
            enc = kCFStringEncodingMacGurmukhi ;
            break ;
        case wxFONTENCODING_MACGUJARATI :
            enc = kCFStringEncodingMacGujarati ;
            break ;
        case wxFONTENCODING_MACORIYA :
            enc = kCFStringEncodingMacOriya ;
            break ;
        case wxFONTENCODING_MACBENGALI :
            enc = kCFStringEncodingMacBengali ;
            break ;
        case wxFONTENCODING_MACTAMIL :
            enc = kCFStringEncodingMacTamil ;
            break ;
        case wxFONTENCODING_MACTELUGU :
            enc = kCFStringEncodingMacTelugu ;
            break ;
        case wxFONTENCODING_MACKANNADA :
            enc = kCFStringEncodingMacKannada ;
            break ;
        case wxFONTENCODING_MACMALAJALAM :
            enc = kCFStringEncodingMacMalayalam ;
            break ;
        case wxFONTENCODING_MACSINHALESE :
            enc = kCFStringEncodingMacSinhalese ;
            break ;
        case wxFONTENCODING_MACBURMESE :
            enc = kCFStringEncodingMacBurmese ;
            break ;
        case wxFONTENCODING_MACKHMER :
            enc = kCFStringEncodingMacKhmer ;
            break ;
        case wxFONTENCODING_MACTHAI :
            enc = kCFStringEncodingMacThai ;
            break ;
        case wxFONTENCODING_MACLAOTIAN :
            enc = kCFStringEncodingMacLaotian ;
            break ;
        case wxFONTENCODING_MACGEORGIAN :
            enc = kCFStringEncodingMacGeorgian ;
            break ;
        case wxFONTENCODING_MACARMENIAN :
            enc = kCFStringEncodingMacArmenian ;
            break ;
        case wxFONTENCODING_MACCHINESESIMP :
            enc = kCFStringEncodingMacChineseSimp ;
            break ;
        case wxFONTENCODING_MACTIBETAN :
            enc = kCFStringEncodingMacTibetan ;
            break ;
        case wxFONTENCODING_MACMONGOLIAN :
            enc = kCFStringEncodingMacMongolian ;
            break ;
        case wxFONTENCODING_MACETHIOPIC :
            enc = kCFStringEncodingMacEthiopic ;
            break ;
        case wxFONTENCODING_MACCENTRALEUR :
            enc = kCFStringEncodingMacCentralEurRoman ;
            break ;
        case wxFONTENCODING_MACVIATNAMESE :
            enc = kCFStringEncodingMacVietnamese ;
            break ;
        case wxFONTENCODING_MACARABICEXT :
            enc = kCFStringEncodingMacExtArabic ;
            break ;
        case wxFONTENCODING_MACSYMBOL :
            enc = kCFStringEncodingMacSymbol ;
            break ;
        case wxFONTENCODING_MACDINGBATS :
            enc = kCFStringEncodingMacDingbats ;
            break ;
        case wxFONTENCODING_MACTURKISH :
            enc = kCFStringEncodingMacTurkish ;
            break ;
        case wxFONTENCODING_MACCROATIAN :
            enc = kCFStringEncodingMacCroatian ;
            break ;
        case wxFONTENCODING_MACICELANDIC :
            enc = kCFStringEncodingMacIcelandic ;
            break ;
        case wxFONTENCODING_MACROMANIAN :
            enc = kCFStringEncodingMacRomanian ;
            break ;
        case wxFONTENCODING_MACCELTIC :
            enc = kCFStringEncodingMacCeltic ;
            break ;
        case wxFONTENCODING_MACGAELIC :
            enc = kCFStringEncodingMacGaelic ;
            break ;
//      case wxFONTENCODING_MACKEYBOARD :
//          enc = kCFStringEncodingMacKeyboardGlyphs ;
//          break ;
        default :
            // because gcc is picky
            break ;
    } ;
    return enc ;
}

class wxMBConv_cocoa : public wxMBConv
{
public:
    wxMBConv_cocoa()
    {
        Init(CFStringGetSystemEncoding()) ;
    }

    wxMBConv_cocoa(const wxChar* name)
    {
        Init( wxCFStringEncFromFontEnc(wxFontMapper::Get()->CharsetToEncoding(name, false) ) ) ;
    }

    wxMBConv_cocoa(wxFontEncoding encoding)
    {
        Init( wxCFStringEncFromFontEnc(encoding) );
    }

    ~wxMBConv_cocoa()
    {
    }

    void Init( CFStringEncoding encoding)
    {
        m_encoding = encoding ;
    }

    size_t MB2WC(wchar_t * szOut, const char * szUnConv, size_t nOutSize) const
    {
        wxASSERT(szUnConv);

        CFStringRef theString = CFStringCreateWithBytes (
                                                NULL, //the allocator
                                                (const UInt8*)szUnConv,
                                                strlen(szUnConv),
                                                m_encoding,
                                                false //no BOM/external representation
                                                );

        wxASSERT(theString);

        size_t nOutLength = CFStringGetLength(theString);

        if (szOut == NULL)
        {
            CFRelease(theString);
            return nOutLength;
        }

        CFRange theRange = { 0, nOutSize };

#if SIZEOF_WCHAR_T == 4
        UniChar* szUniCharBuffer = new UniChar[nOutSize];
#endif
 
        CFStringGetCharacters(theString, theRange, szUniCharBuffer);
        
        CFRelease(theString);

        szUniCharBuffer[nOutLength] = '\0' ;

#if SIZEOF_WCHAR_T == 4
        wxMBConvUTF16 converter ;
        converter.MB2WC(szOut, (const char*)szUniCharBuffer , nOutSize ) ;
        delete[] szUniCharBuffer;
#endif
    
        return nOutLength;
    }

    size_t WC2MB(char *szOut, const wchar_t *szUnConv, size_t nOutSize) const
    {
        wxASSERT(szUnConv);
        
        size_t nRealOutSize;
        size_t nBufSize = wxWcslen(szUnConv);
        UniChar* szUniBuffer = (UniChar*) szUnConv;

#if SIZEOF_WCHAR_T == 4
        wxMBConvUTF16BE converter ;
        nBufSize = converter.WC2MB( NULL , szUnConv , 0 );
        szUniBuffer = new UniChar[ (nBufSize / sizeof(UniChar)) + 1] ;
        converter.WC2MB( (char*) szUniBuffer , szUnConv, nBufSize + sizeof(UniChar)) ;
        nBufSize /= sizeof(UniChar);
#endif

        CFStringRef theString = CFStringCreateWithCharactersNoCopy(
                                NULL, //allocator
                                szUniBuffer,
                                nBufSize,
                                kCFAllocatorNull //deallocator - we want to deallocate it ourselves
                            );

        wxASSERT(theString);

        //Note that CER puts a BOM when converting to unicode
        //so we  check and use getchars instead in that case
        if (m_encoding == kCFStringEncodingUnicode)
        {
            if (szOut != NULL)
                CFStringGetCharacters(theString, CFRangeMake(0, nOutSize - 1), (UniChar*) szOut);
            
            nRealOutSize = CFStringGetLength(theString) + 1;
        }
        else
        {
            CFStringGetBytes(
                theString,
                CFRangeMake(0, CFStringGetLength(theString)),
                m_encoding,
                0, //what to put in characters that can't be converted -
                    //0 tells CFString to return NULL if it meets such a character
                false, //not an external representation
                (UInt8*) szOut,
                nOutSize, 
                (CFIndex*) &nRealOutSize
                        );
        }

        CFRelease(theString);

#if SIZEOF_WCHAR_T == 4
        delete[] szUniBuffer;
#endif

        return  nRealOutSize - 1;
    }

    bool IsOk() const
    {
        return m_encoding != kCFStringEncodingInvalidId && 
              CFStringIsEncodingAvailable(m_encoding);
    }

private:
    CFStringEncoding m_encoding ;
};

#endif // defined(__WXCOCOA__)

// ============================================================================
// Mac conversion classes
// ============================================================================

#if defined(__WXMAC__) && defined(TARGET_CARBON)

class wxMBConv_mac : public wxMBConv
{
public:
    wxMBConv_mac()
    {
        Init(CFStringGetSystemEncoding()) ;
    }

    wxMBConv_mac(const wxChar* name)
    {
        Init( wxMacGetSystemEncFromFontEnc(wxFontMapper::Get()->CharsetToEncoding(name, false) ) ) ;
    }

    wxMBConv_mac(wxFontEncoding encoding)
    {
        Init( wxMacGetSystemEncFromFontEnc(encoding) );
    }

    ~wxMBConv_mac()
    {
        OSStatus status = noErr ;
        status = TECDisposeConverter(m_MB2WC_converter);
        status = TECDisposeConverter(m_WC2MB_converter);
    }


    void Init( TextEncodingBase encoding)
    {
        OSStatus status = noErr ;
        m_char_encoding = encoding ;
        m_unicode_encoding = CreateTextEncoding(kTextEncodingUnicodeDefault,0,kUnicode16BitFormat) ;

        status = TECCreateConverter(&m_MB2WC_converter,
                                    m_char_encoding,
                                    m_unicode_encoding);
        status = TECCreateConverter(&m_WC2MB_converter,
                                    m_unicode_encoding,
                                    m_char_encoding);
    }

    size_t MB2WC(wchar_t *buf, const char *psz, size_t n) const
    {
        OSStatus status = noErr ;
        ByteCount byteOutLen ;
        ByteCount byteInLen = strlen(psz) ;
        wchar_t *tbuf = NULL ;
        UniChar* ubuf = NULL ;
        size_t res = 0 ;

        if (buf == NULL)
        {
            //apple specs say at least 32
            n = wxMax( 32 , byteInLen ) ;
            tbuf = (wchar_t*) malloc( n * SIZEOF_WCHAR_T) ;
        }
        ByteCount byteBufferLen = n * sizeof( UniChar ) ;
#if SIZEOF_WCHAR_T == 4
        ubuf = (UniChar*) malloc( byteBufferLen + 2 ) ;
#else
        ubuf = (UniChar*) (buf ? buf : tbuf) ;
#endif
        status = TECConvertText(m_MB2WC_converter, (ConstTextPtr) psz , byteInLen, &byteInLen,
          (TextPtr) ubuf , byteBufferLen, &byteOutLen);
#if SIZEOF_WCHAR_T == 4
        // we have to terminate here, because n might be larger for the trailing zero, and if UniChar
        // is not properly terminated we get random characters at the end
        ubuf[byteOutLen / sizeof( UniChar ) ] = 0 ;
        wxMBConvUTF16BE converter ;
        res = converter.MB2WC( (buf ? buf : tbuf) , (const char*)ubuf , n ) ;
        free( ubuf ) ;
#else
        res = byteOutLen / sizeof( UniChar ) ;
#endif
        if ( buf == NULL )
             free(tbuf) ;

        if ( buf  && res < n)
            buf[res] = 0;

        return res ;
    }

    size_t WC2MB(char *buf, const wchar_t *psz, size_t n) const
    {
        OSStatus status = noErr ;
        ByteCount byteOutLen ;
        ByteCount byteInLen = wxWcslen(psz) * SIZEOF_WCHAR_T ;

        char *tbuf = NULL ;

        if (buf == NULL)
        {
            //apple specs say at least 32
            n = wxMax( 32 , ((byteInLen / SIZEOF_WCHAR_T) * 8) + SIZEOF_WCHAR_T );
            tbuf = (char*) malloc( n ) ;
        }

        ByteCount byteBufferLen = n ;
        UniChar* ubuf = NULL ;
#if SIZEOF_WCHAR_T == 4
        wxMBConvUTF16BE converter ;
        size_t unicharlen = converter.WC2MB( NULL , psz , 0 ) ;
        byteInLen = unicharlen ;
        ubuf = (UniChar*) malloc( byteInLen + 2 ) ;
        converter.WC2MB( (char*) ubuf , psz, unicharlen + 2 ) ;
#else
        ubuf = (UniChar*) psz ;
#endif
        status = TECConvertText(m_WC2MB_converter, (ConstTextPtr) ubuf , byteInLen, &byteInLen,
            (TextPtr) (buf ? buf : tbuf) , byteBufferLen, &byteOutLen);
#if SIZEOF_WCHAR_T == 4
        free( ubuf ) ;
#endif
        if ( buf == NULL )
            free(tbuf) ;

        size_t res = byteOutLen ;
        if ( buf  && res < n)
        {
            buf[res] = 0;
            
            //we need to double-trip to verify it didn't insert any ? in place
            //of bogus characters
            wxWCharBuffer wcBuf(n);
            size_t pszlen = wxWcslen(psz);
            if ( MB2WC(wcBuf.data(), buf, n) == (size_t)-1 ||
                        wxWcslen(wcBuf) != pszlen ||
                        memcmp(wcBuf, psz, pszlen * sizeof(wchar_t)) != 0 )
            {
                // we didn't obtain the same thing we started from, hence
                // the conversion was lossy and we consider that it failed
                return (size_t)-1;
            }
        }

        return res ;
    }

    bool IsOk() const
        { return m_MB2WC_converter !=  NULL && m_WC2MB_converter != NULL  ; }

private:
    TECObjectRef m_MB2WC_converter ;
    TECObjectRef m_WC2MB_converter ;

    TextEncodingBase m_char_encoding ;
    TextEncodingBase m_unicode_encoding ;
};

#endif // defined(__WXMAC__) && defined(TARGET_CARBON)

// ============================================================================
// wxEncodingConverter based conversion classes
// ============================================================================

#if wxUSE_FONTMAP

class wxMBConv_wxwin : public wxMBConv
{
private:
    void Init()
    {
        m_ok = m2w.Init(m_enc, wxFONTENCODING_UNICODE) &&
               w2m.Init(wxFONTENCODING_UNICODE, m_enc);
    }

public:
    // temporarily just use wxEncodingConverter stuff,
    // so that it works while a better implementation is built
    wxMBConv_wxwin(const wxChar* name)
    {
        if (name)
            m_enc = wxFontMapper::Get()->CharsetToEncoding(name, false);
        else
            m_enc = wxFONTENCODING_SYSTEM;

        Init();
    }

    wxMBConv_wxwin(wxFontEncoding enc)
    {
        m_enc = enc;

        Init();
    }

    size_t MB2WC(wchar_t *buf, const char *psz, size_t WXUNUSED(n)) const
    {
        size_t inbuf = strlen(psz);
        if (buf)
            m2w.Convert(psz,buf);
        return inbuf;
    }

    size_t WC2MB(char *buf, const wchar_t *psz, size_t WXUNUSED(n)) const
    {
        const size_t inbuf = wxWcslen(psz);
        if (buf)
            w2m.Convert(psz,buf);

        return inbuf;
    }

    bool IsOk() const { return m_ok; }

public:
    wxFontEncoding m_enc;
    wxEncodingConverter m2w, w2m;

    // were we initialized successfully?
    bool m_ok;

    DECLARE_NO_COPY_CLASS(wxMBConv_wxwin)
};

#endif // wxUSE_FONTMAP

// ============================================================================
// wxCSConv implementation
// ============================================================================

void wxCSConv::Init()
{
    m_name = NULL;
    m_convReal =  NULL;
    m_deferred = true;
}

wxCSConv::wxCSConv(const wxChar *charset)
{
    Init();

    if ( charset )
    {
        SetName(charset);
    }

    m_encoding = wxFONTENCODING_SYSTEM;
}

wxCSConv::wxCSConv(wxFontEncoding encoding)
{
    if ( encoding == wxFONTENCODING_MAX || encoding == wxFONTENCODING_DEFAULT )
    {
        wxFAIL_MSG( _T("invalid encoding value in wxCSConv ctor") );

        encoding = wxFONTENCODING_SYSTEM;
    }

    Init();

    m_encoding = encoding;
}

wxCSConv::~wxCSConv()
{
    Clear();
}

wxCSConv::wxCSConv(const wxCSConv& conv)
        : wxMBConv()
{
    Init();

    SetName(conv.m_name);
    m_encoding = conv.m_encoding;
}

wxCSConv& wxCSConv::operator=(const wxCSConv& conv)
{
    Clear();

    SetName(conv.m_name);
    m_encoding = conv.m_encoding;

    return *this;
}

void wxCSConv::Clear()
{
    free(m_name);
    delete m_convReal;

    m_name = NULL;
    m_convReal = NULL;
}

void wxCSConv::SetName(const wxChar *charset)
{
    if (charset)
    {
        m_name = wxStrdup(charset);
        m_deferred = true;
    }
}

wxMBConv *wxCSConv::DoCreate() const
{
    // check for the special case of ASCII or ISO8859-1 charset: as we have
    // special knowledge of it anyhow, we don't need to create a special
    // conversion object
    if ( m_encoding == wxFONTENCODING_ISO8859_1 )
    {
        // don't convert at all
        return NULL;
    }

    // we trust OS to do conversion better than we can so try external
    // conversion methods first
    //
    // the full order is:
    //      1. OS conversion (iconv() under Unix or Win32 API)
    //      2. hard coded conversions for UTF
    //      3. wxEncodingConverter as fall back

    // step (1)
#ifdef HAVE_ICONV
#if !wxUSE_FONTMAP
    if ( m_name )
#endif // !wxUSE_FONTMAP
    {
        wxString name(m_name);

#if wxUSE_FONTMAP
        if ( name.empty() )
            name = wxFontMapper::Get()->GetEncodingName(m_encoding);
#endif // wxUSE_FONTMAP

        wxMBConv_iconv *conv = new wxMBConv_iconv(name);
        if ( conv->IsOk() )
            return conv;

        delete conv;
    }
#endif // HAVE_ICONV

#ifdef wxHAVE_WIN32_MB2WC
    {
#if wxUSE_FONTMAP
        wxMBConv_win32 *conv = m_name ? new wxMBConv_win32(m_name)
                                      : new wxMBConv_win32(m_encoding);
        if ( conv->IsOk() )
            return conv;

        delete conv;
#else
        return NULL;
#endif
    }
#endif // wxHAVE_WIN32_MB2WC
#if defined(__WXMAC__)
    {
        if ( m_name || ( m_encoding < wxFONTENCODING_UTF16BE ) )
        {

            wxMBConv_mac *conv = m_name ? new wxMBConv_mac(m_name)
                                        : new wxMBConv_mac(m_encoding);
            if ( conv->IsOk() )
                 return conv;

            delete conv;
        }
    }
#endif
#if defined(__WXCOCOA__)
    {
        if ( m_name || ( m_encoding <= wxFONTENCODING_UTF16 ) )
        {

            wxMBConv_cocoa *conv = m_name ? new wxMBConv_cocoa(m_name)
                                          : new wxMBConv_cocoa(m_encoding);
            if ( conv->IsOk() )
                 return conv;

            delete conv;
        }
    }
#endif
    // step (2)
    wxFontEncoding enc = m_encoding;
#if wxUSE_FONTMAP
    if ( enc == wxFONTENCODING_SYSTEM && m_name )
    {
        // use "false" to suppress interactive dialogs -- we can be called from
        // anywhere and popping up a dialog from here is the last thing we want to
        // do
        enc = wxFontMapper::Get()->CharsetToEncoding(m_name, false);
    }
#endif // wxUSE_FONTMAP

    switch ( enc )
    {
        case wxFONTENCODING_UTF7:
             return new wxMBConvUTF7;

        case wxFONTENCODING_UTF8:
             return new wxMBConvUTF8;

        case wxFONTENCODING_UTF16BE:
             return new wxMBConvUTF16BE;

        case wxFONTENCODING_UTF16LE:
             return new wxMBConvUTF16LE;

        case wxFONTENCODING_UTF32BE:
             return new wxMBConvUTF32BE;

        case wxFONTENCODING_UTF32LE:
             return new wxMBConvUTF32LE;

        default:
             // nothing to do but put here to suppress gcc warnings
             ;
    }

    // step (3)
#if wxUSE_FONTMAP
    {
        wxMBConv_wxwin *conv = m_name ? new wxMBConv_wxwin(m_name)
                                      : new wxMBConv_wxwin(m_encoding);
        if ( conv->IsOk() )
            return conv;

        delete conv;
    }
#endif // wxUSE_FONTMAP

    // NB: This is a hack to prevent deadlock. What could otherwise happen
    //     in Unicode build: wxConvLocal creation ends up being here
    //     because of some failure and logs the error. But wxLog will try to
    //     attach timestamp, for which it will need wxConvLocal (to convert
    //     time to char* and then wchar_t*), but that fails, tries to log
    //     error, but wxLog has a (already locked) critical section that
    //     guards static buffer.
    static bool alreadyLoggingError = false;
    if (!alreadyLoggingError)
    {
        alreadyLoggingError = true;
        wxLogError(_("Cannot convert from the charset '%s'!"),
                   m_name ? m_name
                      :
#if wxUSE_FONTMAP
                         wxFontMapper::GetEncodingDescription(m_encoding).c_str()
#else // !wxUSE_FONTMAP
                         wxString::Format(_("encoding %s"), m_encoding).c_str()
#endif // wxUSE_FONTMAP/!wxUSE_FONTMAP
              );
        alreadyLoggingError = false;
    }

    return NULL;
}

void wxCSConv::CreateConvIfNeeded() const
{
    if ( m_deferred )
    {
        wxCSConv *self = (wxCSConv *)this; // const_cast

#if wxUSE_INTL
        // if we don't have neither the name nor the encoding, use the default
        // encoding for this system
        if ( !m_name && m_encoding == wxFONTENCODING_SYSTEM )
        {
            self->m_name = wxStrdup(wxLocale::GetSystemEncodingName());
        }
#endif // wxUSE_INTL

        self->m_convReal = DoCreate();
        self->m_deferred = false;
    }
}

size_t wxCSConv::MB2WC(wchar_t *buf, const char *psz, size_t n) const
{
    CreateConvIfNeeded();

    if (m_convReal)
        return m_convReal->MB2WC(buf, psz, n);

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
    CreateConvIfNeeded();

    if (m_convReal)
        return m_convReal->WC2MB(buf, psz, n);

    // latin-1 (direct)
    const size_t len = wxWcslen(psz);
    if (buf)
    {
        for (size_t c = 0; c <= len; c++)
        {
            if (psz[c] > 0xFF)
                return (size_t)-1;
            buf[c] = (char)psz[c];
        }
    }
    else
    {
        for (size_t c = 0; c <= len; c++)
        {
            if (psz[c] > 0xFF)
                return (size_t)-1;
        }
    }

    return len;
}

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

#ifdef __WINDOWS__
    static wxMBConv_win32 wxConvLibcObj;
#elif defined(__WXMAC__) && !defined(__MACH__)
    static wxMBConv_mac wxConvLibcObj ;
#else
    static wxMBConvLibc wxConvLibcObj;
#endif

static wxCSConv wxConvLocalObj(wxFONTENCODING_SYSTEM);
static wxCSConv wxConvISO8859_1Obj(wxFONTENCODING_ISO8859_1);
static wxMBConvUTF7 wxConvUTF7Obj;
static wxMBConvUTF8 wxConvUTF8Obj;


WXDLLIMPEXP_DATA_BASE(wxMBConv&) wxConvLibc = wxConvLibcObj;
WXDLLIMPEXP_DATA_BASE(wxCSConv&) wxConvLocal = wxConvLocalObj;
WXDLLIMPEXP_DATA_BASE(wxCSConv&) wxConvISO8859_1 = wxConvISO8859_1Obj;
WXDLLIMPEXP_DATA_BASE(wxMBConvUTF7&) wxConvUTF7 = wxConvUTF7Obj;
WXDLLIMPEXP_DATA_BASE(wxMBConvUTF8&) wxConvUTF8 = wxConvUTF8Obj;
WXDLLIMPEXP_DATA_BASE(wxMBConv *) wxConvCurrent = &wxConvLibcObj;

#else // !wxUSE_WCHAR_T

// stand-ins in absence of wchar_t
WXDLLIMPEXP_DATA_BASE(wxMBConv) wxConvLibc,
                                wxConvISO8859_1,
                                wxConvLocal,
                                wxConvUTF8;

#endif // wxUSE_WCHAR_T/!wxUSE_WCHAR_T


