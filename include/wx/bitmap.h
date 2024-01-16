///////////////////////////////////////////////////////////////////////////////
// Name:        wx/bitmap.h
// Purpose:     wxBitmap class interface
// Author:      Vaclav Slavik
// Created:     22.04.01
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BITMAP_H_BASE_
#define _WX_BITMAP_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/string.h"
#include "wx/gdicmn.h"  // for wxBitmapType
#include "wx/colour.h"
#include "wx/image.h"
#include "wx/variant.h"

class WXDLLIMPEXP_FWD_CORE wxBitmap;
class WXDLLIMPEXP_FWD_CORE wxBitmapHandler;
class WXDLLIMPEXP_FWD_CORE wxCursor;
class WXDLLIMPEXP_FWD_CORE wxDC;
class WXDLLIMPEXP_FWD_CORE wxIcon;
class WXDLLIMPEXP_FWD_CORE wxMask;
class WXDLLIMPEXP_FWD_CORE wxPalette;
class WXDLLIMPEXP_FWD_CORE wxPixelDataBase;

// ----------------------------------------------------------------------------
// wxMask represents the transparent area of the bitmap
// ----------------------------------------------------------------------------

// TODO: all implementation of wxMask, except the generic one,
//       do not derive from wxMaskBase,,, they should
class WXDLLIMPEXP_CORE wxMaskBase : public wxObject
{
public:
    // create the mask from bitmap pixels of the given colour
    bool Create(const wxBitmap& bitmap, const wxColour& colour);

#if wxUSE_PALETTE
    // create the mask from bitmap pixels with the given palette index
    bool Create(const wxBitmap& bitmap, int paletteIndex);
#endif // wxUSE_PALETTE

    // create the mask from the given mono bitmap
    bool Create(const wxBitmap& bitmap);

protected:
    // this function is called from Create() to free the existing mask data
    virtual void FreeData() = 0;

    // these functions must be overridden to implement the corresponding public
    // Create() methods, they shouldn't call FreeData() as it's already called
    // by the public wrappers
    virtual bool InitFromColour(const wxBitmap& bitmap,
                                const wxColour& colour) = 0;
    virtual bool InitFromMonoBitmap(const wxBitmap& bitmap) = 0;
};

#if defined(__WXDFB__) || \
    defined(__WXMAC__) || \
    defined(__WXGTK__) || \
    defined(__WXX11__) || \
    defined(__WXQT__)
    #define wxUSE_BITMAP_BASE 1
#else
    #define wxUSE_BITMAP_BASE 0
#endif

// a more readable way to tell
#define wxBITMAP_SCREEN_DEPTH       (-1)


// ----------------------------------------------------------------------------
// wxBitmapHelpers: container for various bitmap methods common to all ports.
// ----------------------------------------------------------------------------

// Unfortunately, currently wxBitmap does not inherit from wxBitmapBase on all
// platforms and this is not easy to fix. So we extract at least some common
// methods into this class from which both wxBitmapBase (and hence wxBitmap on
// all platforms where it does inherit from it) and wxBitmap in wxMSW and other
// exceptional ports (only wxPM and old wxCocoa) inherit.
class WXDLLIMPEXP_CORE wxBitmapHelpers
{
public:
    // Create a new wxBitmap from the PNG data in the given buffer.
    static wxBitmap NewFromPNGData(const void* data, size_t size);

    // Rescale the given bitmap to the requested size.
    static void Rescale(wxBitmap& bmp, const wxSize& sizeNeeded);

    // wxVariant support
    wxDECLARE_VARIANT_OBJECT_EXPORTED(wxBitmap, WXDLLIMPEXP_CORE);
};


// All ports except wxMSW use wxBitmapHandler and wxBitmapBase as
// base class for wxBitmapHandler; wxMSW uses wxGDIImageHandler as
// base class since it allows some code reuse there.
#if wxUSE_BITMAP_BASE

