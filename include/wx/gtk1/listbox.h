/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
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
class wxArrayInt;

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
    inline wxListBox( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = (const wxString *) NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxListBoxNameStr )
    {
      Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = (const wxString *) NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxListBoxNameStr );
    void Append( const wxString &item );
    void Append( const wxString &item, char *clientData );
    void Clear(void);
    void Delete( int n );
    void Deselect( int n );
    int FindString( const wxString &item ) const;
    char *GetClientData( int n ) const;
    int GetSelection(void) const;
    int GetSelections( class wxArrayInt &) const;
    wxString GetString( int n ) const;
    wxString GetStringSelection(void) const;
    int Number(void);
    bool Selected( int n );
    void Set( int n, const wxString *choices );
    void SetClientData( int n, char *clientData );
    void SetFirstItem( int n );
    void SetFirstItem( const wxString &item );
    void SetSelection( int n, bool select = TRUE );
    void SetString( int n, const wxString &string );
    void SetStringSelection( const wxString &string, bool select = TRUE );
    
    void SetDropTarget( wxDropTarget *dropTarget );

  // implementation
    
    int GetIndex( GtkWidget *item ) const;
    GtkWidget *GetConnectWidget(void);
    bool IsOwnGtkWindow( GdkWindow *window );
    void ApplyWidgetStyle();
    
    GtkList   *m_list;
    wxList     m_clientData;
    
};

#endif // __GTKLISTBOXH__
