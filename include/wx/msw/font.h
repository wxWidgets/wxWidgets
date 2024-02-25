/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_FONT_H_
#define _WX_FONT_H_

#include "wx/gdicmn.h"

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() = default;

    wxFont(const wxFontInfo& info);

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
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        return DoCreate(InfoFromLegacyParams(size,
                                             family,
                                             style,
                                             weight,
                                             underlined,
                                             face,
                                             encoding));
    }

    wxFont(const wxSize& pixelSize,
           wxFontFamily family,
           wxFontStyle style,
           wxFontWeight weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        (void)Create(pixelSize, family, style, weight,
                     underlined, face, encoding);
    }

    wxFont(const wxNativeFontInfo& info, WXHFONT hFont = nullptr)
    {
        Create(info, hFont);
    }

    wxFont(const wxString& fontDesc);


    bool Create(const wxSize& pixelSize,
                wxFontFamily family,
                wxFontStyle style,
                wxFontWeight weight,
                bool underlined = false,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        return DoCreate(InfoFromLegacyParams(pixelSize,
                                             family,
                                             style,
                                             weight,
                                             underlined,
                                             face,
                                             encoding));
    }

    bool Create(const wxNativeFontInfo& info, WXHFONT hFont = nullptr);

    // implement base class pure virtuals
    virtual double GetFractionalPointSize() const override;
    virtual wxSize GetPixelSize() const override;
    virtual bool IsUsingSizeInPixels() const override;
    virtual wxFontStyle GetStyle() const override;
    virtual int GetNumericWeight() const override;
    virtual bool GetUnderlined() const override;
    virtual bool GetStrikethrough() const override;
    virtual wxString GetFaceName() const override;
    virtual wxFontEncoding GetEncoding() const override;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const override;

    virtual void SetFractionalPointSize(double pointSize) override;
    virtual void SetPixelSize(const wxSize& pixelSize) override;
    virtual void SetFamily(wxFontFamily family) override;
    virtual void SetStyle(wxFontStyle style) override;
    virtual void SetNumericWeight(int weight) override;
    virtual bool SetFaceName(const wxString& faceName) override;
    virtual void SetUnderlined(bool underlined) override;
    virtual void SetStrikethrough(bool strikethrough) override;
    virtual void SetEncoding(wxFontEncoding encoding) override;

    wxDECLARE_COMMON_FONT_METHODS();

    virtual bool IsFixedWidth() const override;

    // MSW needs to modify the font object when the DPI of the window it
    // is used with changes, this function can be used to do it.
    //
    // This method is not considered to be part of wxWidgets public API.
    void WXAdjustToPPI(const wxSize& ppi);

    wxDEPRECATED_MSG("use wxFONT{FAMILY,STYLE,WEIGHT}_XXX constants ie: wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD")
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

    wxDEPRECATED_MSG("use wxFONT{FAMILY,STYLE,WEIGHT}_XXX constants ie: wxFONTFAMILY_SWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD")
    wxFont(const wxSize& pixelSize,
           int family,
           int style,
           int weight,
           bool underlined = false,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        (void)Create(pixelSize, (wxFontFamily)family, (wxFontStyle)style, (wxFontWeight)weight,
                     underlined, face, encoding);
    }

    // implementation only from now on
    // -------------------------------

    virtual bool IsFree() const override;
    virtual bool RealizeResource() override;
    virtual WXHANDLE GetResourceHandle() const override;
    virtual bool FreeResource(bool force = false) override;

    // for consistency with other wxMSW classes
    WXHFONT GetHFONT() const;

protected:
    // Common helper of overloaded Create() methods.
    bool DoCreate(const wxFontInfo& info);

    virtual void DoSetNativeFontInfo(const wxNativeFontInfo& info) override;
    virtual wxFontFamily DoGetFamily() const override;

    // implement wxObject virtuals which are used by AllocExclusive()
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

private:
    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif // _WX_FONT_H_
