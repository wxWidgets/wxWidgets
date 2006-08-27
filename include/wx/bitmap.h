///////////////////////////////////////////////////////////////////////////////
// Name:        wx/bitmap.h
// Purpose:     wxBitmap class interface
// Author:      Vaclav Slavik
// Modified by:
// Created:     22.04.01
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BITMAP_H_BASE_
#define _WX_BITMAP_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/string.h"
#include "wx/gdiobj.h"
#include "wx/gdicmn.h"  // for wxBitmapType

class WXDLLEXPORT wxBitmap;
class WXDLLEXPORT wxBitmapHandler;
class WXDLLEXPORT wxImage;
class WXDLLEXPORT wxMask;
class WXDLLEXPORT wxPalette;

#if defined(__WXMGL__) || \
    defined(__WXDFB__) || \
    defined(__WXMAC__) || \
    defined(__WXGTK__) || \
    defined(__WXCOCOA__) || \
    defined(__WXMOTIF__) || \
    defined(__WXX11__)
// Only used by some ports
// FIXME -- make all ports (but MSW which uses wxGDIImage) use these base classes

// ----------------------------------------------------------------------------
// wxBitmapHandler: class which knows how to create/load/save bitmaps in
// different formats
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxBitmapHandlerBase : public wxObject
{
public:
    wxBitmapHandlerBase()
        : m_name()
        , m_extension()
        , m_type(wxBITMAP_TYPE_INVALID)
    { }

    virtual ~wxBitmapHandlerBase() { }

    virtual bool Create(wxBitmap *bitmap, void *data, long flags,
                          int width, int height, int depth = 1) = 0;
    virtual bool LoadFile(wxBitmap *bitmap, const wxString& name, long flags,
                          int desiredWidth, int desiredHeight) = 0;
    virtual bool SaveFile(const wxBitmap *bitmap, const wxString& name,
                          int type, const wxPalette *palette = NULL) = 0;

    void SetName(const wxString& name)      { m_name = name; }
    void SetExtension(const wxString& ext)  { m_extension = ext; }
    void SetType(wxBitmapType type)         { m_type = type; }
    wxString GetName() const                { return m_name; }
    wxString GetExtension() const           { return m_extension; }
    wxBitmapType GetType() const            { return m_type; }

private:
    wxString      m_name;
    wxString      m_extension;
    wxBitmapType  m_type;

private:
    DECLARE_ABSTRACT_CLASS(wxBitmapHandlerBase)
};

class WXDLLEXPORT wxBitmapBase : public wxGDIObject
{
public:
    /*
    Derived class must implement these:

    wxBitmap();
    wxBitmap(int width, int height, int depth = -1);
    wxBitmap(const char bits[], int width, int height, int depth = 1);
    wxBitmap(const char **bits);
    wxBitmap(char **bits);
    wxBitmap(const wxString &filename, wxBitmapType type = wxBITMAP_TYPE_XPM);
    wxBitmap(const wxImage& image, int depth = -1);
    bool operator == (const wxBitmap& bmp) const;
    bool operator != (const wxBitmap& bmp) const;

    bool Create(int width, int height, int depth = -1);

    static void InitStandardHandlers();
    */

    virtual bool Ok() const = 0;

    virtual int GetHeight() const = 0;
    virtual int GetWidth() const = 0;
    virtual int GetDepth() const = 0;

    virtual wxImage ConvertToImage() const = 0;

    virtual wxMask *GetMask() const = 0;
    virtual void SetMask(wxMask *mask) = 0;

    virtual wxBitmap GetSubBitmap(const wxRect& rect) const = 0;

    virtual bool SaveFile(const wxString &name, wxBitmapType type,
                          const wxPalette *palette = (wxPalette *)NULL) const = 0;
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

    // copies the contents and mask of the given (colour) icon to the bitmap
    virtual bool CopyFromIcon(const wxIcon& icon) = 0;

    // implementation:
    virtual void SetHeight(int height) = 0;
    virtual void SetWidth(int width) = 0;
    virtual void SetDepth(int depth) = 0;

    // Format handling
    static inline wxList& GetHandlers() { return sm_handlers; }
    static void AddHandler(wxBitmapHandlerBase *handler);
    static void InsertHandler(wxBitmapHandlerBase *handler);
    static bool RemoveHandler(const wxString& name);
    static wxBitmapHandler *FindHandler(const wxString& name);
    static wxBitmapHandler *FindHandler(const wxString& extension, wxBitmapType bitmapType);
    static wxBitmapHandler *FindHandler(wxBitmapType bitmapType);

    //static void InitStandardHandlers();
    //  (wxBitmap must implement this one)

    static void CleanUpHandlers();

protected:
    static wxList sm_handlers;

    DECLARE_ABSTRACT_CLASS(wxBitmapBase)
};
#endif

#if defined(__WXPALMOS__)
#include "wx/palmos/bitmap.h"
#elif defined(__WXMSW__)
#include "wx/msw/bitmap.h"
#elif defined(__WXMOTIF__)
#include "wx/x11/bitmap.h"
#elif defined(__WXGTK20__)
#include "wx/gtk/bitmap.h"
#elif defined(__WXGTK__)
#include "wx/gtk1/bitmap.h"
#elif defined(__WXX11__)
#include "wx/x11/bitmap.h"
#elif defined(__WXMGL__)
#include "wx/mgl/bitmap.h"
#elif defined(__WXDFB__)
#include "wx/dfb/bitmap.h"
#elif defined(__WXMAC__)
#include "wx/mac/bitmap.h"
#elif defined(__WXCOCOA__)
#include "wx/cocoa/bitmap.h"
#elif defined(__WXPM__)
#include "wx/os2/bitmap.h"
#endif

#endif
    // _WX_BITMAP_H_BASE_
