///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/window.h
// Purpose:     wxWindow class which is the base class for all
//              wxUniv port controls, it supports the customization of the
//              window drawing and input processing.
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIV_WINDOW_H_
#define _WX_UNIV_WINDOW_H_

#ifdef __GNUG__
    #pragma interface "univwindow.h"
#endif

#include "wx/bitmap.h"      // for m_bitmapBg

class WXDLLEXPORT wxControlRenderer;
class WXDLLEXPORT wxEventLoop;
class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxRenderer;
class WXDLLEXPORT wxScrollBar;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// control state flags used in wxRenderer and wxColourScheme
enum
{
    wxCONTROL_DISABLED   = 0x00000001,  // control is disabled
    wxCONTROL_FOCUSED    = 0x00000002,  // currently has keyboard focus
    wxCONTROL_PRESSED    = 0x00000004,  // (button) is pressed
    wxCONTROL_ISDEFAULT  = 0x00000008,  // only applies to the buttons
    wxCONTROL_ISSUBMENU  = wxCONTROL_ISDEFAULT, // only for menu items
    wxCONTROL_CURRENT    = 0x00000010,  // mouse is currently over the control
    wxCONTROL_SELECTED   = 0x00000020,  // selected item in e.g. listbox
    wxCONTROL_CHECKED    = 0x00000040,  // (check/radio button) is checked
    wxCONTROL_CHECKABLE  = 0x00000080,  // (menu) item can be checked

    wxCONTROL_FLAGS_MASK = 0x000000ff,

    // this is a pseudo flag not used directly by wxRenderer but rather by some
    // controls internally
    wxCONTROL_DIRTY      = 0x80000000
};

#ifdef __WXX11__
#define wxUSE_TWO_WINDOWS 1
#else
#define wxUSE_TWO_WINDOWS 0
#endif

// ----------------------------------------------------------------------------
// wxWindow
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
#define wxWindowNative wxWindowMSW
#elif defined(__WXGTK__)
#define wxWindowNative wxWindowGTK
#elif defined(__WXMGL__)
#define wxWindowNative wxWindowMGL
#elif defined(__WXX11__)
#define wxWindowNative wxWindowX11
#elif defined(__WXMAC__)
#define wxWindowNative wxWindowMac
#endif

class WXDLLEXPORT wxWindow : public wxWindowNative
{
public:
    // ctors and create functions
    // ---------------------------

    wxWindow() { Init(); }

    wxWindow(wxWindow *parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             const wxString& name = wxPanelNameStr)
        : wxWindowNative(parent, id, pos, size, style | wxCLIP_CHILDREN, name)
        { Init(); }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPanelNameStr);

    // background pixmap support
    // -------------------------

    virtual void SetBackground(const wxBitmap& bitmap,
                               int alignment = wxALIGN_CENTRE,
                               wxStretch stretch = wxSTRETCH_NOT);

    const wxBitmap& GetBackgroundBitmap(int *alignment = NULL,
                                        wxStretch *stretch = NULL) const;

    // scrollbars: we (re)implement it ourselves using our own scrollbars
    // instead of the native ones
    // ------------------------------------------------------------------

    virtual void SetScrollbar(int orient,
                              int pos,
                              int page,
                              int range,
                              bool refresh = TRUE );
    virtual void SetScrollPos(int orient, int pos, bool refresh = TRUE);
    virtual int GetScrollPos(int orient) const;
    virtual int GetScrollThumb(int orient) const;
    virtual int GetScrollRange(int orient) const;
    virtual void ScrollWindow(int dx, int dy,
                              const wxRect* rect = (wxRect *) NULL);

    // take into account the borders here
    virtual wxPoint GetClientAreaOrigin() const;

    // popup menu support
    // ------------------

    // NB: all menu related functions are implemented in menu.cpp

#if wxUSE_MENUS
    virtual bool DoPopupMenu(wxMenu *menu, int x, int y);

    // this is wxUniv-specific private method to be used only by wxMenu
    void DismissPopupMenu();
