/////////////////////////////////////////////////////////////////////////////
// Name:        unix/fontutil.cpp
// Purpose:     Font helper functions for X11 (GDK/X)
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.11.99
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
    #pragma implementation "fontutil.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/encinfo.h"
#endif // PCH

#include "wx/fontutil.h"
#include "wx/fontmap.h"
#include "wx/tokenzr.h"
#include "wx/hash.h"
#include "wx/module.h"

#if wxUSE_PANGO

#include "pango/pango.h"

#ifdef __WXGTK20__
#include "wx/gtk/private.h"
#else
#include "wx/x11/private.h"
#endif

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

void wxNativeFontInfo::Init()
{
    description = NULL;
}

void
wxNativeFontInfo::Init(const wxNativeFontInfo& info)
{
    if (info.description)
        description = pango_font_description_copy(info.description);
    else
        description = NULL;
}

void wxNativeFontInfo::Free()
{
    if (description)
        pango_font_description_free(description);
}

int wxNativeFontInfo::GetPointSize() const
{
    return pango_font_description_get_size( description ) / PANGO_SCALE;
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

wxFontWeight wxNativeFontInfo::GetWeight() const
{
    wxFontWeight m_weight = wxFONTWEIGHT_NORMAL;

    switch (pango_font_description_get_weight( description ))
    {
        case PANGO_WEIGHT_ULTRALIGHT:
            m_weight = wxFONTWEIGHT_LIGHT;
            break;
        case PANGO_WEIGHT_LIGHT:
            m_weight = wxFONTWEIGHT_LIGHT;
            break;
        case PANGO_WEIGHT_NORMAL:
            m_weight = wxFONTWEIGHT_NORMAL;
            break;
        case PANGO_WEIGHT_BOLD:
            m_weight = wxFONTWEIGHT_BOLD;
            break;
        case PANGO_WEIGHT_ULTRABOLD:
            m_weight = wxFONTWEIGHT_BOLD;
            break;
        case PANGO_WEIGHT_HEAVY:
            m_weight = wxFONTWEIGHT_BOLD;
            break;
    }

    return m_weight;
}

bool wxNativeFontInfo::GetUnderlined() const
{
    return FALSE;
}

wxString wxNativeFontInfo::GetFaceName() const
{
    wxString tmp = wxGTK_CONV_BACK( pango_font_description_get_family( description ) );

    return tmp;
}

wxFontFamily wxNativeFontInfo::GetFamily() const
{
    return wxFONTFAMILY_SWISS;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
    return wxFONTENCODING_SYSTEM;
}


void wxNativeFontInfo::SetPointSize(int WXUNUSED(pointsize))
{
    wxFAIL_MSG( _T("not implemented") );
}

void wxNativeFontInfo::SetStyle(wxFontStyle WXUNUSED(style))
{
    wxFAIL_MSG( _T("not implemented") );
}

void wxNativeFontInfo::SetWeight(wxFontWeight WXUNUSED(weight))
{
    wxFAIL_MSG( _T("not implemented") );
}

void wxNativeFontInfo::SetUnderlined(bool WXUNUSED(underlined))
{
    wxFAIL_MSG( _T("not implemented") );
}

void wxNativeFontInfo::SetFaceName(wxString WXUNUSED(facename))
{
    wxFAIL_MSG( _T("not implemented") );
}

void wxNativeFontInfo::SetFamily(wxFontFamily WXUNUSED(family))
{
    wxFAIL_MSG( _T("not implemented") );
}

void wxNativeFontInfo::SetEncoding(wxFontEncoding WXUNUSED(encoding))
{
    wxFAIL_MSG( _T("not implemented") );
}



bool wxNativeFontInfo::FromString(const wxString& s)
{
    if (description)
        pango_font_description_free( description );

    description = pango_font_description_from_string( wxGTK_CONV( s ) );

    return TRUE;
}

wxString wxNativeFontInfo::ToString() const
{
    char *str = pango_font_description_to_string( description );
    wxString tmp = wxGTK_CONV_BACK(  str );
    g_free( str );

    return tmp;
}

bool wxNativeFontInfo::FromUserString(const wxString& s)
{
    return FromString( s );
}

wxString wxNativeFontInfo::ToUserString() const
{
    return ToString();
}

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    return FALSE;
}

