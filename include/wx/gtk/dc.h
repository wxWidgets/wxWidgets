/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/dc.h
// Purpose:
// Author:      Robert Roebling
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTKDC_H_
#define _WX_GTKDC_H_

#ifdef __WXGTK3__

#include "wx/dcgraph.h"

class wxGTKCairoDCImpl: public wxGCDCImpl
{
    typedef wxGCDCImpl BaseType;
public:
    wxGTKCairoDCImpl(wxDC* owner);
    wxGTKCairoDCImpl(wxDC* owner, wxWindow* window, wxLayoutDirection dir = wxLayout_Default, int width = 0);

    virtual void DoDrawBitmap(const wxBitmap& bitmap, int x, int y, bool useMask) override;
    virtual void DoDrawCheckMark(int x, int y, int width, int height) override;
    virtual void DoDrawIcon(const wxIcon& icon, int x, int y) override;
    virtual void DoDrawText(const wxString& text, int x, int y) override;
    virtual void DoDrawRotatedText(const wxString& text, int x, int y, double angle) override;
#if wxUSE_IMAGE
    virtual bool DoFloodFill(int x, int y, const wxColour& col, wxFloodFillStyle style) override;
#endif
    virtual wxBitmap DoGetAsBitmap(const wxRect* subrect) const override;
    virtual bool DoGetPixel(int x, int y, wxColour* col) const override;
    virtual void DoGetSize(int* width, int* height) const override;
    virtual bool DoStretchBlit(int xdest, int ydest, int dstWidth, int dstHeight, wxDC* source, int xsrc, int ysrc, int srcWidth, int srcHeight, wxRasterOperationMode rop, bool useMask, int xsrcMask, int ysrcMask) override;
    virtual void* GetCairoContext() const override;

    virtual wxSize GetPPI() const override;
    virtual void SetLayoutDirection(wxLayoutDirection dir) override;
    virtual wxLayoutDirection GetLayoutDirection() const override;

protected:
    // Set m_size from the given (valid) GdkWindow.
    void InitSize(GdkWindow* window);
    void AdjustForRTL(cairo_t* cr);

    wxSize m_size;
    wxLayoutDirection m_layoutDir;

    wxDECLARE_NO_COPY_CLASS(wxGTKCairoDCImpl);
};
//-----------------------------------------------------------------------------

class wxWindowDCImpl: public wxGTKCairoDCImpl
{
public:
    wxWindowDCImpl(wxWindowDC* owner, wxWindow* window);

    wxDECLARE_NO_COPY_CLASS(wxWindowDCImpl);
};
//-----------------------------------------------------------------------------

class wxClientDCImpl: public wxGTKCairoDCImpl
{
public:
    wxClientDCImpl(wxClientDC* owner, wxWindow* window);

    static bool CanBeUsedForDrawing(const wxWindow* window);

    wxDECLARE_NO_COPY_CLASS(wxClientDCImpl);
};
//-----------------------------------------------------------------------------

class wxPaintDCImpl: public wxGTKCairoDCImpl
{
public:
    wxPaintDCImpl(wxPaintDC* owner, wxWindow* window);

    wxDECLARE_NO_COPY_CLASS(wxPaintDCImpl);
};
//-----------------------------------------------------------------------------

class wxScreenDCImpl: public wxGTKCairoDCImpl
{
public:
    wxScreenDCImpl(wxScreenDC* owner);

    virtual wxSize GetPPI() const override;

    wxDECLARE_NO_COPY_CLASS(wxScreenDCImpl);
};
//-----------------------------------------------------------------------------

class wxMemoryDCImpl: public wxGTKCairoDCImpl
{
public:
    wxMemoryDCImpl(wxMemoryDC* owner);
    wxMemoryDCImpl(wxMemoryDC* owner, wxBitmap& bitmap);
    wxMemoryDCImpl(wxMemoryDC* owner, wxDC* dc);
    virtual wxBitmap DoGetAsBitmap(const wxRect* subrect) const override;
    virtual void DoSelect(const wxBitmap& bitmap) override;
    virtual const wxBitmap& GetSelectedBitmap() const override;
    virtual wxBitmap& GetSelectedBitmap() override;

private:
    void Setup();
    wxBitmap m_bitmap;

    wxDECLARE_NO_COPY_CLASS(wxMemoryDCImpl);
};
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGTKCairoDC: public wxDC
{
public:
    wxGTKCairoDC(cairo_t* cr, wxWindow* window, wxLayoutDirection dir = wxLayout_LeftToRight, int width = 0);

    wxDECLARE_NO_COPY_CLASS(wxGTKCairoDC);
};

#else

#include "wx/dc.h"

//-----------------------------------------------------------------------------
// wxDC
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxGTKDCImpl : public wxDCImpl
{
public:
    wxGTKDCImpl( wxDC *owner );
    virtual ~wxGTKDCImpl();

#if wxUSE_PALETTE
    void SetColourMap( const wxPalette& palette ) { SetPalette(palette); }
#endif // wxUSE_PALETTE

    // Resolution in pixels per logical inch
    virtual wxSize GetPPI() const override;

    virtual bool StartDoc( const wxString& WXUNUSED(message) ) override { return true; }
    virtual void EndDoc() override { }
    virtual void StartPage() override { }
    virtual void EndPage() override { }

    virtual GdkWindow* GetGDKWindow() const { return nullptr; }
    virtual void* GetHandle() const override { return GetGDKWindow(); }

    // base class pure virtuals implemented here
    virtual void DoSetClippingRegion(wxCoord x, wxCoord y, wxCoord width, wxCoord height) override;
    virtual void DoGetSizeMM(int* width, int* height) const override;

    wxDECLARE_ABSTRACT_CLASS(wxGTKDCImpl);
};

// this must be defined when wxDC::Blit() honours the DC origin and needed to
// allow wxUniv code in univ/winuniv.cpp to work with versions of wxGTK
// 2.3.[23]
#ifndef wxHAS_WORKING_GTK_DC_BLIT
    #define wxHAS_WORKING_GTK_DC_BLIT
#endif

#endif
#endif // _WX_GTKDC_H_