#endif // wxUSE_MENUS

    // miscellaneous other methods
    // ---------------------------

    // get the state information
    virtual bool IsFocused() const;
    virtual bool IsCurrent() const;
    virtual bool IsPressed() const;
    virtual bool IsDefault() const;

    // return all state flags at once (combination of wxCONTROL_XXX values)
    int GetStateFlags() const;

    // set the "highlighted" flag and return TRUE if it changed
    virtual bool SetCurrent(bool doit = TRUE);

    // get the scrollbar (may be NULL) for the given orientation
    wxScrollBar *GetScrollbar(int orient) const
    {
        return orient & wxVERTICAL ? m_scrollbarVert : m_scrollbarHorz;
    }

    // methods used by wxColourScheme to choose the colours for this window
    // --------------------------------------------------------------------

    // return TRUE if this is a panel/canvas window which contains other
    // controls only
    virtual bool IsCanvasWindow() const { return FALSE; }

    // return TRUE if this a container window which contains the other items:
    // e.g, a listbox, listctrl, treectrl, ... and FALSE if it is a monolithic
    // control (e.g. a button, checkbox, ...)
    virtual bool IsContainerWindow() const { return FALSE; }

    // returns TRUE if the control has "transparent" areas such
    // as a wxStaticText and wxCheckBox and the background should
    // be adapted from a parent window
    virtual bool HasTransparentBackground() { return FALSE; }
    
    // to be used with function above: transparent windows get
    // their background from parents that return TRUE here,
    // so this is mostly for wxPanel, wxTopLevelWindow etc.
    virtual bool ProvidesBackground() const { return FALSE; }

    // return TRUE if this control can be highlighted when the mouse is over
    // it (the theme decides itself whether it is really highlighted or not)
    virtual bool CanBeHighlighted() const { return FALSE; }

    // return TRUE if we should use the colours/fonts returned by the
    // corresponding GetXXX() methods instead of the default ones
    bool UseBgCol() const { return m_hasBgCol; }
    bool UseFgCol() const { return m_hasFgCol; }
    bool UseFont() const { return m_hasFont; }

    // return TRUE if this window serves as a container for the other windows
    // only and doesn't get any input itself
    virtual bool IsStaticBox() const { return FALSE; }

    // returns the (low level) renderer to use for drawing the control by
    // querying the current theme
    wxRenderer *GetRenderer() const { return m_renderer; }

    // scrolling helper: like ScrollWindow() except that it doesn't refresh the
    // uncovered window areas but returns the rectangle to update (don't call
    // this with both dx and dy non zero)
    wxRect ScrollNoRefresh(int dx, int dy, const wxRect *rect = NULL);

    // after scrollbars are added or removed they must be refreshed by calling
    // this function
    void RefreshScrollbars();

    // erase part of the control
    virtual void EraseBackground(wxDC& dc, const wxRect& rect);
    
    // overridden base class methods
    // -----------------------------

    // the rect coordinates are, for us, in client coords, but if no rect is
    // specified, the entire window is refreshed
    virtual void Refresh(bool eraseBackground = TRUE,
                         const wxRect *rect = (const wxRect *) NULL);

    // we refresh the window when it is dis/enabled
    virtual bool Enable(bool enable = TRUE);

protected:
    // common part of all ctors
    void Init();

    // overridden base class virtuals

    // we deal with the scrollbars in these functions
    virtual void DoSetClientSize(int width, int height);
    virtual void DoGetClientSize(int *width, int *height) const;
    virtual wxHitTest DoHitTest(wxCoord x, wxCoord y) const;

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnNcPaint(wxPaintEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnErase(wxEraseEvent& event);

#if wxUSE_ACCEL || wxUSE_MENUS
    void OnKeyDown(wxKeyEvent& event);
#endif // wxUSE_ACCEL

#if wxUSE_MENUS
    void OnChar(wxKeyEvent& event);
    void OnKeyUp(wxKeyEvent& event);
#endif // wxUSE_MENUS

    // draw the control background, return TRUE if done
    virtual bool DoDrawBackground(wxDC& dc);

    // draw the controls border
    virtual void DoDrawBorder(wxDC& dc, const wxRect& rect);

    // draw the controls contents
    virtual void DoDraw(wxControlRenderer *renderer);

    // calculate the best size for the client area of the window: default
    // implementation of DoGetBestSize() uses this method and adds the border
    // width to the result
    virtual wxSize DoGetBestClientSize() const;
    virtual wxSize DoGetBestSize() const;

    // adjust the size of the window to take into account its borders
    wxSize AdjustSize(const wxSize& size) const;

    // put the scrollbars along the edges of the window
    void PositionScrollbars();

#if wxUSE_MENUS
    // return the menubar of the parent frame or NULL
    wxMenuBar *GetParentFrameMenuBar() const;
#endif // wxUSE_MENUS

    // the renderer we use
    wxRenderer *m_renderer;
    
    // background bitmap info
    wxBitmap  m_bitmapBg;
    int       m_alignBgBitmap;
    wxStretch m_stretchBgBitmap;
    
    // old size
    wxSize m_oldSize;

    // is the mouse currently inside the window?
    bool m_isCurrent:1;

#ifdef __WXMSW__
    // override MSWWindowProc() to process WM_NCHITTEST
    long MSWWindowProc(WXUINT message, WXWPARAM wParam, WXLPARAM lParam);
#endif // __WXMSW__

private:
    // the window scrollbars
    wxScrollBar *m_scrollbarHorz,
                *m_scrollbarVert;

#if wxUSE_MENUS
    // the current modal event loop for the popup menu we show or NULL
    static wxEventLoop *ms_evtLoopPopup;

    // the last window over which Alt was pressed (used by OnKeyUp)
    static wxWindow *ms_winLastAltPress;
#endif // wxUSE_MENUS

    DECLARE_DYNAMIC_CLASS(wxWindow)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_UNIV_WINDOW_H_

