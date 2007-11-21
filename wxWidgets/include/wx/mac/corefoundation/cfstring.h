/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/corefoundation/cfstring.h
// Purpose:     wxMacCFStringHolder and other string functions
// Author:      Stefan Csomor
// Modified by:
// Created:     2004-10-29 (from code in wx/mac/carbon/private.h)
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_CFSTRINGHOLDER_H__
#define __WX_CFSTRINGHOLDER_H__

#ifdef __DARWIN__
    #include <CoreFoundation/CFString.h>
#else
    #include <CFString.h>
#endif

#include "wx/fontenc.h"

class WXDLLIMPEXP_BASE wxString;

void wxMacConvertNewlines13To10( char * data ) ;
void wxMacConvertNewlines10To13( char * data ) ;
void wxMacConvertNewlines13To10( wxString *data ) ;
void wxMacConvertNewlines10To13( wxString *data ) ;

#if wxUSE_UNICODE
void wxMacConvertNewlines13To10( wxChar * data ) ;
void wxMacConvertNewlines10To13( wxChar * data ) ;
#endif

wxUint32 wxMacGetSystemEncFromFontEnc(wxFontEncoding encoding) ;
wxFontEncoding wxMacGetFontEncFromSystemEnc(wxUint32 encoding) ;
void wxMacWakeUp() ;

class wxMacCFStringHolder
{
public:
    wxMacCFStringHolder()
        : m_cfs(NULL) , m_release(false)
    {
    }

    wxMacCFStringHolder(const wxString &str,
                        wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
        : m_cfs(NULL) , m_release(false)
    {
        Assign( str , encoding ) ;
    }

    wxMacCFStringHolder(CFStringRef ref , bool release = true )
        : m_cfs(ref) , m_release(release)
    {
    }

    ~wxMacCFStringHolder()
    {
        Release() ;
    }

    CFStringRef Detach()
    {
        CFStringRef retval = m_cfs ;
        m_release = false ;
        m_cfs = NULL ;
        return retval ;
    }

    void Release()
    {
        if ( m_release && m_cfs)
            CFRelease( m_cfs ) ;
        m_cfs = NULL ;
    }

    void Assign(const wxString &str,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    operator CFStringRef () const { return m_cfs; }
    wxString AsString( wxFontEncoding encoding = wxFONTENCODING_DEFAULT ) ;

private:

    CFStringRef m_cfs;
    bool m_release ;

    DECLARE_NO_COPY_CLASS( wxMacCFStringHolder )
} ;

// corresponding class for holding UniChars (native unicode characters)

class wxMacUniCharBuffer
{
public :
    wxMacUniCharBuffer( const wxString &str ) ;

    ~wxMacUniCharBuffer() ;

    UniChar* GetBuffer() ;

    UniCharCount GetChars() ;

private :
    UniChar* m_ubuf ;
    UniCharCount m_chars ;
};
#endif //__WXCFSTRINGHOLDER_H__
