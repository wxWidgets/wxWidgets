/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKFRAMEH__
#define __GTKFRAMEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/window.h"
#include "wx/icon.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMDIChildFrame;
class wxMDIClientWindow;
class wxMenu;
class wxMenuBar;
class wxToolBar;
class wxStatusBar;

class wxFrame;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar *wxFrameNameStr;
extern const wxChar *wxToolBarNameStr;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

class wxFrame: public wxWindow
{
DECLARE_DYNAMIC_CLASS(wxFrame)

public:
    wxFrame();
    wxFrame( wxWindow *parent, wxWindowID id, const wxString &title,
            const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
    bool Create( wxWindow *parent, wxWindowID id, const wxString &title,
            const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
    ~wxFrame();
    bool Destroy();

    virtual bool Show( bool show );
    virtual void Centre( int direction = wxHORIZONTAL );

    virtual wxStatusBar* CreateStatusBar(int number=1, long style = wxST_SIZEGRIP, wxWindowID id = 0,
            const wxString& name = _T("statusBar"));
    virtual wxStatusBar *OnCreateStatusBar( int number, long style, wxWindowID id,
            const wxString& name );
    virtual wxStatusBar *GetStatusBar() const;
    inline void SetStatusBar(wxStatusBar *statusBar) { m_frameStatusBar = statusBar; }
    virtual void SetStatusText( const wxString &text, int number = 0 );
    virtual void SetStatusWidths( int n, const int widths_field[] );

    virtual wxToolBar* CreateToolBar( long style = wxNO_BORDER|wxTB_HORIZONTAL, wxWindowID id = -1,
            const wxString& name = wxToolBarNameStr);
    virtual wxToolBar *OnCreateToolBar( long style, wxWindowID id, const wxString& name );
    virtual wxToolBar *GetToolBar() const;
    inline void SetToolBar(wxToolBar *toolbar) { m_frameToolBar = toolbar; }

    virtual void SetMenuBar( wxMenuBar *menuBar );
    virtual wxMenuBar *GetMenuBar() const;

    virtual void SetTitle( const wxString &title );
    virtual wxString GetTitle() const { return m_title; }

        // make the window modal (all other windows unresponsive)
    virtual void MakeModal(bool modal = TRUE);

    virtual void SetIcon( const wxIcon &icon );
    virtual void Iconize( bool WXUNUSED(iconize)) { }
    virtual bool IsIconized() const { return FALSE; }
    bool Iconized() const { return IsIconized(); }
    virtual void Maximize(bool WXUNUSED(maximize)) {}
    virtual void Restore() {}

    virtual void Command( int id );
    
    void OnCloseWindow( wxCloseEvent& event );
    void OnActivate( wxActivateEvent &WXUNUSED(event) ) { } // called from docview.cpp
    void OnSize( wxSizeEvent &event );

    void OnMenuHighlight( wxMenuEvent& event );

    // implementation

    virtual void GtkOnSize( int x, int y, int width, int height );
    void DoMenuUpdates();
    void DoMenuUpdates(wxMenu* menu, wxWindow* focusWin);
    virtual void OnInternalIdle();

    wxMenuBar    *m_frameMenuBar;
    wxStatusBar  *m_frameStatusBar;
    wxToolBar    *m_frameToolBar;
    wxString      m_title;
    wxIcon        m_icon;
    int           m_miniEdge,m_miniTitle;
    GtkWidget    *m_mainWidget;
    bool          m_menuBarDetached;
    bool          m_toolBarDetached;
    bool          m_insertInClientArea;  /* not from within OnCreateXXX */

protected:
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    virtual void DoSetClientSize(int width, int height);
    virtual void DoGetClientSize( int *width, int *height ) const;

private:
    DECLARE_EVENT_TABLE()
};

#endif // __GTKFRAMEH__
