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

#ifdef __WXMSW__
    // wxGTK defines wxMenuItem inside menu.h
#include <wx/menuitem.h>
#endif

#ifdef __WXMSW__
#include <wx/minifram.h>
#endif
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i


//---------------------------------------------------------------------------

class wxEvtHandler {
public:
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


    void CaptureMouse();
    //void Center(int direction = wxHORIZONTAL);
    void Centre(int direction = wxHORIZONTAL);
    void ClientToScreen(int *BOTH, int *BOTH);
    bool Close(int force = FALSE);
    bool Destroy();
    void DestroyChildren();
#ifdef __WXMSW__
    void DragAcceptFiles(bool accept);
#endif
    void Enable(bool enable);
    //bool FakePopupMenu(wxMenu* menu, int x, int y);
    void Fit();
    wxColour GetBackgroundColour();
#ifdef __WXMSW__
    int  GetCharHeight();
    int  GetCharWidth();
#endif
    void GetClientSize(int *OUTPUT, int *OUTPUT);
    wxLayoutConstraints * GetConstraints();
#ifdef __WXMSW__
    wxButton* GetDefaultItem();
#endif
    //wxEvtHandler* GetEventHandler();
    wxFont* GetFont();
#ifdef __WXMSW__
    wxColour GetForegroundColour();
    wxWindow * GetGrandParent();
#endif
    int GetId();
    void GetPosition(int *OUTPUT, int *OUTPUT);
#ifdef __WXMSW__
    wxString& GetLabel();
    wxString& GetName();
#else
    wxString GetLabel();
    wxString GetName();
#endif
    wxWindow * GetParent();
    int  GetReturnCode();
    int GetScrollThumb(int orientation);
    int GetScrollPos(int orientation);
    int GetScrollRange(int orientation);
    void GetSize(int *OUTPUT, int *OUTPUT);
#ifdef __WXMSW__
    void GetTextExtent(const wxString& string, int *OUTPUT, int *OUTPUT); // int* descent = NULL, int* externalLeading = NULL, const wxFont* font = NULL, bool use16 = FALSE)
#endif
#ifdef __WXMSW__
    wxString& GetTitle();
#else
    wxString GetTitle();
#endif
    long GetWindowStyleFlag();
    void InitDialog();
    bool IsEnabled();
    bool IsRetained();
    bool IsShown();
    void Layout();
#ifdef __WXMSW__
    bool LoadFromResource(wxWindow* parent, const wxString& resourceName, const wxResourceTable* resourceTable = NULL);
#endif
    void Lower();
    void MakeModal(bool flag);
    void Move(int x, int y);

    //wxEvtHandler* PopEventHandler(bool deleteHandler = FALSE);
    bool PopupMenu(wxMenu *menu, int x, int y);
    //void PushEventHandler(wxEvtHandler* handler);

    void Raise();
    void Refresh(bool eraseBackground = TRUE, const wxRect* rect = NULL);
    void ReleaseMouse();
    void ScreenToClient(int *BOTH, int *BOTH);
    void ScrollWindow(int dx, int dy, const wxRect* rect = NULL);
    void SetAutoLayout(bool autoLayout);
    void SetBackgroundColour(const wxColour& colour);
    void SetConstraints(wxLayoutConstraints *constraints);
    void SetDoubleClick(bool allowDoubleClick);
    void SetFocus();
    void SetFont(const wxFont& font);
#ifdef __WXMSW__
    void SetForegroundColour(const wxColour& colour);
#endif
    void SetId(int id);
    void SetName(const wxString& name);
    void SetReturnCode(int retCode);
    void SetScrollbar(int orientation, int position, int thumbSize, int range, bool refresh = TRUE);
    void SetScrollPos(int orientation, int pos, bool refresh = TRUE);

    //void SetSize(int x, int y, int width, int height, int sizeFlags=wxSIZE_AUTO);
    //%name(SetSizeOnly) void SetSize(int width, int height);

    %name(SetDimensions) void SetSize(int x, int y, int width, int height, int sizeFlags=wxSIZE_AUTO);
    %addmethods {
        void SetSize(const wxSize& size) {
            self->SetSize(size.x, size.y);
        }

        void SetPosition(const wxPoint& pos) {
            self->SetSize(pos.x, pos.y, -1, -1);
        }
    }

#ifdef __WXMSW__
    void SetSizeHints(int minW=-1, int minH=-1, int maxW=-1, int maxH=-1, int incW=-1, int incH=-1);
#endif
    void SetClientSize(int width, int height);
    //void SetPalette(wxPalette* palette);
    //void SetColourMap(wxColourMap *colourMap);
    void SetCursor(const wxCursor&cursor);
    //void SetEventHandler(wxEvtHandler* handler);
    void SetTitle(const wxString& title);
    bool Show(bool show);
    bool TransferDataFromWindow();
    bool TransferDataToWindow();
    bool Validate();
#ifdef __WXMSW__
    void WarpPointer(int x, int y);
#endif

};


