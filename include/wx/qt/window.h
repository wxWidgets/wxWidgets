///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/window.h
// Purpose:     wxWindow class
// Author:      Peter Most, Javier Torres, Mariano Reingart
// Copyright:   (c) 2009 wxWidgets dev team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_WINDOW_H_
#define _WX_QT_WINDOW_H_

#include <list>

class QShortcut;
template < class T > class QList;

class QWidget;
class QScrollWindow;
class QAbstractScrollArea;
class QScrollArea;
class QPicture;
class QPainter;

class QPaintEvent;
class QResizeEvent;
class QWheelEvent;
class QKeyEvent;
class QMouseEvent;
class QEvent;
class QMoveEvent;
class QEvent;
class QEvent;
class QCloseEvent;
class QContextMenuEvent;
class QFocusEvent;

class WXDLLIMPEXP_FWD_CORE wxScrollBar;
class WXDLLIMPEXP_FWD_CORE wxQtShortcutHandler;

/* wxQt specific notes:
 *
 * Remember to implement the Qt object getters on all subclasses:
 *  - GetHandle() returns the Qt object
 *  - QtGetScrollBarsContainer() returns the widget where scrollbars are placed
 * For example, for wxFrame, GetHandle() is the QMainWindow,
 * QtGetScrollBarsContainer() is the central widget and QtGetContainer() is a widget
 * in a layout inside the central widget that also contains the scrollbars.
 * Return 0 from QtGetScrollBarsContainer() to disable SetScrollBar() and friends
 * for wxWindow subclasses.
 *
 *
 * Event handling is achieved by using the template class wxQtEventForwarder
 * found in winevent_qt.(h|cpp) to send all Qt events here to QtHandleXXXEvent()
 * methods. All these methods receive the Qt event and the handler. This is
 * done because events of the containers (the scrolled part of the window) are
 * sent to the same wxWindow instance, that must be able to differenciate them
 * as some events need different handling (paintEvent) depending on that.
 * We pass the QWidget pointer to all event handlers for consistency.
 */
class WXDLLIMPEXP_CORE wxWindowQt : public wxWindowBase
{
public:
    wxWindowQt();
    ~wxWindowQt();
    wxWindowQt(wxWindowQt *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    bool Create(wxWindowQt *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    // Used by all window classes in the widget creation process.
    void PostCreation( bool generic = true );

    void AddChild( wxWindowBase *child ) wxOVERRIDE;

    virtual bool Show( bool show = true ) wxOVERRIDE;

    virtual void SetLabel(const wxString& label) wxOVERRIDE;
    virtual wxString GetLabel() const wxOVERRIDE;

    virtual void DoEnable( bool enable ) wxOVERRIDE;
    virtual void SetFocus() wxOVERRIDE;

    // Parent/Child:
    static void QtReparent( QWidget *child, QWidget *parent );
    virtual bool Reparent( wxWindowBase *newParent ) wxOVERRIDE;

    // Z-order
    virtual void Raise() wxOVERRIDE;
    virtual void Lower() wxOVERRIDE;

    // move the mouse to the specified position
    virtual void WarpPointer(int x, int y) wxOVERRIDE;

    virtual void Update() wxOVERRIDE;
    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL ) wxOVERRIDE;

    virtual bool SetCursor( const wxCursor &cursor ) wxOVERRIDE;
    virtual bool SetFont(const wxFont& font) wxOVERRIDE;

    // get the (average) character size for the current font
    virtual int GetCharHeight() const wxOVERRIDE;
    virtual int GetCharWidth() const wxOVERRIDE;

    virtual void SetScrollbar( int orient,
                               int pos,
                               int thumbvisible,
                               int range,
                               bool refresh = true ) wxOVERRIDE;
    virtual void SetScrollPos( int orient, int pos, bool refresh = true ) wxOVERRIDE;
    virtual int GetScrollPos( int orient ) const wxOVERRIDE;
    virtual int GetScrollThumb( int orient ) const wxOVERRIDE;
    virtual int GetScrollRange( int orient ) const wxOVERRIDE;

        // scroll window to the specified position
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = NULL ) wxOVERRIDE;

    // Styles
    virtual void SetWindowStyleFlag( long style ) wxOVERRIDE;
    virtual void SetExtraStyle( long exStyle ) wxOVERRIDE;

    virtual bool SetBackgroundStyle(wxBackgroundStyle style) wxOVERRIDE;
    virtual bool IsTransparentBackgroundSupported(wxString* reason = NULL) const wxOVERRIDE;
    virtual bool SetTransparent(wxByte alpha) wxOVERRIDE;
    virtual bool CanSetTransparent() wxOVERRIDE { return true; }

