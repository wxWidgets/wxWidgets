/////////////////////////////////////////////////////////////////////////////
// Name:        windows.i
// Purpose:     SWIG definitions of various window classes
//
// Author:      Robin Dunn
//
// Created:     6/24/97
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module windows

%{
#include "helpers.h"
#include <wx/menuitem.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import clip_dnd.i

%pragma(python) code = "import wx"

//---------------------------------------------------------------------------

class wxEvtHandler {
public:
    bool ProcessEvent(wxEvent& event);
    void AddPendingEvent(wxEvent& event);
    //bool SearchEventTable(wxEventTable& table, wxEvent& event);

    bool GetEvtHandlerEnabled();
    void SetEvtHandlerEnabled(bool enabled);

    wxEvtHandler* GetNextHandler();
    wxEvtHandler* GetPreviousHandler();
    void SetNextHandler(wxEvtHandler* handler);
    void SetPreviousHandler(wxEvtHandler* handler);


    %addmethods {
        void Connect( int id, int lastId, int eventType, PyObject* func) {
            if (PyCallable_Check(func)) {
                self->Connect(id, lastId, eventType,
                          (wxObjectEventFunction) &wxPyCallback::EventThunker,
                          new wxPyCallback(func));
            }
        }

        bool Disconnect(int id, int lastId = -1,
                        wxEventType eventType = wxEVT_NULL) {
            return self->Disconnect(id, lastId, eventType,
                                   (wxObjectEventFunction)
                                    &wxPyCallback::EventThunker);
        }

    }

    %pragma(python) addtoclass = "
    _prop_list_ = {}
    "

//      %pragma(python) addtoclass = "
//      def __getattr__(self, name):
//          pl = self._prop_list_
//          if pl.has_key(name):
//              getFunc, setFunc = pl[name]
//              if getFunc:
//                  return getattr(self, getFunc)()
//              else:
//                  raise TypeError, '%s property is write-only' % name
//          raise AttributeError, name

//      def __setattr__(self, name, value):
//          pl = self._prop_list_
//          if pl.has_key(name):
//              getFunc, setFunc = pl[name]
//              if setFunc:
//                  return getattr(self, setFunc)(value)
//              else:
//                  raise TypeError, '%s property is read-only' % name
//          self.__dict__[name] = value
//      "
};


//----------------------------------------------------------------------

class wxValidator : public wxEvtHandler {
public:
    wxValidator();
    //~wxValidator();

    wxValidator* Clone();
    wxWindow* GetWindow();
    void SetWindow(wxWindow* window);

    // Properties list
    %pragma(python) addtoclass = "
    _prop_list_ = {
        'window' : ('GetWindow', 'SetWindow'),
    }
    _prop_list_.update(wxEvtHandler._prop_list_)
    "
};

%inline %{
    bool wxValidator_IsSilent() {
        return wxValidator::IsSilent();
    }

    void wxValidator_SetBellOnError(int doIt = TRUE) {
        wxValidator::SetBellOnError(doIt);
    }
%}

//----------------------------------------------------------------------
%{
class wxPyValidator : public wxValidator {
    DECLARE_DYNAMIC_CLASS(wxPyValidator);
public:
    wxPyValidator() {
    }
//    wxPyValidator(const wxPyValidator& other);

    ~wxPyValidator() {
    }

    wxObject* wxPyValidator::Clone() const {
        wxPyValidator* ptr = NULL;
        wxPyValidator* self = (wxPyValidator*)this;

        bool doSave = wxPyRestoreThread();
        if (self->m_myInst.findCallback("Clone")) {
            PyObject* ro;
            ro = self->m_myInst.callCallbackObj(Py_BuildValue("()"));
            if (ro) {
                SWIG_GetPtrObj(ro, (void **)&ptr, "_wxPyValidator_p");
                Py_DECREF(ro);
            }
        }
        // This is very dangerous!!! But is the only way I could find
        // to squash a memory leak.  Currently it is okay, but if the
        // validator architecture in wxWindows ever changes, problems
        // could arise.
        delete self;

        wxPySaveThread(doSave);
        return ptr;
    }

    DEC_PYCALLBACK_BOOL_WXWIN(Validate);
    DEC_PYCALLBACK_BOOL_(TransferToWindow);
    DEC_PYCALLBACK_BOOL_(TransferFromWindow);

    PYPRIVATE;
//    PyObject*   m_data;
};

IMP_PYCALLBACK_BOOL_WXWIN(wxPyValidator, wxValidator, Validate);
IMP_PYCALLBACK_BOOL_(wxPyValidator, wxValidator, TransferToWindow);
IMP_PYCALLBACK_BOOL_(wxPyValidator, wxValidator, TransferFromWindow);

IMPLEMENT_DYNAMIC_CLASS(wxPyValidator, wxValidator);

%}

class wxPyValidator : public wxValidator {
public:
    wxPyValidator();
//    ~wxPyValidator();

    %addmethods { void Destroy() { delete self; } }

    void _setSelf(PyObject* self, PyObject* _class, int incref=TRUE);
    %pragma(python) addtomethod = "__init__:self._setSelf(self, wxPyValidator, 0)"

};

//----------------------------------------------------------------------

%apply int * INOUT { int* x, int* y };

class wxWindow : public wxEvtHandler {
public:

    wxWindow(wxWindow* parent, const wxWindowID id,
             const wxPoint& pos = wxPyDefaultPosition,
             const wxSize& size = wxPyDefaultSize,
             long style = 0,
             char* name = "panel");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void CaptureMouse();
    void Center(int direction = wxBOTH);
    void Centre(int direction = wxBOTH);
    void CentreOnParent(int direction = wxBOTH );
    void CenterOnParent(int direction = wxBOTH );

    // (uses apply'ed INOUT typemap, see above)
    %name(ClientToScreenXY)void ClientToScreen(int* x, int* y);
    wxPoint ClientToScreen(const wxPoint& pt);

    bool Close(int force = FALSE);
    bool Destroy();
    void DestroyChildren();
#ifdef __WXMSW__
    void DragAcceptFiles(bool accept);
#endif
    void Enable(bool enable);
    //bool FakePopupMenu(wxMenu* menu, int x, int y);
    %name(FindWindowById) wxWindow* FindWindow(long id);
    %name(FindWindowByName) wxWindow* FindWindow(const wxString& name);
    void Fit();
    wxColour GetBackgroundColour();

    //wxList& GetChildren();
    %addmethods {
        PyObject* GetChildren() {
            wxWindowList& list = self->GetChildren();
            return wxPy_ConvertList(&list, "wxWindow");
        }
    }

    int  GetCharHeight();
    int  GetCharWidth();
    %name(GetClientSizeTuple) void GetClientSize(int *OUTPUT, int *OUTPUT);
    wxSize GetClientSize();
    wxLayoutConstraints * GetConstraints();
    wxEvtHandler* GetEventHandler();

    wxFont& GetFont();
    wxColour GetForegroundColour();
    wxWindow * GetGrandParent();
    %addmethods {
        long GetHandle() {
            return wxPyGetWinHandle(self); //(long)self->GetHandle();
        }
    }
    int GetId();
    wxString GetLabel();
    void SetLabel(const wxString& label);
    wxString GetName();
    wxWindow * GetParent();
    %name(GetPositionTuple) void GetPosition(int *OUTPUT, int *OUTPUT);
    wxPoint GetPosition();
    wxRect GetRect();
    int GetScrollThumb(int orientation);
    int GetScrollPos(int orientation);
    int GetScrollRange(int orientation);
    %name(GetSizeTuple) void GetSize(int *OUTPUT, int *OUTPUT);
    wxSize GetSize();
    void GetTextExtent(const wxString& string, int *OUTPUT, int *OUTPUT);
    %name(GetFullTextExtent)void GetTextExtent(const wxString& string,
                       int *OUTPUT, int *OUTPUT, int *OUTPUT, int* OUTPUT,
                       const wxFont* font = NULL); //, bool use16 = FALSE)
    wxString GetTitle();
    wxRegion GetUpdateRegion();
    long GetWindowStyleFlag();
    void SetWindowStyleFlag(long style);
    void SetWindowStyle(long style);
    bool Hide();
    void InitDialog();
    bool IsEnabled();
    bool IsExposed( int x, int y, int w=0, int h=0 );
    %name(IsExposedPoint) bool IsExposed( const wxPoint& pt );
    %name(IsExposedRect)  bool IsExposed( const wxRect& rect );
    bool IsRetained();
    bool IsShown();
    bool IsTopLevel();
    void Layout();
    bool LoadFromResource(wxWindow* parent, const wxString& resourceName, const wxResourceTable* resourceTable = NULL);
    void Lower();
    void MakeModal(bool flag);
    %name(MoveXY)void Move(int x, int y);
    void Move(const wxPoint& point);

    wxEvtHandler* PopEventHandler(bool deleteHandler = FALSE);
    void PushEventHandler(wxEvtHandler* handler);

    %name(PopupMenuXY)bool PopupMenu(wxMenu *menu, int x, int y);
    bool PopupMenu(wxMenu *menu, const wxPoint& pos);

    void Raise();
    void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);
    void ReleaseMouse();
    void RemoveChild(wxWindow* child);
    bool Reparent( wxWindow* newParent );

    // (uses apply'ed INOUT typemap, see above)
    %name(ScreenToClientXY)void ScreenToClient(int *x, int *y);
    wxPoint ScreenToClient(const wxPoint& pt);

    void ScrollWindow(int dx, int dy, const wxRect* rect = NULL);
    void SetAcceleratorTable(const wxAcceleratorTable& accel);
    void SetAutoLayout(bool autoLayout);
    bool GetAutoLayout();
    void SetBackgroundColour(const wxColour& colour);
    void SetConstraints(wxLayoutConstraints *constraints);
    void UnsetConstraints(wxLayoutConstraints *constraints);
    void SetFocus();
    bool AcceptsFocus();
    void SetFont(const wxFont& font);
    void SetForegroundColour(const wxColour& colour);
    void SetId(int id);
    void SetName(const wxString& name);
    void SetScrollbar(int orientation, int position, int thumbSize, int range, int refresh = TRUE);
    void SetScrollPos(int orientation, int pos, bool refresh = TRUE);

    %name(SetDimensions) void SetSize(int x, int y, int width, int height, int sizeFlags=wxSIZE_AUTO);
    %addmethods {
        void SetSize(const wxSize& size) {
            self->SetSize(size);
        }

        void SetPosition(const wxPoint& pos) {
            self->Move(pos);
        }
    }

    void SetSizeHints(int minW=-1, int minH=-1, int maxW=-1, int maxH=-1, int incW=-1, int incH=-1);
    %name(SetClientSizeWH)void SetClientSize(int width, int height);
    void SetClientSize(const wxSize& size);
    //void SetPalette(wxPalette* palette);
    void SetCursor(const wxCursor&cursor);
    void SetEventHandler(wxEvtHandler* handler);
    void SetTitle(const wxString& title);
    bool Show(bool show);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
    bool Validate();
    void WarpPointer(int x, int y);

    %name(ConvertDialogPointToPixels) wxPoint ConvertDialogToPixels(const wxPoint& pt);
    %name(ConvertDialogSizeToPixels)  wxSize  ConvertDialogToPixels(const wxSize& sz);

    %name(DLG_PNT) wxPoint ConvertDialogToPixels(const wxPoint& pt);
    %name(DLG_SZE) wxSize  ConvertDialogToPixels(const wxSize& sz);

    %name(ConvertPixelPointToDialog) wxPoint ConvertPixelsToDialog(const wxPoint& pt);
    %name(ConvertPixelSizeToDialog)  wxSize  ConvertPixelsToDialog(const wxSize& sz);

    %name(SetToolTipString)void SetToolTip(const wxString &tip);
    void SetToolTip(wxToolTip *tooltip);
    wxToolTip* GetToolTip();

    void SetSizer(wxSizer* sizer);
    wxSizer* GetSizer();

    wxValidator* GetValidator();
    void SetValidator(const wxValidator& validator);

    void SetDropTarget(wxDropTarget* target);
    wxDropTarget* GetDropTarget();
    %pragma(python) addtomethod = "SetDropTarget:_args[0].thisown = 0"

    wxSize GetBestSize();

    void SetCaret(wxCaret *caret);
    wxCaret *GetCaret();
    %pragma(python) addtoclass = "# replaces broken shadow method
    def GetCaret(self, *_args, **_kwargs):
        from misc2 import wxCaretPtr
        val = apply(windowsc.wxWindow_GetCaret,(self,) + _args, _kwargs)
        if val: val = wxCaretPtr(val)
        return val
    "


    // Properties list
    %pragma(python) addtoclass = "
    _prop_list_ = {
        'size'          : ('GetSize',                  'SetSize'),
        'enabled'       : ('IsEnabled',                'Enable'),
        'background'    : ('GetBackgroundColour',      'SetBackgroundColour'),
        'foreground'    : ('GetForegroundColour',      'SetForegroundColour'),
        'children'      : ('GetChildren',              None),
        'charHeight'    : ('GetCharHeight',            None),
        'charWidth'     : ('GetCharWidth',             None),
        'clientSize'    : ('GetClientSize',            'SetClientSize'),
        'font'          : ('GetFont',                  'SetFont'),
        'grandParent'   : ('GetGrandParent',           None),
        'handle'        : ('GetHandle',                None),
        'label'         : ('GetLabel',                 'SetLabel'),
        'name'          : ('GetName',                  'SetName'),
        'parent'        : ('GetParent',                None),
        'position'      : ('GetPosition',              'SetPosition'),
        'title'         : ('GetTitle',                 'SetTitle'),
        'style'         : ('GetWindowStyleFlag',       'SetWindowStyleFlag'),
        'visible'       : ('IsShown',                  'Show'),
        'toolTip'       : ('GetToolTip',               'SetToolTip'),
        'sizer'         : ('GetSizer',                 'SetSizer'),
        'validator'     : ('GetValidator',             'SetValidator'),
        'dropTarget'    : ('GetDropTarget',            'SetDropTarget'),
        'caret'         : ('GetCaret',                 'SetCaret'),
        'autoLayout'    : ('GetAutoLayout',            'SetAutoLayout'),
        'constraints'   : ('GetConstraints',           'SetConstraints'),

    }
    _prop_list_.update(wxEvtHandler._prop_list_)
    "
};