wxString wxNativeEncodingInfo::ToString() const
{
    return wxEmptyString;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    return TRUE;
}

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
    // we *must* return true for default encoding as otherwise wxFontMapper
    // considers that we can't load any font and aborts with wxLogFatalError!
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        info->facename.clear();
        info->encoding = wxFONTENCODING_SYSTEM;
    }

    // pretend that we support everything, it's better than to always return
    // false as the old code did
    return true;
}

#else // GTK+ 1.x

#ifdef __X__
    #ifdef __VMS__
        #pragma message disable nosimpint
    #endif

    #include <X11/Xlib.h>

    #ifdef __VMS__
        #pragma message enable nosimpint
    #endif

    #include "wx/utils.h"       // for wxGetDisplay()
#elif defined(__WXGTK__)
    // we have to declare struct tm to avoid problems with first forward
    // declaring it in C code (glib.h included from gdk.h does it) and then
    // defining it when time.h is included from the headers below - this is
    // known not to work at least with Sun CC 6.01
    #include <time.h>

    #include <gdk/gdk.h>
#endif


// ----------------------------------------------------------------------------
// private data
// ----------------------------------------------------------------------------

static wxHashTable *g_fontHash = (wxHashTable*) NULL;

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// define the functions to create and destroy native fonts for this toolkit
#ifdef __X__
    wxNativeFont wxLoadFont(const wxString& fontSpec)
    {
        return XLoadQueryFont((Display *)wxGetDisplay(), fontSpec);
    }

    inline void wxFreeFont(wxNativeFont font)
    {
        XFreeFont((Display *)wxGetDisplay(), (XFontStruct *)font);
    }
#elif defined(__WXGTK__)
    wxNativeFont wxLoadFont(const wxString& fontSpec)
    {
        // VZ: we should use gdk_fontset_load() instead of gdk_font_load()
        //     here to be able to display Japanese fonts correctly (at least
        //     this is what people report) but unfortunately doing it results
        //     in tons of warnings when using GTK with "normal" European
        //     languages and so we can't always do it and I don't know enough
        //     to determine when should this be done... (FIXME)
        return gdk_font_load( wxConvertWX2MB(fontSpec) );
    }

    inline void wxFreeFont(wxNativeFont font)
    {
        gdk_font_unref(font);
    }
#else
    #error "Unknown GUI toolkit"
#endif

static bool wxTestFontSpec(const wxString& fontspec);

static wxNativeFont wxLoadQueryFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool underlined,
                                    const wxString& facename,
                                    const wxString& xregistry,
                                    const wxString& xencoding,
                                    wxString* xFontName);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxNativeEncodingInfo
// ----------------------------------------------------------------------------

// convert to/from the string representation: format is
//      encodingid;registry;encoding[;facename]
bool wxNativeEncodingInfo::FromString(const wxString& s)
{
    // use ";", not "-" because it may be part of encoding name
    wxStringTokenizer tokenizer(s, _T(";"));

    wxString encid = tokenizer.GetNextToken();
    long enc;
    if ( !encid.ToLong(&enc) )
        return FALSE;
    encoding = (wxFontEncoding)enc;

    xregistry = tokenizer.GetNextToken();
    if ( !xregistry )
        return FALSE;

    xencoding = tokenizer.GetNextToken();
    if ( !xencoding )
        return FALSE;

    // ok even if empty
    facename = tokenizer.GetNextToken();

    return TRUE;
}

wxString wxNativeEncodingInfo::ToString() const
{
    wxString s;
    s << (long)encoding << _T(';') << xregistry << _T(';') << xencoding;
    if ( !!facename )
    {
        s << _T(';') << facename;
    }

    return s;
}

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

void wxNativeFontInfo::Init()
{
    m_isDefault = TRUE;
}

bool wxNativeFontInfo::FromString(const wxString& s)
{
    wxStringTokenizer tokenizer(s, _T(";"));

    // check the version
    wxString token = tokenizer.GetNextToken();
    if ( token != _T('0') )
        return FALSE;

    xFontName = tokenizer.GetNextToken();

    // this should be the end
    if ( tokenizer.HasMoreTokens() )
        return FALSE;

    return FromXFontName(xFontName);
}

