/////////////////////////////////////////////////////////////////////////////
// Name:        wx/buttonbox.h
// Purpose:     wxButtonBox - a row of button-like segments
// Author:      Wlodzimierz ABX Skiba, Assisted-by: Claude Sonnet 5.5
// Created:     12.11.26
// Copyright:   (c) 2026 Wlodzimierz ABX Skiba
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_BUTTONBOX_H_BASE_
#define _WX_BUTTONBOX_H_BASE_

#include "wx/defs.h"

#if wxUSE_BUTTONBOX

#include "wx/control.h"
#include "wx/bmpbndl.h"

// ----------------------------------------------------------------------------
// styles (mirror NSSegmentSwitchTracking)
// ----------------------------------------------------------------------------

#define wxBB_SELECT_ONE   0x0000   // exactly one segment can be selected (radio-like)
#define wxBB_SELECT_ANY   0x0001   // every segment toggles independently
#define wxBB_MOMENTARY    0x0002   // segments only send events, never stay selected
#define wxBB_MODE_MASK    0x0003

#define wxBB_DEFAULT_STYLE wxBB_SELECT_ONE

extern WXDLLIMPEXP_DATA_CORE(const char) wxButtonBoxNameStr[];

// ----------------------------------------------------------------------------
// wxButtonBoxBase: common (abstract) interface
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxButtonBoxBase : public wxControl
{
public:
    // --- segments -----------------------------------------------------------
    virtual size_t GetCount() const = 0;
    virtual void Insert(size_t pos, const wxString& label,
                        const wxBitmapBundle& bitmap = wxBitmapBundle()) = 0;
    virtual void Delete(size_t pos) = 0;
    virtual void Clear() = 0;

    void Append(const wxString& label,
                const wxBitmapBundle& bitmap = wxBitmapBundle())
        { Insert(GetCount(), label, bitmap); }

    // --- selection ----------------------------------------------------------
    // wxNOT_FOUND if nothing is selected. With wxBB_SELECT_ANY returns the
    // most recently changed selected segment; use IsSelected() for the rest.
    virtual int  GetSelection() const = 0;
    virtual void SetSelection(int n) = 0; // wxNOT_FOUND clears
    virtual bool IsSelected(size_t n) const = 0;
    virtual void SetSelected(size_t n, bool selected = true) = 0;

    // --- per-segment attributes -----
    virtual void SetItemLabel(size_t n, const wxString& label) = 0;
    virtual wxString GetItemLabel(size_t n) const = 0;

    virtual void SetItemBitmap(size_t n, const wxBitmapBundle& bitmap) = 0;
    virtual wxBitmapBundle GetItemBitmap(size_t n) const = 0;

    // width in pixels, wxDefaultCoord (-1) means automatic
    virtual void SetItemWidth(size_t n, int width) = 0;
    virtual int  GetItemWidth(size_t n) const = 0;

    virtual void EnableItem(size_t n, bool enable = true) = 0;
    virtual bool IsItemEnabled(size_t n) const = 0;

    virtual void SetItemToolTip(size_t n, const wxString& tip) = 0;
    virtual wxString GetItemToolTip(size_t n) const = 0;

protected:
    // Sends wxEVT_BUTTONBOX: GetInt() is the segment index,
    // GetExtraLong() is 1 if the segment is now selected, 0 otherwise.
    void SendButtonBoxEvent(int index, bool selected);
};

// ----------------------------------------------------------------------------
// event
// ----------------------------------------------------------------------------

wxDECLARE_EXPORTED_EVENT(WXDLLIMPEXP_CORE, wxEVT_BUTTONBOX, wxCommandEvent);

#define EVT_BUTTONBOX(id, fn) \
    wx__DECLARE_EVT1(wxEVT_BUTTONBOX, id, wxCommandEventHandler(fn))

// ----------------------------------------------------------------------------
// port dispatch
// ----------------------------------------------------------------------------

#if defined(__WXOSX_COCOA__)
    #define wxHAS_NATIVE_BUTTONBOX
    #include "wx/osx/buttonbox.h"
#else
    #include "wx/generic/buttonbox.h"
    #define wxButtonBox wxGenericButtonBox
#endif

#endif // wxUSE_BUTTONBOX

#endif // _WX_BUTTONBOX_H_BASE_
