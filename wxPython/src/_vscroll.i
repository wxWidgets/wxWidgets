/////////////////////////////////////////////////////////////////////////////
// Name:        _vscroll.i
// Purpose:     SWIG interface defs for wxVScrolledWindow, wxVListBox, and
//              wxHtmlListBox
//
// Author:      Robin Dunn
//
// Created:     14-Aug-2003
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

%{
#include <wx/tipwin.h>
#include <wx/vscroll.h>
%}

%newgroup;
//---------------------------------------------------------------------------
// Base classes.  I don't expect that these will ever be used directly from
// Python, (especially being derived from) so I own't worry about making their
// virutals overridable.  They're just defined here for their public methods.



// Provides all base common scroll calculations needed for either orientation,
// automatic scrollbar functionality, saved scroll positions, functionality
// for changing the target window to be scrolled, as well as defining all
// required virtual functions that need to be implemented for any orientation
// specific work.

class wxVarScrollHelperBase
{
public:

//     wxVarScrollHelperBase(wxWindow *winToScroll);    *** ABC, can't instantiate
//     virtual ~wxVarScrollHelperBase();


    // with physical scrolling on, the device origin is changed properly when
    // a wxPaintDC is prepared, children are actually moved and laid out
    // properly, and the contents of the window (pixels) are actually moved
    void EnablePhysicalScrolling(bool scrolling = true);

    // wxNOT_FOUND if none, i.e. if it is below the last item
    virtual int HitTest(wxCoord coord) const;

    // recalculate all our parameters and redisplay all units
    virtual void RefreshAll();


    // get the first currently visible unit
    size_t GetVisibleBegin() const;

    // get the last currently visible unit
    size_t GetVisibleEnd() const;

    // is this unit currently visible?
    bool IsVisible(size_t unit) const;

    // translate between scrolled and unscrolled coordinates
    int CalcScrolledPosition(int coord) const;
    int CalcUnscrolledPosition(int coord) const;

    // update the thumb size shown by the scrollbar
    virtual void UpdateScrollbar();
    void RemoveScrollbar();

    // Normally the wxScrolledWindow will scroll itself, but in some rare
    // occasions you might want it to scroll [part of] another window (e.g. a
    // child of it in order to scroll only a portion the area between the
    // scrollbars (spreadsheet: only cell area will move).
    virtual void SetTargetWindow(wxWindow *target);
    virtual wxWindow *GetTargetWindow() const;


    // these functions must be overidden in the derived class to return
    // orientation specific data (e.g. the width for vertically scrolling
    // derivatives in the case of GetOrientationTargetSize())
    virtual int GetOrientationTargetSize() const; // = 0;
    virtual int GetNonOrientationTargetSize() const; // = 0;
    virtual wxOrientation GetOrientation() const; // = 0;
};





// Provides public API functions targeted for vertical-specific scrolling,
// wrapping the functionality of wxVarScrollHelperBase.

class wxVarVScrollHelper : public wxVarScrollHelperBase
{
public:

//    wxVarVScrollHelper(wxWindow *winToScroll);    *** ABC, can't instantiate
    
    void SetRowCount(size_t rowCount);
    bool ScrollToRow(size_t row);

    virtual bool ScrollRows(int rows);
    virtual bool ScrollRowPages(int pages);

    virtual void RefreshRow(size_t row);
    virtual void RefreshRows(size_t from, size_t to);

    virtual int HitTest(wxCoord y) const;


    size_t GetRowCount() const;
    size_t GetVisibleRowsBegin() const;
    size_t GetVisibleRowsEnd() const;
    bool IsRowVisible(size_t row) const;

    virtual int GetOrientationTargetSize() const;
    virtual int GetNonOrientationTargetSize() const;
    virtual wxOrientation GetOrientation() const;
};





// Provides public API functions targeted for horizontal-specific scrolling,
// wrapping the functionality of wxVarScrollHelperBase.

class wxVarHScrollHelper : public wxVarScrollHelperBase
{
public:

//     wxVarHScrollHelper(wxWindow *winToScroll)     *** ABC, can't instantiate

    void SetColumnCount(size_t columnCount);

    bool ScrollToColumn(size_t column);
    virtual bool ScrollColumns(int columns);
    virtual bool ScrollColumnPages(int pages);

    virtual void RefreshColumn(size_t column);
    virtual void RefreshColumns(size_t from, size_t to);
    virtual int HitTest(wxCoord x) const;


