/////////////////////////////////////////////////////////////////////////////
// Name:        font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:       wxWindows licence
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

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/font.h"
    #include "wx/log.h"
#endif // WX_PRECOMP

#include "wx/fontutil.h"
#include "wx/tokenzr.h"

#include "wx/msw/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the default font size in points
static const int wxDEFAULT_FONT_SIZE = 12;

// ----------------------------------------------------------------------------
// wxFontRefData - the internal description of the font
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontRefData: public wxGDIRefData
{
friend class WXDLLEXPORT wxFont;

public:
    wxFontRefData()
    {
        Init(wxDEFAULT_FONT_SIZE, wxDEFAULT, wxNORMAL, wxNORMAL, FALSE,
             "", wxFONTENCODING_DEFAULT);
    }

    wxFontRefData(const wxFontRefData& data)
    {
        Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
             data.m_underlined, data.m_faceName, data.m_encoding);

        m_fontId = data.m_fontId;
    }

    wxFontRefData(int size,
                  int family,
                  int style,
                  int weight,
                  bool underlined,
                  const wxString& faceName,
                  wxFontEncoding encoding)
    {
        Init(size, family, style, weight, underlined, faceName, encoding);
    }

    wxFontRefData(const wxNativeFontInfo& info)
    {
        Init(info);
    }

    virtual ~wxFontRefData();

protected:
    // common part of all ctors
    void Init(int size,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    void Init(const wxNativeFontInfo& info);

    // If TRUE, the pointer to the actual font is temporary and SHOULD NOT BE
    // DELETED by destructor
    bool          m_temporary;

    int           m_fontId;

    // font characterstics
    int           m_pointSize;
    int           m_family;
    int           m_style;
    int           m_weight;
    bool          m_underlined;
    wxString      m_faceName;
    wxFontEncoding m_encoding;

    // Windows font handle
    WXHFONT       m_hFont;

    // Native font info
    wxNativeFontInfo m_nativeFontInfo;
    bool             m_nativeFontInfoOk;
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
    m_style = style;
    m_pointSize = pointSize;
    m_family = family;
    m_style = style;
    m_weight = weight;
    m_underlined = underlined;
    m_faceName = faceName;
    m_encoding = encoding;

    m_fontId = 0;
    m_temporary = FALSE;

    m_hFont = 0;

    m_nativeFontInfoOk = FALSE;
}

void wxFontRefData::Init(const wxNativeFontInfo& info)
{
    // extract family from pitch-and-family
    int lfFamily = info.lf.lfPitchAndFamily;
    if ( lfFamily & FIXED_PITCH )
        lfFamily -= FIXED_PITCH;
    if ( lfFamily & VARIABLE_PITCH )
        lfFamily -= VARIABLE_PITCH;

    switch ( lfFamily )
    {
        case FF_ROMAN:
            m_family = wxROMAN;
            break;

        case FF_SWISS:
            m_family = wxSWISS;
            break;

        case FF_SCRIPT:
            m_family = wxSCRIPT;
            break;

        case FF_MODERN:
            m_family = wxMODERN;
            break;

        case FF_DECORATIVE:
            m_family = wxDECORATIVE;
            break;

        default:
            m_family = wxSWISS;
    }

    // weight and style
    switch ( info.lf.lfWeight )
    {
        case FW_LIGHT:
            m_weight = wxLIGHT;
            break;

        default:
        case FW_NORMAL:
            m_weight = wxNORMAL;
            break;

        case FW_BOLD:
            m_weight = wxBOLD;
            break;
    }

    m_style = info.lf.lfItalic ? wxITALIC : wxNORMAL;

    m_underlined = info.lf.lfUnderline != 0;

    m_faceName = info.lf.lfFaceName;

    int height = abs(info.lf.lfHeight);

    // remember that 1pt = 1/72inch
    const int ppInch = ::GetDeviceCaps(ScreenHDC(), LOGPIXELSY);
    m_pointSize = (int) (((72.0*((double)height))/(double) ppInch) + 0.5);

    m_encoding = wxGetFontEncFromCharSet(info.lf.lfCharSet);

    m_fontId = 0;
    m_temporary = FALSE;

    m_hFont = 0;

    m_nativeFontInfoOk = TRUE;
    m_nativeFontInfo = info;
}

wxFontRefData::~wxFontRefData()
{
    if ( m_hFont )
    {
        if ( !::DeleteObject((HFONT) m_hFont) )
        {
            wxLogLastError(wxT("DeleteObject(font)"));
        }
    }
}


// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

bool wxNativeFontInfo::FromString(const wxString& s)
{
    long l;

    wxStringTokenizer tokenizer(s, _T(";"));

    wxString token = tokenizer.GetNextToken();
    //
    //  Ignore the version for now
    //

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfHeight = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfWidth = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfEscapement = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfOrientation = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfWeight = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfItalic = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfUnderline = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfStrikeOut = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfCharSet = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfOutPrecision = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfClipPrecision = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfQuality = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfPitchAndFamily = l;

    token = tokenizer.GetNextToken();
    if(!token)
        return FALSE;
    wxStrcpy(lf.lfFaceName, token.c_str());

    return TRUE;
}

