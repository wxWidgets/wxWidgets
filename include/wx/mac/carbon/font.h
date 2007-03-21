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

class WXDLLEXPORT wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { }

    wxFont(int size,
           int family,
           int style,
           int weight,
           bool underlined = FALSE,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        (void)Create(size, family, style, weight, underlined, face, encoding);
    }

    wxFont(const wxNativeFontInfo& info)
    {
        (void)Create(info);
    }

    wxFont(const wxString& fontDesc);

    bool Create(int size,
                int family,
                int style,
                int weight,
                bool underlined = FALSE,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    bool Create(const wxNativeFontInfo& info);

    bool MacCreateThemeFont( wxUint16 themeFontID ) ;
    
    virtual ~wxFont();

    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual wxSize GetPixelSize() const;
    virtual int GetFamily() const;
    virtual int GetStyle() const;
    virtual int GetWeight() const;
    virtual bool GetUnderlined() const;
    virtual wxString GetFaceName() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;

    virtual void SetPointSize(int pointSize);
    virtual void SetFamily(int family);
    virtual void SetStyle(int style);
    virtual void SetWeight(int weight);
    virtual bool SetFaceName(const wxString& faceName);
    virtual void SetUnderlined(bool underlined);
    virtual void SetEncoding(wxFontEncoding encoding);

    // implementation only from now on
    // -------------------------------

    virtual bool RealizeResource();

    // Unofficial API, don't use
    virtual void SetNoAntiAliasing( bool noAA = TRUE ) ;
    virtual bool GetNoAntiAliasing() const  ;

    // Mac-specific, risks to change, don't use in portable code
    
    // 'old' Quickdraw accessors
    
    short MacGetFontNum() const;
    short MacGetFontSize() const;
    wxByte  MacGetFontStyle() const;
    
    // 'new' ATSUI accessors
    
    wxUint32 MacGetATSUFontID() const;
    wxUint32 MacGetATSUAdditionalQDStyles() const;
    wxUint16 MacGetThemeFontID() const ;
    // Returns an ATSUStyle not ATSUStyle*
    void* MacGetATSUStyle() const ; 
    
private:
    void Unshare();

    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif
    // _WX_FONT_H_
