/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/fontenum.cpp
// Purpose:     wxFontEnumerator class for X11/GDK
// Author:      Vadim Zeitlin
// Modified by:
// Created:     01.10.99
// RCS-ID:      $Id$
// Copyright:   (c) Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "fontenum.h"
#endif

// for compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/dynarray.h"
#include "wx/string.h"
#include "wx/regex.h"
#include "wx/utils.h"
#include "wx/app.h"
#include "wx/fontmap.h"
#include "wx/fontenum.h"
#include "wx/fontutil.h"
#include "wx/encinfo.h"

// ----------------------------------------------------------------------------
// Pango
// ----------------------------------------------------------------------------

#if wxUSE_PANGO

#include "pango/pango.h"

#ifdef __WXGTK20__
#include "gtk/gtk.h"
extern GtkWidget *wxGetRootWindow();
#endif

static int
cmp_families (const void *a, const void *b)
{
  const char *a_name = pango_font_family_get_name (*(PangoFontFamily **)a);
  const char *b_name = pango_font_family_get_name (*(PangoFontFamily **)b);
  
  return g_utf8_collate (a_name, b_name);
}

// I admit I don't yet understand encodings with Pango
bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
#ifndef HAVE_PANGO_FONT_FAMILY_IS_MONOSPACE
    if ( fixedWidthOnly )
    {
        OnFacename( wxT("monospace") );
    }
    else
#endif
    {
        PangoFontFamily **families = NULL;
        gint n_families = 0;
        pango_context_list_families ( 
#ifdef __WXGTK20__
            gtk_widget_get_pango_context( wxGetRootWindow() ),
#else
            wxTheApp->GetPangoContext(),
#endif
            &families, &n_families );
        qsort (families, n_families, sizeof (PangoFontFamily *), cmp_families);

        for (int i=0; i<n_families; i++)
        {
#ifdef HAVE_PANGO_FONT_FAMILY_IS_MONOSPACE
            if (!fixedWidthOnly ||
                pango_font_family_is_monospace(families[i]))
#endif
            {
                const gchar *name = pango_font_family_get_name(families[i]);
                OnFacename(wxString(name, wxConvUTF8));
            }
        }
    }
    
    return TRUE;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
    return FALSE;
}


#else
  // Pango

#ifdef __VMS__ // Xlib.h for VMS is not (yet) compatible with C++
               // The resulting warnings are switched off here
#pragma message disable nosimpint
#endif
#include <X11/Xlib.h>
#ifdef __VMS__
#pragma message enable nosimpint
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// create the list of all fonts with the given spacing and encoding
static char **CreateFontList(wxChar spacing, wxFontEncoding encoding,
                             int *nFonts);

// extract all font families from the given font list and call our
// OnFacename() for each of them
static bool ProcessFamiliesFromFontList(wxFontEnumerator *This,
                                        char **fonts,
                                        int nFonts);


// ----------------------------------------------------------------------------
// private types
// ----------------------------------------------------------------------------

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

#if !wxUSE_NANOX
static char **CreateFontList(wxChar spacing,
                             wxFontEncoding encoding,
                             int *nFonts)
{
    wxNativeEncodingInfo info;
    wxGetNativeFontEncoding(encoding, &info);

#if wxUSE_FONTMAP
    if ( !wxTestFontEncoding(info) )
    {
        // ask font mapper for a replacement
        (void)wxFontMapper::Get()->GetAltForEncoding(encoding, &info);
    }
#endif // wxUSE_FONTMAP

    wxString pattern;
    pattern.Printf(wxT("-*-*-*-*-*-*-*-*-*-*-%c-*-%s-%s"),
                   spacing,
                   info.xregistry.c_str(),
                   info.xencoding.c_str());

    // get the list of all fonts
    return XListFonts((Display *)wxGetDisplay(), pattern.mb_str(), 32767, nFonts);
}

