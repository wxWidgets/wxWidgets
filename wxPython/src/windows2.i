/////////////////////////////////////////////////////////////////////////////
// Name:        windows2.i
// Purpose:     SWIG definitions of MORE window classes
//
// Author:      Robin Dunn
//
// Created:     6/2/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module windows2

%{
#include "helpers.h"
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/imaglist.h>
#ifdef __WXMSW__
#include <wx/taskbar.h>
#endif
%}

//----------------------------------------------------------------------

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(NOTEBOOK_NAME);
    DECLARE_DEF_STRING(PanelNameStr);
    DECLARE_DEF_STRING(ControlNameStr);

    static const wxChar* wxSplitterNameStr = wxT("splitter");
    DECLARE_DEF_STRING(SplitterNameStr);
    static const wxString wxPyEmptyString(wxT(""));
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import controls.i
%import events.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

enum {
    // notebook control event types
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,

    // styles
    wxNB_FIXEDWIDTH,
    wxNB_TOP,
    wxNB_LEFT,
    wxNB_RIGHT,
    wxNB_BOTTOM,
    wxNB_MULTILINE,

    // hittest flags
    wxNB_HITTEST_NOWHERE = 1,   // not on tab
    wxNB_HITTEST_ONICON  = 2,   // on icon
    wxNB_HITTEST_ONLABEL = 4,   // on label
    wxNB_HITTEST_ONITEM  = wxNB_HITTEST_ONICON | wxNB_HITTEST_ONLABEL,

};


class wxNotebookEvent : public wxNotifyEvent {
public:
    wxNotebookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                    int sel = -1, int oldSel = -1);

    int GetSelection();
    int GetOldSelection();
    void SetOldSelection(int page);
    void SetSelection(int page);
};



class wxNotebook : public wxControl {
public:
    wxNotebook(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyNOTEBOOK_NAME);
    %name(wxPreNotebook)wxNotebook();

    bool Create(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyNOTEBOOK_NAME);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreNotebook:val._setOORInfo(val)"


    // get number of pages in the dialog
    int GetPageCount() const;

    // get the panel which represents the given page
    wxWindow *GetPage(int page);

    // get the currently selected page
     int GetSelection() const;

    // set/get the title of a page
     bool SetPageText(int page, const wxString& text);
     wxString GetPageText(int page) const;

    // image list stuff: each page may have an image associated with it (all
    // images belong to the same image list)
     void SetImageList(wxImageList* imageList);

    // as SetImageList() but the notebook will take ownership of the image list
    void AssignImageList(wxImageList* imageList);
    %pragma(python) addtomethod = "AssignImageList:_args[0].thisown = 0"

    // get pointer (may be NULL) to the associated image list
    wxImageList* GetImageList() const;

    // sets/returns item's image index in the current image list
     int GetPageImage(int page) const;
     bool SetPageImage(int page, int nImage);

    // get the number of rows for a control with wxNB_MULTILINE style (not all
    // versions support it - they will always return 1 then)
     int GetRowCount() const;

    // set the size (the same for all pages)
     void SetPageSize(const wxSize& size);

    // set the padding between tabs (in pixels)
     void SetPadding(const wxSize& padding);

    // set the size of the tabs for wxNB_FIXEDWIDTH controls
     void SetTabSize(const wxSize& sz);

    // calculate the size of the notebook from the size of its page
     wxSize CalcSizeFromPage(const wxSize& sizePage) const;


    // remove one page from the notebook and delete it
     bool DeletePage(int page);

    // remove one page from the notebook, without deleting it
     bool RemovePage(int page);

    // remove all pages and delete them
     bool DeleteAllPages();

    // adds a new page to the notebook (it will be deleted by the notebook,
    // don't delete it yourself) and make it the current one if select
     bool AddPage(wxWindow *page,
                         const wxString& text,
                         bool telect = FALSE,
                  int imageId = -1);

    // the same as AddPage(), but adds the page at the specified position
     bool InsertPage(int index,
                            wxWindow *page,
                            const wxString& text,
                            bool select = FALSE,
                            int imageId = -1);

    // set the currently selected page, return the index of the previously
    // selected one (or -1 on error)
    //
    // NB: this function will _not_ generate wxEVT_NOTEBOOK_PAGE_xxx events
     int SetSelection(int page);

    // hit test, returns which tab is hit and, optionally, where (icon, label)
    // (not implemented on all platforms)
     int HitTest(const wxPoint& pt, long* OUTPUT);

    // cycle thru the tabs
    void AdvanceSelection(bool forward = TRUE);

};

//---------------------------------------------------------------------------


