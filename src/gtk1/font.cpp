/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "font.h"
#endif

#include "wx/font.h"
#include "wx/utils.h"
#include <strings.h>

//-----------------------------------------------------------------------------
// local data
//-----------------------------------------------------------------------------

static char *wx_font_family [] = {
    "wxDEFAULT", "wxDECORATIVE", "wxMODERN", "wxROMAN", "wxSCRIPT",
    "wxSWISS", "wxTELETYPE",
};

static char *wx_font_style [] = {
    "wxDEFAULT", "wxNORMAL", "wxSLANT", "wxITALIC",
};

static char *wx_font_weight [] = {
    "wxDEFAULT", "wxNORMAL", "wxBOLD", "wxLIGHT",
};

extern wxFontNameDirectory *wxTheFontNameDirectory;

//-----------------------------------------------------------------------------
// wxFont
//-----------------------------------------------------------------------------

class wxFontRefData: public wxObjectRefData
{
  public:
  
    wxFontRefData(void);
    ~wxFontRefData(void);
 
    wxList   m_scaled_xfonts;
    int      m_pointSize;
    int      m_family, m_style, m_weight;
    bool     m_underlined;
    int      m_fontId;
    char*    m_faceName;
     
    bool     m_byXFontName;
    GdkFont *m_font;
    
    friend wxFont;
};

wxFontRefData::wxFontRefData(void) : m_scaled_xfonts(wxKEY_INTEGER)
{
  m_byXFontName = FALSE;
  m_pointSize = -1;
  m_family = -1;
  m_style = -1;
  m_weight = -1;
  m_underlined = FALSE;
  m_fontId = 0;
  m_faceName = NULL;
  m_font = NULL;
}

wxFontRefData::~wxFontRefData(void)
{
  wxNode *node = m_scaled_xfonts.First();
  while (node) 
  {
    GdkFont *font = (GdkFont*)node->Data();
    wxNode *next = node->Next();
    gdk_font_unref( font );
    node = next;
  }
  if (m_faceName) 
  {
    delete m_faceName;
    m_faceName = NULL;
  }
  if (m_font) gdk_font_unref( m_font );
}

//-----------------------------------------------------------------------------

#define M_FONTDATA ((wxFontRefData *)m_refData)

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

wxFont::wxFont(void)
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

wxFont::wxFont(int PointSize, int FontIdOrFamily, int Style, int Weight,
	       bool Underlined, const char* Face)
{
  m_refData = new wxFontRefData();
  
  if ((M_FONTDATA->m_faceName = (Face) ? copystring(Face) : (char*)NULL) ) 
  {
    M_FONTDATA->m_fontId = wxTheFontNameDirectory->FindOrCreateFontId( Face, FontIdOrFamily );
    M_FONTDATA->m_family  = wxTheFontNameDirectory->GetFamily( FontIdOrFamily );
  }
  else 
  {
    M_FONTDATA->m_fontId = FontIdOrFamily;
    M_FONTDATA->m_family  = wxTheFontNameDirectory->GetFamily( FontIdOrFamily );
  }
  M_FONTDATA->m_style = Style;
  M_FONTDATA->m_weight = Weight;
  M_FONTDATA->m_pointSize = PointSize;
  M_FONTDATA->m_underlined = Underlined;

  if (wxTheFontList) wxTheFontList->Append( this );
}

wxFont::wxFont(int PointSize, const char *Face, int Family, int Style, 
	       int Weight, bool Underlined)
{
  m_refData = new wxFontRefData();

  M_FONTDATA->m_fontId = wxTheFontNameDirectory->FindOrCreateFontId( Face, Family );
  M_FONTDATA->m_faceName = (Face) ? copystring(Face) : (char*)NULL;
  M_FONTDATA->m_family = wxTheFontNameDirectory->GetFamily( M_FONTDATA->m_fontId );
  M_FONTDATA->m_style = Style;
  M_FONTDATA->m_weight = Weight;
  M_FONTDATA->m_pointSize = PointSize;
  M_FONTDATA->m_underlined = Underlined;

  if (wxTheFontList) wxTheFontList->Append( this );
}

wxFont::wxFont( const wxFont& font )
{ 
  Ref( font ); 
  
  if (wxTheFontList) wxTheFontList->Append( this );
}

wxFont::wxFont( const wxFont* font ) 
{ 
  UnRef(); 
  if (font) Ref( *font ); 
  
  if (wxTheFontList) wxTheFontList->Append( this );
}