// ----------------------------------------------------------------------------
// wxBitmapHandler: class which knows how to create/load/save bitmaps in
// different formats
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapHandler : public wxObject
{
public:
    wxBitmapHandler() { m_type = wxBITMAP_TYPE_INVALID; }
    virtual ~wxBitmapHandler() = default;

    // NOTE: the following functions should be pure virtuals, but they aren't
    //       because otherwise almost all ports would have to implement
    //       them as "return false"...

    virtual bool Create(wxBitmap *WXUNUSED(bitmap), const void* WXUNUSED(data),
                         wxBitmapType WXUNUSED(type), int WXUNUSED(width), int WXUNUSED(height),
                         int WXUNUSED(depth) = 1)
        { return false; }

    virtual bool LoadFile(wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name),
                           wxBitmapType WXUNUSED(type), int WXUNUSED(desiredWidth),
                           int WXUNUSED(desiredHeight))
        { return false; }

    virtual bool SaveFile(const wxBitmap *WXUNUSED(bitmap), const wxString& WXUNUSED(name),
                           wxBitmapType WXUNUSED(type), const wxPalette *WXUNUSED(palette) = nullptr) const
        { return false; }

    void SetName(const wxString& name)      { m_name = name; }
    void SetExtension(const wxString& ext)  { m_extension = ext; }
    void SetType(wxBitmapType type)         { m_type = type; }
    const wxString& GetName() const         { return m_name; }
    const wxString& GetExtension() const    { return m_extension; }
    wxBitmapType GetType() const            { return m_type; }

private:
    wxString      m_name;
    wxString      m_extension;
    wxBitmapType  m_type;

    wxDECLARE_ABSTRACT_CLASS(wxBitmapHandler);
};

// ----------------------------------------------------------------------------
// wxBitmap: class which represents platform-dependent bitmap (unlike wxImage)
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxBitmapBase : public wxGDIObject,
                                      public wxBitmapHelpers
{
public:
    /*
    Derived class must implement these:

    wxBitmap();
    wxBitmap(const wxBitmap& bmp);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH);
    wxBitmap(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH);
    wxBitmap(const char* const* bits);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_XPM);
    wxBitmap(const wxImage& image, int depth = wxBITMAP_SCREEN_DEPTH, double scale = 1.0);

    static void InitStandardHandlers();
    */

    virtual bool Create(int width, int height, int depth = wxBITMAP_SCREEN_DEPTH) = 0;
    virtual bool Create(const wxSize& sz, int depth = wxBITMAP_SCREEN_DEPTH) = 0;

    // DIP size and logical size are the same thing for ports using scaling,
    // i.e. where physical and logical sizes are different (e.g. wxGTK and
    // wxOSX), but we want to have both sets of functions to use them in the
    // ports where physical and logical sizes are the same (wxMSW).

    bool CreateWithDIPSize(const wxSize& sz,
                           double scale,
                           int depth = wxBITMAP_SCREEN_DEPTH)
        { return DoCreate(sz, scale, depth); }
    bool CreateWithDIPSize(int width, int height,
                           double scale,
                           int depth = wxBITMAP_SCREEN_DEPTH)
        { return DoCreate(wxSize(width, height), scale, depth); }

    bool CreateWithLogicalSize(const wxSize& sz,
                               double scale,
                               int depth = wxBITMAP_SCREEN_DEPTH)
        { return DoCreate(sz, scale, depth); }
    bool CreateWithLogicalSize(int width, int height,
                               double scale,
                               int depth = wxBITMAP_SCREEN_DEPTH)
        { return DoCreate(wxSize(width, height), scale, depth); }

    virtual int GetHeight() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetDepth() const = 0;

    wxSize GetSize() const
        { return wxSize(GetWidth(), GetHeight()); }

    // Store or return the scale factor, which determines the ratio between the
    // bitmap physical size and its DIP size (on all platforms). By default
    // it's just 1.
    virtual void SetScaleFactor(double scale);
    virtual double GetScaleFactor() const;

    // This function returns the size divided by the scale factor, so that a
    // 64x64 bitmap with a scale factor of 2 has DIP size of 32x32 everywhere.
    wxSize GetDIPSize() const;

    // These functions return the corresponding metrics divided by the scale
    // factor on platforms with DPI-independent pixels (e.g. GTK, Mac) and just
    // return the same thing as normal accessors elsewhere (e.g. MSW).
    double GetLogicalWidth() const;
    double GetLogicalHeight() const;
    wxSize GetLogicalSize() const;

    // Old synonyms for CreateWithDIPSize() and GetLogicalXXX() functions,
    // prefer the new names in the new code.
    bool CreateScaled(int w, int h, int d, double logicalScale)
        { return CreateWithDIPSize(w, h, logicalScale, d); }
    double GetScaledWidth() const { return GetLogicalWidth(); }
    double GetScaledHeight() const { return GetLogicalHeight(); }
    wxSize GetScaledSize() const { return GetLogicalSize(); }

#if wxUSE_IMAGE
    virtual wxImage ConvertToImage() const = 0;

    // Convert to disabled (dimmed) bitmap.
    wxBitmap ConvertToDisabled(unsigned char brightness = 255) const;
#endif // wxUSE_IMAGE

    virtual wxMask *GetMask() const = 0;
    virtual void SetMask(wxMask *mask) = 0;

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const = 0;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = nullptr) const = 0;
    virtual bool LoadFile(const wxString &name, wxBitmapType type) = 0;

    /*
       If raw bitmap access is supported (see wx/rawbmp.h), the following
       methods should be implemented:

       virtual bool GetRawData(wxRawBitmapData *data) = 0;
       virtual void UngetRawData(wxRawBitmapData *data) = 0;
     */

