/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/frame.h
// Purpose:
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __MGL_FRAME_H__
#define __MGL_FRAME_H__

#ifdef __GNUG__
    #pragma interface "frame.h"
#endif

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class WXDLLEXPORT wxMDIChildFrame;
class WXDLLEXPORT wxMDIClientWindow;
class WXDLLEXPORT wxMenu;
class WXDLLEXPORT wxMenuBar;
class WXDLLEXPORT wxToolBar;
class WXDLLEXPORT wxStatusBar;

class WXDLLEXPORT wxFrame;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

//FIXME_MGL
class WXDLLEXPORT wxFrame : public wxFrameBase
{
public:
    // construction
    wxFrame() { Init(); }
    wxFrame(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxFrameNameStr)
    {
        Init();

        Create(parent, id, title, pos, size, style, name);
    }

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxFrameNameStr) {}

    virtual ~wxFrame() {}

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = TRUE) {}
    virtual bool IsMaximized() const {}
    virtual void Iconize(bool iconize = TRUE) {}
    virtual bool IsIconized() const {}
    virtual void SetIcon(const wxIcon& icon) {}
    virtual void MakeModal(bool modal = TRUE) {}
    virtual void Restore() {}

#if wxUSE_MENUS
    virtual void SetMenuBar( wxMenuBar *menuBar ) {}
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    virtual void PositionStatusBar() {}

    virtual wxStatusBar* CreateStatusBar(int number = 1,
                                         long style = wxST_SIZEGRIP,
                                         wxWindowID id = 0,
                                         const wxString& name = wxStatusLineNameStr) {}
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT,
                                     wxWindowID id = -1,
                                     const wxString& name = wxToolBarNameStr) {}
    void SetToolBar(wxToolBar *toolbar) {}
#endif // wxUSE_TOOLBAR

    virtual bool Show(bool show = TRUE) {}

    virtual void SetTitle( const wxString &title ) {}
    virtual wxString GetTitle() const { return m_title; }

    // implementation from now on
    // --------------------------

    // move the window to the specified location and resize it: this is called
    // from both DoSetSize() and DoSetClientSize() 
    virtual void DoMoveWindow(int x, int y, int width, int height) {}
    
    // GTK callbacks
    virtual void GtkOnSize( int x, int y, int width, int height ) {}
    virtual void OnInternalIdle() {}

    wxString      m_title;
    int           m_miniEdge,
                  m_miniTitle;
    bool          m_menuBarDetached;
    bool          m_toolBarDetached;
    bool          m_insertInClientArea;  /* not from within OnCreateXXX */

protected:
    // common part of all ctors
    void Init() {}

    // override wxWindow methods to take into account tool/menu/statusbars
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO) {}

    virtual void DoSetClientSize(int width, int height) {}
    virtual void DoGetClientSize( int *width, int *height ) const {}

private:
    DECLARE_DYNAMIC_CLASS(wxFrame)
};

#endif // __WX_FRAME_H__
