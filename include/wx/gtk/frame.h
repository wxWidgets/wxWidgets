/////////////////////////////////////////////////////////////////////////////
// Name:        frame.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKFRAMEH__
#define __GTKFRAMEH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/window.h"
#include "wx/menu.h"
#include "wx/statusbr.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxRadioBox;

class wxFrame;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxFrameNameStr;

//-----------------------------------------------------------------------------
// wxFrame
//-----------------------------------------------------------------------------

class wxFrame: public wxWindow
{
  DECLARE_DYNAMIC_CLASS(wxFrame)

  public:
  
    wxFrame(void);
    wxFrame( wxWindow *parent, wxWindowID id, const wxString &title, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
    bool Create( wxWindow *parent, wxWindowID id, const wxString &title,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
    ~wxFrame(void);
    bool Destroy(void);
    void OnCloseWindow( wxCloseEvent& event );
    virtual bool Show( bool show );
    virtual void Enable( bool enable );
    virtual void GetClientSize( int *width, int *height ) const;
    void OnSize( wxSizeEvent &event );
    void SetMenuBar( wxMenuBar *menuBar );
    virtual bool CreateStatusBar( int number = 1 );
    virtual void SetStatusText( const wxString &text, int number = 0 );
    virtual void SetStatusWidths( int n, int *width );
    wxStatusBar *GetStatusBar(void);
    wxMenuBar *GetMenuBar(void);
    void SetTitle( const wxString &title );
    wxString GetTitle(void) const;
    void OnActivate( wxActivateEvent &WXUNUSED(event) ) {};
    void OnIdle( wxIdleEvent &event );
    
  //private:    
    
    void GtkOnSize( int x, int y, int width, int height );
    void DoMenuUpdates(void);
    void DoMenuUpdates(wxMenu* menu);
    
  private:
  
    friend        wxWindow;
    
    GtkWidget    *m_mainWindow;
    wxMenuBar    *m_frameMenuBar;
    wxStatusBar  *m_frameStatusBar;
    bool          m_doingOnSize;
    wxString      m_title;
    
    
  DECLARE_EVENT_TABLE()
    
};

#endif // __GTKFRAMEH__
