/////////////////////////////////////////////////////////////////////////////
// Name:        _gbsizer.i
// Purpose:     SWIG interface stuff for wxGBGridBagSizer and etc.
//
// Author:      Robin Dunn
//
// Created:     05-Nov-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
%}

//---------------------------------------------------------------------------

%typemap(in) wxGBPosition& (wxGBPosition temp) {
    $1 = &temp;
    if ( ! wxGBPosition_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxGBPosition& {
    $1 = wxPySimple_typecheck($input, wxT("wxGBPosition"), 2);
}

%typemap(in) wxGBSpan& (wxGBSpan temp) {
    $1 = &temp;
    if ( ! wxGBSpan_helper($input, &$1)) SWIG_fail;
}
%typemap(typecheck, precedence=SWIG_TYPECHECK_POINTER) wxGBSpan& {
    $1 = wxPySimple_typecheck($input, wxT("wxGBSpan"), 2);
}


%{
bool wxGBPosition_helper(PyObject* source, wxGBPosition** obj)
{
    return wxPyTwoIntItem_helper(source, obj, wxT("wxGBPosition"));
}

bool wxGBSpan_helper(PyObject* source, wxGBSpan** obj)
{
    return wxPyTwoIntItem_helper(source, obj, wxT("wxGBSpan"));
}

%}


//---------------------------------------------------------------------------
%newgroup;


class wxGBPosition
{
public:
    wxGBPosition(int row=0, int col=0);

    int GetRow() const;
    int GetCol() const;
    void SetRow(int row);
    void SetCol(int col);
    
    bool operator==(const wxGBPosition& p) const;
    bool operator!=(const wxGBPosition& p) const;

    %extend {
        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetRow()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetCol()));
            wxPyEndBlockThreads();
            return tup;
        }
    }
    %pythoncode {
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxGBPosition'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRow(val)
        elif index == 1: self.SetCol(val)
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
    }

    %pythoncode {
    row = property(GetRow, SetRow)
    col = property(GetCol, SetCol)
    }
};




class wxGBSpan
{
public:
    wxGBSpan(int rowspan=1, int colspan=1);

    int GetRowspan() const;
    int GetColspan() const;
    void SetRowspan(int rowspan);
    void SetColspan(int colspan);
    
    bool operator==(const wxGBSpan& o) const;
    bool operator!=(const wxGBSpan& o) const;

    %extend {
        PyObject* asTuple() {
            wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetRowspan()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetColspan()));
            wxPyEndBlockThreads();
            return tup;
        }
    }
    %pythoncode {
    def __str__(self):                   return str(self.asTuple())
    def __repr__(self):                  return 'wxGBSpan'+str(self.asTuple())
    def __len__(self):                   return len(self.asTuple())
    def __getitem__(self, index):        return self.asTuple()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRowspan(val)
        elif index == 1: self.SetColspan(val)
        else: raise IndexError
    def __nonzero__(self):               return self.asTuple() != (0,0)
    def __getinitargs__(self):           return ()
    def __getstate__(self):              return self.asTuple()
    def __setstate__(self, state):       self.Set(*state)
    }

    %pythoncode {
    rowspan = property(GetRowspan, SetRowspan)
    colspan = property(GetColspan, SetColspan)
    }
};

   
%immutable;
const wxGBSpan wxDefaultSpan;
%mutable;


//---------------------------------------------------------------------------


class wxGBSizerItem : public wxSizerItem
{
public:
    wxGBSizerItem();
    
    %name(GBSizerItemWindow) wxGBSizerItem( wxWindow *window,
                   const wxGBPosition& pos,
                   const wxGBSpan& span,
                   int flag,
                   int border,
                   wxObject* userData );

    %name(GBSizerItemSizer) wxGBSizerItem( wxSizer *sizer,
                   const wxGBPosition& pos,
                   const wxGBSpan& span,
                   int flag,
                   int border,
                   wxObject* userData );

    %name(GBSizerItemSpacer) wxGBSizerItem( int width,
                   int height,
                   const wxGBPosition& pos,
                   const wxGBSpan& span,
                   int flag,
                   int border,
                   wxObject* userData);


    // Get the grid position of the item
    wxGBPosition GetPos() const;
    %pythoncode { def GetPosTuple(self): return self.GetPos().asTuple() }

    // Get the row and column spanning of the item
    wxGBSpan GetSpan() const;
    %pythoncode { def GetSpanTuple(self): return self.GetSpan().asTuple() }

    // If the item is already a member of a sizer then first ensure that there
    // is no other item that would intersect with this one at the new
    // position, then set the new position.  Returns true if the change is
    // successful and after the next Layout the item will be moved.
    bool SetPos( const wxGBPosition& pos );