enum {
    /* splitter window events */
    wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING,
    wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
    wxEVT_COMMAND_SPLITTER_UNSPLIT,
    wxEVT_COMMAND_SPLITTER_DOUBLECLICKED,

    wxSPLIT_HORIZONTAL,
    wxSPLIT_VERTICAL,
    wxSPLIT_DRAG_NONE,
    wxSPLIT_DRAG_DRAGGING,
    wxSPLIT_DRAG_LEFT_DOWN,

    wxSP_VERTICAL,
    wxSP_HORIZONTAL,
    wxSP_ARROW_KEYS,
    wxSP_WRAP,
    wxSP_NOBORDER,
    wxSP_3D,
    wxSP_3DSASH,
    wxSP_3DBORDER,
    wxSP_FULLSASH,
    wxSP_BORDER,
    wxSP_LIVE_UPDATE,
    wxSP_PERMIT_UNSPLIT
};


class wxSplitterEvent : public wxNotifyEvent {
public:
     wxSplitterEvent(wxEventType type = wxEVT_NULL,
                     wxSplitterWindow *splitter = NULL);

    int GetSashPosition();
    int GetX();
    int GetY();
    wxWindow* GetWindowBeingRemoved();
    void SetSashPosition(int pos);
}




class wxSplitterWindow : public wxWindow {
public:
    wxSplitterWindow(wxWindow* parent, wxWindowID id,
                     const wxPoint& point = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style=wxSP_3D|wxCLIP_CHILDREN,
                     const wxString& name = wxPySplitterNameStr);
    %name(wxPreSplitterWindow)wxSplitterWindow();

    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style=wxSP_3D|wxCLIP_CHILDREN,
                const wxString& name = wxPySplitterNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreSplitterWindow:val._setOORInfo(val)"

    // Gets the only or left/top pane
    wxWindow *GetWindow1();

    // Gets the right/bottom pane
    wxWindow *GetWindow2();

    // Sets the split mode
    void SetSplitMode(int mode);

    // Gets the split mode
    int GetSplitMode();

    // Initialize with one window
    void Initialize(wxWindow *window);

    // Associates the given window with window 2, drawing the appropriate sash
    // and changing the split mode.
    // Does nothing and returns FALSE if the window is already split.
    // A sashPosition of 0 means choose a default sash position,
    // negative sashPosition specifies the size of right/lower pane as it's
    // absolute value rather than the size of left/upper pane.
    virtual bool SplitVertically(wxWindow *window1,
                                 wxWindow *window2,
                                 int sashPosition = 0);
    virtual bool SplitHorizontally(wxWindow *window1,
                                   wxWindow *window2,
                                   int sashPosition = 0);

    // Removes the specified (or second) window from the view
    // Doesn't actually delete the window.
    bool Unsplit(wxWindow *toRemove = NULL);

    // Replaces one of the windows with another one (neither old nor new
    // parameter should be NULL)
    bool ReplaceWindow(wxWindow *winOld, wxWindow *winNew);

    // Is the window split?
    bool IsSplit();

    // Sets the sash size
    void SetSashSize(int width);

    // Sets the border size
    void SetBorderSize(int width);

    // Gets the sash size
    int GetSashSize();

    // Gets the border size
    int GetBorderSize();

    // Set the sash position
    void SetSashPosition(int position, bool redraw = TRUE);

    // Gets the sash position
    int GetSashPosition();

    // If this is zero, we can remove panes by dragging the sash.
    void SetMinimumPaneSize(int min);
    int GetMinimumPaneSize();

    // Resizes subwindows
    virtual void SizeWindows();

    void SetNeedUpdating(bool needUpdating) { m_needUpdating = needUpdating; }
    bool GetNeedUpdating() const { return m_needUpdating ; }

};

//---------------------------------------------------------------------------

// TODO:  This should be usable on wxGTK now too...
#ifdef __WXMSW__

enum {
    wxEVT_TASKBAR_MOVE,
    wxEVT_TASKBAR_LEFT_DOWN,
    wxEVT_TASKBAR_LEFT_UP,
    wxEVT_TASKBAR_RIGHT_DOWN,
    wxEVT_TASKBAR_RIGHT_UP,
    wxEVT_TASKBAR_LEFT_DCLICK,
    wxEVT_TASKBAR_RIGHT_DCLICK
};


class wxTaskBarIconEvent : public wxEvent
{
public:
    wxTaskBarIconEvent(wxEventType evtType, wxTaskBarIcon *tbIcon);
};


class wxTaskBarIcon : public wxEvtHandler {
public:
    wxTaskBarIcon();
    ~wxTaskBarIcon();

