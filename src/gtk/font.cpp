/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/font.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "font.h"
#endif

#include "wx/font.h"
#include "wx/fontutil.h"
#include "wx/cmndata.h"
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/gdicmn.h"
#include "wx/tokenzr.h"
#include "wx/settings.h"

#include <strings.h>

#include "wx/gtk/private.h"
#include <gdk/gdkprivate.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the default size (in points) for the fonts
static const int wxDEFAULT_FONT_SIZE = 12;

// ----------------------------------------------------------------------------
// wxScaledFontList
// ----------------------------------------------------------------------------

// TODO: replace this with a type safe list or hash!!
class wxScaledFontList : public wxList
{
public:
    wxScaledFontList() : wxList(wxKEY_INTEGER) { }
};

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

class wxFontRefData : public wxObjectRefData
{
public:
    // from broken down font parameters, also default ctor
    wxFontRefData(int size = -1,
                  int family = wxFONTFAMILY_DEFAULT,
                  int style = wxFONTSTYLE_NORMAL,
                  int weight = wxFONTWEIGHT_NORMAL,
                  bool underlined = FALSE,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // from XFLD
    wxFontRefData(const wxString& fontname);

    // copy ctor
    wxFontRefData( const wxFontRefData& data );

    virtual ~wxFontRefData();

    // do we have the native font info?
    bool HasNativeFont() const
    {
        return !m_nativeFontInfo.IsDefault();
    }

    // setters: all of them also take care to modify m_nativeFontInfo if we
    // have it so as to not lose the information not carried by our fields
    void SetPointSize(int pointSize);
    void SetFamily(int family);
    void SetStyle(int style);
    void SetWeight(int weight);
    void SetUnderlined(bool underlined);
    void SetFaceName(const wxString& facename);
    void SetEncoding(wxFontEncoding encoding);

    // debugger helper: shows what the font really is
    //
    // VZ: I need this as my gdb either shows wildly wrong values or crashes
    //     when I ask it to "p fontRefData" :-(
#ifdef __WXDEBUG__
    void Dump() const
    {
        wxPrintf(_T("%s-%s-%s-%d-%d\n"),
                 m_faceName.c_str(),
                 m_weight == wxFONTWEIGHT_NORMAL
                    ? _T("normal")
                    : m_weight == wxFONTWEIGHT_BOLD
                        ? _T("bold")
                        : _T("light"),
                 m_style == wxFONTSTYLE_NORMAL ? _T("regular") : _T("italic"),
                 m_pointSize,
                 m_encoding);
    }
#endif // Debug

protected:
    // common part of all ctors
    void Init(int pointSize,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

private:
    // the map of font sizes to "GdkFont *"
    wxScaledFontList  m_scaled_xfonts;

    // the broken down font parameters
    int             m_pointSize;
    int             m_family,
                    m_style,
                    m_weight;
    bool            m_underlined;
    wxString        m_faceName;
    wxFontEncoding  m_encoding;

    // the native font info, basicly an XFLD
    wxNativeFontInfo m_nativeFontInfo;

    friend class wxFont;
};

// ============================================================================
// wxFontRefData implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontRefData creation
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         int family,
                         int style,
                         int weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_family = family == wxFONTFAMILY_DEFAULT ? wxFONTFAMILY_SWISS : family;

    m_faceName = faceName;

    // we accept both wxDEFAULT and wxNORMAL here - should we?
    m_style = style == wxDEFAULT ? wxFONTSTYLE_NORMAL : style;
    m_weight = weight == wxDEFAULT ? wxFONTWEIGHT_NORMAL : weight;

    // and here, do we really want to forbid creation of the font of the size
    // 90 (the value of wxDEFAULT)??
    m_pointSize = pointSize == wxDEFAULT ||
                    pointSize == -1 ? wxDEFAULT_FONT_SIZE : pointSize;

    m_underlined = underlined;
    m_encoding = encoding;
}

wxFontRefData::wxFontRefData( const wxFontRefData& data )
    : wxObjectRefData()
{
    m_pointSize = data.m_pointSize;
    m_family = data.m_family;
    m_style = data.m_style;
    m_weight = data.m_weight;

    m_underlined = data.m_underlined;

    m_faceName = data.m_faceName;
    m_encoding = data.m_encoding;

    m_nativeFontInfo = data.m_nativeFontInfo;
}

wxFontRefData::wxFontRefData(int size, int family, int style,
                             int weight, bool underlined,
                             const wxString& faceName,
                             wxFontEncoding encoding)
{
    Init(size, family, style, weight, underlined, faceName, encoding);
}

wxFontRefData::wxFontRefData(const wxString& fontname)
{
    // remember the X font name
    m_nativeFontInfo.SetXFontName(fontname);

    // get the font parameters from the XLFD
    // -------------------------------------

    m_faceName = m_nativeFontInfo.GetXFontComponent(wxXLFD_FAMILY);

    m_weight = wxFONTWEIGHT_NORMAL;

    wxString w = m_nativeFontInfo.GetXFontComponent(wxXLFD_WEIGHT).Upper();
    if ( !w.empty() && w != _T('*') )
    {
        // the test below catches all of BOLD, EXTRABOLD, DEMIBOLD, ULTRABOLD
        // and BLACK
        if ( ((w[0u] == _T('B') && (!strcmp(w.c_str() + 1, _T("OLD")) ||
                                   !strcmp(w.c_str() + 1, _T("LACK"))))) ||
             strstr(w.c_str() + 1, _T("BOLD")) )
        {
            m_weight = wxFONTWEIGHT_BOLD;
        }
        else if ( w == _T("LIGHT") || w == _T("THIN") )
        {
            m_weight = wxFONTWEIGHT_LIGHT;
        }
    }

    switch ( wxToupper(*m_nativeFontInfo.
                            GetXFontComponent(wxXLFD_SLANT).c_str()) )
    {
        case _T('I'):   // italique
            m_style = wxFONTSTYLE_ITALIC;
            break;

        case _T('O'):   // oblique
            m_style = wxFONTSTYLE_SLANT;
            break;

        default:
            m_style = wxFONTSTYLE_NORMAL;
    }

    long ptSize;
    if ( m_nativeFontInfo.GetXFontComponent(wxXLFD_POINTSIZE).ToLong(&ptSize) )
    {
        // size in XLFD is in 10 point units
        m_pointSize = (int)(ptSize / 10);
    }
    else
    {
        m_pointSize = wxDEFAULT_FONT_SIZE;
    }

    // examine the spacing: if the font is monospaced, assume wxTELETYPE
    // family for compatibility with the old code which used it instead of
    // IsFixedWidth()
    if ( m_nativeFontInfo.GetXFontComponent(wxXLFD_SPACING).Upper() == _T('M') )
    {
        m_family = wxFONTFAMILY_TELETYPE;
    }
    else // not monospaceed
    {
        // don't even try guessing it, it doesn't work for too many fonts
        // anyhow
        m_family = wxFONTFAMILY_UNKNOWN;
    }

    // X fonts are never underlined...
    m_underlined = FALSE;

    // deal with font encoding
    wxString
        registry = m_nativeFontInfo.GetXFontComponent(wxXLFD_REGISTRY).Upper(),
        encoding = m_nativeFontInfo.GetXFontComponent(wxXLFD_ENCODING).Upper();

    if ( registry == _T("ISO8859") )
    {
        int cp;
        if ( wxSscanf(encoding, wxT("%d"), &cp) == 1 )
        {
            m_encoding = (wxFontEncoding)(wxFONTENCODING_ISO8859_1 + cp - 1);
        }
    }
    else if ( registry == _T("MICROSOFT") )
    {
        int cp;
        if ( wxSscanf(encoding, wxT("cp125%d"), &cp) == 1 )
        {
            m_encoding = (wxFontEncoding)(wxFONTENCODING_CP1250 + cp);
        }
    }
    else if ( registry == _T("KOI8") )
    {
        m_encoding = wxFONTENCODING_KOI8;
    }
    else // unknown encoding
    {
        // may be give a warning here?
        m_encoding = wxFONTENCODING_SYSTEM;
    }
}

wxFontRefData::~wxFontRefData()
{
    wxNode *node = m_scaled_xfonts.First();
    while (node)
    {
        GdkFont *font = (GdkFont*)node->Data();
        wxNode *next = node->Next();
        gdk_font_unref( font );
        node = next;
    }
}

// ----------------------------------------------------------------------------
// wxFontRefData SetXXX()
// ----------------------------------------------------------------------------

void wxFontRefData::SetPointSize(int pointSize)
{
    m_pointSize = pointSize;

    if ( HasNativeFont() )
    {
        wxString size;
        if ( pointSize == -1 )
            size = _T('*');
        else
            size.Printf(_T("%d"), 10*pointSize);

        m_nativeFontInfo.SetXFontComponent(wxXLFD_POINTSIZE, size);
    }
}

void wxFontRefData::SetFamily(int family)
{
    m_family = family;

    // TODO: what are we supposed to do with m_nativeFontInfo here?
}

void wxFontRefData::SetStyle(int style)
{
    m_style = style;

    if ( HasNativeFont() )
    {
        wxString slant;
        switch ( style )
        {
            case wxFONTSTYLE_ITALIC:
                slant = _T('i');
                break;

            case wxFONTSTYLE_SLANT:
                slant = _T('o');
                break;

            default:
                wxFAIL_MSG( _T("unknown font style") );
                // fall through

            case wxFONTSTYLE_NORMAL:
                slant = _T('r');
        }

        m_nativeFontInfo.SetXFontComponent(wxXLFD_SLANT, slant);
    }
}

void wxFontRefData::SetWeight(int weight)
{
    m_weight = weight;

    if ( HasNativeFont() )
    {
        wxString boldness;
        switch ( weight )
        {
            case wxFONTWEIGHT_BOLD:
                boldness = _T("bold");
                break;

            case wxFONTWEIGHT_LIGHT:
                boldness = _T("light");
                break;

            default:
                wxFAIL_MSG( _T("unknown font weight") );
                // fall through

            case wxFONTWEIGHT_NORMAL:
                // unspecified
                boldness = _T("medium");
        }

        m_nativeFontInfo.SetXFontComponent(wxXLFD_WEIGHT, boldness);
    }
}

void wxFontRefData::SetUnderlined(bool underlined)
{
    m_underlined = underlined;

    // the XLFD doesn't have "underlined" field anyhow
}

void wxFontRefData::SetFaceName(const wxString& facename)
{
    m_faceName = facename;

    if ( HasNativeFont() )
    {
        m_nativeFontInfo.SetXFontComponent(wxXLFD_FAMILY, facename);
    }
}

void wxFontRefData::SetEncoding(wxFontEncoding encoding)
{
    m_encoding = encoding;

    if ( HasNativeFont() )
    {
        wxNativeEncodingInfo info;
        if ( wxGetNativeFontEncoding(encoding, &info) )
        {
            m_nativeFontInfo.SetXFontComponent(wxXLFD_REGISTRY, info.xregistry);
            m_nativeFontInfo.SetXFontComponent(wxXLFD_ENCODING, info.xencoding);
        }
    }
}

// ============================================================================
// wxFont implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFont creation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

void wxFont::Init()
{
}

wxFont::wxFont(const wxNativeFontInfo& info)
{
    Init();

    Create(info.GetXFontName());
}

bool wxFont::Create( int pointSize,
                     int family,
                     int style,
                     int weight,
                     bool underlined,
                     const wxString& face,
                     wxFontEncoding encoding)
{
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, face, encoding);

