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

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/palette.h"
#include "wx/gdiobj.h"

#ifdef __WXGTK20__
typedef struct _GdkPixbuf GdkPixbuf;
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMask;
class wxBitmap;
class wxImage;

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

class wxBitmap: public wxBitmapBase
{
public:
    wxBitmap();
    wxBitmap( int width, int height, int depth = -1 );
    wxBitmap( const char bits[], int width, int height, int depth = 1 );
    wxBitmap( const char **bits ) { (void)CreateFromXpm(bits); }
    wxBitmap( char **bits ) { (void)CreateFromXpm((const char **)bits); }
    wxBitmap( const wxBitmap& bmp );
    wxBitmap( const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_XPM );
    wxBitmap( const wxImage& image, int depth = -1 ) { (void)CreateFromImage(image, depth); }
    ~wxBitmap();
    wxBitmap& operator = ( const wxBitmap& bmp );
    bool operator == ( const wxBitmap& bmp ) const;
    bool operator != ( const wxBitmap& bmp ) const;
    bool Ok() const;

    bool Create(int width, int height, int depth = -1);
    
    int GetHeight() const;
    int GetWidth() const;
    int GetDepth() const;
    
    wxImage ConvertToImage() const;

    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon);

    wxMask *GetMask() const;
    void SetMask( wxMask *mask );
    
    wxBitmap GetSubBitmap( const wxRect& rect ) const;

    bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = (wxPalette *)NULL) const;
    bool LoadFile(const wxString &name, wxBitmapType type = wxBITMAP_TYPE_XPM );

#if wxUSE_PALETTE
    wxPalette *GetPalette() const;
    void SetPalette(const wxPalette& palette);
    wxPalette *GetColourMap() const { return GetPalette(); };
#endif // wxUSE_PALETTE

    static void InitStandardHandlers();

    // implementation
    // --------------

    void SetHeight( int height );
    void SetWidth( int width );
    void SetDepth( int depth );
    void SetPixmap( GdkPixmap *pixmap );
    void SetBitmap( GdkBitmap *bitmap );
#ifdef __WXGTK20__
    void SetPixbuf(GdkPixbuf *pixbuf);
#endif

    GdkPixmap *GetPixmap() const;
    GdkBitmap *GetBitmap() const;
    bool HasPixmap() const;
#ifdef __WXGTK20__
    bool HasPixbuf() const;
    GdkPixbuf *GetPixbuf() const;
#endif
    
    // Basically, this corresponds to Win32 StretchBlt()
    wxBitmap Rescale( int clipx, int clipy, int clipwidth, int clipheight, int width, int height );
protected:
    bool CreateFromXpm(const char **bits);
    bool CreateFromImage(const wxImage& image, int depth);

private:
    // to be called from CreateFromImage only!
    bool CreateFromImageAsBitmap(const wxImage& image);
    bool CreateFromImageAsPixmap(const wxImage& image);

#ifdef __WXGTK20__
    bool CreateFromImageAsPixbuf(const wxImage& image);

    enum Representation
    {
        Pixmap,
        Pixbuf
    };
    // removes other representations from memory, keeping only 'keep'
    // (wxBitmap may keep same bitmap e.g. as both pixmap and pixbuf):
    void PurgeOtherRepresentations(Representation keep);

    friend class wxMemoryDC;
#endif
    friend class wxBitmapHandler;

private:    
    DECLARE_DYNAMIC_CLASS(wxBitmap)
};

//-----------------------------------------------------------------------------
// wxBitmapHandler
//-----------------------------------------------------------------------------

class wxBitmapHandler: public wxBitmapHandlerBase
{
public:
    wxBitmapHandler() { }
    virtual ~wxBitmapHandler();

    virtual bool Create(wxBitmap *bitmap, void *data, long flags, int width, int height, int depth = 1);
    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
        int desiredWidth, int desiredHeight);
    virtual bool SaveFile(const wxBitmap *bitmap, const wxString& name, int type, const wxPalette *palette = NULL);

private:
    DECLARE_DYNAMIC_CLASS(wxBitmapHandler)
};


#endif // __GTKBITMAPH__
