/////////////////////////////////////////////////////////////////////////////
// Name:        font.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
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
    wxFont(void);
    wxFont( int PointSize, int FontIdOrFamily, int Style, int Weight,
	   bool underlined = FALSE, const char *Face=NULL );
    wxFont( int PointSize, const char *Face, int Family, int Style, int Weight, 
	   bool underlined = FALSE );
    wxFont( const wxFont& font );
    wxFont( const wxFont* font );
    ~wxFont(void);
    wxFont& operator = ( const wxFont& font );
    bool operator == ( const wxFont& font );
    bool operator != ( const wxFont& font );
    bool Ok();

    int GetPointSize(void) const;
    wxString GetFaceName(void) const;
    int GetFamily(void) const;
    wxString GetFamilyString(void) const;
    int GetFontId(void) const;
    wxString GetFaceString(void) const;
    int GetStyle(void) const;
    wxString GetStyleString(void) const;
    int GetWeight(void) const;
    wxString GetWeightString(void) const;
    bool GetUnderlined(void) const;

    wxFont( char *xFontName );
    
  private:
  
    friend wxDC;
    friend wxPaintDC;
    friend wxWindow;
    
    GdkFont* GetInternalFont(float scale = 1.0);

    // no data :-)
};

//-----------------------------------------------------------------------------
// wxFontDirectory
//-----------------------------------------------------------------------------

class wxFontNameDirectory: public wxObject 
{
  DECLARE_DYNAMIC_CLASS(wxFontNameDirectory)
  
  public:
    wxFontNameDirectory(void);
    ~wxFontNameDirectory();

    void  Initialize(void);
    void  Initialize(int fontid, int family, const char *name);

    int   FindOrCreateFontId(const char *name, int family);
    char* GetAFMName(int fontid, int weight, int style);
    int   GetFamily(int fontid);
    int   GetFontId(const char *name);
    char* GetFontName(int fontid);
    int   GetNewFontId(void);
    char* GetPostScriptName(int fontid, int weight, int style);
    char* GetScreenName(int fontid, int weight, int style);
    
    
    class wxHashTable *table;
    int   nextFontId;
};

extern wxFontNameDirectory *wxTheFontNameDirectory;

#endif // __GTKFONTH__
