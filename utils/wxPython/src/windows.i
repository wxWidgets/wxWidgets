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

%pragma(python) code = "import wx"


//---------------------------------------------------------------------------

class wxEvtHandler {
public:
    bool ProcessEvent(wxEvent& event);
    %addmethods {
        void Connect( int id, int lastId, int eventType, PyObject* func) {
            if (PyCallable_Check(func)) {
                self->Connect(id, lastId, eventType,
                          (wxObjectEventFunction) &wxPyCallback::EventThunker,
                          new wxPyCallback(func));
            }
        }
    }
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

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

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
    int  GetCharHeight();
    int  GetCharWidth();
    %name(GetClientSizeTuple) void GetClientSize(int *OUTPUT, int *OUTPUT);
    wxSize GetClientSize();
    wxLayoutConstraints * GetConstraints();
    //wxEvtHandler* GetEventHandler();

    wxFont& GetFont();
    wxColour GetForegroundColour();
    wxWindow * GetGrandParent();
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
    bool Hide();
    void InitDialog();
    bool IsEnabled();
    bool IsRetained();
    bool IsShown();
    bool IsTopLevel();
    void Layout();
    bool LoadFromResource(wxWindow* parent, const wxString& resourceName, const wxResourceTable* resourceTable = NULL);
    void Lower();
    void MakeModal(bool flag);
    %name(MoveXY)void Move(int x, int y);
    void Move(const wxPoint& point);

    //wxEvtHandler* PopEventHandler(bool deleteHandler = FALSE);
    //void PushEventHandler(wxEvtHandler* handler);

    %name(PopupMenuXY)bool PopupMenu(wxMenu *menu, int x, int y);
    bool PopupMenu(wxMenu *menu, const wxPoint& pos);

    void Raise();
    void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);
    void ReleaseMouse();
    bool Reparent( wxWindow* newParent );

    // (uses apply'ed INOUT typemap, see above)
    %name(ScreenToClientXY)void ScreenToClient(int *x, int *y);
    wxPoint ScreenToClient(const wxPoint& pt);

    void ScrollWindow(int dx, int dy, const wxRect* rect = NULL);
    void SetAcceleratorTable(const wxAcceleratorTable& accel);
    void SetAutoLayout(bool autoLayout);
    void SetBackgroundColour(const wxColour& colour);
    void SetConstraints(wxLayoutConstraints *constraints);
    void SetFocus();
    void SetFont(const wxFont& font);
    void SetForegroundColour(const wxColour& colour);
    void SetId(int id);
    void SetName(const wxString& name);
    void SetScrollbar(int orientation, int position, int thumbSize, int range, bool refresh = TRUE);
    void SetScrollPos(int orientation, int pos, bool refresh = TRUE);

    %name(SetDimensions) void SetSize(int x, int y, int width, int height, int sizeFlags=wxSIZE_AUTO);
    %addmethods {
        void SetSize(const wxSize& size) {
            self->SetSize(size.x, size.y);
        }

        void SetPosition(const wxPoint& pos) {
            self->SetSize(pos.x, pos.y, -1, -1);
        }
    }

    void SetSizeHints(int minW=-1, int minH=-1, int maxW=-1, int maxH=-1, int incW=-1, int incH=-1);
    %name(SetClientSizeWH)void SetClientSize(int width, int height);
    void SetClientSize(const wxSize& size);
    //void SetPalette(wxPalette* palette);
    void SetCursor(const wxCursor&cursor);
    //void SetEventHandler(wxEvtHandler* handler);
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



//---------------------------------------------------------------------------

class wxPanel : public wxWindow {
public:
    wxPanel(wxWindow* parent,
            const wxWindowID id,
            const wxPoint& pos = wxPyDefaultPosition,
            const wxSize& size = wxPyDefaultSize,
            long style = wxTAB_TRAVERSAL,
            const char* name = "panel");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

    void InitDialog();
    wxButton* GetDefaultItem();
    void SetDefaultItem(wxButton *btn);

    // fix a SWIG turd...
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

    %pragma(python) addtomethod = "__init__:wx._StdDialogCallbacks(self)"

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

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"
    %pragma(python) addtomethod = "__init__:wx._StdOnScrollCallbacks(self)"