    //%pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    // We still use the magic methods here since that is the way it is documented...
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnMouseMove',    wxEVT_TASKBAR_MOVE)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLButtonDown',  wxEVT_TASKBAR_LEFT_DOWN)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLButtonUp',    wxEVT_TASKBAR_LEFT_UP)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnRButtonDown',  wxEVT_TASKBAR_RIGHT_DOWN)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnRButtonUp',    wxEVT_TASKBAR_RIGHT_UP)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnLButtonDClick',wxEVT_TASKBAR_LEFT_DCLICK)"
    %pragma(python) addtomethod = "__init__:wx._checkForCallback(self, 'OnRButtonDClick',wxEVT_TASKBAR_RIGHT_DCLICK)"

    bool SetIcon(const wxIcon& icon, const wxString& tooltip = wxPyEmptyString);
    bool RemoveIcon(void);
    bool PopupMenu(wxMenu *menu);
    bool IsIconInstalled();
    bool IsOK();
};



%pragma(python) code = "
def EVT_TASKBAR_MOVE(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_MOVE, func)

def EVT_TASKBAR_LEFT_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_LEFT_DOWN, func)

def EVT_TASKBAR_LEFT_UP(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_LEFT_UP, func)

def EVT_TASKBAR_RIGHT_DOWN(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_RIGHT_DOWN, func)

def EVT_TASKBAR_RIGHT_UP(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_RIGHT_UP, func)

def EVT_TASKBAR_LEFT_DCLICK(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_LEFT_DCLICK, func)

def EVT_TASKBAR_RIGHT_DCLICK(win, func):
    win.Connect(-1, -1, wxEVT_TASKBAR_RIGHT_DCLICK, func)
"


#endif

//---------------------------------------------------------------------------
//---------------------------------------------------------------------------
// wxPyWindow derives from wxWindow and adds support for overriding many of
// the virtual methods in Python derived classes.

//      Which (any?) of these should be done also???
//      Destroy
//      DoCaptureMouse
//      DoClientToScreen
//      DoHitTest
//      DoMoveWindow
//      DoPopupMenu
//      DoReleaseMouse
//      DoScreenToClient
//      DoSetToolTip
//      Enable
//      Fit
//      GetCharHeight
//      GetCharWidth
//      GetClientAreaOrigin
//      GetDefaultItem
//      IsTopLevel
//      SetBackgroundColour
//      SetDefaultItem
//      SetFocus
//      SetFocusFromKbd
//      SetForegroundColour
//      SetSizeHints
//      SetVirtualSizeHints
//      Show


%{ // C++ version of Python aware wxWindow
class wxPyWindow : public wxWindow
{
    DECLARE_DYNAMIC_CLASS(wxPyWindow)
public:
    wxPyWindow() : wxWindow() {}
    wxPyWindow(wxWindow* parent, const wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr)
        : wxWindow(parent, id, pos, size, style, name) {}


    DEC_PYCALLBACK_VOID_INT4(DoMoveWindow);
    DEC_PYCALLBACK_VOID_INT5(DoSetSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetClientSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetVirtualSize);

    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetClientSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetPosition);

    DEC_PYCALLBACK_SIZE_const(DoGetVirtualSize);
    DEC_PYCALLBACK_SIZE_const(DoGetBestSize);

    DEC_PYCALLBACK__(InitDialog);
    DEC_PYCALLBACK_BOOL_(TransferDataFromWindow);
    DEC_PYCALLBACK_BOOL_(TransferDataToWindow);
    DEC_PYCALLBACK_BOOL_(Validate);

    DEC_PYCALLBACK_BOOL_const(AcceptsFocus);
    DEC_PYCALLBACK_BOOL_const(AcceptsFocusFromKeyboard);
    DEC_PYCALLBACK_SIZE_const(GetMaxSize);

    DEC_PYCALLBACK_VOID_WXWINBASE(AddChild);
    DEC_PYCALLBACK_VOID_WXWINBASE(RemoveChild);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyWindow, wxWindow);

IMP_PYCALLBACK_VOID_INT4(wxPyWindow, wxWindow, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyWindow, wxWindow, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyWindow, wxWindow, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyWindow, wxWindow, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWindow, wxWindow, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWindow, wxWindow, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyWindow, wxWindow, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyWindow, wxWindow, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyWindow, wxWindow, DoGetBestSize);

IMP_PYCALLBACK__(wxPyWindow, wxWindow, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyWindow, wxWindow, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyWindow, wxWindow, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyWindow, wxWindow, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyWindow, wxWindow, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyWindow, wxWindow, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyWindow, wxWindow, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyWindow, wxWindow, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyWindow, wxWindow, RemoveChild);

