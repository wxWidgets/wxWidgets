/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
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
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/gdicmn.h"
#include "wx/tokenzr.h"

#include <strings.h>

#include "gdk/gdk.h"

// ----------------------------------------------------------------------------
// local data
// ----------------------------------------------------------------------------

#if wxUSE_FONTNAMEDIRECTORY
    extern wxFontNameDirectory *wxTheFontNameDirectory;
#endif

// ----------------------------------------------------------------------------
// private functions
// ----------------------------------------------------------------------------

// returns TRUE if there are any fonts matching this font spec
static bool wxTestFontSpec(const wxString& fontspec);

static GdkFont *wxLoadQueryFont( int pointSize,
                                 int family,
                                 int style,
                                 int weight,
                                 bool underlined,
                                 const wxString &facename,
                                 wxFontEncoding encoding );

static GdkFont *wxLoadQueryNearestFont( int pointSize,
                                        int family,
                                        int style,
                                        int weight,
                                        bool underlined,
                                        const wxString &facename,
                                        wxFontEncoding encoding);

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

class wxFontRefData : public wxObjectRefData
{
public:
    wxFontRefData(int size = wxDEFAULT,
                  int family = wxDEFAULT,
                  int style = wxDEFAULT,
                  int weight = wxDEFAULT,
                  bool underlined = FALSE,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
        : m_scaled_xfonts(wxKEY_INTEGER)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    wxFontRefData( const wxFontRefData& data );

    virtual ~wxFontRefData();

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
    wxList    m_scaled_xfonts;

    int       m_pointSize;
    int       m_family,
              m_style,
              m_weight;
    bool      m_underlined;
    wxString  m_faceName;
    wxFontEncoding m_encoding;

    bool      m_byXFontName;
    GdkFont  *m_font;

    friend wxFont;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         int family,
                         int style,
                         int weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    if (family == wxDEFAULT)
        m_family = wxSWISS;
    else
        m_family = family;

    m_faceName = faceName;

    if (style == wxDEFAULT)
        m_style = wxNORMAL;
    else
        m_style = style;

    if (weight == wxDEFAULT)
        m_weight = wxNORMAL;
    else
        m_weight = weight;

    if (pointSize == wxDEFAULT)
        m_pointSize = 12;
    else
        m_pointSize = pointSize;

    m_underlined = underlined;
    m_encoding = encoding;

    m_byXFontName = FALSE;
    m_font = (GdkFont *) NULL;
}

wxFontRefData::wxFontRefData( const wxFontRefData& data )
             : m_scaled_xfonts(wxKEY_INTEGER)
{
    Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
         data.m_underlined, data.m_faceName, data.m_encoding);

    if (data.m_font)
        m_font = gdk_font_ref( data.m_font );
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

    if (m_font)
        gdk_font_unref( m_font );
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

void wxFont::Init()
{
    if (wxTheFontList)
        wxTheFontList->Append( this );
}

wxFont::wxFont( GdkFont *font, char *xFontName )
{
    if (!xFontName)
        return;

    // VZ: this ctor ddidn't append the font to wxTheFontList before, but
    //     there is no reason to not do it, is there?
    Init();

    m_refData = new wxFontRefData();

//  M_FONTDATA->m_byXFontName = TRUE;
    M_FONTDATA->m_font = font;

    wxString tmp;

    wxString fontname( xFontName );
    wxStringTokenizer tn( fontname, _T("-") );

    tn.GetNextToken();                           // foundry

    M_FONTDATA->m_faceName = tn.GetNextToken();  // courier

    tmp = tn.GetNextToken().MakeUpper();
    if (tmp == _T("BOLD")) M_FONTDATA->m_weight = wxBOLD;

    tmp = tn.GetNextToken().MakeUpper();
    if (tmp == _T("I")) M_FONTDATA->m_style = wxITALIC;
    if (tmp == _T("O")) M_FONTDATA->m_style = wxITALIC;

    tn.GetNextToken();                           // set width
    tn.GetNextToken();                           // ?
    tn.GetNextToken();                           // pixel size

    tmp = tn.GetNextToken();                     // pointsize
    int num =  wxStrtol (tmp.c_str(), (wxChar **) NULL, 10);
    M_FONTDATA->m_pointSize = num / 10;

    tn.GetNextToken();                           // x-res
    tn.GetNextToken();                           // y-res

    tmp = tn.GetNextToken().MakeUpper();
    if (tmp == _T("M")) M_FONTDATA->m_family = wxMODERN;
    else if (M_FONTDATA->m_faceName == _T("TIMES")) M_FONTDATA->m_family = wxROMAN;
    else if (M_FONTDATA->m_faceName == _T("HELVETICA")) M_FONTDATA->m_family = wxSWISS;
    else if (M_FONTDATA->m_faceName == _T("LUCIDATYPEWRITER")) M_FONTDATA->m_family = wxTELETYPE;
    else if (M_FONTDATA->m_faceName == _T("LUCIDA")) M_FONTDATA->m_family = wxDECORATIVE;
    else if (M_FONTDATA->m_faceName == _T("UTOPIA")) M_FONTDATA->m_family = wxSCRIPT;
}

bool wxFont::Create( int pointSize,
                     int family,
                     int style,
                     int weight,
                     bool underlined,
                     const wxString& face,
                     wxFontEncoding encoding )
{
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, face, encoding);

