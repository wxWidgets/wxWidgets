/////////////////////////////////////////////////////////////////////////////
// Name:        mdi.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
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

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxMDIParentFrame;
class wxMDIClientWindow;
class wxMDIChildFrame;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char* wxFrameNameStr;
extern const char* wxStatusLineNameStr;

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

  void OnSize( wxSizeEvent& event );
  void OnActivate( wxActivateEvent& event );

  void SetMenuBar( wxMenuBar *menu_bar );
  void GetClientSize(int *width, int *height) const;
  wxMDIChildFrame *GetActiveChild(void) const;
  
  wxMDIClientWindow *GetClientWindow(void) const; 
  virtual wxMDIClientWindow *OnCreateClient(void);
  
  virtual void Cascade(void) {};
  virtual void Tile(void) {};
  virtual void ArrangeIcons(void) {};
  virtual void ActivateNext(void);
  virtual void ActivatePrevious(void);

  void OnSysColourChanged(wxSysColourChangedEvent& event);
    
 protected:
    wxMDIClientWindow *             m_clientWindow;
    wxMDIChildFrame *               m_currentChild;
    bool                            m_parentFrameActive;

//  DECLARE_EVENT_TABLE()    
};

//-----------------------------------------------------------------------------
// wxMDIChildFrame
//-----------------------------------------------------------------------------

class wxMDIChildFrame: public wxPanel
{
  DECLARE_DYNAMIC_CLASS(wxMDIChildFrame)
  
  public:

    wxMDIChildFrame(void);
    wxMDIChildFrame( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr );
    ~wxMDIChildFrame(void);
    bool Create( wxMDIParentFrame *parent,
      wxWindowID id, const wxString& title,
      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
      long style = wxDEFAULT_FRAME_STYLE, const wxString& name = wxFrameNameStr );
    void SetMenuBar( wxMenuBar *menu_bar );

    // no status bars in wxGTK
    virtual bool CreateStatusBar( int WXUNUSED(number) = 1 ) { return FALSE; };
    virtual void SetStatusText( const wxString &WXUNUSED(text), int WXUNUSED(number) ) {};
    virtual void SetStatusWidths( int WXUNUSED(n), int *WXUNUSED(width) ) {};

    virtual void Maximize(void) {};
    virtual void Restore(void) {};
    virtual void Activate(void);
    
  public:
  
    wxString    m_title;
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
    void AddChild( wxWindow *child );
};

#endif // __MDIH__