wxFont::~wxFont(void)
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

bool wxFont::Ok()
{
  return (m_refData != NULL);
}

int wxFont::GetPointSize(void) const
{
  return M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceString(void) const
{
  wxString s = wxTheFontNameDirectory->GetFontName( M_FONTDATA->m_fontId );
  return s;
}

wxString wxFont::GetFaceName(void) const
{
  wxString s = wxTheFontNameDirectory->GetFontName( M_FONTDATA->m_fontId );
  return s; 
}

int wxFont::GetFamily(void) const
{
  return M_FONTDATA->m_family;
}

wxString wxFont::GetFamilyString(void) const
{
  wxString s = wx_font_family[M_FONTDATA->m_family];
  return s;
}

int wxFont::GetFontId(void) const
{
  return M_FONTDATA->m_fontId; // stub
}

int wxFont::GetStyle(void) const
{
  return M_FONTDATA->m_style;
}

wxString wxFont::GetStyleString(void) const
{
  wxString s =  wx_font_style[M_FONTDATA->m_style];
  return s;
}

int wxFont::GetWeight(void) const
{
  return M_FONTDATA->m_weight;
}

wxString wxFont::GetWeightString(void) const
{
  wxString s = wx_font_weight[M_FONTDATA->m_weight];
  return s;
}

bool wxFont::GetUnderlined(void) const
{
  return M_FONTDATA->m_underlined;
}

//-----------------------------------------------------------------------------
// get internal representation of font
//-----------------------------------------------------------------------------

// local help function
static GdkFont *wxLoadQueryNearestFont(int point_size, int fontid,
					   int style, int weight, 
					   bool underlined);

GdkFont *wxFont::GetInternalFont(float scale) const
{
  if (M_FONTDATA->m_byXFontName) return M_FONTDATA->m_font;
   
  long int_scale = long(scale * 100.0 + 0.5); // key for fontlist
  int point_scale = (M_FONTDATA->m_pointSize * 10 * int_scale) / 100;
  GdkFont *font = NULL;

  wxNode *node = M_FONTDATA->m_scaled_xfonts.Find(int_scale);
  if (node) 
  {
    font = (GdkFont*)node->Data(); 
  } 
  else 
  {
     font = wxLoadQueryNearestFont( point_scale, M_FONTDATA->m_fontId, M_FONTDATA->m_style,
				    M_FONTDATA->m_weight, M_FONTDATA->m_underlined );
     M_FONTDATA->m_scaled_xfonts.Append( int_scale, (wxObject*)font );
  }
  if (!font)
	printf("could not load any font");
//	wxError("could not load any font", "wxFont");
  return font;
}

//-----------------------------------------------------------------------------
// local utilities to find a X font
//-----------------------------------------------------------------------------

static GdkFont *wxLoadQueryFont(int point_size, int fontid, int style,
				    int weight, bool WXUNUSED(underlined))
{
    char buffer[512];
    char *name = wxTheFontNameDirectory->GetScreenName( fontid, weight, style );

    if (!name)
	name = "-*-*-*-*-*-*-*-%d-*-*-*-*-*-*";
    sprintf(buffer, name, point_size);

    return gdk_font_load( buffer );
}

static GdkFont *wxLoadQueryNearestFont(int point_size, int fontid,
					   int style, int weight,
					   bool underlined)
{
    GdkFont *font;

    font = wxLoadQueryFont( point_size, fontid, style, weight, underlined );

    if (!font) {
	// search up and down by stepsize 10
	int max_size = point_size + 20 * (1 + (point_size/180));
	int min_size = point_size - 20 * (1 + (point_size/180));
	int i;

	// Search for smaller size (approx.)
	for (i=point_size-10; !font && i >= 10 && i >= min_size; i -= 10)
	    font = wxLoadQueryFont(i, fontid, style, weight, underlined);
	// Search for larger size (approx.)
	for (i=point_size+10; !font && i <= max_size; i += 10)
	    font = wxLoadQueryFont(i, fontid, style, weight, underlined);
	// Try default family
	if (!font && fontid != wxDEFAULT)
	    font = wxLoadQueryFont(point_size, wxDEFAULT, style, 
				   weight, underlined);
	// Bogus font
	if (!font)
	    font = wxLoadQueryFont(120, wxDEFAULT, wxNORMAL, wxNORMAL,
				    underlined);
    }
    return font;
}

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

    "ScreenDefaultBase", "misc-fixed",
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
    NULL
};

