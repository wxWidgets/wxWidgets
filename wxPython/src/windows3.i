/////////////////////////////////////////////////////////////////////////////
// Name:        windows3.i
// Purpose:     SWIG definitions of MORE window classes
//
// Author:      Robin Dunn
//
// Created:     22-Dec-1998
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module windows3

%{
#include "helpers.h"
#include <wx/sashwin.h>
#include <wx/laywin.h>
#include <wx/popupwin.h>
#include <wx/tipwin.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import windows2.i
%import mdi.i
%import events.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    static const wxChar* wxSashNameStr = wxT("sashWindow");
    DECLARE_DEF_STRING(SashNameStr);
    static const wxChar* wxSashLayoutNameStr = wxT("layoutWindow");
    DECLARE_DEF_STRING(SashLayoutNameStr);
%}
//---------------------------------------------------------------------------

enum wxSashEdgePosition {
    wxSASH_TOP = 0,
    wxSASH_RIGHT,
    wxSASH_BOTTOM,
    wxSASH_LEFT,
    wxSASH_NONE = 100
};

enum {
    wxEVT_SASH_DRAGGED,
    wxSW_3D,
    wxSW_3DSASH,
    wxSW_3DBORDER,
    wxSW_BORDER
};

enum wxSashDragStatus
{
    wxSASH_STATUS_OK,
    wxSASH_STATUS_OUT_OF_RANGE
};


class wxSashEvent : public wxCommandEvent {
public:
    wxSashEvent(int id = 0, wxSashEdgePosition edge = wxSASH_NONE);

    void SetEdge(wxSashEdgePosition edge);
    wxSashEdgePosition GetEdge();
    void SetDragRect(const wxRect& rect);
    wxRect GetDragRect();
    void SetDragStatus(wxSashDragStatus status);
    wxSashDragStatus GetDragStatus();
};



class wxSashWindow: public wxWindow {
public:
    wxSashWindow(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCLIP_CHILDREN | wxSW_3D,
                 const wxString& name = wxPySashNameStr);
    %name(wxPreSashWindow)wxSashWindow();

    bool Create(wxWindow* parent, wxWindowID id,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = wxCLIP_CHILDREN | wxSW_3D,
                 const wxString& name = wxPySashNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreSashWindow:val._setOORInfo(val)"

    bool GetSashVisible(wxSashEdgePosition edge);
    int GetDefaultBorderSize();
    int GetEdgeMargin(wxSashEdgePosition edge);
    int GetExtraBorderSize();
    int GetMaximumSizeX();
    int GetMaximumSizeY();
    int GetMinimumSizeX();
    int GetMinimumSizeY();
    bool HasBorder(wxSashEdgePosition edge);
    void SetDefaultBorderSize(int width);
    void SetExtraBorderSize(int width);
    void SetMaximumSizeX(int min);
    void SetMaximumSizeY(int min);
    void SetMinimumSizeX(int min);
    void SetMinimumSizeY(int min);
    void SetSashVisible(wxSashEdgePosition edge, bool visible);
    void SetSashBorder(wxSashEdgePosition edge, bool hasBorder);

};


//---------------------------------------------------------------------------

enum wxLayoutOrientation {
    wxLAYOUT_HORIZONTAL,
    wxLAYOUT_VERTICAL
};

enum wxLayoutAlignment {
    wxLAYOUT_NONE,
    wxLAYOUT_TOP,
    wxLAYOUT_LEFT,
    wxLAYOUT_RIGHT,
    wxLAYOUT_BOTTOM,
};


enum {
     wxEVT_QUERY_LAYOUT_INFO,
     wxEVT_CALCULATE_LAYOUT,
};


class wxQueryLayoutInfoEvent: public wxEvent {
public:
    wxQueryLayoutInfoEvent(wxWindowID id = 0);

    void SetRequestedLength(int length);
    int GetRequestedLength();
    void SetFlags(int flags);
    int GetFlags();
    void SetSize(const wxSize& size);
    wxSize GetSize();
    void SetOrientation(wxLayoutOrientation orient);
    wxLayoutOrientation GetOrientation();
    void SetAlignment(wxLayoutAlignment align);
    wxLayoutAlignment GetAlignment();
};



class wxCalculateLayoutEvent: public wxEvent {
public:
    wxCalculateLayoutEvent(wxWindowID id = 0);

