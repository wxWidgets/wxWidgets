/////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/core/strconv_cf.cpp
// Purpose:     Unicode conversion classes
// Author:      David Elliott
// Modified by:
// Created:     2007-07-06
// Copyright:   (c) 2007 David Elliott
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/string.h"
#endif

#include "wx/strconv.h"
#include "wx/fontmap.h"

#ifdef __DARWIN__

#include "wx/osx/core/private/strconv_cf.h"
#include "wx/osx/core/cfref.h"


// ============================================================================
// CoreFoundation conversion classes
// ============================================================================

/* Provide factory functions for unit tests.  Not in any header.  Do not
 * assume ABI compatibility even within a given wxWidgets release.
 */

#if wxUSE_FONTMAP
WXDLLIMPEXP_BASE wxMBConv* new_wxMBConv_cf( const char* name)
{
    wxMBConv_cf *result = new wxMBConv_cf(name);
    if(!result->IsOk())
    {
        delete result;
        return nullptr;
    }
    else
        return result;
}
#endif // wxUSE_FONTMAP

WXDLLIMPEXP_BASE wxMBConv* new_wxMBConv_cf(wxFontEncoding encoding)
{
    wxMBConv_cf *result = new wxMBConv_cf(encoding);
    if(!result->IsOk())
    {
        delete result;
        return nullptr;
    }
    else
        return result;
}

// Provide a constant for the wchat_t encoding used by the host platform.
#ifdef WORDS_BIGENDIAN
    static const CFStringEncoding wxCFStringEncodingWcharT = kCFStringEncodingUTF32BE;
#else
    static const CFStringEncoding wxCFStringEncodingWcharT = kCFStringEncodingUTF32LE;
#endif

    size_t wxMBConv_cf::ToWChar(wchar_t * dst, size_t dstSize, const char * src, size_t srcSize) const
    {
        wxCHECK(src, wxCONV_FAILED);

        /* NOTE: This is wrong if the source encoding has an element size
         * other than char (e.g. it's kCFStringEncodingUnicode)
         * If the user specifies it, it's presumably right though.
         * Right now we don't support UTF-16 in anyway since wx can do a better job.
         */
        if(srcSize == wxNO_LEN)
            srcSize = strlen(src) + 1;

        // First create the temporary CFString
        wxCFRef<CFStringRef> theString( CFStringCreateWithBytes (
                                                nullptr, //the allocator
                                                (const UInt8*)src,
                                                srcSize,
                                                m_encoding,
                                                false //no BOM/external representation
                                                ));

        if ( theString == nullptr )
            return wxCONV_FAILED;

        if ( m_normalization & ToWChar_C )
        {
            // Ensure that the string is in canonical composed form (NFC): this is
            // important because Darwin uses decomposed form (NFD) for e.g. file
            // names but we want to use NFC internally.
            wxCFRef<CFMutableStringRef>
            cfMutableString(CFStringCreateMutableCopy(nullptr, 0, theString));
            CFStringNormalize(cfMutableString, kCFStringNormalizationFormC);
            theString = cfMutableString;
        }

        /* NOTE: The string content includes the null element if the source string did
         * That means we have to do nothing special because the destination will have
         * the null element iff the source did and the null element will be included
         * in the count iff it was included in the source count.
         */

            CFRange fullStringRange = CFRangeMake(0, CFStringGetLength(theString));
            CFIndex usedBufLen;

            CFIndex charsConverted = CFStringGetBytes(
                    theString,
                    fullStringRange,
                    wxCFStringEncodingWcharT,
                    0,
                    false,
                    // if dstSize is 0 then pass nullptr to get required length in usedBufLen
                    dstSize != 0?(UInt8*)dst:nullptr,
                    dstSize * sizeof(wchar_t),
                    &usedBufLen);

            if(charsConverted < CFStringGetLength(theString))
                return wxCONV_FAILED;

            /* usedBufLen is the number of bytes written, so we divide by
             * sizeof(wchar_t) to get the number of elements written.
             */
            wxASSERT( (usedBufLen % sizeof(wchar_t)) == 0 );

            // CFStringGetBytes does exactly the right thing when buffer
            // pointer is null and returns the number of bytes required
            return usedBufLen / sizeof(wchar_t);

    }

    size_t wxMBConv_cf::FromWChar(char *dst, size_t dstSize, const wchar_t *src, size_t srcSize) const
    {
        wxCHECK(src, wxCONV_FAILED);

        if(srcSize == wxNO_LEN)
            srcSize = wxStrlen(src) + 1;

        // Temporary CFString
        wxCFRef<CFStringRef> theString;

            theString = wxCFRef<CFStringRef>(CFStringCreateWithBytes(
                    kCFAllocatorDefault,
                    reinterpret_cast<const UInt8*>(src),
                    srcSize * sizeof(wchar_t),
                    wxCFStringEncodingWcharT,
                    false));

        wxCHECK(theString != nullptr, wxCONV_FAILED);

        if ( m_normalization & FromWChar_D )
        {
            wxCFRef<CFMutableStringRef> normalizedFormD = CFStringCreateMutableCopy(kCFAllocatorDefault,0,theString);
            CFStringNormalize(normalizedFormD, kCFStringNormalizationFormD);
            theString = normalizedFormD;
        }

        CFIndex usedBufLen;
        CFIndex charsConverted = CFStringGetBytes(
                theString,
                CFRangeMake(0, CFStringGetLength(theString)),
                m_encoding,
                0, // FAIL on unconvertible characters
                false, // not an external representation
                (UInt8*)dst,
                dstSize,
                &usedBufLen
            );

        // when dst is non-null, we check usedBufLen against dstSize as
        // CFStringGetBytes sometimes treats dst as being null when dstSize==0
        if( (charsConverted < CFStringGetLength(theString)) ||
                (dst && (size_t) usedBufLen > dstSize) )
            return wxCONV_FAILED;

        return usedBufLen;
    }

//
// wxMacUniCharBuffer
//

wxMacUniCharBuffer::wxMacUniCharBuffer( const wxString &str )
{
    wxMBConvUTF16 converter ;
    size_t unicharlen = converter.WC2MB( nullptr , str.wc_str() , 0 ) ;
    m_ubuf = (UniChar*) malloc( unicharlen + 2 ) ;
    converter.WC2MB( (char*) m_ubuf , str.wc_str(), unicharlen + 2 ) ;
    m_chars = unicharlen / 2 ;
}

wxMacUniCharBuffer::~wxMacUniCharBuffer()
{
    free( m_ubuf ) ;
}

UniCharPtr wxMacUniCharBuffer::GetBuffer()
{
    return m_ubuf ;
}

UniCharCount wxMacUniCharBuffer::GetChars()
{
    return m_chars ;
}

#endif // __DARWIN__


