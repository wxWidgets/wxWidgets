///////////////////////////////////////////////////////////////////////////////
// Name:        src/osx/fontutil.cpp
// Purpose:     font-related helper functions for OS X
// Author:      Vadim Zeitlin, Stefan Csomor
// Created:     05.11.99
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/wxcrtvararg.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif

#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/encinfo.h"
#include "wx/filename.h"
#include "wx/stdpaths.h"
#include "wx/tokenzr.h"


// convert to/from the string representation:
// format is facename[;charset]
//
bool wxNativeEncodingInfo::FromString( const wxString& s )
{
    wxStringTokenizer tokenizer(s, wxT(";"));

    facename = tokenizer.GetNextToken();
    if ( facename.empty() )
        return false;

    wxString tmp = tokenizer.GetNextToken();
    if ( tmp.empty() )
    {
        // default charset
        charset = 0;
    }
    else
    {
        if ( wxSscanf( tmp, wxT("%u"), &charset ) != 1 )
            // should be a number!
            return false;
    }

    return true;
}

wxString wxNativeEncodingInfo::ToString() const
{
    wxString s(facename);
    if ( charset != 0 )
        s << wxT(';') << charset;

    return s;
}

// ----------------------------------------------------------------------------
// Private Fonts
// ----------------------------------------------------------------------------

#if wxUSE_PRIVATE_FONTS

// On OSX one can provide private fonts simply by putting the font files in
// with the resources in your application bundle. So the API for adding fonts
// does not do anything except checking that the file you pass to it actually
// does exist and is in the correct directory.

bool wxFontBase::AddPrivateFont(const wxString& filename)
{
    wxFileName fn(filename);
    if ( !fn.FileExists() )
    {
        wxLogError(_("Font file \"%s\" doesn't exist."), filename);
        return false;
    }

    wxString fontsDir;
    fontsDir << wxStandardPaths::Get().GetResourcesDir() << "/Fonts";
    if ( fn.GetPath() != fontsDir )
    {
        wxLogError(_("Font file \"%s\" cannot be used as it is not inside "
                     "the font directory \"%s\"."), filename, fontsDir);
        return false;
    }

    return true;
}

#endif // wxUSE_PRIVATE_FONTS

// ----------------------------------------------------------------------------
// helper functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding( wxFontEncoding encoding, wxNativeEncodingInfo *info )
{
    wxCHECK_MSG( info, false, wxT("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
        encoding = wxFont::GetDefaultEncoding();

    info->encoding = encoding;

    return true;
}

bool wxTestFontEncoding( const wxNativeEncodingInfo& WXUNUSED(info) )
{
    // basically we should be able to support every encoding via the OS
    return true;
}
