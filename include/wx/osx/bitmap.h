/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/bitmap.h
// Purpose:     wxBitmap class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BITMAP_H_
#define _WX_BITMAP_H_

#include "wx/palette.h"

// Bitmap
class WXDLLIMPEXP_FWD_CORE wxBitmap;
class wxBitmapRefData ;
class WXDLLIMPEXP_FWD_CORE wxBitmapHandler;
class WXDLLIMPEXP_FWD_CORE wxControl;
class WXDLLIMPEXP_FWD_CORE wxCursor;
class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxIcon;
class WXDLLIMPEXP_FWD_CORE wxImage;
class WXDLLIMPEXP_FWD_CORE wxPixelDataBase;

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
    virtual void FreeData() wxOVERRIDE;

    // these functions must be overridden to implement the corresponding public
    // Create() methods, they shouldn't call FreeData() as it's already called
    // by the public wrappers
    virtual bool InitFromColour(const wxBitmap& bitmap,
                                const wxColour& colour) wxOVERRIDE;
    virtual bool InitFromMonoBitmap(const wxBitmap& bitmap) wxOVERRIDE;

private:
    void DoCreateMaskBitmap(int width, int height, int bytesPerRow = -1);

    wxCFRef<CGContextRef> m_maskBitmap ;

};

class WXDLLEXPORT wxBitmapRep: public wxGDIRefData
{
    friend class WXDLLIMPEXP_FWD_CORE wxIcon;
    friend class WXDLLIMPEXP_FWD_CORE wxCursor;
public:
    wxBitmapRep();
    wxBitmapRep(int width , int height , int depth, wxBitmapScale scale = wxBitmapScale() );
    wxBitmapRep(CGImageRef image, wxBitmapScale scale = wxBitmapScale() );

    wxBitmapRep(const wxBitmapRep &tocopy);
    wxBitmapRep(const wxImage& image, int depth = wxBITMAP_SCREEN_DEPTH, wxBitmapScale scale = wxBitmapScale());

    virtual ~wxBitmapRep();

    bool IsOk() const wxOVERRIDE;

    void Free();

    int GetWidth() const;
    int GetHeight() const;
    int GetDepth() const;
    int GetBytesPerRow() const;

    bool HasAlpha() const;
    wxImage ConvertToImage() const;

    void SetScale(const wxBitmapScale& scale) { m_scale = scale; }
    const wxBitmapScale& GetScale() const { return m_scale; }
    double GetScaleFactor() const { return m_scale.GetScaleFactor(); }

    const void *GetRawAccess() const;
    void *BeginRawAccess();
    void EndRawAccess();

    void UseAlpha(bool use = true);

public:
#if wxUSE_PALETTE
    wxPalette     m_bitmapPalette;
#endif // wxUSE_PALETTE

    wxMask*       GetMask() { return m_bitmapMask; }
    const wxMask* GetMask() const { return m_bitmapMask; }
    void          SetMask(wxMask* mask) { m_bitmapMask = mask; }

    wxMask *      m_bitmapMask; // Optional mask
    CGImageRef    CreateCGImage() const;

#if wxOSX_USE_ICONREF
    // caller should increase ref count if needed longer
    // than the bitmap exists
    IconRef       GetIconRef() const;
#endif

    CGContextRef  GetBitmapContext() const;

    void SetSelectedInto(wxDC *dc);
    wxDC *GetSelectedInto() const;

private :
    bool Create(int width , int height , int depth, wxBitmapScale scale);
 
    void Init();

    // make sure we have a bitmapContext before accessing raw data
    // might need a rendering phase of a native image first
    void EnsureBitmapExists() const;
    void CreateBitmapContextFromCGImage();

    void FreeDerivedRepresentations();

    int                     m_rawAccessCount;

    wxCFRef<CGImageRef>     m_cgImageRef;

    wxBitmapScale           m_scale;

#if wxOSX_USE_ICONREF
    mutable IconRef         m_iconRef;
#endif

    wxCFRef<CGContextRef>   m_hBitmap;
    wxDC*                   m_selectedInto;
};

/**
    Smart pointer wrapping wxBitmapRep.

    wxBitmapRepPtr takes ownership of wxBitmapRep passed to it on
    construction and calls DecRef() on it automatically when it is destroyed.
    It also provides transparent access to wxBitmapRep methods by allowing
    to use objects of this class as if they were wxBitmapRep pointers.
*/
typedef wxObjectDataPtr<wxBitmapRep> wxBitmapRepPtr;
typedef wxVector<wxBitmapRepPtr> wxBitmapRepresentationArray;

