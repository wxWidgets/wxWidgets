/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __WX_WINDOW_H__
#define __WX_WINDOW_H__

#ifdef __GNUG__
    #pragma interface "window.h"
#endif


//-----------------------------------------------------------------------------
// wxWindow
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxWindowMGL : public wxWindowBase
{
    DECLARE_DYNAMIC_CLASS(wxWindowMGL)

public:
    // creating the window
    // -------------------
    wxWindowMGL() {}
    wxWindowMGL(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr) {}
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr) {}
    virtual ~wxWindowMGL() {}

    // implement base class (pure) virtual methods
    // -------------------------------------------
    virtual bool Destroy() {return TRUE;}

    virtual void Raise() {}
    virtual void Lower() {}

    virtual bool Show( bool show = TRUE )  {return TRUE;}
    virtual bool Enable( bool enable = TRUE )  {return TRUE;}

    virtual bool IsRetained() const  {return TRUE;}

    virtual void SetFocus() {}
    virtual bool AcceptsFocus() const  {return TRUE;}

    virtual bool Reparent( wxWindowBase *newParent )  {return TRUE;}

    virtual void WarpPointer(int x, int y) {}
    virtual void CaptureMouse() {}
    virtual void ReleaseMouse() {}

    virtual void Refresh( bool eraseBackground = TRUE,
                          const wxRect *rect = (const wxRect *) NULL ) {}
    virtual void Clear() {}

    virtual bool SetBackgroundColour( const wxColour &colour )  {return TRUE;}
    virtual bool SetForegroundColour( const wxColour &colour ) {return TRUE;}
    virtual bool SetCursor( const wxCursor &cursor )  {return TRUE;}
    virtual bool SetFont( const wxFont &font )  {return TRUE;}

    virtual int GetCharHeight() const {return 0;}
    virtual int GetCharWidth() const {return 0;}
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const {}

    virtual bool DoPopupMenu( wxMenu *menu, int x, int y )  {return TRUE;}

    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = TRUE ) {}
    virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE ) {}
    virtual int GetScrollPos( int orient ) const  {return 0;}
    virtual int GetScrollThumb( int orient ) const  {return 0;}
    virtual int GetScrollRange( int orient ) const  {return 0;}
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL ) {}

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) {}
#endif // wxUSE_DRAG_AND_DROP

    virtual WXWidget GetHandle() const { return NULL; }

    /* For compatibility across platforms (not in event table) */
    void OnIdle(wxIdleEvent& WXUNUSED(event)) {};

    // implement the base class pure virtuals
    virtual void DoClientToScreen( int *x, int *y ) const {}
    virtual void DoScreenToClient( int *x, int *y ) const {}
    virtual void DoGetPosition( int *x, int *y ) const {}
    virtual void DoGetSize( int *width, int *height ) const {}
    virtual void DoGetClientSize( int *width, int *height ) const {}
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) {}
    virtual void DoSetClientSize(int width, int height) {}
    virtual void DoMoveWindow(int x, int y, int width, int height) {}

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip ) {}
#endif // wxUSE_TOOLTIPS

    // common part of all ctors (can't be virtual because called from ctor)
    void Init() {}

private:
    DECLARE_NO_COPY_CLASS(wxWindowMGL);
};

#endif // ___WX_WINDOW_H__
