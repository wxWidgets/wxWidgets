/////////////////////////////////////////////////////////////////////////////
// Name:        bmpbutton.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __BMPBUTTONH__
#define __BMPBUTTONH__

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

class wxBitmapButton;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxButtonNameStr;

//-----------------------------------------------------------------------------
// wxBitmapButton
//-----------------------------------------------------------------------------

class wxBitmapButton: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxBitmapButton)

  public:

    wxBitmapButton(void);
    wxBitmapButton( wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = 0, const wxString &name = wxButtonNameStr  );
    bool Create(  wxWindow *parent, wxWindowID id, const wxBitmap &bitmap,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      long style = 0, const wxString &name = wxButtonNameStr  );
    void SetDefault(void);
    void SetLabel( const wxString &label );
    wxString GetLabel(void) const;
    
  public:
  
    wxBitmap   m_bitmap;    
    
};
#endif // __BMPBUTTONH__
