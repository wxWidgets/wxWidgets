/////////////////////////////////////////////////////////////////////////////
// Name:        choice.h
// Purpose:
// Author:      Robert Roebling
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling
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

  wxChoice();
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
  ~wxChoice();
  bool Create( wxWindow *parent, wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               int n = 0, const wxString choices[] = (wxString *) NULL,
               long style = 0,
               const wxValidator& validator = wxDefaultValidator,
               const wxString& name = wxChoiceNameStr );

  void Append( const wxString &item );
  void Append( const wxString &item, void* clientData );
  void Append( const wxString &item, wxClientData* clientData );
  
  void SetClientData( int n, void* clientData );
  void* GetClientData( int n );
  void SetClientObject( int n, wxClientData* clientData );
  wxClientData* GetClientObject( int n );
  
  void Clear();
  void Delete(int n);
  
  int FindString( const wxString &string ) const;
  int GetColumns() const;
  int GetSelection();
  wxString GetString( int n ) const;
  wxString GetStringSelection() const;
  int Number() const;
  void SetColumns( int n = 1 );
  void SetSelection( int n );
  void SetStringSelection( const wxString &string );
    
// implementation
    
  wxList  m_clientDataList;

  void AppendCommon( const wxString &item );
  void ApplyWidgetStyle();
};

#endif // __GTKCHOICEH__
