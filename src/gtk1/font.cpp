/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:       wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "font.h"
#endif

#include "wx/font.h"
#include "wx/utils.h"
#include "wx/log.h"
#include <strings.h>

#include "gdk/gdk.h"

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

/*
extern wxFontNameDirectory *wxTheFontNameDirectory;
*/

//-----------------------------------------------------------------------------
// wxFont
//-----------------------------------------------------------------------------

class wxFontRefData: public wxObjectRefData
{
public:

    wxFontRefData();
    wxFontRefData( const wxFontRefData& data );
    ~wxFontRefData();

    wxList    m_scaled_xfonts;
    int       m_pointSize;
    int       m_family, m_style, m_weight;
    bool      m_underlined;
    wxString  m_faceName;

    bool      m_byXFontName;
    GdkFont  *m_font;

    friend wxFont;
};

wxFontRefData::wxFontRefData() : m_scaled_xfonts(wxKEY_INTEGER)
{
    m_byXFontName = FALSE;
    m_pointSize = 12;
    m_family = wxSWISS;
    m_style = wxNORMAL;
    m_weight = wxNORMAL;
    m_underlined = FALSE;
    m_font = (GdkFont *) NULL;
}

wxFontRefData::wxFontRefData( const wxFontRefData& data ) : m_scaled_xfonts(wxKEY_INTEGER)
{
    m_byXFontName = FALSE;
    m_pointSize = data.m_pointSize;
    m_family = data.m_family;
    m_style = data.m_style;
    m_weight = data.m_weight;
    m_underlined = data.m_underlined;
    m_faceName = data.m_faceName;
    m_font = (GdkFont *) NULL;
    if (data.m_font) m_font = gdk_font_ref( data.m_font );
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
    if (m_font) gdk_font_unref( m_font );
}

//-----------------------------------------------------------------------------

#define M_FONTDATA ((wxFontRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

wxFont::wxFont()
{
    if (wxTheFontList) wxTheFontList->Append( this );
}

wxFont::wxFont( char *xFontName )
{
    if (!xFontName) return;

    m_refData = new wxFontRefData();

    M_FONTDATA->m_byXFontName = TRUE;
    M_FONTDATA->m_font = gdk_font_load( xFontName );
}

wxFont::wxFont( int pointSize, int family, int style, int weight, bool underlined, const wxString& face )
{
    m_refData = new wxFontRefData();

    if (family == wxDEFAULT) 
        M_FONTDATA->m_family = wxSWISS;
    else
        M_FONTDATA->m_family = family;
    
    if (!face.IsEmpty()) M_FONTDATA->m_faceName = face;

    if (style == wxDEFAULT) 
        M_FONTDATA->m_style = wxNORMAL;
    else
        M_FONTDATA->m_style = style;
    
    if (weight == wxDEFAULT) 
        M_FONTDATA->m_weight = wxNORMAL;
    else
        M_FONTDATA->m_weight = weight;
    
    if (pointSize == wxDEFAULT) 
        M_FONTDATA->m_pointSize = 12;
    else
        M_FONTDATA->m_pointSize = pointSize;
    
    M_FONTDATA->m_underlined = underlined;

    if (wxTheFontList) wxTheFontList->Append( this );
  
}

wxFont::wxFont( const wxFont& font )
{
    Ref( font );

    if (wxTheFontList) wxTheFontList->Append( this );
}

wxFont::~wxFont()
{
    if (wxTheFontList) wxTheFontList->DeleteObject( this );
}

wxFont& wxFont::operator = ( const wxFont& font )
{
    if (*this == font) return (*this);
    Ref( font );
    return *this;
}

bool wxFont::operator == ( const wxFont& font )
{
    return m_refData == font.m_refData;
}

bool wxFont::operator != ( const wxFont& font )
{
    return m_refData != font.m_refData;
}

bool wxFont::Ok() const
{
    return (m_refData != NULL);
}

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, "invalid font" );

    return M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), "", "invalid font" );

    return M_FONTDATA->m_faceName;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, "invalid font" );

    return M_FONTDATA->m_family;
}

wxString wxFont::GetFamilyString() const
{
    wxCHECK_MSG( Ok(), "wxDEFAULT", "invalid font" );

    switch (M_FONTDATA->m_family)
    {
        case wxDECORATIVE:   return wxString("wxDECORATIVE");
        case wxROMAN:        return wxString("wxROMAN");
        case wxSCRIPT:       return wxString("wxSCRIPT");
        case wxSWISS:        return wxString("wxSWISS");
        case wxMODERN:       return wxString("wxMODERN");
        case wxTELETYPE:     return wxString("wxTELETYPE");
        default:             return "wxDEFAULT";
    }

    return "wxDEFAULT";
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, "invalid font" );

    return M_FONTDATA->m_style;
}

