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
        return true;
    }
    return wxPyTwoIntItem_helper(source, obj, wxT("wxGBPosition"));
}

bool wxGBSpan_helper(PyObject* source, wxGBSpan** obj)
{
    if (source == Py_None) {
        **obj = wxGBSpan(-1,-1);
        return true;
    }
    return wxPyTwoIntItem_helper(source, obj, wxT("wxGBSpan"));
}

%}


//---------------------------------------------------------------------------
%newgroup;

DocStr(wxGBPosition,
"This class represents the position of an item in a virtual grid of
rows and columns managed by a `wx.GridBagSizer`.  wxPython has
typemaps that will automatically convert from a 2-element sequence of
integers to a wx.GBPosition, so you can use the more pythonic
representation of the position nearly transparently in Python code.", "");

class wxGBPosition
{
public:
    wxGBPosition(int row=0, int col=0);

    int GetRow() const;
    int GetCol() const;
    void SetRow(int row);
    void SetCol(int col);

//     %extend {
//         bool __eq__(const wxGBPosition* other) { return other ? (*self == *other) : false; }
//         bool __ne__(const wxGBPosition* other) { return other ? (*self != *other) : true;  }
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




DocStr(wxGBSpan,
"This class is used to hold the row and column spanning attributes of
items in a `wx.GridBagSizer`.  wxPython has typemaps that will
automatically convert from a 2-element sequence of integers to a
wx.GBSpan, so you can use the more pythonic representation of the span
nearly transparently in Python code.
", "");

class wxGBSpan
{
public:
    DocCtorStr(
        wxGBSpan(int rowspan=1, int colspan=1),
        "Construct a new wxGBSpan, optionally setting the rowspan and
colspan. The default is (1,1). (Meaning that the item occupies one
cell in each direction.", "");

    int GetRowspan() const;
    int GetColspan() const;
    void SetRowspan(int rowspan);
    void SetColspan(int colspan);
    
//     %extend {
//         bool __eq__(const wxGBSpan* other) { return other ? (*self == *other) : false; }
//         bool __ne__(const wxGBSpan* other) { return other ? (*self != *other) : true;  }
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


DocStr(wxGBSizerItem,
"The wx.GBSizerItem class is used to track the additional data about
items in a `wx.GridBagSizer` such as the item's position in the grid
and how many rows or columns it spans.
", "");
class wxGBSizerItem : public wxSizerItem
{
public:
    DocCtorStr(
        wxGBSizerItem(),
        "Constructs an empty wx.GBSizerItem.  Either a window, sizer or spacer
size will need to be set, as well as a position and span before this
item can be used in a Sizer.

You will probably never need to create a wx.GBSizerItem directly as they
are created automatically when the sizer's Add method is called.", "");

    %extend {
        DocStr(wxGBSizerItem( wxWindow *window, const wxGBPosition& pos,const wxGBSpan& span,int flag,int border,PyObject* userData=NULL ),
               "Construct a `wx.GBSizerItem` for a window.", "");
        %name(GBSizerItemWindow) wxGBSizerItem( wxWindow *window,
                                                const wxGBPosition& pos,
                                                const wxGBSpan& span,
                                                int flag,
                                                int border,
                                                PyObject* userData=NULL )
            {
                wxPyUserData* data = NULL;
                if ( userData ) {
                    bool blocked = wxPyBeginBlockThreads();
                    data = new wxPyUserData(userData);
                    wxPyEndBlockThreads(blocked);
                }
                return new wxGBSizerItem(window, pos, span, flag, border, data);
            }


        DocStr(wxGBSizerItem( wxSizer *sizer,const wxGBPosition& pos,const wxGBSpan& span,int flag,int border,PyObject* userData=NULL ),
               "Construct a `wx.GBSizerItem` for a sizer", "");
        %name(GBSizerItemSizer) wxGBSizerItem( wxSizer *sizer,
                                               const wxGBPosition& pos,
                                               const wxGBSpan& span,
                                               int flag,
                                               int border,
                                               PyObject* userData=NULL )
            {
                wxPyUserData* data = NULL;
                if ( userData ) {
                    bool blocked = wxPyBeginBlockThreads();
                    data = new wxPyUserData(userData);
                    wxPyEndBlockThreads(blocked);
                }
                return new wxGBSizerItem(sizer, pos, span, flag, border, data);
            }

             
        DocStr(wxGBSizerItem( int width,int height,const wxGBPosition& pos,const wxGBSpan& span,int flag,int border,PyObject* userData=NULL),
               "Construct a `wx.GBSizerItem` for a spacer.", "");
        %name(GBSizerItemSpacer) wxGBSizerItem( int width,
                                                int height,
                                                const wxGBPosition& pos,
                                                const wxGBSpan& span,
                                                int flag,
                                                int border,
                                                PyObject* userData=NULL)
            {
                wxPyUserData* data = NULL;
                if ( userData ) {
                    bool blocked = wxPyBeginBlockThreads();
                    data = new wxPyUserData(userData);
                    wxPyEndBlockThreads(blocked);
                }
                return new wxGBSizerItem(width, height, pos, span, flag, border, data);
            }
    }

    
    DocDeclStr(
        wxGBPosition , GetPos() const,
        "Get the grid position of the item", "");
    
    %pythoncode { def GetPosTuple(self): return self.GetPos().Get() }

    
   
    DocDeclStr(
        wxGBSpan , GetSpan() const,
        "Get the row and column spanning of the item", "");
    
    %pythoncode { def GetSpanTuple(self): return self.GetSpan().Get() }

    
    
    DocDeclStr(
        bool , SetPos( const wxGBPosition& pos ),
        "If the item is already a member of a sizer then first ensure that
there is no other item that would intersect with this one at the new
position, then set the new position.  Returns True if the change is
successful and after the next Layout() the item will be moved.", "");
    

    DocDeclStr(
        bool , SetSpan( const wxGBSpan& span ),
        "If the item is already a member of a sizer then first ensure that
there is no other item that would intersect with this one with its new
spanning size, then set the new spanning.  Returns True if the change
is successful and after the next Layout() the item will be resized.
", "");
    

    
    DocDeclStr(
        bool , Intersects(const wxGBSizerItem& other),
        "Returns True if this item and the other item instersect.", "");
    

    DocDeclStrName(
        bool , Intersects(const wxGBPosition& pos, const wxGBSpan& span),
        "Returns True if the given pos/span would intersect with this item.", "",
        IntersectsPos);
    

    %extend {
        DocStr(GetEndPos,
               "Get the row and column of the endpoint of this item.", "");
        wxGBPosition GetEndPos() {
            int row, col;
            self->GetEndPos(row, col);
            return wxGBPosition(row, col);
        }
    }
    
    
    DocDeclStr(
        wxGridBagSizer* , GetGBSizer() const,
        "Get the sizer this item is a member of.", "");
    
    DocDeclStr(
        void , SetGBSizer(wxGridBagSizer* sizer),
        "Set the sizer this item is a member of.", "");   
    
};


//---------------------------------------------------------------------------

DocStr(wxGridBagSizer,
"A `wx.Sizer` that can lay out items in a virtual grid like a
`wx.FlexGridSizer` but in this case explicit positioning of the items
is allowed using `wx.GBPosition`, and items can optionally span more
than one row and/or column using `wx.GBSpan`.  The total size of the
virtual grid is determined by the largest row and column that items are
positioned at, adjusted for spanning.
", "");

class wxGridBagSizer : public wxFlexGridSizer
{
public:
    DocCtorStr(
        wxGridBagSizer(int vgap = 0, int hgap = 0 ),
        "Constructor, with optional parameters to specify the gap between the
rows and columns.", "");

    
    %extend {
        DocAStr(Add,
                "Add(self, item, GBPosition pos, GBSpan span=DefaultSpan, int flag=0,
int border=0, userData=None) -> wx.GBSizerItem",
                
                "Adds an item to the sizer at the grid cell *pos*, optionally spanning
more than one row or column as specified with *span*.  The remaining
args behave similarly to `wx.Sizer.Add`.

Returns True if the item was successfully placed at the given cell
position, False if something was already there.
", "");
        wxGBSizerItem* Add( PyObject* item,
                            const wxGBPosition& pos,
                            const wxGBSpan& span = wxDefaultSpan,
                            int flag = 0,
                            int border = 0,
                            PyObject* userData = NULL ) {

            wxPyUserData* data = NULL;
            bool blocked = wxPyBeginBlockThreads();
            wxPySizerItemInfo info = wxPySizerItemTypeHelper(item, true, false);
            if ( userData && (info.window || info.sizer || info.gotSize) )
                data = new wxPyUserData(userData);
            wxPyEndBlockThreads(blocked);
            
            // Now call the real Add method if a valid item type was found
            if ( info.window )
                return (wxGBSizerItem*)self->Add(info.window, pos, span, flag, border, data);
            else if ( info.sizer )
                return (wxGBSizerItem*)self->Add(info.sizer, pos, span, flag, border, data);
            else if (info.gotSize)
                return (wxGBSizerItem*)self->Add(info.size.GetWidth(), info.size.GetHeight(),
                                                 pos, span, flag, border, data);
            return NULL;
        }
    }
    
    DocDeclAStrName(
        wxGBSizerItem* , Add( wxGBSizerItem *item ),
        "Add(self, GBSizerItem item) -> wx.GBSizerItem",
        "Add an item to the sizer using a `wx.GBSizerItem`.  Returns True if
the item was successfully placed at its given cell position, False if
something was already there.", "",
        AddItem);

    DocDeclStr(
        wxSize , GetCellSize(int row, int col) const,
        "Get the size of the specified cell, including hgap and
vgap.  Only valid after a Layout.", "");

    DocDeclStr(
        wxSize , GetEmptyCellSize() const,
        "Get the size used for cells in the grid with no item.", "");
    
    DocDeclStr(
        void , SetEmptyCellSize(const wxSize& sz),
        "Set the size used for cells in the grid with no item.", "");
    

    
    %nokwargs GetItemPosition;
    %noautodoc GetItemPosition;
    DocStr(GetItemPosition,
           "GetItemPosition(self, item) -> GBPosition

Get the grid position of the specified *item* where *item* is either a
window or subsizer that is a member of this sizer, or a zero-based
index of an item.", "");
    wxGBPosition GetItemPosition(wxWindow *window);
    wxGBPosition GetItemPosition(wxSizer *sizer);
    wxGBPosition GetItemPosition(size_t index);

    
    %nokwargs SetItemPosition;
    %noautodoc SetItemPosition;
    DocStr(SetItemPosition,
           "SetItemPosition(self, item, GBPosition pos) -> bool

Set the grid position of the specified *item* where *item* is either a
window or subsizer that is a member of this sizer, or a zero-based
index of an item.  Returns True on success.  If the move is not
allowed (because an item is already there) then False is returned.
", "");
    bool SetItemPosition(wxWindow *window, const wxGBPosition& pos);
    bool SetItemPosition(wxSizer *sizer, const wxGBPosition& pos);
    bool SetItemPosition(size_t index, const wxGBPosition& pos);


    
    %nokwargs GetItemSpan;
    %noautodoc GetItemSpan;
    DocStr(GetItemSpan,
           "GetItemSpan(self, item) -> GBSpan

Get the row/col spanning of the specified *item* where *item* is
either a window or subsizer that is a member of this sizer, or a
zero-based index of an item.", "");
    wxGBSpan GetItemSpan(wxWindow *window);
    wxGBSpan GetItemSpan(wxSizer *sizer);
    wxGBSpan GetItemSpan(size_t index);


    
    %nokwargs SetItemSpan;
    %noautodoc SetItemSpan;
    DocStr(SetItemSpan,
            "SetItemSpan(self, item, GBSpan span) -> bool

Set the row/col spanning of the specified *item* where *item* is
either a window or subsizer that is a member of this sizer, or a
zero-based index of an item.  Returns True on success.  If the move is
not allowed (because an item is already there) then False is returned.", "");
    bool SetItemSpan(wxWindow *window, const wxGBSpan& span);
    bool SetItemSpan(wxSizer *sizer, const wxGBSpan& span);
    bool SetItemSpan(size_t index, const wxGBSpan& span);

    

    %nokwargs FindItem;
    %noautodoc FindItem;
    DocStr(FindItem,
            "FindItem(self, item) -> GBSizerItem

Find the sizer item for the given window or subsizer, returns None if
not found. (non-recursive)", "");
    wxGBSizerItem* FindItem(wxWindow* window);
    wxGBSizerItem* FindItem(wxSizer* sizer);

    
    DocDeclStr(
        wxGBSizerItem* , FindItemAtPosition(const wxGBPosition& pos),
        "Return the sizer item for the given grid cell, or None if there is no
item at that position. (non-recursive)", "");
    

    
    DocDeclStr(
        wxGBSizerItem* , FindItemAtPoint(const wxPoint& pt),
        "Return the sizer item located at the point given in *pt*, or None if
there is no item at that point. The (x,y) coordinates in pt correspond
to the client coordinates of the window using the sizer for
layout. (non-recursive)", "");
    

    
//     DocDeclStr(
//         wxGBSizerItem* , FindItemWithData(const wxObject* userData),
//         "Return the sizer item that has a matching user data (it only compares
// pointer values) or None if not found. (non-recursive)", "");
    
    

    // Look at all items and see if any intersect (or would overlap) the given
    // item.  Returns True if so, False if there would be no overlap.  If an
    // excludeItem is given then it will not be checked for intersection, for
    // example it may be the item we are checking the position of.


    DocDeclStr(
        bool , CheckForIntersection(wxGBSizerItem* item, wxGBSizerItem* excludeItem = NULL),
        "Look at all items and see if any intersect (or would overlap) the
given *item*.  Returns True if so, False if there would be no overlap.
If an *excludeItem* is given then it will not be checked for
intersection, for example it may be the item we are checking the
position of.
", "");
    
    DocDeclStrName(
        bool , CheckForIntersection(const wxGBPosition& pos, const wxGBSpan& span, wxGBSizerItem* excludeItem = NULL),
        "Look at all items and see if any intersect (or would overlap) the
given position and span.  Returns True if so, False if there would be
no overlap.  If an *excludeItem* is given then it will not be checked
for intersection, for example it may be the item we are checking the
position of.", "",
        CheckForIntersectionPos);
    

};


//---------------------------------------------------------------------------