%}

// And now the one for SWIG to see
class wxPyWindow : public wxWindow
{
public:
    wxPyWindow(wxWindow* parent, const wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyWindow)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"


    void base_DoMoveWindow(int x, int y, int width, int height);
    void base_DoSetSize(int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO);
    void base_DoSetClientSize(int width, int height);
    void base_DoSetVirtualSize( int x, int y );

    void base_DoGetSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetClientSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetPosition( int *OUTPUT, int *OUTPUT ) const;

    wxSize base_DoGetVirtualSize() const;
    wxSize base_DoGetBestSize() const;

    void base_InitDialog();
    bool base_TransferDataToWindow();
    bool base_TransferDataFromWindow();
    bool base_Validate();

    bool base_AcceptsFocus() const;
    bool base_AcceptsFocusFromKeyboard() const;
    wxSize base_GetMaxSize() const;

    void base_AddChild(wxWindow* child);
    void base_RemoveChild(wxWindow* child);
}

//---------------------------------------------------------------------------
// Do the same thing for wxControl


%{ // C++ version of Python aware wxControl
class wxPyControl : public wxControl
{
    DECLARE_DYNAMIC_CLASS(wxPyControl)
public:
    wxPyControl() : wxControl() {}
    wxPyControl(wxWindow* parent, const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator=wxDefaultValidator,
                const wxString& name = wxPyControlNameStr)
        : wxControl(parent, id, pos, size, style, validator, name) {}


    DEC_PYCALLBACK_VOID_INT4(DoMoveWindow);
    DEC_PYCALLBACK_VOID_INT5(DoSetSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetClientSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetVirtualSize);

    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetClientSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetPosition);

    DEC_PYCALLBACK_SIZE_const(DoGetVirtualSize);
    DEC_PYCALLBACK_SIZE_const(DoGetBestSize);

    DEC_PYCALLBACK__(InitDialog);
    DEC_PYCALLBACK_BOOL_(TransferDataFromWindow);
    DEC_PYCALLBACK_BOOL_(TransferDataToWindow);
    DEC_PYCALLBACK_BOOL_(Validate);

    DEC_PYCALLBACK_BOOL_const(AcceptsFocus);
    DEC_PYCALLBACK_BOOL_const(AcceptsFocusFromKeyboard);
    DEC_PYCALLBACK_SIZE_const(GetMaxSize);

    DEC_PYCALLBACK_VOID_WXWINBASE(AddChild);
    DEC_PYCALLBACK_VOID_WXWINBASE(RemoveChild);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyControl, wxControl);

IMP_PYCALLBACK_VOID_INT4(wxPyControl, wxControl, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyControl, wxControl, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyControl, wxControl, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyControl, wxControl, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyControl, wxControl, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyControl, wxControl, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyControl, wxControl, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyControl, wxControl, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyControl, wxControl, DoGetBestSize);

IMP_PYCALLBACK__(wxPyControl, wxControl, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyControl, wxControl, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyControl, wxControl, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyControl, wxControl, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyControl, wxControl, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyControl, wxControl, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyControl, wxControl, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyControl, wxControl, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyControl, wxControl, RemoveChild);

%}

// And now the one for SWIG to see
class wxPyControl : public wxControl
{
public:
    wxPyControl(wxWindow* parent, const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                const wxValidator& validator=wxDefaultValidator,
                const wxString& name = wxPyControlNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyControl)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"


    void base_DoMoveWindow(int x, int y, int width, int height);
    void base_DoSetSize(int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO);
    void base_DoSetClientSize(int width, int height);
    void base_DoSetVirtualSize( int x, int y );

    void base_DoGetSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetClientSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetPosition( int *OUTPUT, int *OUTPUT ) const;

    wxSize base_DoGetVirtualSize() const;
    wxSize base_DoGetBestSize() const;

    void base_InitDialog();
    bool base_TransferDataToWindow();
    bool base_TransferDataFromWindow();
    bool base_Validate();

    bool base_AcceptsFocus() const;
    bool base_AcceptsFocusFromKeyboard() const;
    wxSize base_GetMaxSize() const;

    void base_AddChild(wxWindow* child);
    void base_RemoveChild(wxWindow* child);
}

//---------------------------------------------------------------------------
// and for wxPanel

