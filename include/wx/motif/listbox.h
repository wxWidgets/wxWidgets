/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:     wxListBox class
// Author:      Julian Smart
// Modified by:
// Created:     17/09/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:   	wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_LISTBOX_H_
#define _WX_LISTBOX_H_

#ifdef __GNUG__
#pragma interface "listbox.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxListBoxNameStr;

// forward decl for GetSelections()
class WXDLLEXPORT wxArrayInt;

WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// List box item
class WXDLLEXPORT wxListBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxListBox)
 public:

  wxListBox();
  inline wxListBox(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxListBoxNameStr):
              m_clientDataList(wxKEY_INTEGER)
  {
    Create(parent, id, pos, size, n, choices, style, validator, name);
  }

  bool Create(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxListBoxNameStr);

  ~wxListBox();

  virtual void Append(const wxString& item);
  virtual void Append(const wxString& item, char *clientData);
  virtual void Set(int n, const wxString* choices, char **clientData = NULL);
  virtual int FindString(const wxString& s) const ;
  virtual void Clear();
  virtual void SetSelection(int n, bool select = TRUE);

  virtual void Deselect(int n);

  // For single choice list item only
  virtual int GetSelection() const ;
  virtual void Delete(int n);
  virtual char *GetClientData(int n) const ;
  virtual void SetClientData(int n, char *clientData);
  virtual void SetString(int n, const wxString& s);

  // For single or multiple choice list item
  virtual int GetSelections(wxArrayInt& aSelections) const;
  virtual bool Selected(int n) const ;
  virtual wxString GetString(int n) const ;
  virtual void SetSize(int x, int y, int width, int height, int sizeFlags = wxSIZE_AUTO);

  // Set the specified item at the first visible item
  // or scroll to max range.
  virtual void SetFirstItem(int n) ;
  virtual void SetFirstItem(const wxString& s) ;

  virtual void InsertItems(int nItems, const wxString items[], int pos);

  virtual wxString GetStringSelection() const ;
  virtual bool SetStringSelection(const wxString& s, bool flag = TRUE);
  virtual int Number() const ;

  void Command(wxCommandEvent& event);

  WXWidget GetTopWidget() const;

protected:
  int       m_noItems;
  int       m_selected;

  // List mapping positions->client data
  wxList    m_clientDataList;

public:
  bool      m_inSetValue;
};

#endif
    // _WX_LISTBOX_H_
