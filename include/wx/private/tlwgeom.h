///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/tlwgeom.h
// Purpose:     Declaration of platform-specific and private wxTLWGeometry.
// Author:      Vadim Zeitlin
// Created:     2018-04-29
// Copyright:   (c) 2018 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_TLWGEOM_H_
#define _WX_PRIVATE_TLWGEOM_H_

#include "wx/display.h"
#include "wx/toplevel.h"

#include "wx/private/rescale.h"

// Name of the entry storing the DPI at which the geometry was saved: it's the
// same as wxPERSIST_DPI in wx/persist.h, but avoid including it just for this.
#define wxPERSIST_TLW_DPI "DPI"

// ----------------------------------------------------------------------------
// wxTLWGeometryBase: abstract base class for platform-specific classes
// ----------------------------------------------------------------------------

// wxTLWGeometry contains full information about the window geometry, which may
// include things other than the obvious ones like its current position and
// size (e.g. under MSW it also stores the position of the maximized window,
// under GTK the size of non-client decorations etc). It is private to wx and
// is only used indirectly, via wxTopLevelWindow::SaveGeometry() and
// RestoreToGeometry() methods, in the public API.

class wxTLWGeometryBase
{
public:
    typedef wxTopLevelWindow::GeometryStore Store;

    wxTLWGeometryBase() = default;
    virtual ~wxTLWGeometryBase() = default;

    // Initialize from the given window.
    virtual bool GetFrom(const wxTopLevelWindow* tlw) = 0;

    // Resize the window to use this geometry.
    virtual bool ApplyTo(wxTopLevelWindow* tlw) = 0;

    // Serialize or deserialize the object by using the provided object for
    // writing/reading the values of the different fields of this object.
    virtual bool Save(Store& store) const = 0;
    virtual bool Restore(const Store& store) = 0;

protected:
#ifdef wxHAS_DPI_INDEPENDENT_PIXELS
    // The geometry doesn't change with the DPI under these platforms, so we
    // don't need to save DPI at all but still provide stubs for the functions
    // below to avoid having to use #ifdefs when calling them.

    bool SaveDPI(Store& WXUNUSED(store)) const
    {
        return true;
    }

    void RestoreDPI(const Store& WXUNUSED(store))
    {
    }

    void SetDPIFrom(const wxTopLevelWindow* WXUNUSED(tlw))
    {
    }

    wxSize RescaleSize(const wxSize& size, const wxSize& WXUNUSED(dpi)) const
    {
        return size;
    }
#else // !wxHAS_DPI_INDEPENDENT_PIXELS
    // Helpers for the derived classes Save() and Restore() implementations:
    // they must be used to save and restore the DPI at which the geometry was
    // taken, as it is needed by RescaleSize() below.
    bool SaveDPI(Store& store) const
    {
        wxASSERT_MSG( m_dpi != wxDefaultSize, "forgot to call SetDPIFrom()?" );

        return store.SaveValue(wxPERSIST_TLW_DPI, m_dpi.y);
    }

    void RestoreDPI(const Store& store)
    {
        int dpi;
        if ( store.RestoreValue(wxPERSIST_TLW_DPI, &dpi) )
        {
            if ( dpi <= 0 )
            {
                // Probably not worth logging this, just ignore.
                return;
            }

            m_dpi = wxSize(dpi, dpi);
        }
        //else: leave it as wxDefaultSize, which means that we don't know the
        //      DPI at which the geometry was saved and so shouldn't rescale it.
    }

    // Return the size to use for a window which had the given size when its
    // geometry was saved if it is going to be used with the given DPI now.
    //
    // Note that this function only deals with the size because the position of
    // the window doesn't need to be changed, screen resolution doesn't depend
    // on the DPI on the platforms without DPI-independent pixels, where this
    // really matters.
    wxSize RescaleSize(const wxSize& size, const wxSize& dpi) const
    {
        if ( m_dpi == wxDefaultSize || dpi == m_dpi || !dpi.x || !dpi.y )
            return size;

        return wxRescaleCoord(size).From(m_dpi).To(dpi);
    }

    // Set the DPI to save together with the geometry of the given window, must
    // be called from GetFrom().
    void SetDPIFrom(const wxTopLevelWindow* tlw)
    {
        m_dpi = tlw->GetDPI();
    }

private:
    // The DPI at which the geometry was saved or wxDefaultSize if it is not
    // known, which is the case when restoring the geometry saved by a previous
    // version of the library which didn't store it.
    wxSize m_dpi = wxDefaultSize;
#endif // wxHAS_DPI_INDEPENDENT_PIXELS
};

// ----------------------------------------------------------------------------
// wxTLWGeometryGeneric: simplest possible generic implementation
// ----------------------------------------------------------------------------