//%clear int* x, int* y;



%pragma(python) code = "
def wxDLG_PNT(win, point_or_x, y=None):
    if y is None:
        return win.ConvertDialogPointToPixels(point_or_x)
    else:
        return win.ConvertDialogPointToPixels(wxPoint(point_or_x, y))

def wxDLG_SZE(win, size_width, height=None):
    if height is None:
        return win.ConvertDialogSizeToPixels(size_width)
    else:
        return win.ConvertDialogSizeToPixels(wxSize(size_width, height))
"

#ifdef __WXMSW__
%inline %{
    wxWindow* wxWindow_FindFocus() {
        return wxWindow::FindFocus();
    }
%}


%inline %{
wxWindow* wxWindow_FromHWND(unsigned long hWnd) {
    wxWindow* win = new wxWindow;
    win->SetHWND(hWnd);
    win->SubclassWin(hWnd);
    return win;
}
%}
#endif

%inline %{
    int wxWindow_NewControlId() {
        return wxWindow::NewControlId();
    }
    int wxWindow_NextControlId(int id) {
        return wxWindow::NextControlId(id);
    }
    int wxWindow_PrevControlId(int id) {
        return wxWindow::PrevControlId(id);
    }
%}


//---------------------------------------------------------------------------

class wxPanel : public wxWindow {
public:
    wxPanel(wxWindow* parent,
            const wxWindowID id,
            const wxPoint& pos = wxPyDefaultPosition,
            const wxSize& size = wxPyDefaultSize,
            long style = wxTAB_TRAVERSAL,
            const char* name = "panel");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"

