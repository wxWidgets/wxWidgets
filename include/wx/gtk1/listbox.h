/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKLISTBOXH__
#define __GTKLISTBOXH__

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

class wxListBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxListBoxNameStr;

//-----------------------------------------------------------------------------
// wxListBox
//-----------------------------------------------------------------------------

class wxListBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxListBox)

  public:

    wxListBox(void);
    wxListBox( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const int n = 0, const wxString choices[] = NULL,
      const long style = 0, const wxString &name = wxListBoxNameStr );
    bool Create( wxWindow *parent, wxWindowID id, 
      const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, 
      const int n = 0, const wxString choices[] = NULL,
      const long style = 0, const wxString &name = wxListBoxNameStr );
    void Append( const wxString &item );
    void Append( const wxString &item, char *clientData );
    void Clear(void);
    void Delete( int n );
    void Deselect( int n );
    int FindString( const wxString &item ) const;
    char *GetClientData( const int n ) const;
    int GetSelection(void) const;
    int GetSelections( int **selections ) const;
    wxString GetString( int n ) const;
    wxString GetStringSelection(void) const;
    int Number(void);
    bool Selected( const int n );
    void Set( const int n, const wxString *choices );
    void SetClientData( const int n, char *clientData );
    void SetFirstItem( int n );
    void SetFirstItem( const wxString &item );
    void SetSelection( const int n, const bool select = TRUE );
    void SetString( const int n, const wxString &string );
    void SetStringSelection( const wxString &string, const bool select = TRUE );
    
  private:
  
    GtkList   *m_list;
    
  public:
  
    int GetIndex( GtkWidget *item ) const;
};

#endif // __GTKLISTBOXH__