    QWidget *GetHandle() const wxOVERRIDE;

#if wxUSE_DRAG_AND_DROP
    virtual void SetDropTarget( wxDropTarget *dropTarget ) wxOVERRIDE;
#endif

#if wxUSE_ACCEL
    // accelerators
    // ------------
    virtual void SetAcceleratorTable( const wxAcceleratorTable& accel ) wxOVERRIDE;
#endif // wxUSE_ACCEL

    // wxQt implementation internals:

    virtual QPicture *QtGetPicture() const;

    QPainter *QtGetPainter();

    virtual bool QtHandlePaintEvent  ( QWidget *handler, QPaintEvent *event );
    virtual bool QtHandleResizeEvent ( QWidget *handler, QResizeEvent *event );
    virtual bool QtHandleWheelEvent  ( QWidget *handler, QWheelEvent *event );
    virtual bool QtHandleKeyEvent    ( QWidget *handler, QKeyEvent *event );
    virtual bool QtHandleMouseEvent  ( QWidget *handler, QMouseEvent *event );
    virtual bool QtHandleEnterEvent  ( QWidget *handler, QEvent *event );
    virtual bool QtHandleMoveEvent   ( QWidget *handler, QMoveEvent *event );
    virtual bool QtHandleShowEvent   ( QWidget *handler, QEvent *event );
    virtual bool QtHandleChangeEvent ( QWidget *handler, QEvent *event );
    virtual bool QtHandleCloseEvent  ( QWidget *handler, QCloseEvent *event );
    virtual bool QtHandleContextMenuEvent  ( QWidget *handler, QContextMenuEvent *event );
    virtual bool QtHandleFocusEvent  ( QWidget *handler, QFocusEvent *event );

    static void QtStoreWindowPointer( QWidget *widget, const wxWindowQt *window );
    static wxWindowQt *QtRetrieveWindowPointer( const QWidget *widget );

#if wxUSE_ACCEL
    virtual void QtHandleShortcut ( int command );
#endif // wxUSE_ACCEL

    virtual QScrollArea *QtGetScrollBarsContainer() const;

protected:
    virtual void DoGetTextExtent(const wxString& string,
                                 int *x, int *y,
                                 int *descent = NULL,
                                 int *externalLeading = NULL,
                                 const wxFont *font = NULL) const wxOVERRIDE;

    // coordinates translation
    virtual void DoClientToScreen( int *x, int *y ) const wxOVERRIDE;
    virtual void DoScreenToClient( int *x, int *y ) const wxOVERRIDE;

    // capture/release the mouse, used by Capture/ReleaseMouse()
    virtual void DoCaptureMouse() wxOVERRIDE;
    virtual void DoReleaseMouse() wxOVERRIDE;

    // retrieve the position/size of the window
    virtual void DoGetPosition(int *x, int *y) const wxOVERRIDE;

    virtual void DoSetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO) wxOVERRIDE;
    virtual void DoGetSize(int *width, int *height) const wxOVERRIDE;

    // same as DoSetSize() for the client size
    virtual void DoSetClientSize(int width, int height) wxOVERRIDE;
    virtual void DoGetClientSize(int *width, int *height) const wxOVERRIDE;

    virtual void DoMoveWindow(int x, int y, int width, int height) wxOVERRIDE;

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip ) wxOVERRIDE;
#endif // wxUSE_TOOLTIPS

#if wxUSE_MENUS
    virtual bool DoPopupMenu(wxMenu *menu, int x, int y) wxOVERRIDE;
#endif // wxUSE_MENUS

    QWidget *m_qtWindow;

private:
    void Init();
    QScrollArea *m_qtContainer;

    wxScrollBar *m_horzScrollBar;
    wxScrollBar *m_vertScrollBar;
    void QtOnScrollBarEvent( wxScrollEvent& event );
    
    wxScrollBar *QtGetScrollBar( int orientation ) const;
    wxScrollBar *QtSetScrollBar( int orientation, wxScrollBar *scrollBar=NULL );

    bool QtSetBackgroundStyle();

    QPicture *m_qtPicture;
    QPainter *m_qtPainter;

    bool m_mouseInside;

#if wxUSE_ACCEL
    QList< QShortcut* > *m_qtShortcuts;
    wxQtShortcutHandler *m_qtShortcutHandler;
    bool m_processingShortcut;
#endif // wxUSE_ACCEL

    wxDECLARE_DYNAMIC_CLASS_NO_COPY( wxWindowQt );
};

#endif // _WX_QT_WINDOW_H_