enum {wxWEIGHT_NORMAL, wxWEIGHT_BOLD,  wxWEIGHT_LIGHT, wxNUM_WEIGHTS};
enum {wxSTYLE_NORMAL,  wxSTYLE_ITALIC, wxSTYLE_SLANT,  wxNUM_STYLES};

static int WCoordinate(int w)
{
    switch (w) {
    case wxBOLD:   return wxWEIGHT_BOLD;
    case wxLIGHT:  return wxWEIGHT_LIGHT;
    case wxNORMAL:
    default:       return wxWEIGHT_NORMAL;
    }
};

static int SCoordinate(int s)
{
    switch (s) {
    case wxITALIC: return wxSTYLE_ITALIC;
    case wxSLANT:  return wxSTYLE_SLANT;
    case wxNORMAL:
    default:       return wxSTYLE_NORMAL;
    }
};

//-----------------------------------------------------------------------------
// wxSuffixMap
//-----------------------------------------------------------------------------

class wxSuffixMap {
public:
    ~wxSuffixMap(void);

    inline char *GetName(int weight, int style)
    {
	return ( map [WCoordinate(weight)] [SCoordinate(style)] );
    }

    char *map[wxNUM_WEIGHTS][wxNUM_STYLES];
    void Initialize(const char *, const char *);
};

//#if !USE_RESOURCES
#define wxGetResource(a, b, c) 0
//#endif

static void SearchResource(const char *prefix, const char **names, int count, char **v)
{
    int k, i, j;
    char resource[1024], **defaults, *internal;

    k = 1 << count;
    
    *v = NULL;
    internal = NULL;

    for (i = 0; i < k; i++) {
	strcpy(resource, prefix);
	for (j = 0; j < count; j++) {
	    if (!(i & (1 << j)))
		strcat(resource, names[j]);
	    else
		strcat(resource, "_");
	}
	if (wxGetResource(wxAPP_CLASS, (char *)resource, v))
	    return;
	if (!internal) {
	    defaults = font_defaults;
	    while (*defaults) {
		if (!strcmp(*defaults, resource)) {
		    internal = defaults[1];
		    break;
		}
		defaults += 2;
	    }
	}
    }
    if (internal)
	*v = copystring(internal);
}

wxSuffixMap::~wxSuffixMap(void)
{
    int k, j;

    for (k = 0; k < wxNUM_WEIGHTS; ++k)
	for (j = 0; j < wxNUM_STYLES; ++j)
	    if (map[k][j]) {
		delete[] map[k][j];
		map[k][j] = NULL;
	    }
}

