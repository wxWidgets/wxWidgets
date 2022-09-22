///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/cairo.h
// Purpose:     Cairo-related helpers
// Author:      Vadim Zeitlin
// Created:     2022-09-23
// Copyright:   (c) 2022 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_CAIRO_H_
#define _WX_GTK_PRIVATE_CAIRO_H_

// ----------------------------------------------------------------------------
// RAII helper creating a Cairo context in ctor and destroying it in dtor
// ----------------------------------------------------------------------------

namespace wxGTKImpl
{

class CairoContext
{
public:
    explicit CairoContext(GdkWindow* window)
        : m_cr(gdk_cairo_create(window))
    {
    }

    ~CairoContext()
    {
        cairo_destroy(m_cr);
    }

    operator cairo_t*() const { return m_cr; }

private:
    cairo_t* const m_cr;

    wxDECLARE_NO_COPY_CLASS(CairoContext);
};

} // namespace wxGTKImpl

#endif // _WX_GTK_PRIVATE_CAIRO_H_
