///////////////////////////////////////////////////////////////////////////////
// Name:        mac/fontenum.cpp
// Purpose:     wxFontEnumerator class for MacOS
// Author:      Stefan Csomor
// Modified by: 
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "fontenum.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
  #pragma hdrstop
#endif

#ifndef WX_PRECOMP
  #include "wx/font.h"
#endif

#include "wx/fontenum.h"
#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/fontutil.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

class wxFontEnumeratorHelper
{
public:
    wxFontEnumeratorHelper(wxFontEnumerator *fontEnum);

    // control what exactly are we enumerating
    bool SetEncoding(wxFontEncoding encoding);
    void SetFixedOnly(bool fixedOnly)
        { m_fixedOnly = fixedOnly; }

    // call to start enumeration
    void DoEnumerate();

private:
    // the object we forward calls to OnFont() to
    wxFontEnumerator *m_fontEnum;

    // if != -1, enum only fonts which have this encoding
    int m_charset;

    // if not empty, enum only the fonts with this facename
    wxString m_facename;

    // if TRUE, enum only fixed fonts
    bool m_fixedOnly;
};
// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontEnumeratorHelper
// ----------------------------------------------------------------------------

wxFontEnumeratorHelper::wxFontEnumeratorHelper(wxFontEnumerator *fontEnum)
{
    m_fontEnum = fontEnum;
    m_charset = -1;
    m_fixedOnly = FALSE;
}

bool wxFontEnumeratorHelper::SetEncoding(wxFontEncoding encoding)
{
    wxNativeEncodingInfo info;
    if ( !wxGetNativeFontEncoding(encoding, &info) )
    {
        if ( !wxTheFontMapper->GetAltForEncoding(encoding, &info) )
        {
            // no such encodings at all
            return FALSE;
        }
    }
    m_charset = info.charset;
    m_facename = info.facename;

    return TRUE;
}

void wxFontEnumeratorHelper::DoEnumerate()
{
	MenuHandle	menu ;
	Str255		p_name ;
	char        c_name[256] ;
	short 		lines ;
	
	menu = NewMenu( 32000 , "\pFont" )  ;
	AppendResMenu( menu , 'FONT' ) ;
	lines = CountMenuItems( menu ) ;

	for ( int i = 1 ; i < lines+1  ; i ++ )
	{
	    GetMenuItemText( menu , i , p_name ) ;
#if TARGET_CARBON
	    p2cstrcpy( c_name, p_name ) ;
#else
		p2cstr( p_name ) ;
	    strcpy( c_name, (char *)p_name ) ;
#endif
	    /*
	      
	      if ( m_fixedOnly )
	    {
	        // check that it's a fixed pitch font (there is *no* error here, the
	        // flag name is misleading!)
	        if ( tm->tmPitchAndFamily & TMPF_FIXED_PITCH )
	        {
	            // not a fixed pitch font
	            return TRUE;
	        }
	    }
	
	    if ( m_charset != -1 )
	    {
	        // check that we have the right encoding
	        if ( lf->lfCharSet != m_charset )
	        {
	            return TRUE;
	        }
	    }
	
		*/
		m_fontEnum->OnFacename( c_name ) ;
	}
	DisposeMenu( menu ) ;
}

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    wxFontEnumeratorHelper fe(this);
    if ( fe.SetEncoding(encoding) )
    {
        fe.SetFixedOnly(fixedWidthOnly);

        fe.DoEnumerate();
    }
    // else: no such fonts, unknown encoding

    return TRUE;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    wxFAIL_MSG(wxT("TODO"));

    return TRUE;
}