    void SetFlags(int flags);
    int GetFlags();
    void SetRect(const wxRect& rect);
    wxRect GetRect();
};


class wxSashLayoutWindow: public wxSashWindow {
public:
    wxSashLayoutWindow(wxWindow* parent, wxWindowID id,
                       const wxPoint& pos = wxDefaultPosition,
                       const wxSize& size = wxDefaultSize,
                       long style = wxCLIP_CHILDREN | wxSW_3D,
                       const wxString& name = wxPySashLayoutNameStr);
    %name(wxPreSashLayoutWindow)wxSashLayoutWindow();

    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxCLIP_CHILDREN | wxSW_3D,
                const wxString& name = wxPySashLayoutNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreSashLayoutWindow:val._setOORInfo(val)"

    wxLayoutAlignment GetAlignment();
    wxLayoutOrientation GetOrientation();
    void SetAlignment(wxLayoutAlignment alignment);
    void SetDefaultSize(const wxSize& size);
    void SetOrientation(wxLayoutOrientation orientation);
};

//---------------------------------------------------------------------------

class wxLayoutAlgorithm : public wxObject {
public:
    wxLayoutAlgorithm();
    ~wxLayoutAlgorithm();

    bool LayoutMDIFrame(wxMDIParentFrame* frame, wxRect* rect = NULL);
    bool LayoutFrame(wxFrame* frame, wxWindow* mainWindow = NULL);
    bool LayoutWindow(wxWindow* parent, wxWindow* mainWindow = NULL);
};


//---------------------------------------------------------------------------
#ifndef __WXMAC__

// wxPopupWindow: a special kind of top level window used for popup menus,
// combobox popups and such.
class wxPopupWindow : public wxWindow {
public:
    wxPopupWindow(wxWindow *parent, int flags = wxBORDER_NONE);
    %name(wxPrePopupWindow)wxPopupWindow();

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPrePopupWindow:val._setOORInfo(val)"

    bool Create(wxWindow *parent, int flags = wxBORDER_NONE);

    // the point must be given in screen coordinates!
    void Position(const wxPoint& ptOrigin,
                  const wxSize& size);
};


%{
class wxPyPopupTransientWindow : public wxPopupTransientWindow
{
public:
    wxPyPopupTransientWindow() : wxPopupTransientWindow() {}
    wxPyPopupTransientWindow(wxWindow* parent, int style = wxBORDER_NONE)
        : wxPopupTransientWindow(parent, style) {}

    DEC_PYCALLBACK_BOOL_ME(ProcessLeftDown);
    DEC_PYCALLBACK__(OnDismiss);
    DEC_PYCALLBACK_BOOL_(CanDismiss);
    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_ME(wxPyPopupTransientWindow, wxPopupTransientWindow, ProcessLeftDown);
IMP_PYCALLBACK__(wxPyPopupTransientWindow, wxPopupTransientWindow, OnDismiss);
IMP_PYCALLBACK_BOOL_(wxPyPopupTransientWindow, wxPopupTransientWindow, CanDismiss);
%}



// wxPopupTransientWindow: a wxPopupWindow which disappears automatically
// when the user clicks mouse outside it or if it loses focus in any other way
%name(wxPopupTransientWindow) class wxPyPopupTransientWindow : public wxPopupWindow
{
public:
    wxPyPopupTransientWindow(wxWindow *parent, int style = wxBORDER_NONE);
    %name(wxPrePopupTransientWindow)wxPyPopupTransientWindow();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPopupTransientWindow)"

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPrePopupTransientWindow:val._setOORInfo(val)"

    // popup the window (this will show it too) and keep focus at winFocus
    // (or itself if it's NULL), dismiss the popup if we lose focus
    virtual void Popup(wxWindow *focus = NULL);

    // hide the window
    virtual void Dismiss();
};

//---------------------------------------------------------------------------

class wxTipWindow : public wxPyPopupTransientWindow
{
public:
    %addmethods {
        wxTipWindow(wxWindow *parent,
                    const wxString* text,
                    wxCoord maxLength = 100,
                    wxRect* rectBound = NULL) {
            wxString tmp = *text;
            return new wxTipWindow(parent, tmp, maxLength, NULL, rectBound);
        }
    }

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    // If rectBound is not NULL, the window will disappear automatically when
    // the mouse leave the specified rect: note that rectBound should be in the
    // screen coordinates!
    void SetBoundingRect(const wxRect& rectBound);

