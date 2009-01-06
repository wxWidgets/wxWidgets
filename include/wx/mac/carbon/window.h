/////////////////////////////////////////////////////////////////////////////
// Name:        include/wx/mac/carbon/window.h
// Purpose:     wxWindowMac class
// Author:      Stefan Csomor
// Modified by:
// Created:     1998-01-01
// RCS-ID:      $Id$
// Copyright:   (c) Stefan Csomor
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#include "wx/brush.h"
#include "wx/dc.h"

class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxScrollBar;
class WXDLLEXPORT wxTopLevelWindowMac;

class wxMacControl ;

class WXDLLEXPORT wxWindowMac: public wxWindowBase
{
    DECLARE_DYNAMIC_CLASS(wxWindowMac)

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

    // implement base class pure virtuals
    virtual void SetLabel( const wxString& label );
    virtual wxString GetLabel() const;

    virtual void Raise();
    virtual void Lower();

    virtual bool Show( bool show = true );
    virtual bool Enable( bool enable = true );

    virtual void SetFocus();

    virtual void WarpPointer( int x, int y );

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = NULL );
    virtual void Freeze();
    virtual void Thaw();
    virtual bool IsFrozen() const;
    
    virtual void Update() ;
    virtual void ClearBackground();

    virtual bool SetCursor( const wxCursor &cursor );
    virtual bool SetFont( const wxFont &font );
    virtual bool SetBackgroundColour( const wxColour &colour );
    virtual bool SetForegroundColour( const wxColour &colour );
    virtual wxVisualAttributes GetDefaultAttributes() const;

    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent( const wxString& string,
                               int *x, int *y,
                               int *descent = NULL,
                               int *externalLeading = NULL,
                               const wxFont *theFont = NULL )
                               const;
protected:
    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );
public:
    virtual void SetScrollbar( int orient, int pos, int thumbVisible,
                               int range, bool refresh = true );
    virtual void SetScrollPos( int orient, int pos, bool refresh = true );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = (wxRect *) NULL );
    virtual bool Reparent( wxWindowBase *newParent );

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget );
#endif

    // Accept files for dragging
    virtual void DragAcceptFiles( bool accept );

    // implementation from now on
    // --------------------------

    void MacClientToRootWindow( int *x , int *y ) const;
    void MacRootWindowToClient( int *x , int *y ) const;

    void MacWindowToRootWindow( int *x , int *y ) const;
    void MacWindowToRootWindow( short *x , short *y ) const;

    void MacRootWindowToWindow( int *x , int *y ) const;
    void MacRootWindowToWindow( short *x , short *y ) const;

    virtual wxString MacGetToolTipString( wxPoint &where );

    // simple accessors
    // ----------------

    virtual WXWidget GetHandle() const;

    virtual bool SetTransparent(wxByte alpha);
    virtual bool CanSetTransparent();
    virtual wxByte GetTransparent() const ;
    
#if WXWIN_COMPATIBILITY_2_4
    bool GetTransparentBackground() const { return m_backgroundTransparent; }
    void SetTransparent(bool t = true) { m_backgroundTransparent = t; }