    Init();

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
    if (wxTheFontList)
        wxTheFontList->DeleteObject( this );
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid font") );

    return M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), _T(""), _T("invalid font") );

    return M_FONTDATA->m_faceName;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid font") );

    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid font") );

    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, _T("invalid font") );

    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( Ok(), FALSE, _T("invalid font") );

    return M_FONTDATA->m_underlined;
}


wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, _T("invalid font") );

    return M_FONTDATA->m_encoding;
}

// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    Unshare();

    M_FONTDATA->m_pointSize = pointSize;
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->m_family = family;
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->m_style = style;
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->m_weight = weight;
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_faceName = faceName;
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->m_underlined = underlined;
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->m_encoding = encoding;
}

// ----------------------------------------------------------------------------
// get internal representation of font
// ----------------------------------------------------------------------------

GdkFont *wxFont::GetInternalFont( float scale ) const
{
    if (!Ok())
    {
        wxFAIL_MSG( _T("invalid font") );

        return (GdkFont*) NULL;
    }

    /* short cut if the special X font constructor has been used */
    if (M_FONTDATA->m_byXFontName)
        return M_FONTDATA->m_font;

    long int_scale = long(scale * 100.0 + 0.5); /* key for fontlist */
    int point_scale = (M_FONTDATA->m_pointSize * 10 * int_scale) / 100;
    GdkFont *font = (GdkFont *) NULL;

    wxNode *node = M_FONTDATA->m_scaled_xfonts.Find(int_scale);
    if (node)
    {
        font = (GdkFont*)node->Data();
    }
    else
    {
#if 0
        if ((int_scale == 100) &&
                (M_FONTDATA->m_family == wxSWISS) &&
                (M_FONTDATA->m_style == wxNORMAL) &&
                (M_FONTDATA->m_pointSize == 12) &&
                (M_FONTDATA->m_weight == wxNORMAL) &&
                (M_FONTDATA->m_underlined == FALSE))
        {
            font = gdk_font_load( "-adobe-helvetica-medium-r-normal--*-120-*-*-*-*-*-*" );
        }
        else
#endif // 0
        {
            font = wxLoadQueryNearestFont( point_scale,
                                           M_FONTDATA->m_family,
                                           M_FONTDATA->m_style,
                                           M_FONTDATA->m_weight,
                                           M_FONTDATA->m_underlined,
                                           M_FONTDATA->m_faceName,
                                           M_FONTDATA->m_encoding );
        }

        M_FONTDATA->m_scaled_xfonts.Append( int_scale, (wxObject*)font );
    }

    if (!font)
    {
        wxLogError(_T("could not load any font"));
    }

    return font;
}

//-----------------------------------------------------------------------------
// local utilities to find a X font
//-----------------------------------------------------------------------------