    // Hide and destroy the window
    void Close();
};

#endif // ! __WXMAC__

//---------------------------------------------------------------------------

#ifdef __WXMAC__

%import frames.i

class wxTipWindow : public wxFrame
{
public:
    %addmethods {
        wxTipWindow(wxWindow *parent,
                    const wxString* text,
                    wxCoord maxLength = 100,
                    wxRect* rectBound = NULL) {
            wxString tmp = *text;
            return new wxTipWindow(parent, tmp, maxLength, NULL, rectBound);
        }
    }

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    // If rectBound is not NULL, the window will disappear automatically when
    // the mouse leave the specified rect: note that rectBound should be in the
    // screen coordinates!
    void SetBoundingRect(const wxRect& rectBound);

    // Hide and destroy the window
    void Close();
};

#endif // __WXMAC__

//---------------------------------------------------------------------------
// wxVScrolledWindow

%{
#include <wx/vscroll.h>
DECLARE_DEF_STRING(PanelNameStr);
%}


// First, the C++ version
%{
class wxPyVScrolledWindow  : public wxVScrolledWindow
{
    DECLARE_ABSTRACT_CLASS(wxPyVScrolledWindow);
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
    DEC_PYCALLBACK_COORD_SIZET_constpure(OnGetLineHeight);


    // this function doesn't have to be overridden but it may be useful to do
    // it if calculating the lines heights is a relatively expensive operation
    // as it gives the user code a possibility to calculate several of them at
    // once
    //
    // OnGetLinesHint() is normally called just before OnGetLineHeight() but you
    // shouldn't rely on the latter being called for all lines in the interval
    // specified here. It is also possible that OnGetLineHeight() will be
    // called for the lines outside of this interval, so this is really just a
    // hint, not a promise.
    //
    // finally note that lineMin is inclusive, while lineMax is exclusive, as
    // usual
    DEC_PYCALLBACK_VOID_SIZETSIZET_const(OnGetLinesHint);


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


    // find the index of the line we need to show at the top of the window such
    // that the last (fully or partially) visible line is the given one
    size_t FindFirstFromBottom(size_t lineLast, bool fullyVisible = false)
    { return wxVScrolledWindow::FindFirstFromBottom(lineLast, fullyVisible); }

    // get the total height of the lines between lineMin (inclusive) and
    // lineMax (exclusive)
    wxCoord GetLinesHeight(size_t lineMin, size_t lineMax) const
    { return wxVScrolledWindow::GetLinesHeight(lineMin, lineMax); }


    PYPRIVATE;
};

IMPLEMENT_ABSTRACT_CLASS(wxPyVScrolledWindow, wxVScrolledWindow);

IMP_PYCALLBACK_COORD_SIZET_constpure(wxPyVScrolledWindow, wxVScrolledWindow, OnGetLineHeight);
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

%name(wxVScrolledWindow) class wxPyVScrolledWindow : public wxPanel
{
public:

    wxPyVScrolledWindow(wxWindow *parent,
                        wxWindowID id = wxID_ANY,
                        const wxPoint& pos = wxDefaultPosition,
                        const wxSize& size = wxDefaultSize,
                        long style = 0,
                        const wxString& name = wxPyPanelNameStr);

    %name(wxPreVScrolledWindow)wxPyVScrolledWindow();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxVScrolledWindow)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreVScrolledWindow:val._setOORInfo(val)"

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyPanelNameStr);


    // set the number of lines the window contains: the derived class must
    // provide the heights for all lines with indices up to the one given here
    // in its OnGetLineHeight()
    void SetLineCount(size_t count);

    // scroll to the specified line: it will become the first visible line in
    // the window
    //
    // return true if we scrolled the window, false if nothing was done
    bool ScrollToLine(size_t line);

