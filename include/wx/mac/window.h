/////////////////////////////////////////////////////////////////////////////
// Name:        window.h
// Purpose:     wxWindowMac class
// Author:      AUTHOR
// Modified by:
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WINDOW_H_
#define _WX_WINDOW_H_

#ifdef __GNUG__
#pragma interface "window.h"
#endif

#include <wx/brush.h>
// ---------------------------------------------------------------------------
// forward declarations
// ---------------------------------------------------------------------------

class WXDLLEXPORT wxButton;
class WXDLLEXPORT wxScrollBar;
class WXDLLEXPORT wxTopLevelWindowMac;

// ---------------------------------------------------------------------------
// constants
// ---------------------------------------------------------------------------

// FIXME does anybody use those? they're unused by wxWindows...
enum
{
    wxKEY_SHIFT = 1,
    wxKEY_CTRL  = 2
};

class WXDLLEXPORT wxWindowMac: public wxWindowBase
{
  DECLARE_DYNAMIC_CLASS(wxWindowMac);

  friend class wxDC;
  friend class wxPaintDC;

public:
	
    wxWindowMac() { Init(); }

    wxWindowMac(wxWindowMac *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxPanelNameStr)
    {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    virtual ~wxWindowMac();

    bool Create(wxWindowMac *parent,
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
    virtual bool SetFont(const wxFont& font)
           { return wxWindowBase::SetFont(font); }
    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    virtual void GetTextExtent(const wxString& string,
                               int *x, int *y,
                               int *descent = (int *) NULL,
                               int *externalLeading = (int *) NULL,
                               const wxFont *theFont = (const wxFont *) NULL)
                               const;

    virtual bool DoPopupMenu( wxMenu *menu, int x, int y );

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

#if WXWIN_COMPATIBILITY
    // Set/get scroll attributes
    virtual void SetScrollRange(int orient, int range, bool refresh = TRUE);
    virtual void SetScrollPage(int orient, int page, bool refresh = TRUE);
    virtual int OldGetScrollRange(int orient) const;
    virtual int GetScrollPage(int orient) const;

    // event handlers
        // Handle a control command
    virtual void OnCommand(wxWindowMac& win, wxCommandEvent& event);

        // Override to define new behaviour for default action (e.g. double
        // clicking on a listbox)
    virtual void OnDefaultAction(wxControl * WXUNUSED(initiatingItem)) { }
#endif // WXWIN_COMPATIBILITY

#if wxUSE_CARET && WXWIN_COMPATIBILITY
    // caret manipulation (old MSW only functions, see wxCaret class for the
    // new API)
    void CreateCaret(int w, int h);
    void CreateCaret(const wxBitmap *bitmap);
    void DestroyCaret();
    void ShowCaret(bool show);
    void SetCaretPos(int x, int y);
    void GetCaretPos(int *x, int *y) const;
#endif // wxUSE_CARET

    // Native resource loading (implemented in src/msw/nativdlg.cpp)
    // FIXME: should they really be all virtual?
    wxWindowMac* GetWindowChild1(wxWindowID id);
    wxWindowMac* GetWindowChild(wxWindowID id);

    // implementation from now on
    // --------------------------

	void MacClientToRootWindow( int *x , int *y ) const ;
	void MacRootWindowToClient( int *x , int *y ) const ;
	void MacWindowToRootWindow( int *x , int *y ) const ;
	void MacRootWindowToWindow( int *x , int *y ) const ;
	
	virtual wxString MacGetToolTipString( wxPoint &where ) ;

    // simple accessors
    // ----------------

//    WXHWND GetHWND() const { return m_hWnd; }
//    void SetHWND(WXHWND hWnd) { m_hWnd = hWnd; }
	virtual WXWidget GetHandle() const { return (WXWidget) NULL ; }

    bool GetUseCtl3D() const { return m_useCtl3D; }
    bool GetTransparentBackground() const { return m_backgroundTransparent; }
    void SetTransparent(bool t = TRUE) { m_backgroundTransparent = t; }

    // event handlers
    // --------------
	void OnSetFocus(wxFocusEvent& event) ;
    void OnNcPaint(wxNcPaintEvent& event);
    void OnEraseBackground(wxEraseEvent& event);
    void OnIdle(wxIdleEvent& event);
    void MacOnScroll(wxScrollEvent&event ) ;
    
    bool AcceptsFocus() const ;

public:
    // For implementation purposes - sometimes decorations make the client area
    // smaller
    virtual wxPoint GetClientAreaOrigin() const;

    wxWindowMac *FindItem(long id) const;
    wxWindowMac *FindItemByHWND(WXHWND hWnd, bool controlOnly = FALSE) const;

    // Make a Windows extended style from the given wxWindows window style
    static WXDWORD MakeExtendedStyle(long style,
                                     bool eliminateBorders = TRUE);
    // Determine whether 3D effects are wanted
    WXDWORD Determine3DEffects(WXDWORD defaultBorderStyle, bool *want3D) const;

    // MSW only: TRUE if this control is part of the main control
    virtual bool ContainsHWND(WXHWND WXUNUSED(hWnd)) const { return FALSE; };

 #if WXWIN_COMPATIBILITY
    wxObject *GetChild(int number) const;
    virtual void MSWDeviceToLogical(float *x, float *y) const;
#endif // WXWIN_COMPATIBILITY

    // Setup background and foreground colours correctly
    virtual void SetupColours();


#if WXWIN_COMPATIBILITY
    void SetShowing(bool show) { (void)Show(show); }
    bool IsUserEnabled() const { return IsEnabled(); }
#endif // WXWIN_COMPATIBILITY

public :
	static bool							MacGetWindowFromPoint( const wxPoint &point , wxWindowMac** outWin ) ;
	virtual	void						MacRedraw( RgnHandle updatergn , long time , bool erase) ;
	virtual bool						MacCanFocus() const { return true ; }

	virtual bool						MacDispatchMouseEvent(wxMouseEvent& event ) ;

	virtual void 						MacPaintBorders( int left , int top ) ;
	WindowRef						    MacGetRootWindow() const  ;
	wxTopLevelWindowMac*                MacGetTopLevelWindow() const ;

	virtual ControlHandle 				MacGetContainerForEmbedding() ;
	
	virtual long						MacGetLeftBorderSize() const ;
	virtual long						MacGetRightBorderSize() const ;
	virtual long						MacGetTopBorderSize() const ;
	virtual long						MacGetBottomBorderSize() const ;

	static long							MacRemoveBordersFromStyle( long style ) ;
	virtual void 						MacSuperChangedPosition() ;
	virtual void                        MacTopLevelWindowChangedPosition() ;
	virtual void						MacSuperShown( bool show ) ;
	virtual void                        MacSuperEnabled( bool enable ) ;
	bool								MacIsReallyShown() const ;
	virtual void                        Update() ;
	// for compatibility
	void                                MacUpdateImmediately() { Update() ; }
	
//	virtual bool						MacSetPortDrawingParams( const Point & localOrigin, const Rect & clipRect, WindowRef window , wxWindowMac* rootwin )  ;
//	virtual void						MacGetPortParams(Point* localOrigin, Rect* clipRect, WindowRef *window , wxWindowMac** rootwin ) ;
//	virtual void						MacGetPortClientParams(Point* localOrigin, Rect* clipRect, WindowRef *window  , wxWindowMac** rootwin) ;
	const wxBrush&                      MacGetBackgroundBrush() ;
    const wxRegion&                     MacGetVisibleRegion() ;
	bool								MacIsWindowScrollbar( const wxScrollBar* sb ) { return (m_hScrollBar == sb || m_vScrollBar == sb) ; }
	static wxWindowMac*					s_lastMouseWindow ;
private:
	virtual bool						MacGetWindowFromPointSub( const wxPoint &point , wxWindowMac** outWin ) ;
protected:
//	RgnHandle					m_macUpdateRgn ;
//	bool						m_macEraseOnRedraw ;
    wxBrush                                 m_macBackgroundBrush ;
    wxRegion                                m_macVisibleRegion ;
	int 									m_x ;
	int 									m_y ;	
	int 									m_width ;
	int 									m_height ;

	wxScrollBar*					m_hScrollBar ;
	wxScrollBar*					m_vScrollBar ;
	wxString						m_label ;

	void									MacCreateScrollBars( long style ) ;
	void									MacRepositionScrollBars() ;

    // additional (MSW specific) flags
    bool                  m_useCtl3D:1; // Using CTL3D for this control
    bool                  m_backgroundTransparent:1;
    bool                  m_mouseInWindow:1;
    bool                  m_doubleClickAllowed:1;
    bool                  m_winCaptured:1;

    // the size of one page for scrolling
    int                   m_xThumbSize;
    int                   m_yThumbSize;

//    WXHMENU               m_hMenu; // Menu, if any

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

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() and would usually just call
    // ::MoveWindow() except for composite controls which will want to arrange
    // themselves inside the given rectangle
    virtual void DoMoveWindow(int x, int y, int width, int height);

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

private:
    // common part of all ctors
    void Init();

    DECLARE_NO_COPY_CLASS(wxWindowMac)
    DECLARE_EVENT_TABLE()
};

class wxMacDrawingHelper
{
public :
	wxMacDrawingHelper( wxWindowMac * theWindow , bool clientArea = false ) ;
	~wxMacDrawingHelper() ;
	bool Ok() { return m_ok ; }
	void LocalToWindow( Rect *rect) { OffsetRect( rect , m_origin.h , m_origin.v ) ; }
	void LocalToWindow( Point *pt ) { AddPt( m_origin , pt ) ; }
	void LocalToWindow( RgnHandle rgn ) { OffsetRgn( rgn , m_origin.h , m_origin.v ) ; }
  const Point& GetOrigin() { return m_origin ; }
private :
  Point     m_origin ;
	GrafPtr 	m_formerPort ;
	GrafPtr		m_currentPort ;
	PenState 	m_savedPenState ;
	bool			m_ok ;
} ;

#endif
    // _WX_WINDOW_H_
