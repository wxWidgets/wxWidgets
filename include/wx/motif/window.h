/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindow class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#ifdef __GNUG__
    #pragma interface "window.h"
#endif

// ----------------------------------------------------------------------------
// wxWindow class for Motif - see also wxWindowBase
// ----------------------------------------------------------------------------

class wxWindow : public wxWindowBase
{
DECLARE_DYNAMIC_CLASS(wxWindow)

friend class WXDLLEXPORT wxDC;
friend class WXDLLEXPORT wxWindowDC;

public:
    wxWindow() { Init(); }

    wxWindow(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxPanelNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindow();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    // implement base class pure virtuals
    virtual void SetTitle( const wxString& title);
    virtual wxString GetTitle() const;

    virtual void Raise();
    virtual void Lower();

    virtual bool Show( bool show = TRUE );
    virtual bool Enable( bool enable = TRUE );

    virtual void SetFocus();

    virtual void WarpPointer(int x, int y);
    virtual void CaptureMouse();
    virtual void ReleaseMouse();

    virtual void Refresh( bool eraseBackground = TRUE,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void Clear();

    virtual bool SetCursor( const wxCursor &cursor );
    virtual bool SetFont( const wxFont &font );

    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const;

    virtual bool PopupMenu( wxMenu *menu, int x, int y );

    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = TRUE );
    virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget );
#endif // wxUSE_DRAG_AND_DROP

    // Accept files for dragging
    virtual void DragAcceptFiles(bool accept);

protected:
    // event handlers (not virtual by design)
    void OnIdle(wxIdleEvent& event);

    // For implementation purposes - sometimes decorations make the client area
    // smaller
    virtual wxPoint GetClientAreaOrigin() const;

    // Makes an adjustment to the window position (for example, a frame that has
    // a toolbar that it manages itself).
    virtual void AdjustForParentClientOrigin(int& x, int& y, int sizeFlags);

    virtual void AddChild(wxWindow *child);         // Adds reference to the child object
    virtual void RemoveChild(wxWindow *child);   // Removes reference to child
    virtual void DestroyChildren();  // Removes and destroys all children

    wxWindow *GetChild(int number) const
        { return GetChildren().Item(number)->GetData(); }

    // Responds to colour changes: passes event on to children.
    void OnSysColourChanged(wxSysColourChangedEvent& event);

    // Motif-specific

        // empties the m_updateRects list
    void ClearUpdateRects();

        // CanvasXXXSiize functions
    void CanvasGetSize(int* width, int* height) const; // If have drawing area
    void CanvasGetClientSize(int *width, int *height) const;
    void CanvasGetPosition(int *x, int *y) const; // If have drawing area
    void CanvasSetClientSize(int width, int size);
    void CanvasSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);

    // Gives window a chance to do something in response to a size message, e.g.
    // arrange status bar, toolbar etc.
    virtual bool PreResize() { return TRUE; }

    // accessors
    // ---------

    // Get main widget for this window, e.g. a text widget
    virtual WXWidget GetMainWidget() const;
    // Get the widget that corresponds to the label (for font setting, label setting etc.)
    virtual WXWidget GetLabelWidget() const;
    // Get the client widget for this window (something we can create other
    // windows on)
    virtual WXWidget GetClientWidget() const;
    // Get the top widget for this window, e.g. the scrolled widget parent of a
    // multi-line text widget. Top means, top in the window hierarchy that
    // implements this window.
    virtual WXWidget GetTopWidget() const;

    // base class pure virtual
    virtual WXWidget GetHandle() const { return GetMainWidget(); }

    void SetMainWidget(WXWidget w) { m_mainWidget = w; }

    bool CanAddEventHandler() const { return m_canAddEventHandler; }
    void SetCanAddEventHandler(bool flag) { m_canAddEventHandler = flag; }

    // Get the underlying X window and display
    WXWindow GetXWindow() const;
    WXDisplay *GetXDisplay() const;

    WXPixmap GetBackingPixmap() const { return m_backingPixmap; }
    int GetPixmapWidth() const { return m_pixmapWidth; }
    int GetPixmapHeight() const { return m_pixmapHeight; }

    // Change properties
    virtual void ChangeFont(bool keepOriginalSize = TRUE);             // Change to the current font (often overridden)
    virtual void DoChangeForegroundColour(WXWidget widget, wxColour& foregroundColour);
    virtual void DoChangeBackgroundColour(WXWidget widget, wxColour& backgroundColour, bool changeArmColour = FALSE);

    // Change background and foreground colour using current background colour
    // setting (Motif generates foreground based on background)
    virtual void ChangeBackgroundColour();
    // Change foreground colour using current foreground colour setting
    virtual void ChangeForegroundColour();

    // Adds the widget to the hash table and adds event handlers.
    bool AttachWidget(wxWindow* parent, WXWidget mainWidget,
                      WXWidget formWidget, int x, int y, int width, int height);
    bool DetachWidget(WXWidget widget);

    // Generates a paint event
    virtual void DoPaint();

    // How to implement accelerators. If we find a key event, translate to
    // wxWindows wxKeyEvent form. Find a widget for the window. Now find a
    // wxWindow for the widget. If there isn't one, go up the widget hierarchy
    // trying to find one. Once one is found, call ProcessAccelerator for the
    // window. If it returns TRUE (processed the event), skip the X event,
    // otherwise carry on up the wxWindows window hierarchy calling
    // ProcessAccelerator. If all return FALSE, process the X event as normal.
    // Eventually we can implement OnCharHook the same way, but concentrate on
    // accelerators for now. ProcessAccelerator must look at the current
    // accelerator table, and try to find what menu id or window (beneath it)
    // has this ID. Then construct an appropriate command
    // event and send it.
    virtual bool ProcessAccelerator(wxKeyEvent& event);

