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
%}

MAKE_CONST_WXSTRING(ListBoxNameStr);


//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxListBox);

class wxListBox : public wxControlWithItems
{
public:
    %pythonAppend wxListBox         "self._setOORInfo(self)"
    %pythonAppend wxListBox()       ""

    wxListBox(wxWindow* parent,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const wxArrayString& choices = wxPyEmptyStringArray,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyListBoxNameStr);

//    wxListBox(wxWindow* parent,
//            wxWindowID id = -1,
//            const wxPoint& pos = wxDefaultPosition,
//            const wxSize& size = wxDefaultSize,
//            int n = 0,
//            const wxString choices[] = NULL,
//            long style = 0,
//            const wxValidator& validator = wxDefaultValidator,
//            const wxString& name = wxPyListBoxNameStr);
    %RenameCtor(PreListBox, wxListBox());

    bool Create(wxWindow* parent,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const wxArrayString& choices = wxPyEmptyStringArray,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyListBoxNameStr);

//    bool Create(wxWindow *parent,
//            wxWindowID id = -1,
//            const wxPoint& pos = wxDefaultPosition,
//            const wxSize& size = wxDefaultSize,
//            int n = 0,
//            const wxString choices[] = NULL,
//            long style = 0,
//            const wxValidator& validator = wxDefaultValidator,
//            const wxString& name = wxPyListBoxNameStr);

    // all generic methods are in wxControlWithItems...

    %extend
    {
        void Insert(const wxString& item, int pos, PyObject* clientData = NULL)
        {
            if (clientData)
            {
                wxPyClientData* data = new wxPyClientData(clientData);
                self->Insert(item, pos, data);
            }
            else
                self->Insert(item, pos);
        }
    }

    void InsertItems(const wxArrayString& items, unsigned int pos);
    void Set(const wxArrayString& items/*, void **clientData = NULL */);

    // multiple selection logic
    virtual bool IsSelected(int n) const;
    virtual void SetSelection(int n, bool select = true);
    virtual void Select(int n);

    void Deselect(int n);
    void DeselectAll(int itemToLeaveSelected = -1);

    virtual bool SetStringSelection(const wxString& s, bool select = true);

    // works for single as well as multiple selection listboxes (unlike
    // GetSelection which only works for listboxes with single selection)
    //virtual int GetSelections(wxArrayInt& aSelections) const;
    %extend {
        PyObject* GetSelections() {
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            wxArrayInt lst;
            self->GetSelections(lst);
            PyObject *tup = PyTuple_New(lst.GetCount());
            for (size_t i=0; i<lst.GetCount(); i++)
            {
                PyTuple_SetItem(tup, i, PyInt_FromLong(lst[i]));
            }
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }

    // set the specified item at the first visible item or scroll to max
    // range.
    void SetFirstItem(int n);
    %Rename(SetFirstItemStr,  void, SetFirstItem(const wxString& s));

    // ensures that the given item is visible scrolling the listbox
    // if necessary
    virtual void EnsureVisible(int n);

    // a combination of Append() and EnsureVisible(): appends the item to the
    // listbox and ensures that it is visible i.e. not scrolled out of view
    void AppendAndEnsureVisible(const wxString& s);

    // return True if this listbox is sorted
    bool IsSorted() const;

    // return the index of the item at this position or wxNOT_FOUND
    int HitTest(const wxPoint& pt) const;

    %extend
    {
        void SetItemForegroundColour(int item, const wxColour& c)
        {
            %#ifdef __WXMSW__
                 if (self->GetWindowStyle() & wxLB_OWNERDRAW)
                     self->GetItem(item)->SetTextColour(c);
            %#endif
        }
        void SetItemBackgroundColour(int item, const wxColour& c)
        {
            %#ifdef __WXMSW__
                 if (self->GetWindowStyle() & wxLB_OWNERDRAW)
                     self->GetItem(item)->SetBackgroundColour(c);
            %#endif
        }
        void SetItemFont(int item, const wxFont& f)
        {
            %#ifdef __WXMSW__
                 if (self->GetWindowStyle() & wxLB_OWNERDRAW)
                     self->GetItem(item)->SetFont(f);
            %#endif
        }
    }

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};


//---------------------------------------------------------------------------
%newgroup


MustHaveApp(wxCheckListBox);

// wxCheckListBox: a listbox whose items may be checked
class wxCheckListBox : public wxListBox
{
public:
    %pythonAppend wxCheckListBox         "self._setOORInfo(self)"
    %pythonAppend wxCheckListBox()       ""

    wxCheckListBox(wxWindow *parent,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const wxArrayString& choices = wxPyEmptyStringArray,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyListBoxNameStr);
    %RenameCtor(PreCheckListBox, wxCheckListBox());

    bool Create(wxWindow *parent,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            const wxArrayString& choices = wxPyEmptyStringArray,
            long style = 0,
            const wxValidator& validator = wxDefaultValidator,
            const wxString& name = wxPyListBoxNameStr);

    bool  IsChecked(unsigned int index);
    void  Check(unsigned int index, int check = true);

#if defined(__WXMSW__) || defined(__WXGTK__)
    int GetItemHeight();
#endif

    %property(ItemHeight, GetItemHeight, doc="See `GetItemHeight`");
};

//---------------------------------------------------------------------------
