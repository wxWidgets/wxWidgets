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
}

wxFontRefData::wxFontRefData( const wxFontRefData& data )
    : m_scaled_xfonts(wxKEY_INTEGER)
{
    Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
        data.m_underlined, data.m_faceName, data.m_encoding);
}

wxFontRefData::wxFontRefData(int size, int family, int style,
    int weight, bool underlined, const wxString& faceName, wxFontEncoding encoding )
    : m_scaled_xfonts(wxKEY_INTEGER)
{
    Init(size, family, style, weight, 
        underlined, faceName, encoding);
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
// wxFont
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

void wxFont::Init()
{
    if (wxTheFontList)
        wxTheFontList->Append( this );
}

wxFont::wxFont( GdkFont *WXUNUSED(font), char *xFontName )
{
    if (!xFontName)
        return;

    Init();

    m_refData = new wxFontRefData();

    wxString tmp;

    wxString fontname( xFontName );
    wxStringTokenizer tn( fontname, T("-") );

    tn.GetNextToken();                           // foundry

    M_FONTDATA->m_faceName = tn.GetNextToken();  // courier

    tmp = tn.GetNextToken().MakeUpper();
    if (tmp == T("BOLD")) M_FONTDATA->m_weight = wxBOLD;

    tmp = tn.GetNextToken().MakeUpper();
    if (tmp == T("I")) M_FONTDATA->m_style = wxITALIC;
    if (tmp == T("O")) M_FONTDATA->m_style = wxITALIC;

    tn.GetNextToken();                           // set width
    tn.GetNextToken();                           // ?
    tn.GetNextToken();                           // pixel size

    tmp = tn.GetNextToken();                     // pointsize
    int num =  wxStrtol (tmp.c_str(), (wxChar **) NULL, 10);
    M_FONTDATA->m_pointSize = num / 10;

    tn.GetNextToken();                           // x-res
    tn.GetNextToken();                           // y-res

    tmp = tn.GetNextToken().MakeUpper();
    if (tmp == T("M")) M_FONTDATA->m_family = wxMODERN;
    else if (M_FONTDATA->m_faceName == T("TIMES")) M_FONTDATA->m_family = wxROMAN;
    else if (M_FONTDATA->m_faceName == T("HELVETICA")) M_FONTDATA->m_family = wxSWISS;
    else if (M_FONTDATA->m_faceName == T("LUCIDATYPEWRITER")) M_FONTDATA->m_family = wxTELETYPE;
    else if (M_FONTDATA->m_faceName == T("LUCIDA")) M_FONTDATA->m_family = wxDECORATIVE;
    else if (M_FONTDATA->m_faceName == T("UTOPIA")) M_FONTDATA->m_family = wxSCRIPT;
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
    wxCHECK_MSG( Ok(), 0, T("invalid font") );

    return M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), T(""), T("invalid font") );

    return M_FONTDATA->m_faceName;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, T("invalid font") );

    return M_FONTDATA->m_family;
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, T("invalid font") );

    return M_FONTDATA->m_style;
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, T("invalid font") );

    return M_FONTDATA->m_weight;
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( Ok(), FALSE, T("invalid font") );

    return M_FONTDATA->m_underlined;
}


wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, T("invalid font") );

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
        wxFAIL_MSG( T("invalid font") );

        return (GdkFont*) NULL;
    }

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
        wxLogError(T("could not load any font"));
    }

    return font;
}