    void InitDialog();
    wxButton* GetDefaultItem();
    void SetDefaultItem(wxButton *btn);

    // fix some SWIG trouble...
    %pragma(python) addtoclass = "
    def GetDefaultItem(self):
        import controls
        val = windowsc.wxPanel_GetDefaultItem(self.this)
        val = controls.wxButtonPtr(val)
        return val
"
};

//---------------------------------------------------------------------------

class wxDialog : public wxPanel {
public:
    wxDialog(wxWindow* parent,
             const wxWindowID id,
             const wxString& title,
             const wxPoint& pos = wxPyDefaultPosition,
             const wxSize& size = wxPyDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const char* name = "dialogBox");

    %pragma(python) addtomethod = "__init__:#wx._StdDialogCallbacks(self)"

    void Centre(int direction = wxBOTH);
    void EndModal(int retCode);
    wxString GetTitle();
    void Iconize(bool iconize);
    bool IsIconized();
    void SetModal(bool flag);
    bool IsModal();
    void SetTitle(const wxString& title);
    bool Show(bool show);
    int ShowModal();

    int  GetReturnCode();
    void SetReturnCode(int retCode);
};

//---------------------------------------------------------------------------

class wxScrolledWindow : public wxPanel {
public:
    wxScrolledWindow(wxWindow* parent,
                     const wxWindowID id = -1,
                     const wxPoint& pos = wxPyDefaultPosition,
                     const wxSize& size = wxPyDefaultSize,
                     long style = wxHSCROLL | wxVSCROLL,
                     char* name = "scrolledWindow");

