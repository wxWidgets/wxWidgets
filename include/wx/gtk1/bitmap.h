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
class wxFrame;
class wxDialog;
class wxTreeCtrl;

class wxMask;
class wxBitmap;

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

class wxMask: public wxObject
{
  DECLARE_DYNAMIC_CLASS(wxMask)

  public:

    wxMask();
    wxMask( const wxBitmap& bitmap, const wxColour& colour );
    wxMask( const wxBitmap& bitmap, int paletteIndex );
    wxMask( const wxBitmap& bitmap );
    ~wxMask();

  private:

    friend wxBitmap;
    friend wxDC;
    friend wxPaintDC;
    friend wxToolBar;
    friend wxBitmapButton;
    friend wxStaticBitmap;
    friend wxFrame;
    friend wxDialog;
    friend wxTreeCtrl;

    GdkBitmap *GetBitmap() const;

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

    wxBitmap();
    wxBitmap( int width, int height, int depth = -1 );
    wxBitmap( const char bits[], int width, int height, int depth = 1 );
    wxBitmap( char **bits );
    wxBitmap( const wxBitmap& bmp );
    wxBitmap( const wxBitmap* bmp );
    wxBitmap( const wxString &filename, int type = wxBITMAP_TYPE_XPM);
    ~wxBitmap();
    wxBitmap& operator = ( const wxBitmap& bmp );
    bool operator == ( const wxBitmap& bmp );
    bool operator != ( const wxBitmap& bmp );
    bool Ok() const;

    int GetHeight() const;
    int GetWidth() const;
    int GetDepth() const;
    void SetHeight( int height );
    void SetWidth( int width );
    void SetDepth( int depth );

    wxMask *GetMask() const;
    void SetMask( wxMask *mask );

    void Resize( int height, int width );

    bool SaveFile( const wxString &name, int type, wxPalette *palette = (wxPalette *) NULL );
    bool LoadFile( const wxString &name, int type = wxBITMAP_TYPE_XPM);

    wxPalette *GetPalette() const;
    wxPalette *GetColourMap() const
      { return GetPalette(); };

  private:

    friend wxDC;
    friend wxPaintDC;
    friend wxMemoryDC;
    friend wxToolBar;
    friend wxBitmapButton;
    friend wxStaticBitmap;
    friend wxFrame;
    friend wxDialog;
    friend wxTreeCtrl;

    GdkPixmap *GetPixmap() const;
    GdkBitmap *GetBitmap() const;
  
    void DestroyImage();
    void RecreateImage();
    void Render();

    // no data :-)
};

#endif // __GTKBITMAPH__
