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
#include <wx/tooltip.h>
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


%{
    static wxString wxPyEmptyStr("");
%}

//---------------------------------------------------------------------------

class wxEvtHandler : public wxObject {
public:
    wxEvtHandler();

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
            else if (func == Py_None) {
                self->Disconnect(id, lastId, eventType,
                                 (wxObjectEventFunction)
                                 &wxPyCallback::EventThunker);
            }
            else {
                PyErr_SetString(PyExc_TypeError, "Expected callable object or None.");
            }
        }

        bool Disconnect(int id, int lastId = -1,
                        wxEventType eventType = wxEVT_NULL) {
            return self->Disconnect(id, lastId, eventType,
                                   (wxObjectEventFunction)
                                    &wxPyCallback::EventThunker);
        }
    }

    %addmethods {
        void _setOORInfo(PyObject* _self) {
            self->SetClientObject(new wxPyClientData(_self));
        }
    }
};


//----------------------------------------------------------------------

class wxValidator : public wxEvtHandler {
public:
    wxValidator();
    //~wxValidator();

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    wxValidator* Clone();
    wxWindow* GetWindow();
    void SetWindow(wxWindow* window);

    static bool IsSilent();
    static void SetBellOnError(int doIt = TRUE);

};


//----------------------------------------------------------------------
%{
class wxPyValidator : public wxValidator {
    DECLARE_DYNAMIC_CLASS(wxPyValidator);
public:
    wxPyValidator() {
    }

    ~wxPyValidator() {
    }

    wxObject* Clone() const {
        wxPyValidator* ptr = NULL;
        wxPyValidator* self = (wxPyValidator*)this;

        wxPyTState* state = wxPyBeginBlockThreads();
        if (self->m_myInst.findCallback("Clone")) {
            PyObject* ro;
            ro = self->m_myInst.callCallbackObj(Py_BuildValue("()"));
            if (ro) {
                SWIG_GetPtrObj(ro, (void **)&ptr, "_wxPyValidator_p");
                Py_DECREF(ro);
            }
        }
        wxPyEndBlockThreads(state);

        // This is very dangerous!!! But is the only way I could find
        // to squash a memory leak.  Currently it is okay, but if the
        // validator architecture in wxWindows ever changes, problems
        // could arise.
        delete self;
        return ptr;
    }


    DEC_PYCALLBACK_BOOL_WXWIN(Validate);
    DEC_PYCALLBACK_BOOL_(TransferToWindow);
    DEC_PYCALLBACK_BOOL_(TransferFromWindow);

    PYPRIVATE;
};

IMP_PYCALLBACK_BOOL_WXWIN(wxPyValidator, wxValidator, Validate);
IMP_PYCALLBACK_BOOL_(wxPyValidator, wxValidator, TransferToWindow);
IMP_PYCALLBACK_BOOL_(wxPyValidator, wxValidator, TransferFromWindow);

IMPLEMENT_DYNAMIC_CLASS(wxPyValidator, wxValidator);

%}

class wxPyValidator : public wxValidator {
public:
    wxPyValidator();

    void _setCallbackInfo(PyObject* self, PyObject* _class, int incref=TRUE);
    %pragma(python) addtomethod = "__init__:self._setCallbackInfo(self, wxPyValidator, 1)"

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
};

//----------------------------------------------------------------------

%apply int * INOUT { int* x, int* y };

class wxWindow : public wxEvtHandler {
public:
    wxWindow(wxWindow* parent, const wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = 0,
             char* name = "panel");
    %name(wxPreWindow)wxWindow();

    bool Create(wxWindow* parent, const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = 0,
                char* name = "panel");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreWindow:val._setOORInfo(val)"


    void CaptureMouse();
    void Center(int direction = wxBOTH);
    void Centre(int direction = wxBOTH);
    void CentreOnParent(int direction = wxBOTH );
    void CenterOnParent(int direction = wxBOTH );
    void CentreOnScreen(int direction = wxBOTH );
    void CenterOnScreen(int direction = wxBOTH );

    void Clear();

    // (uses apply'ed INOUT typemap, see above)
    %name(ClientToScreenXY)void ClientToScreen(int* x, int* y);
    wxPoint ClientToScreen(const wxPoint& pt);

    bool Close(int force = FALSE);
    bool Destroy();
    void DestroyChildren();
    bool IsBeingDeleted();
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

    wxFont GetFont();
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
    void MakeModal(bool flag=TRUE);
    %name(MoveXY)void Move(int x, int y);
    void Move(const wxPoint& point);

    wxEvtHandler* PopEventHandler(bool deleteHandler = FALSE);
    void PushEventHandler(wxEvtHandler* handler);

    %name(PopupMenuXY)bool PopupMenu(wxMenu *menu, int x, int y);
    bool PopupMenu(wxMenu *menu, const wxPoint& pos);

