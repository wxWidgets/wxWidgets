/////////////////////////////////////////////////////////////////////////////
// Name:        button.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKBUTTONH__
#define __GTKBUTTONH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxButton;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxButtonNameStr;

//-----------------------------------------------------------------------------
// wxButton
//-----------------------------------------------------------------------------

class wxButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxButton)

  public:

    wxButton(void);
    inline wxButton(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr)
    {
      Create(parent, id, label, pos, size, style, validator, name);
    }
    bool Create(wxWindow *parent, wxWindowID id, const wxString& label,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize, long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxButtonNameStr);
    void SetDefault(void);
    void SetLabel( const wxString &label );
    void Enable( bool enable );
    void SetFont( const wxFont &font );
    void SetBackgroundColour( const wxColour &colour );
};

#endif // __GTKBUTTONH__
