/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { }

#if FUTURE_WXWIN_COMPATIBILITY_3_0
    wxFont(int size,
           int family,
           int style,
           int weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        (void)Create(size, (wxFontFamily)family, (wxFontStyle)style, (wxFontWeight)weight, underlined, face, encoding);
    }
#endif

    wxFont(int size,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        Create(size, family, style, weight, underlined, face, encoding);
    }

    bool Create(int size,
                wxFontFamily family,
                wxFontStyle style,
                wxFontWeight weight,
                bool underlined = false,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    wxFont(const wxNativeFontInfo& info)
    {
        (void)Create(info);
    }

    wxFont(const wxString& fontDesc);

    bool Create(const wxNativeFontInfo& info);

#if wxOSX_USE_ATSU_TEXT
    bool MacCreateFromThemeFont( wxUint16 themeFontID ) ;
#endif
#if wxOSX_USE_CORE_TEXT
    bool MacCreateFromUIFont( wxUint32 coreTextFontType );
    bool MacCreateFromCTFontDescriptor( const void * ctFontDescriptor, int pointSize = 0 );
    bool MacCreateFromCTFont( const void * ctFont );
#endif

    virtual ~wxFont();

    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual wxSize GetPixelSize() const;
    virtual wxFontFamily GetFamily() const;
    virtual wxFontStyle GetStyle() const;
    virtual wxFontWeight GetWeight() const;
    virtual bool GetUnderlined() const;
    virtual wxString GetFaceName() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;

    virtual void SetPointSize(int pointSize);
    virtual void SetFamily(wxFontFamily family);
    virtual void SetStyle(wxFontStyle style);
    virtual void SetWeight(wxFontWeight weight);
    virtual bool SetFaceName(const wxString& faceName);
    virtual void SetUnderlined(bool underlined);
    virtual void SetEncoding(wxFontEncoding encoding);

    WXDECLARE_COMPAT_SETTERS

    // implementation only from now on
    // -------------------------------

    virtual bool RealizeResource();

    // Unofficial API, don't use
    virtual void SetNoAntiAliasing( bool noAA = TRUE ) ;
    virtual bool GetNoAntiAliasing() const  ;

    // Mac-specific, risks to change, don't use in portable code

#if wxOSX_USE_ATSU_TEXT
    // 'old' Quickdraw accessors
    short MacGetFontNum() const;
    short MacGetFontSize() const;
    wxByte  MacGetFontStyle() const;

    // 'new' ATSUI accessors
    wxUint32 MacGetATSUFontID() const;
    wxUint32 MacGetATSUAdditionalQDStyles() const;
    wxUint16 MacGetThemeFontID() const ;

    // Returns an ATSUStyle not ATSUStyle*
#endif
#if wxOSX_USE_CORE_TEXT
    const void * MacGetCTFont() const;
#endif
#if wxOSX_USE_CORE_TEXT || wxOSX_USE_ATSU_TEXT
    void* MacGetATSUStyle() const ;
#endif

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

private:
    void Unshare();

    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif // _WX_FONT_H_