// names for various persistent options
#define wxPERSIST_TLW_X "x"
#define wxPERSIST_TLW_Y "y"
#define wxPERSIST_TLW_W "w"
#define wxPERSIST_TLW_H "h"

#define wxPERSIST_TLW_MAXIMIZED "Maximized"
#define wxPERSIST_TLW_ICONIZED "Iconized"

// MSW has its own native implementation and doesn't use this class.
#ifndef __WXMSW__

class wxTLWGeometryGeneric : public wxTLWGeometryBase
{
public:
    wxTLWGeometryGeneric()
    {
        m_hasPos =
        m_hasSize =
        m_iconized =
        m_maximized = false;
    }

    virtual bool Save(Store& store) const override
    {
        if ( !store.SaveValue(wxPERSIST_TLW_X, m_rectScreen.x) ||
             !store.SaveValue(wxPERSIST_TLW_Y, m_rectScreen.y) )
            return false;

        if ( !store.SaveValue(wxPERSIST_TLW_W, m_rectScreen.width) ||
             !store.SaveValue(wxPERSIST_TLW_H, m_rectScreen.height) )
            return false;

        if ( !store.SaveValue(wxPERSIST_TLW_MAXIMIZED, m_maximized) )
            return false;

        if ( !store.SaveValue(wxPERSIST_TLW_ICONIZED, m_iconized) )
            return false;

        if ( !SaveDPI(store) )
            return false;

        return true;
    }

    virtual bool Restore(const Store& store) override
    {
        m_hasPos = store.RestoreValue(wxPERSIST_TLW_X, &m_rectScreen.x) &&
                   store.RestoreValue(wxPERSIST_TLW_Y, &m_rectScreen.y);

        m_hasSize = store.RestoreValue(wxPERSIST_TLW_W, &m_rectScreen.width) &&
                    store.RestoreValue(wxPERSIST_TLW_H, &m_rectScreen.height);

        int tmp;
        if ( store.RestoreValue(wxPERSIST_TLW_MAXIMIZED, &tmp) )
            m_maximized = tmp != 0;

        if ( store.RestoreValue(wxPERSIST_TLW_ICONIZED, &tmp) )
            m_iconized = tmp != 0;

        RestoreDPI(store);

        // If we restored at least something, return true.
        return m_hasPos || m_hasSize || m_maximized || m_iconized;
    }

    virtual bool GetFrom(const wxTopLevelWindow* tlw) override
    {
        m_rectScreen = tlw->GetScreenRect();
        SetDPIFrom(tlw);
        m_hasPos =
        m_hasSize = true;
        m_iconized = tlw->IsIconized();
        m_maximized = tlw->IsMaximized();

        return true;
    }

    virtual bool ApplyTo(wxTopLevelWindow* tlw) override
    {
        if ( m_hasPos )
        {
            // to avoid making the window completely invisible if it had been
            // shown on a monitor which was disconnected since the last run
            // (this is pretty common for notebook with external displays)
            //
            // NB: we should allow window position to be (slightly) off screen,
            //     it's not uncommon to position the window so that its upper
            //     left corner has slightly negative coordinate
            if ( wxDisplay::GetFromPoint(m_rectScreen.GetTopLeft()) != wxNOT_FOUND ||
                 (m_hasSize &&
                  wxDisplay::GetFromPoint(m_rectScreen.GetBottomRight()) != wxNOT_FOUND) )
            {
                tlw->Move(m_rectScreen.GetTopLeft(), wxSIZE_ALLOW_MINUS_ONE);
            }
            //else: should we try to adjust position/size somehow?
        }

        if ( m_hasSize )
        {
            // Adjust the size to the currently used DPI if necessary.
            wxSize size = RescaleSize(m_rectScreen.GetSize(), tlw->GetDPI());

            // a previous version of the program could have saved the window
            // size which used to be big enough, but which is not big enough
            // any more for the new version, so check that the size we restore
            // doesn't cut off parts of the window
            size.IncTo(tlw->GetBestSize());
            tlw->SetSize(size);
        }

        // note that the window can be both maximized and iconized
        if ( m_maximized )
            tlw->Maximize();

        if ( m_iconized )
            tlw->Iconize();

        return true;
    }

private:
    wxRect m_rectScreen;
    bool m_hasPos;
    bool m_hasSize;
    bool m_iconized;
    bool m_maximized;
};

#endif // !__WXMSW__

#ifdef __WXGTK__
    #include "wx/gtk/private/tlwgeom.h"
#elif defined(__WXMSW__)
    #include "wx/msw/private/tlwgeom.h"
#else
    class wxTLWGeometry : public wxTLWGeometryGeneric
    {
    };
#endif

#endif // _WX_PRIVATE_TLWGEOM_H_