#if wxUSE_PALETTE
    virtual wxPalette *GetPalette() const = 0;
    virtual void SetPalette(const wxPalette& palette) = 0;
#endif // wxUSE_PALETTE

    // Alpha support for 32bpp bitmaps: check if it's used, request that it be
    // used or not.
    virtual bool HasAlpha() const;
    virtual bool UseAlpha(bool use = true);
    void ResetAlpha() { UseAlpha(false); }

    // copies the contents and mask of the given (colour) icon to the bitmap
    bool CopyFromIcon(const wxIcon& icon);

    // implementation:
#if WXWIN_COMPATIBILITY_3_0
    // deprecated
    virtual void SetHeight(int height) = 0;
    virtual void SetWidth(int width) = 0;
    virtual void SetDepth(int depth) = 0;
#endif

    // Format handling
    static inline wxList& GetHandlers() { return sm_handlers; }
    static void AddHandler(wxBitmapHandler *handler);
    static void InsertHandler(wxBitmapHandler *handler);
    static bool RemoveHandler(const wxString& name);
    static wxBitmapHandler *FindHandler(const wxString& name);
    static wxBitmapHandler *FindHandler(const wxString& extension, wxBitmapType bitmapType);
    static wxBitmapHandler *FindHandler(wxBitmapType bitmapType);

    //static void InitStandardHandlers();
    //  (wxBitmap must implement this one)

    static void CleanUpHandlers();

    // this method is only used by the generic implementation of wxMask
    // currently but could be useful elsewhere in the future: it can be
    // overridden to quantize the colour to correspond to bitmap colour depth
    // if necessary; default implementation simply returns the colour as is
    virtual wxColour QuantizeColour(const wxColour& colour) const
    {
        return colour;
    }

protected:
    virtual bool DoCreate(const wxSize& sz, double scale, int depth);

    static wxList sm_handlers;

    wxDECLARE_ABSTRACT_CLASS(wxBitmapBase);
};

#endif // wxUSE_BITMAP_BASE


// the wxBITMAP_DEFAULT_TYPE constant defines the default argument value
// for wxBitmap's ctor and wxBitmap::LoadFile() functions.
#if defined(__WXMSW__)
    #define wxBITMAP_DEFAULT_TYPE    wxBITMAP_TYPE_BMP_RESOURCE
    #include "wx/msw/bitmap.h"
#elif defined(__WXGTK__)
    #ifdef __WINDOWS__
        #define wxBITMAP_DEFAULT_TYPE    wxBITMAP_TYPE_BMP_RESOURCE
    #else
        #define wxBITMAP_DEFAULT_TYPE    wxBITMAP_TYPE_XPM
    #endif
    #include "wx/gtk/bitmap.h"
#elif defined(__WXX11__)
    #define wxBITMAP_DEFAULT_TYPE    wxBITMAP_TYPE_XPM
    #include "wx/x11/bitmap.h"
#elif defined(__WXDFB__)
    #define wxBITMAP_DEFAULT_TYPE    wxBITMAP_TYPE_BMP_RESOURCE
    #include "wx/dfb/bitmap.h"
#elif defined(__WXMAC__)
    #define wxBITMAP_DEFAULT_TYPE    wxBITMAP_TYPE_PICT_RESOURCE
    #include "wx/osx/bitmap.h"
#elif defined(__WXQT__)
    #define wxBITMAP_DEFAULT_TYPE    wxBITMAP_TYPE_XPM
    #include "wx/qt/bitmap.h"
#endif

#if wxUSE_IMAGE
inline
wxBitmap
#if wxUSE_BITMAP_BASE
wxBitmapBase::
#else
wxBitmap::
#endif
ConvertToDisabled(unsigned char brightness) const
{
    const wxImage imgDisabled = ConvertToImage().ConvertToDisabled(brightness);
    return wxBitmap(imgDisabled, -1, GetScaleFactor());
}
#endif // wxUSE_IMAGE

// we must include generic mask.h after wxBitmap definition
#if defined(__WXDFB__)
    #define wxUSE_GENERIC_MASK 1
#else
    #define wxUSE_GENERIC_MASK 0
#endif

#if wxUSE_GENERIC_MASK
    #include "wx/generic/mask.h"
#endif

#endif // _WX_BITMAP_H_BASE_
