/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKFONTH__
#define __GTKFONTH__

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma interface
#endif

#include "wx/hash.h"

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxDC;
class WXDLLIMPEXP_CORE wxPaintDC;
class WXDLLIMPEXP_CORE wxWindow;

class WXDLLIMPEXP_CORE wxFont;

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { Init(); }
    wxFont(const wxFont& font) : wxFontBase() { Init(); Ref(font); }

    // wxGTK-specific
    wxFont(const wxString& fontname)
    {
        Init();

        Create(fontname);
    }

    wxFont(const wxNativeFontInfo& info);

    wxFont(int size,
           int family,
           int style,
           int weight,
           bool underlined = false,
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
                bool underlined = false,
                const wxString& face = wxEmptyString,
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    // wxGTK-specific
    bool Create(const wxString& fontname);

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
    virtual const wxNativeFontInfo *GetNativeFontInfo() const;
    virtual bool IsFixedWidth() const;

    virtual void SetPointSize( int pointSize );
    virtual void SetFamily( int family );
    virtual void SetStyle( int style );
    virtual void SetWeight( int weight );
    virtual void SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);

    virtual void SetNoAntiAliasing( bool no = true );
    virtual bool GetNoAntiAliasing() const ;

    // implementation from now on
    void Unshare();

#ifndef __WXGTK20__
    GdkFont* GetInternalFont(float scale = 1.0) const;
#endif

    // no data :-)

protected:
    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info );

    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif // __GTKFONTH__
