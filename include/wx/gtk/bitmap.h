/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKBITMAPH__
#define __GTKBITMAPH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/palette.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxDC;
class wxPaintDC;
class wxMemoryDC;
class wxToolBarGTK;
class wxBitmapButton;
class wxStaticBitmap;

class wxMask;
class wxBitmap;

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

class wxMask: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxMask)

  public:
  
    wxMask(void);
    wxMask( const wxBitmap& bitmap, const wxColour& colour );
    wxMask( const wxBitmap& bitmap, const int paletteIndex );
    wxMask( const wxBitmap& bitmap );
    ~wxMask(void);

  private:
  
    friend wxBitmap;
    friend wxDC;
    friend wxPaintDC;
    friend wxToolBarGTK;
    friend wxBitmapButton;
    friend wxStaticBitmap;
    
    GdkBitmap *GetBitmap(void) const;
    
  protected:
  
    GdkBitmap *m_bitmap;     
    
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

// CMB 20/5/98: added xbm constructor and GetBitmap() method
class wxBitmap: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxBitmap)

  public:

    wxBitmap(void);
    wxBitmap( const int width, const int height, const int depth = -1 );
    wxBitmap( const char bits[], const int width, const int height, const int depth = 1 );
    wxBitmap( char **bits );
    wxBitmap( const wxBitmap& bmp );
    wxBitmap( const wxBitmap* bmp );
    wxBitmap( const wxString &filename, const int type );
    ~wxBitmap(void);
    wxBitmap& operator = ( const wxBitmap& bmp );
    bool operator == ( const wxBitmap& bmp );
    bool operator != ( const wxBitmap& bmp );
    bool Ok(void) const;
    
    int GetHeight(void) const;
    int GetWidth(void) const;
    int GetDepth(void) const;
    void SetHeight( const int height );
    void SetWidth( const int width );
    void SetDepth( const int depth );

    wxMask *GetMask(void) const;
    void SetMask( wxMask *mask );
    
    bool SaveFile( const wxString &name, const int type, wxPalette *palette = NULL );
    bool LoadFile( const wxString &name, const int type );
        
    wxPalette *GetPalette(void) const;
    wxPalette *GetColourMap(void) const
      { return GetPalette(); };

  private:
  
    friend wxDC;
    friend wxPaintDC;
    friend wxMemoryDC;
    friend wxToolBarGTK;
    friend wxBitmapButton;
    friend wxStaticBitmap;

    GdkPixmap *GetPixmap(void) const;
    GdkBitmap *GetBitmap(void) const;
    
    // no data :-)
};

#endif // __GTKBITMAPH__
