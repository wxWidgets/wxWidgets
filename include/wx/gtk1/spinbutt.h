/////////////////////////////////////////////////////////////////////////////
// Name:        spinbutt.h
// Purpose:     wxSpinButton class
// Author:      Robert Roebling
// Modified by:
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef __GTKSPINPUTTH__
#define __GTKSPINBUTTH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxSpinButton;
class wxSpinEvent;

//-----------------------------------------------------------------------------
// wxSpinButton
//-----------------------------------------------------------------------------

class wxSpinButton : public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxSpinButton)
 
public:
  wxSpinButton();
  inline wxSpinButton( wxWindow *parent, wxWindowID id = -1, 
                      const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                      long style = wxSP_VERTICAL, const wxString& name = "wxSpinButton")
  {
    Create(parent, id, pos, size, style, name);
  }
  ~wxSpinButton();
  bool Create( wxWindow *parent, wxWindowID id = -1, 
               const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
               long style = wxSP_VERTICAL, const wxString& name = "wxSpinButton" );

  int GetValue() const;
  void SetValue( int value );
  void SetRange( int minVal, int maxVal );
  int GetMin() const;
  int GetMax() const;

// implementation

  void OnSize( wxSizeEvent &event );  
  
  bool IsOwnGtkWindow( GdkWindow *window );
  void ApplyWidgetStyle();
    
  GtkAdjustment  *m_adjust;
  float           m_oldPos;
  
  DECLARE_EVENT_TABLE()
};

//-----------------------------------------------------------------------------
// wxSpinEvent
//-----------------------------------------------------------------------------

class wxSpinEvent : public wxScrollEvent
{
  DECLARE_DYNAMIC_CLASS(wxSpinEvent)

public:
  wxSpinEvent(wxEventType commandType = wxEVT_NULL, int id = 0);
};

typedef void (wxEvtHandler::*wxSpinEventFunction)(wxSpinEvent&);

// Spin events

#define EVT_SPIN_UP(id, func) { wxEVT_SCROLL_LINEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func }
#define EVT_SPIN_DOWN(id, func) { wxEVT_SCROLL_LINEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func }

#define EVT_SPIN(id, func) \
  { wxEVT_SCROLL_TOP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_BOTTOM, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_LINEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_LINEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_PAGEUP, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_PAGEDOWN, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },\
  { wxEVT_SCROLL_THUMBTRACK, id, -1, (wxObjectEventFunction) (wxEventFunction) (wxSpinEventFunction) & func },

#endif
    // __GTKSPINBUTTH__