    void EnableScrolling(bool xScrolling, bool yScrolling);
    void GetScrollPixelsPerUnit(int* OUTPUT, int* OUTPUT);
    void GetVirtualSize(int* OUTPUT, int* OUTPUT);
    bool IsRetained();
    void PrepareDC(wxDC& dc);
    void Scroll(int x, int y);
    void SetScrollbars(int pixelsPerUnitX, int pixelsPerUnitY,
                       int noUnitsX, int noUnitsY,
                       int xPos = 0, int yPos = 0);
    void ViewStart(int* OUTPUT, int* OUTPUT);
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
#ifndef __WXGTK__
    %name(AppendItem)void Append(const wxMenuItem* item);
#endif

    void AppendSeparator();
    void Break();
    void Check(int id, bool flag);
    void Enable(int id, bool enable);
    int FindItem(const wxString& itemString);
    wxString GetTitle();
    void SetTitle(const wxString& title);
    wxMenuItem* FindItemForId(int id);
    wxString GetHelpString(int id);
    wxString GetLabel(int id);
    void SetHelpString(int id, const wxString& helpString);
    bool IsChecked(int id);
    bool IsEnabled(int id);
    void SetLabel(int id, const wxString& label);
    void UpdateUI(wxEvtHandler* source = NULL);

    %addmethods {
        void Destroy() {
            delete self;
        }
    }

};


//
// This one knows how to set a callback and handle INC- and DECREFing it.  To
// be used for PopupMenus, but you must retain a referece to it while using
// it.
//
//  class wxPyMenu : public wxMenu {
//  public:
//      wxPyMenu(const wxString& title = wxPyEmptyStr, PyObject* func = NULL);
//      ~wxPyMenu();
//  };

//----------------------------------------------------------------------

class wxMenuBar : public wxEvtHandler {
public:
    wxMenuBar();

    void Append(wxMenu *menu, const wxString& title);
    void Check(int id, bool flag);
    bool Checked(int id);
    void Enable(int id, bool enable);
    bool Enabled(int id);
    int FindMenuItem(const wxString& menuString, const wxString& itemString);
    wxMenuItem * FindItemForId(int id);
#ifdef __WXMSW__
    void EnableTop(int pos, bool enable);
    wxString GetHelpString(int id);
    wxString GetLabel(int id);
    void SetHelpString(int id, const wxString& helpString);
    void SetLabel(int id, const wxString& label);
    wxString GetLabelTop(int pos);
    void SetLabelTop(int pos, const wxString& label);
#endif
    int GetMenuCount();
    wxMenu* GetMenu(int i);
};


//----------------------------------------------------------------------

class wxMenuItem {
public:
#ifndef __WXGTK__
    wxMenuItem(wxMenu* parentMenu=NULL, int id=ID_SEPARATOR,
               const wxString& text = wxPyEmptyStr,
               const wxString& helpString = wxPyEmptyStr,
               bool checkable = FALSE, wxMenu* subMenu = NULL);
#else
    wxMenuItem();
#endif

    bool IsSeparator();
    bool IsEnabled();
    bool IsChecked();
    bool IsCheckable();
    int  GetId();
    wxMenu* GetSubMenu();
    void SetName(const wxString& strName);
    wxString GetName();
    wxString GetHelp();
    void SetHelp(const wxString& strHelp);
    void Enable(bool bDoEnable = TRUE);
    void Check(bool bDoCheck = TRUE);

#ifdef __WXMSW__
    wxColour& GetBackgroundColour();
    wxBitmap GetBitmap(bool checked = TRUE);
    wxFont& GetFont();
    int GetMarginWidth();
    wxColour& GetTextColour();
    void SetBackgroundColour(const wxColour& colour);
    void SetBitmaps(const wxBitmap& checked, const wxBitmap& unchecked = wxNullBitmap);
    void SetFont(const wxFont& font);
    void SetMarginWidth(int width);
    void SetTextColour(const wxColour& colour);
    void DeleteSubMenu();
#endif
};

//---------------------------------------------------------------------------


