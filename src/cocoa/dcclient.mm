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
#include "wx/log.h"

#import <AppKit/NSView.h>
#import <AppKit/NSAffineTransform.h>
#import <AppKit/NSColor.h>
#import <AppKit/NSGraphicsContext.h>
#import <AppKit/NSBezierPath.h>
#import <AppKit/NSWindow.h>

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

void wxWindowDC::Clear()
{
    wxASSERT(m_window);

    NSGraphicsContext *context = [NSGraphicsContext currentContext];
    [context saveGraphicsState];

    [m_backgroundBrush.GetNSColor() set];
    [NSBezierPath fillRect:[m_window->GetNSView() bounds]];

    [context restoreGraphicsState];
}

/*
 * wxClientDC
 */
IMPLEMENT_DYNAMIC_CLASS(wxClientDC, wxWindowDC)

wxClientDC::wxClientDC(void)
{
};

wxClientDC::wxClientDC( wxWindow *window )
{
    m_window = window;
};

wxClientDC::~wxClientDC(void)
{
    CocoaUnwindStackAndLoseFocus();
};

bool wxClientDC::CocoaLockFocus()
{
    wxLogDebug("Locking focus on wxClientDC=%p, NSView=%p",this, m_window->GetNSView());
    if([m_window->GetNSView() lockFocusIfCanDraw])
    {
        sm_cocoaDCStack.Insert(this);
        m_cocoaFlipped = [m_window->GetNSView() isFlipped];
        m_cocoaHeight = [m_window->GetNSView() bounds].size.height;
        CocoaApplyTransformations();
        return true;
    }
    wxLogDebug("focus lock failed!");
    return false;
}

bool wxClientDC::CocoaUnlockFocus()
{
    wxLogDebug("Unlocking focus on wxClientDC=%p, NSView=%p",this, m_window->GetNSView());
    [[m_window->GetNSView() window] flushWindow];
    [m_window->GetNSView() unlockFocus];
    return true;
}

/*
 * wxPaintDC
 */
IMPLEMENT_DYNAMIC_CLASS(wxPaintDC, wxWindowDC)

wxPaintDC::wxPaintDC(void)
{
};

wxPaintDC::wxPaintDC( wxWindow *window )
{
    m_window = window;
    wxASSERT_MSG([NSView focusView]==window->GetNSView(), "PaintDC's NSView does not have focus.  Please use wxPaintDC only as the first DC created in a paint handler");
    sm_cocoaDCStack.Insert(this);
    m_cocoaFlipped = [window->GetNSView() isFlipped];
    m_cocoaHeight = [window->GetNSView() bounds].size.height;
    CocoaApplyTransformations();
};

wxPaintDC::~wxPaintDC(void)
{
    CocoaUnwindStackAndLoseFocus();
};

bool wxPaintDC::CocoaLockFocus()
{
    wxFAIL_MSG("wxPaintDC cannot be asked to lock focus!");
    return false;
}

bool wxPaintDC::CocoaUnlockFocus()
{
    // wxPaintDC focus can never be unlocked.
    return false;
}

