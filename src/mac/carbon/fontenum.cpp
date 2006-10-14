///////////////////////////////////////////////////////////////////////////////
// Name:        src/mac/carbon/fontenum.cpp
// Purpose:     wxFontEnumerator class for MacOS
// Author:      Stefan Csomor
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTMAP

#include "wx/fontenum.h"

#ifndef WX_PRECOMP
    #include "wx/font.h"
    #include "wx/intl.h"
#endif

#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/encinfo.h"

#include "wx/mac/private.h"

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    //
    // From Apple's QA 1471 http://developer.apple.com/qa/qa2006/qa1471.html
    //
    
    ATSFontFamilyIterator theFontFamilyIterator = NULL;
    ATSFontFamilyRef theATSFontFamilyRef = 0;
    OSStatus status = noErr;
    
    wxArrayString fontFamilies ;
    
    // Create the iterator
    status = ATSFontFamilyIteratorCreate(kATSFontContextLocal, nil,nil,
                                         kATSOptionFlagsUnRestrictedScope,
                                         &theFontFamilyIterator );
    
    wxUint32 macEncoding = wxMacGetSystemEncFromFontEnc(encoding) ;
    
    while (status == noErr)
    {
        // Get the next font in the iteration.
        status = ATSFontFamilyIteratorNext( theFontFamilyIterator, &theATSFontFamilyRef );
        if(status == noErr)
        {
 #ifndef __LP64__
            // TODO CS : Find replacement
            // added CS : avoid showing fonts that won't be displayable
            FMFontStyle intrinsicStyle = 0 ;
            FMFont fontInstance ;
            FMFontFamily fmFamily = FMGetFontFamilyFromATSFontFamilyRef( theATSFontFamilyRef );
            status = FMGetFontFromFontFamilyInstance( fmFamily , 0 , &fontInstance , &intrinsicStyle);
            if ( status != noErr )
            {
                status = noErr;
                continue ;
            }
#endif
            if ( encoding != wxFONTENCODING_SYSTEM )
            {
                TextEncoding fontFamiliyEncoding = ATSFontFamilyGetEncoding(theATSFontFamilyRef) ;
                if ( fontFamiliyEncoding != macEncoding )
                    continue ;
            }
            
            // TODO: determine fixed widths ...

            CFStringRef theName = NULL;
            ATSFontFamilyGetName(theATSFontFamilyRef, kATSOptionFlagsDefault, &theName);
            wxMacCFStringHolder cfName(theName) ;
            fontFamilies.Add(cfName.AsString(wxLocale::GetSystemEncoding()));
        }
        else if (status == kATSIterationScopeModified) // Make sure the font database hasn’t changed.
        {
            // reset the iterator
            status = ATSFontFamilyIteratorReset (kATSFontContextLocal, nil, nil,
                                                 kATSOptionFlagsUnRestrictedScope,
                                                 &theFontFamilyIterator);
            fontFamilies.Clear() ;
        }
    }
    ATSFontFamilyIteratorRelease(&theFontFamilyIterator);
    
    for ( size_t i = 0 ; i < fontFamilies.Count() ; ++i )
    {
        if ( OnFacename( fontFamilies[i] ) == false )
            break ;
    }
    
    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    wxFAIL_MSG(wxT("wxFontEnumerator::EnumerateEncodings() not yet implemented"));

    return true;
}

#endif // wxUSE_FONTMAP
