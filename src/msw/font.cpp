/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/font.cpp
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/font.h"

#ifndef WX_PRECOMP
    #include "wx/list.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/encinfo.h"
#endif // WX_PRECOMP

#include "wx/msw/private.h"

#include "wx/fontutil.h"
#include "wx/fontmap.h"

#ifndef __WXWINCE__
    #include "wx/sysopt.h"
#endif

#include "wx/tokenzr.h"

#if wxUSE_EXTENDED_RTTI

wxBEGIN_ENUM( wxFontFamily )
    wxENUM_MEMBER( wxDEFAULT )
    wxENUM_MEMBER( wxDECORATIVE )
    wxENUM_MEMBER( wxROMAN )
    wxENUM_MEMBER( wxSCRIPT )
    wxENUM_MEMBER( wxSWISS )
    wxENUM_MEMBER( wxMODERN )
    wxENUM_MEMBER( wxTELETYPE )
wxEND_ENUM( wxFontFamily )

wxBEGIN_ENUM( wxFontStyle )
    wxENUM_MEMBER( wxNORMAL )
    wxENUM_MEMBER( wxITALIC )
    wxENUM_MEMBER( wxSLANT )
wxEND_ENUM( wxFontStyle )

wxBEGIN_ENUM( wxFontWeight )
    wxENUM_MEMBER( wxNORMAL )
    wxENUM_MEMBER( wxLIGHT )
    wxENUM_MEMBER( wxBOLD )
wxEND_ENUM( wxFontWeight )

IMPLEMENT_DYNAMIC_CLASS_WITH_COPY_XTI(wxFont, wxGDIObject,"wx/font.h")

