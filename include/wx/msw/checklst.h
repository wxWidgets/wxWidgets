///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.h
// Purpose:     wxCheckListBox class - a listbox with checkable items
// Author:      Vadim Zeitlin
// Modified by: 
// Created:     16.11.97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   __CHECKLST__H_
#define   __CHECKLST__H_

#ifdef __GNUG__
#pragma interface "checklst.h"
#endif

typedef   unsigned int  size_t;

#if !wxUSE_OWNER_DRAWN
  #error  "wxCheckListBox class requires owner-drawn functionality."
#endif

class wxCheckListBoxItem; // fwd decl, define in checklst.cpp

class WXDLLEXPORT wxCheckListBox : public wxListBox
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
//                 const wxFont& font = wxNullFont);

  // items may be checked
  bool  IsChecked(size_t uiIndex) const;
  void  Check(size_t uiIndex, bool bCheck = TRUE);

  // accessors
  size_t  GetItemHeight() const { return m_nItemHeight; }

protected:
  // we create our items ourselves and they have non-standard size,
  // so we need to override these functions
  virtual wxOwnerDrawn *CreateItem(size_t n);
  virtual bool          MSWOnMeasure(WXMEASUREITEMSTRUCT *item);

  // pressing space or clicking the check box toggles the item
  void OnChar(wxKeyEvent& event);
  void OnLeftClick(wxMouseEvent& event);

private:
  size_t    m_nItemHeight;  // height of checklistbox items (the same for all)

  DECLARE_EVENT_TABLE()
};

#endif    //_CHECKLST_H
