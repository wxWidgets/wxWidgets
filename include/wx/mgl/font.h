/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_FONT_H__
#define __WX_FONT_H__

#ifdef __GNUG__
    #pragma interface "font.h"
#endif

#include "wx/hash.h"

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxDC;
class WXDLLEXPORT wxPaintDC;
class WXDLLEXPORT wxWindow;

class WXDLLEXPORT wxFont;

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { Init(); }
    wxFont(const wxFont& font) { Init(); Ref(font); }
    wxFont(const wxString& fontname,
           wxFontEncoding fontenc = wxFONTENCODING_DEFAULT)
    {
        Init();

        Create(fontname, fontenc);
    }

#if 0 // FIXME_MGL -- not in wxUniversal branch
    wxFont(const wxNativeFontInfo& info);
#endif

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
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // FIXME_MGL - may disappear in wxMGL
    // wxGTK-specific
    bool Create(const wxString& fontname,
                wxFontEncoding fontenc = wxFONTENCODING_DEFAULT);
#if 0 // FIXME_MGL -- not in wxUniversal branch
    bool Create(const wxNativeFontInfo& fontinfo);
#endif

    ~wxFont();

    // assignment
    wxFont& operator=(const wxFont& font);

    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual int GetFamily() const;
    virtual int GetStyle() const;
    virtual int GetWeight() const;
    virtual wxString GetFaceName() const;
    virtual bool GetUnderlined() const;
    virtual wxFontEncoding GetEncoding() const;

#if 0 // FIXME_MGL -- not in wxUniversal branch
    virtual wxNativeFontInfo *GetNativeFontInfo() const;
#endif

    virtual void SetPointSize( int pointSize );
    virtual void SetFamily( int family );
    virtual void SetStyle( int style );
    virtual void SetWeight( int weight );
    virtual void SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);

#if 0 // FIXME_MGL -- not in wxUniversal branch
    virtual void SetNativeFontInfo( const wxNativeFontInfo& info );
#endif

protected:
    void Unshare();

    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif // __WX_FONT_H__