    size_t GetColumnCount() const;
    size_t GetVisibleColumnsBegin() const;
    size_t GetVisibleColumnsEnd() const;
    bool IsColumnVisible(size_t column) const;


    virtual int GetOrientationTargetSize() const;
    virtual int GetNonOrientationTargetSize() const;
    virtual wxOrientation GetOrientation() const;
    
};






// Provides public API functions targeted at functions with similar names in
// both wxVScrollHelper and wxHScrollHelper so class scope doesn't need to be
// specified (since we are using multiple inheritance). It also provides
// functions to make changing values for both orientations at the same time
// easier.

class wxVarHVScrollHelper : public wxVarVScrollHelper,
                            public wxVarHScrollHelper
{
public:

//     wxVarHVScrollHelper(wxWindow *winToScroll);   *** ABC, can't instantiate


    // set the number of units the window contains for each axis: the derived
    // class must provide the widths and heights for all units with indices up
    // to each of the one given here in its OnGetColumnWidth() and
    // OnGetRowHeight()
    void SetRowColumnCount(size_t rowCount, size_t columnCount);


    // with physical scrolling on, the device origin is changed properly when
    // a wxPaintDC is prepared, children are actually moved and laid out
    // properly, and the contents of the window (pixels) are actually moved
    void EnablePhysicalScrolling(bool vscrolling = true, bool hscrolling = true);
    

    // scroll to the specified row/column: it will become the first visible
    // cell in the window
    //
    // return true if we scrolled the window, false if nothing was done
//     bool ScrollToRowColumn(size_t row, size_t column);
    bool ScrollToRowColumn(const wxPosition &pos);

    // redraw the specified cell
//     virtual void RefreshRowColumn(size_t row, size_t column);
    virtual void RefreshRowColumn(const wxPosition &pos);

    // redraw the specified regions (inclusive).  If the target window for
    // both orientations is the same the rectangle of cells is refreshed; if
    // the target windows differ the entire client size opposite the
    // orientation direction is refreshed between the specified limits
//     virtual void RefreshRowsColumns(size_t fromRow, size_t toRow,
//                                     size_t fromColumn, size_t toColumn);
    virtual void RefreshRowsColumns(const wxPosition& from,
                                    const wxPosition& to);

    // Override wxPanel::HitTest to use our version
//     virtual wxPosition HitTest(wxCoord x, wxCoord y) const;
    virtual wxPosition HitTest(const wxPoint &pos) const;

    // replacement implementation of wxWindow::Layout virtual method.  To
    // properly forward calls to wxWindow::Layout use
    // WX_FORWARD_TO_SCROLL_HELPER() derived class. We use this version to
    // call both base classes' ScrollLayout()
    bool ScrollLayout();

    // get the number of units this window contains (previously set by
    // Set[Column/Row/RowColumn/Unit]Count())
    wxSize GetRowColumnCount() const;

    // get the first currently visible units
    wxPosition GetVisibleBegin() const;
    wxPosition GetVisibleEnd() const;

    // is this cell currently visible?
//     bool IsVisible(size_t row, size_t column) const;
    bool IsVisible(const wxPosition &pos) const;
};




//---------------------------------------------------------------------------
// wxVScrolledWindow



