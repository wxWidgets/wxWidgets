/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKWINDOWH__
#define __GTKWINDOWH__

#ifdef __GNUG__
    #pragma interface
#endif

//-----------------------------------------------------------------------------
// callback definition for inserting a window (internal)
//-----------------------------------------------------------------------------

typedef void (*wxInsertChildFunction)( wxWindow*, wxWindow* );

//-----------------------------------------------------------------------------
// wxWindow
//-----------------------------------------------------------------------------

class wxWindow : public wxWindowBase
{
    DECLARE_DYNAMIC_CLASS(wxWindow)

public:
    // creating the window
    // -------------------
    wxWindow();
    wxWindow(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxPanelNameStr);
    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);
    virtual ~wxWindow();

    // implement base class (pure) virtual methods
    // -------------------------------------------
    virtual bool Destroy();

    virtual void Raise();
    virtual void Lower();

    virtual bool Show( bool show = TRUE );
    virtual bool Enable( bool enable = TRUE );

    virtual bool IsRetained() const;

    virtual void SetFocus();
    virtual bool AcceptsFocus() const;

    virtual bool Reparent( wxWindow *newParent );

    virtual void WarpPointer(int x, int y);
    virtual void CaptureMouse();
    virtual void ReleaseMouse();

    virtual void Refresh( bool eraseBackground = TRUE,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void Clear();

    virtual bool SetBackgroundColour( const wxColour &colour );
    virtual bool SetForegroundColour( const wxColour &colour );
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

    virtual void ClientToScreen( int *x, int *y ) const;
    virtual void ScreenToClient( int *x, int *y ) const;

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

    // implementation
    // --------------

    // wxWindows callbacks
    void OnKeyDown( wxKeyEvent &event );

    // also sets the global flag
    void SetScrolling(bool scroll);

    bool HasScrolling() const { return m_hasScrolling; }
    bool IsScrolling() const { return m_isScrolling; }

    /* I don't want users to override what's done in idle so everything that
       has to be done in idle time in order for wxGTK to work is done in
       OnInternalIdle */
    virtual void OnInternalIdle();

    /* For compatibility across platforms (not in event table) */
    void OnIdle(wxIdleEvent& WXUNUSED(event)) {};

    /* used by all classes in the widget creation process */
    void PreCreation( wxWindow *parent, wxWindowID id, const wxPoint &pos,
            const wxSize &size, long style, const wxString &name );
    void PostCreation();

    void InsertChild(wxWindow *child) { (*m_insertCallback)(this, child); }
    void DoAddChild(wxWindow *child) { AddChild(child); InsertChild(child); }

    /* the methods below are required because many native widgets
       are composed of several subwidgets and setting a style for
       the widget means setting it for all subwidgets as well.
       also, it is nor clear, which native widget is the top
       widget where (most of) the input goes. even tooltips have
       to be applied to all subwidgets. */

    virtual GtkWidget* GetConnectWidget();
    virtual bool IsOwnGtkWindow( GdkWindow *window );
    void ConnectWidget( GtkWidget *widget );

    // creates a new widget style if none is there
    // and sets m_widgetStyle to this value.
    GtkStyle *GetWidgetStyle();
    
    // called by SetFont() and SetXXXColour etc
    void SetWidgetStyle();
    
    // overridden in many GTK widgets
    virtual void ApplyWidgetStyle();

#if wxUSE_TOOLTIPS
    virtual void ApplyToolTip( GtkTooltips *tips, const wxChar *tip );
#endif // wxUSE_TOOLTIPS

    // called from GTK signales handlers. it indicates that
    // the layouting functions have to be called later on
    // (i.e. in idle time, implemented in OnInternalIdle() ).
    void UpdateSize() { m_sizeSet = FALSE; }

    // position and size of the window
    int                  m_x, m_y;
    int                  m_width, m_height;

    // see the docs in src/gtk/window.cpp
    GtkWidget           *m_widget;
    GtkWidget           *m_wxwindow;

    // scrolling stuff
    GtkAdjustment       *m_hAdjust,*m_vAdjust;
    float                m_oldHorizontalPos;
    float                m_oldVerticalPos;

    // we need an extra XGC flag set to get exposed
    // events from overlapping children upon moving
    // them. this flag will be set in this GC and
    // the GC will be used in wxWindow::ScrollWindow().
    GdkGC               *m_scrollGC;

    // extra (wxGTK-specific) flags
    bool                 m_needParent:1;    /* ! wxFrame, wxDialog, wxNotebookPage ?  */
    bool                 m_hasScrolling:1;
    bool                 m_isScrolling:1;
    bool                 m_hasVMT:1;
    bool                 m_sizeSet:1;
    bool                 m_resizing:1;
    bool                 m_isStaticBox:1;   /* faster than IS_KIND_OF */
    bool                 m_isFrame:1;       /* faster than IS_KIND_OF */
    bool                 m_acceptsFocus:1;  /* ! wxStaticBox etc.  */
    
    // these are true if the style were set before the
    // widget was realized (typcally in the constructor)
    // but the actual GTK style must not be set before
    // the widget has been "realized"
    bool                 m_delayedFont:1;
    bool                 m_delayedForegroundColour:1;
    bool                 m_delayedBackgroundColour:1;
    bool                 m_delayedCursor:1;

    // contains GTK's widgets internal information
    // about non-default widget font and colours.
    // we create one for each widget that gets any
    // non-default attribute set via SetFont() or
    // SetForegroundColour() / SetBackgroundColour().
    GtkStyle            *m_widgetStyle;

    // C++ has no virtual methods in the constrcutor
    // of any class but we need different methods
    // of inserting a child window into a wxFrame,
    // wxMDIFrame, wxNotebook etc. this is the
    // callback that will get used.
    wxInsertChildFunction  m_insertCallback;

    // implement the base class pure virtuals
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

    // common part of all ctors (can't be virtual because called from ctor)
    void Init();

private:
    DECLARE_EVENT_TABLE()
};

#endif // __GTKWINDOWH__