// returns TRUE if there are any fonts matching this font spec
static bool wxTestFontSpec(const wxString& fontSpec)
{
    GdkFont *test = gdk_font_load( wxConvCurrent->cWX2MB(fontSpec) );
    if ( test )
    {
        gdk_font_unref( test );

        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

static GdkFont *wxLoadQueryFont( int pointSize,
                                 int family,
                                 int style,
                                 int weight,
                                 bool WXUNUSED(underlined),
                                 const wxString &facename,
                                 wxFontEncoding encoding )
{
    wxString xfamily;
    switch (family)
    {
        case wxDECORATIVE: xfamily = _T("lucida"); break;
        case wxROMAN:      xfamily = _T("times");  break;
        case wxMODERN:     xfamily = _T("courier"); break;
        case wxSWISS:      xfamily = _T("helvetica"); break;
        case wxTELETYPE:   xfamily = _T("lucidatypewriter"); break;
        case wxSCRIPT:     xfamily = _T("utopia"); break;
        default:           xfamily = _T("*");
    }

    wxString fontSpec;
    if (!facename.IsEmpty())
    {
        fontSpec.Printf(_T("-*-%s-*-*-normal-*-*-*-*-*-*-*-*-*"),
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
        case wxITALIC:     xstyle = _T("i"); break;
        case wxSLANT:      xstyle = _T("o"); break;
        case wxNORMAL:     xstyle = _T("r"); break;
        default:           xstyle = _T("*"); break;
    }

    wxString xweight;
    switch (weight)
    {
        case wxBOLD:       xweight = _T("bold"); break;
        case wxLIGHT:
        case wxNORMAL:     xweight = _T("medium"); break;
        default:           xweight = _T("*"); break;
    }

    wxString xregistry, xencoding;
    if ( encoding == wxFONTENCODING_DEFAULT )
    {
        // use the apps default
        encoding = wxFont::GetDefaultEncoding();
    }

    bool test = TRUE;   // should we test for availability of encoding?
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
        case wxFONTENCODING_ISO8859_13:
        case wxFONTENCODING_ISO8859_14:
        case wxFONTENCODING_ISO8859_15:
            {
                int cp = encoding - wxFONTENCODING_ISO8859_1 + 1;
                xregistry = _T("iso8859");
                xencoding.Printf(_T("%d"), cp);
            }
            break;

        case wxFONTENCODING_KOI8:
            xregistry = _T("koi8");
            if ( wxTestFontSpec(_T("-*-*-*-*-*-*-*-*-*-*-*-*-koi8-1")) )
            {
                xencoding = _T("1");

                // test passed, no need to do it once more
                test = FALSE;
            }
            else
            {
                xencoding = _T("*");
            }
            break;

        case wxFONTENCODING_CP1250:
        case wxFONTENCODING_CP1251:
        case wxFONTENCODING_CP1252:
            {
                int cp = encoding - wxFONTENCODING_CP1250 + 1250;
                fontSpec.Printf(_T("-*-*-*-*-*-*-*-*-*-*-*-*-microsoft-cp%d"),
                                cp);
                if ( wxTestFontSpec(fontSpec) )
                {
                    xregistry = _T("microsoft");
                    xencoding.Printf(_T("cp%d"), cp);

                    // test passed, no need to do it once more
                    test = FALSE;
                }
                else
                {
                    // fall back to LatinX
                    xregistry = _T("iso8859");
                    xencoding.Printf(_T("%d"), cp - 1249);
                }
            }
            break;

        case wxFONTENCODING_SYSTEM:
        default:
            test = FALSE;
            xregistry =
            xencoding = _T("*");
    }

    if ( test )
    {
        fontSpec.Printf(_T("-*-*-*-*-*-*-*-*-*-*-*-*-%s-%s"),
                        xregistry.c_str(), xencoding.c_str());
        if ( !wxTestFontSpec(fontSpec) )
        {
            // this encoding isn't available - what to do?
            xregistry =
            xencoding = _T("*");
        }
    }

    // construct the X font spec from our data
    fontSpec.Printf(_T("-*-%s-%s-%s-normal-*-*-%d-*-*-*-*-%s-%s"),
                    xfamily.c_str(), xweight.c_str(), xstyle.c_str(),
                    pointSize, xregistry.c_str(), xencoding.c_str());

    return gdk_font_load( wxConvCurrent->cWX2MB(fontSpec) );
}

static GdkFont *wxLoadQueryNearestFont( int pointSize,
                                        int family,
                                        int style,
                                        int weight,
                                        bool underlined,
                                        const wxString &facename,
                                        wxFontEncoding encoding )
{
    GdkFont *font = wxLoadQueryFont( pointSize, family, style, weight,
                                     underlined, facename, encoding );

    if (!font)
    {
        /* search up and down by stepsize 10 */
        int max_size = pointSize + 20 * (1 + (pointSize/180));
        int min_size = pointSize - 20 * (1 + (pointSize/180));

        int i;

        /* Search for smaller size (approx.) */
        for ( i = pointSize - 10; !font && i >= 10 && i >= min_size; i -= 10 )
        {
            font = wxLoadQueryFont(i, family, style, weight, underlined,
                                   facename, encoding );
        }

        /* Search for larger size (approx.) */
        for ( i = pointSize + 10; !font && i <= max_size; i += 10 )
        {
            font = wxLoadQueryFont( i, family, style, weight, underlined,
                                    facename, encoding );
        }

        /* Try default family */
        if ( !font && family != wxDEFAULT )
        {
            font = wxLoadQueryFont( pointSize, wxDEFAULT, style, weight,
                                    underlined, facename, encoding );
        }

        /* Bogus font */
        if ( !font )
        {
            font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
                                   underlined, facename, encoding );
        }
    }

    return font;
}

// wow, what's this stuff? Is it used/useful? (VZ)
#if 0

//-----------------------------------------------------------------------------
// face names and index functions
//-----------------------------------------------------------------------------

static char *font_defaults[] = {
    "FamilyDefault", "Default",
    "FamilyRoman", "Roman",
    "FamilyDecorative", "Decorative",
    "FamilyModern", "Modern",
    "FamilyTeletype", "Teletype",
    "FamilySwiss", "Swiss",
    "FamilyScript", "Script",

    "AfmMedium", "",
    "AfmBold", "Bo",
    "AfmLight", "",
    "AfmStraight", "",
    "AfmItalic", "${AfmSlant}",
    "AfmSlant", "O",
    "AfmRoman", "Ro",
    "AfmTimes", "Times",
    "AfmHelvetica", "Helv",
    "AfmCourier", "Cour",

    "Afm___", "${AfmTimes,$[weight],$[style]}",

    "AfmTimes__", "${AfmTimes}${Afm$[weight]}${Afm$[style]}",
    "AfmTimesMediumStraight", "${AfmTimes}${AfmRoman}",
    "AfmTimesLightStraight", "${AfmTimes}${AfmRoman}",
    "AfmTimes_Italic", "${AfmTimes}$[weight]${AfmItalic}",
    "AfmTimes_Slant", "${AfmTimes}$[weight]${AfmItalic}",

    "AfmSwiss__", "${AfmHelvetica}${Afm$[weight]}${Afm$[style]}",
    "AfmModern__", "${AfmCourier}${Afm$[weight]}${Afm$[style]}",

    "AfmTeletype__", "${AfmModern,$[weight],$[style]}",

    "PostScriptMediumStraight", "",
    "PostScriptMediumItalic", "-Oblique",
    "PostScriptMediumSlant", "-Oblique",
    "PostScriptLightStraight", "",
    "PostScriptLightItalic", "-Oblique",
    "PostScriptLightSlant", "-Oblique",
    "PostScriptBoldStraight", "-Bold",
    "PostScriptBoldItalic", "-BoldOblique",
    "PostScriptBoldSlant", "-BoldOblique",

#if WX_NORMALIZED_PS_FONTS
    "PostScript___", "${PostScriptTimes,$[weight],$[style]}",
#else
    "PostScriptRoman__", "${PostScriptTimes,$[weight],$[style]}",
    "PostScript___", "LucidaSans${PostScript$[weight]$[style]}",
#endif

    "PostScriptTimesMedium", "",
    "PostScriptTimesLight", "",
    "PostScriptTimesBold", "Bold",

    "PostScriptTimes__", "Times${PostScript$[weight]$[style]}",
    "PostScriptTimesMediumStraight", "Times-Roman",
    "PostScriptTimesLightStraight", "Times-Roman",
    "PostScriptTimes_Slant", "Times-${PostScriptTimes$[weight]}Italic",
    "PostScriptTimes_Italic", "Times-${PostScriptTimes$[weight]}Italic",

    "PostScriptSwiss__", "Helvetica${PostScript$[weight]$[style]}",
    "PostScriptModern__", "Courier${PostScript$[weight]$[style]}",

    "PostScriptTeletype__", "${PostScriptModern,$[weight],$[style]}",

#if !WX_NORMALIZED_PS_FONTS
    "PostScriptScript__", "Zapf-Chancery-MediumItalic",
#endif

    "ScreenMedium", "medium",
    "ScreenBold", "bold",
    "ScreenLight", "light",
    "ScreenStraight", "r",
    "ScreenItalic", "i",
    "ScreenSlant", "o",

    "ScreenDefaultBase", "*-times",

    "ScreenRomanBase", "*-times",
    "ScreenDecorativeBase", "*-helvetica",
    "ScreenModernBase", "*-courier",
    "ScreenTeletypeBase", "*-lucidatypewriter",
    "ScreenSwissBase", "*-lucida",
    "ScreenScriptBase", "*-zapfchancery",

    "ScreenStdSuffix", "-${Screen$[weight]}-${Screen$[style]}"
        "-normal-*-*-%d-*-*-*-*-*-*",

    "Screen___",
    "-${ScreenDefaultBase}${ScreenStdSuffix}",
    "ScreenRoman__",
    "-${ScreenRomanBase}${ScreenStdSuffix}",
    "ScreenDecorative__",
    "-${ScreenDecorativeBase}${ScreenStdSuffix}",
    "ScreenModern__",
    "-${ScreenModernBase}${ScreenStdSuffix}",
    "ScreenTeletype__",
    "-${ScreenTeletypeBase}${ScreenStdSuffix}",
    "ScreenSwiss__",
    "-${ScreenSwissBase}${ScreenStdSuffix}",
    "ScreenScript__",
    "-${ScreenScriptBase}${ScreenStdSuffix}",
    (char *) NULL
};

enum {wxWEIGHT_NORMAL, wxWEIGHT_BOLD,  wxWEIGHT_LIGHT, wxNUM_WEIGHTS};
enum {wxSTYLE_NORMAL,  wxSTYLE_ITALIC, wxSTYLE_SLANT,  wxNUM_STYLES};

static int WCoordinate(int w)
{
    switch (w)
    {
        case wxBOLD:   return wxWEIGHT_BOLD;
        case wxLIGHT:  return wxWEIGHT_LIGHT;
        case wxNORMAL:
        default:       return wxWEIGHT_NORMAL;
    }
};

static int SCoordinate(int s)
{
    switch (s)
    {
        case wxITALIC: return wxSTYLE_ITALIC;
        case wxSLANT:  return wxSTYLE_SLANT;
        case wxNORMAL:
        default:       return wxSTYLE_NORMAL;
    }
};

//-----------------------------------------------------------------------------
// wxSuffixMap
//-----------------------------------------------------------------------------

class wxSuffixMap
{
public:
    ~wxSuffixMap();

    inline char *GetName(int weight, int style)
    {
        return ( map [WCoordinate(weight)] [SCoordinate(style)] );
    }

    char *map[wxNUM_WEIGHTS][wxNUM_STYLES];
    void Initialize(const char *, const char *);
};

static void SearchResource(const char *prefix, const char **names, int count, char **v)
{
    int k, i, j;
    char resource[1024], **defaults, *internal;

    k = 1 << count;

    *v = (char *) NULL;
    internal = (char *) NULL;

    for (i = 0; i < k; i++)
    {
        strcpy(resource, prefix);
        for (j = 0; j < count; j++)
        {
            // upon failure to find a matching fontname
            //   in the default fonts above, we substitute more
            //   and more values by _ so that at last ScreenMyFontBoldNormal
            //   would turn into Screen___ and this will then get
            //   converted to -${ScreenDefaultBase}${ScreenStdSuffix}

            if (!(i & (1 << j)))
                strcat(resource, names[j]);
            else
                strcat(resource, "_");
        }

        // we previously search the Xt-resources here

        if (!internal)
        {
            defaults = font_defaults;
            while (*defaults)
            {
                if (!strcmp(*defaults, resource))
                {
                    internal = defaults[1];
                    break;
                }
                defaults += 2;
            }
        }
    }

    if (internal)
    {
        if ((strcmp(internal,"-${ScreenDefaultBase}${ScreenStdSuffix}") == 0) &&
            (strcmp(names[0], "Default") != 0))
        {
            // we did not find any font name in the standard list.
            //   this can (hopefully does) mean that someone supplied
            //   the facename in the wxFont constructor so we insert
            //   it here

            strcpy( resource,"-*-" );                  // any producer
            strcat( resource, names[0] );              // facename
            strcat( resource, "${ScreenStdSuffix}" );  // add size params later on
            *v = copystring(resource);
        }
        else
        {
            *v = copystring(internal);
        }
    }
}

wxSuffixMap::~wxSuffixMap()
{
    int k, j;

    for (k = 0; k < wxNUM_WEIGHTS; ++k)
        for (j = 0; j < wxNUM_STYLES; ++j)
            if (map[k][j])
            {
                delete[] map[k][j];
                map[k][j] = (char *) NULL;
            }
}

void wxSuffixMap::Initialize(const char *resname, const char *devresname)
{
    const char *weight, *style;
    char *v;
    int i, j, k;
    const char *names[3];

    for (k = 0; k < wxNUM_WEIGHTS; k++)
    {
        switch (k)
        {
            case wxWEIGHT_NORMAL: weight = "Medium"; break;
            case wxWEIGHT_LIGHT:  weight = "Light"; break;
            case wxWEIGHT_BOLD:
            default:              weight = "Bold";
        }
        for (j = 0; j < wxNUM_STYLES; j++)
        {
            switch (j)
            {
                case wxSTYLE_NORMAL: style = "Straight"; break;
                case wxSTYLE_ITALIC: style = "Italic"; break;
                case wxSTYLE_SLANT:
                default:         style = "Slant";
            }
            names[0] = resname;
            names[1] = weight;
            names[2] = style;

            SearchResource(devresname, names, 3, &v);

            // Expand macros in the found string:
found:
            int len, closer = 0, startpos = 0;

            len = (v ? strlen(v) : 0);
            for (i = 0; i < len; i++)
            {
                if (v[i] == '$' && ((v[i+1] == '[') || (v[i+1] == '{')))
                {
                    startpos = i;
                    closer   = (v[i+1] == '[') ? ']' : '}';
                    ++i;
                }
                else if (v[i] == closer)
                {
                    int newstrlen;
                    const char *r = (char *) NULL; bool delete_r = FALSE;
                    char *name;

                    name = v + startpos + 2;
                    v[i] = 0;

                    if (closer == '}')
                    {
                        int i, count, len;
                        char **names;

                        for (i = 0, count = 1; name[i]; i++)
                            if (name[i] == ',')
                                count++;

                        len = i;

                        names = new char*[count];
                        names[0] = name;
                        for (i = 0, count = 1; i < len; i++)
                            if (name[i] == ',')
                            {
                                names[count++] = name + i + 1;
                                name[i] = 0;
                            }

                        SearchResource("", (const char **)names, count, (char **)&r);
                        delete_r = (r != 0);
                        delete[] names;

                        if (!r)
                        {
                            for (i = 0; i < len; i++)
                                if (!name[i])
                                    name[i] = ',';
                            r = "";
                            wxLogError( "Bad resource name in font lookup." );
                        }
                    } else if (!strcmp(name, "weight")) {
                        r = weight;
                    } else if (!strcmp(name, "style")) {
                        r = style;
                    } else if (!strcmp(name, "family")) {
                        r = resname;
                    } else {
                        r = "";
                        wxLogError( "Bad font macro name." );
                    }

                    // add r to v
                    newstrlen = strlen(r);
                    char *naya = new char[startpos + newstrlen + len - i];
                    memcpy(naya, v, startpos);
                    memcpy(naya + startpos, r, newstrlen);
                    memcpy(naya + startpos + newstrlen, v + i + 1, len - i);
                    if (delete_r)
                        delete[] (char*)r;
                    delete[] v;
                    v = naya;

                    goto found;
                }
            }
            // We have a final value:
            map[k][j] = v;
        }
    }
}

//-----------------------------------------------------------------------------
// wxFontNameItem
//-----------------------------------------------------------------------------

class wxFontNameItem : public wxObject
{
    DECLARE_DYNAMIC_CLASS(wxFontNameItem)
public:
        wxFontNameItem(const char *name, int id, int family);
        ~wxFontNameItem();

        inline char* GetScreenName(int w, int s)     {return screen.GetName(w, s);}
        inline char* GetPostScriptName(int w, int s) {return printing.GetName(w, s);}
        inline char* GetAFMName(int w, int s)        {return afm.GetName(w, s);}
        inline char* GetName()                   {return name;}
        inline int   GetFamily()                 {return family;}
        inline int   GetId()                     {return id;}
        inline bool  IsRoman()                   {return isroman;}
#if defined(__WXDEBUG__)
        void Dump(ostream& str);
#endif

        int id;
        int family;
        char *name;
        wxSuffixMap screen, printing, afm;
        bool isroman;
};

IMPLEMENT_ABSTRACT_CLASS(wxFontNameItem, wxObject)

wxFontNameItem::wxFontNameItem(const char *Name, int Id, int Family)
{
    name   = copystring(Name);
    id     = Id;
    family = Family;

    screen.  Initialize(name, "Screen");
    printing.Initialize(name, "PostScript");
    afm.     Initialize(name, "Afm");
}

wxFontNameItem::~wxFontNameItem()
{
    if (name)
        delete[] name;
    name = (char *) NULL;
}

#if defined(__WXDEBUG__)
void wxFontNameItem::Dump(ostream& str)
{
    str << "wxFontNameItem(" << name << ")";
}
#endif

//-----------------------------------------------------------------------------
// wxFontDirectory
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFontNameDirectory, wxObject)

