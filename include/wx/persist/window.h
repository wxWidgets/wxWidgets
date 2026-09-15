///////////////////////////////////////////////////////////////////////////////
// Name:        wx/persist/window.h
// Purpose:     wxPersistentWindow declaration
// Author:      Vadim Zeitlin
// Created:     2009-01-23
// Copyright:   (c) 2009 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PERSIST_WINDOW_H_
#define _WX_PERSIST_WINDOW_H_

#include "wx/persist.h"

#include "wx/window.h"

// ----------------------------------------------------------------------------
// wxPersistentWindow: base class for persistent windows, uses the window name
//                     as persistent name by default and automatically reacts
//                     to the window destruction
// ----------------------------------------------------------------------------

// type-independent part of wxPersistentWindow
class wxPersistentWindowBase : public wxPersistentObject
{
public:
    wxPersistentWindowBase(wxWindow *win)
        : wxPersistentObject(win)
    {
        win->Bind(wxEVT_DESTROY, &wxPersistentWindowBase::HandleDestroy, this);
    }

    virtual wxString GetName() const override
    {
        const wxString name = GetWindow()->GetName();
        wxASSERT_MSG( !name.empty(), "persistent windows should be named!" );

        return name;
    }

protected:
    wxWindow *GetWindow() const { return static_cast<wxWindow *>(GetObject()); }

    // Save a coordinate, i.e. either a position or a size, in pixels.
    //
    // Unlike SaveValue(), this function also saves the DPI at which the
    // coordinate was computed, which allows RestoreCoord() to rescale it if
    // the DPI changes in the meanwhile.
    //
    // The special values which are not really coordinates, such as -1, may be
    // passed to this function too, they're just saved as is.
    bool SaveCoord(const wxString& name, int value) const
    {
        if ( !SaveValue(name, value) )
            return false;

        // Under the platforms using DPI-independent pixels the values don't
        // change with the DPI, so we don't need to store it at all.
#ifndef wxHAS_DPI_INDEPENDENT_PIXELS
        // Don't store the DPI if we haven't saved anything depending on it.
        if ( value > 0 )
        {
            // All the values of the same object are saved at the same DPI, so
            // write it just once, using the same key for all of them, but do
            // write it again if it has somehow changed since the last time.
            const int dpi = GetWindow()->GetDPI().y;
            if ( dpi != m_dpiSaved )
            {
                if ( !SaveValue(wxASCII_STR(wxPERSIST_DPI), dpi) )
                    return false;

                m_dpiSaved = dpi;
            }
        }
#endif // !wxHAS_DPI_INDEPENDENT_PIXELS

        return true;
    }

    // Restore a coordinate saved by SaveCoord(), rescaling it if the DPI has
    // changed since it had been saved.
    bool RestoreCoord(const wxString& name, int* value)
    {
        if ( !RestoreValue(name, value) )
            return false;

#ifndef wxHAS_DPI_INDEPENDENT_PIXELS
        // Only the values really expressed in pixels can be rescaled, the
        // special ones, such as -1, must be preserved as is.
        if ( *value <= 0 )
            return true;

        int dpiSaved;
        if ( !RestoreValue(wxASCII_STR(wxPERSIST_DPI), &dpiSaved) )
        {
            // The value was saved by a version of the library which didn't
            // store the DPI, so we have no choice but to use it as is.
            return true;
        }

        if ( dpiSaved <= 0 )
        {
            // Don't use invalid value, just ignore the saved DPI in this case.
            return true;
        }

        const int dpi = GetWindow()->GetDPI().y;
        if ( dpi != dpiSaved )
            *value = wxRound(*value*static_cast<double>(dpi)/dpiSaved);
#endif // !wxHAS_DPI_INDEPENDENT_PIXELS

        return true;
    }

private:
    void HandleDestroy(wxWindowDestroyEvent& event)
    {
        event.Skip();

        // only react to the destruction of this object itself, not of any of
        // its children
        if ( event.GetEventObject() == GetObject() )
        {
            // this will delete this object itself
            wxPersistenceManager::Get().SaveAndUnregister(GetWindow());
        }
    }

#ifndef wxHAS_DPI_INDEPENDENT_PIXELS
    // The last DPI written by SaveCoord() or 0 if none yet.
    mutable int m_dpiSaved = 0;
#endif // !wxHAS_DPI_INDEPENDENT_PIXELS

    wxDECLARE_NO_COPY_CLASS(wxPersistentWindowBase);
};

template <class T>
class wxPersistentWindow : public wxPersistentWindowBase
{
public:
    typedef T WindowType;

    wxPersistentWindow(WindowType *win)
        : wxPersistentWindowBase(win)
    {
    }

    WindowType *Get() const { return static_cast<WindowType *>(GetWindow()); }
};

#endif // _WX_PERSIST_WINDOW_H_

