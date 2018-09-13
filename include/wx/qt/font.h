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
    virtual float GetFractionalPointSize() const wxOVERRIDE;
    virtual wxFontStyle GetStyle() const;
    virtual int GetNumericWeight() const wxOVERRIDE;
    virtual bool GetUnderlined() const;
    virtual wxString GetFaceName() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;

    // change the font characteristics
    virtual void SetFractionalPointSize(float pointSize) wxOVERRIDE;
    virtual void SetFamily( wxFontFamily family );
    virtual void SetStyle( wxFontStyle style );
    virtual void SetNumericWeight(int weight) wxOVERRIDE;
    virtual bool SetFaceName(const wxString& facename);
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);
    
    wxDECLARE_COMMON_FONT_METHODS();

    virtual QFont GetHandle() const;
    
protected:
    virtual wxGDIRefData *CreateGDIRefData() const;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const;
    virtual wxFontFamily DoGetFamily() const;

    wxDECLARE_DYNAMIC_CLASS(wxFont);

};


#endif // _WX_QT_FONT_H_
