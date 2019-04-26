/////////////////////////////////////////////////////////////////////////////
// Name:        wx/osx/window.h
// Purpose:     wxWindowMac class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#include "wx/brush.h"
#include "wx/dc.h"

class WXDLLIMPEXP_FWD_CORE wxButton;
class WXDLLIMPEXP_FWD_CORE wxScrollBar;
class WXDLLIMPEXP_FWD_CORE wxPanel;
class WXDLLIMPEXP_FWD_CORE wxNonOwnedWindow;

#if wxOSX_USE_COCOA_OR_IPHONE
    class WXDLLIMPEXP_FWD_CORE wxWidgetImpl ;
    typedef wxWidgetImpl wxOSXWidgetImpl;
#endif


class WXDLLIMPEXP_CORE wxWindowMac: public wxWindowBase
{
    wxDECLARE_DYNAMIC_CLASS(wxWindowMac);

    friend class wxDC;
    friend class wxPaintDC;

public:
    wxWindowMac();

    wxWindowMac( wxWindowMac *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr );

    virtual ~wxWindowMac();

    bool Create( wxWindowMac *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr );

    virtual void SendSizeEvent(int flags = 0) wxOVERRIDE;

    // implement base class pure virtuals
    virtual void SetLabel( const wxString& label ) wxOVERRIDE;
    virtual wxString GetLabel() const wxOVERRIDE;

    virtual void Raise() wxOVERRIDE;
    virtual void Lower() wxOVERRIDE;

    virtual bool Show( bool show = true ) wxOVERRIDE;
    virtual bool ShowWithEffect(wxShowEffect effect,
                                unsigned timeout = 0) wxOVERRIDE
    {
        return OSXShowWithEffect(true, effect, timeout);
    }
    virtual bool HideWithEffect(wxShowEffect effect,
                                unsigned timeout = 0) wxOVERRIDE
    {
        return OSXShowWithEffect(false, effect, timeout);
    }

    virtual bool IsShownOnScreen() const wxOVERRIDE;

    virtual void SetFocus() wxOVERRIDE;

    virtual void WarpPointer( int x, int y ) wxOVERRIDE;
    virtual bool EnableTouchEvents(int eventsMask) wxOVERRIDE;

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = NULL ) wxOVERRIDE;

    virtual void Update() wxOVERRIDE;
    virtual void ClearBackground() wxOVERRIDE;

    virtual bool SetCursor( const wxCursor &cursor ) wxOVERRIDE;
    virtual bool SetFont( const wxFont &font ) wxOVERRIDE;
    virtual bool SetBackgroundColour( const wxColour &colour ) wxOVERRIDE;
    virtual bool SetForegroundColour( const wxColour &colour ) wxOVERRIDE;

    virtual bool SetBackgroundStyle(wxBackgroundStyle style) wxOVERRIDE;
    virtual bool IsTransparentBackgroundSupported(wxString* reason = NULL) const wxOVERRIDE;

    virtual int GetCharHeight() const wxOVERRIDE;
    virtual int GetCharWidth() const wxOVERRIDE;

public:
    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = true ) wxOVERRIDE;
    virtual void SetScrollPos( int orient, int pos, bool refresh = true ) wxOVERRIDE;
    virtual int GetScrollPos( int orient ) const wxOVERRIDE;
    virtual int GetScrollThumb( int orient ) const wxOVERRIDE;
    virtual int GetScrollRange( int orient ) const wxOVERRIDE;
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = NULL ) wxOVERRIDE;
    virtual void AlwaysShowScrollbars(bool horz = true, bool vert = true) wxOVERRIDE;
    virtual bool IsScrollbarAlwaysShown(int orient) const wxOVERRIDE
    {
        return orient == wxHORIZONTAL ? m_hScrollBarAlwaysShown
                                      : m_vScrollBarAlwaysShown;
    }

    virtual bool Reparent( wxWindowBase *newParent ) wxOVERRIDE;

#if wxUSE_HOTKEY && wxOSX_USE_COCOA_OR_CARBON
    // hot keys (system wide accelerators)
    // -----------------------------------

    virtual bool RegisterHotKey(int hotkeyId, int modifiers, int keycode) wxOVERRIDE;
    virtual bool UnregisterHotKey(int hotkeyId) wxOVERRIDE;
#endif // wxUSE_HOTKEY

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) wxOVERRIDE;

#endif

    // implementation from now on
    // --------------------------

    void MacClientToRootWindow( int *x , int *y ) const;

    void MacWindowToRootWindow( int *x , int *y ) const;

    void MacRootWindowToWindow( int *x , int *y ) const;

    virtual wxString MacGetToolTipString( wxPoint &where );

    // simple accessors
    // ----------------

    virtual WXWidget GetHandle() const wxOVERRIDE;

    virtual bool SetTransparent(wxByte alpha) wxOVERRIDE;
    virtual bool CanSetTransparent() wxOVERRIDE;
    virtual wxByte GetTransparent() const;

    // event handlers
    // --------------

    void OnMouseEvent( wxMouseEvent &event );

    void MacOnScroll( wxScrollEvent&event );

    virtual bool AcceptsFocus() const wxOVERRIDE;

    virtual bool IsDoubleBuffered() const wxOVERRIDE { return true; }

