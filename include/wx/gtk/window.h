/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/window.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKWINDOWH__
#define __GTKWINDOWH__

#if defined(__GNUG__) && !defined(__APPLE__)
    #pragma interface
#endif

//-----------------------------------------------------------------------------
// callback definition for inserting a window (internal)
//-----------------------------------------------------------------------------

class wxWindowGTK;
typedef void (*wxInsertChildFunction)( wxWindowGTK*, wxWindowGTK* );

//-----------------------------------------------------------------------------
// wxWindowGTK
//-----------------------------------------------------------------------------

class wxWindowGTK : public wxWindowBase
{
public:
    // creating the window
    // -------------------
    wxWindowGTK();
    wxWindowGTK(wxWindow *parent,
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
    virtual ~wxWindowGTK();

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

    virtual bool Reparent( wxWindowBase *newParent );

    virtual void WarpPointer(int x, int y);

    virtual void Refresh( bool eraseBackground = TRUE,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void Update();
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

#if wxUSE_MENUS_NATIVE
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );
#endif // wxUSE_MENUS_NATIVE

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

    virtual WXWidget GetHandle() const { return m_widget; }

    // I don't want users to override what's done in idle so everything that
    // has to be done in idle time in order for wxGTK to work is done in
    // OnInternalIdle
    virtual void OnInternalIdle();

    // Internal represention of Update()
    void GtkUpdate();
    
    // For delayed background 
    void GtkSetBackgroundColour( const wxColour &colour );
    void GtkSetForegroundColour( const wxColour &colour );
    
    // For compatibility across platforms (not in event table)
    void OnIdle(wxIdleEvent& WXUNUSED(event)) {}

    // wxGTK-specific: called recursively by Enable,
    // to give widgets an oppprtunity to correct their colours after they
    // have been changed by Enable
    virtual void OnParentEnable( bool WXUNUSED(enable) ) {}

    // Used by all window classes in the widget creation process.
    bool PreCreation( wxWindowGTK *parent, const wxPoint &pos, const wxSize &size );
    void PostCreation();

    // Internal addition of child windows. differs from class
    // to class not by using virtual functions but by using
    // the m_insertCallback.
    void DoAddChild(wxWindowGTK *child);
    
    // This methods sends wxPaintEvents to the window. It reads the
    // update region, breaks it up into rects and sends an event
    // for each rect. It is also responsible for background erase
    // events and NC paint events. It is called from "draw" and
    // "expose" handlers as well as from ::Update()
    void GtkSendPaintEvents();

    // The methods below are required because many native widgets
    // are composed of several subwidgets and setting a style for
    // the widget means setting it for all subwidgets as well.
    // also, it is nor clear, which native widget is the top
    // widget where (most of) the input goes. even tooltips have
    // to be applied to all subwidgets.
    virtual GtkWidget* GetConnectWidget();
    virtual bool IsOwnGtkWindow( GdkWindow *window );
    void ConnectWidget( GtkWidget *widget );

    // Creates a new widget style if none is there
    // and sets m_widgetStyle to this value.
    GtkStyle *GetWidgetStyle();

#ifdef __WXGTK20__
    // Returns the default context which usually is anti-aliased
    PangoContext   *GtkGetPangoDefaultContext();
    
    // Returns the X11 context which renders on the X11 client
    // side (which can be remote) and which usually is not
    // anti-aliased and is thus faster
    PangoContext   *GtkGetPangoX11Context();
    PangoContext   *m_x11Context;
#endif

    // Called by SetFont() and SetXXXColour etc
    void SetWidgetStyle();

    // Overridden in many GTK widgets who have to handle subwidgets
    virtual void ApplyWidgetStyle();

#if wxUSE_TOOLTIPS
    virtual void ApplyToolTip( GtkTooltips *tips, const wxChar *tip );
#endif // wxUSE_TOOLTIPS

    // Called from GTK signales handlers. it indicates that
    // the layouting functions have to be called later on
    // (i.e. in idle time, implemented in OnInternalIdle() ).
    void GtkUpdateSize() { m_sizeSet = FALSE; }

    // fix up the mouse event coords, used by wxListBox only so far
    virtual void FixUpMouseEvent(GtkWidget * WXUNUSED(widget),
                                 wxCoord& WXUNUSED(x),
                                 wxCoord& WXUNUSED(y)) { }

    // is this window transparent for the mouse events (as wxStaticBox is)?
    virtual bool IsTransparentForMouse() const { return FALSE; }

    // is this a radiobutton (used by radiobutton code itself only)?
    virtual bool IsRadioButton() const { return FALSE; }

    // position and size of the window
    int                  m_x, m_y;
    int                  m_width, m_height;
    int                  m_oldClientWidth,m_oldClientHeight;

    // see the docs in src/gtk/window.cpp
    GtkWidget           *m_widget;          // mostly the widget seen by the rest of GTK
    GtkWidget           *m_wxwindow;        // mostly the client area as per wxWindows

    // this widget will be queried for GTK's focus events
    GtkWidget           *m_focusWidget;

#ifdef __WXGTK20__
    GtkIMMulticontext   *m_imContext;
#else
#if HAVE_XIM
    // XIM support for wxWindows
    GdkIC               *m_ic;
    GdkICAttr           *m_icattr;
#endif
#endif

#ifndef __WXGTK20__
    // The area to be cleared (and not just refreshed)
    // We cannot make this distinction under GTK 2.0.
    wxRegion             m_clearRegion;
#endif

    // scrolling stuff
    GtkAdjustment       *m_hAdjust,*m_vAdjust;
    float                m_oldHorizontalPos;
    float                m_oldVerticalPos;

    // extra (wxGTK-specific) flags
    bool                 m_needParent:1;        // ! wxFrame, wxDialog, wxNotebookPage ?
    bool                 m_noExpose:1;          // wxGLCanvas has its own redrawing
    bool                 m_nativeSizeEvent:1;   // wxGLCanvas sends wxSizeEvent upon "alloc_size"
    bool                 m_hasScrolling:1;
    bool                 m_hasVMT:1;
    bool                 m_sizeSet:1;
    bool                 m_resizing:1;
    bool                 m_acceptsFocus:1;      // true if not static
    bool                 m_hasFocus:1;          // true if == FindFocus()
    bool                 m_isScrolling:1;       // dragging scrollbar thumb?
    bool                 m_clipPaintRegion:1;   // TRUE after ScrollWindow()
    bool                 m_queuedFullRedraw:1;  // TRUE after DoMoveWindow

    // These are true if the style were set before the widget was realized
    // (typcally in the constructor) but the actual GTK style must not be set
    // before the widget has been "realized"
    bool                 m_delayedForegroundColour:1;
    bool                 m_delayedBackgroundColour:1;

    // Contains GTK's widgets internal information about non-default widget
    // font and colours. we create one for each widget that gets any
    // non-default attribute set via SetFont() or SetForegroundColour() /
    // SetBackgroundColour().
    GtkStyle            *m_widgetStyle;

    // C++ has no virtual methods in the constrcutor of any class but we need
    // different methods of inserting a child window into a wxFrame,
    // wxMDIFrame, wxNotebook etc. this is the callback that will get used.
    wxInsertChildFunction  m_insertCallback;

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
    virtual void DoMoveWindow(int x, int y, int width, int height);

    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();
    
#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

protected:
    // common part of all ctors (not virtual because called from ctor)
    void Init();

private:
    DECLARE_DYNAMIC_CLASS(wxWindowGTK)
    DECLARE_NO_COPY_CLASS(wxWindowGTK)
};

extern wxWindow *wxFindFocusedChild(wxWindowGTK *win);

#endif // __GTKWINDOWH__
