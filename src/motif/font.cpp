/////////////////////////////////////////////////////////////////////////////
// Name:        src/motif/font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "font.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/defs.h"

#ifdef __VMS
#pragma message disable nosimpint
#include "wx/vms_x_fix.h"
#endif
#include <Xm/Xm.h>
#ifdef __VMS
#pragma message enable nosimpint
#endif

#include "wx/string.h"
#include "wx/font.h"
#include "wx/gdicmn.h"
#include "wx/utils.h"       // for wxGetDisplay()
#include "wx/fontutil.h"    // for wxNativeFontInfo
#include "wx/tokenzr.h"
#include "wx/settings.h"
#include "wx/motif/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// For every wxFont, there must be a font for each display and scale requested.
// So these objects are stored in wxFontRefData::m_fonts
class wxXFont : public wxObject
{
public:
    wxXFont();
    ~wxXFont();

    WXFontStructPtr     m_fontStruct;   // XFontStruct
    WXFontList          m_fontList;     // Motif XmFontList
#if wxCHECK_MOTIF_VERSION( 2, 0 )
    WXRenderTable       m_renderTable;  // Motif XmRenderTable
#endif
    WXDisplay*          m_display;      // XDisplay
    int                 m_scale;        // Scale * 100
};

class wxFontRefData: public wxGDIRefData
{
friend class wxFont;

public:
    wxFontRefData(int size = wxDEFAULT,
                  int family = wxDEFAULT,
                  int style = wxDEFAULT,
                  int weight = wxDEFAULT,
                  bool underlined = false,
                  const wxString& faceName = wxEmptyString,
                  wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    wxFontRefData(const wxFontRefData& data)
    {
        Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
             data.m_underlined, data.m_faceName, data.m_encoding);
    }

    ~wxFontRefData();

protected:
    // common part of all ctors
    void Init(int size,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    // font attributes
    int           m_pointSize;
    int           m_family;
    int           m_style;
    int           m_weight;
    bool          m_underlined;
    wxString      m_faceName;
    wxFontEncoding m_encoding;

    wxNativeFontInfo m_nativeFontInfo;

    // A list of wxXFonts
    wxList        m_fonts;
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxXFont
// ----------------------------------------------------------------------------

wxXFont::wxXFont()
{
    m_fontStruct = (WXFontStructPtr) 0;
    m_fontList = (WXFontList) 0;
#if wxCHECK_MOTIF_VERSION( 2, 0 )
    m_renderTable = (WXRenderTable) 0;
#endif
    m_display = (WXDisplay*) 0;
    m_scale = 100;
}

wxXFont::~wxXFont()
{
    XmFontList fontList = (XmFontList) m_fontList;
    XmFontListFree (fontList);

#if wxCHECK_MOTIF_VERSION( 2, 0 ) && !wxCHECK_LESSTIF()
    XmRenderTable renderTable = (XmRenderTable) m_renderTable;
    XmRenderTableFree (renderTable);
#endif

    // TODO: why does freeing the font produce a segv???
    // Note that XFreeFont wasn't called in wxWin 1.68 either.
    // XFontStruct* fontStruct = (XFontStruct*) m_fontStruct;
    //        XFreeFont((Display*) m_display, fontStruct);
}

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

wxFontRefData::~wxFontRefData()
{
    wxList::compatibility_iterator node = m_fonts.GetFirst();
    while (node)
    {
        wxXFont* f = (wxXFont*) node->GetData();
        delete f;
        node = node->GetNext();
    }
    m_fonts.Clear();
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

wxFont::wxFont(const wxNativeFontInfo& info)
{
    Init();

    (void)Create(info.GetXFontName());
}

void wxFont::Init()
{
}

bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& faceName,
                    wxFontEncoding encoding)
{
    UnRef();
    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, faceName, encoding);

    RealizeResource();

    return true;
}

bool wxFont::Create(const wxString& fontname, wxFontEncoding enc)
{
    if( !fontname )
    {
        *this = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT);
        return true;
    }

    m_refData = new wxFontRefData();

    M_FONTDATA->m_nativeFontInfo.SetXFontName(fontname);  // X font name

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
            return false;
    }
    return true;
}

wxFont::~wxFont()
{
}

// ----------------------------------------------------------------------------
// change the font attributes
// ----------------------------------------------------------------------------

void wxFont::Unshare()
{
    // Don't change shared data
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
    M_FONTDATA->m_nativeFontInfo.GetXFontName().Clear(); // invalid now

    RealizeResource();
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->m_family = family;
    M_FONTDATA->m_nativeFontInfo.GetXFontName().Clear(); // invalid now

    RealizeResource();
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->m_style = style;
    M_FONTDATA->m_nativeFontInfo.GetXFontName().Clear(); // invalid now

    RealizeResource();
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->m_weight = weight;
    M_FONTDATA->m_nativeFontInfo.GetXFontName().Clear(); // invalid now

    RealizeResource();
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_faceName = faceName;
    M_FONTDATA->m_nativeFontInfo.GetXFontName().Clear(); // invalid now

    RealizeResource();
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->m_underlined = underlined;

    RealizeResource();
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->m_encoding = encoding;
    M_FONTDATA->m_nativeFontInfo.GetXFontName().Clear(); // invalid now

    RealizeResource();
}

