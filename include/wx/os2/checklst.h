///////////////////////////////////////////////////////////////////////////////
// Name:        checklst.h
// Purpose:     wxCheckListBox class - a listbox with checkable items
//              Note: this is an optional class.
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_CHECKLST_H_
#define _WX_CHECKLST_H_

#include <stddef.h>

#include "wx/setup.h"

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
#if wxUSE_VALIDATORS
#  if defined(__VISAGECPP__)
                 const wxValidator* validator = wxDefaultValidator,
#  else
                 const wxValidator& validator = wxDefaultValidator,
#  endif
#endif
                 const wxString& name = wxListBoxNameStr);

  // override base class virtuals
  virtual void Delete(int n);
  virtual void InsertItems(int nItems, const wxString items[], int pos);

  virtual bool SetFont( const wxFont &font );

  // items may be checked
  bool IsChecked(size_t uiIndex) const;
  void Check(size_t uiIndex, bool bCheck = TRUE);

  // accessors
  size_t GetItemHeight() const { return m_nItemHeight; }

protected:
  // we create our items ourselves and they have non-standard size,
  // so we need to override these functions
  virtual wxOwnerDrawn *CreateItem(size_t n);
//  virtual bool          OS2OnMeasure(WXMEASUREITEMSTRUCT *item);

  // pressing space or clicking the check box toggles the item
  void OnChar(wxKeyEvent& event);
  void OnLeftClick(wxMouseEvent& event);

private:
  size_t    m_nItemHeight;  // height of checklistbox items (the same for all)

  //Virtual function hiding suppression, do not use
  virtual wxControl *CreateItem(const wxItemResource* childResource,
                                const wxItemResource* parentResource,
                                const wxResourceTable *table = (const wxResourceTable *) NULL)
  { return(wxWindowBase::CreateItem(childResource, parentResource, table));};

  DECLARE_EVENT_TABLE()
};

#endif
   // _WX_CHECKLST_H_