wxString wxNativeFontInfo::ToString() const
{
    // 0 is the version
    return wxString::Format(_T("%d;%s"), 0, GetXFontName().c_str());
}

bool wxNativeFontInfo::FromUserString(const wxString& s)
{
    return FromXFontName(s);
}

wxString wxNativeFontInfo::ToUserString() const
{
    return GetXFontName();
}

bool wxNativeFontInfo::HasElements() const
{
    // we suppose that the foundry is never empty, so if it is it means that we
    // had never parsed the XLFD
    return !fontElements[0].empty();
}

wxString wxNativeFontInfo::GetXFontComponent(wxXLFDField field) const
{
    wxCHECK_MSG( field < wxXLFD_MAX, _T(""), _T("invalid XLFD field") );

    if ( !HasElements() )
    {
        // const_cast
        if ( !((wxNativeFontInfo *)this)->FromXFontName(xFontName) )
            return _T("");
    }

    return fontElements[field];
}

bool wxNativeFontInfo::FromXFontName(const wxString& fontname)
{
    // TODO: we should be able to handle the font aliases here, but how?
    wxStringTokenizer tokenizer(fontname, _T("-"));

    // skip the leading, usually empty field (font name registry)
    if ( !tokenizer.HasMoreTokens() )
        return FALSE;

    (void)tokenizer.GetNextToken();

    for ( size_t n = 0; n < WXSIZEOF(fontElements); n++ )
    {
        if ( !tokenizer.HasMoreTokens() )
        {
            // not enough elements in the XLFD - or maybe an alias
            return FALSE;
        }

        wxString field = tokenizer.GetNextToken();
        if ( !field.empty() && field != _T('*') )
        {
            // we're really initialized now
            m_isDefault = FALSE;
        }

        fontElements[n] = field;
    }

    // this should be all
    if ( tokenizer.HasMoreTokens() )
        return FALSE;

    return TRUE;
}

wxString wxNativeFontInfo::GetXFontName() const
{
    if ( xFontName.empty() )
    {
        for ( size_t n = 0; n < WXSIZEOF(fontElements); n++ )
        {
            // replace the non specified elements with '*' except for the
            // additional style which is usually just omitted
            wxString elt = fontElements[n];
            if ( elt.empty() && n != wxXLFD_ADDSTYLE )
            {
                elt = _T('*');
            }

            // const_cast
            ((wxNativeFontInfo *)this)->xFontName << _T('-') << elt;
        }
    }

    return xFontName;
}

void
wxNativeFontInfo::SetXFontComponent(wxXLFDField field, const wxString& value)
{
    wxCHECK_RET( field < wxXLFD_MAX, _T("invalid XLFD field") );

    // this class should be initialized with a valid font spec first and only
    // then the fields may be modified!
    wxASSERT_MSG( !IsDefault(), _T("can't modify an uninitialized XLFD") );

    if ( !HasElements() )
    {
        // const_cast
        if ( !((wxNativeFontInfo *)this)->FromXFontName(xFontName) )
        {
            wxFAIL_MSG( _T("can't set font element for invalid XLFD") );

            return;
        }
    }

    fontElements[field] = value;

    // invalidate the XFLD, it doesn't correspond to the font elements any more
    xFontName.clear();
}

void wxNativeFontInfo::SetXFontName(const wxString& xFontName_)
{
    // invalidate the font elements, GetXFontComponent() will reparse the XLFD
    fontElements[0].clear();

    xFontName = xFontName_;

    m_isDefault = FALSE;
}

int wxNativeFontInfo::GetPointSize() const
{
    const wxString s = GetXFontComponent(wxXLFD_POINTSIZE);

    // return -1 to indicate that the size is unknown
    long l;
    return s.ToLong(&l) ? l : -1;
}

