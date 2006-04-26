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
#endif

#include "wx/gdicmn.h"


// ============================================================================
// wxString <-> wxColour conversions
// ============================================================================

bool wxColourBase::FromString(const wxChar *str)
{
    if ( str == NULL || str[0] == wxT('\0'))
        return false;       // invalid or empty string

    if ( wxStrncmp(str, wxT("RGB"), 3) == 0 ||
         wxStrncmp(str, wxT("rgb"), 3) == 0 )
    {
        // RGB specification CSS-like
        int red, green, blue;
        if (wxSscanf(&str[3], wxT("(%d, %d, %d)"), &red, &green, &blue) != 3)
            return false;

        Set((unsigned char)red, (unsigned char)green, (unsigned char)blue);
    }
    else if ( str[0] == wxT('#') && wxStrlen(str) == 7 )
    {
        // hexadecimal prefixed with # (HTML syntax)
        unsigned long tmp;
        if (wxSscanf(&str[1], wxT("%lX"), &tmp) != 1)
            return false;

        Set(tmp);   // set from packed long
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

    if (flags & wxC2S_NAME)
        colName = wxTheColourDatabase->FindName((const wxColour &)(*this)).MakeLower();

    if ( colName.empty() && (flags & wxC2S_CSS_SYNTAX) )
    {
        // no name for this colour; return it in CSS syntax
        colName.Printf(wxT("rgb(%d, %d, %d)"),
                       Red(), Green(), Blue());
    }
    else if ( colName.empty() && (flags & wxC2S_HTML_SYNTAX) )
    {
        // no name for this colour; return it in HTML syntax
        colName.Printf(wxT("#%02X%02X%02X"),
                       Red(), Green(), Blue());
    }

    // this function always returns a non-empty string
    wxASSERT_MSG(!colName.empty(),
                 wxT("Invalid wxColour -> wxString conversion flags"));

    return colName;
}

wxColour wxColourBase::CreateByName(const wxString& name)
{
    return wxColour(name);
}