/*
 * wxBitmapRef data contains either multiple representations via wxBitmapRep
 * or is generated from a NSImage, which always gets precedence over potentially
 * stored wxBitmapReps because it contains other OS-private hints eg about
 * the corresponding appearance etc, so we always leave the selection for the best
 * rendering to the OS in this case
 */

class WXDLLEXPORT wxBitmapRefData : public wxGDIRefData
{
public:
    wxBitmapRefData();

    wxBitmapRefData(WXImage image);

    wxBitmapRefData(const wxBitmapRefData &tocopy);

    ~wxBitmapRefData();

    virtual bool IsOk() const;

    wxBitmapRepresentationArray& GetRepresentations() {return m_representations; }

    const wxBitmapRepresentationArray& GetRepresentations() const {return m_representations; }

    WXImage GetImage() const;

    // was this bitmap created from a NSImage
    bool IsNativeImage() const { return m_nsImageIsPrimary; }

    const wxBitmapScale& GetNativeImageScale() const { return m_nsScale; }
private:
    void Init();

    wxBitmapRepresentationArray m_representations;

    bool                        m_nsImageIsPrimary;
    wxBitmapScale               m_nsScale;
    WXImage                     m_nsImage;

    bool                        m_isTemplate;
};

class WXDLLIMPEXP_CORE wxBitmap: public wxBitmapBase
{
    wxDECLARE_DYNAMIC_CLASS(wxBitmap);

    friend class WXDLLIMPEXP_FWD_CORE wxBitmapHandler;

public:
    // Initialize with an empty array of wxBitmapReps
    wxBitmap();

    // Initialize with raw data.
    wxBitmap(const char bits[], int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);

    // Initialize with XPM data
    wxBitmap(const char* const* bits);

    // Load a file or resource
    wxBitmap(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE);

    // Constructor for generalised creation from data
    wxBitmap(const void* data, wxBitmapType type, int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);

    // creates an bitmap from the native image format
    wxBitmap(CGImageRef image, wxBitmapScale scale = wxBitmapScale());
    wxBitmap(WXImage image);

    // Create a bitmap compatible with the given DC
    wxDEPRECATED_MSG("You should create an empty bitmap and then call CreateFromDCCoords")
    wxBitmap(int width, int height, const wxDC& dc);

    // If depth is omitted, will create a bitmap compatible with the display
    wxBitmap(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH, wxBitmapScale scale = wxBitmapScale()) { (void)Create(width, height, depth, scale); }
    wxBitmap(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH, wxBitmapScale scale = wxBitmapScale()) { (void)Create(sz.GetWidth(), sz.GetHeight(), depth, scale); }

    // Convert from wxImage:
    wxBitmap(const wxImage& image, int depth = wxBITMAP_SCREEN_DEPTH, wxBitmapScale scale = wxBitmapScale());

    // Convert from wxIcon
    wxBitmap(const wxIcon& icon) { CopyFromIcon(icon); }

    virtual ~wxBitmap() {}

    static wxBitmap NewFromPNGDataWithScale(const void* data, size_t size, wxBitmapScale scale);

    void AddRepresentation( wxBitmapRep* other);

    void AddRepresentation( const wxBitmapRepPtr& other);

    wxBitmap& AddRepresentations(const wxBitmap& bitmapWithRep);

    wxImage ConvertToImage() const wxOVERRIDE;

    // get the given part of bitmap
    wxBitmap GetSubBitmap( const wxRect& rect ) const wxOVERRIDE;

    const wxBitmapRep* GetBestRepresentation( const wxSize& dimensions) const;
    wxBitmapRep* GetBestRepresentation( const wxSize& dimensions);
    const wxBitmapRep* GetDefaultRepresentation() const;
    wxBitmapRep* GetDefaultRepresentation();

