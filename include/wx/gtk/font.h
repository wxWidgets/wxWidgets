/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/font.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_FONT_H_
#define _WX_GTK_FONT_H_

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    wxFont() = default;

    wxFont(const wxFontInfo& info);

    wxFont(const wxString& nativeFontInfoString)
    {
        Create(nativeFontInfoString);
    }

    wxFont(const wxNativeFontInfo& info);

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

    // wxGTK-specific
    bool Create(const wxString& fontname);

    virtual ~wxFont();

    // implement base class pure virtuals
    virtual double GetFractionalPointSize() const override;
    virtual wxFontStyle GetStyle() const override;
    virtual int GetNumericWeight() const override;
    virtual wxString GetFaceName() const override;
    virtual bool GetUnderlined() const override;
    virtual bool GetStrikethrough() const override;
    virtual wxFontEncoding GetEncoding() const override;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const override;
    virtual bool IsFixedWidth() const override;

    virtual void SetFractionalPointSize(double pointSize) override;
    virtual void SetFamily(wxFontFamily family) override;
    virtual void SetStyle(wxFontStyle style) override;
    virtual void SetNumericWeight(int weight) override;
    virtual bool SetFaceName( const wxString& faceName ) override;
    virtual void SetUnderlined( bool underlined ) override;
    virtual void SetStrikethrough(bool strikethrough) override;
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

    // Set Pango attributes in the specified layout. Currently only
    // underlined and strike-through attributes are handled by this function.
    //
    // If neither of them is specified, returns false, otherwise sets up the
    // attributes and returns true.
    bool GTKSetPangoAttrs(PangoLayout* layout) const;

protected:
    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info ) override;

    virtual wxGDIRefData* CreateGDIRefData() const override;
    virtual wxGDIRefData* CloneGDIRefData(const wxGDIRefData* data) const override;

    virtual wxFontFamily DoGetFamily() const override;

private:
    void Init();

    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif // _WX_GTK_FONT_H_