    // If the item is already a member of a sizer then first ensure that there
    // is no other item that would intersect with this one with its new
    // spanning size, then set the new spanning.  Returns true if the change
    // is successful and after the next Layout the item will be resized.
    bool SetSpan( const wxGBSpan& span );

    %nokwargs Intersects;
    
    // Returns true if this item and the other item instersect
    bool Intersects(const wxGBSizerItem& other);

    // Returns true if the given pos/span would intersect with this item.
    bool Intersects(const wxGBPosition& pos, const wxGBSpan& span);

    // Get the row and column of the endpoint of this item
    void GetEndPos(int& row, int& col);

    
    wxGridBagSizer* GetGBSizer() const;
    void SetGBSizer(wxGridBagSizer* sizer);
    
};


//---------------------------------------------------------------------------


class wxGridBagSizer : public wxFlexGridSizer
{
public:
    wxGridBagSizer(int vgap = 0, int hgap = 0 );

    // The Add method returns true if the item was successfully placed at the
    // given cell position, false if something was already there.
    %extend {
        bool Add( PyObject* item,
                  const wxGBPosition& pos,
                  const wxGBSpan& span = wxDefaultSpan,
                  int flag = 0,
                  int border = 0,
                  PyObject* userData = NULL ) {

            wxPyUserData* data = NULL;
            wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, true, false);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            wxPyEndBlockThreads();
            
            // Now call the real Add method if a valid item type was found
            if ( info.window )
                return self->Add(info.window, pos, span, flag, border, data);
            else if ( info.sizer )
                return self->Add(info.sizer, pos, span, flag, border, data);
            else if (info.gotSize)
                return self->Add(info.size.GetWidth(), info.size.GetHeight(),
                                 pos, span, flag, border, data);
            return false;
        }
    }
    
    %name(AddItem) bool Add( wxGBSizerItem *item );


    // Get/Set the size used for cells in the grid with no item.
    wxSize GetEmptyCellSize() const;
    void SetEmptyCellSize(const wxSize& sz);

    // Get the grid position of the specified item
    %nokwargs GetItemPosition;
    wxGBPosition GetItemPosition(wxWindow *window);
    wxGBPosition GetItemPosition(wxSizer *sizer);
    wxGBPosition GetItemPosition(size_t index);

    // Set the grid position of the specified item.  Returns true on success.
    // If the move is not allowed (because an item is already there) then
    // false is returned.
    %nokwargs SetItemPosition;
    bool SetItemPosition(wxWindow *window, const wxGBPosition& pos);
    bool SetItemPosition(wxSizer *sizer, const wxGBPosition& pos);
    bool SetItemPosition(size_t index, const wxGBPosition& pos);

    // Get the row/col spanning of the specified item
    %nokwargs GetItemSpan;
    wxGBSpan GetItemSpan(wxWindow *window);
    wxGBSpan GetItemSpan(wxSizer *sizer);
    wxGBSpan GetItemSpan(size_t index);

    // Set the row/col spanning of the specified item. Returns true on
    // success.  If the move is not allowed (because an item is already there)
    // then false is returned.
    %nokwargs SetItemSpan;
    bool SetItemSpan(wxWindow *window, const wxGBSpan& span);
    bool SetItemSpan(wxSizer *sizer, const wxGBSpan& span);
    bool SetItemSpan(size_t index, const wxGBSpan& span);
    

    // Find the sizer item for the given window or subsizer, returns NULL if
    // not found. (non-recursive)
    %nokwargs FindItem;
    wxGBSizerItem* FindItem(wxWindow* window);
    wxGBSizerItem* FindItem(wxSizer* sizer);

    
    // Return the sizer item for the given grid cell, or NULL if there is no
    // item at that position. (non-recursive)
    wxGBSizerItem* FindItemAtPosition(const wxGBPosition& pos);

    
    // Return the sizer item that has a matching user data (it only compares
    // pointer values) or NULL if not found. (non-recursive)
    wxGBSizerItem* FindItemWithData(const wxObject* userData);

    
    // These are what make the sizer do size calculations and layout
    virtual void RecalcSizes();
    virtual wxSize CalcMin();


    // Look at all items and see if any intersect (or would overlap) the given
    // item.  Returns true if so, false if there would be no overlap.  If an
    // excludeItem is given then it will not be checked for intersection, for
    // example it may be the item we are checking the position of.
    %nokwargs CheckForIntersection;
    bool CheckForIntersection(wxGBSizerItem* item, wxGBSizerItem* excludeItem = NULL);
    bool CheckForIntersection(const wxGBPosition& pos, const wxGBSpan& span, wxGBSizerItem* excludeItem = NULL);

};


//---------------------------------------------------------------------------