wxFontStyle wxNativeFontInfo::GetStyle() const
{
    const wxString s = GetXFontComponent(wxXLFD_SLANT);

    if ( s.length() != 1 )
    {
        // it is really unknown but we don't have any way to return it from
        // here
        return wxFONTSTYLE_NORMAL;
    }

    switch ( s[0] )
    {
        default:
            // again, unknown but consider normal by default

        case _T('r'):
            return wxFONTSTYLE_NORMAL;

        case _T('i'):
            return wxFONTSTYLE_ITALIC;

        case _T('o'):
            return wxFONTSTYLE_SLANT;
    }
}

wxFontWeight wxNativeFontInfo::GetWeight() const
{
    const wxString s = GetXFontComponent(wxXLFD_WEIGHT).MakeLower();
    if ( s.find(_T("bold")) != wxString::npos || s == _T("black") )
        return wxFONTWEIGHT_BOLD;
    else if ( s == _T("light") )
        return wxFONTWEIGHT_LIGHT;

    return wxFONTWEIGHT_NORMAL;
}

bool wxNativeFontInfo::GetUnderlined() const
{
    // X fonts are never underlined
    return FALSE;
}

wxString wxNativeFontInfo::GetFaceName() const
{
    // wxWidgets facename probably more accurately corresponds to X family
    return GetXFontComponent(wxXLFD_FAMILY);
}

wxFontFamily wxNativeFontInfo::GetFamily() const
{
    // and wxWidgets family -- to X foundry, but we have to translate it to
    // wxFontFamily somehow...
    wxFAIL_MSG(_T("not implemented")); // GetXFontComponent(wxXLFD_FOUNDRY);

    return wxFONTFAMILY_DEFAULT;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
    // we already have the code for this but need to refactor it first
    wxFAIL_MSG( _T("not implemented") );

    return wxFONTENCODING_MAX;
}

void wxNativeFontInfo::SetPointSize(int pointsize)
{
    SetXFontComponent(wxXLFD_POINTSIZE, wxString::Format(_T("%d"), pointsize));
}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{
    wxString s;
    switch ( style )
    {
        case wxFONTSTYLE_ITALIC:
            s = _T('i');
            break;

        case wxFONTSTYLE_SLANT:
            s = _T('o');
            break;

        case wxFONTSTYLE_NORMAL:
            s = _T('r');

        default:
            wxFAIL_MSG( _T("unknown wxFontStyle in wxNativeFontInfo::SetStyle") );
            return;
    }

    SetXFontComponent(wxXLFD_SLANT, s);
}

void wxNativeFontInfo::SetWeight(wxFontWeight weight)
{
    wxString s;
    switch ( weight )
    {
        case wxFONTWEIGHT_BOLD:
            s = _T("bold");
            break;

        case wxFONTWEIGHT_LIGHT:
            s = _T("light");
            break;

        case wxFONTWEIGHT_NORMAL:
            s = _T("medium");
            break;

        default:
            wxFAIL_MSG( _T("unknown wxFontWeight in wxNativeFontInfo::SetWeight") );
            return;
    }

    SetXFontComponent(wxXLFD_WEIGHT, s);
}

void wxNativeFontInfo::SetUnderlined(bool WXUNUSED(underlined))
{
    // can't do this under X
}

