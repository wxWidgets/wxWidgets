/////////////////////////////////////////////////////////////////////////////
// Name:        gizmos.i
// Purpose:     Wrappers for the "gizmo" classes in wx/contrib
//
// Author:      Robin Dunn
//
// Created:     23-Nov-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%define DOCSTRING
"Various *gizmo* classes: `DynamicSashWindow`, `EditableListBox`,
`LEDNumberCtrl`, `TreeListCtrl`, etc."
%enddef

%module(package="wx", docstring=DOCSTRING) gizmos


%{
#include "wx/wxPython/wxPython.h"
#include "wx/wxPython/pyclasses.h"

#include <wx/gizmos/dynamicsash.h>
#include <wx/gizmos/editlbox.h>
#include <wx/gizmos/splittree.h>
#include <wx/gizmos/ledctrl.h>
#include <wx/gizmos/statpict.h>

#include <wx/listctrl.h>
#include <wx/treectrl.h>
#include <wx/imaglist.h>

%}

//---------------------------------------------------------------------------

%import windows.i
%import controls.i
%pythoncode { import wx }
%pythoncode { __docfilter__ = wx._core.__DocFilter(globals()) }


MAKE_CONST_WXSTRING2(DynamicSashNameStr,     wxT("dynamicSashWindow"));
MAKE_CONST_WXSTRING2(EditableListBoxNameStr, wxT("editableListBox"));
MAKE_CONST_WXSTRING(StaticPictureNameStr);                     
    
MAKE_CONST_WXSTRING_NOSWIG(EmptyString);

//---------------------------------------------------------------------------

enum {
    wxDS_MANAGE_SCROLLBARS,
    wxDS_DRAG_CORNER,
};

%constant wxEventType wxEVT_DYNAMIC_SASH_SPLIT;
%constant wxEventType wxEVT_DYNAMIC_SASH_UNIFY;


/*
    wxDynamicSashSplitEvents are sent to your view by wxDynamicSashWindow
    whenever your view is being split by the user.  It is your
    responsibility to handle this event by creating a new view window as
    a child of the wxDynamicSashWindow.  wxDynamicSashWindow will
    automatically reparent it to the proper place in its window hierarchy.
*/
class wxDynamicSashSplitEvent : public wxCommandEvent {
public:
    wxDynamicSashSplitEvent(wxObject *target);
};


/*
    wxDynamicSashUnifyEvents are sent to your view by wxDynamicSashWindow
    whenever the sash which splits your view and its sibling is being
    reunified such that your view is expanding to replace its sibling.
    You needn't do anything with this event if you are allowing
    wxDynamicSashWindow to manage your view's scrollbars, but it is useful
    if you are managing the scrollbars yourself so that you can keep
    the scrollbars' event handlers connected to your view's event handler
    class.
*/
class wxDynamicSashUnifyEvent : public wxCommandEvent {
public:
    wxDynamicSashUnifyEvent(wxObject *target);
};



/*

    wxDynamicSashWindow

    wxDynamicSashWindow widgets manages the way other widgets are viewed.
    When a wxDynamicSashWindow is first shown, it will contain one child
    view, a viewport for that child, and a pair of scrollbars to allow the
    user to navigate the child view area.  Next to each scrollbar is a small
    tab.  By clicking on either tab and dragging to the appropriate spot, a
    user can split the view area into two smaller views separated by a
    draggable sash.  Later, when the user wishes to reunify the two subviews,
    the user simply drags the sash to the side of the window.
    wxDynamicSashWindow will automatically reparent the appropriate child
    view back up the window hierarchy, and the wxDynamicSashWindow will have
    only one child view once again.

    As an application developer, you will simply create a wxDynamicSashWindow
    using either the Create() function or the more complex constructor
    provided below, and then create a view window whose parent is the
    wxDynamicSashWindow.  The child should respond to
    wxDynamicSashSplitEvents -- perhaps with an OnSplit() event handler -- by
    constructing a new view window whose parent is also the
    wxDynamicSashWindow.  That's it!  Now your users can dynamically split
    and reunify the view you provided.

    If you wish to handle the scrollbar events for your view, rather than
    allowing wxDynamicSashWindow to do it for you, things are a bit more
    complex.  (You might want to handle scrollbar events yourself, if,
    for instance, you wish to scroll a subwindow of the view you add to
    your wxDynamicSashWindow object, rather than scrolling the whole view.)
    In this case, you will need to construct your wxDynamicSashWindow without
    the wxDS_MANAGE_SCROLLBARS style and  you will need to use the
    GetHScrollBar() and GetVScrollBar() methods to retrieve the scrollbar
    controls and call SetEventHanler() on them to redirect the scrolling
    events whenever your window is reparented by wxDyanmicSashWindow.
    You will need to set the scrollbars' event handler at three times:

        *  When your view is created
        *  When your view receives a wxDynamicSashSplitEvent
        *  When your view receives a wxDynamicSashUnifyEvent

    See the dynsash_switch sample application for an example which does this.

*/

