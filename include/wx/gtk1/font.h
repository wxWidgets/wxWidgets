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
/*
class wxFontNameDirectory;
*/

//-----------------------------------------------------------------------------
// global variables
//-----------------------------------------------------------------------------

/*
extern wxFontNameDirectory *wxTheFontNameDirectory;
*/
extern const char* wxEmptyString;

//-----------------------------------------------------------------------------
// wxFont
//-----------------------------------------------------------------------------

class wxFont: public wxGDIObject
{
  DECLARE_DYNAMIC_CLASS(wxFont)

public:
  wxFont();
  wxFont( int pointSize, int family, int style, int weight, bool underlined = FALSE, 
          const wxString& face = wxEmptyString );
  wxFont( const wxFont& font );
  ~wxFont();
  wxFont& operator = ( const wxFont& font );
  bool operator == ( const wxFont& font );
  bool operator != ( const wxFont& font );
  bool Ok() const;

  int GetPointSize() const;
  int GetFamily() const;
  int GetStyle() const;
  int GetWeight() const;
  bool GetUnderlined() const;

  void SetPointSize( int pointSize );
  void SetFamily( int family );
  void SetStyle( int style );
  void SetWeight( int weight );
  void SetFaceName( const wxString& faceName );
  void SetUnderlined( bool underlined );
    
  wxString GetFaceName() const;
  wxString GetFamilyString() const;
  wxString GetStyleString() const;
  wxString GetWeightString() const;
    
  // implementation
  
  wxFont( char *xFontName );
  void Unshare();

  GdkFont* GetInternalFont(float scale = 1.0) const;

  // no data :-)
};

/*
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
*/

#endif // __GTKFONTH__
