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
    if (source == Py_None) {
        **obj = wxGBPosition(-1,-1);
        return True;
    }
    return wxPyTwoIntItem_helper(source, obj, wxT("wxGBPosition"));
}

bool wxGBSpan_helper(PyObject* source, wxGBSpan** obj)
{
    if (source == Py_None) {
        **obj = wxGBSpan(-1,-1);
        return True;
    }
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

//     %extend {
//         bool __eq__(const wxGBPosition* other) { return other ? (*self == *other) : False; }
//         bool __ne__(const wxGBPosition* other) { return other ? (*self != *other) : True;  }
//     }

    bool operator==(const wxGBPosition& other);
    bool operator!=(const wxGBPosition& other);
    
    %extend {
        void Set(int row=0, int col=0) {
            self->SetRow(row);
            self->SetCol(col);
        }
        
        PyObject* Get() {
            bool blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetRow()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetCol()));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }
    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.GBPosition'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRow(val)
        elif index == 1: self.SetCol(val)
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.GBPosition, self.Get())
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
    
//     %extend {
//         bool __eq__(const wxGBSpan* other) { return other ? (*self == *other) : False; }
//         bool __ne__(const wxGBSpan* other) { return other ? (*self != *other) : True;  }
//     }
    bool operator==(const wxGBSpan& other);
    bool operator!=(const wxGBSpan& other);
    

    %extend {
        void Set(int rowspan=1, int colspan=1) {
            self->SetRowspan(rowspan);
            self->SetColspan(colspan);
        }
        
        PyObject* Get() {
            bool blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(self->GetRowspan()));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(self->GetColspan()));
            wxPyEndBlockThreads(blocked);
            return tup;
        }
    }
    %pythoncode {
    asTuple = wx._deprecated(Get, "asTuple is deprecated, use `Get` instead")
    def __str__(self):                   return str(self.Get())
    def __repr__(self):                  return 'wx.GBSpan'+str(self.Get())
    def __len__(self):                   return len(self.Get())
    def __getitem__(self, index):        return self.Get()[index]
    def __setitem__(self, index, val):
        if index == 0: self.SetRowspan(val)
        elif index == 1: self.SetColspan(val)
        else: raise IndexError
    def __nonzero__(self):               return self.Get() != (0,0)
    __safe_for_unpickling__ = True
    def __reduce__(self):                return (wx.GBSpan, self.Get())
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
    %pythoncode { def GetPosTuple(self): return self.GetPos().Get() }

    // Get the row and column spanning of the item
    wxGBSpan GetSpan() const;
    %pythoncode { def GetSpanTuple(self): return self.GetSpan().Get() }

    // If the item is already a member of a sizer then first ensure that there
    // is no other item that would intersect with this one at the new
    // position, then set the new position.  Returns True if the change is
    // successful and after the next Layout the item will be moved.
    bool SetPos( const wxGBPosition& pos );

    // If the item is already a member of a sizer then first ensure that there
    // is no other item that would intersect with this one with its new
    // spanning size, then set the new spanning.  Returns True if the change
    // is successful and after the next Layout the item will be resized.
    bool SetSpan( const wxGBSpan& span );

    %nokwargs Intersects;
    
    // Returns True if this item and the other item instersect
    bool Intersects(const wxGBSizerItem& other);

    // Returns True if the given pos/span would intersect with this item.
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

    // The Add method returns True if the item was successfully placed at the
    // given cell position, False if something was already there.
    %extend {
        bool Add( PyObject* item,
                  const wxGBPosition& pos,
                  const wxGBSpan& span = wxDefaultSpan,
                  int flag = 0,
                  int border = 0,
                  PyObject* userData = NULL ) {

            wxPyUserData* data = NULL;
            bool blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, True, False);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            wxPyEndBlockThreads(blocked);
            
            // Now call the real Add method if a valid item type was found
            if ( info.window )
                return self->Add(info.window, pos, span, flag, border, data);
            else if ( info.sizer )
                return self->Add(info.sizer, pos, span, flag, border, data);
            else if (info.gotSize)
                return self->Add(info.size.GetWidth(), info.size.GetHeight(),
                                 pos, span, flag, border, data);
            return False;
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

    // Set the grid position of the specified item.  Returns True on success.
    // If the move is not allowed (because an item is already there) then
    // False is returned.
    %nokwargs SetItemPosition;
    bool SetItemPosition(wxWindow *window, const wxGBPosition& pos);
    bool SetItemPosition(wxSizer *sizer, const wxGBPosition& pos);
    bool SetItemPosition(size_t index, const wxGBPosition& pos);

    // Get the row/col spanning of the specified item
    %nokwargs GetItemSpan;
    wxGBSpan GetItemSpan(wxWindow *window);
    wxGBSpan GetItemSpan(wxSizer *sizer);
    wxGBSpan GetItemSpan(size_t index);

    // Set the row/col spanning of the specified item. Returns True on
    // success.  If the move is not allowed (because an item is already there)
    // then False is returned.
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

    
    // Return the sizer item located at the point given in pt, or NULL if
    // there is no item at that point. The (x,y) coordinates in pt correspond
    // to the client coordinates of the window using the sizer for
    // layout. (non-recursive)
    wxGBSizerItem* FindItemAtPoint(const wxPoint& pt);

    
    // Return the sizer item that has a matching user data (it only compares
    // pointer values) or NULL if not found. (non-recursive)
    wxGBSizerItem* FindItemWithData(const wxObject* userData);

    
    // These are what make the sizer do size calculations and layout
    virtual void RecalcSizes();
    virtual wxSize CalcMin();


    // Look at all items and see if any intersect (or would overlap) the given
    // item.  Returns True if so, False if there would be no overlap.  If an
    // excludeItem is given then it will not be checked for intersection, for
    // example it may be the item we are checking the position of.
    %nokwargs CheckForIntersection;
    bool CheckForIntersection(wxGBSizerItem* item, wxGBSizerItem* excludeItem = NULL);
    bool CheckForIntersection(const wxGBPosition& pos, const wxGBSpan& span, wxGBSizerItem* excludeItem = NULL);

};


//---------------------------------------------------------------------------
