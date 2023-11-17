/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/bitmap.h
// Purpose:     wxBitmap class
// Author:      Stefan Csomor
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BITMAP_H_
#define _WX_BITMAP_H_

#include "wx/palette.h"

// Bitmap
class wxBitmapRefData ;

// A mask is a bitmap used for drawing bitmaps
// Internally it is stored as a 8 bit deep memory chunk, 0 = black means the source will be drawn
// 255 = white means the source will not be drawn, no other values will be present
// 8 bit is chosen only for performance reasons, note also that this is the inverse value range
// from alpha, where 0 = invisible , 255 = fully drawn

class WXDLLIMPEXP_CORE wxMask: public wxMaskBase
{
    wxDECLARE_DYNAMIC_CLASS(wxMask);

public:
    wxMask();

    // Copy constructor
    wxMask(const wxMask& mask);

    // Construct a mask from a bitmap and a colour indicating
    // the transparent area
    wxMask(const wxBitmap& bitmap, const wxColour& colour);

    // Construct a mask from a mono bitmap (black meaning show pixels, white meaning transparent)
    wxMask(const wxBitmap& bitmap);

    virtual ~wxMask();

    wxBitmap GetBitmap() const;

    // Implementation below

    void Init() ;

    // a 8 bit depth mask
    void* GetRawAccess() const;
    int GetBytesPerRow() const;
    int GetWidth() const;
    int GetHeight() const;

    // renders/updates native representation when necessary
    void RealizeNative() ;

    WXHBITMAP GetHBITMAP() const ;

    // implementation helper only : construct a mask from a 8 bpp memory buffer
    bool OSXCreate(const wxMemoryBuffer& buf, int width , int height , int bytesPerRow ) ;

protected:
    // this function is called from Create() to free the existing mask data
    virtual void FreeData() override;

    // these functions must be overridden to implement the corresponding public
    // Create() methods, they shouldn't call FreeData() as it's already called
    // by the public wrappers
    virtual bool InitFromColour(const wxBitmap& bitmap,
                                const wxColour& colour) override;
    virtual bool InitFromMonoBitmap(const wxBitmap& bitmap) override;

private:
    void DoCreateMaskBitmap(int width, int height, int bytesPerRow = -1);

    wxCFRef<CGContextRef> m_maskBitmap ;

};

class WXDLLIMPEXP_CORE wxBitmap: public wxBitmapBase
{
    wxDECLARE_DYNAMIC_CLASS(wxBitmap);

    friend class WXDLLIMPEXP_FWD_CORE wxBitmapHandler;

public:
    wxBitmap() = default; // Platform-specific

    // Initialize with raw data.
    wxBitmap(const char bits[], int width, int height, int depth = 1);

    // Initialize with XPM data
    wxBitmap(const char* const* bits);

    // Load a file or resource
    wxBitmap(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);

    // Constructor for generalised creation from data
    wxBitmap(const void* data, wxBitmapType type, int width, int height, int depth = 1);

    // creates an bitmap from the native image format
    wxBitmap(CGImageRef image, double scale = 1.0);
    wxBitmap(WXImage image);
    wxBitmap(CGContextRef bitmapcontext);

    // Create a bitmap compatible with the given DC
    wxBitmap(int width, int height, const wxDC& dc);

    // If depth is omitted, will create a bitmap compatible with the display
    wxBitmap(int width, int height, int depth = -1) { (void)Create(width, height, depth); }
    wxBitmap(const wxSize& sz, int depth = -1) { (void)Create(sz, depth); }

    // Convert from wxImage:
    wxBitmap(const wxImage& image, int depth = -1, double scale = 1.0);
    wxBitmap(const wxImage& image, const wxDC& dc);

    // Convert from wxIcon
    wxBitmap(const wxIcon& icon) { CopyFromIcon(icon); }

#if wxOSX_USE_COCOA
    // Convert from wxCursor
    wxBitmap(const wxCursor &cursor);
#endif

    virtual ~wxBitmap() = default;

    wxImage ConvertToImage() const override;

