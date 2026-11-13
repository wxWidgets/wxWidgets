/////////////////////////////////////////////////////////////////////////////
// Name:        src/retro68/fontenum.cpp
// Purpose:     wxFontEnumerator class
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTENUM

#include "wx/fontenum.h"
#include "wx/font.h"
#include "wx/arrstr.h"

#include "wx/retro68/private/qd.h"

// Implemented in src/retro68/dc.cpp.
extern void wxRetro68GetTextExtent(const wxFont& font,
                                   const wxString& text,
                                   wxCoord *x, wxCoord *y,
                                   wxCoord *descent,
                                   wxCoord *externalLeading);

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding WXUNUSED(encoding),
                                          bool fixedWidthOnly)
{
    // All the installed font families have a 'FOND' resource in the System
    // file (or in the Fonts folder files, which are in the resource chain).
    wxArrayString names;

    SetResLoad(false);

    const short count = CountResources('FOND');
    for ( short i = 1; i <= count; i++ )
    {
        Handle h = GetIndResource('FOND', i);
        if ( !h )
            continue;

        short id;
        ResType type;
        Str255 name;
        GetResInfo(h, &id, &type, name);

        if ( name[0] && name[1] != '.' && name[1] != '%' )
        {
            const wxString facename = wxMacMakeStringFromPascal(name);
            if ( names.Index(facename) == wxNOT_FOUND )
                names.push_back(facename);
        }
    }

    SetResLoad(true);

    names.Sort();

    for ( const wxString& facename : names )
    {
        if ( fixedWidthOnly )
        {
            const wxFont font(wxFontInfo(12).FaceName(facename));
            if ( !font.IsFixedWidth() )
                continue;
        }

        if ( !OnFacename(facename) )
            break;
    }

    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& facename)
{
    return EnumerateEncodingsUTF8(facename);
}

#endif // wxUSE_FONTENUM