public:
    static long MacRemoveBordersFromStyle( long style );

public:
    // For implementation purposes:
    // sometimes decorations make the client area smaller
    virtual wxPoint GetClientAreaOrigin() const wxOVERRIDE;

    wxWindowMac *FindItem(long id) const;
    wxWindowMac *FindItemByHWND(WXHWND hWnd, bool controlOnly = false) const;

    virtual void        TriggerScrollEvent( wxEventType scrollEvent ) ;
    // this should not be overridden in classes above wxWindowMac
    // because it is called from its destructor via DeleteChildren
    virtual void        RemoveChild( wxWindowBase *child ) wxOVERRIDE;

    virtual bool        MacDoRedraw( long time ) ;
    virtual void        MacPaintChildrenBorders();
    virtual void        MacPaintBorders( int left , int top ) ;
    void                MacPaintGrowBox();

    // invalidates the borders and focus area around the control;
    // must not be virtual as it will be called during destruction
    void                MacInvalidateBorders() ;

    WXWindow            MacGetTopLevelWindowRef() const ;
    wxNonOwnedWindow*   MacGetTopLevelWindow() const ;

    virtual long        MacGetWXBorderSize() const;
    virtual long        MacGetLeftBorderSize() const ;
    virtual long        MacGetRightBorderSize() const ;
    virtual long        MacGetTopBorderSize() const ;
    virtual long        MacGetBottomBorderSize() const ;

    virtual void        MacSuperChangedPosition() ;

    // absolute coordinates of this window's root have changed
    virtual void        MacTopLevelWindowChangedPosition() ;

    virtual void        MacChildAdded() ;
    virtual void        MacVisibilityChanged() ;
    virtual void        MacEnabledStateChanged() ;
    virtual void        MacHiliteChanged() ;
    virtual wxInt32     MacControlHit( WXEVENTHANDLERREF handler , WXEVENTREF event ) ;

    bool                MacIsReallyEnabled() ;
    bool                MacIsReallyHilited() ;

#if WXWIN_COMPATIBILITY_2_8
    bool                MacIsUserPane();
#endif
    bool                MacIsUserPane() const;

    virtual bool        MacSetupCursor( const wxPoint& pt ) ;

    // return the rectangle that would be visible of this control,
    // regardless whether controls are hidden
    // only taking into account clipping by parent windows
    const wxRect&       MacGetClippedClientRect() const ;
    const wxRect&       MacGetClippedRect() const ;
    const wxRect&       MacGetClippedRectWithOuterStructure() const ;

    // returns the visible region of this control in window ie non-client coordinates
    const wxRegion&     MacGetVisibleRegion( bool includeOuterStructures = false ) ;

    // returns true if children have to clipped to the content area
    // (e.g., scrolled windows)
    bool                MacClipChildren() const { return m_clipChildren ; }
    void                MacSetClipChildren( bool clip ) { m_clipChildren = clip ; }

    // returns true if the grandchildren need to be clipped to the children's content area
    // (e.g., splitter windows)
    virtual bool        MacClipGrandChildren() const { return false ; }
    bool                MacIsWindowScrollbar( const wxWindow* sb ) const
    { return ((wxWindow*)m_hScrollBar == sb || (wxWindow*)m_vScrollBar == sb) ; }
    virtual bool IsClientAreaChild(const wxWindow *child) const wxOVERRIDE
    {
        return !MacIsWindowScrollbar(child) && !((wxWindow*)m_growBox==child) &&
               wxWindowBase::IsClientAreaChild(child);
    }

    void                MacPostControlCreate(const wxPoint& pos, const wxSize& size) ;
    wxList&             GetSubcontrols() { return m_subControls; }

    // translate wxWidgets coords into ones suitable
    // to be passed to CreateControl calls
    //
    // returns true if non-default coords are returned, false otherwise
    bool                MacGetBoundsForControl(const wxPoint& pos,
                                           const wxSize& size,
                                           int& x, int& y,
                                           int& w, int& h , bool adjustForOrigin ) const ;

    // the 'true' OS level control for this wxWindow
    wxOSXWidgetImpl*    GetPeer() const;

    // optimization to avoid creating a user pane in wxWindow::Create if we already know
    // we will replace it with our own peer
    void                DontCreatePeer();

    // return true unless DontCreatePeer() had been called
    bool                ShouldCreatePeer() const;

    // sets the native implementation wrapper, can replace an existing peer, use peer = NULL to
    // release existing peer
    void                SetPeer(wxOSXWidgetImpl* peer);

    // wraps the already existing peer with the wrapper
    void                SetWrappingPeer(wxOSXWidgetImpl* wrapper);

