/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/mdi.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __MDIH__
#define __MDIH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/panel.h"
#include "wx/frame.h"
#include "wx/toolbar.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMDIParentFrame;
class wxMDIClientWindow;
class wxMDIChildFrame;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const wxChar* wxFrameNameStr;
extern const wxChar* wxStatusLineNameStr;

//-----------------------------------------------------------------------------
// wxMDIParentFrame
//-----------------------------------------------------------------------------

class wxMDIParentFrame: public wxFrame
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

    ~wxMDIParentFrame(void);
    bool Create( wxWindow *parent,
       wxWindowID id, const wxString& title,
       const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
       long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
       const wxString& name = wxFrameNameStr );

    wxMDIChildFrame *GetActiveChild() const;

    wxMDIClientWindow *GetClientWindow() const;
    virtual wxMDIClientWindow *OnCreateClient();

    virtual void Cascade() {}
    virtual void Tile() {}
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

class wxMDIChildFrame: public wxFrame
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
    virtual void SetSizeHints( int WXUNUSED(minW), int WXUNUSED(minH),
                               int WXUNUSED(maxW), int WXUNUSED(maxH),
                               int WXUNUSED(incW), int WXUNUSED(incH) ) {}

#if wxUSE_TOOLBAR
    // no toolbar bars
    virtual wxToolBar* CreateToolBar( long WXUNUSED(style),
                                       wxWindowID WXUNUSED(id),
                                       const wxString& WXUNUSED(name) )
        { return (wxToolBar*)NULL; }
    virtual wxToolBar *GetToolBar() const { return (wxToolBar*)NULL; }
#endif

    // no icon
    void SetIcon( const wxIcon &icon ) { m_icon = icon; }

    // no title
    void SetTitle( const wxString &title );
    wxString GetTitle() const { return m_title; }

    // no maximize etc
    virtual void Maximize( bool WXUNUSED(maximize) ) {}
    virtual void Restore() {}

    virtual bool IsTopLevel() const { return FALSE; }

    void OnActivate( wxActivateEvent& event );
    void OnMenuHighlight( wxMenuEvent& event );

    // implementation

    wxMenuBar         *m_menuBar;
    GtkNotebookPage   *m_page;
    bool               m_justInserted;

protected:
    virtual void DoGetClientSize( int *width, int *height ) const;

private:
    DECLARE_EVENT_TABLE()
    DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
};

//-----------------------------------------------------------------------------
// wxMDIClientWindow
//-----------------------------------------------------------------------------

class wxMDIClientWindow: public wxWindow
{
public:
    wxMDIClientWindow();
    wxMDIClientWindow( wxMDIParentFrame *parent, long style = 0 );
    ~wxMDIClientWindow();
    virtual bool CreateClient( wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL );

private:
    DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)
};

#endif // __MDIH__