protected:
    // unmanage and destroy an X widget f it's !NULL (passing NULL is ok)
    void UnmanageAndDestroy(WXWidget widget);

    // map or unmap an X widget (passing NULL is ok), returns TRUE if widget was
    // mapped/unmapped
    bool MapOrUnmap(WXWidget widget, bool map);

    // get either hor or vert scrollbar widget
    WXWidget GetScrollbar(wxOrientation orient) const
        { return orient == wxHORIZONTAL ? m_hScrollBar : m_vScrollBar; }

    // set the scroll pos
    void SetInternalScrollPos(wxOrientation orient, int pos)
    {
        if ( orient == wxHORIZONTAL )
            m_scrollPosX = pos;
        else
            m_scrollPosY = pos;
    }

    // Motif-specific flags
    bool m_needsRefresh:1;          // repaint backing store?
    bool m_canAddEventHandler:1;    // ???
    bool m_button1Pressed:1;
    bool m_button2Pressed:1;
    bool m_button3Pressed:1;

    // For double-click detection
    long   m_lastTS;         // last timestamp
    int    m_lastButton;     // last pressed button
    wxList m_updateRects;    // List of wxRects representing damaged region

protected:
    WXWidget              m_mainWidget;
    WXWidget              m_hScrollBar;
    WXWidget              m_vScrollBar;
    WXWidget              m_borderWidget;
    WXWidget              m_scrolledWindow;
    WXWidget              m_drawingArea;
    bool                  m_winCaptured;
    bool                  m_hScroll;
    bool                  m_vScroll;
    WXPixmap              m_backingPixmap;
    int                   m_pixmapWidth;
    int                   m_pixmapHeight;
    int                   m_pixmapOffsetX;
    int                   m_pixmapOffsetY;

    // Store the last scroll pos, since in wxWin the pos isn't set automatically
    // by system
    int                   m_scrollPosX;
    int                   m_scrollPosY;

    // implement the base class pure virtuals
    virtual void DoClientToScreen( int *x, int *y ) const;
    virtual void DoScreenToClient( int *x, int *y ) const;
    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetClientSize( int *width, int *height ) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

private:
    // common part of all ctors
    void Init();

    DECLARE_NO_COPY_CLASS(wxWindow);
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// A little class to switch off size optimization while an instance of the object
// exists
//
// TODO what is it for??
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxNoOptimize: public wxObject
{
public:
  wxNoOptimize();
  ~wxNoOptimize();

  static bool CanOptimize();

protected:
  static int m_count;
};

#endif
    // _WX_WINDOW_H_