#if wxOSX_USE_COCOA_OR_IPHONE
    // the NSView or NSWindow of this window: can be used for both child and
    // non-owned windows
    //
    // this is useful for a few Cocoa function which can work with either views
    // or windows indiscriminately, e.g. for setting NSViewAnimationTargetKey
    virtual void *OSXGetViewOrWindow() const;
#endif // Cocoa

    void *              MacGetCGContextRef() { return m_cgContextRef ; }
    void                MacSetCGContextRef(void * cg) { m_cgContextRef = cg ; }

    // osx specific event handling common for all osx-ports

    virtual bool        OSXHandleClicked( double timestampsec );
    virtual bool        OSXHandleKeyEvent( wxKeyEvent& event );
    virtual void        OSXSimulateFocusEvents();

    bool                IsNativeWindowWrapper() const { return m_isNativeWindowWrapper; }

    double              GetContentScaleFactor() const wxOVERRIDE;

    // internal response to size events
    virtual void MacOnInternalSize() {}

protected:
    // For controls like radio buttons which are genuinely composite
    wxList              m_subControls;

    // the peer object, allowing for cleaner API support

    void *              m_cgContextRef ;

    // cache the clipped rectangles within the window hierarchy
    void                MacUpdateClippedRects() const ;

    mutable bool        m_cachedClippedRectValid ;
    mutable wxRect      m_cachedClippedRectWithOuterStructure ;
    mutable wxRect      m_cachedClippedRect ;
    mutable wxRect      m_cachedClippedClientRect ;
    mutable wxRegion    m_cachedClippedRegionWithOuterStructure ;
    mutable wxRegion    m_cachedClippedRegion ;
    mutable wxRegion    m_cachedClippedClientRegion ;

    // insets of the mac control from the wx top left corner
    wxPoint             m_macTopLeftInset ;
    wxPoint             m_macBottomRightInset ;
    wxByte              m_macAlpha ;

    wxScrollBar*        m_hScrollBar ;
    wxScrollBar*        m_vScrollBar ;
    bool                m_hScrollBarAlwaysShown;
    bool                m_vScrollBarAlwaysShown;
    wxWindow*           m_growBox ;
    wxString            m_label ;

    bool                m_isNativeWindowWrapper;

    // set to true if we do a sharp clip at the content area of this window
    // must be dynamic as eg a panel normally is not clipping precisely, but if
    // it becomes the target window of a scrolled window it has to...
    bool                m_clipChildren ;

    virtual bool        MacIsChildOfClientArea( const wxWindow* child ) const ;

    bool                MacHasScrollBarCorner() const;
    void                MacCreateScrollBars( long style ) ;
    void                MacRepositionScrollBars() ;
    void                MacUpdateControlFont() ;

    // implement the base class pure virtuals
    virtual void DoGetTextExtent(const wxString& string,
                                 int *x, int *y,
                                 int *descent = NULL,
                                 int *externalLeading = NULL,
                                 const wxFont *theFont = NULL ) const wxOVERRIDE;

    virtual void DoEnable( bool enable ) wxOVERRIDE;
#if wxUSE_MENUS
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y ) wxOVERRIDE;
#endif

    virtual void DoFreeze() wxOVERRIDE;
    virtual void DoThaw() wxOVERRIDE;

    virtual wxSize DoGetBestSize() const wxOVERRIDE;
    virtual wxSize DoGetSizeFromClientSize( const wxSize & size ) const;
    virtual void DoClientToScreen( int *x, int *y ) const wxOVERRIDE;
    virtual void DoScreenToClient( int *x, int *y ) const wxOVERRIDE;
    virtual void DoGetPosition( int *x, int *y ) const wxOVERRIDE;
    virtual void DoGetSize( int *width, int *height ) const wxOVERRIDE;
    virtual void DoGetClientSize( int *width, int *height ) const wxOVERRIDE;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
    virtual void DoSetClientSize(int width, int height) wxOVERRIDE;

    virtual void DoCaptureMouse() wxOVERRIDE;
    virtual void DoReleaseMouse() wxOVERRIDE;

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow( int x, int y, int width, int height ) wxOVERRIDE;
    virtual void DoSetWindowVariant( wxWindowVariant variant ) wxOVERRIDE;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip ) wxOVERRIDE;
#endif

    // common part of Show/HideWithEffect()
    virtual bool OSXShowWithEffect(bool show,
                                   wxShowEffect effect,
                                   unsigned timeout);

private:
    wxOSXWidgetImpl *   m_peer ;
    // common part of all ctors
    void Init();

    // show/hide scrollbars as needed, common part of SetScrollbar() and
    // AlwaysShowScrollbars()
    void DoUpdateScrollbarVisibility();

    wxDECLARE_NO_COPY_CLASS(wxWindowMac);
    wxDECLARE_EVENT_TABLE();
};

#endif // _WX_WINDOW_H_