    return TRUE;
}

bool wxFont::Create(const wxString& fontname)
{
    // VZ: does this really happen?
    if ( fontname.empty() )
    {
        *this = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

        return TRUE;
    }

    m_refData = new wxFontRefData(fontname);

    return TRUE;
}

void wxFont::Unshare()
{
    if (!m_refData)
    {
        m_refData = new wxFontRefData();
    }
    else
    {
        wxFontRefData* ref = new wxFontRefData(*(wxFontRefData*)m_refData);
        UnRef();
        m_refData = ref;
    }
}

wxFont::~wxFont()
{
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

// all accessors are just forwarded to wxFontRefData which has everything we
// need

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), wxT(""), wxT("invalid font") );

    return M_FONTDATA->m_faceName;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid font") );

    return M_FONTDATA->m_underlined;
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->m_encoding;
}

wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( Ok(), (wxNativeFontInfo *)NULL, wxT("invalid font") );

    if ( M_FONTDATA->m_nativeFontInfo.GetXFontName().empty() )
        GetInternalFont();

    return new wxNativeFontInfo(M_FONTDATA->m_nativeFontInfo);
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid font") );

    if ( M_FONTDATA->HasNativeFont() )
    {
        // the monospace fonts are supposed to have "M" in the spacing field
        wxString spacing = M_FONTDATA->
                            m_nativeFontInfo.GetXFontComponent(wxXLFD_SPACING);

        return spacing.Upper() == _T('M');
    }

    return wxFontBase::IsFixedWidth();
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    Unshare();

    M_FONTDATA->SetPointSize(pointSize);
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->SetFamily(family);
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->SetStyle(style);
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->SetWeight(weight);
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->SetEncoding(encoding);
}

