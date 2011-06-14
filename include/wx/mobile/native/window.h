/////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/native/window.h
// Purpose:     wxMobile/wxiOS (GSoC 2011)
// Author:      Julian Smart, Linas Valiukas
// Modified by:
// Created:     2011-06-14
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart, Linas Valiukas
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_NATIVE_WINDOW_H_
#define _WX_MOBILE_NATIVE_WINDOW_H_

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
    // _WX_MOBILE_NATIVE_WINDOW_H_