// First, the C++ version that can redirect to overridden Python methods
%{
class wxPyVScrolledWindow  : public wxVScrolledWindow
{
    DECLARE_ABSTRACT_CLASS(wxPyVScrolledWindow)
public:
    wxPyVScrolledWindow() : wxVScrolledWindow() {}

    wxPyVScrolledWindow(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPyPanelNameStr)
        : wxVScrolledWindow(parent, id, pos, size, style, name)
    {}

    // Overridable virtuals

    // this function must be overridden in the derived class and it should
    // return the height of the given line in pixels
    DEC_PYCALLBACK_COORD_SIZET_constpure(OnGetRowHeight);
    DEC_PYCALLBACK_COORD_SIZET_constpure(OnGetLineHeight);  // old name

    // this function doesn't have to be overridden but it may be useful to do
    // it if calculating the lines heights is a relatively expensive operation
    // as it gives the user code a possibility to calculate several of them at
    // once
    //
    // OnGetLinesHint() is normally called just before OnGetRowHeight() but you
    // shouldn't rely on the latter being called for all lines in the interval
    // specified here. It is also possible that OnGetRowHeight() will be
    // called for the lines outside of this interval, so this is really just a
    // hint, not a promise.
    //
    // finally note that lineMin is inclusive, while lineMax is exclusive, as
    // usual
    DEC_PYCALLBACK_VOID_SIZETSIZET_const(OnGetRowsHeightHint);
    DEC_PYCALLBACK_VOID_SIZETSIZET_const(OnGetLinesHint);      // old name

    // when the number of lines changes, we try to estimate the total height
    // of all lines which is a rather expensive operation in terms of lines
    // access, so if the user code may estimate the average height
    // better/faster than we do, it should override this function to implement
    // its own logic
    //
    // this function should return the best guess for the total height it may
    // make
    DEC_PYCALLBACK_COORD_const(EstimateTotalHeight);


    // Also expose some other interesting protected methods


    // get the total height of the lines between lineMin (inclusive) and
    // lineMax (exclusive)
    wxCoord GetRowsHeight(size_t lineMin, size_t lineMax) const
    { return wxVScrolledWindow::GetRowsHeight(lineMin, lineMax); }


    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyVScrolledWindow, wxVScrolledWindow);

IMP_PYCALLBACK_COORD_SIZET_constpure(wxPyVScrolledWindow, wxVScrolledWindow, OnGetRowHeight);
IMP_PYCALLBACK_COORD_SIZET_constpure(wxPyVScrolledWindow, wxVScrolledWindow, OnGetLineHeight);
IMP_PYCALLBACK_VOID_SIZETSIZET_const(wxPyVScrolledWindow, wxVScrolledWindow, OnGetRowsHeightHint);
IMP_PYCALLBACK_VOID_SIZETSIZET_const(wxPyVScrolledWindow, wxVScrolledWindow, OnGetLinesHint);

IMP_PYCALLBACK_COORD_const          (wxPyVScrolledWindow, wxVScrolledWindow, EstimateTotalHeight);
%}





// Now define this class for SWIG

/*
   In the name of this class, "V" may stand for "variable" because it can be
   used for scrolling lines of variable heights; "virtual" because it is not
   necessary to know the heights of all lines in advance -- only those which
   are shown on the screen need to be measured; or, even, "vertical" because
   this class only supports scrolling in one direction currently (this could
   and probably will change in the future however).

   In any case, this is a generalization of the wxScrolledWindow class which
   can be only used when all lines have the same height. It lacks some other
   wxScrolledWindow features however, notably it currently lacks support for
   horizontal scrolling; it can't scroll another window nor only a rectangle
   of the window and not its entire client area.
 */

MustHaveApp(wxPyVScrolledWindow);

%rename(VScrolledWindow) wxPyVScrolledWindow;

class wxPyVScrolledWindow : public wxPanel,
                            public wxVarVScrollHelper
{
public:
    %pythonAppend wxPyVScrolledWindow         "self._setOORInfo(self);" setCallbackInfo(VScrolledWindow)
    %pythonAppend wxPyVScrolledWindow()       ""
    

    wxPyVScrolledWindow(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPyPanelNameStr);

    %RenameCtor(PreVScrolledWindow, wxPyVScrolledWindow());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyPanelNameStr);


    // get the total height of the lines between lineMin (inclusive) and
    // lineMax (exclusive)
    wxCoord GetRowsHeight(size_t lineMin, size_t lineMax) const;
    %pythoncode { GetLinesHeight = wx._deprecated(GetRowsHeight,
                                                  "Use GetRowsHeight instead.") }
                                                  
    virtual wxCoord EstimateTotalHeight() const;

    int HitTest(const wxPoint& pt) const;

    
    // Deprecated wrappers for methods whose name changed when adding the H
    // classes.  I just put them here instead of wrapping the
    // wxVarVScrollLegacyAdaptor class.
    %pythoncode {
        def GetFirstVisibleLine(self):
            return self.GetVisibleRowsBegin()
        GetFirstVisibleLine = wx._deprecated(GetFirstVisibleLine, "Use GetVisibleRowsBegin instead" )

        def GetLastVisibleLine(self):
            return self.GetVisibleRowsEnd() - 1
        GetLastVisibleLine = wx._deprecated(GetLastVisibleLine, "Use GetVisibleRowsEnd instead")

        def GetLineCount(self):
            return self.GetRowCount()
        GetLineCount = wx._deprecated(GetLineCount, "Use GetRowCount instead")

        def SetLineCount(self, count):
            self.SetRowCount(count)
        SetLineCount = wx._deprecated(SetLineCount, "Use SetRowCount instead")

        def RefreshLine(self, line):
            self.RefreshRow(line)
        RefreshLine = wx._deprecated(RefreshLine, "Use RefreshRow instead")

        def RefreshLines(self, frm, to):
            self.RefreshRows(frm, to)
        RefreshLines = wx._deprecated(RefreshLines, "Use RefreshRows instead")
            
        def ScrollToLine(self, line):
            return self.ScrollToRow(line)
        ScrollToLine = wx._deprecated(ScrollToLine, "Use RefreshRow instead")

        def ScrollLines(self, lines):
            return self.ScrollRows(lines)
        ScrollLines = wx._deprecated(ScrollLines, "Use ScrollRows instead")

        def ScrollPages(self, pages):
            return self.ScrollRowPages(pages)
        ScrollPages = wx._deprecated(ScrollPages, "Use ScrollRowPages instead")
    }
    
};