wxBEGIN_PROPERTIES_TABLE(wxFont)
    wxPROPERTY( Size,int, SetPointSize, GetPointSize, 12 , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Family, int  , SetFamily, GetFamily, (int)wxDEFAULT , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // wxFontFamily
    wxPROPERTY( Style, int , SetStyle, GetStyle, (int)wxNORMAL , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // wxFontStyle
    wxPROPERTY( Weight, int , SetWeight, GetWeight, (int)wxNORMAL , 0 /*flags*/ , wxT("Helpstring") , wxT("group")) // wxFontWeight
    wxPROPERTY( Underlined, bool , SetUnderlined, GetUnderlined, false , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Face, wxString , SetFaceName, GetFaceName, EMPTY_MACROVALUE , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
    wxPROPERTY( Encoding, wxFontEncoding , SetEncoding, GetEncoding, wxFONTENCODING_DEFAULT , 0 /*flags*/ , wxT("Helpstring") , wxT("group"))
wxEND_PROPERTIES_TABLE()

wxCONSTRUCTOR_6( wxFont , int , Size , int , Family , int , Style , int , Weight , bool , Underlined , wxString , Face )

wxBEGIN_HANDLERS_TABLE(wxFont)
wxEND_HANDLERS_TABLE()

#else
    IMPLEMENT_DYNAMIC_CLASS(wxFont, wxGDIObject)
#endif


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
        Init(-1, wxSize(0,0), false, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL,
             wxFONTWEIGHT_NORMAL, false, wxEmptyString,
             wxFONTENCODING_DEFAULT);
    }

    wxFontRefData(int size,
                  const wxSize& pixelSize,
                  bool sizeUsingPixels,
                  wxFontFamily family,
                  wxFontStyle style,
                  wxFontWeight weight,
                  bool underlined,
                  const wxString& faceName,
                  wxFontEncoding encoding)
    {
        Init(size, pixelSize, sizeUsingPixels, family, style, weight,
             underlined, faceName, encoding);
    }

    wxFontRefData(const wxNativeFontInfo& info, WXHFONT hFont = 0)
    {
        Init(info, hFont);
    }

    wxFontRefData(const wxFontRefData& data) : wxGDIRefData()
    {
        Init(data.m_nativeFontInfo);
    }

    virtual ~wxFontRefData();

    // operations
    bool Alloc();

    void Free();

    // all wxFont accessors
    int GetPointSize() const
    {
        return m_nativeFontInfo.GetPointSize();
    }

    wxSize GetPixelSize() const
    {
        return m_nativeFontInfo.GetPixelSize();
    }

    bool IsUsingSizeInPixels() const
    {
        return m_sizeUsingPixels;
    }

    wxFontFamily GetFamily() const
    {
        return m_nativeFontInfo.GetFamily();
    }

    wxFontStyle GetStyle() const
    {
        return m_nativeFontInfo.GetStyle();
    }

    wxFontWeight GetWeight() const
    {
        return m_nativeFontInfo.GetWeight();
    }

    bool GetUnderlined() const
    {
        return m_nativeFontInfo.GetUnderlined();
    }

    wxString GetFaceName() const
    {
        return m_nativeFontInfo.GetFaceName();
    }

    wxFontEncoding GetEncoding() const
    {
        return m_nativeFontInfo.GetEncoding();
    }

    WXHFONT GetHFONT() const
    {
        if ( !m_hFont )
            const_cast<wxFontRefData *>(this)->Alloc();

        return (WXHFONT)m_hFont;
    }

    bool HasHFONT() const
    {
        return m_hFont != 0;
    }

    // ... and setters: notice that all of them invalidate the currently
    // allocated HFONT, if any, so that the next call to GetHFONT() recreates a
    // new one
    void SetPointSize(int pointSize)
    {
        Free();

        m_nativeFontInfo.SetPointSize(pointSize);
        m_sizeUsingPixels = false;
    }

    void SetPixelSize(const wxSize& pixelSize)
    {
        Free();

        m_nativeFontInfo.SetPixelSize(pixelSize);
        m_sizeUsingPixels = true;
    }

    void SetFamily(wxFontFamily family)
    {
        Free();

        m_nativeFontInfo.SetFamily(family);
    }

    void SetStyle(wxFontStyle style)
    {
        Free();

        m_nativeFontInfo.SetStyle(style);
    }

    void SetWeight(wxFontWeight weight)
    {
        Free();

        m_nativeFontInfo.SetWeight(weight);
    }

    bool SetFaceName(const wxString& faceName)
    {
        Free();

        return m_nativeFontInfo.SetFaceName(faceName);
    }

    void SetUnderlined(bool underlined)
    {
        Free();

        m_nativeFontInfo.SetUnderlined(underlined);
    }

    void SetEncoding(wxFontEncoding encoding)
    {
        Free();

        m_nativeFontInfo.SetEncoding(encoding);
    }

    const wxNativeFontInfo& GetNativeFontInfo() const
        { return m_nativeFontInfo; }

    void SetNativeFontInfo(const wxNativeFontInfo& nativeFontInfo)
    {
        Free();

        m_nativeFontInfo = nativeFontInfo;
    }

protected:
    // common part of all ctors
    void Init(int size,
              const wxSize& pixelSize,
              bool sizeUsingPixels,
              wxFontFamily family,
              wxFontStyle style,
              wxFontWeight weight,
              bool underlined,
              const wxString& faceName,
              wxFontEncoding encoding);

    void Init(const wxNativeFontInfo& info, WXHFONT hFont = 0);

    // are we using m_nativeFontInfo.lf.lfHeight for point size or pixel size?
    bool             m_sizeUsingPixels;

    // Windows font handle, created on demand in GetHFONT()
    HFONT            m_hFont;

    // Native font info
    wxNativeFontInfo m_nativeFontInfo;
};

#define M_FONTDATA ((wxFontRefData*)m_refData)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxFontRefData
// ----------------------------------------------------------------------------

void wxFontRefData::Init(int pointSize,
                         const wxSize& pixelSize,
                         bool sizeUsingPixels,
                         wxFontFamily family,
                         wxFontStyle style,
                         wxFontWeight weight,
                         bool underlined,
                         const wxString& faceName,
                         wxFontEncoding encoding)
{
    m_hFont = NULL;

    m_sizeUsingPixels = sizeUsingPixels;
    if ( m_sizeUsingPixels )
        SetPixelSize(pixelSize);
    else
        SetPointSize(pointSize);

    SetStyle(style);
    SetWeight(weight);
    SetUnderlined(underlined);

    // set the family/facename
    SetFamily(family);
    if ( !faceName.empty() )
        SetFaceName(faceName);

    // deal with encoding now (it may override the font family and facename
    // so do it after setting them)
    SetEncoding(encoding);
}

void wxFontRefData::Init(const wxNativeFontInfo& info, WXHFONT hFont)
{
    // hFont may be zero, or it be passed in case we really want to
    // use the exact font created in the underlying system
    // (for example where we can't guarantee conversion from HFONT
    // to LOGFONT back to HFONT)
    m_hFont = (HFONT)hFont;
    m_nativeFontInfo = info;

    // TODO: m_sizeUsingPixels?
}

wxFontRefData::~wxFontRefData()
{
    Free();
}

bool wxFontRefData::Alloc()
{
    m_hFont = ::CreateFontIndirect(&m_nativeFontInfo.lf);
    if ( !m_hFont )
    {
        wxLogLastError(wxT("CreateFont"));
        return false;
    }

    return true;
}

void wxFontRefData::Free()
{
    if ( m_hFont )
    {
        if ( !::DeleteObject(m_hFont) )
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

    // we get better font quality if we use PROOF_QUALITY instead of
    // DEFAULT_QUALITY but some fonts (e.g. "Terminal 6pt") are not available
    // then so we allow to set a global option to choose between quality and
    // wider font selection
#ifdef __WXWINCE__
    lf.lfQuality = CLEARTYPE_QUALITY;
#else
    lf.lfQuality = wxSystemOptions::GetOptionInt("msw.font.no-proof-quality")
                    ? DEFAULT_QUALITY
                    : PROOF_QUALITY;
#endif
}

int wxNativeFontInfo::GetPointSize() const
{
    // FIXME: using the screen here results in incorrect font size calculation
    //        for printing!
    const int ppInch = ::GetDeviceCaps(ScreenHDC(), LOGPIXELSY);

    // BC++ 2007 doesn't provide abs(long) overload, hence the cast
    return (int) (((72.0*abs((int)lf.lfHeight)) / (double) ppInch) + 0.5);
}

wxSize wxNativeFontInfo::GetPixelSize() const
{
    wxSize ret;
    ret.SetHeight(abs((int)lf.lfHeight));
    ret.SetWidth(lf.lfWidth);
    return ret;
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
        case 0:
            family = wxFONTFAMILY_UNKNOWN;
            break;

        case FF_ROMAN:
            family = wxFONTFAMILY_ROMAN;
            break;

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

        default:
            wxFAIL_MSG( "unknown LOGFONT::lfFamily value" );
            family = wxFONTFAMILY_UNKNOWN;
                // just to avoid a warning
    }

    return family;
}

wxFontEncoding wxNativeFontInfo::GetEncoding() const
{
    return wxGetFontEncFromCharSet(lf.lfCharSet);
}

void wxNativeFontInfo::SetPointSize(int pointsize)
{
    // FIXME: using the screen here results in incorrect font size calculation
    //        for printing!
    const int ppInch = ::GetDeviceCaps(ScreenHDC(), LOGPIXELSY);

    lf.lfHeight = -(int)((pointsize*((double)ppInch)/72.0) + 0.5);
}

void wxNativeFontInfo::SetPixelSize(const wxSize& pixelSize)
{
    // NOTE: although the MSW port allows for negative pixel size heights,
    //       other ports don't and since it's a very useful feature assert
    //       here if we get a negative height:
    wxCHECK_RET( pixelSize.GetWidth() >= 0 && pixelSize.GetHeight() > 0,
                 "Negative values for the pixel size or zero pixel height are not allowed" );

    lf.lfHeight = pixelSize.GetHeight();
    lf.lfWidth = pixelSize.GetWidth();
}

void wxNativeFontInfo::SetStyle(wxFontStyle style)
{
    switch ( style )
    {
        default:
            wxFAIL_MSG( "unknown font style" );
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
            wxFAIL_MSG( "unknown font weight" );
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

bool wxNativeFontInfo::SetFaceName(const wxString& facename)
{
    wxStrlcpy(lf.lfFaceName, facename.c_str(), WXSIZEOF(lf.lfFaceName));
    return true;
}

void wxNativeFontInfo::SetFamily(wxFontFamily family)
{
    BYTE ff_family;
    wxArrayString facename;

    // the list of fonts associated with a family was partially
    // taken from http://www.codestyle.org/css/font-family

    switch ( family )
    {
        case wxFONTFAMILY_SCRIPT:
            ff_family = FF_SCRIPT;
            facename.Add(wxS("Script"));
            facename.Add(wxS("Brush Script MT"));
            facename.Add(wxS("Comic Sans MS"));
            facename.Add(wxS("Lucida Handwriting"));
            break;

        case wxFONTFAMILY_DECORATIVE:
            ff_family = FF_DECORATIVE;
            facename.Add(wxS("Old English Text MT"));
            facename.Add(wxS("Comic Sans MS"));
            facename.Add(wxS("Lucida Handwriting"));
            break;

        case wxFONTFAMILY_ROMAN:
            ff_family = FF_ROMAN;
            facename.Add(wxS("Times New Roman"));
            facename.Add(wxS("Georgia"));
            facename.Add(wxS("Garamond"));
            facename.Add(wxS("Bookman Old Style"));
            facename.Add(wxS("Book Antiqua"));
            break;

        case wxFONTFAMILY_TELETYPE:
        case wxFONTFAMILY_MODERN:
            ff_family = FF_MODERN;
            facename.Add(wxS("Courier New"));
            facename.Add(wxS("Lucida Console"));
            facename.Add(wxS("Andale Mono"));
            facename.Add(wxS("OCR A Extended"));
            facename.Add(wxS("Terminal"));
            break;

        case wxFONTFAMILY_SWISS:
            ff_family = FF_SWISS;
            facename.Add(wxS("Arial"));
            facename.Add(wxS("Century Gothic"));
            facename.Add(wxS("Lucida Sans Unicode"));
            facename.Add(wxS("Tahoma"));
            facename.Add(wxS("Trebuchet MS"));
            facename.Add(wxS("Verdana"));
            break;

        case wxFONTFAMILY_DEFAULT:
        default:
        {
            // We want Windows 2000 or later to have new fonts even MS Shell Dlg
            // is returned as default GUI font for compatibility
            int verMaj;
            ff_family = FF_SWISS;
            if(wxGetOsVersion(&verMaj) == wxOS_WINDOWS_NT && verMaj >= 5)
                facename.Add(wxS("MS Shell Dlg 2"));
            else
                facename.Add(wxS("MS Shell Dlg"));

            // Quoting the MSDN:
            //     "MS Shell Dlg is a mapping mechanism that enables
            //     U.S. English Microsoft Windows NT, and Microsoft Windows 2000 to
            //     support locales that have characters that are not contained in code
            //     page 1252. It is not a font but a face name for a nonexistent font."
        }
    }

    lf.lfPitchAndFamily = (BYTE)(DEFAULT_PITCH) | ff_family;

    // reset the facename so that CreateFontIndirect() will automatically choose a
    // face name based only on the font family.
    lf.lfFaceName[0] = '\0';
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

    lf.lfCharSet = (BYTE)info.charset;
}

bool wxNativeFontInfo::FromString(const wxString& s)
{
    long l;

    wxStringTokenizer tokenizer(s, wxS(";"));

    // first the version
    wxString token = tokenizer.GetNextToken();
    if ( token != wxS('0') )
        return false;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfHeight = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfWidth = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfEscapement = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfOrientation = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfWeight = l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfItalic = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfUnderline = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfStrikeOut = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfCharSet = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfOutPrecision = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfClipPrecision = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfQuality = (BYTE)l;

    token = tokenizer.GetNextToken();
    if ( !token.ToLong(&l) )
        return false;
    lf.lfPitchAndFamily = (BYTE)l;

    token = tokenizer.GetNextToken();
    if(!token)
        return false;
    wxStrcpy(lf.lfFaceName, token.c_str());

    return true;
}

wxString wxNativeFontInfo::ToString() const
{
    wxString s;

    s.Printf(wxS("%d;%ld;%ld;%ld;%ld;%ld;%d;%d;%d;%d;%d;%d;%d;%d;%s"),
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
             (const wxChar*)lf.lfFaceName);

    return s;
}

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

wxFont::wxFont(const wxString& fontdesc)
{
    wxNativeFontInfo info;
    if ( info.FromString(fontdesc) )
        (void)Create(info);
}

bool wxFont::Create(const wxNativeFontInfo& info, WXHFONT hFont)
{
    UnRef();

    m_refData = new wxFontRefData(info, hFont);

    return RealizeResource();
}

bool wxFont::DoCreate(int pointSize,
                      const wxSize& pixelSize,
                      bool sizeUsingPixels,
                      wxFontFamily family,
                      wxFontStyle style,
                      wxFontWeight weight,
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

    m_refData = new wxFontRefData(pointSize, pixelSize, sizeUsingPixels,
                                  family, style, weight,
                                  underlined, faceName, encoding);

    return RealizeResource();
}

wxFont::~wxFont()
{
}

// ----------------------------------------------------------------------------
// real implementation
// ----------------------------------------------------------------------------

wxGDIRefData *wxFont::CreateGDIRefData() const
{
    return new wxFontRefData();
}

wxGDIRefData *wxFont::CloneGDIRefData(const wxGDIRefData *data) const
{
    return new wxFontRefData(*static_cast<const wxFontRefData *>(data));
}

bool wxFont::RealizeResource()
{
    // NOTE: the GetHFONT() call automatically triggers a reallocation of
    //       the HFONT if necessary (will do nothing if we already have the resource);
    //       it returns NULL only if there is a failure in wxFontRefData::Alloc()...
    return GetHFONT() != NULL;
}

bool wxFont::FreeResource(bool WXUNUSED(force))
{
    if ( !M_FONTDATA )
        return false;

    M_FONTDATA->Free();

    return true;
}

WXHANDLE wxFont::GetResourceHandle() const
{
    return (WXHANDLE)GetHFONT();
}

WXHFONT wxFont::GetHFONT() const
{
    // NOTE: wxFontRefData::GetHFONT() will automatically call
    //       wxFontRefData::Alloc() if necessary
    return M_FONTDATA ? M_FONTDATA->GetHFONT() : 0;
}

bool wxFont::IsFree() const
{
    return M_FONTDATA && !M_FONTDATA->HasHFONT();
}

// ----------------------------------------------------------------------------
// change font attribute: we recreate font when doing it
// ----------------------------------------------------------------------------

void wxFont::SetPointSize(int pointSize)
{
    AllocExclusive();

    M_FONTDATA->Free();
    M_FONTDATA->SetPointSize(pointSize);
}

void wxFont::SetPixelSize(const wxSize& pixelSize)
{
    AllocExclusive();

    M_FONTDATA->SetPixelSize(pixelSize);
}

void wxFont::SetFamily(wxFontFamily family)
{
    AllocExclusive();

    M_FONTDATA->SetFamily(family);
}

void wxFont::SetStyle(wxFontStyle style)
{
    AllocExclusive();

    M_FONTDATA->SetStyle(style);
}

void wxFont::SetWeight(wxFontWeight weight)
{
    AllocExclusive();

    M_FONTDATA->SetWeight(weight);
}

bool wxFont::SetFaceName(const wxString& faceName)
{
    AllocExclusive();

    if ( !M_FONTDATA->SetFaceName(faceName) )
        return false;

    // NB: using win32's GetObject() API on M_FONTDATA->GetHFONT()
    //     to retrieve a LOGFONT and then compare lf.lfFaceName
    //     with given facename is not reliable at all:
    //     Windows copies the facename given to ::CreateFontIndirect()
    //     without any validity check.
    //     Thus we use wxFontBase::SetFaceName to check if facename
    //     is valid...
    return wxFontBase::SetFaceName(faceName);
}

void wxFont::SetUnderlined(bool underlined)
{
    AllocExclusive();

    M_FONTDATA->SetUnderlined(underlined);
}

void wxFont::SetEncoding(wxFontEncoding encoding)
{
    AllocExclusive();

    M_FONTDATA->SetEncoding(encoding);
}

void wxFont::DoSetNativeFontInfo(const wxNativeFontInfo& info)
{
    AllocExclusive();

    M_FONTDATA->SetNativeFontInfo(info);
}

// ----------------------------------------------------------------------------
// accessors
// ----------------------------------------------------------------------------

int wxFont::GetPointSize() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid font") );

    return M_FONTDATA->GetPointSize();
}

wxSize wxFont::GetPixelSize() const
{
    wxCHECK_MSG( IsOk(), wxDefaultSize, wxT("invalid font") );

    return M_FONTDATA->GetPixelSize();
}

bool wxFont::IsUsingSizeInPixels() const
{
    wxCHECK_MSG( IsOk(), 0, wxT("invalid font") );

    return M_FONTDATA->IsUsingSizeInPixels();
}

wxFontFamily wxFont::GetFamily() const
{
    wxCHECK_MSG( IsOk(), wxFONTFAMILY_MAX, wxT("invalid font") );

    return M_FONTDATA->GetFamily();
}

wxFontStyle wxFont::GetStyle() const
{
    wxCHECK_MSG( IsOk(), wxFONTSTYLE_MAX, wxT("invalid font") );

    return M_FONTDATA->GetStyle();
}

wxFontWeight wxFont::GetWeight() const
{
    wxCHECK_MSG( IsOk(), wxFONTWEIGHT_MAX, wxT("invalid font") );

    return M_FONTDATA->GetWeight();
}

bool wxFont::GetUnderlined() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    return M_FONTDATA->GetUnderlined();
}

wxString wxFont::GetFaceName() const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("invalid font") );

    return M_FONTDATA->GetFaceName();
}

