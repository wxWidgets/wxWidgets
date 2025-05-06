/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cursor.h
// Purpose:     wxCursor base header
// Author:      Julian Smart, Vadim Zeitlin
// Created:
// Copyright:   (c) Julian Smart
//              (c) 2014 Vadim Zeitlin (wxCursorBase)
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CURSOR_H_BASE_
#define _WX_CURSOR_H_BASE_

#include "wx/gdiobj.h"
#include "wx/gdicmn.h"

// Under most ports, wxCursor derives directly from wxGDIObject, but in wxMSW
// there is an intermediate wxGDIImage class.
#ifdef __WXMSW__
    #include "wx/msw/gdiimage.h"
#else
    typedef wxGDIObject wxGDIImage;
#endif

class WXDLLIMPEXP_FWD_CORE wxBitmapBundle;
class WXDLLIMPEXP_FWD_CORE wxWindow;

class WXDLLIMPEXP_CORE wxCursorBase : public wxGDIImage
{
public:
/*
    wxCursor classes should provide the following ctors:

    wxCursor();
    wxCursor(const wxImage& image);
    wxCursor(const wxBitmap& bitmap, const wxPoint& hotSpot);
    wxCursor(const wxBitmap& bitmap, int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(const wxString& name, wxBitmapType type, const wxPoint& hotSpot);
    wxCursor(const wxString& name,
             wxBitmapType type = wxCURSOR_DEFAULT_TYPE,
             int hotSpotX = 0, int hotSpotY = 0);
    wxCursor(wxStockCursor id) { InitFromStock(id); }
*/

    virtual wxPoint GetHotSpot() const { return wxDefaultPosition; }
};

#if defined(__WXMSW__)
    #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_CUR_RESOURCE
    #include "wx/msw/cursor.h"
#elif defined(__WXGTK__)
    #ifdef __WINDOWS__
        #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_CUR_RESOURCE
    #else
        #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_XPM
    #endif
    #include "wx/gtk/cursor.h"
#elif defined(__WXX11__)
    #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_XPM
    #include "wx/x11/cursor.h"
#elif defined(__WXDFB__)
    #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_CUR_RESOURCE
    #include "wx/dfb/cursor.h"
#elif defined(__WXMAC__)
    #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_MACCURSOR_RESOURCE
    #include "wx/osx/cursor.h"
#elif defined(__WXQT__)
    #ifdef __WINDOWS__
        #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_CUR_RESOURCE
    #else
        #define wxCURSOR_DEFAULT_TYPE   wxBITMAP_TYPE_CUR
    #endif
    #include "wx/qt/cursor.h"
#endif

// ----------------------------------------------------------------------------
// wxCursorBundle stores 1 or more versions of the same cursor, to be used at
// different DPI scaling levels.
// ----------------------------------------------------------------------------

class wxCursorBundleImpl;

class WXDLLIMPEXP_CORE wxCursorBundle
{
public:
    // Default ctor constructs an empty bundle which can't be used for
    // anything, but can be assigned something later.
    wxCursorBundle();

    // Create a cursor bundle from the given bitmap bundle.
    //
    // Note that the hotspot coordinates must be relative to the default size
    // of the bitmap bundle and are scaled for other sizes.
    explicit wxCursorBundle(const wxBitmapBundle& bitmaps,
                            const wxPoint& hotSpot);

    explicit wxCursorBundle(const wxBitmapBundle& bitmaps,
                            int hotSpotX = 0, int hotSpotY = 0)
        : wxCursorBundle(bitmaps, wxPoint(hotSpotX, hotSpotY))
    {
    }

    // This conversion ctor from a single cursor only exists for
    // interoperability with the existing code using wxCursor.
    wxCursorBundle(const wxCursor& cursor);

    // Default copy ctor and assignment operator and dtor would be ok, but need
    // to be defined out of line, where wxCursorBundleImpl is fully declared.

    wxCursorBundle(const wxCursorBundle& other);
    wxCursorBundle& operator=(const wxCursorBundle& other);

    ~wxCursorBundle();

    // Check if cursor bundle is non-empty.
    wxNODISCARD bool IsOk() const { return m_impl.get() != nullptr; }

    // Clear the bundle contents, IsOk() will return false after doing this.
    void Clear();

    // Get the cursor of the size suitable for the DPI used by the given window.
    wxNODISCARD wxCursor GetCursorFor(const wxWindow* window) const;

    // Check if two objects refer to the same bundle.
    wxNODISCARD bool IsSameAs(const wxCursorBundle& other) const
    {
        return m_impl == other.m_impl;
    }

private:
    using ImplPtr = wxObjectDataPtr<wxCursorBundleImpl>;

    // Private ctor used by static factory functions to create objects of this
    // class. It takes ownership of the pointer (which must be non-null).
    explicit wxCursorBundle(const ImplPtr& impl);

    ImplPtr m_impl;
};

// ----------------------------------------------------------------------------
// Other cursor-related stuff
// ----------------------------------------------------------------------------

#include "wx/utils.h"

/* This is a small class which can be used by all ports
   to temporarily suspend the busy cursor. Useful in modal
   dialogs.

   Actually that is not (any longer) quite true..  currently it is
   only used in wxGTK Dialog::ShowModal() and now uses static
   wxBusyCursor methods that are only implemented for wxGTK so far.
   The BusyCursor handling code should probably be implemented in
   common code somewhere instead of the separate implementations we
   currently have.  Also the name BusyCursorSuspender is a little
   misleading since it doesn't actually suspend the BusyCursor, just
   masks one that is already showing.
   If another call to wxBeginBusyCursor is made while this is active
   the Busy Cursor will again be shown.  But at least now it doesn't
   interfere with the state of wxIsBusy() -- RL

*/
class wxBusyCursorSuspender
{
public:
    wxBusyCursorSuspender()
    {
        if( wxIsBusy() )
        {
            wxSetCursor( wxBusyCursor::GetStoredCursor() );
        }
    }
    ~wxBusyCursorSuspender()
    {
        if( wxIsBusy() )
        {
            wxSetCursor( wxBusyCursor::GetBusyCursor() );
        }
    }
};
#endif
    // _WX_CURSOR_H_BASE_