// Static method(s)
#ifdef __WXMSW__
%inline %{
    wxWindow* wxWindow_FindFocus() {
        return wxWindow::FindFocus();
    }
%}
#endif

//----------------------------------------------------------------------

class wxFrame : public wxWindow {
public:
    wxFrame(wxWindow* parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos = wxPyDefaultPosition,
            const wxSize& size = wxPyDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            char* name = "frame");

    void Centre(int direction = wxBOTH);
#ifdef __WXMSW__
    void Command(int id);
#endif
    bool CreateStatusBar(int number = 1);
    wxMenuBar* GetMenuBar();
    wxStatusBar* GetStatusBar();
#ifdef __WXMSW__
    wxString& GetTitle();
#else
    wxString GetTitle();
#endif
#ifdef __WXMSW__
    void Iconize(bool iconize);
    bool IsIconized();
    void SetAcceleratorTable(const wxAcceleratorTable& accel);
    void Maximize(bool maximize);
#endif
    void SetIcon(const wxIcon& icon);
    void SetMenuBar(wxMenuBar* menuBar);
    void SetStatusText(const wxString& text, int number = 0);
    void SetStatusWidths(int LCOUNT, int* LIST); // use typemap
    void SetTitle(const wxString& title);

};

//---------------------------------------------------------------------------

#ifdef __WXMSW__
class wxMiniFrame : public wxFrame {
public:
    wxMiniFrame(wxWindow* parent, const wxWindowID id, const wxString& title,
                const wxPoint& pos = wxPyDefaultPosition,
                const wxSize& size = wxPyDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                char* name = "frame");
};
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

    void Centre(int direction = wxBOTH);
    void EndModal(int retCode);
    wxString GetTitle();
#ifdef __WXMSW__
    void Iconize(bool iconize);
    bool IsIconized();
    void SetModal(bool flag);
#endif
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
    void AppendSeparator();
    void Break();
    void Check(int id, bool flag);
    void Enable(int id, bool enable);
    int FindItem(const wxString& itemString);
#ifdef __WXMSW__
    wxMenuItem* FindItemForId(int id);
    wxString& GetHelpString(int id);
    wxString GetLabel(int id);
    wxString GetTitle();
    void SetHelpString(int id, const wxString& helpString);
    void SetTitle(const wxString& title);
#endif
    bool IsChecked(int id);
    bool IsEnabled(int id);
    void SetLabel(int id, const wxString& label);
};


#ifdef __WXMSW__
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
#endif

//----------------------------------------------------------------------

class wxMenuBar : public wxEvtHandler {
public:
    wxMenuBar();

    void Append(wxMenu *menu, const wxString& title);
    void Check(int id, bool flag);
    bool Checked(int id);
    void Enable(int id, bool enable);
    int FindMenuItem(const wxString& menuString, const wxString& itemString);
#ifdef __WXMSW__
    void EnableTop(int pos, bool enable);
    wxMenuItem * FindItemForId(int id);
    wxString GetHelpString(int id);
    wxString GetLabel(int id);
    void SetHelpString(int id, const wxString& helpString);
    void SetLabel(int id, const wxString& label);
    wxString GetLabelTop(int pos);
    void SetLabelTop(int pos, const wxString& label);
#endif
};


//----------------------------------------------------------------------

class wxMenuItem {
public:
    bool IsSeparator();
    bool IsEnabled();
    bool IsChecked();
    int  GetId();
    wxMenu* GetSubMenu();
#ifdef __WXMSW__
    const wxString& GetHelp();
    void SetName(const wxString& strName);
    void SetHelp(const wxString& strHelp);
#endif
    void Enable(bool bDoEnable = TRUE);
    void Check(bool bDoCheck = TRUE);
#ifdef __WXMSW__
    void DeleteSubMenu();
    const wxString& GetName();
#endif
    bool IsCheckable();
};

//---------------------------------------------------------------------------
/////////////////////////////////////////////////////////////////////////////
//
// $Log$
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


