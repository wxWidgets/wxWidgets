/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/window.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_WINDOW_H_
#define _WX_GTK_WINDOW_H_

#include "wx/dynarray.h"

// helper structure that holds class that holds GtkIMContext object and
// some additional data needed for key events processing
struct wxGtkIMData;

WX_DEFINE_EXPORTED_ARRAY_PTR(GdkWindow *, wxArrayGdkWindows);

//-----------------------------------------------------------------------------
// callback definition for inserting a window (internal)
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowGTK;
typedef void (*wxInsertChildFunction)( wxWindowGTK*, wxWindowGTK* );

//-----------------------------------------------------------------------------
// wxWindowGTK
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxWindowGTK : public wxWindowBase
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

    virtual void SetLabel(const wxString& WXUNUSED(label)) { }
    virtual wxString GetLabel() const { return wxEmptyString; }

    virtual bool Destroy();

    virtual void Raise();
    virtual void Lower();

    virtual bool Show( bool show = true );
    virtual void DoEnable( bool enable );

    virtual void SetWindowStyleFlag( long style );

    virtual bool IsRetained() const;

    virtual void SetFocus();
    virtual void SetCanFocus(bool canFocus);

    virtual bool Reparent( wxWindowBase *newParent );

    virtual void WarpPointer(int x, int y);

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );
    virtual void Update();
    virtual void ClearBackground();

    virtual bool SetBackgroundColour( const wxColour &colour );
    virtual bool SetForegroundColour( const wxColour &colour );
    virtual bool SetCursor( const wxCursor &cursor );
    virtual bool SetFont( const wxFont &font );

    virtual bool SetBackgroundStyle(wxBackgroundStyle style) ;

    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const;

    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = true );
    virtual void SetScrollPos( int orient, int pos, bool refresh = true );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );
    virtual bool ScrollLines(int lines);
    virtual bool ScrollPages(int pages);

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget );
#endif // wxUSE_DRAG_AND_DROP

    virtual void AddChild( wxWindowBase *child );
    virtual void RemoveChild( wxWindowBase *child );

    virtual void SetLayoutDirection(wxLayoutDirection dir);
    virtual wxLayoutDirection GetLayoutDirection() const;
    virtual wxCoord AdjustForLayoutDirection(wxCoord x,
                                             wxCoord width,
                                             wxCoord widthTotal) const;

    virtual bool DoIsExposed( int x, int y ) const;
    virtual bool DoIsExposed( int x, int y, int w, int h ) const;

    // currently wxGTK2-only
    void SetDoubleBuffered(bool on);
    virtual bool IsDoubleBuffered() const;

    // implementation
    // --------------

    virtual WXWidget GetHandle() const { return m_widget; }

    // many important things are done here, this function must be called
    // regularly
    virtual void OnInternalIdle();

    // Internal represention of Update()
    void GtkUpdate();

    // For compatibility across platforms (not in event table)
    void OnIdle(wxIdleEvent& WXUNUSED(event)) {}

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
    // also, it is not clear which native widget is the top
    // widget where (most of) the input goes. even tooltips have
    // to be applied to all subwidgets.
    virtual GtkWidget* GetConnectWidget();
    void ConnectWidget( GtkWidget *widget );

    // Called from several event handlers, if it returns true or false, the
    // same value should be immediately returned by the handler without doing
    // anything else. If it returns -1, the handler should continue as usual
    int GTKCallbackCommonPrologue(struct _GdkEventAny *event) const;

    // override this if some events should never be consumed by wxWidgets but
    // but have to be left for the native control
    //
    // base version just does GetEventHandler()->ProcessEvent()
    virtual bool GTKProcessEvent(wxEvent& event) const;

    // Map GTK widget direction of the given widget to/from wxLayoutDirection
    static wxLayoutDirection GTKGetLayout(GtkWidget *widget);
    static void GTKSetLayout(GtkWidget *widget, wxLayoutDirection dir);

    // return true if this window must have a non-NULL parent, false if it can
    // be created without parent (normally only top level windows but in wxGTK
    // there is also the exception of wxMenuBar)
    virtual bool GTKNeedsParent() const { return !IsTopLevel(); }

