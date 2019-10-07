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
    virtual int GetPointSize() const wxOVERRIDE;
    virtual float GetFractionalPointSize() const wxOVERRIDE;
    virtual wxSize GetPixelSize() const wxOVERRIDE;
    virtual wxFontStyle GetStyle() const wxOVERRIDE;
    virtual int GetNumericWeight() const wxOVERRIDE;
    virtual bool GetUnderlined() const wxOVERRIDE;
    virtual wxString GetFaceName() const wxOVERRIDE;
    virtual wxFontEncoding GetEncoding() const wxOVERRIDE;
    virtual const wxNativeFontInfo *GetNativeFontInfo() const wxOVERRIDE;
    virtual bool GetStrikethrough() const wxOVERRIDE;

    // change the font characteristics
    virtual void SetFractionalPointSize(float pointSize) wxOVERRIDE;
    virtual void SetPixelSize(const wxSize& pixelSize) wxOVERRIDE;
    virtual void SetFamily( wxFontFamily family ) wxOVERRIDE;
    virtual void SetStyle( wxFontStyle style ) wxOVERRIDE;
    virtual void SetNumericWeight(int weight) wxOVERRIDE;
    virtual bool SetFaceName(const wxString& facename) wxOVERRIDE;
    virtual void SetUnderlined( bool underlined ) wxOVERRIDE;
    virtual void SetStrikethrough(bool strikethrough) wxOVERRIDE;
    virtual void SetEncoding(wxFontEncoding encoding) wxOVERRIDE;

    wxDECLARE_COMMON_FONT_METHODS();

    virtual QFont GetHandle() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;
    virtual wxFontFamily DoGetFamily() const wxOVERRIDE;
    virtual void DoSetNativeFontInfo(const wxNativeFontInfo& info) wxOVERRIDE;

    wxDECLARE_DYNAMIC_CLASS(wxFont);

};


#endif // _WX_QT_FONT_H_