    virtual bool Create(int width, int height, int depth, wxBitmapScale scale);
    virtual bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH) wxOVERRIDE
        { return Create(width, height, depth, wxBitmapScale()); }
    virtual bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH) wxOVERRIDE
        { return Create(sz.GetWidth(), sz.GetHeight(), depth); }

    virtual bool Create(const void* data, wxBitmapType type, int width, int height, int depth = 1);
    bool Create(CGImageRef image, wxBitmapScale scale = wxBitmapScale());
    bool Create(WXImage image );

    // Create a bitmap compatible with the given DC, inheriting its magnification factor
    wxDEPRECATED_MSG("You should use CreateFromDCCoords")
    bool Create(int width, int height, const wxDC& dc)
        { return CreateFromDCCoords(width, height, dc); }

    // Create a bitmap with a scale factor, width and height are multiplied with that factor
    wxDEPRECATED_MSG("You should use Create methods with pixel dimenisions, or call a CreateForXXX method")
    bool CreateScaled(int logwidth, int logheight, int depth, double logicalScale) wxOVERRIDE;

    // Create a bitmap compatible with the given DC, applying its contentScale factor to the coordinates to
    // calculate the true pixel dimensions
    bool CreateFromDCCoords(int widthInDCCoords, int heightInDCCoords, const wxDC& dc);

    // virtual bool Create( WXHICON icon) ;
    virtual bool LoadFile(const wxString& name, wxBitmapType type = wxBITMAP_DEFAULT_TYPE) wxOVERRIDE;
    virtual bool SaveFile(const wxString& name, wxBitmapType type, const wxPalette *cmap = NULL) const wxOVERRIDE;

    const wxBitmapRefData *GetBitmapData() const
        { return (const wxBitmapRefData *)m_refData; }

    wxBitmapRefData *GetBitmapData()
        { return (wxBitmapRefData *)m_refData; }

    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon) wxOVERRIDE;

    int GetWidth() const wxOVERRIDE;
    int GetHeight() const wxOVERRIDE;
    int GetDepth() const wxOVERRIDE;

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_MSG("this value is determined during creation, this method could lead to inconsistencies")
    void SetWidth(int width) wxOVERRIDE;
    wxDEPRECATED_MSG("this value is determined during creation, this method could lead to inconsistencies")
    void SetHeight(int height) wxOVERRIDE;
    wxDEPRECATED_MSG("this value is determined during creation, this method could lead to inconsistencies")
    void SetDepth(int depth) wxOVERRIDE;
#endif

#if wxUSE_PALETTE
    wxPalette* GetPalette() const wxOVERRIDE;
    void SetPalette(const wxPalette& palette) wxOVERRIDE;
#endif // wxUSE_PALETTE

    wxMask *GetMask() const wxOVERRIDE;
    void SetMask(wxMask *mask) wxOVERRIDE;

    static void InitStandardHandlers();

    // raw bitmap access support functions, for internal use only
    void *GetRawData(wxPixelDataBase& data, int bpp);
    void UngetRawData(wxPixelDataBase& data);

    // these functions are internal and shouldn't be used, they risk to
    // disappear in the future
    bool HasAlpha() const;
    void UseAlpha(bool use = true);

    // returns the 'native' implementation, a GWorldPtr for the content and one for the mask
    WXHBITMAP GetHBITMAP( WXHBITMAP * mask = NULL ) const;

    // returns a CGImageRef which must released after usage with CGImageRelease
    CGImageRef CreateCGImage() const;

    // is this wxBitmap generated from an NSImage, then this is preferred for drawing
    // over individual wxBitmapRepresentations
    bool IsNativeImage() const;

    WXImage GetImage() const;
#if wxOSX_USE_COCOA
    // returns an autoreleased version of the image
    WX_NSImage GetNSImage() const
        { return GetImage(); }
#endif
#if wxOSX_USE_IPHONE
    // returns an autoreleased version of the image
    WX_UIImage GetUIImage() const
        { return GetImage(); }
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

    void SetScale( const wxBitmapScale &scale);
    const wxBitmapScale& GetScale() const wxOVERRIDE;

    double GetScaleFactor() const wxOVERRIDE { return GetScale().GetScaleFactor(); }

    void SetSelectedInto(wxDC *dc);
    wxDC *GetSelectedInto() const;

    wxBitmapRepresentationArray& GetRepresentations();

    const wxBitmapRepresentationArray& GetRepresentations() const;

protected:
    virtual wxGDIRefData *CreateGDIRefData() const wxOVERRIDE;
    virtual wxGDIRefData *CloneGDIRefData(const wxGDIRefData *data) const wxOVERRIDE;
};

#endif // _WX_BITMAP_H_
