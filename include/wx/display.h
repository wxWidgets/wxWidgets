/////////////////////////////////////////////////////////////////////////////
// Name:        display.h
// Purpose:     wxDisplay class
// Author:      Royce Mitchell III
// Modified by: 
// Created:     06/21/02
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DISPLAY_H_BASE_
#define _WX_DISPLAY_H_BASE_

#if wxUSE_DISPLAY

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface "display.h"
#endif

class wxPoint;
class wxRect;
class wxString;

class WXDLLEXPORT wxDisplayBase
{
public:
    // initialize the object containing all information about the given
    // display
    wxDisplayBase ( size_t index = 0 ) : m_index ( index )
    {
        wxASSERT_MSG(m_index < wxDisplayBase::GetCount(), wxT("An invalid index was passed to wxDisplay"));
    }

    // accessors

    // return the number of available displays, valid parameters to
    // wxDisplay ctor are from 0 up to this number
    static size_t GetCount();

    // find the display where the given point lies, return -1 if
    // it doesn't belong to any display
    static int GetFromPoint ( const wxPoint& pt );

    virtual wxRect GetGeometry() const = 0;
    virtual int GetDepth() const = 0;
    bool IsColour() const { return GetDepth() != 1; }

    // some people never learn to spell ;-)
    bool IsColor() const { return IsColour(); }

    // name may be empty
    virtual wxString GetName() const = 0;

    // let display 0 always be the primary display
    bool IsPrimary() { return m_index == 0; }


    virtual ~wxDisplayBase() {}

protected:
    size_t m_index; // which display did we select when creating this file?

    DECLARE_NO_COPY_CLASS(wxDisplayBase);
};

#if defined(__WXMSW__)
    #include "wx/msw/display.h"
#elif defined(__WXMOTIF__)
    #include "wx/motif/display.h"
#elif defined(__WXGTK__)
    #include "wx/gtk/display.h"
#elif defined(__WXMAC__)
    #include "wx/mac/display.h"
#elif defined(__WXPM__)
    #include "wx/os2/display.h"
#endif

#endif // wxUSE_DISPLAY

#endif // _WX_DISPLAY_H_BASE_
