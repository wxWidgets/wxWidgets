/////////////////////////////////////////////////////////////////////////////
// Name:        bitmap.h
// Purpose:
// Author:      Robert Roebling
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
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
#include "wx/gdiobj.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMask;
class wxBitmap;

//-----------------------------------------------------------------------------
// wxMask
//-----------------------------------------------------------------------------

class wxMask: public wxObject
{
public:
    wxMask();
    wxMask( const wxBitmap& bitmap, const wxColour& colour );
    wxMask( const wxBitmap& bitmap, int paletteIndex );
    wxMask( const wxBitmap& bitmap );
    ~wxMask();
  
    bool Create( const wxBitmap& bitmap, const wxColour& colour );
    bool Create( const wxBitmap& bitmap, int paletteIndex );
    bool Create( const wxBitmap& bitmap );

    // implementation
    GdkBitmap   *m_bitmap;
    
    GdkBitmap *GetBitmap() const;
  
private:
    DECLARE_DYNAMIC_CLASS(wxMask)
};

//-----------------------------------------------------------------------------
// wxBitmap
//-----------------------------------------------------------------------------

class wxBitmap: public wxGDIObject
{
public:
    wxBitmap();
    wxBitmap( int width, int height, int depth = -1 );
    wxBitmap( const char bits[], int width, int height, int depth = 1 );
    wxBitmap( const char **bits ) { (void)CreateFromXpm(bits); }
    wxBitmap( char **bits ) { (void)CreateFromXpm((const char **)bits); }
    wxBitmap( const wxBitmap& bmp );
    wxBitmap( const wxString &filename, int type = wxBITMAP_TYPE_XPM );
    ~wxBitmap();
    wxBitmap& operator = ( const wxBitmap& bmp );
    bool operator == ( const wxBitmap& bmp ) const;
    bool operator != ( const wxBitmap& bmp ) const;
    bool Ok() const;

    bool Create(int width, int height, int depth = -1);
    
    int GetHeight() const;
    int GetWidth() const;
    int GetDepth() const;

    wxMask *GetMask() const;
    void SetMask( wxMask *mask );
    
    wxBitmap GetSubBitmap( const wxRect& rect ) const;

    bool SaveFile( const wxString &name, int type, wxPalette *palette = (wxPalette *) NULL );
    bool LoadFile( const wxString &name, int type = wxBITMAP_TYPE_XPM );

    wxPalette *GetPalette() const;
    wxPalette *GetColourMap() const
    { return GetPalette(); };

    // implementation
    // --------------

    void SetHeight( int height );
    void SetWidth( int width );
    void SetDepth( int depth );
    void SetPixmap( GdkPixmap *pixmap );
    void SetBitmap( GdkBitmap *bitmap );

    GdkPixmap *GetPixmap() const;
    GdkBitmap *GetBitmap() const;
    
protected:
    bool CreateFromXpm(const char **bits);

private:
    DECLARE_DYNAMIC_CLASS(wxBitmap)
};

#endif // __GTKBITMAPH__
