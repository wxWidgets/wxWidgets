/////////////////////////////////////////////////////////////////////////////
// Name:        wx/retro68/font.h
// Purpose:     wxFont declaration for classic Mac OS
// Created:     2026-11-12
// Copyright:   (c) 2026 wxWidgets development team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_RETRO68_FONT_H_
#define _WX_RETRO68_FONT_H_

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    wxFont() = default;

    wxFont(const wxFontInfo& info)
    {
        Create(info.GetPointSize(),
               info.GetFamily(),
               info.GetStyle(),
               info.GetWeight(),
               info.IsUnderlined(),
               info.GetFaceName(),
               info.GetEncoding());

        if ( info.IsUsingSizeInPixels() )
            SetPixelSize(info.GetPixelSize());
    }

    wxFont(const wxNativeFontInfo& info) { Create(info); }

    wxFont(const wxString& nativeFontInfoString);

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

    wxFont(const wxSize& pixelSize,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        Create(10, family, style, weight, underlined, face, encoding);
        SetPixelSize(pixelSize);
    }

    bool Create(int size,
                wxFontFamily family,
                wxFontStyle style,
                wxFontWeight weight,
                bool underlined = false,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    bool Create(const wxNativeFontInfo& fontinfo);

    // implement base class pure virtuals
    virtual double GetFractionalPointSize() const override;
    virtual wxFontStyle GetStyle() const override;
    virtual int GetNumericWeight() const override;
    virtual wxString GetFaceName() const override;
    virtual bool GetUnderlined() const override;
    virtual wxFontEncoding GetEncoding() const override;
    virtual bool IsFixedWidth() const override;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const override;

    virtual void SetFractionalPointSize(double pointSize) override;
    virtual void SetFamily(wxFontFamily family) override;
    virtual void SetStyle(wxFontStyle style) override;
    virtual void SetNumericWeight(int weight) override;
    virtual bool SetFaceName(const wxString& faceName) override;
    virtual void SetUnderlined(bool underlined) override;
    virtual void SetEncoding(wxFontEncoding encoding) override;

    wxDECLARE_COMMON_FONT_METHODS();

    wxDEPRECATED_MSG("use wxFONT{FAMILY,STYLE,WEIGHT}_XXX constants")
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

    // implementation from now on:

    // Return the Font Manager font family ID, size in points and style bits
    // (combination of bold, italic, underline QuickDraw constants).
    short GetMacFontID() const;
    short GetMacFontSize() const;
    short GetMacFontStyle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    wxNODISCARD virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

    virtual wxFontFamily DoGetFamily() const override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif // _WX_RETRO68_FONT_H_
