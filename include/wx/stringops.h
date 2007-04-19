///////////////////////////////////////////////////////////////////////////////
// Name:        wx/stringops.h
// Purpose:     implementation of wxString primitive operations
// Author:      Vaclav Slavik
// Modified by:
// Created:     2007-04-16
// RCS-ID:      $Id$
// Copyright:   (c) 2007 REA Elektronik GmbH
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXSTRINGOPS_H__
#define _WX_WXSTRINGOPS_H__

#include "wx/chartype.h"
#include "wx/stringimpl.h"
#include "wx/unichar.h"

// This header contains wxStringOperations "namespace" class that implements
// elementary operations on string data as static methods; wxString methods and
// iterators are implemented in terms of it. Two implementations are available,
// one for UTF-8 encoded char* string and one for "raw" wchar_t* strings (or
// char* in ANSI build).

// FIXME-UTF8: only wchar after we remove ANSI build
#if wxUSE_UNICODE_WCHAR || !wxUSE_UNICODE
struct WXDLLIMPEXP_BASE wxStringOperationsWchar
{
    // moves the iterator to the next Unicode character
    static void IncIter(wxStringImpl::iterator& i) { ++i; }
    static void IncIter(wxStringImpl::const_iterator& i) { ++i; }

    // moves the iterator to the previous Unicode character
    static void DecIter(wxStringImpl::iterator& i) { --i; }
    static void DecIter(wxStringImpl::const_iterator& i) { --i; }

    // moves the iterator by n Unicode characters
    static wxStringImpl::iterator AddToIter(const wxStringImpl::iterator& i, int n)
        { return i + n; }
    static wxStringImpl::const_iterator AddToIter(const wxStringImpl::const_iterator& i, int n)
        { return i + n; }

    // returns distance of the two iterators in Unicode characters
    static int DiffIters(const wxStringImpl::iterator& i1,
                         const wxStringImpl::iterator& i2)
        { return i1 - i2; }
    static int DiffIters(const wxStringImpl::const_iterator& i1,
                         const wxStringImpl::const_iterator& i2)
        { return i1 - i2; }

    // encodes the character to a form used to represent it in internal
    // representation (returns a string in UTF8 version)
    static wxChar EncodeChar(const wxUniChar& ch) { return (wxChar)ch; }

    static wxUniChar DecodeChar(const wxStringImpl::const_iterator& i)
        { return *i; }
};
#endif // wxUSE_UNICODE_WCHAR || !wxUSE_UNICODE


#if wxUSE_UNICODE_UTF8
struct WXDLLIMPEXP_BASE wxStringOperationsUtf8
{
    // checks correctness of UTF-8 sequence
    static bool IsValidUtf8String(const char *c);
#ifdef __WXDEBUG__
    static bool IsValidUtf8LeadByte(unsigned char c);
#endif

    // table of offsets to skip forward when iterating over UTF-8 sequence
    static unsigned char ms_utf8IterTable[256];


    template<typename Iterator>
    static void IncIter(Iterator& i)
    {
        wxASSERT( IsValidUtf8LeadByte(*i) );
        i += ms_utf8IterTable[(unsigned char)*i];
    }

    template<typename Iterator>
    static void DecIter(Iterator& i)
    {
        wxASSERT( IsValidUtf8LeadByte(*i) );

        // Non-lead bytes are all in the 0x80..0xBF range (i.e. 10xxxxxx in
        // binary), so we just have to go back until we hit a byte that is
        // either < 0x80 (i.e. 0xxxxxxx in binary) or 0xC0..0xFF (11xxxxxx in
        // binary; this includes some invalid values, but we can ignore it
        // here, because we assume valid UTF-8 input for the purpose of
        // efficient implementation).
        --i;
        while ( ((*i) & 0xC0) == 0x80 /* 2 highest bits are '10' */ )
            --i;
    }

    template<typename Iterator>
    static Iterator AddToIter(const Iterator& i, int n)
    {
        Iterator out(i);

        if ( n > 0 )
        {
            for ( int j = 0; j < n; ++j )
                IncIter(out);
        }
        else if ( n < 0 )
        {
            for ( int j = 0; j > n; --j )
                DecIter(out);
        }

        return out;
    }

    template<typename Iterator>
    static int DiffIters(Iterator i1, Iterator i2)
    {
        int dist = 0;

        if ( i1 < i2 )
        {
            while ( i1 != i2 )
            {
                IncIter(i1);
                dist--;
            }
        }
        else if ( i2 < i1 )
        {
            while ( i2 != i1 )
            {
                IncIter(i2);
                dist++;
            }
        }

        return dist;
    }

    // buffer for single UTF-8 character
    struct Utf8CharBuffer
    {
        char data[5];
        operator const char*() const { return data; }
    };

    // encodes the character as UTF-8:
    static Utf8CharBuffer EncodeChar(const wxUniChar& ch);

    // returns n copies of ch encoded in UTF-8 string
    static wxCharBuffer EncodeNChars(size_t n, const wxUniChar& ch);

    // returns the length of UTF-8 encoding of the character with lead byte 'c'
    static size_t GetUtf8CharLength(char c)
    {
        wxASSERT( IsValidUtf8LeadByte(c) );
        return ms_utf8IterTable[(unsigned char)c];
    }

    // decodes single UTF-8 character from UTF-8 string
    static wxUniChar DecodeChar(wxStringImpl::const_iterator i);
};
#endif // wxUSE_UNICODE_UTF8


#if wxUSE_UNICODE_UTF8
typedef wxStringOperationsUtf8 wxStringOperations;
#else
typedef wxStringOperationsWchar wxStringOperations;
#endif

#endif  // _WX_WXSTRINGOPS_H_