MustHaveApp(wxDynamicSashWindow);

class wxDynamicSashWindow : public wxWindow {
public:
    %pythonAppend wxDynamicSashWindow         "self._setOORInfo(self)"
    %pythonAppend wxDynamicSashWindow()       ""

    wxDynamicSashWindow(wxWindow *parent, wxWindowID id=-1,
                        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                        long style = wxCLIP_CHILDREN | wxDS_MANAGE_SCROLLBARS | wxDS_DRAG_CORNER,
                        const wxString& name = wxPyDynamicSashNameStr);
    %RenameCtor(PreDynamicSashWindow, wxDynamicSashWindow());

    bool Create(wxWindow *parent, wxWindowID id=-1,
                const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize,
                long style = wxCLIP_CHILDREN | wxDS_MANAGE_SCROLLBARS | wxDS_DRAG_CORNER,
                const wxString& name = wxPyDynamicSashNameStr);

    wxScrollBar *GetHScrollBar(const wxWindow *child) const;
    wxScrollBar *GetVScrollBar(const wxWindow *child) const;
};



%pythoncode {
EVT_DYNAMIC_SASH_SPLIT = wx.PyEventBinder( wxEVT_DYNAMIC_SASH_SPLIT, 1 )
EVT_DYNAMIC_SASH_UNIFY = wx.PyEventBinder( wxEVT_DYNAMIC_SASH_UNIFY, 1 )
}

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------

enum {
    wxEL_ALLOW_NEW,
    wxEL_ALLOW_EDIT,
    wxEL_ALLOW_DELETE,
};

// This class provides a composite control that lets the
// user easily enter list of strings
MustHaveApp(wxEditableListBox);
class wxEditableListBox : public wxPanel
{
public:
    %pythonAppend wxEditableListBox         "self._setOORInfo(self)"
    %pythonAppend wxEditableListBox()       ""

    wxEditableListBox(wxWindow *parent, wxWindowID id=-1,
                      const wxString& label = wxPyEmptyString,
                      const wxPoint& pos = wxDefaultPosition,
                      const wxSize& size = wxDefaultSize,
                      long style = wxEL_ALLOW_NEW | wxEL_ALLOW_EDIT | wxEL_ALLOW_DELETE,
                      const wxString& name = wxPyEditableListBoxNameStr);


    void SetStrings(const wxArrayString& strings);

    //void GetStrings(wxArrayString& strings);
    %extend {
        PyObject* GetStrings() {
            wxArrayString strings;
            self->GetStrings(strings);
            return wxArrayString2PyList_helper(strings);
        }
    }

    wxPyListCtrl* GetListCtrl();
    wxBitmapButton* GetDelButton();
    wxBitmapButton* GetNewButton();
    wxBitmapButton* GetUpButton();
    wxBitmapButton* GetDownButton();
    wxBitmapButton* GetEditButton();
};



//---------------------------------------------------------------------------


/*
 * wxRemotelyScrolledTreeCtrl
 *
 * This tree control disables its vertical scrollbar and catches scroll
 * events passed by a scrolled window higher in the hierarchy.
 * It also updates the scrolled window vertical scrollbar as appropriate.
 */