wxFontNameDirectory::wxFontNameDirectory()
{
    table = new wxHashTable(wxKEY_INTEGER, 20);
    nextFontId = -1;
}

wxFontNameDirectory::~wxFontNameDirectory()
{
    // Cleanup wxFontNameItems allocated
    table->BeginFind();
    wxNode *node = table->Next();
    while (node)
    {
        wxFontNameItem *item = (wxFontNameItem*)node->Data();
        delete item;
        node = table->Next();
    }
    delete table;
}

int wxFontNameDirectory::GetNewFontId()
{
    return (nextFontId--);
}

void wxFontNameDirectory::Initialize()
{
    Initialize(wxDEFAULT,    wxDEFAULT,    "Default");
    Initialize(wxDECORATIVE, wxDECORATIVE, "Decorative");
    Initialize(wxROMAN,      wxROMAN,      "Roman");
    Initialize(wxMODERN,     wxMODERN,     "Modern");
    Initialize(wxTELETYPE,   wxTELETYPE,   "Teletype");
    Initialize(wxSWISS,      wxSWISS,      "Swiss");
    Initialize(wxSCRIPT,     wxSCRIPT,     "Script");
}

void wxFontNameDirectory::Initialize(int fontid, int family, const char *resname)
{
    char *fam, resource[256];

    sprintf(resource, "Family%s", resname);
    SearchResource((const char *)resource, (const char **) NULL, 0, (char **)&fam);

    if (fam)
    {
        if      (!strcmp(fam, "Default"))    family = wxDEFAULT;
        else if (!strcmp(fam, "Roman"))        family = wxROMAN;
        else if (!strcmp(fam, "Decorative"))    family = wxDECORATIVE;
        else if (!strcmp(fam, "Modern"))    family = wxMODERN;
        else if (!strcmp(fam, "Teletype"))    family = wxTELETYPE;
        else if (!strcmp(fam, "Swiss"))        family = wxSWISS;
        else if (!strcmp(fam, "Script"))    family = wxSCRIPT;
        delete[] fam; // free resource
    }
    table->Put(fontid, new wxFontNameItem(resname, fontid, family));
}