    // get the given part of bitmap
    wxBitmap GetSubBitmap( const wxRect& rect ) const override;

    virtual bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH) override;
    virtual bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH) override
        { return Create(sz.GetWidth(), sz.GetHeight(), depth); }

    virtual bool Create(const void* data, wxBitmapType type, int width, int height, int depth = 1);
    bool Create( CGImageRef image, double scale = 1.0 );
    bool Create( WXImage image );
    bool Create( CGContextRef bitmapcontext);

    // Create a bitmap compatible with the given DC, inheriting its magnification factor
    bool Create(int width, int height, const wxDC& dc);

    // virtual bool Create( WXHICON icon) ;
    virtual bool LoadFile(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE) override;
    virtual bool SaveFile(const wxString& name, wxBitmapType type, const wxPalette *cmap = nullptr) const override;

    const wxBitmapRefData *GetBitmapData() const
        { return (const wxBitmapRefData *)m_refData; }

    wxBitmapRefData *GetBitmapData()
        { return (wxBitmapRefData *)m_refData; }

    int GetWidth() const override;
    int GetHeight() const override;
    int GetDepth() const override;

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_MSG("this value is determined during creation, this method could lead to inconsistencies")
    void SetWidth(int width) override;
    wxDEPRECATED_MSG("this value is determined during creation, this method could lead to inconsistencies")
    void SetHeight(int height) override;
    wxDEPRECATED_MSG("this value is determined during creation, this method could lead to inconsistencies")
    void SetDepth(int depth) override;
#endif

#if wxUSE_PALETTE
    wxPalette* GetPalette() const override;
    void SetPalette(const wxPalette& palette) override;
#endif // wxUSE_PALETTE

    wxMask *GetMask() const override;
    void SetMask(wxMask *mask) override;

    static void InitStandardHandlers();

    // raw bitmap access support functions, for internal use only
    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    bool HasAlpha() const override;
    bool UseAlpha(bool use = true) override;

    // returns the 'native' implementation, a GWorldPtr for the content and one for the mask
    WXHBITMAP GetHBITMAP( WXHBITMAP * mask = nullptr ) const;

    // returns a CGImageRef which must released after usage with CGImageRelease
    CGImageRef CreateCGImage() const ;

    // returns nil for invalid bitmap
    WXImage OSXGetImage() const;
#if wxOSX_USE_COCOA
    // returns an autoreleased version of the image
    WX_NSImage GetNSImage() const
        { return OSXGetImage(); }
#endif
#if wxOSX_USE_IPHONE
    // returns an autoreleased version of the image
    WX_UIImage GetUIImage() const
        { return OSXGetImage(); }
#endif

#if WXWIN_COMPATIBILITY_3_0

#if wxOSX_USE_ICONREF
    // returns a IconRef which must be retained before and released after usage
    wxDEPRECATED_MSG("IconRefs are deprecated, this will be removed in the future")
    IconRef GetIconRef() const;
    // returns a IconRef which must be released after usage
    wxDEPRECATED_MSG("IconRefs are deprecated, this will be removed in the future")
    IconRef CreateIconRef() const;
#endif

    // get read only access to the underlying buffer
    wxDEPRECATED_MSG("use GetRawData for accessing the buffer")
    const void *GetRawAccess() const;
    // brackets to the underlying OS structure for read/write access
    // makes sure that no cached images will be constructed until terminated
    wxDEPRECATED_MSG("use GetRawData for accessing the buffer")
    void *BeginRawAccess();
    wxDEPRECATED_MSG("use GetRawData for accessing the buffer")
    void EndRawAccess();
#endif

    void SetScaleFactor(double scale) override;
    double GetScaleFactor() const override;

    void SetSelectedInto(wxDC *dc);
    wxDC *GetSelectedInto() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const override;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const override;

    virtual bool DoCreate(const wxSize& sz, double scale, int depth) override;

private:
    void InitFromImage(const wxImage& image, int depth, double scale);
};

#endif // _WX_BITMAP_H_