%{
    typedef wxTreeCtrl wxPyTreeCtrl;
%}

MustHaveApp(wxRemotelyScrolledTreeCtrl);

class wxRemotelyScrolledTreeCtrl: public wxPyTreeCtrl
{
public:
    %pythonAppend wxRemotelyScrolledTreeCtrl         "self._setOORInfo(self)"
    %pythonAppend wxRemotelyScrolledTreeCtrl()       ""

    wxRemotelyScrolledTreeCtrl(wxWindow* parent, wxWindowID id,
                               const wxPoint& pos = wxDefaultPosition,
                               const wxSize& size = wxDefaultSize,
                               long style = wxTR_HAS_BUTTONS);


    void HideVScrollbar();

    // Adjust the containing wxScrolledWindow's scrollbars appropriately
    void AdjustRemoteScrollbars();

    // Find the scrolled window that contains this control
    wxScrolledWindow* GetScrolledWindow() const;

    // Scroll to the given line (in scroll units where each unit is
    // the height of an item)
    void ScrollToLine(int posHoriz, int posVert);

    // The companion window is one which will get notified when certain
    // events happen such as node expansion
    void SetCompanionWindow(wxWindow* companion);
    wxWindow* GetCompanionWindow() const;
};



/*
 * wxTreeCompanionWindow
 *
 * A window displaying values associated with tree control items.
 */

%{
class wxPyTreeCompanionWindow: public wxTreeCompanionWindow
{
public:
    wxPyTreeCompanionWindow(wxWindow* parent, wxWindowID id = -1,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0)
        : wxTreeCompanionWindow(parent, id, pos, size, style) {}


    virtual void DrawItem(wxDC& dc, wxTreeItemId id, const wxRect& rect) {
        bool found;
        wxPyBlock_t blocked = wxPyBeginBlockThreads();
        if ((found = wxPyCBH_findCallback(m_myInst, "DrawItem"))) {
            PyObject* dcobj = wxPyMake_wxObject(&dc,false);
            PyObject* idobj = wxPyConstructObject((void*)&id, wxT("wxTreeItemId"), false);
            PyObject* recobj= wxPyConstructObject((void*)&rect, wxT("wxRect"), false);
            wxPyCBH_callCallback(m_myInst, Py_BuildValue("(OOO)", dcobj, idobj, recobj));
            Py_DECREF(dcobj);
            Py_DECREF(idobj);
            Py_DECREF(recobj);
        }
        wxPyEndBlockThreads(blocked);
        if (! found)
            wxTreeCompanionWindow::DrawItem(dc, id, rect);
    }

    PYPRIVATE;
};
%}


MustHaveApp(wxPyTreeCompanionWindow);

%rename(TreeCompanionWindow) wxPyTreeCompanionWindow;
class wxPyTreeCompanionWindow: public wxWindow
{
public:
    %pythonAppend wxPyTreeCompanionWindow         "self._setOORInfo(self);self._setCallbackInfo(self, TreeCompanionWindow)"
    %pythonAppend wxPyTreeCompanionWindow()       ""

    wxPyTreeCompanionWindow(wxWindow* parent, wxWindowID id = -1,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = 0);
    void _setCallbackInfo(PyObject* self, PyObject* _class);

    wxRemotelyScrolledTreeCtrl* GetTreeCtrl() const;
    void SetTreeCtrl(wxRemotelyScrolledTreeCtrl* treeCtrl);
};



/*
 * wxThinSplitterWindow
 *
 * Implements a splitter with a less obvious sash
 * than the usual one.
 */

MustHaveApp(wxThinSplitterWindow);

class wxThinSplitterWindow: public wxSplitterWindow
{
public:
    %pythonAppend wxThinSplitterWindow         "self._setOORInfo(self)"
    %pythonAppend wxThinSplitterWindow()       ""

    wxThinSplitterWindow(wxWindow* parent, wxWindowID id = -1,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxSP_3D | wxCLIP_CHILDREN);
};