    %pragma(python) addtomethod = "__init__:#wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:#wx._StdOnScrollCallbacks(self)"

    void EnableScrolling(bool xScrolling, bool yScrolling);
    int GetScrollPageSize(int orient);
    void GetScrollPixelsPerUnit(int* OUTPUT, int* OUTPUT);
    wxWindow* GetTargetWindow();
    void GetVirtualSize(int* OUTPUT, int* OUTPUT);
    bool IsRetained();
    void PrepareDC(wxDC& dc);
    void Scroll(int x, int y);
    void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                       int noUnitsX, int noUnitsY,
                       int xPos = 0, int yPos = 0, int noRefresh=FALSE);
    void SetScrollPageSize(int orient, int pageSize);
    void SetTargetWindow(wxWindow* window);
    void GetViewStart(int* OUTPUT, int* OUTPUT);
    void ViewStart(int* OUTPUT, int* OUTPUT);

    void CalcScrolledPosition( int x, int y, int *OUTPUT, int *OUTPUT);
    void CalcUnscrolledPosition( int x, int y, int *OUTPUT, int *OUTPUT);

};

//----------------------------------------------------------------------


class wxMenu : public wxEvtHandler {
public:
    wxMenu(const wxString& title = wxPyEmptyStr, long style = 0);

    void Append(int id, const wxString& item,
                const wxString& helpString = wxPyEmptyStr,
                int checkable = FALSE);
    %name(AppendMenu)void Append(int id, const wxString& item, wxMenu *subMenu,
                const wxString& helpString = wxPyEmptyStr);
    %name(AppendItem)void Append(const wxMenuItem* item);