//---------------------------------------------------------------------------
// wxHScrolledWindow


// First, the C++ version that can redirect to overridden Python methods
%{
class wxPyHScrolledWindow  : public wxHScrolledWindow
{
    DECLARE_ABSTRACT_CLASS(wxPyHScrolledWindow)
public:
    wxPyHScrolledWindow() : wxHScrolledWindow() {}

    wxPyHScrolledWindow(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPyPanelNameStr)
        : wxHScrolledWindow(parent, id, pos, size, style, name)
    {}

    // Overridable virtuals
    DEC_PYCALLBACK_COORD_SIZET_constpure(OnGetColumnWidth);
    DEC_PYCALLBACK_VOID_SIZETSIZET_const(OnGetColumnsWidthHint);
    DEC_PYCALLBACK_COORD_const(EstimateTotalWidth);

    wxCoord GetColumnsWidth(size_t columnMin, size_t columnMax) const
    { return wxHScrolledWindow::GetColumnsWidth(columnMin, columnMax); }

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyHScrolledWindow, wxHScrolledWindow);

IMP_PYCALLBACK_COORD_SIZET_constpure(wxPyHScrolledWindow, wxHScrolledWindow, OnGetColumnWidth);
IMP_PYCALLBACK_VOID_SIZETSIZET_const(wxPyHScrolledWindow, wxHScrolledWindow, OnGetColumnsWidthHint);
IMP_PYCALLBACK_COORD_const          (wxPyHScrolledWindow, wxHScrolledWindow, EstimateTotalWidth);

%}



// Now define this class for SWIG

// In the name of this class, "H" stands for "horizontal" because it can be
// used for scrolling columns of variable widths. It is not necessary to know
// the widths of all columns in advance -- only those which are shown on the
// screen need to be measured.

// This is a generalization of the wxScrolledWindow class which can be only
// used when all columns have the same width. It lacks some other
// wxScrolledWindow features however, notably it can't scroll only a rectangle
// of the window and not its entire client area.

MustHaveApp(wxPyHScrolledWindow);

%rename(HScrolledWindow) wxPyHScrolledWindow;

class wxPyHScrolledWindow : public wxPanel,
                            public wxVarHScrollHelper
{
public:
    %pythonAppend wxPyHScrolledWindow         "self._setOORInfo(self);" setCallbackInfo(HScrolledWindow)
    %pythonAppend wxPyHScrolledWindow()       ""
    

    wxPyHScrolledWindow(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPyPanelNameStr);

    %RenameCtor(PreHScrolledWindow, wxPyHScrolledWindow());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyPanelNameStr);

    
    int HitTest(const wxPoint& pt) const;
    wxCoord GetColumnsWidth(size_t columnMin, size_t columnMax) const;
    wxCoord EstimateTotalWidth() const;
};



//---------------------------------------------------------------------------
// wxHVScrolledWindow



