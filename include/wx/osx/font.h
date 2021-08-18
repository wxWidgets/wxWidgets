/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

// font styles
enum wxOSXSystemFont
{
    wxOSX_SYSTEM_FONT_NONE = 0,
    wxOSX_SYSTEM_FONT_NORMAL,
    wxOSX_SYSTEM_FONT_BOLD,
    wxOSX_SYSTEM_FONT_SMALL,
    wxOSX_SYSTEM_FONT_SMALL_BOLD,
    wxOSX_SYSTEM_FONT_MINI,
    wxOSX_SYSTEM_FONT_MINI_BOLD,
    wxOSX_SYSTEM_FONT_LABELS,
    wxOSX_SYSTEM_FONT_VIEWS,
    wxOSX_SYSTEM_FONT_FIXED
};


class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { }

    wxFont(const wxFontInfo& info);

    wxFont( wxOSXSystemFont systemFont );
    wxFont(CTFontRef font);

#if wxOSX_USE_COCOA
    wxFont(WX_NSFont nsfont);
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

    wxFont(const wxNativeFontInfo& info)
    {
        (void)Create(info);
    }

    wxFont(const wxString& fontDesc);

    bool Create(const wxNativeFontInfo& info);

    virtual ~wxFont();

    // implement base class pure virtuals
    virtual double GetFractionalPointSize() const wxOVERRIDE;
    virtual wxSize GetPixelSize() const wxOVERRIDE;
    virtual wxFontStyle GetStyle() const wxOVERRIDE;
    virtual int GetNumericWeight() const wxOVERRIDE;
    virtual bool GetUnderlined() const wxOVERRIDE;
    virtual bool GetStrikethrough() const wxOVERRIDE;
    virtual wxString GetFaceName() const wxOVERRIDE;
    virtual wxFontEncoding GetEncoding() const wxOVERRIDE;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const wxOVERRIDE;

    virtual bool IsFixedWidth() const wxOVERRIDE;

    virtual void SetFractionalPointSize(double pointSize) wxOVERRIDE;
    virtual void SetFamily(wxFontFamily family) wxOVERRIDE;
    virtual void SetStyle(wxFontStyle style) wxOVERRIDE;
    virtual void SetNumericWeight(int weight) wxOVERRIDE;
    virtual bool SetFaceName(const wxString& faceName) wxOVERRIDE;
    virtual void SetUnderlined(bool underlined) wxOVERRIDE;
    virtual void SetStrikethrough(bool strikethrough) wxOVERRIDE;
    virtual void SetEncoding(wxFontEncoding encoding) wxOVERRIDE;

    wxDECLARE_COMMON_FONT_METHODS();

    wxDEPRECATED_MSG("use wxFONT{FAMILY,STYLE,WEIGHT}_XXX constants")
    wxFont(int size,
           int family,
           int style,
           int weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT);


    // implementation only from now on
    // -------------------------------

    virtual bool RealizeResource();

    // Mac-specific, risks to change, don't use in portable code

#if wxOSX_USE_COCOA_OR_CARBON
    CGFontRef OSXGetCGFont() const;
#endif

    CTFontRef OSXGetCTFont() const;
    CFDictionaryRef OSXGetCTFontAttributes() const;

#if wxOSX_USE_COCOA
    WX_NSFont OSXGetNSFont() const;
#endif

#if wxOSX_USE_IPHONE
    WX_UIFont OSXGetUIFont() const;
#endif

protected:
    virtual void DoSetNativeFontInfo(const wxNativeFontInfo& info) wxOVERRIDE;
    virtual wxFontFamily DoGetFamily() const wxOVERRIDE;

    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;

private:

    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif // _WX_FONT_H_
