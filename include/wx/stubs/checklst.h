///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.h
// Purpose:     wxCheckListBox class - a listbox with checkable items
//              Note: this is an optional class.
// Author:      AUTHOR
// Modified by: 
// Created:     ??/??/98
// RCS-ID:      $Id$
// Copyright:   (c) AUTHOR
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKLST_H_
#define _WX_CHECKLST_H_

#ifdef __GNUG__
#pragma interface "checklst.h"
#endif

typedef   unsigned int  uint;

class wxCheckListBoxItem; // fwd decl, define in checklst.cpp

class wxCheckListBox : public wxListBox
{
  DECLARE_DYNAMIC_CLASS(wxCheckListBox)
public:
  // ctors
  wxCheckListBox();
  wxCheckListBox(wxWindow *parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 int nStrings = 0, 
                 const wxString choices[] = NULL,
                 long style = 0,
                 const wxValidator& validator = wxDefaultValidator,
                 const wxString& name = wxListBoxNameStr);

  // items may be checked
  bool  IsChecked(uint uiIndex) const;
  void  Check(uint uiIndex, bool bCheck = TRUE);

  // accessors
  uint  GetItemHeight() const { return m_nItemHeight; }

protected:
  // we create our items ourselves and they have non-standard size,
  // so we need to override these functions
  virtual wxOwnerDrawn *CreateItem(uint n);

  // pressing space or clicking the check box toggles the item
  void OnChar(wxKeyEvent& event);
  void OnLeftClick(wxMouseEvent& event);

private:
  uint    m_nItemHeight;  // height of checklistbox items (the same for all)

  DECLARE_EVENT_TABLE()
};

#endif
    // _WX_CHECKLST_H_