// First, the C++ version that can redirect to overridden Python methods
%{
class wxPyHVScrolledWindow  : public wxHVScrolledWindow
{
    DECLARE_ABSTRACT_CLASS(wxPyHScrolledWindow)
public:
    wxPyHVScrolledWindow() : wxHVScrolledWindow() {}

    wxPyHVScrolledWindow(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPyPanelNameStr)
        : wxHVScrolledWindow(parent, id, pos, size, style, name)
    {}

    // Overridable virtuals
    DEC_PYCALLBACK_COORD_SIZET_constpure(OnGetRowHeight);
    DEC_PYCALLBACK_VOID_SIZETSIZET_const(OnGetRowsHeightHint);
    DEC_PYCALLBACK_COORD_const(EstimateTotalHeight);

    DEC_PYCALLBACK_COORD_SIZET_constpure(OnGetColumnWidth);
    DEC_PYCALLBACK_VOID_SIZETSIZET_const(OnGetColumnsWidthHint);
    DEC_PYCALLBACK_COORD_const(EstimateTotalWidth);

    wxCoord GetRowsHeight(size_t lineMin, size_t lineMax) const
    { return wxHVScrolledWindow::GetRowsHeight(lineMin, lineMax); }

    wxCoord GetColumnsWidth(size_t columnMin, size_t columnMax) const
    { return wxHVScrolledWindow::GetColumnsWidth(columnMin, columnMax); }

    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyHVScrolledWindow, wxHVScrolledWindow);

IMP_PYCALLBACK_COORD_SIZET_constpure(wxPyHVScrolledWindow, wxHVScrolledWindow, OnGetRowHeight);
IMP_PYCALLBACK_VOID_SIZETSIZET_const(wxPyHVScrolledWindow, wxHVScrolledWindow, OnGetRowsHeightHint);
IMP_PYCALLBACK_COORD_const          (wxPyHVScrolledWindow, wxHVScrolledWindow, EstimateTotalHeight);

IMP_PYCALLBACK_COORD_SIZET_constpure(wxPyHVScrolledWindow, wxHVScrolledWindow, OnGetColumnWidth);
IMP_PYCALLBACK_VOID_SIZETSIZET_const(wxPyHVScrolledWindow, wxHVScrolledWindow, OnGetColumnsWidthHint);
IMP_PYCALLBACK_COORD_const          (wxPyHVScrolledWindow, wxHVScrolledWindow, EstimateTotalWidth);

%}




// Now define this class for SWIG

// This window inherits all functionality of both vertical and horizontal
// scrolled windows automatically handling everything needed to scroll both
// axis simultaneously.

MustHaveApp(wxPyHVScrolledWindow);

%rename(HVScrolledWindow) wxPyHVScrolledWindow;

class wxPyHVScrolledWindow : public wxPanel,
                             public wxVarHVScrollHelper
{
public:
    %pythonAppend wxPyHVScrolledWindow         "self._setOORInfo(self);" setCallbackInfo(HVScrolledWindow)
    %pythonAppend wxPyHVScrolledWindow()       ""
    

    wxPyHVScrolledWindow(wxWindow *parent,
                         wxWindowID id = wxID_ANY,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = 0,
                         const wxString& name = wxPyPanelNameStr);

    %RenameCtor(PreHVScrolledWindow, wxPyHVScrolledWindow());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyPanelNameStr);

    
    wxPosition HitTest(const wxPoint& pt) const;

    wxCoord GetRowsHeight(size_t lineMin, size_t lineMax) const;
    wxCoord EstimateTotalHeight() const;

    wxCoord GetColumnsWidth(size_t columnMin, size_t columnMax) const;
    wxCoord EstimateTotalWidth() const;
};



//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// wxVListBox

%{
#include <wx/vlbox.h>
%}

MAKE_CONST_WXSTRING(VListBoxNameStr);