%{ // C++ version of Python aware wxPanel
class wxPyPanel : public wxPanel
{
    DECLARE_DYNAMIC_CLASS(wxPyPanel)
public:
    wxPyPanel() : wxPanel() {}
    wxPyPanel(wxWindow* parent, const wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr)
        : wxPanel(parent, id, pos, size, style, name) {}


    DEC_PYCALLBACK_VOID_INT4(DoMoveWindow);
    DEC_PYCALLBACK_VOID_INT5(DoSetSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetClientSize);
    DEC_PYCALLBACK_VOID_INTINT(DoSetVirtualSize);

    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetClientSize);
    DEC_PYCALLBACK_VOID_INTPINTP_const(DoGetPosition);

    DEC_PYCALLBACK_SIZE_const(DoGetVirtualSize);
    DEC_PYCALLBACK_SIZE_const(DoGetBestSize);

    DEC_PYCALLBACK__(InitDialog);
    DEC_PYCALLBACK_BOOL_(TransferDataFromWindow);
    DEC_PYCALLBACK_BOOL_(TransferDataToWindow);
    DEC_PYCALLBACK_BOOL_(Validate);

    DEC_PYCALLBACK_BOOL_const(AcceptsFocus);
    DEC_PYCALLBACK_BOOL_const(AcceptsFocusFromKeyboard);
    DEC_PYCALLBACK_SIZE_const(GetMaxSize);

    DEC_PYCALLBACK_VOID_WXWINBASE(AddChild);
    DEC_PYCALLBACK_VOID_WXWINBASE(RemoveChild);

    PYPRIVATE;
};

IMPLEMENT_DYNAMIC_CLASS(wxPyPanel, wxPanel);

IMP_PYCALLBACK_VOID_INT4(wxPyPanel, wxPanel, DoMoveWindow);
IMP_PYCALLBACK_VOID_INT5(wxPyPanel, wxPanel, DoSetSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyPanel, wxPanel, DoSetClientSize);
IMP_PYCALLBACK_VOID_INTINT(wxPyPanel, wxPanel, DoSetVirtualSize);

IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyPanel, wxPanel, DoGetSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyPanel, wxPanel, DoGetClientSize);
IMP_PYCALLBACK_VOID_INTPINTP_const(wxPyPanel, wxPanel, DoGetPosition);

IMP_PYCALLBACK_SIZE_const(wxPyPanel, wxPanel, DoGetVirtualSize);
IMP_PYCALLBACK_SIZE_const(wxPyPanel, wxPanel, DoGetBestSize);

IMP_PYCALLBACK__(wxPyPanel, wxPanel, InitDialog);
IMP_PYCALLBACK_BOOL_(wxPyPanel, wxPanel, TransferDataFromWindow);
IMP_PYCALLBACK_BOOL_(wxPyPanel, wxPanel, TransferDataToWindow);
IMP_PYCALLBACK_BOOL_(wxPyPanel, wxPanel, Validate);

IMP_PYCALLBACK_BOOL_const(wxPyPanel, wxPanel, AcceptsFocus);
IMP_PYCALLBACK_BOOL_const(wxPyPanel, wxPanel, AcceptsFocusFromKeyboard);
IMP_PYCALLBACK_SIZE_const(wxPyPanel, wxPanel, GetMaxSize);

IMP_PYCALLBACK_VOID_WXWINBASE(wxPyPanel, wxPanel, AddChild);
IMP_PYCALLBACK_VOID_WXWINBASE(wxPyPanel, wxPanel, RemoveChild);

%}

// And now the one for SWIG to see
class wxPyPanel : public wxPanel
{
public:
    wxPyPanel(wxWindow* parent, const wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               const wxString& name = wxPyPanelNameStr);

    void _setCallbackInfo(PyObject* self, PyObject* _class);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyPanel)"
    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"


    void base_DoMoveWindow(int x, int y, int width, int height);
    void base_DoSetSize(int x, int y, int width, int height,
                        int sizeFlags = wxSIZE_AUTO);
    void base_DoSetClientSize(int width, int height);
    void base_DoSetVirtualSize( int x, int y );

    void base_DoGetSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetClientSize( int *OUTPUT, int *OUTPUT ) const;
    void base_DoGetPosition( int *OUTPUT, int *OUTPUT ) const;

    wxSize base_DoGetVirtualSize() const;
    wxSize base_DoGetBestSize() const;

    void base_InitDialog();
    bool base_TransferDataToWindow();
    bool base_TransferDataFromWindow();
    bool base_Validate();

    bool base_AcceptsFocus() const;
    bool base_AcceptsFocusFromKeyboard() const;
    wxSize base_GetMaxSize() const;

    void base_AddChild(wxWindow* child);
    void base_RemoveChild(wxWindow* child);
};


//---------------------------------------------------------------------------
