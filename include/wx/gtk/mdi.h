/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
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
  DECLARE_DYNAMIC_CLASS(wxMDIParentFrame)

  friend class wxMDIChildFrame;
  
public:

    wxMDIParentFrame(void);
    wxMDIParentFrame( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
      const wxString& name = wxFrameNameStr );
  ~wxMDIParentFrame(void);
   bool Create( wxWindow *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE | wxVSCROLL | wxHSCROLL,
      const wxString& name = wxFrameNameStr );


    void GetClientSize(int *width, int *height) const;
    wxMDIChildFrame *GetActiveChild() const;
    
    wxMDIClientWindow *GetClientWindow() const; 
    virtual wxMDIClientWindow *OnCreateClient();
  
    virtual void Cascade() {}
    virtual void Tile() {}
    virtual void ArrangeIcons() {}
    virtual void ActivateNext();
    virtual void ActivatePrevious();

    void OnActivate( wxActivateEvent& event );
    void OnSysColourChanged( wxSysColourChangedEvent& event );
    
  // implementation

    wxMDIClientWindow  *m_clientWindow;
    bool                m_justInserted;

    virtual void GtkOnSize( int x, int y, int width, int height );
    virtual void OnInternalIdle();
    
  DECLARE_EVENT_TABLE()    
};

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

class wxMDIChildFrame: public wxFrame
{
  DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
  
  public:

    wxMDIChildFrame();
    wxMDIChildFrame( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr );
    ~wxMDIChildFrame(void);
    bool Create( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr );
      
  virtual void SetMenuBar( wxMenuBar *menu_bar );
  virtual wxMenuBar *GetMenuBar() const;

  virtual void GetClientSize( int *width, int *height ) const;
  virtual void AddChild( wxWindow *child );

  virtual void Activate();
    
    // no status bars
  virtual wxStatusBar* CreateStatusBar( int WXUNUSED(number)=1, long WXUNUSED(style)=1, 
    wxWindowID WXUNUSED(id)=1, const wxString& WXUNUSED(name)=WXSTRINGCAST NULL ) {return (wxStatusBar*)NULL; }
  virtual wxStatusBar *GetStatusBar() { return (wxStatusBar*)NULL; }
  virtual void SetStatusText( const wxString &WXUNUSED(text), int WXUNUSED(number) ) {}
  virtual void SetStatusWidths( int WXUNUSED(n), int *WXUNUSED(width) ) {}

    // no size hints
  virtual void SetSizeHints( int WXUNUSED(minW), int WXUNUSED(minH),
                             int WXUNUSED(maxW), int WXUNUSED(maxH),
                             int WXUNUSED(incW) )
  {
  }
  
    // no toolbar bars
  virtual wxToolBar* CreateToolBar( long WXUNUSED(style), wxWindowID WXUNUSED(id), 
    const wxString& WXUNUSED(name) ) { return (wxToolBar*)NULL; }
  virtual wxToolBar *GetToolBar() const { return (wxToolBar*)NULL; }
  
    // no icon
  void SetIcon( const wxIcon &icon ) { m_icon = icon; }
    
    // no title 
  void SetTitle( const wxString &title ) { m_title = title; }
  wxString GetTitle() const { return m_title; }
  
    // no maximize etc
  virtual void Maximize( bool WXUNUSED(maximize) ) {}
  virtual void Restore() {}
    
  void OnActivate( wxActivateEvent &event );
    
  // implementation
  
    wxMenuBar         *m_menuBar;
    GtkNotebookPage   *m_page;
    bool               m_justInserted;
    
  DECLARE_EVENT_TABLE()    
};

//-----------------------------------------------------------------------------
// wxMDIClientWindow
//-----------------------------------------------------------------------------

class wxMDIClientWindow: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxMDIClientWindow)
  
public:
  
    wxMDIClientWindow(void);
    wxMDIClientWindow( wxMDIParentFrame *parent, long style = 0 );
    ~wxMDIClientWindow(void);
    virtual bool CreateClient( wxMDIParentFrame *parent, long style = wxVSCROLL | wxHSCROLL );
};

#endif // __MDIH__

