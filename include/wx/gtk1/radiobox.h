/////////////////////////////////////////////////////////////////////////////
// Name:        radiobox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKRADIOBOXH__
#define __GTKRADIOBOXH__

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/list.h"
#include "wx/control.h"
#include "wx/bitmap.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxRadioBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxRadioBoxNameStr;

//-----------------------------------------------------------------------------
// wxRadioBox
//-----------------------------------------------------------------------------

class wxRadioBox: public wxControl
{

  DECLARE_DYNAMIC_CLASS(wxRadioBox)
  
  public:
  
    wxRadioBox(void);
    wxRadioBox( wxWindow *parent, const wxWindowID id, const wxString& title,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const int n = 0, const wxString choices[] = NULL,
      const int majorDim = 0, const long style = wxRA_HORIZONTAL,
      const wxString &name = wxRadioBoxNameStr );
    bool Create( wxWindow *parent, const wxWindowID id, const wxString& title,
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const int n = 0, const wxString choices[] = NULL,
      const int majorDim = 0, const long style = wxRA_HORIZONTAL,
      const wxString &name = wxRadioBoxNameStr );
    int FindString( const wxString& s) const;
    void SetSelection( const int n );
    int GetSelection(void) const;
    wxString GetString( const int n ) const;
    wxString GetLabel(void) const;
    void SetLabel( const wxString& label );
    void SetLabel( const int item, const wxString& label );
    void SetLabel( const int item, wxBitmap *bitmap );
    wxString GetLabel( const int item ) const;
    bool Show( const bool show );
    void Enable( const bool enable );
    void Enable( const int item, const bool enable );
    void Show( const int item, const bool show );
    virtual wxString GetStringSelection(void) const;
    virtual bool SetStringSelection( const wxString& s );
    virtual int Number(void) const;
    int GetNumberOfRowsOrCols(void) const;
    void SetNumberOfRowsOrCols( const int n );
    
  private:
  
    GtkRadioButton *m_radio;
    
};

#endif // __GTKRADIOBOXH__
