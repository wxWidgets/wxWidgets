/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/colourcmn.cpp
// Purpose:     wxColourBase implementation
// Author:      Francesco Montorsi
// Modified by:
// Created:     20/4/2006
// RCS-ID:      $Id$
// Copyright:   (c) Francesco Montorsi
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/colour.h"

#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/gdicmn.h"
    #include "wx/wxcrtvararg.h"
#endif

#if wxUSE_VARIANT
IMPLEMENT_VARIANT_OBJECT_EXPORTED(wxColour,WXDLLEXPORT)
#endif

// ============================================================================
// wxString <-> wxColour conversions
// ============================================================================

bool wxColourBase::FromString(const wxString& str)
{
    if ( str.empty() )
        return false;       // invalid or empty string

    if ( wxStrnicmp(str, wxT("RGB"), 3) == 0 )
    {
        // CSS-like RGB specification
        // according to http://www.w3.org/TR/css3-color/#colorunits
        // values outside 0-255 range are allowed but should be clipped
        int red, green, blue,
            alpha = wxALPHA_OPAQUE;
        if ( str.length() > 3 && (str[3] == wxT('a') || str[3] == wxT('A')) )
        {
            float a;
            // TODO: use locale-independent function
            if ( wxSscanf(str.wx_str() + 4, wxT("( %d , %d , %d , %f )"),
                                                &red, &green, &blue, &a) != 4 )
                return false;

            alpha = wxRound(a * 255);
        }
        else // no 'a' following "rgb"
        {
            if ( wxSscanf(str.wx_str() + 3, wxT("( %d , %d , %d )"),
                                                &red, &green, &blue) != 3 )
                return false;
        }

        Set((unsigned char)wxClip(red, 0, 255),
            (unsigned char)wxClip(green, 0, 255),
            (unsigned char)wxClip(blue, 0, 255),
            (unsigned char)wxClip(alpha, 0, 255));
    }
    else if ( str[0] == wxT('#') && wxStrlen(str) == 7 )
    {
        // hexadecimal prefixed with # (HTML syntax)
        unsigned long tmp;
        if (wxSscanf(str.wx_str() + 1, wxT("%lx"), &tmp) != 1)
            return false;

        Set((unsigned char)(tmp >> 16),
            (unsigned char)(tmp >> 8),
            (unsigned char)tmp);
    }
    else if (wxTheColourDatabase) // a colour name ?
    {
        // we can't do
        // *this = wxTheColourDatabase->Find(str)
        // because this place can be called from constructor
        // and 'this' could not be available yet
        wxColour clr = wxTheColourDatabase->Find(str);
        if (clr.Ok())
            Set((unsigned char)clr.Red(),
                (unsigned char)clr.Green(),
                (unsigned char)clr.Blue());
    }

    if (Ok())
        return true;

    wxLogDebug(wxT("wxColour::Set - couldn't set to colour string '%s'"), str);
    return false;
}

wxString wxColourBase::GetAsString(long flags) const
{
    wxString colName;

    const bool isOpaque = Alpha() == wxALPHA_OPAQUE;

    // we can't use the name format if the colour is not opaque as the alpha
    // information would be lost
    if ( (flags & wxC2S_NAME) && isOpaque )
    {
        colName = wxTheColourDatabase->FindName(
                    static_cast<const wxColour &>(*this)).MakeLower();
    }

    if ( colName.empty() )
    {
        const int red = Red(),
                  blue = Blue(),
                  green = Green();

        if ( flags & wxC2S_CSS_SYNTAX )
        {
            // no name for this colour; return it in CSS syntax
            if ( isOpaque )
            {
                colName.Printf(wxT("rgb(%d, %d, %d)"), red, green, blue);
            }
            else // use rgba() form
            {
                // TODO: use locale-independent function
                colName.Printf(wxT("rgba(%d, %d, %d, %.3f)"),
                               red, green, blue, Alpha() / 255.);
            }
        }
        else if ( flags & wxC2S_HTML_SYNTAX )
        {
            wxASSERT_MSG( isOpaque, "alpha is lost in HTML syntax" );

            // no name for this colour; return it in HTML syntax
            colName.Printf(wxT("#%02X%02X%02X"), red, green, blue);
        }
    }

    // this function should alway returns a non-empty string
    wxASSERT_MSG(!colName.empty(),
                 wxT("Invalid wxColour -> wxString conversion flags"));

    return colName;
}

#if WXWIN_COMPATIBILITY_2_6

// static
wxColour wxColourBase::CreateByName(const wxString& name)
{
    return wxColour(name);
}

void wxColourBase::InitFromName(const wxString& col)
{
    Set(col);
}

#endif // WXWIN_COMPATIBILITY_2_6

// wxColour <-> wxString utilities, used by wxConfig
wxString wxToString(const wxColourBase& col)
{
    return col.IsOk() ? col.GetAsString(wxC2S_CSS_SYNTAX)
                      : wxString();
}

bool wxFromString(const wxString& str, wxColourBase *col)
{
    wxCHECK_MSG( col, false, _T("NULL output parameter") );

    if ( str.empty() )
    {
        *col = wxNullColour;
        return true;
    }

    return col->Set(str);
}