    // scroll by the specified number of lines/pages
    virtual bool ScrollLines(int lines);
    virtual bool ScrollPages(int pages);

    // redraw the specified line
    void RefreshLine(size_t line);

    // redraw all lines in the specified range (inclusive)
    void RefreshLines(size_t from, size_t to);

    // return the item at the specified (in physical coordinates) position or
    // wxNOT_FOUND if none, i.e. if it is below the last item
    %name(HitTestXT) int HitTest(wxCoord x, wxCoord y) const;
    int HitTest(const wxPoint& pt) const;

    // recalculate all our parameters and redisplay all lines
    virtual void RefreshAll();


    // get the number of lines this window contains (previously set by
    // SetLineCount())
    size_t GetLineCount() const;

    // get the first currently visible line
    size_t GetFirstVisibleLine() const;

    // get the last currently visible line
    size_t GetLastVisibleLine() const;

    // is this line currently visible?
    bool IsVisible(size_t line) const;

};

//---------------------------------------------------------------------------
// wxVListBox

%{
#include <wx/vlbox.h>
DECLARE_DEF_STRING(VListBoxNameStr);
%}


// First, the C++ version
%{
class wxPyVListBox  : public wxVListBox
{
    DECLARE_ABSTRACT_CLASS(wxPyVListBox);
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
    DEC_PYCALLBACK__DCRECTSIZET_constpure(OnDrawSeparator);


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
IMP_PYCALLBACK__DCRECTSIZET_constpure(wxPyVListBox, wxVListBox, OnDrawSeparator);
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
%name(wxVListBox) class wxPyVListBox : public wxPyVScrolledWindow
{
public:

    wxPyVListBox(wxWindow *parent,
                 wxWindowID id = wxID_ANY,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyVListBoxNameStr);

    %name(wxPreVListBox) wxPyVListBox();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxVListBox)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreVListBox:val._setOORInfo(val)"


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

    // get the first selected item, returns wxNOT_FOUND if none
    //
    // cookie is an opaque parameter which should be passed to
    // GetNextSelected() later
    //
    // this method is only valid for the multi selection listboxes
    int GetFirstSelected(unsigned long& cookie) const;

    // get next selection item, return wxNOT_FOUND if no more
    //
    // cookie must be the same parameter that was passed to GetFirstSelected()
    // before
    //
    // this method is only valid for the multi selection listboxes
    int GetNextSelected(unsigned long& cookie) const;

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
    // return true if the items selection status has changed or false
    // otherwise
    //
    // this function is only valid for the multiple selection listboxes
    bool Select(size_t item, bool select = true);

    // selects the items in the specified range whose end points may be given
    // in any order
    //
    // return true if any items selection status has changed, false otherwise
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
    // (true) or if nothing has changed (false)
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
    %name(SetMarginsXY) void SetMargins(wxCoord x, wxCoord y);

    // change the background colour of the selected cells
    void SetSelectionBackground(const wxColour& col);

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
    DECLARE_ABSTRACT_CLASS(wxPyHtmlListBox);
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


    PYPRIVATE;
};


IMPLEMENT_ABSTRACT_CLASS(wxPyHtmlListBox, wxHtmlListBox)

IMP_PYCALLBACK_STRING_SIZET_pure(wxPyHtmlListBox, wxHtmlListBox, OnGetItem);
IMP_PYCALLBACK_STRING_SIZET     (wxPyHtmlListBox, wxHtmlListBox, OnGetItemMarkup);

%}



// Now define this class for SWIG


// wxHtmlListBox is a listbox whose items are wxHtmlCells
%name(wxHtmlListBox) class wxPyHtmlListBox : public wxPyVListBox
{
public:

    // normal constructor which calls Create() internally
    wxPyHtmlListBox(wxWindow *parent,
                    wxWindowID id = wxID_ANY,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = 0,
                    const wxString& name = wxPyVListBoxNameStr);

    %name(wxPreHtmlListBox) wxPyHtmlListBox();

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxHtmlListBox)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreHtmlListBox:val._setOORInfo(val)"

    bool Create(wxWindow *parent,
                wxWindowID id = wxID_ANY,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxString& name = wxPyVListBoxNameStr);


    void RefreshAll();
    void SetItemCount(size_t count);

};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

