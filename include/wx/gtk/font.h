/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_FONT_H_
#define _WX_GTK_FONT_H_

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxFont : public wxFontBase
{
public:
    wxFont() { }

    // wxGTK-specific
    wxFont(const wxString& fontname)
    {
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

    virtual ~wxFont();

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
    virtual bool SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);

    virtual void SetNoAntiAliasing( bool no = true );
    virtual bool GetNoAntiAliasing() const ;

    // implementation from now on
    void Unshare();

    // no data :-)

protected:
    virtual void DoSetNativeFontInfo( const wxNativeFontInfo& info );

    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#endif // _WX_GTK_FONT_H_
