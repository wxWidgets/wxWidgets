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

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDC;
class wxPaintDC;
class wxWindow;

class wxFont;
class wxFontNameDirectory;

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

//extern wxFontNameDirectory *wxTheFontNameDirectory;  // defined below

//-----------------------------------------------------------------------------
// wxFont
//-----------------------------------------------------------------------------

class wxFont: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxFont)

  public:
    wxFont();
    wxFont( int PointSize, int FontIdOrFamily, int Style, int Weight,
            bool underlined = FALSE, const char *Face = ( const char *) NULL );
    wxFont( int PointSize, const char *Face, int Family,
            int Style, int Weight, bool underlined = FALSE );
    wxFont( const wxFont& font );
    ~wxFont();
    wxFont& operator = ( const wxFont& font );
    bool operator == ( const wxFont& font );
    bool operator != ( const wxFont& font );
    bool Ok() const;

    int GetPointSize() const;
    wxString GetFaceName() const;
    int GetFamily() const;
    wxString GetFamilyString() const;
    int GetFontId() const;
    wxString GetFaceString() const;
    int GetStyle() const;
    wxString GetStyleString() const;
    int GetWeight() const;
    wxString GetWeightString() const;
    bool GetUnderlined() const;

    wxFont( char *xFontName );

  // implementation

    GdkFont* GetInternalFont(float scale = 1.0) const;

    // no data :-)
};

//-----------------------------------------------------------------------------
// wxFontDirectory
//-----------------------------------------------------------------------------

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

extern wxFontNameDirectory *wxTheFontNameDirectory;

#endif // __GTKFONTH__
