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
    wxFrame( wxWindow *parent, const wxWindowID id, const wxString &title, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
    bool Create( wxWindow *parent, const wxWindowID id, const wxString &title,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const long style = wxDEFAULT_FRAME_STYLE, const wxString &name = wxFrameNameStr );
    ~wxFrame(void);
    bool Destroy(void);
    void OnCloseWindow( wxCloseEvent& event );
    virtual bool Show( const bool show );
    virtual void Enable( const bool enable );
    virtual void GetClientSize( int *width, int *height ) const;
    void OnSize( wxSizeEvent &event );
    void SetMenuBar( wxMenuBar *menuBar );
    virtual bool CreateStatusBar( const int number = 1 );
    virtual void SetStatusText( const wxString &text, const int number = 0 );
    virtual void SetStatusWidths( const int n, const int *width );
    wxStatusBar *GetStatusBar(void);
    wxMenuBar *GetMenuBar(void);
    void SetTitle( const wxString &title );
    wxString GetTitle(void) const;
    void OnActivate( wxActivateEvent &WXUNUSED(event) ) {};
    
    void GtkOnSize( int width, int height );
    
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