/*
 * wxSplitterScrolledWindow
 *
 * This scrolled window is aware of the fact that one of its
 * children is a splitter window. It passes on its scroll events
 * (after some processing) to both splitter children for them
 * scroll appropriately.
 */

MustHaveApp(wxSplitterScrolledWindow);

class wxSplitterScrolledWindow: public wxScrolledWindow
{
public:
    %pythonAppend wxSplitterScrolledWindow         "self._setOORInfo(self)"
    %pythonAppend wxSplitterScrolledWindow()       ""

    wxSplitterScrolledWindow(wxWindow* parent, wxWindowID id = -1,
                             const wxPoint& pos = wxDefaultPosition,
                             const wxSize& size = wxDefaultSize,
                             long style = 0);
};


//---------------------------------------------------------------------------
//---------------------------------------------------------------------------


enum wxLEDValueAlign
{
    wxLED_ALIGN_LEFT,
    wxLED_ALIGN_RIGHT,
    wxLED_ALIGN_CENTER,

    wxLED_ALIGN_MASK,

    wxLED_DRAW_FADED,
};


MustHaveApp(wxLEDNumberCtrl);

class wxLEDNumberCtrl :	public wxControl
{
public:
    %pythonAppend wxLEDNumberCtrl         "self._setOORInfo(self)"
    %pythonAppend wxLEDNumberCtrl()       ""

    wxLEDNumberCtrl(wxWindow *parent, wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style =  wxLED_ALIGN_LEFT | wxLED_DRAW_FADED);
    %RenameCtor(PreLEDNumberCtrl,  wxLEDNumberCtrl());

    bool Create(wxWindow *parent, wxWindowID id = -1,
                    const wxPoint& pos = wxDefaultPosition,
                    const wxSize& size = wxDefaultSize,
                    long style = wxLED_ALIGN_LEFT | wxLED_DRAW_FADED);

    wxLEDValueAlign GetAlignment() const;
    bool GetDrawFaded() const;
    const wxString &GetValue() const;

    void SetAlignment(wxLEDValueAlign Alignment, bool Redraw = true);
    void SetDrawFaded(bool DrawFaded, bool Redraw = true);
    void SetValue(const wxString &Value, bool Redraw = true);

};



//----------------------------------------------------------------------

%include _treelist.i

//----------------------------------------------------------------------

enum
{
    wxSCALE_HORIZONTAL,
    wxSCALE_VERTICAL,
    wxSCALE_UNIFORM,
    wxSCALE_CUSTOM    
};

MustHaveApp(wxStaticPicture);

class wxStaticPicture : public wxControl
{
public:
    %pythonAppend wxStaticPicture         "self._setOORInfo(self)"
    %pythonAppend wxStaticPicture()       ""

    wxStaticPicture( wxWindow* parent, wxWindowID id=-1,
                     const wxBitmap& label=wxNullBitmap,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = 0,
                     const wxString& name = wxPyStaticPictureNameStr );

    %RenameCtor(PreStaticPicture, wxStaticPicture());

    bool Create( wxWindow* parent, wxWindowID id=-1,
                 const wxBitmap& label=wxNullBitmap,
                 const wxPoint& pos = wxDefaultPosition,
                 const wxSize& size = wxDefaultSize,
                 long style = 0,
                 const wxString& name = wxPyStaticPictureNameStr );

    void SetBitmap( const wxBitmap& bmp );
    wxBitmap GetBitmap() const;
    void SetIcon( const wxIcon& icon );
    wxIcon GetIcon() const;

    void SetAlignment( int align );
    int GetAlignment() const;

    void SetScale( int scale );
    int GetScale() const; 

    void SetCustomScale( float sx, float sy );
    void GetCustomScale( float* OUTPUT, float* OUTPUT ) const;

};


//----------------------------------------------------------------------
//----------------------------------------------------------------------

%init %{
    wxPyPtrTypeMap_Add("wxTreeCompanionWindow", "wxPyTreeCompanionWindow");
%}

//----------------------------------------------------------------------
//----------------------------------------------------------------------