    void AppendSeparator();
    void Break();
    void Check(int id, bool flag);
    bool IsChecked(int id);
    void Enable(int id, bool enable);
    bool IsEnabled(int id);

    int FindItem(const wxString& itemString);
    %name(FindItemById)wxMenuItem* FindItem(int id/*, wxMenu **menu = NULL*/);

    wxString GetTitle();
    void SetTitle(const wxString& title);

    wxString GetLabel(int id);
    void SetLabel(int id, const wxString& label);

    wxString GetHelpString(int id);
    void SetHelpString(int id, const wxString& helpString);
    void UpdateUI(wxEvtHandler* source = NULL);

    bool Delete(int id);
    %name(DeleteItem)bool Delete(wxMenuItem *item);
    bool Insert(size_t pos, wxMenuItem *item);
    wxMenuItem *Remove(int id);
    %name(RemoveItem) wxMenuItem *Remove(wxMenuItem *item);

    %addmethods {
        void Destroy() { delete self; }
    }
    %name(DestroyId)bool Destroy(int id);
    %name(DestroyItem)bool Destroy(wxMenuItem *item);

    size_t GetMenuItemCount();
    //wxMenuItemList& GetMenuItems();
    %addmethods {
        PyObject* GetMenuItems() {
            wxMenuItemList& list = self->GetMenuItems();
            return wxPy_ConvertList(&list, "wxMenuItem");
        }
    }

