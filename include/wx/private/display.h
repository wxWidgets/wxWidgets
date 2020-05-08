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
    wxDisplayFactory() { }
    virtual ~wxDisplayFactory() { ClearImpls(); }

    // Create the display if necessary using CreateDisplay(), otherwise just
    // get it from cache.
    wxDisplayImpl* GetDisplay(unsigned n)
    {
        // Normally, m_impls should be cleared if the number of displays in the
        // system changes because InvalidateCache() must be called. However in
        // some ports (e.g. Mac right now, see #18318), cache invalidation never
        // happens, so we can end up with m_impls size being out of sync with
        // the actual number of monitors. Compensate for this here by checking
        // if the index is invalid and invalidating the cache at least in this
        // case.
        //
        // Note that this is still incorrect because we continue using outdated
        // information if the first monitor is disconnected, for example. The
        // only real solution is to ensure that InvalidateCache() is called,
        // but for now this at least avoids crashes when a new display is
        // connected.
        if ( n >= m_impls.size() )
        {
            // This strange two-step resize is done to clear all the existing
            // elements: they may not be valid any longer if the number of
            // displays has changed.
            m_impls.resize(0);
            m_impls.resize(GetCount());
        }
        else if ( m_impls[n] )
        {
            // Just return the existing display if we have it.
            return m_impls[n];
        }

        m_impls[n] = CreateDisplay(n);
        return m_impls[n];
    }

    // Return the primary display object, creating it if necessary.
    wxDisplayImpl* GetPrimaryDisplay();

    // get the total number of displays
    virtual unsigned GetCount() = 0;

    // return the display for the given point or wxNOT_FOUND
    virtual int GetFromPoint(const wxPoint& pt) = 0;

    // return the display for the given window or wxNOT_FOUND
    //
    // the window pointer must not be NULL (i.e. caller should check it)
    virtual int GetFromWindow(const wxWindow *window);

    // Trigger recreation of wxDisplayImpl when they're needed the next time.
    virtual void InvalidateCache() { ClearImpls(); }

protected:
    // create a new display object
    //
    // it can return a NULL pointer if the display creation failed
    virtual wxDisplayImpl *CreateDisplay(unsigned n) = 0;

private:
    // Delete all the elements of m_impls vector and clear it.
    void ClearImpls();

    // On-demand populated vector of wxDisplayImpl objects.
    wxVector<wxDisplayImpl*> m_impls;

    wxDECLARE_NO_COPY_CLASS(wxDisplayFactory);
};

// ----------------------------------------------------------------------------
// wxDisplayImpl: base class for all wxDisplay implementations
// ----------------------------------------------------------------------------

class wxDisplayImpl
{
public:
    // virtual dtor for this base class
    virtual ~wxDisplayImpl() { }


    // return the full area of this display
    virtual wxRect GetGeometry() const = 0;

    // return the area of the display available for normal windows
    virtual wxRect GetClientArea() const { return GetGeometry(); }

    // return the depth or 0 if unknown
    virtual int GetDepth() const = 0;

    // return the scale factor used to convert logical pixels to physical ones
    virtual double GetScaleFactor() const { return 1.0; }

    // return the resolution of the display, uses GetSize(), GetScaleFactor()
    // and GetSizeMM() by default but can be also overridden directly
    virtual wxSize GetPPI() const;

    // return the physical size of the display or (0, 0) if unknown: this is
    // only used by GetPPI() implementation in the base class, so if GetPPI()
    // is overridden, this one doesn't have to be implemented
    virtual wxSize GetSizeMM() const { return wxSize(0, 0); }

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

protected:
    // create the object providing access to the display with the given index
    wxDisplayImpl(unsigned n) : m_index(n) { }

    // Compute PPI from the sizes in pixels and mm.
    //
    // Return (0, 0) if physical size (in mm) is not known, i.e. 0.
    static wxSize ComputePPI(int pxX, int pxY, int mmX, int mmY);


    // the index of this display (0 is always the primary one)
    const unsigned m_index;


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
    GetModes(const wxVideoMode& WXUNUSED(mode)) const wxOVERRIDE
    {
        return wxArrayVideoModes();
    }

    virtual wxVideoMode GetCurrentMode() const wxOVERRIDE
    {
        return wxVideoMode();
    }

    virtual bool ChangeMode(const wxVideoMode& WXUNUSED(mode)) wxOVERRIDE
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
    virtual unsigned GetCount() wxOVERRIDE { return 1; }
    virtual int GetFromPoint(const wxPoint& pt) wxOVERRIDE;

protected:
    virtual wxDisplayImpl *CreateDisplay(unsigned n) wxOVERRIDE;

    virtual wxDisplayImpl *CreateSingleDisplay() = 0;
};

#endif // _WX_PRIVATE_DISPLAY_H_