// First, the C++ version
%{
class wxPyVListBox  : public wxVListBox
{
    DECLARE_ABSTRACT_CLASS(wxPyVListBox)
public:
    wxPyVListBox() : wxVListBox() {}

    wxPyVListBox(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyVListBoxNameStr)
        : wxVListBox(parent, id, pos, size, style, name)
    {}

    // Overridable virtuals

    // the derived class must implement this function to actually draw the item
    // with the given index on the provided DC
    // virtual void OnDrawItem(wxDC& dc, const wxRect& rect, size_t n) const = 0;
    DEC_PYCALLBACK__DCRECTSIZET_constpure(OnDrawItem);


    // the derived class must implement this method to return the height of the
    // specified item
    // virtual wxCoord OnMeasureItem(size_t n) const = 0;
    DEC_PYCALLBACK_COORD_SIZET_constpure(OnMeasureItem);


    // this method may be used to draw separators between the lines; note that
    // the rectangle may be modified, typically to deflate it a bit before
    // passing to OnDrawItem()
    //
    // the base class version doesn't do anything
    //    virtual void OnDrawSeparator(wxDC& dc, wxRect& rect, size_t n) const;
    DEC_PYCALLBACK__DCRECTSIZET2_const(OnDrawSeparator);


    // this method is used to draw the items background and, maybe, a border
    // around it
    //
    // the base class version implements a reasonable default behaviour which
    // consists in drawing the selected item with the standard background
    // colour and drawing a border around the item if it is either selected or
    // current
    //     virtual void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;
    DEC_PYCALLBACK__DCRECTSIZET_const(OnDrawBackground);


    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyVListBox, wxVListBox);

IMP_PYCALLBACK__DCRECTSIZET_constpure(wxPyVListBox, wxVListBox, OnDrawItem);
IMP_PYCALLBACK_COORD_SIZET_constpure (wxPyVListBox, wxVListBox, OnMeasureItem);
IMP_PYCALLBACK__DCRECTSIZET2_const   (wxPyVListBox, wxVListBox, OnDrawSeparator);
IMP_PYCALLBACK__DCRECTSIZET_const    (wxPyVListBox, wxVListBox, OnDrawBackground);

%}



// Now define this class for SWIG

/*
    This class has two main differences from a regular listbox: it can have an
    arbitrarily huge number of items because it doesn't store them itself but
    uses OnDrawItem() callback to draw them and its items can have variable
    height as determined by OnMeasureItem().

    It emits the same events as wxListBox and the same event macros may be used
    with it.
 */
MustHaveApp(wxPyVListBox);

%rename(VListBox) wxPyVListBox;
class wxPyVListBox : public wxPyVScrolledWindow
{
public:
    %pythonAppend wxPyVListBox         "self._setOORInfo(self);" setCallbackInfo(VListBox)
    %pythonAppend wxPyVListBox()       ""
    

    wxPyVListBox(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyVListBoxNameStr);

    %RenameCtor(PreVListBox,  wxPyVListBox());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyVListBoxNameStr);

    // get the number of items in the control
    size_t GetItemCount() const;

    // does this control use multiple selection?
    bool HasMultipleSelection() const;

    // get the currently selected item or wxNOT_FOUND if there is no selection
    //
    // this method is only valid for the single selection listboxes
    int GetSelection() const;

    // is this item the current one?
    bool IsCurrent(size_t item) const;

    // is this item selected?
    bool IsSelected(size_t item) const;

    // get the number of the selected items (maybe 0)
    //
    // this method is valid for both single and multi selection listboxes
    size_t GetSelectedCount() const;

    %extend {
        // get the first selected item, returns wxNOT_FOUND if none
        //
        // cookie is an opaque parameter which should be passed to
        // GetNextSelected() later
        //
        // this method is only valid for the multi selection listboxes
        //int GetFirstSelected(unsigned long& cookie) const;
        PyObject* GetFirstSelected() {
            unsigned long cookie = 0;
            int selected = self->GetFirstSelected(cookie);
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(selected));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(cookie));
            wxPyEndBlockThreads(blocked);
            return tup;           
        }   

        // get next selection item, return wxNOT_FOUND if no more
        //
        // cookie must be the same parameter that was passed to GetFirstSelected()
        // before
        //
        // this method is only valid for the multi selection listboxes
        // int GetNextSelected(unsigned long& cookie) const;
        PyObject* GetNextSelected(unsigned long cookie) {
            int selected = self->GetNextSelected(cookie);
            wxPyBlock_t blocked = wxPyBeginBlockThreads();
            PyObject* tup = PyTuple_New(2);
            PyTuple_SET_ITEM(tup, 0, PyInt_FromLong(selected));
            PyTuple_SET_ITEM(tup, 1, PyInt_FromLong(cookie));
            wxPyEndBlockThreads(blocked);
            return tup;           
        }   
    }

    
    // get the margins around each item
    wxPoint GetMargins() const;

    // get the background colour of selected cells
    const wxColour& GetSelectionBackground() const;


    // set the number of items to be shown in the control
    //
    // this is just a synonym for wxVScrolledWindow::SetLineCount()
    void SetItemCount(size_t count);

    // delete all items from the control
    void Clear();

    // set the selection to the specified item, if it is wxNOT_FOUND the
    // selection is unset
    //
    // this function is only valid for the single selection listboxes
    void SetSelection(int selection);

    // selects or deselects the specified item which must be valid (i.e. not
    // equal to wxNOT_FOUND)
    //
    // return True if the items selection status has changed or False
    // otherwise
    //
    // this function is only valid for the multiple selection listboxes
    bool Select(size_t item, bool select = true);

    // selects the items in the specified range whose end points may be given
    // in any order
    //
    // return True if any items selection status has changed, False otherwise
    //
    // this function is only valid for the single selection listboxes
    bool SelectRange(size_t from, size_t to);

    // toggle the selection of the specified item (must be valid)
    //
    // this function is only valid for the multiple selection listboxes
    void Toggle(size_t item);

    // select all items in the listbox
    //
    // the return code indicates if any items were affected by this operation
    // (True) or if nothing has changed (False)
    bool SelectAll();

    // unselect all items in the listbox
    //
    // the return code has the same meaning as for SelectAll()
    bool DeselectAll();

    // set the margins: horizontal margin is the distance between the window
    // border and the item contents while vertical margin is half of the
    // distance between items
    //
    // by default both margins are 0
    void SetMargins(const wxPoint& pt);
    %Rename(SetMarginsXY, void, SetMargins(wxCoord x, wxCoord y));

    // change the background colour of the selected cells
    void SetSelectionBackground(const wxColour& col);

    // refreshes only the selected items
    void RefreshSelected();
 
    virtual void OnDrawSeparator(wxDC& dc, wxRect& rect, size_t n) const;
    virtual void OnDrawBackground(wxDC& dc, const wxRect& rect, size_t n) const;

    %property(FirstSelected, GetFirstSelected, doc="See `GetFirstSelected`");
    %property(ItemCount, GetItemCount, SetItemCount, doc="See `GetItemCount` and `SetItemCount`");
    %property(Margins, GetMargins, SetMargins, doc="See `GetMargins` and `SetMargins`");
    %property(SelectedCount, GetSelectedCount, doc="See `GetSelectedCount`");
    %property(Selection, GetSelection, SetSelection, doc="See `GetSelection` and `SetSelection`");
    %property(SelectionBackground, GetSelectionBackground, SetSelectionBackground, doc="See `GetSelectionBackground` and `SetSelectionBackground`");
};