wxString wxNativeFontInfo::ToString() const
{
    wxString s;

    s.Printf(_T("%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%d;%s"),
             0, // version, in case we want to change the format later
             lf.lfHeight,
             lf.lfWidth,
             lf.lfEscapement,
             lf.lfOrientation,
             lf.lfWeight,
             lf.lfItalic,
             lf.lfUnderline,
             lf.lfStrikeOut,
             lf.lfCharSet,
             lf.lfOutPrecision,
             lf.lfClipPrecision,
             lf.lfQuality,
             lf.lfPitchAndFamily,
             lf.lfFaceName);

    return s;
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

void wxFont::Init()
{
    if ( wxTheFontList )
        wxTheFontList->Append(this);
}

bool wxFont::Create(const wxNativeFontInfo& info)
{
    UnRef();

    m_refData = new wxFontRefData(info);

    RealizeResource();

    return TRUE;
}

wxFont::wxFont(const wxString& fontdesc)
{
    wxNativeFontInfo info;
    if ( info.FromString(fontdesc) )
        (void)Create(info);
}

/* Constructor for a font. Note that the real construction is done
 * in wxDC::SetFont, when information is available about scaling etc.
 */
bool wxFont::Create(int pointSize,
                    int family,
                    int style,
                    int weight,
                    bool underlined,
                    const wxString& faceName,
                    wxFontEncoding encoding)
{
    UnRef();

    // wxDEFAULT is a valid value for the font size too so we must treat it
    // specially here (otherwise the size would be 70 == wxDEFAULT value)
    if ( pointSize == wxDEFAULT )
        pointSize = wxDEFAULT_FONT_SIZE;

    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, faceName, encoding);

    RealizeResource();

    return TRUE;
}

wxFont::~wxFont()
{
    if ( wxTheFontList )
        wxTheFontList->DeleteObject(this);
}

// ----------------------------------------------------------------------------
// real implementation
// ----------------------------------------------------------------------------

bool wxFont::RealizeResource()
{
    if ( GetResourceHandle() )
    {
        // VZ: the old code returned FALSE in this case, but it doesn't seem
        //     to make sense because the font _was_ created
        return TRUE;
    }

    if(!M_FONTDATA->m_nativeFontInfoOk)
    {
      wxFillLogFont(&M_FONTDATA->m_nativeFontInfo.lf, this);
      M_FONTDATA->m_nativeFontInfoOk = TRUE;
    }

    M_FONTDATA->m_hFont = (WXHFONT)::CreateFontIndirect(&M_FONTDATA->m_nativeFontInfo.lf);
    M_FONTDATA->m_faceName = M_FONTDATA->m_nativeFontInfo.lf.lfFaceName;
    if ( !M_FONTDATA->m_hFont )
    {
        wxLogLastError(wxT("CreateFont"));

        return FALSE;
    }

    return TRUE;
}

bool wxFont::FreeResource(bool force)
{
    if ( GetResourceHandle() )
    {
        if ( !::DeleteObject((HFONT) M_FONTDATA->m_hFont) )
        {
            wxLogLastError(wxT("DeleteObject(font)"));
        }

        M_FONTDATA->m_hFont = 0;

        return TRUE;
    }
    return FALSE;
}

WXHANDLE wxFont::GetResourceHandle()
{
    return GetHFONT();
}

WXHFONT wxFont::GetHFONT() const
{
    if ( !M_FONTDATA )
        return 0;
    else
        return (WXHANDLE)M_FONTDATA->m_hFont;
}

bool wxFont::IsFree() const
{
    return (M_FONTDATA && (M_FONTDATA->m_hFont == 0));
}

void wxFont::Unshare()
{
    // Don't change shared data
    if ( !m_refData )
    {
        m_refData = new wxFontRefData();
    }
    else
    {
        wxFontRefData* ref = new wxFontRefData(*M_FONTDATA);
        UnRef();
        m_refData = ref;
    }
}

// ----------------------------------------------------------------------------
// change font attribute: we recreate font when doing it
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    Unshare();

    M_FONTDATA->m_pointSize = pointSize;
    M_FONTDATA->m_nativeFontInfoOk = FALSE;

    RealizeResource();
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->m_family = family;
    M_FONTDATA->m_nativeFontInfoOk = FALSE;

    RealizeResource();
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->m_style = style;
    M_FONTDATA->m_nativeFontInfoOk = FALSE;

    RealizeResource();
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->m_weight = weight;
    M_FONTDATA->m_nativeFontInfoOk = FALSE;

    RealizeResource();
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->m_faceName = faceName;
    M_FONTDATA->m_nativeFontInfoOk = FALSE;

    RealizeResource();
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->m_underlined = underlined;
    M_FONTDATA->m_nativeFontInfoOk = FALSE;

    RealizeResource();
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->m_encoding = encoding;
    M_FONTDATA->m_nativeFontInfoOk = FALSE;

    RealizeResource();
}

void wxFont::SetNativeFontInfo(const wxNativeFontInfo& info)
{
    Unshare();

    FreeResource();

    M_FONTDATA->Init(info);

    RealizeResource();
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_pointSize;
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_family;
}

int wxFont::GetFontId() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->m_fontId;
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

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), wxT(""), wxT("invalid font") );

    return M_FONTDATA->m_faceName;
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->m_encoding;
}

wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    if( M_FONTDATA->m_nativeFontInfoOk )
        return new wxNativeFontInfo(M_FONTDATA->m_nativeFontInfo);

    return 0;
}

