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
    inline wxRadioBox( wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = (const wxString *) NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, 
	     const wxString& name = wxRadioBoxNameStr )
    {
      Create( parent, id, title, pos, size, n, choices, majorDim, style, val, name );
    }
    ~wxRadioBox(void);
    bool Create( wxWindow *parent, wxWindowID id, const wxString& title,
             const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
             int n = 0, const wxString choices[] = (const wxString *) NULL,
             int majorDim = 0, long style = wxRA_HORIZONTAL,
             const wxValidator& val = wxDefaultValidator, 
	     const wxString& name = wxRadioBoxNameStr );
    int FindString( const wxString& s) const;
    void SetSelection( int n );
    int GetSelection(void) const;
    wxString GetString( int n ) const;
    void SetLabel( const wxString& label );
    void SetLabel( int item, const wxString& label );
    void SetLabel( int item, wxBitmap *bitmap );
    wxString GetLabel( int item ) const;
    bool Show( bool show );
    void Enable( bool enable );
    void Enable( int item, bool enable );
    void Show( int item, bool show );
    virtual wxString GetStringSelection(void) const;
    virtual bool SetStringSelection( const wxString& s );
    virtual int Number(void) const;
    int GetNumberOfRowsOrCols(void) const;
    void SetNumberOfRowsOrCols( int n );
    
    void SetFont( const wxFont &font );
    void SetBackgroundColour( const wxColour &colour );
    void OnSize( wxSizeEvent &event );
    
  // implementation    
    
    bool IsOwnGtkWindow( GdkWindow *window );
    
    bool             m_alreadySent;
    wxList           m_boxes;
    
  DECLARE_EVENT_TABLE()    
};

#endif // __GTKRADIOBOXH__
