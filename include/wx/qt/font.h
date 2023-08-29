/////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/font.h
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2010 wxWidgets dev team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_FONT_H_
#define _WX_QT_FONT_H_

class QFont;
class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    wxFont();
    wxFont(const wxFontInfo& info);
    wxFont(const wxString& nativeFontInfoString);
    wxFont(const wxNativeFontInfo& info);
    wxFont(const QFont& font);
    wxFont(int size,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT);
    wxFont(const wxSize& pixelSize,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    wxDEPRECATED_MSG("use wxFONT{FAMILY,STYLE,WEIGHT}_XXX constants")
    wxFont(int size,
           int family,
           int style,
           int weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    bool Create(wxSize size,
                wxFontFamily family,
                wxFontStyle style,
                wxFontWeight weight,
                bool underlined = false,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // accessors: get the font characteristics
    virtual int GetPointSize() const override;
    virtual double GetFractionalPointSize() const override;
    virtual wxSize GetPixelSize() const override;
    virtual wxFontStyle GetStyle() const override;
    virtual int GetNumericWeight() const override;
    virtual bool GetUnderlined() const override;
    virtual wxString GetFaceName() const override;
    virtual wxFontEncoding GetEncoding() const override;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const override;
    virtual bool GetStrikethrough() const override;

    // change the font characteristics
    virtual void SetFractionalPointSize(double pointSize) override;
    virtual void SetPixelSize(const wxSize& pixelSize) override;
    virtual void SetFamily( wxFontFamily family ) override;
    virtual void SetStyle( wxFontStyle style ) override;
    virtual void SetNumericWeight(int weight) override;
    virtual bool SetFaceName(const wxString& facename) override;
    virtual void SetUnderlined( bool underlined ) override;
    virtual void SetStrikethrough(bool strikethrough) override;
    virtual void SetEncoding(wxFontEncoding encoding) override;

    wxDECLARE_COMMON_FONT_METHODS();

    virtual QFont GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;
    virtual wxFontFamily DoGetFamily() const override;
    virtual void DoSetNativeFontInfo(const wxNativeFontInfo& info) override;

    wxDECLARE_DYNAMIC_CLASS(wxFont);

};


#endif // _WX_QT_FONT_H_
