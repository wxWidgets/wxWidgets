/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCHOICEH__
#define __GTKCHOICEH__

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

class wxChoice;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char *wxChoiceNameStr;

//-----------------------------------------------------------------------------
// wxChoice
//-----------------------------------------------------------------------------

class wxChoice: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxChoice)

  public:

    wxChoice(void);
    inline wxChoice( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = (const wxString *) NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxChoiceNameStr )
    {
      Create(parent, id, pos, size, n, choices, style, validator, name);
    }
    bool Create( wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = (wxString *) NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxChoiceNameStr );
    void Append( const wxString &item );
    void Clear(void);
    void Delete(int n);
    int FindString( const wxString &string ) const;
    int GetColumns(void) const;
    int GetSelection(void);
    wxString GetString( int n ) const;
    wxString GetStringSelection(void) const;
    int Number(void) const;
    void SetColumns( int n = 1 );
    void SetSelection( int n );
    void SetStringSelection( const wxString &string );
    
    void SetFont( const wxFont &font );
};

#endif // __GTKCHOICEH__
