///////////////////////////////////////////////////////////////////////////////
// Name:        wx/compositewin.h
// Purpose:     wxCompositeWindow<> declaration
// Author:      Vadim Zeitlin
// Created:     2011-01-02
// RCS-ID:      $Id: wxhead.h,v 1.12 2010-04-22 12:44:51 zeitlin Exp $
// Copyright:   (c) 2011 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_COMPOSITEWIN_H_
#define _WX_COMPOSITEWIN_H_

#include "wx/window.h"

// NB: This is an experimental and, as for now, undocumented class used only by
//     wxWidgets itself internally. Don't use it in your code until its API is
//     officially stabilized unless you are ready to change it with the next
//     wxWidgets release.

// ----------------------------------------------------------------------------
// wxCompositeWindow is a helper for implementing composite windows: to define
// a class using subwindows, simply inherit from it specialized with the real
// base class name and implement GetCompositeWindowParts() pure virtual method.
// ----------------------------------------------------------------------------

// The template parameter W must be a wxWindow-derived class.
template <class W>
class wxCompositeWindow : public W
{
public:
    typedef W BaseWindowClass;

    // Default ctor doesn't do anything.
    wxCompositeWindow() { }

    // Override all wxWindow methods which must be forwarded to the composite
    // window parts.

    // Attribute setters group.
    //
    // NB: Unfortunately we can't factor out the call for the setter itself
    //     into DoSetForAllParts() because we can't call the function passed to
    //     it non-virtually and we need to do this to avoid infinite recursion,
    //     so we work around this by calling the method of this object itself
    //     manually in each function.
    virtual bool SetForegroundColour(const wxColour& colour)
    {
        if ( !BaseWindowClass::SetForegroundColour(colour) )
            return false;

        DoSetForAllParts(&wxWindowBase::SetForegroundColour, colour);

        return true;
    }

    virtual bool SetBackgroundColour(const wxColour& colour)
    {
        if ( !BaseWindowClass::SetBackgroundColour(colour) )
            return false;

        DoSetForAllParts(&wxWindowBase::SetBackgroundColour, colour);

        return true;
    }

    virtual bool SetFont(const wxFont& font)
    {
        if ( !BaseWindowClass::SetFont(font) )
            return false;

        DoSetForAllParts(&wxWindowBase::SetFont, font);

        return true;
    }

    virtual bool SetCursor(const wxCursor& cursor)
    {
        if ( !BaseWindowClass::SetCursor(cursor) )
            return false;

        DoSetForAllParts(&wxWindowBase::SetCursor, cursor);

        return true;
    }

private:
    // Must be implemented by the derived class to return all children to which
    // the public methods we override should forward to.
    virtual wxWindowList GetCompositeWindowParts() const = 0;

    template <class T>
    void DoSetForAllParts(bool (wxWindowBase::*func)(const T&), T arg)
    {
        // Simply call the setters for all parts of this composite window.
        const wxWindowList parts = GetCompositeWindowParts();
        for ( wxWindowList::const_iterator i = parts.begin();
              i != parts.end();
              ++i )
        {
            wxWindow * const child = *i;

            (child->*func)(arg);
        }
    }

    wxDECLARE_NO_COPY_TEMPLATE_CLASS(wxCompositeWindow, W);
};

#endif // _WX_COMPOSITEWIN_H_
