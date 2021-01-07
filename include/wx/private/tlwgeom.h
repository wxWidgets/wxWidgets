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
    typedef wxTopLevelWindow::GeometrySerializer Serializer;

    wxTLWGeometryBase() {}
    virtual ~wxTLWGeometryBase() {}

    // Initialize from the given window.
    virtual bool GetFrom(const wxTopLevelWindow* tlw) = 0;

    // Resize the window to use this geometry.
    virtual bool ApplyTo(wxTopLevelWindow* tlw) = 0;

    // Serialize or deserialize the object by using the provided object for
    // writing/reading the values of the different fields of this object.
    virtual bool Save(const Serializer& ser) const = 0;
    virtual bool Restore(Serializer& ser) = 0;
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

    virtual bool Save(const Serializer& ser) const wxOVERRIDE
    {
        if ( !ser.SaveField(wxPERSIST_TLW_X, m_rectScreen.x) ||
             !ser.SaveField(wxPERSIST_TLW_Y, m_rectScreen.y) )
            return false;

        if ( !ser.SaveField(wxPERSIST_TLW_W, m_rectScreen.width) ||
             !ser.SaveField(wxPERSIST_TLW_H, m_rectScreen.height) )
            return false;

        if ( !ser.SaveField(wxPERSIST_TLW_MAXIMIZED, m_maximized) )
            return false;

        if ( !ser.SaveField(wxPERSIST_TLW_ICONIZED, m_iconized) )
            return false;

        return true;
    }

    virtual bool Restore(Serializer& ser) wxOVERRIDE
    {
        m_hasPos = ser.RestoreField(wxPERSIST_TLW_X, &m_rectScreen.x) &&
                   ser.RestoreField(wxPERSIST_TLW_Y, &m_rectScreen.y);

        m_hasSize = ser.RestoreField(wxPERSIST_TLW_W, &m_rectScreen.width) &&
                    ser.RestoreField(wxPERSIST_TLW_H, &m_rectScreen.height);

        int tmp;
        if ( ser.RestoreField(wxPERSIST_TLW_MAXIMIZED, &tmp) )
            m_maximized = tmp != 0;

        if ( ser.RestoreField(wxPERSIST_TLW_ICONIZED, &tmp) )
            m_iconized = tmp != 0;

        // If we restored at least something, return true.
        return m_hasPos || m_hasSize || m_maximized || m_iconized;
    }

    virtual bool GetFrom(const wxTopLevelWindow* tlw) wxOVERRIDE
    {
        m_rectScreen = tlw->GetScreenRect();
        m_hasPos =
        m_hasSize = true;
        m_iconized = tlw->IsIconized();
        m_maximized = tlw->IsMaximized();

        return true;
    }

    virtual bool ApplyTo(wxTopLevelWindow* tlw) wxOVERRIDE
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
            // a previous version of the program could have saved the window
            // size which used to be big enough, but which is not big enough
            // any more for the new version, so check that the size we restore
            // doesn't cut off parts of the window
            wxSize size = m_rectScreen.GetSize();
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

#ifdef __WXGTK20__
    #include "wx/gtk/private/tlwgeom.h"
#elif defined(__WXMSW__)
    #include "wx/msw/private/tlwgeom.h"
#else
    class wxTLWGeometry : public wxTLWGeometryGeneric
    {
    };
#endif

#endif // _WX_PRIVATE_TLWGEOM_H_