    void Raise();
    void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);
    void RefreshRect(const wxRect& rect);

    void ReleaseMouse();
    void RemoveChild(wxWindow* child);
    bool Reparent( wxWindow* newParent );

    // (uses apply'ed INOUT typemap, see above)
    %name(ScreenToClientXY)void ScreenToClient(int *x, int *y);
    wxPoint ScreenToClient(const wxPoint& pt);

    void ScrollWindow(int dx, int dy, const wxRect* rect = NULL);
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

        void SetRect(const wxRect& rect, int sizeFlags=wxSIZE_AUTO) {
            self->SetSize(rect, sizeFlags);
        }
    }

    void SetSizeHints(int minW=-1, int minH=-1, int maxW=-1, int maxH=-1, int incW=-1, int incH=-1);
    %name(SetClientSizeWH)void SetClientSize(int width, int height);
    void SetClientSize(const wxSize& size);
    //void SetPalette(wxPalette* palette);
    void SetCursor(const wxCursor& cursor);
    void SetEventHandler(wxEvtHandler* handler);
    void SetExtraStyle(long exStyle);
    void SetTitle(const wxString& title);
    bool Show(bool show=TRUE);
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

#ifndef __WXMAC__
    void SetDropTarget(wxDropTarget* target);
    wxDropTarget* GetDropTarget();
    %pragma(python) addtomethod = "SetDropTarget:_args[0].thisown = 0"
#endif

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

    void Freeze();
    void Thaw();
    void Update();

    wxString GetHelpText();
    void SetHelpText(const wxString& helpText);

    bool ScrollLines(int lines);
    bool ScrollPages(int pages);
    bool LineUp();
    bool LineDown();
    bool PageUp();
    bool PageDown();

    static wxWindow* FindFocus();
    static int NewControlId();
    static int NextControlId(int id);
    static int PrevControlId(int id);

    void SetAcceleratorTable(const wxAcceleratorTable& accel);
    wxAcceleratorTable *GetAcceleratorTable();

#ifdef __WXMSW__
    // This seems pretty bogus, but is currently needed for SplitTree
    void OnPaint(wxPaintEvent& event);
#endif
};




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
wxWindow* wxWindow_FromHWND(unsigned long hWnd) {
    wxWindow* win = new wxWindow;
    win->SetHWND(hWnd);
    win->SubclassWin(hWnd);
    return win;
}
%}
#endif


//---------------------------------------------------------------------------

class wxPanel : public wxWindow {
public:
    wxPanel(wxWindow* parent,
            const wxWindowID id,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxTAB_TRAVERSAL,
            const char* name = "panel");
    %name(wxPrePanel)wxPanel();

    bool Create(wxWindow* parent,
                const wxWindowID id,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxTAB_TRAVERSAL,
                const char* name = "panel");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPrePanel:val._setOORInfo(val)"

    void InitDialog();
    wxButton* GetDefaultItem();
    void SetDefaultItem(wxButton *btn);

};

//---------------------------------------------------------------------------


// TODO: Add wrappers for the wxScrollHelper class, make wxScrolledWindow
//       derive from it and wxPanel.


class wxScrolledWindow : public wxPanel {
public:
    wxScrolledWindow(wxWindow* parent,
                     const wxWindowID id = -1,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxHSCROLL | wxVSCROLL,
                     char* name = "scrolledWindow");
    %name(wxPreScrolledWindow)wxScrolledWindow();

    bool Create(wxWindow* parent,
                const wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHSCROLL | wxVSCROLL,
                char* name = "scrolledWindow");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreScrolledWindow:val._setOORInfo(val)"

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
    %pragma(python) addtoclass = "ViewStart = GetViewStart"

    void CalcScrolledPosition( int x, int y, int *OUTPUT, int *OUTPUT);
    void CalcUnscrolledPosition( int x, int y, int *OUTPUT, int *OUTPUT);

    void SetScale(double xs, double ys);
    double GetScaleX();
    double GetScaleY();

    void AdjustScrollbars();
};

//----------------------------------------------------------------------


class wxMenu : public wxEvtHandler {
public:
    wxMenu(const wxString& title = wxPyEmptyStr, long style = 0);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

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
    wxMenuBar(long style = 0);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    bool Append(wxMenu *menu, const wxString& title);
    bool Insert(size_t pos, wxMenu *menu, const wxString& title);
    size_t GetMenuCount();
    wxMenu *GetMenu(size_t pos);
    wxMenu *Replace(size_t pos, wxMenu *menu, const wxString& title);
    wxMenu *Remove(size_t pos);
    void EnableTop(size_t pos, bool enable);
    void SetLabelTop(size_t pos, const wxString& label);
    wxString GetLabelTop(size_t pos);
    int FindMenu(const wxString& title);
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

class wxMenuItem : public wxObject {
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

    static wxString GetLabelFromText(const wxString& text);

    // wxOwnerDrawn methods
#ifdef __WXMSW__
    void SetFont(const wxFont& font);
    wxFont GetFont();
    void SetTextColour(const wxColour& colText);
    wxColour GetTextColour();
    void SetBackgroundColour(const wxColour& colBack);
    wxColour GetBackgroundColour();
    void SetBitmaps(const wxBitmap& bmpChecked,
                    const wxBitmap& bmpUnchecked = wxNullBitmap);
    void SetBitmap(const wxBitmap& bmpChecked);
    wxBitmap GetBitmap(bool bChecked = TRUE);
    void SetMarginWidth(int nWidth);
    int GetMarginWidth();
    static int GetDefaultMarginWidth();
    //void SetName(const wxString& strName);
    //const wxString& GetName();
    //void SetCheckable(bool checkable);
    //bool IsCheckable();
    bool IsOwnerDrawn();
    void ResetOwnerDrawn();
#endif
};

//---------------------------------------------------------------------------



