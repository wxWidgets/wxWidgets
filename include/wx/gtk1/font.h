/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk1/font.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKFONTH__
#define __GTKFONTH__

#include "wx/hash.h"

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxPaintDC;
class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_FWD_CORE wxFont;

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { }

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

    wxFont(const wxString& fontname)
    {
        Create(fontname);
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
    virtual float GetFractionalPointSize() const;
    virtual wxFontStyle GetStyle() const;
    virtual int GetNumericWeight() const;
    virtual wxString GetFaceName() const;
    virtual bool GetUnderlined() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;
    virtual bool IsFixedWidth() const;

    virtual void SetFractionalPointSize(float pointSize);
    virtual void SetFamily(wxFontFamily family);
    virtual void SetStyle(wxFontStyle style);
    virtual void SetNumericWeight(int weight);
    virtual bool SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
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

    // implementation from now on
    void Unshare();

    GdkFont* GetInternalFont(float scale = 1.0) const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;

    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info );
    virtual wxFontFamily DoGetFamily() const;

private:
    wxDECLARE_DYNAMIC_CLASS(wxFont);
};

#endif // __GTKFONTH__
