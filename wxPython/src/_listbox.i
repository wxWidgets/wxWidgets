/////////////////////////////////////////////////////////////////////////////
// Name:        _listbox.i
// Purpose:     SWIG interface defs for wxListBox and wxCheckListBox
//
// Author:      Robin Dunn
//
// Created:     10-June-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/checklst.h>
    
    DECLARE_DEF_STRING(ListBoxNameStr);
%}

//---------------------------------------------------------------------------
%newgroup

class wxListBox : public wxControlWithItems
{
public:
    %addtofunc wxListBox         "self._setOORInfo(self)"
    %addtofunc wxListBox()       ""

    wxListBox(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int choices=0, wxString* choices_array = NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxPyListBoxNameStr);
    %name(PreListBox)wxListBox();

    bool Create(wxWindow* parent, wxWindowID id,
              const wxPoint& pos = wxDefaultPosition,
              const wxSize& size = wxDefaultSize,
              int choices=0, wxString* choices_array = NULL,
              long style = 0,
              const wxValidator& validator = wxDefaultValidator,
              const wxString& name = wxPyListBoxNameStr);

    // all generic methods are in wxControlWithItems...

    %extend {
        void Insert(const wxString& item, int pos, PyObject* clientData=NULL) {
            if (clientData) {
                wxPyClientData* data = new wxPyClientData(clientData);
                self->Insert(item, pos, data);
            } else
                self->Insert(item, pos);
        }
    }

    void InsertItems(const wxArrayString& items, int pos);
    void Set(const wxArrayString& items/*, void **clientData = NULL */);

    // multiple selection logic
    virtual bool IsSelected(int n) const;
    virtual void SetSelection(int n, bool select = TRUE);
    virtual void Select(int n);
    void Deselect(int n);
    void DeselectAll(int itemToLeaveSelected = -1);

    virtual bool SetStringSelection(const wxString& s, bool select = TRUE);

    // works for single as well as multiple selection listboxes (unlike
    // GetSelection which only works for listboxes with single selection)
    //virtual int GetSelections(wxArrayInt& aSelections) const;
    %extend {
      PyObject* GetSelections() {
          wxArrayInt lst;
          self->GetSelections(lst);
          PyObject *tup = PyTuple_New(lst.GetCount());
          for(size_t i=0; i<lst.GetCount(); i++) {
              PyTuple_SetItem(tup, i, PyInt_FromLong(lst[i]));
          }
          return tup;
      }
    }

    // set the specified item at the first visible item or scroll to max
    // range.
    void SetFirstItem(int n);
    %name(SetFirstItemStr) void SetFirstItem(const wxString& s);

    // ensures that the given item is visible scrolling the listbox if
    // necessary
    virtual void EnsureVisible(int n);

    // a combination of Append() and EnsureVisible(): appends the item to the
    // listbox and ensures that it is visible i.e. not scrolled out of view
    void AppendAndEnsureVisible(const wxString& s);

    // return TRUE if the listbox allows multiple selection
    bool HasMultipleSelection() const;

    // return TRUE if this listbox is sorted
    bool IsSorted() const;
};


//---------------------------------------------------------------------------
%newgroup


// wxCheckListBox: a listbox whose items may be checked
class wxCheckListBox : public wxListBox
{
public:
    %addtofunc wxListBox         "self._setOORInfo(self)"
    %addtofunc wxListBox()       ""

    wxCheckListBox(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int choices = 0, wxString* choices_array = NULL,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyListBoxNameStr);
    %name(PreCheckListBox)wxCheckListBox();

    bool Create(wxWindow *parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   int choices = 0, wxString* choices_array = NULL,
                   long style = 0,
                   const wxValidator& validator = wxDefaultValidator,
                   const wxString& name = wxPyListBoxNameStr);

    bool  IsChecked(int index);
    void  Check(int index, int check = TRUE);

#ifndef __WXMAC__
    int GetItemHeight();
#endif

    // return the index of the item at this position or wxNOT_FOUND
    int HitTest(const wxPoint& pt) const;
    %name(HitTestXY)int HitTest(wxCoord x, wxCoord y) const;
};

//---------------------------------------------------------------------------
