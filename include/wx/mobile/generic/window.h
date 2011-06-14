/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/window.h
// Purpose:     wxMoWindow class
// Author:      Julian Smart
// Modified by:
// Created:     2009-06-07
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_WINDOW_H_
#define _WX_MOBILE_GENERIC_WINDOW_H_

#include "wx/window.h"

/**
    @class wxMoWindow

    @category{wxMobile}
*/


class WXDLLEXPORT wxMoWindow : public wxWindow
{
public:
    /// Default constructor.
    wxMoWindow() { Init(); }

    // Constructor.
    wxMoWindow(wxWindow *parent,
            wxWindowID winid = wxID_ANY,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxNO_BORDER,
            const wxString& name = wxT("window"))
    {
        Init();

        Create(parent, winid, pos, size, style, name);
    }

    /// Creation function.
    bool Create(wxWindow *parent,
                wxWindowID winid = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxNO_BORDER,
                const wxString& name = wxT("window"));

    virtual ~wxMoWindow();

    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = true );
    virtual void SetScrollPos( int orient, int pos, bool refresh = true );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;

    virtual bool ScrollLines(int lines);
    virtual bool ScrollPages(int pages);

protected:

    // common part of all ctors
    void Init();

    int m_scrollPosX;
    int m_scrollPosY;
    int m_scrollThumbSizeX;
    int m_scrollThumbSizeY;
    int m_scrollRangeX;
    int m_scrollRangeY;

private:
    DECLARE_DYNAMIC_CLASS_NO_COPY(wxMoWindow)
    DECLARE_EVENT_TABLE()
};


#endif
    // _WX_MOBILE_GENERIC_WINDOW_H_
