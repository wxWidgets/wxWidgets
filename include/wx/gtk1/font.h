/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKFONTH__
#define __GTKFONTH__

#ifdef __GNUG__
    #pragma interface
#endif

#include "wx/hash.h"

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class wxDC;
class wxPaintDC;
class wxWindow;

class wxFont;

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { Init(); }
    wxFont(const wxFont& font) { Init(); Ref(font); }
    wxFont(const wxString& fontname, const wxFontData& fontdata) { Create(fontname, fontdata); }
    wxFont(const wxNativeFontInfo& info);

    // assignment
    wxFont& operator=(const wxFont& font);

    wxFont(int size,
           int family,
           int style,
           int weight,
           bool underlined = FALSE,
           const wxString& face = wxEmptyString,
           wxFontEncoding encoding = wxFONTENCODING_DEFAULT)
    {
        Init();

        (void)Create(size, family, style, weight, underlined, face, encoding);
    }

    bool Create(int size,
                int family,
                int style,
                int weight,
                bool underlined = FALSE,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT,
                const wxNativeFontInfo& info = wxNullNativeFontInfo);

    bool Create(const wxString& fontname, const wxFontData& fontdata);

    ~wxFont();

    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual int GetFamily() const;
    virtual int GetStyle() const;
    virtual int GetWeight() const;
    virtual wxString GetFaceName() const;
    virtual bool GetUnderlined() const;
    virtual wxFontEncoding GetEncoding() const;
    virtual wxNativeFontInfo GetNativeFontInfo() const;

    virtual void SetPointSize( int pointSize );
    virtual void SetFamily( int family );
    virtual void SetStyle( int style );
    virtual void SetWeight( int weight );
    virtual void SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);
    virtual void SetNativeFontInfo( const wxNativeFontInfo& info );

    // implementation from now on
    void Unshare();

    GdkFont* GetInternalFont(float scale = 1.0) const;

    // no data :-)

protected:
    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif // __GTKFONTH__
