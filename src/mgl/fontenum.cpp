/////////////////////////////////////////////////////////////////////////////
// Name:        src/unix/fontenum.cpp
// Purpose:     wxFontEnumerator class for MGL
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 2001 Vaclav Slavik
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

#include "wx/fontmap.h"
#include "wx/fontenum.h"
#include "wx/fontutil.h"

#include <mgraph.h>

// ============================================================================
// implementation
// ============================================================================


// ----------------------------------------------------------------------------
// wxFontEnumerator
// ----------------------------------------------------------------------------

bool wxFontEnumerator::EnumerateFacenames(wxFontEncoding encoding,
                                          bool fixedWidthOnly)
{
    wxMGLFontFamilyList *list = wxTheFontsManager->GetFamilyList();
    wxMGLFontFamilyList::Node *node;
    wxMGLFontFamily *f = NULL;

    // FIXME_MGL - available encodings    
    for (node = list->GetFirst(); node; node = node->GetNext())
    {
        f = node->GetData();
        if ( !fixedWidthOnly || f->GetInfo()->isFixed )
            if ( !OnFacename(f->GetName()) )
                return TRUE;
    }

    return (f != NULL) /* i.e. FALSE if there are no fonts */;
}

bool wxFontEnumerator::EnumerateEncodings(const wxString& family)
{
#if 0
    static wxFontEncoding encodings[] = {
        wxFONTENCODING_ISO8859_1,
        wxFONTENCODING_ISO8859_2,
        wxFONTENCODING_ISO8859_3,
        wxFONTENCODING_ISO8859_4,
        wxFONTENCODING_ISO8859_5,
        wxFONTENCODING_ISO8859_6,
        wxFONTENCODING_ISO8859_7,
        wxFONTENCODING_ISO8859_8,
        wxFONTENCODING_ISO8859_9,
        wxFONTENCODING_ISO8859_10,
        //wxFONTENCODING_ISO8859_11,
        //wxFONTENCODING_ISO8859_12,
        wxFONTENCODING_ISO8859_13,
        wxFONTENCODING_ISO8859_14,
        wxFONTENCODING_ISO8859_15,
        wxFONTENCODING_CP1250,
        wxFONTENCODING_CP1251,
        wxFONTENCODING_CP1252,
        wxFONTENCODING_CP1253,
        wxFONTENCODING_CP1254,
        wxFONTENCODING_CP1255,
        wxFONTENCODING_CP1256,
        wxFONTENCODING_CP1257,
        wxFONTENCODING_KOI8,
        wxFONTENCODING_SYSTEM
    };
    
    for (size_t i = 0; encodings[i] != wxFONTENCODING_SYSTEM; i++)
        if ( !OnFontEncoding(family, encodings[i]) )
            break;
#endif
    OnFontEncoding(family, wxT("iso8859-1"));
    // FIXME_MGL -- tests for validity

    return TRUE;
}