int wxFontNameDirectory::FindOrCreateFontId(const char *name, int family)
{
    int id;

    // font exists -> return id
    if ( (id = GetFontId(name)) ) return id;

    // create new font
    Initialize(id=GetNewFontId(), family, name);
    return id;
}

char *wxFontNameDirectory::GetScreenName(int fontid, int weight, int style)
{
    wxFontNameItem *item = (wxFontNameItem*)table->Get(fontid); // find font
    if (item)
        return item->GetScreenName(weight, style);

    // font does not exist
    return (char *) NULL;
}

char *wxFontNameDirectory::GetPostScriptName(int fontid, int weight, int style)
{
    wxFontNameItem *item = (wxFontNameItem*)table->Get(fontid); // find font
    if (item)
        return item->GetPostScriptName(weight, style);

    // font does not exist
    return (char *) NULL;
}

char *wxFontNameDirectory::GetAFMName(int fontid, int weight, int style)
{
    wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid); // find font
    if (item)
        return item->GetAFMName(weight, style);
    // font does not exist
    return (char *) NULL;
}

char *wxFontNameDirectory::GetFontName(int fontid)
{
    wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid); // find font
    if (item)
        return item->GetName();

    // font does not exist
    return (char *) NULL;
}

int wxFontNameDirectory::GetFontId(const char *name)
{
    wxNode *node;

    table->BeginFind();

    while ( (node = table->Next()) )
    {
        wxFontNameItem *item = (wxFontNameItem*)node->Data();
        if (!strcmp(name, item->name))
            return item->id;
    }

    // font does not exist
    return 0;
}

int wxFontNameDirectory::GetFamily(int fontid)
{
    wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid);

    if (item)
        return item->family;

    // font does not exist
    return wxDEFAULT;
}

#endif // 0
