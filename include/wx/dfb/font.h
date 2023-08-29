/////////////////////////////////////////////////////////////////////////////
// Name:        wx/dfb/font.h
// Author:      Vaclav Slavik
// Purpose:     wxFont declaration
// Created:     2006-08-08
// Copyright:   (c) 2006 REA Elektronik GmbH
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DFB_FONT_H_
#define _WX_DFB_FONT_H_

#include "wx/dfb/dfbptr.h"

wxDFB_DECLARE_INTERFACE(IDirectFBFont);

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    wxFont() {}

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
    virtual double GetFractionalPointSize() const;
    virtual wxFontStyle GetStyle() const;
    virtual int GetNumericWeight() const;
    virtual wxString GetFaceName() const;
    virtual bool GetUnderlined() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual bool IsFixedWidth() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;

    virtual void SetFractionalPointSize(double pointSize);
    virtual void SetFamily(wxFontFamily family);
    virtual void SetStyle(wxFontStyle style);
    virtual void SetNumericWeight(int weight);
    virtual bool SetFaceName(const wxString& faceName);
    virtual void SetUnderlined(bool underlined);
    virtual void SetEncoding(wxFontEncoding encoding);

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
    wxIDirectFBFontPtr GetDirectFBFont(bool antialiased) const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    virtual wxFontFamily DoGetFamily() const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif // _WX_DFB_FONT_H_
