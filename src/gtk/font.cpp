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
#include "wx/fontutil.h"
#include "wx/cmndata.h"
#include "wx/utils.h"
#include "wx/log.h"
#include "wx/gdicmn.h"
#include "wx/tokenzr.h"
#include "wx/settings.h"

#include <strings.h>

#include <gdk/gdk.h>
#include <gdk/gdkprivate.h>
#include <gtk/gtk.h>

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
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
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
    wxList          m_scaled_xfonts;
    int             m_pointSize;
    int             m_family,
                    m_style,
                    m_weight;
    bool            m_underlined;
    wxString        m_faceName;
    wxFontEncoding  m_encoding;

    wxNativeFontInfo m_nativeFontInfo;

    friend class wxFont;
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
}

wxFontRefData::wxFontRefData( const wxFontRefData& data )
    : m_scaled_xfonts(wxKEY_INTEGER)
{
    Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
         data.m_underlined, data.m_faceName, data.m_encoding);
}

wxFontRefData::wxFontRefData(int size, int family, int style,
                             int weight, bool underlined,
                             const wxString& faceName,
                             wxFontEncoding encoding)
    : m_scaled_xfonts(wxKEY_INTEGER)
{
    Init(size, family, style, weight, underlined, faceName, encoding);
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
// wxNativeFontInfo
// ----------------------------------------------------------------------------

bool wxNativeFontInfo::FromString(const wxString& s)
{
    wxStringTokenizer tokenizer(s, _T(";"));

    wxString token = tokenizer.GetNextToken();
    //
    //  Ignore the version for now
    //

    xFontName = tokenizer.GetNextToken();
    if(!xFontName)
        return FALSE;
        
    return TRUE;
}

wxString wxNativeFontInfo::ToString() const
{
    wxString s;
    
    s.Printf(_T("%d;%s"),
             0,                         // version
             xFontName.c_str());
             
    return s;
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

wxFont::wxFont(const wxNativeFontInfo& info)
{
    Init();

    Create(info.xFontName);
}

bool wxFont::Create(const wxNativeFontInfo& info)
{
    return Create(info.xFontName);
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

bool wxFont::Create(const wxString& fontname, wxFontEncoding enc)
{
    if( !fontname )
    {
         *this = wxSystemSettings::GetSystemFont( wxSYS_DEFAULT_GUI_FONT);
         return TRUE;
    }

    m_refData = new wxFontRefData();

    M_FONTDATA->m_nativeFontInfo.xFontName = fontname;  // X font name

    wxString tmp;

    wxStringTokenizer tn( fontname, wxT("-") );

    tn.GetNextToken();                           // skip initial empty token
    tn.GetNextToken();                           // foundry


    M_FONTDATA->m_faceName = tn.GetNextToken();  // family

    tmp = tn.GetNextToken().MakeUpper();         // weight
    if (tmp == wxT("BOLD")) M_FONTDATA->m_weight = wxBOLD;
    if (tmp == wxT("BLACK")) M_FONTDATA->m_weight = wxBOLD;
    if (tmp == wxT("EXTRABOLD")) M_FONTDATA->m_weight = wxBOLD;
    if (tmp == wxT("DEMIBOLD")) M_FONTDATA->m_weight = wxBOLD;
    if (tmp == wxT("ULTRABOLD")) M_FONTDATA->m_weight = wxBOLD;

    if (tmp == wxT("LIGHT")) M_FONTDATA->m_weight = wxLIGHT;
    if (tmp == wxT("THIN")) M_FONTDATA->m_weight = wxLIGHT;

    tmp = tn.GetNextToken().MakeUpper();        // slant
    if (tmp == wxT("I")) M_FONTDATA->m_style = wxITALIC;
    if (tmp == wxT("O")) M_FONTDATA->m_style = wxITALIC;

    tn.GetNextToken();                           // set width
    tn.GetNextToken();                           // add. style
    tn.GetNextToken();                           // pixel size

    tmp = tn.GetNextToken();                     // pointsize
    if (tmp != wxT("*"))
    {
        long num = wxStrtol (tmp.c_str(), (wxChar **) NULL, 10);
        M_FONTDATA->m_pointSize = (int)(num / 10);
    }

    tn.GetNextToken();                           // x-res
    tn.GetNextToken();                           // y-res

    tmp = tn.GetNextToken().MakeUpper();         // spacing

    if (tmp == wxT("M"))
        M_FONTDATA->m_family = wxMODERN;
    else if (M_FONTDATA->m_faceName == wxT("TIMES"))
        M_FONTDATA->m_family = wxROMAN;
    else if (M_FONTDATA->m_faceName == wxT("HELVETICA"))
        M_FONTDATA->m_family = wxSWISS;
    else if (M_FONTDATA->m_faceName == wxT("LUCIDATYPEWRITER"))
        M_FONTDATA->m_family = wxTELETYPE;
    else if (M_FONTDATA->m_faceName == wxT("LUCIDA"))
        M_FONTDATA->m_family = wxDECORATIVE;
    else if (M_FONTDATA->m_faceName == wxT("UTOPIA"))
        M_FONTDATA->m_family = wxSCRIPT;

    tn.GetNextToken();                           // avg width

    // deal with font encoding
    M_FONTDATA->m_encoding = enc;
    if ( M_FONTDATA->m_encoding == wxFONTENCODING_SYSTEM )
    {
        wxString registry = tn.GetNextToken().MakeUpper(),
                 encoding = tn.GetNextToken().MakeUpper();

        if ( registry == _T("ISO8859") )
        {
            int cp;
            if ( wxSscanf(encoding, wxT("%d"), &cp) == 1 )
            {
                M_FONTDATA->m_encoding =
                    (wxFontEncoding)(wxFONTENCODING_ISO8859_1 + cp - 1);
            }
        }
        else if ( registry == _T("MICROSOFT") )
        {
            int cp;
            if ( wxSscanf(encoding, wxT("cp125%d"), &cp) == 1 )
            {
                M_FONTDATA->m_encoding =
                    (wxFontEncoding)(wxFONTENCODING_CP1250 + cp);
            }
        }
        else if ( registry == _T("KOI8") )
        {
            M_FONTDATA->m_encoding = wxFONTENCODING_KOI8;
        }
        //else: unknown encoding - may be give a warning here?
        else
            return FALSE;
    }
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

    if(M_FONTDATA->m_nativeFontInfo.xFontName.IsEmpty())
        GetInternalFont();

    return new wxNativeFontInfo(M_FONTDATA->m_nativeFontInfo);
}


// ----------------------------------------------------------------------------
// change font attributes
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    Unshare();

    M_FONTDATA->m_pointSize = pointSize;
    M_FONTDATA->m_nativeFontInfo.xFontName.Clear();            // invalid now
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->m_family = family;
    M_FONTDATA->m_nativeFontInfo.xFontName.Clear();            // invalid now
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->m_style = style;
    M_FONTDATA->m_nativeFontInfo.xFontName.Clear();            // invalid now
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->m_weight = weight;
    M_FONTDATA->m_nativeFontInfo.xFontName.Clear();            // invalid now
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_faceName = faceName;
    M_FONTDATA->m_nativeFontInfo.xFontName.Clear();            // invalid now
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
    M_FONTDATA->m_nativeFontInfo.xFontName.Clear();            // invalid now
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

GdkFont *GtkGetDefaultGuiFont()
{
    if (!g_systemDefaultGuiFont)
    {
        GtkWidget *widget = gtk_button_new();
        GtkStyle *def = gtk_rc_get_style( widget );
        if (def)
        {
            g_systemDefaultGuiFont = gdk_font_ref( def->font );
        }
        else
        {
            def = gtk_widget_get_default_style();
            if (def)
                g_systemDefaultGuiFont = gdk_font_ref( def->font );
        }
        gtk_widget_destroy( widget );
    }
    return g_systemDefaultGuiFont;
}

GdkFont *wxFont::GetInternalFont( float scale ) const
{
    if (!Ok())
    {
        wxFAIL_MSG( wxT("invalid font") );

        return (GdkFont*) NULL;
    }

    long int_scale = long(scale * 100.0 + 0.5); /* key for fontlist */
    int point_scale = (int)((M_FONTDATA->m_pointSize * 10 * int_scale) / 100);
    GdkFont *font = (GdkFont *) NULL;

    wxNode *node = M_FONTDATA->m_scaled_xfonts.Find(int_scale);
    if (node)
    {
        font = (GdkFont*)node->Data();
    }
    else
    {
        if (*this == wxSystemSettings::GetSystemFont( wxSYS_DEFAULT_GUI_FONT))
        {
            font = GtkGetDefaultGuiFont();
        }
        if (!font)
        {
            font = wxLoadQueryNearestFont( point_scale,
                                           M_FONTDATA->m_family,
                                           M_FONTDATA->m_style,
                                           M_FONTDATA->m_weight,
                                           M_FONTDATA->m_underlined,
                                           M_FONTDATA->m_faceName,
                                           M_FONTDATA->m_encoding,
                                           &M_FONTDATA->m_nativeFontInfo.xFontName );
        }

        M_FONTDATA->m_scaled_xfonts.Append( int_scale, (wxObject*)font );
    }

    // it's quite useless to make it a wxCHECK because we're going to crash
    // anyhow...
    wxASSERT_MSG( font, wxT("could not load any font?") );

    return font;
}

