/////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/display.h
// Purpose:     wxDisplayImpl class declaration
// Author:      Vadim Zeitlin
// Created:     2006-03-15
// Copyright:   (c) 2002-2006 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_DISPLAY_H_
#define _WX_PRIVATE_DISPLAY_H_

#include "wx/display.h"
#include "wx/gdicmn.h"      // for wxRect
#include "wx/vector.h"

// ----------------------------------------------------------------------------
// wxDisplayFactory: allows to create wxDisplay objects
// ----------------------------------------------------------------------------

class wxDisplayFactory
{
public:
    wxDisplayFactory() = default;
    virtual ~wxDisplayFactory() { ClearImpls(); }

    // Create the display if necessary using CreateDisplay(), otherwise just
    // get it from cache.
    wxObjectDataPtr<wxDisplayImpl> GetDisplay(unsigned n);

    // Return the primary display object, creating it if necessary.
    wxObjectDataPtr<wxDisplayImpl> GetPrimaryDisplay();

    // get the total number of displays
    virtual unsigned GetCount() = 0;

    // return the display for the given point or wxNOT_FOUND
    virtual int GetFromPoint(const wxPoint& pt) = 0;

    // return the display with biggest intersection with the given rectangle or
    // wxNOT_FOUND
    virtual int GetFromRect(const wxRect& rect);

    // return the display for the given window or wxNOT_FOUND
    //
    // the window pointer must not be null (i.e. caller should check it)
    virtual int GetFromWindow(const wxWindow *window);

    // Trigger recreation of wxDisplayImpl when they're needed the next time.
    virtual void UpdateOnDisplayChange();

protected:
    // create a new display object
    //
    // it can return a null pointer if the display creation failed
    virtual wxDisplayImpl *CreateDisplay(unsigned n) = 0;

    // check if the given display is still connected and update its properties
    // if this is the case (notably its index)
    //
    // mark display as disconnected and return false otherwise
    //
    // default implementation considers all displays as being not connected any
    // longer, port-specific implementations should override it to keep the
    // objects corresponding to the still connected displays valid
    virtual bool RefreshOnDisplayChange(wxDisplayImpl& impl) const;

private:
    // Delete all the elements of m_impls vector and clear it.
    void ClearImpls();

    // On-demand populated vector of wxDisplayImpl objects.
    wxVector<wxObjectDataPtr<wxDisplayImpl>> m_impls;

    wxDECLARE_NO_COPY_CLASS(wxDisplayFactory);
};

// ----------------------------------------------------------------------------
// wxDisplayImpl: base class for all wxDisplay implementations
// ----------------------------------------------------------------------------

class wxDisplayImpl : public wxObjectRefData
{
public:
    // virtual dtor for this base class
    virtual ~wxDisplayImpl() = default;


    // return the full area of this display
    virtual wxRect GetGeometry() const = 0;

    // return the area of the display available for normal windows
    virtual wxRect GetClientArea() const { return GetGeometry(); }

    // return the depth or 0 if unknown
    virtual int GetDepth() const = 0;

    // return the scale factor used to convert logical pixels to physical ones
    virtual double GetScaleFactor() const { return 1.0; }

    // return the resolution of the display, by default uses GetScaleFactor(),
    // but can be also overridden directly, as is done in wxMSW
    virtual wxSize GetPPI() const { return wxDisplay::GetStdPPI()*GetScaleFactor(); }

    // return the name (may be empty)
    virtual wxString GetName() const { return wxString(); }

    // return the index of this display
    unsigned GetIndex() const { return m_index; }

    // return true if this is the primary monitor (usually one with index 0)
    virtual bool IsPrimary() const { return GetIndex() == 0; }


#if wxUSE_DISPLAY
    // implements wxDisplay::GetModes()
    virtual wxArrayVideoModes GetModes(const wxVideoMode& mode) const = 0;

    // get current video mode
    virtual wxVideoMode GetCurrentMode() const = 0;

    // change current mode, return true if succeeded, false otherwise
    virtual bool ChangeMode(const wxVideoMode& mode) = 0;
#endif // wxUSE_DISPLAY

    // return true, if this display is still connected physically to system
    bool IsConnected() const { return m_isConnected; }

    // indicate that this display is not connected to system anymore
    void Disconnect() { m_isConnected = false; }

protected:
    // create the object providing access to the display with the given index
    wxDisplayImpl(unsigned n) : m_index(n) { }


    // the index of this display (0 is always the primary one)
    unsigned m_index;

    // true, if this display is still connected physically to system
    bool m_isConnected = true;

    friend class wxDisplayFactory;

    wxDECLARE_NO_COPY_CLASS(wxDisplayImpl);
};

// ----------------------------------------------------------------------------
// wxDisplayImplSingle: the simplest possible impl for the main display only
// ----------------------------------------------------------------------------

// Note that this is still an ABC and GetGeometry() and GetClientArea() methods
// must be implemented in the derived classes.

class WXDLLEXPORT wxDisplayImplSingle : public wxDisplayImpl
{
public:
    wxDisplayImplSingle() : wxDisplayImpl(0) { }

#if wxUSE_DISPLAY
    // no video modes support for us, provide just the stubs
    virtual wxArrayVideoModes
    GetModes(const wxVideoMode& WXUNUSED(mode)) const override
    {
        return wxArrayVideoModes();
    }

    virtual wxVideoMode GetCurrentMode() const override
    {
        return wxVideoMode();
    }

    virtual bool ChangeMode(const wxVideoMode& WXUNUSED(mode)) override
    {
        return false;
    }
#endif // wxUSE_DISPLAY

    wxDECLARE_NO_COPY_CLASS(wxDisplayImplSingle);
};

// ----------------------------------------------------------------------------
// wxDisplayFactorySingle
// ----------------------------------------------------------------------------

// This is the simplest implementation of wxDisplayFactory using single/main
// display only. It is used when wxUSE_DISPLAY == 0 because getting the size of
// the main display is always needed.
//
// Note that this is still an ABC and derived classes must implement
// CreateSingleDisplay().

class wxDisplayFactorySingle : public wxDisplayFactory
{
public:
    virtual unsigned GetCount() override { return 1; }
    virtual int GetFromPoint(const wxPoint& pt) override;

protected:
    virtual wxDisplayImpl *CreateDisplay(unsigned n) override;

    virtual wxDisplayImpl *CreateSingleDisplay() = 0;
};

#endif // _WX_PRIVATE_DISPLAY_H_