#endif

    // event handlers
    // --------------
    void OnSetFocus( wxFocusEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnNcPaint( wxNcPaintEvent& event );
    void OnEraseBackground(wxEraseEvent& event );
    void OnMouseEvent( wxMouseEvent &event );

    void MacOnScroll( wxScrollEvent&event );

    virtual bool AcceptsFocus() const;

    virtual bool IsDoubleBuffered() const { return true; }

public:
    static long         MacRemoveBordersFromStyle( long style ) ;

public:
    void OnInternalIdle();

    // For implementation purposes:
    // sometimes decorations make the client area smaller
    virtual wxPoint GetClientAreaOrigin() const;

    wxWindowMac *FindItem(long id) const;
    wxWindowMac *FindItemByHWND(WXHWND hWnd, bool controlOnly = false) const;

    virtual void        MacHandleControlClick( WXWidget control , wxInt16 controlpart , bool mouseStillDown ) ;
    virtual bool        MacDoRedraw( WXHRGN updatergn , long time ) ;
    virtual bool        MacCanFocus() const ;

    // this should not be overriden in classes above wxWindowMac
    // because it is called from its destructor via DeleteChildren
    virtual void        RemoveChild( wxWindowBase *child );
    virtual void        MacPaintBorders( int left , int top ) ;
    void                MacPaintGrowBox();

    // invalidates the borders and focus area around the control;
    // must not be virtual as it will be called during destruction
    void                MacInvalidateBorders() ;

    WXWindow            MacGetTopLevelWindowRef() const ;
    wxTopLevelWindowMac* MacGetTopLevelWindow() const ;

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

    bool                MacIsReallyShown() ;
    bool                MacIsReallyEnabled() ;
    bool                MacIsReallyHilited() ;

    bool                MacIsUserPane() { return m_macIsUserPane; }

    virtual bool        MacSetupCursor( const wxPoint& pt ) ;
    virtual void        MacSetBackgroundBrush( const wxBrush &brush ) ;
    const wxBrush&      MacGetBackgroundBrush() const { return m_macBackgroundBrush ; }

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
    bool                MacIsWindowScrollbar( const wxWindow* sb )
    { return ((wxWindow*)m_hScrollBar == sb || (wxWindow*)m_vScrollBar == sb) ; }

    virtual void        MacInstallEventHandler(WXWidget native) ;
    void                MacPostControlCreate(const wxPoint& pos, const wxSize& size) ;
    wxList&             GetSubcontrols() { return m_subControls; }
    WXEVENTHANDLERREF   MacGetControlEventHandler() { return m_macControlEventHandler ; }

#ifndef __WXMAC_OSX__
    virtual void            MacControlUserPaneDrawProc(wxInt16 part) ;
    virtual wxInt16         MacControlUserPaneHitTestProc(wxInt16 x, wxInt16 y) ;
    virtual wxInt16         MacControlUserPaneTrackingProc(wxInt16 x, wxInt16 y, void* actionProc) ;
    virtual void            MacControlUserPaneIdleProc() ;
    virtual wxInt16         MacControlUserPaneKeyDownProc(wxInt16 keyCode, wxInt16 charCode, wxInt16 modifiers) ;
    virtual void            MacControlUserPaneActivateProc(bool activating) ;
    virtual wxInt16         MacControlUserPaneFocusProc(wxInt16 action) ;
    virtual void            MacControlUserPaneBackgroundProc(void* info) ;
#endif

    // translate wxWidgets coords into ones suitable
    // to be passed to CreateControl calls
    //
    // returns true if non-default coords are returned, false otherwise
    bool                MacGetBoundsForControl(const wxPoint& pos,
                                           const wxSize& size,
                                           int& x, int& y,
                                           int& w, int& h , bool adjustForOrigin ) const ;

    // calculates the real window position and size from the native control
    void                MacGetPositionAndSizeFromControl(int& x, int& y,
                                           int& w, int& h) const ;

    // gets the inset from every part
    virtual void        MacGetContentAreaInset( int &left , int &top , int &right , int &bottom ) ;

    // visibly flash the current invalid area:
    // useful for debugging in OSX composited (double-buffered) situation
    void                MacFlashInvalidAreas() ;

    // the 'true' OS level control for this wxWindow
    wxMacControl*       GetPeer() const { return m_peer ; }

#if wxMAC_USE_CORE_GRAPHICS
    void *              MacGetCGContextRef() { return m_cgContextRef ; }
    void                MacSetCGContextRef(void * cg) { m_cgContextRef = cg ; }
#endif

protected:
    // For controls like radio buttons which are genuinely composite
    wxList              m_subControls;

    // number of calls to Freeze() minus number of calls to Thaw()
    unsigned int        m_frozenness;

    // the peer object, allowing for cleaner API support
    wxMacControl *       m_peer ;

#if wxMAC_USE_CORE_GRAPHICS
    void *              m_cgContextRef ;
#endif

    // cache the clipped rectangles within the window hierarchy
    void                MacUpdateClippedRects() const ;

    mutable bool        m_cachedClippedRectValid ;
    mutable wxRect      m_cachedClippedRectWithOuterStructure ;
    mutable wxRect      m_cachedClippedRect ;
    mutable wxRect      m_cachedClippedClientRect ;
    mutable wxRegion    m_cachedClippedRegionWithOuterStructure ;
    mutable wxRegion    m_cachedClippedRegion ;
    mutable wxRegion    m_cachedClippedClientRegion ;

    // true if is is not a native control but a wxWindow control
    bool                m_macIsUserPane ;
    wxBrush             m_macBackgroundBrush ;

    // insets of the mac control from the wx top left corner
    wxPoint             m_macTopLeftInset ;
    wxPoint             m_macBottomRightInset ;
    wxByte              m_macAlpha ;

    wxScrollBar*        m_hScrollBar ;
    wxScrollBar*        m_vScrollBar ;
    wxString            m_label ;

    // set to true if we do a sharp clip at the content area of this window
    // must be dynamic as eg a panel normally is not clipping precisely, but if
    // it becomes the target window of a scrolled window it has to...
    bool                m_clipChildren ;

    virtual bool        MacIsChildOfClientArea( const wxWindow* child ) const ;

    bool                MacHasScrollBarCorner() const;
    void                MacCreateScrollBars( long style ) ;
    void                MacRepositionScrollBars() ;
    void                MacUpdateControlFont() ;

    void                MacPropagateVisibilityChanged() ;
    void                MacPropagateEnabledStateChanged() ;
    void                MacPropagateHiliteChanged() ;

#if WXWIN_COMPATIBILITY_2_4
    bool                 m_backgroundTransparent ;
#endif

    // implement the base class pure virtuals
    virtual wxSize DoGetBestSize() const;
    virtual wxSize DoGetSizeFromClientSize( const wxSize & size ) const;
    virtual void DoClientToScreen( int *x, int *y ) const;
    virtual void DoScreenToClient( int *x, int *y ) const;
    virtual void DoGetPosition( int *x, int *y ) const;
    virtual void DoGetSize( int *width, int *height ) const;
    virtual void DoGetClientSize( int *width, int *height ) const;
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);

    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow( int x, int y, int width, int height );
    virtual void DoSetWindowVariant( wxWindowVariant variant );

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif

private:
    // common part of all ctors
    void Init();

    WXEVENTHANDLERREF    m_macControlEventHandler ;

    DECLARE_NO_COPY_CLASS(wxWindowMac)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_WINDOW_H_
