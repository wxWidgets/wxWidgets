/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.h
// Purpose:
// Author:      Robert Roebling
// Created:     01/02/97
// Id:          $Id$
// Copyright:   (c) 1998 Robert Roebling, Julian Smart and Markus Holzem
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////


#ifndef __GTKCOMBOBOXH__
#define __GTKCOMBOBOXH__

#ifdef __GNUG__
#pragma interface "combobox.h"
#endif

#include "wx/defs.h"
#include "wx/object.h"
#include "wx/control.h"

//-----------------------------------------------------------------------------
// classes
//-----------------------------------------------------------------------------

class wxComboBox;

//-----------------------------------------------------------------------------
// global data
//-----------------------------------------------------------------------------

extern const char* wxComboBoxNameStr;
extern const char* wxEmptyString;

//-----------------------------------------------------------------------------
// wxComboBox
//-----------------------------------------------------------------------------

class wxComboBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxComboBox)

 public:
  inline wxComboBox(void) {}

  inline wxComboBox(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxString& name = wxComboBoxNameStr)
  {
    Create(parent, id, value, pos, size, n, choices, style, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxString& value = wxEmptyString,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxString& name = wxComboBoxNameStr);

  // List functions
  void Clear(void);
  void Append( const wxString &item );
  void Append( const wxString &item, char* clientData );
  void Delete( int n );
  int FindString( const wxString &item );
  char* GetClientData( int n );
  void SetClientData( int n, char * clientData );
  int GetSelection(void) const;
  wxString GetString( int n ) const;
  wxString GetStringSelection(void) const;
  int Number(void) const;
  void SetSelection( int n );
  void SetStringSelection( const wxString &string );

  // Text field functions
  wxString GetValue(void) const ;
  void SetValue(const wxString& value);

  // Clipboard operations
  void Copy(void);
  void Cut(void);
  void Paste(void);
  void SetInsertionPoint(long pos);
  void SetInsertionPointEnd(void);
  long GetInsertionPoint(void) const ;
  long GetLastPosition(void) const ;
  void Replace(long from, long to, const wxString& value);
  void Remove(long from, long to);
  void SetSelection(long from, long to);
  void SetEditable(bool editable);

  private:
  
    wxList   m_clientData;
  
  public:
  
    bool     m_alreadySent;
};

#endif // __GTKCOMBOBOXH__
