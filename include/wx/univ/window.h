///////////////////////////////////////////////////////////////////////////////
// Name:        wx/univ/window.h
// Purpose:     wxWindow class which is the base class for all
//              wxUniv port controls, it supports the customization of the
//              window drawing and input processing.
// Author:      Vadim Zeitlin
// Modified by:
// Created:     06.08.00
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UNIX_WINDOW_H_
#define _WX_UNIX_WINDOW_H_

#ifdef __GNUG__
    #pragma interface "univwindow.h"
#endif

#include "wx/bitmap.h"      // for m_bitmapBg

class WXDLLEXPORT wxControlRenderer;
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
    wxCONTROL_CURRENT    = 0x00000010,  // mouse is currently over the control
    wxCONTROL_SELECTED   = 0x00000020,  // selected item in e.g. listbox
    wxCONTROL_CHECKED    = 0x00000040,  // (check/radio button) is checked

    wxCONTROL_FLAGS_MASK = 0x0000007f,

    // this is a pseudo flag not used directly by wxRenderer but rather by some
    // controls internally
    wxCONTROL_DIRTY      = 0x80000000
};

// ----------------------------------------------------------------------------
// wxWindow
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxWindow : public wxWindowNative
{
public:
    // ctors and creatie functions
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

    // miscellaneous other methods
    // ---------------------------

    // get the state information
    virtual bool IsFocused() const;
    virtual bool IsCurrent() const;
    virtual bool IsPressed() const;
    virtual bool IsDefault() const;

    // return all state flags at once (combination of wxCONTROL_XXX values)
    int GetStateFlags() const;

    // operations
    virtual void SetCurrent(bool doit = TRUE);

    // methods used by wxColourScheme to choose the colours for this window
    // --------------------------------------------------------------------

    // return TRUE if this is a panel/canvas window which contains other
    // controls only
    virtual bool IsCanvasWindow() const { return FALSE; }

    // return TRUE if this a container window which contains the other items:
    // e.g, a listbox, listctrl, treectrl, ... and FALSE if it is a monolithic
    // control (e.g. a button, checkbox, ...)
    virtual bool IsContainerWindow() const { return FALSE; }

    // return TRUE if this control can be highlighted when the mouse is over
    // it (the theme decides itself whether it is really highlighted or not)
    virtual bool CanBeHighlighted() const { return FALSE; }

    // return TRUE if we should use the colours/fonts returned by the
    // corresponding GetXXX() methods instead of the default ones
    bool UseBgCol() const { return m_hasBgCol; }
    bool UseFgCol() const { return m_hasFgCol; }
    bool UseFont() const { return m_hasFont; }

    // overridden base class methods
    // -----------------------------

    // remember that the font/colour was changed
    virtual bool SetBackgroundColour(const wxColour& colour);
    virtual bool SetForegroundColour(const wxColour& colour);
    virtual bool SetFont(const wxFont& font);

protected:
    // common part of all ctors
    void Init();

    // overridden base class virtuals

    // we deal with the scrollbars in these functions
    virtual void DoSetClientSize(int width, int height);
    virtual void DoGetClientSize(int *width, int *height) const;

    // event handlers
    void OnSize(wxSizeEvent& event);
    void OnPaint(wxPaintEvent& event);
    void OnErase(wxEraseEvent& event);

    // returns the (low level) renderer to use for drawing the control by
    // querying the current theme
    wxRenderer *GetRenderer() const { return m_renderer; }

    // draw the control background, return TRUE if done
    virtual bool DoDrawBackground(wxControlRenderer *renderer);

    // draw the controls border
    virtual void DoDrawBorder(wxControlRenderer *renderer);

    // draw the controls contents
    virtual void DoDraw(wxControlRenderer *renderer);

    // calculate the best size for the client area of the window: default
    // implementation of DoGetBestSize() uses this method and adds the border
    // width to the result
    virtual wxSize DoGetBestClientSize() const;
    virtual wxSize DoGetBestSize() const;

    // adjust the size of the window to take into account its borders
    wxSize AdjustSize(const wxSize& size) const;

    // get the scrollbar (may be NULL) for the given orientation
    wxScrollBar *GetScrollbar(int orient) const
    {
        return orient & wxVERTICAL ? m_scrollbarVert : m_scrollbarHorz;
    }

    // put the scrollbars along the edges of the window
    void PositionScrollbars();

    // the renderer we use
    wxRenderer *m_renderer;

    // background bitmap info
    wxBitmap  m_bitmapBg;
    int       m_alignBgBitmap;
    wxStretch m_stretchBgBitmap;

    // more flags
    bool m_isCurrent:1; // is the mouse currently inside the window?
    bool m_hasBgCol:1;  // was the bg colour explicitly changed by user?
    bool m_hasFgCol:1;  //         fg
    bool m_hasFont:1;   //         font

private:
    // the window scrollbars
    wxScrollBar *m_scrollbarHorz,
                *m_scrollbarVert;

    DECLARE_DYNAMIC_CLASS(wxWindow)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_UNIX_WINDOW_H_
