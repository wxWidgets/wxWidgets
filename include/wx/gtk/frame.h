/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/frame.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKFRAMEH__
#define __GTKFRAMEH__

#ifdef __GNUG__
    #pragma interface "frame.h"
#endif

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
// wxFrame
//-----------------------------------------------------------------------------

class wxFrame : public wxFrameBase
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
                const wxString& name = wxFrameNameStr);

    virtual ~wxFrame();

    // implement base class pure virtuals
    virtual void Maximize(bool maximize = TRUE);
    virtual bool IsMaximized() const;
    virtual void Iconize(bool iconize = TRUE);
    virtual bool IsIconized() const;
    virtual void SetIcon(const wxIcon& icon);
    virtual void MakeModal(bool modal = TRUE);
    virtual void Restore();

    virtual void SetMenuBar( wxMenuBar *menuBar );

#if wxUSE_STATUSBAR
    virtual wxStatusBar* CreateStatusBar(int number = 1,
                                         long style = wxST_SIZEGRIP,
                                         wxWindowID id = 0,
                                         const wxString& name = wxStatusLineNameStr);
#endif // wxUSE_STATUSBAR

#if wxUSE_TOOLBAR
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER | wxTB_HORIZONTAL | wxTB_FLAT,
                                     wxWindowID id = -1,
                                     const wxString& name = wxToolBarNameStr);
    void SetToolBar(wxToolBar *toolbar);
#endif // wxUSE_TOOLBAR

    virtual bool Show(bool show);

    virtual void SetTitle( const wxString &title );
    virtual wxString GetTitle() const { return m_title; }

    // implementation from now on
    // --------------------------

    // GTK callbacks
    virtual void GtkOnSize( int x, int y, int width, int height );
    virtual void OnInternalIdle();

    wxString      m_title;
    int           m_miniEdge,
                  m_miniTitle;
    GtkWidget    *m_mainWidget;
    bool          m_menuBarDetached;
    bool          m_toolBarDetached;
    bool          m_insertInClientArea;  /* not from within OnCreateXXX */

protected:
    // common part of all ctors
    void Init();

    // override wxWindow methods to take into account tool/menu/statusbars
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);

    virtual void DoSetClientSize(int width, int height);
    virtual void DoGetClientSize( int *width, int *height ) const;

private:
    DECLARE_DYNAMIC_CLASS(wxFrame)
};

#endif // __GTKFRAMEH__