protected:
    // Override GTKWidgetNeedsMnemonic and return true if your
    // needs to set its mnemonic widget, such as for a
    // GtkLabel for wxStaticText, then do the actual
    // setting of the widget inside GTKWidgetDoSetMnemonic
    virtual bool GTKWidgetNeedsMnemonic() const;
    virtual void GTKWidgetDoSetMnemonic(GtkWidget* w);

    // Get the GdkWindows making part of this window: usually there will be
    // only one of them in which case it should be returned directly by this
    // function. If there is more than one GdkWindow (can be the case for
    // composite widgets), return NULL and fill in the provided array
    //
    // This is not pure virtual for backwards compatibility but almost
    // certainly must be overridden in any wxControl-derived class!
    virtual GdkWindow *GTKGetWindow(wxArrayGdkWindows& windows) const;

    // Check if the given window makes part of this widget
    bool GTKIsOwnWindow(GdkWindow *window) const;

    // Set the focus to this window if its setting was delayed because the
    // widget hadn't been realized when SetFocus() was called
    //
    // Return true if focus was set to us, false if nothing was done
    bool GTKSetDelayedFocusIfNeeded();

public:
    // Returns the default context which usually is anti-aliased
    PangoContext   *GtkGetPangoDefaultContext();

#if wxUSE_TOOLTIPS
    virtual void ApplyToolTip( GtkTooltips *tips, const wxChar *tip );
#endif // wxUSE_TOOLTIPS

    // Called from GTK signal handlers. it indicates that
    // the layouting functions have to be called later on
    // (i.e. in idle time, implemented in OnInternalIdle() ).
    void GtkUpdateSize() { m_sizeSet = false; }


    // Called when a window should delay showing itself
    // until idle time. This partly mimmicks defered
    // sizing under MSW.
    void GtkShowOnIdle() { m_showOnIdle = true; }

    // This is called from the various OnInternalIdle methods
    bool GtkShowFromOnIdle();

    // fix up the mouse event coords, used by wxListBox only so far
    virtual void FixUpMouseEvent(GtkWidget * WXUNUSED(widget),
                                 wxCoord& WXUNUSED(x),
                                 wxCoord& WXUNUSED(y)) { }

    // is this window transparent for the mouse events (as wxStaticBox is)?
    virtual bool IsTransparentForMouse() const { return false; }

    // is this a radiobutton (used by radiobutton code itself only)?
    virtual bool IsRadioButton() const { return false; }

    // Common scroll event handling code for wxWindow and wxScrollBar
    wxEventType GetScrollEventType(GtkRange* range);

    void BlockScrollEvent();
    void UnblockScrollEvent();

    // position and size of the window
    int                  m_x, m_y;
    int                  m_width, m_height;
    int                  m_oldClientWidth,m_oldClientHeight;

    // see the docs in src/gtk/window.cpp
    GtkWidget           *m_widget;          // mostly the widget seen by the rest of GTK
    GtkWidget           *m_wxwindow;        // mostly the client area as per wxWidgets

    // return true if the window is of a standard (i.e. not wxWidgets') class
    bool IsOfStandardClass() const { return m_wxwindow == NULL; }
    
    // this widget will be queried for GTK's focus events
    GtkWidget           *m_focusWidget;

    wxGtkIMData         *m_imData;


    // indices for the arrays below
    enum ScrollDir { ScrollDir_Horz, ScrollDir_Vert, ScrollDir_Max };

    // horizontal/vertical scroll bar
    GtkRange* m_scrollBar[ScrollDir_Max];

    // horizontal/vertical scroll position
    double m_scrollPos[ScrollDir_Max];

    // if true, don't notify about adjustment change (without resetting the
    // flag, so this has to be done manually)
    bool m_blockValueChanged[ScrollDir_Max];

    // return the scroll direction index corresponding to the given orientation
    // (which is wxVERTICAL or wxHORIZONTAL)
    static ScrollDir ScrollDirFromOrient(int orient)
    {
        return orient == wxVERTICAL ? ScrollDir_Vert : ScrollDir_Horz;
    }

    // return the orientation for the given scrolling direction
    static int OrientFromScrollDir(ScrollDir dir)
    {
        return dir == ScrollDir_Horz ? wxHORIZONTAL : wxVERTICAL;
    }

    // find the direction of the given scrollbar (must be one of ours)
    ScrollDir ScrollDirFromRange(GtkRange *range) const;


    // extra (wxGTK-specific) flags
    bool                 m_noExpose:1;          // wxGLCanvas has its own redrawing
    bool                 m_nativeSizeEvent:1;   // wxGLCanvas sends wxSizeEvent upon "alloc_size"
    bool                 m_hasScrolling:1;
    bool                 m_hasVMT:1;
    bool                 m_sizeSet:1;
    bool                 m_resizing:1;
    bool                 m_hasFocus:1;          // true if == FindFocus()
    bool                 m_isScrolling:1;       // dragging scrollbar thumb?
    bool                 m_clipPaintRegion:1;   // true after ScrollWindow()
    wxRegion             m_nativeUpdateRegion;  // not transformed for RTL
    bool                 m_dirtyTabOrder:1;     // tab order changed, GTK focus
                                                // chain needs update
    bool                 m_needsStyleChange:1;  // May not be able to change
                                                // background style until OnIdle
    bool                 m_mouseButtonDown:1;
    bool                 m_blockScrollEvent:1;

    bool                 m_showOnIdle:1;        // postpone showing the window until idle

    // C++ has no virtual methods in the constrcutor of any class but we need
    // different methods of inserting a child window into a wxFrame,
    // wxMDIFrame, wxNotebook etc. this is the callback that will get used.
    wxInsertChildFunction  m_insertCallback;

