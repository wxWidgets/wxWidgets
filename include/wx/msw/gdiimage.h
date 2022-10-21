///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/gdiimage.h
// Purpose:     wxGDIImage class: base class for wxBitmap, wxIcon, wxCursor
//              under MSW
// Author:      Vadim Zeitlin
// Modified by:
// Created:     20.11.99
// Copyright:   (c) 1999 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// NB: this is a private header, it is not intended to be directly included by
//     user code (but may be included from other, public, wxWin headers

#ifndef _WX_MSW_GDIIMAGE_H_
#define _WX_MSW_GDIIMAGE_H_

#include "wx/gdiobj.h"          // base class
#include "wx/gdicmn.h"          // wxBITMAP_TYPE_INVALID
#include "wx/list.h"

class WXDLLIMPEXP_FWD_CORE wxGDIImageRefData;
class WXDLLIMPEXP_FWD_CORE wxGDIImageHandler;
class WXDLLIMPEXP_FWD_CORE wxGDIImage;

WX_DECLARE_EXPORTED_LIST(wxGDIImageHandler, wxGDIImageHandlerList);

// ----------------------------------------------------------------------------
// wxGDIImageRefData: common data fields for all derived classes
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGDIImageRefData : public wxGDIRefData
{
public:
    wxGDIImageRefData()
    {
        m_width = m_height = m_depth = 0;

        m_scaleFactor = 1.0;

        m_handle = nullptr;
    }

    wxGDIImageRefData(const wxGDIImageRefData& data) : wxGDIRefData()
    {
        m_width = data.m_width;
        m_height = data.m_height;
        m_depth = data.m_depth;

        m_scaleFactor = data.m_scaleFactor;

        // can't copy handles like this, derived class copy ctor must do it!
        m_handle = nullptr;
    }

    // accessors
    virtual bool IsOk() const override { return m_handle != nullptr; }

    void SetSize(int w, int h) { m_width = w; m_height = h; }

    // free the resources we allocated
    virtual void Free() = 0;

    // for compatibility, the member fields are public

    // the size of the image
    int m_width, m_height;

    // the depth of the image
    int m_depth;

    // scale factor of the image
    double m_scaleFactor;

    // the handle to it
    union
    {
        WXHANDLE  m_handle;     // for untyped access
        WXHBITMAP m_hBitmap;
        WXHICON   m_hIcon;
        WXHCURSOR m_hCursor;
    };
};

// ----------------------------------------------------------------------------
// wxGDIImage: this class supports GDI image handlers which may be registered
// dynamically and will be used for loading/saving the images in the specified
// format. It also falls back to wxImage if no appropriate image is found.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGDIImage : public wxGDIObject
{
public:
    // handlers list interface
    static wxGDIImageHandlerList& GetHandlers() { return ms_handlers; }

    static void AddHandler(wxGDIImageHandler *handler);
    static void InsertHandler(wxGDIImageHandler *handler);
    static bool RemoveHandler(const wxString& name);

    static wxGDIImageHandler *FindHandler(const wxString& name);
    static wxGDIImageHandler *FindHandler(const wxString& extension, long type);
    static wxGDIImageHandler *FindHandler(long type);

    static void InitStandardHandlers();
    static void CleanUpHandlers();

    // access to the ref data casted to the right type
    wxGDIImageRefData *GetGDIImageData() const
        { return (wxGDIImageRefData *)m_refData; }

    // accessors
    WXHANDLE GetHandle() const
        { return IsNull() ? nullptr : GetGDIImageData()->m_handle; }
    void SetHandle(WXHANDLE handle)
        { AllocExclusive(); GetGDIImageData()->m_handle = handle; }

    int GetWidth() const { return IsNull() ? 0 : GetGDIImageData()->m_width; }
    int GetHeight() const { return IsNull() ? 0 : GetGDIImageData()->m_height; }
    int GetDepth() const { return IsNull() ? 0 : GetGDIImageData()->m_depth; }

    // allow setting and storing the scale factor
    void SetScaleFactor(double scale);
    double GetScaleFactor() const;

    // return the size divided by scale factor
    wxSize GetDIPSize() const;

    // logical metrics accessors return the same thing as physical ones, just
    // as in all the other ports without wxHAS_DPI_INDEPENDENT_PIXELS.
    double GetLogicalWidth() const;
    double GetLogicalHeight() const;
    wxSize GetLogicalSize() const;

    wxSize GetSize() const
    {
        return IsNull() ? wxSize(0,0) :
               wxSize(GetGDIImageData()->m_width, GetGDIImageData()->m_height);
    }

#if WXWIN_COMPATIBILITY_3_0
    wxDEPRECATED_INLINE(void SetWidth(int w), AllocExclusive(); GetGDIImageData()->m_width = w; )
    wxDEPRECATED_INLINE(void SetHeight(int h), AllocExclusive(); GetGDIImageData()->m_height = h; )
    wxDEPRECATED_INLINE(void SetDepth(int d), AllocExclusive(); GetGDIImageData()->m_depth = d; )

    wxDEPRECATED_INLINE(void SetSize(int w, int h), AllocExclusive(); GetGDIImageData()->SetSize(w, h); )
    wxDEPRECATED_INLINE(void SetSize(const wxSize& size), AllocExclusive(); GetGDIImageData()->SetSize(size.x, size.y); )
#endif // WXWIN_COMPATIBILITY_3_0

    // forward some of base class virtuals to wxGDIImageRefData
    bool FreeResource(bool force = false) override;
    virtual WXHANDLE GetResourceHandle() const override;

protected:
    // create the data for the derived class here
    virtual wxGDIImageRefData *CreateData() const = 0;

    // implement the wxGDIObject method in terms of our, more specific, one
    virtual wxGDIRefData *CreateGDIRefData() const override { return CreateData(); }

    // we can't [efficiently] clone objects of this class
    virtual wxGDIRefData *
    CloneGDIRefData(const wxGDIRefData *WXUNUSED(data)) const override
    {
        wxFAIL_MSG( wxT("must be implemented if used") );

        return nullptr;
    }

    static wxGDIImageHandlerList ms_handlers;
};

// ----------------------------------------------------------------------------
// wxGDIImageHandler: a class which knows how to load/save wxGDIImages.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGDIImageHandler : public wxObject
{
public:
    // ctor
    wxGDIImageHandler() { m_type = wxBITMAP_TYPE_INVALID; }
    wxGDIImageHandler(const wxString& name,
                      const wxString& ext,
                      wxBitmapType type)
        : m_name(name), m_extension(ext), m_type(type) { }

    // accessors
    void SetName(const wxString& name) { m_name = name; }
    void SetExtension(const wxString& ext) { m_extension = ext; }
    void SetType(wxBitmapType type) { m_type = type; }

    const wxString& GetName() const { return m_name; }
    const wxString& GetExtension() const { return m_extension; }
    wxBitmapType GetType() const { return m_type; }

    // real handler operations: to implement in derived classes
    virtual bool Create(wxGDIImage *image,
                        const void* data,
                        wxBitmapType flags,
                        int width, int height, int depth = 1) = 0;
    virtual bool Load(wxGDIImage *image,
                      const wxString& name,
                      wxBitmapType flags,
                      int desiredWidth, int desiredHeight) = 0;
    virtual bool Save(const wxGDIImage *image,
                      const wxString& name,
                      wxBitmapType type) const = 0;

protected:
    wxString  m_name;
    wxString  m_extension;
    wxBitmapType m_type;
};

#endif // _WX_MSW_GDIIMAGE_H_
