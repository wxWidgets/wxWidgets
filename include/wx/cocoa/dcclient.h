/////////////////////////////////////////////////////////////////////////////
// Name:        wx/cocoa/dcclient.h
// Purpose:     wxClientDC, wxPaintDC and wxWindowDC classes
// Author:      David Elliott
// Modified by:
// Created:     2003/04/01
// RCS-ID:      $Id$
// Copyright:   (c) 2003 David Elliott
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __WX_COCOA_DCCLIENT_H__
#define __WX_COCOA_DCCLIENT_H__

#include "wx/dc.h"

// DFE: A while ago I stumbled upon the fact that retrieving the parent
// NSView of the content view seems to return the entire window rectangle
// (including decorations).  Of course, that is not at all part of the
// Cocoa or OpenStep APIs, but it might be a neat hack.
class wxWindowDC: public wxDC
{
    DECLARE_DYNAMIC_CLASS(wxWindowDC)
public:
    wxWindowDC(void);
    // Create a DC corresponding to a window
    wxWindowDC(wxWindow *win);
    virtual ~wxWindowDC(void);

protected:
    wxWindow *m_window;
    WX_NSView m_lockedNSView;
// DC stack
    virtual bool CocoaLockFocus();
    virtual bool CocoaUnlockFocus();
    bool CocoaLockFocusOnNSView(WX_NSView nsview);
    bool CocoaUnlockFocusOnNSView();
    virtual bool CocoaGetBounds(void *rectData);
};

class wxClientDC: public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxClientDC)
public:
    wxClientDC(void);
    // Create a DC corresponding to a window
    wxClientDC(wxWindow *win);
    virtual ~wxClientDC(void);
protected:
// DC stack
    virtual bool CocoaLockFocus();
    virtual bool CocoaUnlockFocus();
};

class wxPaintDC: public wxWindowDC
{
    DECLARE_DYNAMIC_CLASS(wxPaintDC)
public:
    wxPaintDC(void);
    // Create a DC corresponding to a window
    wxPaintDC(wxWindow *win);
    virtual ~wxPaintDC(void);
protected:
// DC stack
    virtual bool CocoaLockFocus();
    virtual bool CocoaUnlockFocus();
};

#endif // __WX_COCOA_DCCLIENT_H__
