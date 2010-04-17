///////////////////////////////////////////////////////////////////////////////
// Name:        wx/qt/window.h
// Purpose:     wxWindow class
// Author:      Peter Most
// Created:     09/08/09
// RCS-ID:      $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_QT_WINDOW_H_
#define _WX_QT_WINDOW_H_

#include <QtCore/QPointer>
#include <QtGui/QWidget>

class WXDLLIMPEXP_CORE wxWindow : public wxWindowBase
{
public:
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
    
    virtual bool Show( bool show = true );

    virtual void SetLabel(const wxString& label);
    virtual wxString GetLabel() const;

    virtual void SetFocus();
    
    // Z-order
    virtual void Raise();
    virtual void Lower();
    
    // move the mouse to the specified position
    virtual void WarpPointer(int x, int y);

    virtual void Refresh( bool eraseBackground = true,
                          const wxRect *rect = (const wxRect *) NULL );

    virtual bool SetFont(const wxFont& font);
    
    // get the (average) character size for the current font
    virtual int GetCharHeight() const;
    virtual int GetCharWidth() const;
    
    virtual void SetScrollbar( int orient,
                               int pos,
                               int thumbvisible,
                               int range,
                               bool refresh = true );
    virtual void SetScrollPos( int orient, int pos, bool refresh = true );
    virtual int GetScrollPos( int orient ) const;
    virtual int GetScrollThumb( int orient ) const;
    virtual int GetScrollRange( int orient ) const;

        // scroll window to the specified position
    virtual void ScrollWindow( int dx, int dy,
                               const wxRect* rect = NULL );
    
    virtual WXWidget GetHandle() const;
    
    virtual void SetDropTarget( wxDropTarget *dropTarget );
    
protected:
    virtual void DoGetTextExtent(const wxString& string,
                                 int *x, int *y,
                                 int *descent = NULL,
                                 int *externalLeading = NULL,
                                 const wxFont *font = NULL) const;

    // coordinates translation
    virtual void DoClientToScreen( int *x, int *y ) const;
    virtual void DoScreenToClient( int *x, int *y ) const;

    // capture/release the mouse, used by Capture/ReleaseMouse()
    virtual void DoCaptureMouse();
    virtual void DoReleaseMouse();

    // retrieve the position/size of the window
    virtual void DoGetPosition(int *x, int *y) const;
    virtual void DoGetSize(int *width, int *height) const;
    virtual void DoGetClientSize(int *width, int *height) const;

    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    // same as DoSetSize() for the client size
    virtual void DoSetClientSize(int width, int height);
    
    virtual void DoMoveWindow(int x, int y, int width, int height);

#if wxUSE_TOOLTIPS
    virtual void DoSetToolTip( wxToolTip *tip );
#endif // wxUSE_TOOLTIPS

#if wxUSE_MENUS
    virtual bool DoPopupMenu(wxMenu *menu, int x, int y);
#endif // wxUSE_MENUS
        
private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS_NO_COPY( wxWindow )
};

#endif // _WX_QT_WINDOW_H_
