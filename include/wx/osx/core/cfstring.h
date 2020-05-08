/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/core/cfstring.h
// Purpose:     wxCFStringRef and other string functions
// Author:      Stefan Csomor
// Modified by:
// Created:     2004-10-29 (from code in wx/mac/carbon/private.h)
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
// Usage:       Darwin (base library)
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_CFSTRINGHOLDER_H__
#define __WX_CFSTRINGHOLDER_H__

#include "wx/dlimpexp.h"
#include "wx/fontenc.h"
#include "wx/osx/core/cfref.h"

#ifdef WORDS_BIGENDIAN
    #define kCFStringEncodingUTF32Native kCFStringEncodingUTF32BE
#else
    #define kCFStringEncodingUTF32Native kCFStringEncodingUTF32LE
#endif

class WXDLLIMPEXP_FWD_BASE wxString;

WXDLLIMPEXP_BASE wxString wxMacConvertNewlines13To10(const wxString& data);
WXDLLIMPEXP_BASE wxString wxMacConvertNewlines10To13(const wxString& data);

WXDLLIMPEXP_BASE wxUint32 wxMacGetSystemEncFromFontEnc(wxFontEncoding encoding) ;
WXDLLIMPEXP_BASE wxFontEncoding wxMacGetFontEncFromSystemEnc(wxUint32 encoding) ;
WXDLLIMPEXP_BASE void wxMacWakeUp() ;

class WXDLLIMPEXP_BASE wxCFStringRef : public wxCFRef< CFStringRef >
{
public:
    wxCFStringRef()
    {
    }

    wxCFStringRef(const wxString &str,
                        wxFontEncoding encoding = wxFONTENCODING_DEFAULT) ;

#ifdef __OBJC__
    wxCFStringRef(WX_NSString ref)
        : wxCFRef< CFStringRef >((WX_OSX_BRIDGE_RETAINED CFStringRef) ref)
    {
    }
#endif

    wxCFStringRef(CFStringRef ref)
        : wxCFRef< CFStringRef >(ref)
    {
    }

    wxCFStringRef(const wxCFStringRef& otherRef )
        : wxCFRef< CFStringRef >(otherRef)
    {
    }

    ~wxCFStringRef()
    {
    }

    wxString AsString( wxFontEncoding encoding = wxFONTENCODING_DEFAULT ) const;

    static wxString AsString( CFStringRef ref, wxFontEncoding encoding = wxFONTENCODING_DEFAULT ) ;
    static wxString AsStringWithNormalizationFormC( CFStringRef ref, wxFontEncoding encoding = wxFONTENCODING_DEFAULT ) ;
#ifdef __WXMAC__
    static wxString AsString( WX_NSString ref, wxFontEncoding encoding = wxFONTENCODING_DEFAULT ) ;
    static wxString AsStringWithNormalizationFormC( WX_NSString ref, wxFontEncoding encoding = wxFONTENCODING_DEFAULT ) ;
#endif
#ifdef __OBJC__
    WX_NSString AsNSString() const { return (WX_OSX_BRIDGE WX_NSString)(CFStringRef) *this; }
#endif
private:
} ;

#endif //__WXCFSTRINGHOLDER_H__
