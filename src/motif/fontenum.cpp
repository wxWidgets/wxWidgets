/////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/fontenum.cpp
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

#ifdef __GNUG__
    #pragma implementation "fontenum.h"
#endif

#include "wx/defs.h"
#include "wx/dynarray.h"
#include "wx/string.h"
#include "wx/utils.h"

#include "wx/fontenum.h"

#include <X11/Xlib.h>

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// compare function for sorted array of strings
static int CMPFUNC_CONV CompareStrings(const char *s1, const char *s2);

// create the list of all fonts with the given spacing
static char **CreateFontList(wxChar spacing, int *nFonts);

// extract all font families from the given font list and call our
// OnFontFamily() for each of them
static bool ProcessFamiliesFromFontList(wxFontEnumerator *This,
                                        char **fonts,
                                        int nFonts);


// ----------------------------------------------------------------------------
// private types
// ----------------------------------------------------------------------------

WX_DEFINE_SORTED_ARRAY(const char *, wxSortedStringArray);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

static int CMPFUNC_CONV CompareStrings(const char *s1, const char *s2)
{
    return strcmp(s1, s2);
}

static char **CreateFontList(wxChar spacing, int *nFonts)
{
    wxString pattern;
    pattern.Printf(_T("-*-*-*-*-*-*-*-*-*-*-%c-*-*-*"), spacing);

    // get the list of all fonts
    return XListFonts((Display *)wxGetDisplay(), pattern, 32767, nFonts);
}

static bool ProcessFamiliesFromFontList(wxFontEnumerator *This,
                                        char **fonts,
                                        int nFonts)
{
    // extract the list of (unique) font families
    wxSortedStringArray families(CompareStrings);
    for ( int n = 0; n < nFonts; n++ )
    {
        char *font = fonts[n];
        if ( !wxString(font).Matches("-*-*-*-*-*-*-*-*-*-*-*-*-*-*") )
        {
            // it's not a full font name (probably an alias)
            continue;
        }
        
        char *dash = strchr(font + 1, '-');
        char *family = dash + 1;
        dash = strchr(family, '-');
        *dash = '\0'; // !NULL because Matches() above succeeded

        if ( families.Index(family) == wxNOT_FOUND )
        {
            if ( !This->OnFontFamily(family) )
            {
                // stop enumerating
                return FALSE;
            }

            families.Add(family);
        }
        //else: already seen
    }

    return TRUE;
}

// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFamilies(bool fixedWidthOnly)
{
    int nFonts;
    char **fonts;

    if ( fixedWidthOnly )
    {
        bool cont = TRUE;
        fonts = CreateFontList(_T('m'), &nFonts);
        if ( fonts )
        {
            cont = ProcessFamiliesFromFontList(this, fonts, nFonts);

            XFreeFontNames(fonts);
        }

        if ( !cont )
        {
            return TRUE;
        }

        fonts = CreateFontList(_T('c'), &nFonts);
        if ( !fonts )
        {
            return TRUE;
        }
    }
    else
    {
        fonts = CreateFontList(_T('*'), &nFonts);

        if ( !fonts )
        {
            wxFAIL_MSG(_T("No fonts at all on this system?"));

            return FALSE;
        }
    }

    (void)ProcessFamiliesFromFontList(this, fonts, nFonts);

    XFreeFontNames(fonts);

    return TRUE;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
#if 0
    wxString pattern;
    pattern.Printf(_T("-*-%s-*-*-*-*-*-*-*-*-*-*-*-*"),
                   family.IsEmpty() ? _T("*") : family.c_str());

    // get the list of all fonts
    int nFonts;
    char **fonts = XListFonts((Display *)wxGetDisplay(), pattern,
                              32767, nFonts);

    if ( !fonts )
    {
        // unknown family?
        return FALSE;
    }

    // extract the list of (unique) encodings
    wxSortedStringArray families(CompareStrings);
    for ( int n = 0; n < nFonts; n++ )
    {
        char *font = fonts[n];
        if ( !wxString(font).Matches("-*-*-*-*-*-*-*-*-*-*-*-*-*-*") )
        {
            // it's not a full font name (probably an alias)
            continue;
        }

        // extract the family
        char *dash = strchr(font + 1, '-');
        char *family = dash + 1;
        dash = strchr(family, '-');
        *dash = '\0'; // !NULL because Matches() above succeeded

        // now extract the registry/encoding
    }

    return TRUE;
#endif // 0

    return FALSE;   // TODO
}