static bool ProcessFamiliesFromFontList(wxFontEnumerator *This,
                                        char **fonts,
                                        int nFonts)
{
#if wxUSE_REGEX
    wxRegEx re(wxT("^(-[^-]*){14}$"), wxRE_NOSUB);
#endif // wxUSE_REGEX

    // extract the list of (unique) font families
    wxSortedArrayString families;
    for ( int n = 0; n < nFonts; n++ )
    {
        char *font = fonts[n];
#if wxUSE_REGEX
        if ( !re.Matches(font) )
#else // !wxUSE_REGEX
        if ( !wxString(font).Matches(wxT("-*-*-*-*-*-*-*-*-*-*-*-*-*-*")) )
#endif // wxUSE_REGEX/!wxUSE_REGEX
        {
            // it's not a full font name (probably an alias)
            continue;
        }

        char *dash = strchr(font + 1, '-');
        char *family = dash + 1;
        dash = strchr(family, '-');
        *dash = '\0'; // !NULL because Matches() above succeeded
        wxString fam(family);

        if ( families.Index(fam) == wxNOT_FOUND )
        {
            if ( !This->OnFacename(fam) )
            {
                // stop enumerating
                return FALSE;
            }

            families.Add(fam);
        }
        //else: already seen
    }

    return TRUE;
}
#endif
  // wxUSE_NANOX

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
#if wxUSE_NANOX
    return FALSE;
#else
    int nFonts;
    char **fonts;

    if ( fixedWidthOnly )
    {
        bool cont = TRUE;
        fonts = CreateFontList(wxT('m'), encoding, &nFonts);
        if ( fonts )
        {
            cont = ProcessFamiliesFromFontList(this, fonts, nFonts);

            XFreeFontNames(fonts);
        }

        if ( !cont )
        {
            return TRUE;
        }

        fonts = CreateFontList(wxT('c'), encoding, &nFonts);
        if ( !fonts )
        {
            return TRUE;
        }
    }
    else
    {
        fonts = CreateFontList(wxT('*'), encoding, &nFonts);

        if ( !fonts )
        {
            // it's ok if there are no fonts in given encoding - but it's not
            // ok if there are no fonts at all
            wxASSERT_MSG(encoding != wxFONTENCODING_SYSTEM,
                         wxT("No fonts at all on this system?"));

            return FALSE;
        }
    }

    (void)ProcessFamiliesFromFontList(this, fonts, nFonts);

    XFreeFontNames(fonts);
    return TRUE;
#endif
    // wxUSE_NANOX
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
#if wxUSE_NANOX
    return FALSE;
#else
    wxString pattern;
    pattern.Printf(wxT("-*-%s-*-*-*-*-*-*-*-*-*-*-*-*"),
                   family.IsEmpty() ? wxT("*") : family.c_str());

    // get the list of all fonts
    int nFonts;
    char **fonts = XListFonts((Display *)wxGetDisplay(), pattern.mb_str(),
                              32767, &nFonts);

    if ( !fonts )
    {
        // unknown family?
        return FALSE;
    }

    // extract the list of (unique) encodings
    wxSortedArrayString encodings;
    for ( int n = 0; n < nFonts; n++ )
    {
        char *font = fonts[n];
        if ( !wxString(font).Matches(wxT("-*-*-*-*-*-*-*-*-*-*-*-*-*-*")) )
        {
            // it's not a full font name (probably an alias)
            continue;
        }

        // extract the family
        char *dash = strchr(font + 1, '-');
        char *familyFont = dash + 1;
        dash = strchr(familyFont, '-');
        *dash = '\0'; // !NULL because Matches() above succeeded

        if ( !family.IsEmpty() && (family != familyFont) )
        {
            // family doesn't match
            continue;
        }

        // now extract the registry/encoding
        char *p = dash + 1; // just after the dash after family
        dash = strrchr(p, '-');

        wxString registry(dash + 1);
        *dash = '\0';

        dash = strrchr(p, '-');
        wxString encoding(dash + 1);

        encoding << wxT('-') << registry;
        if ( encodings.Index(encoding) == wxNOT_FOUND )
        {
            if ( !OnFontEncoding(familyFont, encoding) )
            {
                break;
            }

            encodings.Add(encoding);
        }
        //else: already had this one
    }

    XFreeFontNames(fonts);

    return TRUE;
#endif
    // wxUSE_NANOX
}

#endif
   // __WXGTK20__
