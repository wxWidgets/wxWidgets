/////////////////////////////////////////////////////////////////////////////
// Name:        listbox.h
// Purpose:     wxListBox class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart and Markus Holzem
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef __LISTBOXH__
#define __LISTBOXH__

#ifdef __GNUG__
#pragma interface "listbox.h"
#endif

#include "wx/control.h"

WXDLLEXPORT_DATA(extern const char*) wxListBoxNameStr;
WXDLLEXPORT_DATA(extern const char*) wxListBoxNameStr;

#if USE_OWNER_DRAWN
  class WXDLLEXPORT wxOwnerDrawn;

  // define the array of list box items
  #include  <wx/dynarray.h>
  WX_DEFINE_ARRAY(wxOwnerDrawn *, wxListBoxItemsArray);
#endif

// forward decl for GetSelections()
class wxArrayInt;

WXDLLEXPORT_DATA(extern const char*) wxEmptyString;

// List box item
class WXDLLEXPORT wxListBox: public wxControl
{
  DECLARE_DYNAMIC_CLASS(wxListBox)
 public:

  wxListBox(void);
  inline wxListBox(wxWindow *parent, wxWindowID id,
           const wxPoint& pos = wxDefaultPosition,
           const wxSize& size = wxDefaultSize,
           int n = 0, const wxString choices[] = NULL,
           long style = 0,
           const wxValidator& validator = wxDefaultValidator,
           const wxString& name = wxListBoxNameStr)
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

  bool MSWCommand(WXUINT param, WXWORD id);

#if USE_OWNER_DRAWN
  bool MSWOnMeasure(WXMEASUREITEMSTRUCT *item);
  bool MSWOnDraw(WXDRAWITEMSTRUCT *item);

  // plug-in for derived classes
  virtual wxOwnerDrawn *CreateItem(uint n);

  // allows to get the item and use SetXXX functions to set it's appearance
  wxOwnerDrawn *GetItem(uint n) const { return m_aItems[n]; }
#endif

  virtual void Append(const wxString& item);
  virtual void Append(const wxString& item, char *clientData);
  virtual void Set(int n, const wxString* choices, char **clientData = NULL);
  virtual int FindString(const wxString& s) const ;
  virtual void Clear(void);
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

  virtual wxString GetStringSelection(void) const ;
  virtual bool SetStringSelection(const wxString& s, bool flag = TRUE);
  virtual int Number(void) const ;

  void Command(wxCommandEvent& event);

  // Windows-specific code to set the horizontal extent of
  // the listbox, if necessary. If s is non-NULL, it's
  // used to calculate the horizontal extent.
  // Otherwise, all strings are used.
  virtual void SetHorizontalExtent(const wxString& s = wxEmptyString);

  virtual WXHBRUSH OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
			WXUINT message, WXWPARAM wParam, WXLPARAM lParam);

  virtual long MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam);
  virtual void SetupColours(void);

 protected:
  int m_noItems;
  int m_selected;

#if USE_OWNER_DRAWN
  // control items
  wxListBoxItemsArray m_aItems;
#endif

};

#endif
    // __LISTBOXH__