    void SetEventHandler(wxEvtHandler *handler);
    wxEvtHandler *GetEventHandler();

    void SetInvokingWindow(wxWindow *win);
    wxWindow *GetInvokingWindow();

    long GetStyle();

    bool IsAttached();

    void SetParent(wxMenu *parent);
    wxMenu *GetParent();
};


//----------------------------------------------------------------------

class wxMenuBar : public wxWindow {
public:
    wxMenuBar();

    bool Append(wxMenu *menu, const wxString& title);
    bool Insert(size_t pos, wxMenu *menu, const wxString& title);
    size_t GetMenuCount();
    wxMenu *GetMenu(size_t pos);
    wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title);
    wxMenu *Remove(size_t pos);
    void EnableTop(size_t pos, bool enable);
    void SetLabelTop(size_t pos, const wxString& label);
    wxString GetLabelTop(size_t pos);
    int FindMenuItem(const wxString& menuString, const wxString& itemString);
    %name(FindItemById)wxMenuItem* FindItem(int id/*, wxMenu **menu = NULL*/);
    void Enable(int id, bool enable);
    void Check(int id, bool check);
    bool IsChecked(int id);
    bool IsEnabled(int id);

    void SetLabel(int id, const wxString &label);
    wxString GetLabel(int id);

    void SetHelpString(int id, const wxString& helpString);
    wxString GetHelpString(int id);

};


//----------------------------------------------------------------------

class wxMenuItem {
public:
    wxMenuItem(wxMenu* parentMenu=NULL, int id=wxID_SEPARATOR,
               const wxString& text = wxPyEmptyStr,
               const wxString& help = wxPyEmptyStr,
               bool isCheckable = FALSE, wxMenu* subMenu = NULL);


    wxMenu *GetMenu();
    void SetId(int id);
    int  GetId();
    bool IsSeparator();
    void SetText(const wxString& str);
    wxString GetLabel();
    const wxString& GetText();
    void SetCheckable(bool checkable);
    bool IsCheckable();
    bool IsSubMenu();
    void SetSubMenu(wxMenu *menu);
    wxMenu *GetSubMenu();
    void Enable(bool enable = TRUE);
    bool IsEnabled();
    void Check(bool check = TRUE);
    bool IsChecked();
    void Toggle();
    void SetHelp(const wxString& str);
    const wxString& GetHelp();
    wxAcceleratorEntry *GetAccel();
    void SetAccel(wxAcceleratorEntry *accel);

};

//---------------------------------------------------------------------------



