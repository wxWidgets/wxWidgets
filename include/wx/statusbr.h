/////////////////////////////////////////////////////////////////////////////
// Name:        wx/statusbr.h
// Purpose:     wxStatusBar class interface
// Author:      Vadim Zeitlin
// Modified by:
// Created:     05.02.00
// RCS-ID:      $Id$
// Copyright:   (c) wxWindows team
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_STATUSBR_H_BASE_
#define _WX_STATUSBR_H_BASE_

#include "wx/window.h"

// ----------------------------------------------------------------------------
// wxStatusBar: a window near the bottom of the frame used for status info
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStatusBarBase : public wxWindow
{
public:
    wxStatusBarBase() { m_statusWidths = NULL; }

    // get/set the number of fields
    virtual void SetFieldsCount(int number = 1,
                                const int *widths = (const int *) NULL) = 0;
    int GetFieldsCount() const { return m_nFields; }

    // get/set the text of the given field
    virtual void SetStatusText(const wxString& text, int number = 0) = 0;
    virtual wxString GetStatusText(int number = 0) const = 0;

    // set status line widths (n should be the same as field count)
    virtual void SetStatusWidths(int n, const int widths[]) = 0;

    // Get the position and size of the field's internal bounding rectangle
    virtual bool GetFieldRect(int i, wxRect& rect) const = 0;

    // sets the minimal vertical size of the status bar
    virtual void SetMinHeight(int height) = 0;

    // get the dimensions of the horizontal and vertical borders
    virtual int GetBorderX() const = 0;
    virtual int GetBorderY() const = 0;

protected:
    int     m_nFields;      // the current number of fields
    int    *m_statusWidths; // the width (if !NULL) of the fields
};

#if defined(__WIN32__) && wxUSE_NATIVE_STATUSBAR
    #include "wx/msw/statbr95.h"

    typedef wxStatusBar95 wxStatusBarReal;
#elif defined(__WXMAC__) && !defined(__UNIX__)
    #include "wx/mac/statusbr.h"

    typedef wxStatusBarMac wxStatusBarReal;
#else
    #include "wx/generic/statusbr.h"

    typedef wxStatusBarGeneric wxStatusBarReal;
#endif

// we can't just typedef wxStatusBar to be one of 95/Generic because we should
// be able to forward declare it (done in several places) and because wxWin
// RTTI wouldn't work then
class WXDLLEXPORT wxStatusBar : public wxStatusBarReal
{
public:
    wxStatusBar() { }
    wxStatusBar(wxWindow *parent,
                wxWindowID id,
                const wxPoint& WXUNUSED(pos) = wxDefaultPosition,
                const wxSize& WXUNUSED(size) = wxDefaultSize,
                long style = wxST_SIZEGRIP,
                const wxString& name = wxPanelNameStr)
    {
        Create(parent, id, style, name);
    }
    wxStatusBar(wxWindow *parent,
                wxWindowID id,
                long style,
                const wxString& name = wxPanelNameStr)
    {
        Create(parent, id, style, name);
    }

private:
    DECLARE_DYNAMIC_CLASS(wxStatusBar)
};

#endif
    // _WX_STATUSBR_H_BASE_
