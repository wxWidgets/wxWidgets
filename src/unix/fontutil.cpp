/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/fontutil.cpp
// Purpose:     Font helper functions for wxX11, wxGTK
// Author:      Vadim Zeitlin
// Created:     05.11.99
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#include "wx/fontutil.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/font.h" // wxFont enums
    #include "wx/utils.h"       // for wxGetDisplay()
    #include "wx/module.h"
#endif // PCH

#include "wx/encinfo.h"
#include "wx/fontmap.h"
#include "wx/math.h"
#include "wx/tokenzr.h"
#include "wx/fontenum.h"

#include <pango/pango.h>

PangoContext* wxGetPangoContext();

#ifdef __WXGTK__
    #include "wx/gtk/private.h"
#else
    #include "wx/x11/private.h"
    #include "wx/gtk/private/string.h"
#endif

#define wxPANGO_CONV(s) s.utf8_str()
#define wxPANGO_CONV_BACK(s) wxString::FromUTF8Unchecked(s)

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

void wxNativeFontInfo::Init()
{
    description = nullptr;
    m_underlined = false;
    m_strikethrough = false;
}

void wxNativeFontInfo::Init(const wxNativeFontInfo& info)
{
    if (info.description)
    {
        description = pango_font_description_copy(info.description);
        m_underlined = info.GetUnderlined();
        m_strikethrough = info.GetStrikethrough();
    }
    else
    {
        description = nullptr;
        m_underlined = false;
        m_strikethrough = false;
    }
}

void wxNativeFontInfo::Free()
{
    if (description)
        pango_font_description_free(description);
}

double wxNativeFontInfo::GetFractionalPointSize() const
{
    return double(pango_font_description_get_size(description)) / PANGO_SCALE;
}

wxFontStyle wxNativeFontInfo::GetStyle() const
{
    wxFontStyle m_style = wxFONTSTYLE_NORMAL;

    switch (pango_font_description_get_style( description ))
    {
        case PANGO_STYLE_NORMAL:
            m_style = wxFONTSTYLE_NORMAL;
            break;
        case PANGO_STYLE_ITALIC:
            m_style = wxFONTSTYLE_ITALIC;
            break;
        case PANGO_STYLE_OBLIQUE:
            m_style = wxFONTSTYLE_SLANT;
            break;
    }

    return m_style;
}

int wxNativeFontInfo::GetNumericWeight() const
{
    // We seem to currently initialize only by string.
    // In that case PANGO_FONT_MASK_WEIGHT is always set.
    // if (!(pango_font_description_get_set_fields(description) & PANGO_FONT_MASK_WEIGHT))
    //    return wxFONTWEIGHT_NORMAL;

    PangoWeight pango_weight = pango_font_description_get_weight( description );
    return pango_weight;
}

bool wxNativeFontInfo::GetUnderlined() const
{
    return m_underlined;
}

bool wxNativeFontInfo::GetStrikethrough() const
{
    return m_strikethrough;
}

wxString wxNativeFontInfo::GetFaceName() const
{
    // the Pango "family" is the wx "face name"
    return wxPANGO_CONV_BACK(pango_font_description_get_family(description));
}

