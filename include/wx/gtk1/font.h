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

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/hash.h"
#include "wx/gdiobj.h"

// ----------------------------------------------------------------------------
// conditional compilation
// ----------------------------------------------------------------------------

#define wxUSE_FONTNAMEDIRECTORY 0

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

class wxDC;
class wxPaintDC;
class wxWindow;

class wxFont;

#if wxUSE_FONTNAMEDIRECTORY
    class wxFontNameDirectory;
#endif

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

extern const wxChar* wxEmptyString;

#if wxUSE_FONTNAMEDIRECTORY
    extern wxFontNameDirectory *wxTheFontNameDirectory;
#endif

// ----------------------------------------------------------------------------
// wxFont
// ----------------------------------------------------------------------------

class wxFont : public wxFontBase
{
public:
    // ctors and such
    wxFont() { Init(); }
    wxFont(const wxFont& font) { Init(); Ref(font); }

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
                wxFontEncoding encoding = wxFONTENCODING_DEFAULT);

    ~wxFont();

    // implement base class pure virtuals
    virtual int GetPointSize() const;
    virtual int GetFamily() const;
    virtual int GetStyle() const;
    virtual int GetWeight() const;
    virtual wxString GetFaceName() const;
    virtual bool GetUnderlined() const;
    virtual wxFontEncoding GetEncoding() const;

    virtual void SetPointSize( int pointSize );
    virtual void SetFamily( int family );
    virtual void SetStyle( int style );
    virtual void SetWeight( int weight );
    virtual void SetFaceName( const wxString& faceName );
    virtual void SetUnderlined( bool underlined );
    virtual void SetEncoding(wxFontEncoding encoding);

    // implementation from now on
    wxFont( GdkFont* font, char *xFontName );
    void Unshare();

    GdkFont* GetInternalFont(float scale = 1.0) const;

    // no data :-)

protected:
    // common part of all ctors
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxFont)
};

#if wxUSE_FONTNAMEDIRECTORY

// ----------------------------------------------------------------------------
// wxFontDirectory
// ----------------------------------------------------------------------------

class wxFontNameDirectory: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxFontNameDirectory)

  public:
    wxFontNameDirectory();
    ~wxFontNameDirectory();

    void  Initialize();
    void  Initialize(int fontid, int family, const char *name);

    int   FindOrCreateFontId(const char *name, int family);
    char* GetAFMName(int fontid, int weight, int style);
    int   GetFamily(int fontid);
    int   GetFontId(const char *name);
    char* GetFontName(int fontid);
    int   GetNewFontId();
    char* GetPostScriptName(int fontid, int weight, int style);
    char* GetScreenName(int fontid, int weight, int style);

    class wxHashTable *table;
    int   nextFontId;
};

#endif // wxUSE_FONTNAMEDIRECTORY

#endif // __GTKFONTH__