void wxNativeFontInfo::SetFaceName(wxString facename)
{
    SetXFontComponent(wxXLFD_FAMILY, facename);
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{
    // wxFontFamily -> X foundry, anyone?
    wxFAIL_MSG( _T("not implemented") );

    // SetXFontComponent(wxXLFD_FOUNDRY, ...);
}

void wxNativeFontInfo::SetEncoding(wxFontEncoding encoding)
{
    wxNativeEncodingInfo info;
    if ( wxGetNativeFontEncoding(encoding, &info) )
    {
        SetXFontComponent(wxXLFD_ENCODING, info.xencoding);
        SetXFontComponent(wxXLFD_REGISTRY, info.xregistry);
    }
}

// ----------------------------------------------------------------------------
// common functions
// ----------------------------------------------------------------------------

bool wxGetNativeFontEncoding(wxFontEncoding encoding,
                             wxNativeEncodingInfo *info)
{
    wxCHECK_MSG( info, FALSE, _T("bad pointer in wxGetNativeFontEncoding") );

    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    switch ( encoding )
    {
        case wxFONTENCODING_ISO8859_1:
        case wxFONTENCODING_ISO8859_2:
        case wxFONTENCODING_ISO8859_3:
        case wxFONTENCODING_ISO8859_4:
        case wxFONTENCODING_ISO8859_5:
        case wxFONTENCODING_ISO8859_6:
        case wxFONTENCODING_ISO8859_7:
        case wxFONTENCODING_ISO8859_8:
        case wxFONTENCODING_ISO8859_9:
        case wxFONTENCODING_ISO8859_10:
        case wxFONTENCODING_ISO8859_11:
        case wxFONTENCODING_ISO8859_12:
        case wxFONTENCODING_ISO8859_13:
        case wxFONTENCODING_ISO8859_14:
        case wxFONTENCODING_ISO8859_15:
            {
                int cp = encoding - wxFONTENCODING_ISO8859_1 + 1;
                info->xregistry = wxT("iso8859");
                info->xencoding.Printf(wxT("%d"), cp);
            }
            break;

        case wxFONTENCODING_UTF8:
            info->xregistry = wxT("iso10646");
            info->xencoding = wxT("*");
            break;

        case wxFONTENCODING_GB2312:
            info->xregistry = wxT("GB2312");   // or the otherway round?
            info->xencoding = wxT("*");
            break;

        case wxFONTENCODING_KOI8:
        case wxFONTENCODING_KOI8_U:
            info->xregistry = wxT("koi8");

            // we don't make distinction between koi8-r, koi8-u and koi8-ru (so far)
            info->xencoding = wxT("*");
            break;

        case wxFONTENCODING_CP1250:
        case wxFONTENCODING_CP1251:
        case wxFONTENCODING_CP1252:
        case wxFONTENCODING_CP1253:
        case wxFONTENCODING_CP1254:
        case wxFONTENCODING_CP1255:
        case wxFONTENCODING_CP1256:
        case wxFONTENCODING_CP1257:
            {
                int cp = encoding - wxFONTENCODING_CP1250 + 1250;
                info->xregistry = wxT("microsoft");
                info->xencoding.Printf(wxT("cp%d"), cp);
            }
            break;

        case wxFONTENCODING_SYSTEM:
            info->xregistry =
            info->xencoding = wxT("*");
            break;

        default:
            // don't know how to translate this encoding into X fontspec
            return FALSE;
    }

    info->encoding = encoding;

    return TRUE;
}

bool wxTestFontEncoding(const wxNativeEncodingInfo& info)
{
    wxString fontspec;
    fontspec.Printf(_T("-*-%s-*-*-*-*-*-*-*-*-*-*-%s-%s"),
                    !info.facename ? _T("*") : info.facename.c_str(),
                    info.xregistry.c_str(),
                    info.xencoding.c_str());

    return wxTestFontSpec(fontspec);
}

// ----------------------------------------------------------------------------
// X-specific functions
// ----------------------------------------------------------------------------

wxNativeFont wxLoadQueryNearestFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool underlined,
                                    const wxString &facename,
                                    wxFontEncoding encoding,
                                    wxString* xFontName)
{
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        encoding = wxFont::GetDefaultEncoding();
    }

    // first determine the encoding - if the font doesn't exist at all in this
    // encoding, it's useless to do all other approximations (i.e. size,
    // family &c don't matter much)
    wxNativeEncodingInfo info;
    if ( encoding == wxFONTENCODING_SYSTEM )
    {
        // This will always work so we don't test to save time
        wxGetNativeFontEncoding(wxFONTENCODING_SYSTEM, &info);
    }
    else
    {
        if ( !wxGetNativeFontEncoding(encoding, &info) ||
             !wxTestFontEncoding(info) )
        {
#if wxUSE_FONTMAP
            if ( !wxFontMapper::Get()->GetAltForEncoding(encoding, &info) )
#endif // wxUSE_FONTMAP
            {
                // unspported encoding - replace it with the default
                //
                // NB: we can't just return 0 from here because wxGTK code doesn't
                //     check for it (i.e. it supposes that we'll always succeed),
                //     so it would provoke a crash
                wxGetNativeFontEncoding(wxFONTENCODING_SYSTEM, &info);
            }
        }
    }

    // OK, we have the correct xregistry/xencoding in info structure
    wxNativeFont font = 0;

    // if we already have the X font name, try to use it
    if( xFontName && !xFontName->IsEmpty() )
    {
        //
        //  Make sure point size is correct for scale factor.
        //
        wxStringTokenizer tokenizer(*xFontName, _T("-"), wxTOKEN_RET_DELIMS);
        wxString newFontName;

        for(int i = 0; i < 8; i++)
          newFontName += tokenizer.NextToken();

        (void) tokenizer.NextToken();

        newFontName += wxString::Format(wxT("%d-"), pointSize);

        while(tokenizer.HasMoreTokens())
          newFontName += tokenizer.GetNextToken();

        font = wxLoadFont(newFontName);

        if(font)
          *xFontName = newFontName;
    }

    if ( !font )
    {
        // search up and down by stepsize 10
        int max_size = pointSize + 20 * (1 + (pointSize/180));
        int min_size = pointSize - 20 * (1 + (pointSize/180));

        int i, round; // counters

        // first round: search for equal, then for smaller and for larger size with the given weight and style
        int testweight = weight;
        int teststyle = style;

        for ( round = 0; round < 3; round++ )
        {
            // second round: use normal weight
            if ( round == 1 )
        {
                if ( testweight != wxNORMAL )
                {
                    testweight = wxNORMAL;
                }
                else
                {
                    ++round; // fall through to third round
                }
            }

            // third round: ... and use normal style
            if ( round == 2 )
            {
                if ( teststyle != wxNORMAL )
                {
                    teststyle = wxNORMAL;
                }
                else
                {
                    break;
                }
            }
            // Search for equal or smaller size (approx.)
            for ( i = pointSize; !font && i >= 10 && i >= min_size; i -= 10 )
            {
                font = wxLoadQueryFont(i, family, teststyle, testweight, underlined,
                                   facename, info.xregistry, info.xencoding,
                                   xFontName);
        }

        // Search for larger size (approx.)
        for ( i = pointSize + 10; !font && i <= max_size; i += 10 )
        {
                font = wxLoadQueryFont(i, family, teststyle, testweight, underlined,
                                   facename, info.xregistry, info.xencoding,
                                   xFontName);
            }
        }

        // Try default family
        if ( !font && family != wxDEFAULT )
        {
            font = wxLoadQueryFont(pointSize, wxDEFAULT, style, weight,
                                   underlined, facename,
                                   info.xregistry, info.xencoding,
                                   xFontName );
        }

        // ignore size, family, style and weight but try to find font with the
        // given facename and encoding
        if ( !font )
        {
            font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
                                   underlined, facename,
                                   info.xregistry, info.xencoding,
                                   xFontName);

            // ignore family as well
            if ( !font )
            {
                font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
                                       underlined, wxEmptyString,
                                       info.xregistry, info.xencoding,
                                       xFontName);

                // if it still failed, try to get the font of any size but
                // with the requested encoding: this can happen if the
                // encoding is only available in one size which happens to be
                // different from 120
                if ( !font )
                {
                    font = wxLoadQueryFont(-1, wxDEFAULT, wxNORMAL, wxNORMAL,
                                           FALSE, wxEmptyString,
                                           info.xregistry, info.xencoding,
                                           xFontName);

                    // this should never happen as we had tested for it in the
                    // very beginning, but if it does, do return something non
                    // NULL or we'd crash in wxFont code
                    if ( !font )
                    {
                        wxFAIL_MSG( _T("this encoding should be available!") );

                        font = wxLoadQueryFont(-1,
                                               wxDEFAULT, wxNORMAL, wxNORMAL,
                                               FALSE, wxEmptyString,
                                               _T("*"), _T("*"),
                                               xFontName);
                    }
                }
            }
        }
    }

    return font;
}

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// returns TRUE if there are any fonts matching this font spec
static bool wxTestFontSpec(const wxString& fontspec)
{
    // some X servers will fail to load this font because there are too many
    // matches so we must test explicitly for this
    if ( fontspec == _T("-*-*-*-*-*-*-*-*-*-*-*-*-*-*") )
    {
        return TRUE;
    }

    wxNativeFont test = (wxNativeFont) g_fontHash->Get( fontspec );
    if (test)
    {
        return TRUE;
    }

    test = wxLoadFont(fontspec);
    g_fontHash->Put( fontspec, (wxObject*) test );

    if ( test )
    {
        wxFreeFont(test);

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static wxNativeFont wxLoadQueryFont(int pointSize,
                                    int family,
                                    int style,
                                    int weight,
                                    bool WXUNUSED(underlined),
                                    const wxString& facename,
                                    const wxString& xregistry,
                                    const wxString& xencoding,
                                    wxString* xFontName)
{
    wxString xfamily;
    switch (family)
    {
        case wxDECORATIVE: xfamily = wxT("lucida"); break;
        case wxROMAN:      xfamily = wxT("times");  break;
        case wxMODERN:     xfamily = wxT("courier"); break;
        case wxSWISS:      xfamily = wxT("helvetica"); break;
        case wxTELETYPE:   xfamily = wxT("lucidatypewriter"); break;
        case wxSCRIPT:     xfamily = wxT("utopia"); break;
        default:           xfamily = wxT("*");
    }
#if wxUSE_NANOX
    int xweight;
    switch (weight)
    {
         case wxBOLD:
             {
                 xweight = MWLF_WEIGHT_BOLD;
                 break;
             }
        case wxLIGHT:
             {
                 xweight = MWLF_WEIGHT_LIGHT;
                 break;
             }
         case wxNORMAL:
             {
                 xweight = MWLF_WEIGHT_NORMAL;
                 break;
             }

     default:
             {
                 xweight = MWLF_WEIGHT_DEFAULT;
                 break;
             }
    }
    GR_SCREEN_INFO screenInfo;
    GrGetScreenInfo(& screenInfo);

    int yPixelsPerCM = screenInfo.ydpcm;

    // A point is 1/72 of an inch.
    // An inch is 2.541 cm.
    // So pixelHeight = (pointSize / 72) (inches) * 2.541 (for cm) * yPixelsPerCM (for pixels)
    // In fact pointSize is 10 * the normal point size so
    // divide by 10.

    int pixelHeight = (int) ( (((float)pointSize) / 720.0) * 2.541 * (float) yPixelsPerCM) ;

    // An alternative: assume that the screen is 72 dpi.
    //int pixelHeight = (int) (((float)pointSize / 720.0) * 72.0) ;
    //int pixelHeight = (int) ((float)pointSize / 10.0) ;

    GR_LOGFONT logFont;
    logFont.lfHeight = pixelHeight;
    logFont.lfWidth = 0;
    logFont.lfEscapement = 0;
    logFont.lfOrientation = 0;
    logFont.lfWeight = xweight;
    logFont.lfItalic = (style == wxNORMAL ? 0 : 1) ;
    logFont.lfUnderline = 0;
    logFont.lfStrikeOut = 0;
    logFont.lfCharSet = MWLF_CHARSET_DEFAULT; // TODO: select appropriate one
    logFont.lfOutPrecision = MWLF_TYPE_DEFAULT;
    logFont.lfClipPrecision = 0; // Not used
    logFont.lfRoman = (family == wxROMAN ? 1 : 0) ;
    logFont.lfSerif = (family == wxSWISS ? 0 : 1) ;
    logFont.lfSansSerif = !logFont.lfSerif ;
    logFont.lfModern = (family == wxMODERN ? 1 : 0) ;
    logFont.lfProportional = (family == wxTELETYPE ? 0 : 1) ;
    logFont.lfOblique = 0;
    logFont.lfSmallCaps = 0;
    logFont.lfPitch = 0; // 0 = default
    strcpy(logFont.lfFaceName, facename.c_str());

    XFontStruct* fontInfo = (XFontStruct*) malloc(sizeof(XFontStruct));
    fontInfo->fid = GrCreateFont((GR_CHAR*) facename.c_str(), pixelHeight, & logFont);
    GrGetFontInfo(fontInfo->fid, & fontInfo->info);
    return (wxNativeFont) fontInfo;

#else
    wxString fontSpec;
    if (!facename.IsEmpty())
    {
        fontSpec.Printf(wxT("-*-%s-*-*-normal-*-*-*-*-*-*-*-*-*"),
                        facename.c_str());

        if ( wxTestFontSpec(fontSpec) )
        {
            xfamily = facename;
        }
        //else: no such family, use default one instead
    }

    wxString xstyle;
    switch (style)
    {
        case wxSLANT:
            fontSpec.Printf(wxT("-*-%s-*-o-*-*-*-*-*-*-*-*-*-*"),
                    xfamily.c_str());
            if ( wxTestFontSpec(fontSpec) )
            {
                xstyle = wxT("o");
                break;
            }
            // fall through - try wxITALIC now

        case wxITALIC:
            fontSpec.Printf(wxT("-*-%s-*-i-*-*-*-*-*-*-*-*-*-*"),
                    xfamily.c_str());
            if ( wxTestFontSpec(fontSpec) )
            {
                xstyle = wxT("i");
            }
            else if ( style == wxITALIC ) // and not wxSLANT
            {
                // try wxSLANT
                fontSpec.Printf(wxT("-*-%s-*-o-*-*-*-*-*-*-*-*-*-*"),
                        xfamily.c_str());
                if ( wxTestFontSpec(fontSpec) )
                {
                    xstyle = wxT("o");
                }
                else
                {
                    // no italic, no slant - leave default
                    xstyle = wxT("*");
                }
            }
            break;

        default:
            wxFAIL_MSG(_T("unknown font style"));
            // fall back to normal

        case wxNORMAL:
            xstyle = wxT("r");
            break;
    }

    wxString xweight;
    switch (weight)
    {
         case wxBOLD:
             {
                  fontSpec.Printf(wxT("-*-%s-bold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("bold");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-heavy-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("heavy");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-extrabold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("extrabold");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-demibold-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("demibold");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-black-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("black");
                      break;
                  }
                  fontSpec.Printf(wxT("-*-%s-ultrablack-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("ultrablack");
                      break;
                  }
              }
              break;
        case wxLIGHT:
             {
                  fontSpec.Printf(wxT("-*-%s-light-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("light");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-thin-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("thin");
                       break;
                  }
             }
             break;
         case wxNORMAL:
             {
                  fontSpec.Printf(wxT("-*-%s-medium-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("medium");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-normal-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                       xweight = wxT("normal");
                       break;
                  }
                  fontSpec.Printf(wxT("-*-%s-regular-*-*-*-*-*-*-*-*-*-*-*"),
                         xfamily.c_str());
                  if ( wxTestFontSpec(fontSpec) )
                  {
                      xweight = wxT("regular");
                      break;
                  }
                  xweight = wxT("*");
              }
              break;
        default:           xweight = wxT("*"); break;
    }

    // if pointSize is -1, don't specify any
    wxString sizeSpec;
    if ( pointSize == -1 )
    {
        sizeSpec = _T('*');
    }
    else
    {
        sizeSpec.Printf(_T("%d"), pointSize);
    }

    // construct the X font spec from our data
    fontSpec.Printf(wxT("-*-%s-%s-%s-normal-*-*-%s-*-*-*-*-%s-%s"),
                    xfamily.c_str(), xweight.c_str(), xstyle.c_str(),
                    sizeSpec.c_str(), xregistry.c_str(), xencoding.c_str());

    if( xFontName )
        *xFontName = fontSpec;

    return wxLoadFont(fontSpec);
#endif
    // wxUSE_NANOX
}

// ----------------------------------------------------------------------------
// wxFontModule
// ----------------------------------------------------------------------------

class wxFontModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxFontModule)
};

IMPLEMENT_DYNAMIC_CLASS(wxFontModule, wxModule)

bool wxFontModule::OnInit()
{
    g_fontHash = new wxHashTable( wxKEY_STRING );

    return TRUE;
}

void wxFontModule::OnExit()
{
    delete g_fontHash;

    g_fontHash = (wxHashTable *)NULL;
}

#endif // GTK 2.0/1.x