wxString wxFont::GetStyleString() const
{
    wxCHECK_MSG( Ok(), "wxDEFAULT", "invalid font" );

    switch (M_FONTDATA->m_style)
    {
        case wxNORMAL:   return wxString("wxNORMAL");
        case wxSLANT:    return wxString("wxSLANT");
        case wxITALIC:   return wxString("wxITALIC");
        default:         return wxString("wxDEFAULT");
    }

    return wxString("wxDEFAULT");
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, "invalid font" );

    return M_FONTDATA->m_weight;
}

wxString wxFont::GetWeightString() const
{
    wxCHECK_MSG( Ok(), "wxDEFAULT", "invalid font" );

    switch (M_FONTDATA->m_weight)
    {
        case wxNORMAL:   return wxString("wxNORMAL");
        case wxBOLD:     return wxString("wxBOLD");
        case wxLIGHT:    return wxString("wxLIGHT");
        default:         return wxString("wxDEFAULT");
    }

    return wxString("wxDEFAULT");
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( Ok(), FALSE, "invalid font" );

    return M_FONTDATA->m_underlined;
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

//-----------------------------------------------------------------------------
// get internal representation of font
//-----------------------------------------------------------------------------

static GdkFont *wxLoadQueryNearestFont( int point_size, int family, int style, int weight, 
                                        bool underlined, const wxString &facename );

GdkFont *wxFont::GetInternalFont( float scale ) const
{
    if (!Ok())
    {
        wxFAIL_MSG( "invalid font" );
        return (GdkFont*) NULL;
    }

    /* short cut if the special X font constructor has been used */
    if (M_FONTDATA->m_byXFontName) return M_FONTDATA->m_font;

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
/*
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
*/
        {
            font = wxLoadQueryNearestFont( point_scale, M_FONTDATA->m_family, M_FONTDATA->m_style,
                    M_FONTDATA->m_weight, M_FONTDATA->m_underlined, M_FONTDATA->m_faceName );
        }
        M_FONTDATA->m_scaled_xfonts.Append( int_scale, (wxObject*)font );
    }
    
    if (!font)
    {
        wxLogError("could not load any font");
    }
	
    return font;
}

//-----------------------------------------------------------------------------
// local utilities to find a X font
//-----------------------------------------------------------------------------

static GdkFont*wxLoadQueryFont( int pointSize, int family, int style, int weight, 
                                bool underlined, const wxString &facename )
{
    char *xfamily = (char*) NULL;
    char *xstyle = (char*) NULL;
    char *xweight = (char*) NULL;
    
    switch (family)
    {
        case wxDECORATIVE: xfamily = "lucida"; break;
        case wxROMAN:      xfamily = "times";  break;
        case wxMODERN:     xfamily = "courier"; break;
        case wxSWISS:      xfamily = "helvetica"; break;
	case wxTELETYPE:   xfamily = "lucidatypewriter"; break;
	case wxSCRIPT:     xfamily = "utopia"; break;
        default:           xfamily = "*";
    }
    
    if (!facename.IsEmpty())
    {
        sprintf( wxBuffer, "-*-%s-*-*-normal-*-*-*-*-*-*-*-*-*", facename.c_str() );
        GdkFont *test = gdk_font_load( wxBuffer );
        if (test)
	{
	    gdk_font_unref( test );
	    xfamily = WXSTRINGCAST facename;
	}
    }
    
    switch (style)
    {
        case wxITALIC:     xstyle = "i"; break;
        case wxSLANT:      xstyle = "o"; break;
        case wxNORMAL:     xstyle = "r"; break;
        default:           xstyle = "*"; break;
    }
    switch (weight)
    {
        case wxBOLD:       xweight = "bold"; break;
        case wxLIGHT:
        case wxNORMAL:     xweight = "medium"; break;
        default:           xweight = "*"; break;
    }
    
    sprintf( wxBuffer, "-*-%s-%s-%s-normal-*-*-%d-*-*-*-*-*-*",
        xfamily, xweight, xstyle, pointSize);
    
    return gdk_font_load( wxBuffer );
}

static GdkFont *wxLoadQueryNearestFont( int point_size, int family, int style, int weight, 
	                                bool underlined, const wxString &facename )
{
    GdkFont *font = wxLoadQueryFont( point_size, family, style, weight, underlined, facename );

    if (!font) 
    {
        /* search up and down by stepsize 10 */
        int max_size = point_size + 20 * (1 + (point_size/180));
        int min_size = point_size - 20 * (1 + (point_size/180));

        int i;

        /* Search for smaller size (approx.) */
        for (i=point_size-10; !font && i >= 10 && i >= min_size; i -= 10)
            font = wxLoadQueryFont(i, family, style, weight, underlined, facename );

        /* Search for larger size (approx.) */
        for (i=point_size+10; !font && i <= max_size; i += 10)
            font = wxLoadQueryFont( i, family, style, weight, underlined, facename );

        /* Try default family */
        if (!font && family != wxDEFAULT)
            font = wxLoadQueryFont( point_size, wxDEFAULT, style, weight, underlined, facename );

        /* Bogus font */
        if (!font)
            font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL, underlined, facename );
    }
    
    return font;
}

/*

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

*/