protected:
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

#if wxUSE_MENUS_NATIVE
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );
#endif // wxUSE_MENUS_NATIVE

    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

    // common part of all ctors (not virtual because called from ctor)
    void Init();

    virtual void DoMoveInTabOrder(wxWindow *win, MoveKind move);
    virtual bool DoNavigateIn(int flags);


    // Copies m_children tab order to GTK focus chain:
    void RealizeTabOrder();

    // Called by ApplyWidgetStyle (which is called by SetFont() and
    // SetXXXColour etc to apply style changed to native widgets) to create
    // modified GTK style with non-standard attributes. If forceStyle=true,
    // creates empty GtkRcStyle if there are no modifications, otherwise
    // returns NULL in such case.
    GtkRcStyle *CreateWidgetStyle(bool forceStyle = false);

    // Overridden in many GTK widgets who have to handle subwidgets
    virtual void ApplyWidgetStyle(bool forceStyle = false);

    // helper function to ease native widgets wrapping, called by
    // ApplyWidgetStyle -- override this, not ApplyWidgetStyle
    virtual void DoApplyWidgetStyle(GtkRcStyle *style);

    // sets the border of a given GtkScrolledWindow from a wx style
    static void GtkScrolledWindowSetBorder(GtkWidget* w, int style);

    // set the current cursor for all GdkWindows making part of this widget
    // (see GTKGetWindow)
    //
    // should be called from OnInternalIdle() if it's overridden
    void GTKUpdateCursor();

    void ConstrainSize();

private:
    enum ScrollUnit { ScrollUnit_Line, ScrollUnit_Page, ScrollUnit_Max };

    // common part of ScrollLines() and ScrollPages() and could be used, in the
    // future, for horizontal scrolling as well
    //
    // return true if we scrolled, false otherwise (on error or simply if we
    // are already at the end)
    bool DoScrollByUnits(ScrollDir dir, ScrollUnit unit, int units);


    DECLARE_DYNAMIC_CLASS(wxWindowGTK)
    DECLARE_NO_COPY_CLASS(wxWindowGTK)
};

extern WXDLLIMPEXP_CORE wxWindow *wxFindFocusedChild(wxWindowGTK *win);

#endif // _WX_GTK_WINDOW_H_
