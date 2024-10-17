/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/fontenum.cpp
// Purpose:     wxFontEnumerator class for X11/GDK
// Author:      Vadim Zeitlin
// Created:     01.10.99
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_FONTENUM

#include "wx/fontenum.h"

#ifndef WX_PRECOMP
    #include "wx/dynarray.h"
    #include "wx/string.h"
    #include "wx/app.h"
    #include "wx/utils.h"
#endif

#include "wx/regex.h"
#include "wx/fontmap.h"
#include "wx/fontutil.h"
#include "wx/encinfo.h"

// ----------------------------------------------------------------------------
// Pango
// ----------------------------------------------------------------------------

#include <pango/pango.h>

PangoContext* wxGetPangoContext();

extern "C"
{
static int wxCMPFUNC_CONV
wxCompareFamilies (const void *a, const void *b)
{
    const PangoFontFamily* fam_a = *static_cast<PangoFontFamily* const*>(a);
    const PangoFontFamily* fam_b = *static_cast<PangoFontFamily* const*>(b);
    const char* a_name = pango_font_family_get_name(const_cast<PangoFontFamily*>(fam_a));
    const char* b_name = pango_font_family_get_name(const_cast<PangoFontFamily*>(fam_b));

  return g_utf8_collate (a_name, b_name);
}
}

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    // This parameter may be unused when pango_font_family_is_monospace() is
    // not available, suppress the (unavoidable) warning in this case.
    wxUnusedVar(fixedWidthOnly);

    if ( encoding != wxFONTENCODING_SYSTEM && encoding != wxFONTENCODING_UTF8 )
    {
        // Pango supports only UTF-8 encoding (and system means any, so we
        // accept it too)
        return false;
    }

    PangoFontFamily **families = nullptr;
    gint n_families = 0;
    PangoContext* context = wxGetPangoContext();
    pango_context_list_families(context, &families, &n_families);
    qsort (families, n_families, sizeof (PangoFontFamily *), wxCompareFamilies);

    for ( int i = 0; i < n_families; i++ )
    {
#if defined(__WXGTK__) || defined(HAVE_PANGO_FONT_FAMILY_IS_MONOSPACE)
        if ( !fixedWidthOnly ||
            pango_font_family_is_monospace(families[i]) )
#endif
        {
            const gchar *name = pango_font_family_get_name(families[i]);
            if ( !OnFacename(wxString(name, wxConvUTF8)) )
            {
                break;
            }
        }
    }
    g_free(families);
    g_object_unref(context);

    return true;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& facename)
{
    return EnumerateEncodingsUTF8(facename);
}


#endif // wxUSE_FONTENUM
