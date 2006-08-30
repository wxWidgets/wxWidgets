/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/mdi.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_MDI_H_
#define _WX_GTK_MDI_H_

#include "wx/frame.h"

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIParentFrame: public wxFrame
{
public:
    wxMDIParentFrame() { Init(); }
    wxMDIParentFrame(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                     const wxString& name = wxFrameNameStr)
    {
        Init();

        (void)Create(parent, id, title, pos, size, style, name);
    }

    ~wxMDIParentFrame();
    bool Create( wxWindow *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
                 const wxString& name = wxFrameNameStr );

    wxMDIChildFrame *GetActiveChild() const;

    wxMDIClientWindow *GetClientWindow() const;
    virtual wxMDIClientWindow *OnCreateClient();

    virtual void Cascade() {}
    virtual void Tile(wxOrientation WXUNUSED(orient) = wxHORIZONTAL) {}
    virtual void ArrangeIcons() {}
    virtual void ActivateNext();
    virtual void ActivatePrevious();

    // implementation

    wxMDIClientWindow  *m_clientWindow;
    bool                m_justInserted;

    virtual void GtkOnSize( int x, int y, int width, int height );
    virtual void OnInternalIdle();

protected:
    void Init();

    virtual void DoGetClientSize(int *width, int *height) const;

private:
    friend class wxMDIChildFrame;

    DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)
};

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIChildFrame: public wxFrame
{
public:
    wxMDIChildFrame();
    wxMDIChildFrame( wxMDIParentFrame *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const wxString& name = wxFrameNameStr );

    virtual ~wxMDIChildFrame();
    bool Create( wxMDIParentFrame *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxDEFAULT_FRAME_STYLE,
                 const wxString& name = wxFrameNameStr );

    virtual void SetMenuBar( wxMenuBar *menu_bar );
    virtual wxMenuBar *GetMenuBar() const;

    virtual void AddChild( wxWindowBase *child );

    virtual void Activate();

#if wxUSE_STATUSBAR
    // no status bars
    virtual wxStatusBar* CreateStatusBar( int WXUNUSED(number) = 1,
                                        long WXUNUSED(style) = 1,
                                        wxWindowID WXUNUSED(id) = 1,
                                        const wxString& WXUNUSED(name) = wxEmptyString)
      { return (wxStatusBar*)NULL; }

    virtual wxStatusBar *GetStatusBar() const { return (wxStatusBar*)NULL; }
    virtual void SetStatusText( const wxString &WXUNUSED(text), int WXUNUSED(number)=0 ) {}
    virtual void SetStatusWidths( int WXUNUSED(n), const int WXUNUSED(widths_field)[] ) {}
#endif

    // no size hints
    virtual void DoSetSizeHints( int WXUNUSED(minW),
                                 int WXUNUSED(minH),
                                 int WXUNUSED(maxW) = wxDefaultCoord,
                                 int WXUNUSED(maxH) = wxDefaultCoord,
                                 int WXUNUSED(incW) = wxDefaultCoord,
                                 int WXUNUSED(incH) = wxDefaultCoord) {}

#if wxUSE_TOOLBAR
    // no toolbar
    virtual wxToolBar* CreateToolBar( long WXUNUSED(style),
                                       wxWindowID WXUNUSED(id),
                                       const wxString& WXUNUSED(name) )
        { return (wxToolBar*)NULL; }
    virtual wxToolBar *GetToolBar() const { return (wxToolBar*)NULL; }
#endif // wxUSE_TOOLBAR

    // no icon
    virtual void SetIcon(const wxIcon& icon)
        { wxTopLevelWindowBase::SetIcon(icon); }
    virtual void SetIcons(const wxIconBundle& icons )
        { wxTopLevelWindowBase::SetIcons(icons); }

    // no title
    virtual void SetTitle( const wxString &title );

    // no maximize etc
    virtual void Maximize( bool WXUNUSED(maximize) = true ) { }
    virtual bool IsMaximized() const { return true; }
    virtual void Iconize(bool WXUNUSED(iconize) = true) { }
    virtual bool IsIconized() const { return false; }
    virtual void Restore() {}

    virtual bool IsTopLevel() const { return false; }

    void OnActivate( wxActivateEvent& event );
    void OnMenuHighlight( wxMenuEvent& event );

    // implementation

    wxMenuBar         *m_menuBar;
    GtkNotebookPage   *m_page;
    bool               m_justInserted;

protected:
    // override wxFrame methods to not do anything
    virtual void DoSetSize(int x, int y,
                           int width, int height,
                           int sizeFlags = wxSIZE_AUTO);
    virtual void DoSetClientSize(int width, int height);
    virtual void DoGetClientSize( int *width, int *height ) const;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
};

//-----------------------------------------------------------------------------
// wxMDIClientWindow
//-----------------------------------------------------------------------------

class WXDLLIMPEXP_CORE wxMDIClientWindow: public wxWindow
{
public:
    wxMDIClientWindow();
    wxMDIClientWindow( wxMDIParentFrame *parent, long style = 0 );
    ~wxMDIClientWindow();
    virtual bool CreateClient( wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL );

private:
    DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)
};

#endif // _WX_GTK_MDI_H_
