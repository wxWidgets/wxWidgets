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
class wxToolBar;
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
    wxMask( const wxBitmap& bitmap, int paletteIndex );
    wxMask( const wxBitmap& bitmap );
    ~wxMask(void);

  private:
  
    friend wxBitmap;
    friend wxDC;
    friend wxPaintDC;
    friend wxToolBar;
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
    wxBitmap( int width, int height, int depth = -1 );
    wxBitmap( const char bits[], int width, int height, int depth = 1 );
    wxBitmap( char **bits );
    wxBitmap( const wxBitmap& bmp );
    wxBitmap( const wxBitmap* bmp );
    wxBitmap( const wxString &filename, int type );
    ~wxBitmap(void);
    wxBitmap& operator = ( const wxBitmap& bmp );
    bool operator == ( const wxBitmap& bmp );
    bool operator != ( const wxBitmap& bmp );
    bool Ok(void) const;
    
    int GetHeight(void) const;
    int GetWidth(void) const;
    int GetDepth(void) const;
    void SetHeight( int height );
    void SetWidth( int width );
    void SetDepth( int depth );

    wxMask *GetMask(void) const;
    void SetMask( wxMask *mask );
    
    void Resize( int height, int width );
    
    bool SaveFile( const wxString &name, int type, wxPalette *palette = NULL );
    bool LoadFile( const wxString &name, int type );

    wxPalette *GetPalette(void) const;
    wxPalette *GetColourMap(void) const
      { return GetPalette(); };

  private:
  
    friend wxDC;
    friend wxPaintDC;
    friend wxMemoryDC;
    friend wxToolBar;
    friend wxBitmapButton;
    friend wxStaticBitmap;

    GdkPixmap *GetPixmap(void) const;
    GdkBitmap *GetBitmap(void) const;
    
    void DestroyImage(void);
    void RecreateImage(void);
    void Render(void);
    
    // no data :-)
};

#endif // __GTKBITMAPH__
