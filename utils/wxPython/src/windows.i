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


class wxWindow : public wxEvtHandler {
public:

    wxWindow(wxWindow* parent, const wxWindowID id,
             const wxPoint& pos = wxPyDefaultPosition,
             const wxSize& size = wxPyDefaultSize,
             long style = 0,
             char* name = "panel");

    %pragma(python) addtomethod = "__init__:wx._StdWindowCallbacks(self)"

    void CaptureMouse();
    void Center(int direction = wxHORIZONTAL);
    void Centre(int direction = wxHORIZONTAL);
    %name(ClientToScreenXY)void ClientToScreen(int *BOTH, int *BOTH);
#ifndef __WXGTK__
    wxPoint ClientToScreen(const wxPoint& pt);
#endif
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
#ifdef __WXMSW__
    wxButton* GetDefaultItem();
#endif
    //wxEvtHandler* GetEventHandler();

    wxFont& GetFont();
    wxColour GetForegroundColour();
    wxWindow * GetGrandParent();
    int GetId();
    wxString GetLabel();
    wxString GetName();
    wxWindow * GetParent();
    %name(GetPositionTuple) void GetPosition(int *OUTPUT, int *OUTPUT);
    wxPoint GetPosition();
    wxRect GetRect();
    int  GetReturnCode();
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
    long GetWindowStyleFlag();
    void InitDialog();
    bool IsEnabled();
    bool IsRetained();
    bool IsShown();
    void Layout();
    bool LoadFromResource(wxWindow* parent, const wxString& resourceName, const wxResourceTable* resourceTable = NULL);
    void Lower();
    void MakeModal(bool flag);
    %name(MoveXY)void Move(int x, int y);
    void Move(const wxPoint& point);

    //wxEvtHandler* PopEventHandler(bool deleteHandler = FALSE);
    bool PopupMenu(wxMenu *menu, int x, int y);
    //void PushEventHandler(wxEvtHandler* handler);

    void Raise();
    void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);
    void ReleaseMouse();
    %name(ScreenToClientXY)void ScreenToClient(int *BOTH, int *BOTH);
#ifndef __WXGTK__
    wxPoint ScreenToClient(const wxPoint& pt);
#endif

    void ScrollWindow(int dx, int dy, const wxRect* rect = NULL);
    void SetAcceleratorTable(const wxAcceleratorTable& accel);
    void SetAutoLayout(bool autoLayout);
    void SetBackgroundColour(const wxColour& colour);
    void SetConstraints(wxLayoutConstraints *constraints);
    void SetDoubleClick(bool allowDoubleClick);
    void SetFocus();
    void SetFont(const wxFont& font);
    void SetForegroundColour(const wxColour& colour);
    void SetId(int id);
    void SetName(const wxString& name);
    void SetReturnCode(int retCode);
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

%pragma(python) code = "
def wxDLG_PNT(win, point):
    return win.ConvertDialogPointToPixels(point)

def wxDLG_SZE(win, size):
    return win.ConvertDialogSizeToPixels(size)
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
};

//---------------------------------------------------------------------------

class wxScrolledWindow : public wxWindow {
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
    wxMenu(const wxString& title = wxPyEmptyStr);

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
};


//
// This one knows how to set a callback and handle INC- and DECREFing it.  To
// be used for PopupMenus, but you must retain a referece to it while using
// it.
//
class wxPyMenu : public wxMenu {
public:
    wxPyMenu(const wxString& title = wxPyEmptyStr, PyObject* func = NULL);
    ~wxPyMenu();
};

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
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
// Revision 1.15  1999/04/30 03:29:19  RD
// wxPython 2.0b9, first phase (win32)
// Added gobs of stuff, see wxPython/README.txt for details
//
// Revision 1.14.4.3  1999/03/27 23:29:15  RD
//
// wxPython 2.0b8
//     Python thread support
//     various minor additions
//     various minor fixes
//
// Revision 1.14.4.2  1999/03/16 06:26:29  RD
//
// wxPython 2.0b7
//
// Revision 1.14.4.1  1999/03/16 06:04:04  RD
//
// wxPython 2.0b7
//
// Revision 1.14  1999/02/23 23:48:33  RD
//
// reenabled some methods for wxPython on wxGTK
//
// Revision 1.13  1999/02/20 10:02:38  RD
//
// Changes needed to enable wxGTK compatibility.
//
// Revision 1.12  1999/02/20 09:03:03  RD
// Added wxWindow_FromHWND(hWnd) for wxMSW to construct a wxWindow from a
// window handle.  If you can get the window handle into the python code,
// it should just work...  More news on this later.
//
// Added wxImageList, wxToolTip.
//
// Re-enabled wxConfig.DeleteAll() since it is reportedly fixed for the
// wxRegConfig class.
//
// As usual, some bug fixes, tweaks, etc.
//
// Revision 1.11  1998/12/18 15:49:10  RR
//
//   wxClipboard now serves the primary selection as well
//   wxPython fixes
//   warning mesages
//
// Revision 1.10  1998/12/17 17:52:20  RD
//
// wxPython 0.5.2
// Minor fixes and SWIG code generation for RR's changes.  MSW and GTK
// versions are much closer now!
//
// Revision 1.9  1998/12/17 14:07:46  RR
//
//   Removed minor differences between wxMSW and wxGTK
//
// Revision 1.8  1998/12/16 22:10:56  RD
//
// Tweaks needed to be able to build wxPython with wxGTK.
//
// Revision 1.7  1998/12/15 20:41:25  RD
// Changed the import semantics from "from wxPython import *" to "from
// wxPython.wx import *"  This is for people who are worried about
// namespace pollution, they can use "from wxPython import wx" and then
// prefix all the wxPython identifiers with "wx."
//
// Added wxTaskbarIcon for wxMSW.
//
// Made the events work for wxGrid.
//
// Added wxConfig.
//
// Added wxMiniFrame for wxGTK, (untested.)
//
// Changed many of the args and return values that were pointers to gdi
// objects to references to reflect changes in the wxWindows API.
//
// Other assorted fixes and additions.
//
// Revision 1.6  1998/10/02 06:40:43  RD
//
// Version 0.4 of wxPython for MSW.
//
// Revision 1.5  1998/08/17 18:29:40  RD
// Removed an extra method definition
//
// Revision 1.4  1998/08/16 04:31:11  RD
// More wxGTK work.
//
// Revision 1.3  1998/08/15 07:36:47  RD
// - Moved the header in the .i files out of the code that gets put into
// the .cpp files.  It caused CVS conflicts because of the RCS ID being
// different each time.
//
// - A few minor fixes.
//
// Revision 1.2  1998/08/14 23:36:46  RD
// Beginings of wxGTK compatibility
//
// Revision 1.1  1998/08/09 08:25:52  RD
// Initial version
//
//