void wxFont::DoSetNativeFontInfo(const wxNativeFontInfo& info)
{
    Unshare();

    M_FONTDATA->m_nativeFontInfo = info;
}

// ----------------------------------------------------------------------------
// query font attributes
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_pointSize;
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), wxT(""), wxT("invalid font") );

    return M_FONTDATA->m_faceName ;
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
    wxCHECK_MSG( Ok(), false, wxT("invalid font") );

    return M_FONTDATA->m_underlined;
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->m_encoding;
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    wxCHECK_MSG( Ok(), (wxNativeFontInfo *)NULL, wxT("invalid font") );

    if(M_FONTDATA->m_nativeFontInfo.GetXFontName().IsEmpty())
        GetInternalFont();

    return &(M_FONTDATA->m_nativeFontInfo);
}

// ----------------------------------------------------------------------------
// real implementation
// ----------------------------------------------------------------------------

// Find an existing, or create a new, XFontStruct
// based on this wxFont and the given scale. Append the
// font to list in the private data for future reference.
wxXFont* wxFont::GetInternalFont(double scale, WXDisplay* display) const
{
    if ( !Ok() )
        return (wxXFont *)NULL;

    long intScale = long(scale * 100.0 + 0.5); // key for wxXFont
    int pointSize = (M_FONTDATA->m_pointSize * 10 * intScale) / 100;

    // search existing fonts first
    wxList::compatibility_iterator node = M_FONTDATA->m_fonts.GetFirst();
    while (node)
    {
        wxXFont* f = (wxXFont*) node->GetData();
        if ((!display || (f->m_display == display)) && (f->m_scale == intScale))
            return f;
        node = node->GetNext();
    }

    // not found, create a new one
    XFontStruct *font = (XFontStruct *)
                        wxLoadQueryNearestFont(pointSize,
                                               M_FONTDATA->m_family,
                                               M_FONTDATA->m_style,
                                               M_FONTDATA->m_weight,
                                               M_FONTDATA->m_underlined,
                                               wxT(""),
                                               M_FONTDATA->m_encoding);

    if ( !font )
    {
        wxFAIL_MSG( wxT("Could not allocate even a default font -- something is wrong.") );

        return (wxXFont*) NULL;
    }

    wxXFont* f = new wxXFont;
    f->m_fontStruct = (WXFontStructPtr)font;
    f->m_display = ( display ? display : wxGetDisplay() );
    f->m_scale = intScale;
    f->m_fontList = XmFontListCreate ((XFontStruct*) font, XmSTRING_DEFAULT_CHARSET);
    M_FONTDATA->m_fonts.Append(f);

#if wxCHECK_MOTIF_VERSION( 2, 0 ) && !wxCHECK_LESSTIF()
    XmRendition rendition;
    XmRenderTable renderTable;
    Arg args[5];
    int count = 0;

    XtSetArg( args[count], XmNfont, font ); ++count;
    XtSetArg( args[count], XmNunderlineType,
              GetUnderlined() ? XmSINGLE_LINE : XmNO_LINE ); ++count;
    rendition = XmRenditionCreate( XmGetXmDisplay( (Display*)f->m_display ),
                                   (XmStringTag)"",
                                   args, count );
    renderTable = XmRenderTableAddRenditions( NULL, &rendition, 1,
                                              XmMERGE_REPLACE );

    f->m_renderTable = (WXRenderTable)renderTable;
#endif

    return f;
}

WXFontStructPtr wxFont::GetFontStruct(double scale, WXDisplay* display) const
{
    wxXFont* f = GetInternalFont(scale, display);

    return (f ? f->m_fontStruct : (WXFontStructPtr) 0);
}

WXFontList wxFont::GetFontList(double scale, WXDisplay* display) const
{
    wxXFont* f = GetInternalFont(scale, display);

    return (f ? f->m_fontList : (WXFontList) 0);
}

#if wxCHECK_MOTIF_VERSION( 2, 0 )

WXRenderTable wxFont::GetRenderTable(WXDisplay* display) const
{
    wxXFont* f = GetInternalFont(1.0, display);

    return (f ? f->m_renderTable : (WXFontList) 0);
}

#endif

WXFontType wxFont::GetFontType(WXDisplay* display) const
{
#if wxCHECK_MOTIF_VERSION( 2, 0 ) && !wxCHECK_LESSTIF()
    return Ok() ? GetRenderTable(display) : NULL;
#else
    return Ok() ? GetFontList(1.0, display) : NULL;
#endif
}

/*static*/ WXString wxFont::GetFontTag()
{
#if wxCHECK_MOTIF_VERSION( 2, 0 ) && !wxCHECK_LESSTIF()
    return (WXString)XmNrenderTable;
#else
    return (WXString)XmNfontList;
#endif
}
