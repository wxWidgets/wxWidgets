/////////////////////////////////////////////////////////////////////////////
// Name:        src/cocoa/dcclient.mm
// Purpose:     wxWindowDC, wxPaintDC, and wxClientDC classes
// Author:      David Elliott
// Modified by:
// Created:     2003/04/01
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include "wx/dcclient.h"
#include "wx/window.h"

#import <AppKit/NSView.h>
#import <AppKit/NSAffineTransform.h>

/*
 * wxWindowDC
 */
IMPLEMENT_DYNAMIC_CLASS(wxWindowDC, wxDC)

wxWindowDC::wxWindowDC(void)
:   m_window(NULL)
{
};

wxWindowDC::wxWindowDC( wxWindow *window )
:   m_window(window)
{
    wxFAIL_MSG("non-client window DC's are not supported");
};

wxWindowDC::~wxWindowDC(void)
{
};

/*
 * wxClientDC
 */
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC(void)
{
};

wxClientDC::wxClientDC( wxWindow *window )
:   wxWindowDC(window)
{
};

wxClientDC::~wxClientDC(void)
{
};

/*
 * wxPaintDC
 */
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)

wxPaintDC::wxPaintDC(void)
{
};

wxPaintDC::wxPaintDC( wxWindow *window )
:   wxWindowDC(window)
{
    wxASSERT_MSG([NSView focusView]==window->GetNSView(), "PaintDC's NSView does not have focus.  Please use wxPaintDC only as the first DC created in a paint handler");
    // This transform flips the graphics since wxDC uses top-left origin
    if(![window->GetNSView() isFlipped])
    {
        // The transform is auto released
        NSAffineTransform *transform = [NSAffineTransform transform];
        /*  x' = 1x + 0y + 0
            y' = 0x + -1y + window's height
        */
        NSAffineTransformStruct matrix = {
            1,  0
        ,   0, -1
        ,   0, [window->GetNSView() bounds].size.height
        };
        [transform setTransformStruct: matrix];
        // Apply the transform 
        [transform concat];
    }
    // TODO: Apply scaling transformation
};

wxPaintDC::~wxPaintDC(void)
{
};