wxFontEncoding wxFont::GetEncoding() const
{
    wxCHECK_MSG( IsOk(), wxFONTENCODING_DEFAULT, wxT("invalid font") );

    return M_FONTDATA->GetEncoding();
}

const wxNativeFontInfo *wxFont::GetNativeFontInfo() const
{
    return IsOk() ? &(M_FONTDATA->GetNativeFontInfo()) : NULL;
}

wxString wxFont::GetNativeFontInfoDesc() const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("invalid font") );

    // be sure we have an HFONT associated...
    const_cast<wxFont*>(this)->RealizeResource();
    return wxFontBase::GetNativeFontInfoDesc();
}

wxString wxFont::GetNativeFontInfoUserDesc() const
{
    wxCHECK_MSG( IsOk(), wxEmptyString, wxT("invalid font") );

    // be sure we have an HFONT associated...
    const_cast<wxFont*>(this)->RealizeResource();
    return wxFontBase::GetNativeFontInfoUserDesc();
}

bool wxFont::IsFixedWidth() const
{
    wxCHECK_MSG( IsOk(), false, wxT("invalid font") );

    // the two low-order bits specify the pitch of the font, the rest is
    // family
    BYTE pitch =
        (BYTE)(M_FONTDATA->GetNativeFontInfo().lf.lfPitchAndFamily & PITCH_MASK);

    return pitch == FIXED_PITCH;
}