//---------------------------------------------------------------------------
// wxHtmlListBox

%{
#include <wx/htmllbox.h>
%}

// First, the C++ version
%{
class wxPyHtmlListBox  : public wxHtmlListBox
{
    DECLARE_ABSTRACT_CLASS(wxPyHtmlListBox)
public:
    wxPyHtmlListBox() : wxHtmlListBox() {}

    wxPyHtmlListBox(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyVListBoxNameStr)
        : wxHtmlListBox(parent, id, pos, size, style, name)
    {}

    // Overridable virtuals

    // this method must be implemented in the derived class and should return
    // the body (i.e. without <html>) of the HTML for the given item
    DEC_PYCALLBACK_STRING_SIZET_pure(OnGetItem);

    // this function may be overridden to decorate HTML returned by OnGetItem()
    DEC_PYCALLBACK_STRING_SIZET(OnGetItemMarkup);

    // These are from wxVListBox
    DEC_PYCALLBACK__DCRECTSIZET2_const(OnDrawSeparator);
    DEC_PYCALLBACK__DCRECTSIZET_const(OnDrawBackground);

// TODO:
//     // this method allows to customize the selection appearance: it may be used
//     // to specify the colour of the text which normally has the given colour
//     // colFg when it is inside the selection
//     //
//     // by default, the original colour is not used at all and all text has the
//     // same (default for this system) colour inside selection
//     virtual wxColour GetSelectedTextColour(const wxColour& colFg) const;

//     // this is the same as GetSelectedTextColour() but allows to customize the
//     // background colour -- this is even more rarely used as you can change it
//     // globally using SetSelectionBackground()
//     virtual wxColour GetSelectedTextBgColour(const wxColour& colBg) const;

    
    // This method may be overriden to handle clicking on a link in
    // the listbox. By default, clicking links is ignored.
    virtual void OnLinkClicked(size_t n,
                               const wxHtmlLinkInfo& link);        

    PYPRIVATE;
};


IMPLEMENT_ABSTRACT_CLASS(wxPyHtmlListBox, wxHtmlListBox)

IMP_PYCALLBACK_STRING_SIZET_pure(wxPyHtmlListBox, wxHtmlListBox, OnGetItem);
IMP_PYCALLBACK_STRING_SIZET     (wxPyHtmlListBox, wxHtmlListBox, OnGetItemMarkup);
IMP_PYCALLBACK__DCRECTSIZET2_const   (wxPyHtmlListBox, wxHtmlListBox, OnDrawSeparator);
IMP_PYCALLBACK__DCRECTSIZET_const    (wxPyHtmlListBox, wxHtmlListBox, OnDrawBackground);


void wxPyHtmlListBox::OnLinkClicked(size_t n,
                                    const wxHtmlLinkInfo& link) {
    bool found;
    wxPyBlock_t blocked = wxPyBeginBlockThreads();
    if ((found = wxPyCBH_findCallback(m_myInst, "OnLinkClicked"))) {
        PyObject* obj = wxPyConstructObject((void*)&link, wxT("wxHtmlLinkInfo"), 0);
        wxPyCBH_callCallback(m_myInst, Py_BuildValue("(iO)", n, obj));
        Py_DECREF(obj);
    }
    wxPyEndBlockThreads(blocked);
    if (! found)
        wxPyHtmlListBox::OnLinkClicked(n, link);
}
 
%}



