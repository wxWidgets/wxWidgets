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

    wxCONTROL_FLAGS_MASK = 0x0000001f
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
        : wxWindowNative(parent, id, pos, size, style, name)
        { Init(); }

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

    virtual void SetScrollbar( int orient,
                               int pos,
                               int thumbVisible,
                               int range,
                               bool refresh = TRUE );
    virtual void SetScrollPos( int orient, int pos, bool refresh = TRUE );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;

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

protected:
    // common part of all ctors
    void Init();

    // event handlers
    void OnPaint(wxPaintEvent& event);
    void OnErase(wxEraseEvent& event);

    // returns the (low level) renderer to use for drawing the control by
    // querying the current theme
    wxRenderer *GetRenderer() const;

    // draw the control background, return TRUE if done
    virtual bool DoDrawBackground(wxControlRenderer *renderer);

    // draw the controls contents
    virtual void DoDraw(wxControlRenderer *renderer);

    // adjust the size of the window to take into account its borders
    wxSize AdjustSize(const wxSize& size) const;

    // background bitmap info
    wxBitmap  m_bitmapBg;
    int       m_alignBgBitmap;
    wxStretch m_stretchBgBitmap;

    // is the mouse currently inside the window?
    bool m_isCurrent;

private:
    // the window scrollbars
    wxScrollBar *m_scrollbarHorz,
                *m_scrollbarVert;

    DECLARE_DYNAMIC_CLASS(wxWindow)
    DECLARE_EVENT_TABLE()
};

#endif // _WX_UNIX_WINDOW_H_