void wxSuffixMap::Initialize(const char *resname, const char *devresname)
{
    const char *weight, *style;
    char *v;
    int i, j, k;
    const char *names[3];

    for (k = 0; k < wxNUM_WEIGHTS; k++) {
	switch (k) {
	case wxWEIGHT_NORMAL: weight = "Medium"; break;
	case wxWEIGHT_LIGHT:  weight = "Light"; break;
	case wxWEIGHT_BOLD:
	default:	      weight = "Bold";
	}
	for (j = 0; j < wxNUM_STYLES; j++) {
	    switch (j) {
	    case wxSTYLE_NORMAL: style = "Straight"; break;
	    case wxSTYLE_ITALIC: style = "Italic"; break;
	    case wxSTYLE_SLANT:
	    default:		 style = "Slant";
	    }
	    names[0] = resname;
	    names[1] = weight;
	    names[2] = style;

	    SearchResource(devresname, names, 3, &v);

	    /* Expand macros in the found string: */
	found:
	    int len, closer = 0, startpos = 0;
	    
	    len = (v ? strlen(v) : 0);
	    for (i = 0; i < len; i++) {
		if (v[i] == '$' && ((v[i+1] == '[') || (v[i+1] == '{'))) {
		    startpos = i;
		    closer   = (v[i+1] == '[') ? ']' : '}';
		    ++i;
		} else if (v[i] == closer) {
		    int newstrlen;
		    const char *r = NULL; bool delete_r = FALSE;
		    char *name;
	  
		    name = v + startpos + 2;
		    v[i] = 0;

		    if (closer == '}') {
			int i, count, len;
			char **names;

			for (i = 0, count = 1; name[i]; i++)
			    if (name[i] == ',')
				count++;
	    
			len = i;

			names = new char*[count];
			names[0] = name;
			for (i = 0, count = 1; i < len; i++)
			    if (name[i] == ',') {
				names[count++] = name + i + 1;
				name[i] = 0;
			    }

			SearchResource("", (const char **)names, count, (char **)&r);
			delete_r = (r != 0);
			delete[] names;
			
			if (!r) {
			    for (i = 0; i < len; i++)
				if (!name[i])
				    name[i] = ',';
			    r = "";
			    printf("Bad resource name \"%s\" in font lookup\n", name);
			}
		    } else if (!strcmp(name, "weight")) {
			r = weight;
		    } else if (!strcmp(name, "style")) {
			r = style;
		    } else if (!strcmp(name, "family")) {
			r = resname;
		    } else {
			r = "";
			printf("Bad font macro name \"%s\"\n", name);
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
	    /* We have a final value: */
	    map[k][j] = v;
	}
    }
}

//-----------------------------------------------------------------------------
// wxFontNameItem
//-----------------------------------------------------------------------------

class wxFontNameItem : public wxObject {
DECLARE_DYNAMIC_CLASS(wxFontNameItem)
public:
    wxFontNameItem(const char *name, int id, int family);
    ~wxFontNameItem();

    inline char* GetScreenName(int w, int s)     {return screen.GetName(w, s);}
    inline char* GetPostScriptName(int w, int s) {return printing.GetName(w, s);}
    inline char* GetAFMName(int w, int s)        {return afm.GetName(w, s);}
    inline char* GetName(void)                   {return name;}
    inline int   GetFamily(void)                 {return family;}
    inline int   GetId(void)                     {return id;}
    inline bool  IsRoman(void)                   {return isroman;}
#if WXDEBUG
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

wxFontNameItem::~wxFontNameItem(void)
{
    if (name)
	delete[] name;
    name = NULL;
}

#if WXDEBUG
void wxFontNameItem::Dump(ostream& str)
{
    str << "wxFontNameItem(" << name << ")";
}
#endif

//-----------------------------------------------------------------------------
// wxFontDirectory
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFontNameDirectory, wxObject)

wxFontNameDirectory::wxFontNameDirectory(void)
{
    table = new wxHashTable(wxKEY_INTEGER, 20);
    nextFontId = -1;
}

wxFontNameDirectory::~wxFontNameDirectory()
{
    // Cleanup wxFontNameItems allocated
    table->BeginFind();
    wxNode *node = table->Next();
    while (node) {
	wxFontNameItem *item = (wxFontNameItem*)node->Data();
	delete item;
	node = table->Next();
    }
    delete table;
}

int wxFontNameDirectory::GetNewFontId(void)
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
    SearchResource((const char *)resource, NULL, 0, (char **)&fam);
    if (fam) {
	if      (!strcmp(fam, "Default"))	family = wxDEFAULT;
	else if (!strcmp(fam, "Roman"))		family = wxROMAN;
	else if (!strcmp(fam, "Decorative"))	family = wxDECORATIVE;
	else if (!strcmp(fam, "Modern"))	family = wxMODERN;
	else if (!strcmp(fam, "Teletype"))	family = wxTELETYPE;
	else if (!strcmp(fam, "Swiss"))		family = wxSWISS;
	else if (!strcmp(fam, "Script"))	family = wxSCRIPT;
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
    return NULL;
}

char *wxFontNameDirectory::GetPostScriptName(int fontid, int weight, int style)
{
    wxFontNameItem *item = (wxFontNameItem*)table->Get(fontid); // find font
    if (item)
	return item->GetPostScriptName(weight, style);
    // font does not exist
    return NULL;
}

char *wxFontNameDirectory::GetAFMName(int fontid, int weight, int style)
{
    wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid); // find font
    if (item)
	return item->GetAFMName(weight, style);
    // font does not exist
    return NULL;
}

char *wxFontNameDirectory::GetFontName(int fontid)
{
    wxFontNameItem *item = (wxFontNameItem *)table->Get(fontid); // find font
    if (item)
	return item->GetName();
    // font does not exist
    return NULL;
}

int wxFontNameDirectory::GetFontId(const char *name)
{
    wxNode *node;

    table->BeginFind();

    while ( (node = table->Next()) ) {
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