wxFontFamily wxNativeFontInfo::GetFamily() const
{
    wxFontFamily ret = wxFONTFAMILY_UNKNOWN;

    const char *family_name = pango_font_description_get_family( description );

    // note: not passing -1 as the 2nd parameter to g_ascii_strdown to work
    // around a bug in the 64-bit glib shipped with solaris 10, -1 causes it
    // to try to allocate 2^32 bytes.
    if ( !family_name )
        return ret;
    wxGtkString family_text(g_ascii_strdown(family_name, strlen(family_name)));

    // Check for some common fonts, to salvage what we can from the current
    // win32 centric wxFont API:
    if (wxStrnicmp( family_text, "monospace", 9 ) == 0)
        ret = wxFONTFAMILY_TELETYPE;    // begins with "Monospace"
    else if (wxStrnicmp( family_text, "courier", 7 ) == 0)
        ret = wxFONTFAMILY_TELETYPE;    // begins with "Courier"
#if defined(__WXGTK__) || defined(HAVE_PANGO_FONT_FAMILY_IS_MONOSPACE)
    else
    {
        PangoFontFamily **families;
        PangoFontFamily  *family = nullptr;
        int n_families;
        PangoContext* context = wxGetPangoContext();
        pango_context_list_families(context, &families, &n_families);

        for (int i = 0; i < n_families; ++i)
        {
            if (g_ascii_strcasecmp(pango_font_family_get_name( families[i] ),
                                   pango_font_description_get_family( description )) == 0 )
            {
                family = families[i];
                break;
            }
        }

        g_free(families);
        g_object_unref(context);

        // Some gtk+ systems might query for a non-existing font from
        // wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT) on initialization,
        // don't assert until wxSystemSettings::GetFont is checked for this - MR
        // wxASSERT_MSG( family, "No appropriate PangoFontFamily found for ::description" );

        if (family != nullptr && pango_font_family_is_monospace( family ))
            ret = wxFONTFAMILY_TELETYPE; // is deemed a monospace font by pango
    }
#endif // GTK+ 2 || HAVE_PANGO_FONT_FAMILY_IS_MONOSPACE

    if (ret == wxFONTFAMILY_UNKNOWN)
    {
        if (strstr( family_text, "sans" ) != nullptr || strstr( family_text, "Sans" ) != nullptr)
            // checked before serif, so that "* Sans Serif" fonts are detected correctly
            ret = wxFONTFAMILY_SWISS;       // contains "Sans"
        else if (strstr( family_text, "serif" ) != nullptr || strstr( family_text, "Serif" ) != nullptr)
            ret = wxFONTFAMILY_ROMAN;       // contains "Serif"
        else if (wxStrnicmp( family_text, "times", 5 ) == 0)
            ret = wxFONTFAMILY_ROMAN;       // begins with "Times"
        else if (wxStrnicmp( family_text, "old", 3 ) == 0)
            ret = wxFONTFAMILY_DECORATIVE;  // begins with "Old" - "Old English", "Old Town"
    }

    return ret;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
    return wxFONTENCODING_SYSTEM;
}

void wxNativeFontInfo::SetFractionalPointSize(double pointsize)
{
    pango_font_description_set_size( description, wxRound(pointsize * PANGO_SCALE) );
}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{
    switch (style)
    {
        case wxFONTSTYLE_ITALIC:
            pango_font_description_set_style( description, PANGO_STYLE_ITALIC );
            break;
        case wxFONTSTYLE_SLANT:
            pango_font_description_set_style( description, PANGO_STYLE_OBLIQUE );
            break;
        default:
            wxFAIL_MSG( "unknown font style" );
            wxFALLTHROUGH;
        case wxFONTSTYLE_NORMAL:
            pango_font_description_set_style( description, PANGO_STYLE_NORMAL );
            break;
    }
}

void wxNativeFontInfo::SetNumericWeight(int weight)
{
    pango_font_description_set_weight(description, (PangoWeight) weight);
}

void wxNativeFontInfo::SetUnderlined(bool underlined)
{
    // Pango doesn't have the underlined attribute so we store it separately
    // (and handle it specially in wxWindowDCImpl::DoDrawText()).
    m_underlined = underlined;
}

void wxNativeFontInfo::SetStrikethrough(bool strikethrough)
{
    // As with the underlined attribute above, we handle this one separately as
    // Pango doesn't support it as part of the font description.
    m_strikethrough = strikethrough;
}

