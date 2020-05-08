/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/font.h
// Purpose:     wxFont class
// Author:      Julian Smart
// Modified by:
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
    wxFont() { }

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

    wxFont(const wxNativeFontInfo& info, WXHFONT hFont = NULL)
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

    bool Create(const wxNativeFontInfo& info, WXHFONT hFont = NULL);

    virtual ~wxFont();

    // implement base class pure virtuals
    virtual double GetFractionalPointSize() const wxOVERRIDE;
    virtual wxSize GetPixelSize() const wxOVERRIDE;
    virtual bool IsUsingSizeInPixels() const wxOVERRIDE;
    virtual wxFontStyle GetStyle() const wxOVERRIDE;
    virtual int GetNumericWeight() const wxOVERRIDE;
    virtual bool GetUnderlined() const wxOVERRIDE;
    virtual bool GetStrikethrough() const wxOVERRIDE;
    virtual wxString GetFaceName() const wxOVERRIDE;
    virtual wxFontEncoding GetEncoding() const wxOVERRIDE;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const wxOVERRIDE;

    virtual void SetFractionalPointSize(double pointSize) wxOVERRIDE;
    virtual void SetPixelSize(const wxSize& pixelSize) wxOVERRIDE;
    virtual void SetFamily(wxFontFamily family) wxOVERRIDE;
    virtual void SetStyle(wxFontStyle style) wxOVERRIDE;
    virtual void SetNumericWeight(int weight) wxOVERRIDE;
    virtual bool SetFaceName(const wxString& faceName) wxOVERRIDE;
    virtual void SetUnderlined(bool underlined) wxOVERRIDE;
    virtual void SetStrikethrough(bool strikethrough) wxOVERRIDE;
    virtual void SetEncoding(wxFontEncoding encoding) wxOVERRIDE;

    wxDECLARE_COMMON_FONT_METHODS();

    virtual bool IsFixedWidth() const wxOVERRIDE;

    virtual void WXAdjustToPPI(const wxSize& ppi) wxOVERRIDE;

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

    virtual bool IsFree() const wxOVERRIDE;
    virtual bool RealizeResource() wxOVERRIDE;
    virtual WXHANDLE GetResourceHandle() const wxOVERRIDE;
    virtual bool FreeResource(bool force = false) wxOVERRIDE;

    // for consistency with other wxMSW classes
    WXHFONT GetHFONT() const;

protected:
    // Common helper of overloaded Create() methods.
    bool DoCreate(const wxFontInfo& info);

    virtual void DoSetNativeFontInfo(const wxNativeFontInfo& info) wxOVERRIDE;
    virtual wxFontFamily DoGetFamily() const wxOVERRIDE;

    // implement wxObject virtuals which are used by AllocExclusive()
    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;

private:
    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif // _WX_FONT_H_