void wxFont::SetNativeFontInfo(const wxNativeFontInfo& info)
{
    Unshare();

    M_FONTDATA->m_nativeFontInfo = info;
}

// ----------------------------------------------------------------------------
// get internal representation of font
// ----------------------------------------------------------------------------

static GdkFont *g_systemDefaultGuiFont = (GdkFont*) NULL;

// this is also used from tbargtk.cpp and tooltip.cpp, hence extern
extern GdkFont *GtkGetDefaultGuiFont()
{
    if (!g_systemDefaultGuiFont)
    {
        GtkWidget *widget = gtk_button_new();
        GtkStyle *def = gtk_rc_get_style( widget );
        if (def)
        {
            g_systemDefaultGuiFont = gdk_font_ref( GET_STYLE_FONT(def) );
        }
        else
        {
            def = gtk_widget_get_default_style();
            if (def)
                g_systemDefaultGuiFont = gdk_font_ref( GET_STYLE_FONT(def) );
        }
        gtk_widget_destroy( widget );
    }
    else
    {
        // already have it, but ref it once more before returning
        gdk_font_ref(g_systemDefaultGuiFont);
    }

    return g_systemDefaultGuiFont;
}

GdkFont *wxFont::GetInternalFont( float scale ) const
{
    GdkFont *font = (GdkFont *) NULL;

    wxCHECK_MSG( Ok(), font, wxT("invalid font") )

    long int_scale = long(scale * 100.0 + 0.5); /* key for fontlist */
    int point_scale = (int)((M_FONTDATA->m_pointSize * 10 * int_scale) / 100);

    wxNode *node = M_FONTDATA->m_scaled_xfonts.Find(int_scale);
    if (node)
    {
        font = (GdkFont*)node->Data();
    }
    else // we don't have this font in this size yet
    {
        if (*this == wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT))
        {
            font = GtkGetDefaultGuiFont();
        }

        if ( !font )
        {
            // do we have the XLFD?
            if ( M_FONTDATA->HasNativeFont() )
            {
                font = wxLoadFont(M_FONTDATA->m_nativeFontInfo.GetXFontName());
            }

            // no XLFD of no exact match - try the approximate one now
            if ( !font )
            {
                wxString xfontname;
                font = wxLoadQueryNearestFont( point_scale,
                                               M_FONTDATA->m_family,
                                               M_FONTDATA->m_style,
                                               M_FONTDATA->m_weight,
                                               M_FONTDATA->m_underlined,
                                               M_FONTDATA->m_faceName,
                                               M_FONTDATA->m_encoding,
                                               &xfontname);
                if ( font )
                {
                    M_FONTDATA->m_nativeFontInfo.SetXFontName(xfontname);
                }
            }
        }

        if ( font )
        {
            M_FONTDATA->m_scaled_xfonts.Append( int_scale, (wxObject*)font );
        }
    }

    // it's quite useless to make it a wxCHECK because we're going to crash
    // anyhow...
    wxASSERT_MSG( font, wxT("could not load any font?") );

    return font;
}