bool wxNativeFontInfo::SetFaceName(const wxString& facename)
{
    pango_font_description_set_family(description, wxPANGO_CONV(facename));

    // we return true because Pango doesn't tell us if the call failed or not;
    // instead on wxGTK wxFont::SetFaceName() will call wxFontBase::SetFaceName()
    // which does the check
    return true;
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{
    wxArrayString facename;

    // the list of fonts associated with a family was partially
    // taken from http://www.codestyle.org/css/font-family

    switch ( family )
    {
        case wxFONTFAMILY_SCRIPT:
            // corresponds to the cursive font family in the page linked above
            facename.Add(wxS("URW Chancery L"));
            facename.Add(wxS("Comic Sans MS"));
            break;

        case wxFONTFAMILY_DECORATIVE:
            // corresponds to the fantasy font family in the page linked above
            facename.Add(wxS("Impact"));
            break;

        case wxFONTFAMILY_ROMAN:
            // corresponds to the serif font family in the page linked above
            facename.Add(wxS("Serif"));
            facename.Add(wxS("DejaVu Serif"));
            facename.Add(wxS("DejaVu LGC Serif"));
            facename.Add(wxS("Bitstream Vera Serif"));
            facename.Add(wxS("Liberation Serif"));
            facename.Add(wxS("FreeSerif"));
            facename.Add(wxS("Luxi Serif"));
            facename.Add(wxS("Times New Roman"));
            facename.Add(wxS("Century Schoolbook L"));
            facename.Add(wxS("URW Bookman L"));
            facename.Add(wxS("URW Palladio L"));
            facename.Add(wxS("Times"));
            break;

        case wxFONTFAMILY_TELETYPE:
        case wxFONTFAMILY_MODERN:
            // corresponds to the monospace font family in the page linked above
            facename.Add(wxS("Monospace"));
            facename.Add(wxS("DejaVu Sans Mono"));
            facename.Add(wxS("DejaVu LGC Sans Mono"));
            facename.Add(wxS("Bitstream Vera Sans Mono"));
            facename.Add(wxS("Liberation Mono"));
            facename.Add(wxS("FreeMono"));
            facename.Add(wxS("Luxi Mono"));
            facename.Add(wxS("Courier New"));
            facename.Add(wxS("Lucida Sans Typewriter"));
            facename.Add(wxS("Nimbus Mono L"));
            facename.Add(wxS("Andale Mono"));
            facename.Add(wxS("Courier"));
            break;

        case wxFONTFAMILY_SWISS:
        case wxFONTFAMILY_DEFAULT:
        default:
            // corresponds to the sans-serif font family in the page linked above
            facename.Add(wxS("Sans"));
            facename.Add(wxS("DejaVu Sans"));
            facename.Add(wxS("DejaVu LGC Sans"));
            facename.Add(wxS("Bitstream Vera Sans"));
            facename.Add(wxS("Liberation Sans"));
            facename.Add(wxS("FreeSans"));
            facename.Add(wxS("Luxi Sans"));
            facename.Add(wxS("Arial"));
            facename.Add(wxS("Lucida Sans"));
            facename.Add(wxS("Nimbus Sans L"));
            facename.Add(wxS("URW Gothic L"));
            break;
    }

    SetFaceName(facename);
}

void wxNativeFontInfo::SetEncoding(wxFontEncoding WXUNUSED(encoding))
{
    wxFAIL_MSG( "not implemented: Pango encoding is always UTF8" );
}

bool wxNativeFontInfo::FromString(const wxString& s)
{
    wxString str(s);

    // Pango font description doesn't have 'underlined' or 'strikethrough'
    // attributes, so we handle them specially by extracting them from the
    // string before passing it to Pango.
    m_underlined = str.StartsWith(wxS("underlined "), &str);
    m_strikethrough = str.StartsWith(wxS("strikethrough "), &str);

    if (description)
        pango_font_description_free( description );

    // there is a bug in at least pango <= 1.13 which makes it (or its backends)
    // segfault for very big point sizes and for negative point sizes.
    // To workaround that bug for pango <= 1.13
    // (see http://bugzilla.gnome.org/show_bug.cgi?id=340229)
    // we do the check on the size here using same (arbitrary) limits used by
    // pango > 1.13. Note that the segfault could happen also for pointsize
    // smaller than this limit !!
    const size_t pos = str.find_last_of(wxS(" "));
    double size;
    if ( pos != wxString::npos && wxString(str, pos + 1).ToDouble(&size) )
    {
        wxString sizeStr;
        if ( size < 1 )
            sizeStr = wxS("1");
        else if ( size >= 1E6 )
            sizeStr = wxS("1E6");

        if ( !sizeStr.empty() )
        {
            // replace the old size with the adjusted one
            str = wxString(s, 0, pos) + sizeStr;
        }
    }

    description = pango_font_description_from_string(wxPANGO_CONV(str));

#if wxUSE_FONTENUM
    // ensure a valid facename is selected
    if (!wxFontEnumerator::IsValidFacename(GetFaceName()))
        SetFaceName(wxNORMAL_FONT->GetFaceName());
#endif // wxUSE_FONTENUM

    return true;
}

wxString wxNativeFontInfo::ToString() const
{
    wxGtkString str(pango_font_description_to_string( description ));
    wxString desc = wxPANGO_CONV_BACK(str);

    // Augment the string with the attributes not handled by Pango.
    //
    // Notice that we must add them in the same order they are extracted in
    // FromString() above.
    if (m_strikethrough)
        desc.insert(0, wxS("strikethrough "));
    if (m_underlined)
        desc.insert(0, wxS("underlined "));

    return desc;
}

bool wxNativeFontInfo::FromUserString(const wxString& s)
{
    return FromString( s );
}

wxString wxNativeFontInfo::ToUserString() const
{
    return ToString();
}
