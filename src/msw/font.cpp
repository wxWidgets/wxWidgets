/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
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

#include "wx/msw/private.h"

#include "wx/fontutil.h"
#include "wx/fontmap.h"

#include "wx/tokenzr.h"

IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// the mask used to extract the pitch from LOGFONT::lfPitchAndFamily field
static const int PITCH_MASK = FIXED_PITCH | VARIABLE_PITCH;

// ----------------------------------------------------------------------------
// wxFontRefData - the internal description of the font
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFontRefData: public wxGDIRefData
{
public:
    // constructors
    wxFontRefData()
    {
        Init(-1, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL,
             FALSE, wxEmptyString, wxFONTENCODING_DEFAULT);
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

    wxFontRefData(const wxNativeFontInfo& info, WXHFONT hFont = 0)
    {
        Init(info, hFont);
    }

    wxFontRefData(const wxFontRefData& data)
    {
        if ( data.m_nativeFontInfoOk )
        {
            Init(data.m_nativeFontInfo);
        }
        else
        {
            Init(data.m_pointSize, data.m_family, data.m_style, data.m_weight,
                 data.m_underlined, data.m_faceName, data.m_encoding);
        }
    }

    virtual ~wxFontRefData();

    // operations
    bool Alloc(wxFont *font);

    void Free();

    // all wxFont accessors
    int GetPointSize() const
    {
        return m_nativeFontInfoOk ? m_nativeFontInfo.GetPointSize()
                                  : m_pointSize;
    }

    int GetFamily() const
    {
        return m_family;
    }

    int GetStyle() const
    {
        return m_nativeFontInfoOk ? m_nativeFontInfo.GetStyle()
                                  : m_style;
    }

    int GetWeight() const
    {
        return m_nativeFontInfoOk ? m_nativeFontInfo.GetWeight()
                                  : m_weight;
    }

    bool GetUnderlined() const
    {
        return m_nativeFontInfoOk ? m_nativeFontInfo.GetUnderlined()
                                  : m_underlined;
    }

    wxString GetFaceName() const
    {
        wxString s;
        if ( m_nativeFontInfoOk )
            s = m_nativeFontInfo.GetFaceName();
        else
            s = m_faceName;

        return s;
    }

    wxFontEncoding GetEncoding() const
    {
        return m_nativeFontInfoOk ? m_nativeFontInfo.GetEncoding()
                                  : m_encoding;
    }

    WXHFONT GetHFONT() const { return m_hFont; }

    // ... and setters
    void SetPointSize(int pointSize)
    {
        if ( m_nativeFontInfoOk )
            m_nativeFontInfo.SetPointSize(pointSize);
        else
            m_pointSize = pointSize;
    }

    void SetFamily(int family)
    {
        m_family = family;
    }

    void SetStyle(int style)
    {
        if ( m_nativeFontInfoOk )
            m_nativeFontInfo.SetStyle((wxFontStyle)style);
        else
            m_style = style;
    }

    void SetWeight(int weight)
    {
        if ( m_nativeFontInfoOk )
            m_nativeFontInfo.SetWeight((wxFontWeight)weight);
        else
            m_weight = weight;
    }

    void SetFaceName(const wxString& faceName)
    {
        if ( m_nativeFontInfoOk )
            m_nativeFontInfo.SetFaceName(faceName);
        else
            m_faceName = faceName;
    }

    void SetUnderlined(bool underlined)
    {
        if ( m_nativeFontInfoOk )
            m_nativeFontInfo.SetUnderlined(underlined);
        else
            m_underlined = underlined;
    }

    void SetEncoding(wxFontEncoding encoding)
    {
        if ( m_nativeFontInfoOk )
            m_nativeFontInfo.SetEncoding(encoding);
        else
            m_encoding = encoding;
    }

    // native font info tests
    bool HasNativeFontInfo() const { return m_nativeFontInfoOk; }

    const wxNativeFontInfo& GetNativeFontInfo() const
        { return m_nativeFontInfo; }

protected:
    // common part of all ctors
    void Init(int size,
              int family,
              int style,
              int weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    void Init(const wxNativeFontInfo& info, WXHFONT hFont = 0);

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
    m_pointSize = pointSize == -1 ? wxNORMAL_FONT->GetPointSize() : pointSize;
    m_family = family;
    m_style = style;
    m_weight = weight;
    m_underlined = underlined;
    m_faceName = faceName;
    m_encoding = encoding;

    m_hFont = 0;

    m_nativeFontInfoOk = FALSE;
}

void wxFontRefData::Init(const wxNativeFontInfo& info, WXHFONT hFont)
{
    // hFont may be zero, or it be passed in case we really want to
    // use the exact font created in the underlying system
    // (for example where we can't guarantee conversion from HFONT
    // to LOGFONT back to HFONT)
    m_hFont = hFont;

    m_nativeFontInfoOk = TRUE;
    m_nativeFontInfo = info;
    // This is the best we can do since we don't have the
    // correct information at this point.
    m_family = wxSWISS;
}

wxFontRefData::~wxFontRefData()
{
    Free();
}

bool wxFontRefData::Alloc(wxFont *font)
{
    if ( !m_nativeFontInfoOk )
    {
        wxFillLogFont(&m_nativeFontInfo.lf, font);
        m_nativeFontInfoOk = TRUE;
    }

    HFONT hfont = ::CreateFontIndirect(&m_nativeFontInfo.lf);
    if ( !hfont )
    {
        wxLogLastError(wxT("CreateFont"));

        return FALSE;
    }

    m_hFont = (WXHFONT)hfont;

    return TRUE;
}

void wxFontRefData::Free()
{
    if ( m_hFont )
    {
        if ( !::DeleteObject((HFONT) m_hFont) )
        {
            wxLogLastError(wxT("DeleteObject(font)"));
        }

        m_hFont = 0;
    }
}

// ----------------------------------------------------------------------------
// wxNativeFontInfo
// ----------------------------------------------------------------------------

void wxNativeFontInfo::Init()
{
    wxZeroMemory(lf);
}

int wxNativeFontInfo::GetPointSize() const
{
    // FIXME: using the screen here results in incorrect font size calculation
    //        for printing!
    const int ppInch = ::GetDeviceCaps(ScreenHDC(), LOGPIXELSY);

    return (int) (((72.0*(double)abs(lf.lfHeight)) / (double) ppInch) + 0.5);
}

wxFontStyle wxNativeFontInfo::GetStyle() const
{
    return lf.lfItalic ? wxFONTSTYLE_ITALIC : wxFONTSTYLE_NORMAL;
}

wxFontWeight wxNativeFontInfo::GetWeight() const
{
    if ( lf.lfWeight <= 300 )
        return wxFONTWEIGHT_LIGHT;

    if ( lf.lfWeight >= 600 )
        return wxFONTWEIGHT_BOLD;

    return wxFONTWEIGHT_NORMAL;
}

bool wxNativeFontInfo::GetUnderlined() const
{
    return lf.lfUnderline != 0;
}

wxString wxNativeFontInfo::GetFaceName() const
{
    return lf.lfFaceName;
}

wxFontFamily wxNativeFontInfo::GetFamily() const
{
    wxFontFamily family;

    // extract family from pitch-and-family
    switch ( lf.lfPitchAndFamily & ~PITCH_MASK )
    {
        case FF_ROMAN:
            family = wxFONTFAMILY_ROMAN;
            break;

        default:
            wxFAIL_MSG( _T("unknown LOGFONT::lfFamily value") );
            // fall through

        case FF_SWISS:
            family = wxFONTFAMILY_SWISS;
            break;

        case FF_SCRIPT:
            family = wxFONTFAMILY_SCRIPT;
            break;

        case FF_MODERN:
            family = wxFONTFAMILY_MODERN;
            break;

        case FF_DECORATIVE:
            family = wxFONTFAMILY_DECORATIVE;
            break;
    }

    return family;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
    return wxGetFontEncFromCharSet(lf.lfCharSet);
}

void wxNativeFontInfo::SetPointSize(int pointsize)
{
#if wxFONT_SIZE_COMPATIBILITY
    // Incorrect, but compatible with old wxWindows behaviour
    lf.lfHeight = (pointSize*ppInch)/72;
#else // wxFONT_SIZE_COMPATIBILITY
    // FIXME: using the screen here results in incorrect font size calculation
    //        for printing!
    const int ppInch = ::GetDeviceCaps(ScreenHDC(), LOGPIXELSY);

    lf.lfHeight = -(int)((pointsize*((double)ppInch)/72.0) + 0.5);
#endif // wxFONT_SIZE_COMPATIBILITY/!wxFONT_SIZE_COMPATIBILITY
}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{
    switch ( style )
    {
        default:
            wxFAIL_MSG( _T("unknown font style") );
            // fall through

        case wxFONTSTYLE_NORMAL:
            lf.lfItalic = FALSE;
            break;

        case wxFONTSTYLE_ITALIC:
        case wxFONTSTYLE_SLANT:
            lf.lfItalic = TRUE;
            break;
    }
}

void wxNativeFontInfo::SetWeight(wxFontWeight weight)
{
    switch ( weight )
    {
        default:
            wxFAIL_MSG( _T("unknown font weight") );
            // fall through

        case wxFONTWEIGHT_NORMAL:
            lf.lfWeight = FW_NORMAL;
            break;

        case wxFONTWEIGHT_LIGHT:
            lf.lfWeight = FW_LIGHT;
            break;

        case wxFONTWEIGHT_BOLD:
            lf.lfWeight = FW_BOLD;
            break;
    }
}

void wxNativeFontInfo::SetUnderlined(bool underlined)
{
    lf.lfUnderline = underlined;
}

void wxNativeFontInfo::SetFaceName(wxString facename)
{
    wxStrncpy(lf.lfFaceName, facename, WXSIZEOF(lf.lfFaceName));
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{
    int ff_family;
    wxString facename;

    switch ( family )
    {
        case wxSCRIPT:
            ff_family = FF_SCRIPT;
            facename = _T("Script");
            break;

        case wxDECORATIVE:
            ff_family = FF_DECORATIVE;
            facename = _T("Old English Text MT");
            break;

        case wxROMAN:
            ff_family = FF_ROMAN;
            facename = _T("Times New Roman");
            break;

        case wxTELETYPE:
        case wxMODERN:
            ff_family = FF_MODERN;
            facename = _T("Courier New");
            break;

        case wxSWISS:
            ff_family = FF_SWISS;
            facename = _T("Arial");
            break;

        case wxDEFAULT:
        default:
            ff_family = FF_SWISS;
            facename = _T("MS Sans Serif");
    }

    lf.lfPitchAndFamily = DEFAULT_PITCH | ff_family;

    if ( !wxStrlen(lf.lfFaceName) )
    {
        SetFaceName(facename);
    }
}

void wxNativeFontInfo::SetEncoding(wxFontEncoding encoding)
{
    wxNativeEncodingInfo info;
    if ( !wxGetNativeFontEncoding(encoding, &info) )
    {
#if wxUSE_FONTMAP
        if ( wxFontMapper::Get()->GetAltForEncoding(encoding, &info) )
        {
            if ( !info.facename.empty() )
            {
                // if we have this encoding only in some particular facename, use
                // the facename - it is better to show the correct characters in a
                // wrong facename than unreadable text in a correct one
                SetFaceName(info.facename);
            }
        }
        else
#endif // wxUSE_FONTMAP
        {
            // unsupported encoding, replace with the default
            info.charset = DEFAULT_CHARSET;
        }
    }

    lf.lfCharSet = info.charset;
}

bool wxNativeFontInfo::FromString(const wxString& s)
{
    long l;

    wxStringTokenizer tokenizer(s, _T(";"));

    // first the version
    wxString token = tokenizer.GetNextToken();
    if ( token != _T('0') )
        return FALSE;

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
    lf.lfItalic = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfUnderline = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfStrikeOut = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfCharSet = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfOutPrecision = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfClipPrecision = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfQuality = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return FALSE;
    lf.lfPitchAndFamily = (BYTE)l;

    token = tokenizer.GetNextToken();
    if(!token)
        return FALSE;
    wxStrcpy(lf.lfFaceName, token.c_str());

    return TRUE;
}

wxString wxNativeFontInfo::ToString() const
{
    wxString s;

    s.Printf(_T("%d;%ld;%ld;%ld;%ld;%ld;%d;%d;%d;%d;%d;%d;%d;%d;%s"),
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
}

bool wxFont::Create(const wxNativeFontInfo& info, WXHFONT hFont)
{
    UnRef();

    m_refData = new wxFontRefData(info, hFont);

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
    {
        pointSize = wxNORMAL_FONT->GetPointSize();
    }

    m_refData = new wxFontRefData(pointSize, family, style, weight,
                                  underlined, faceName, encoding);

    RealizeResource();

    return TRUE;
}

wxFont::~wxFont()
{
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

    return M_FONTDATA->Alloc(this);
}

bool wxFont::FreeResource(bool WXUNUSED(force))
{
    if ( GetResourceHandle() )
    {
        M_FONTDATA->Free();

        return TRUE;
    }

    return FALSE;
}

WXHANDLE wxFont::GetResourceHandle() const
{
    return (WXHANDLE)GetHFONT();
}

WXHFONT wxFont::GetHFONT() const
{
    return M_FONTDATA ? M_FONTDATA->GetHFONT() : 0;
}

bool wxFont::IsFree() const
{
    return M_FONTDATA && (M_FONTDATA->GetHFONT() == 0);
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

    M_FONTDATA->SetPointSize(pointSize);

    RealizeResource();
}

void wxFont::SetFamily(int family)
{
    Unshare();

    M_FONTDATA->SetFamily(family);

    RealizeResource();
}

void wxFont::SetStyle(int style)
{
    Unshare();

    M_FONTDATA->SetStyle(style);

    RealizeResource();
}

void wxFont::SetWeight(int weight)
{
    Unshare();

    M_FONTDATA->SetWeight(weight);

    RealizeResource();
}

void wxFont::SetFaceName(const wxString& faceName)
{
    Unshare();

    M_FONTDATA->SetFaceName(faceName);

    RealizeResource();
}

void wxFont::SetUnderlined(bool underlined)
{
    Unshare();

    M_FONTDATA->SetUnderlined(underlined);

    RealizeResource();
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    Unshare();

    M_FONTDATA->SetEncoding(encoding);

    RealizeResource();
}

void wxFont::DoSetNativeFontInfo(const wxNativeFontInfo& info)
{
    Unshare();

    FreeResource();

    *M_FONTDATA = wxFontRefData(info);

    RealizeResource();
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->GetPointSize();
}

int wxFont::GetFamily() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->GetFamily();
}

int wxFont::GetStyle() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->GetStyle();
}

int wxFont::GetWeight() const
{
    wxCHECK_MSG( Ok(), 0, wxT("invalid font") );

    return M_FONTDATA->GetWeight();
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( Ok(), FALSE, wxT("invalid font") );

    return M_FONTDATA->GetUnderlined();
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( Ok(), wxEmptyString, wxT("invalid font") );

    return M_FONTDATA->GetFaceName();
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( Ok(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->GetEncoding();
}

wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    if ( M_FONTDATA->HasNativeFontInfo() )
        return new wxNativeFontInfo(M_FONTDATA->GetNativeFontInfo());

    return 0;
}

bool wxFont::IsFixedWidth() const
{
    if ( M_FONTDATA->HasNativeFontInfo() )
    {
        // the two low-order bits specify the pitch of the font, the rest is
        // family
        BYTE pitch = M_FONTDATA->GetNativeFontInfo().
                        lf.lfPitchAndFamily & PITCH_MASK;

        return pitch == FIXED_PITCH;
    }

    return wxFontBase::IsFixedWidth();
}