// Now define this class for SWIG


// wxHtmlListBox is a listbox whose items are wxHtmlCells
MustHaveApp(wxPyHtmlListBox);
%rename(HtmlListBox) wxPyHtmlListBox;
class wxPyHtmlListBox : public wxPyVListBox
{
public:
    %pythonAppend wxPyHtmlListBox         "self._setOORInfo(self);" setCallbackInfo(HtmlListBox)
    %pythonAppend wxPyHtmlListBox()       ""
    

    // normal constructor which calls Create() internally
    wxPyHtmlListBox(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyVListBoxNameStr);

    %RenameCtor(PreHtmlListBox,  wxPyHtmlListBox());

    void _setCallbackInfo(PyObject* self, PyObject* _class);

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyVListBoxNameStr);


    void RefreshAll();
    void SetItemCount(size_t count);

    // retrieve the file system used by the wxHtmlWinParser: if you use
    // relative paths in your HTML, you should use its ChangePathTo() method
    wxFileSystem& GetFileSystem();

    void OnLinkClicked(size_t n, const wxHtmlLinkInfo& link);        

    %property(FileSystem, GetFileSystem, doc="See `GetFileSystem`");
};



//---------------------------------------------------------------------------

%{
    const wxArrayString wxPyEmptyStringArray;
%}
MAKE_CONST_WXSTRING(SimpleHtmlListBoxNameStr);


enum {
    wxHLB_DEFAULT_STYLE,
    wxHLB_MULTIPLE
};

MustHaveApp(wxSimpleHtmlListBox);

class wxSimpleHtmlListBox : public wxPyHtmlListBox,
                            public wxItemContainer
{
public:
    %pythonAppend wxSimpleHtmlListBox         "self._setOORInfo(self)";
    %pythonAppend wxSimpleHtmlListBox()       "";
        
    wxSimpleHtmlListBox(wxWindow *parent,
                        wxWindowID id = -1,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        const wxArrayString& choices = wxPyEmptyStringArray,
                        long style = wxHLB_DEFAULT_STYLE,
                        const wxValidator& validator = wxDefaultValidator,
                        const wxString& name = wxPySimpleHtmlListBoxNameStr);
    %RenameCtor(PreSimpleHtmlListBox, wxSimpleHtmlListBox());

    bool Create(wxWindow *parent,
                wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size= wxDefaultSize,
                const wxArrayString& choices = wxPyEmptyStringArray,
                long style = wxHLB_DEFAULT_STYLE,
                const wxValidator& validator = wxDefaultValidator,
                const wxString& name = wxPySimpleHtmlListBoxNameStr);
};

//---------------------------------------------------------------------------

%init %{
    // Map renamed classes back to their common name for OOR
    wxPyPtrTypeMap_Add("wxHtmlListBox",     "wxPyHtmlListBox");
    wxPyPtrTypeMap_Add("wxVListBox",        "wxPyVListBox");
    wxPyPtrTypeMap_Add("wxVScrolledWindow", "wxPyVScrolledWindow");
%}

//---------------------------------------------------------------------------
