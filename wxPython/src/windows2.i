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
#ifdef OLD_GRID
#include <wx/grid.h>
#endif
#include <wx/notebook.h>
#include <wx/splitter.h>
#include <wx/imaglist.h>
#ifdef __WXMSW__
#include <wx/msw/taskbar.h>
#endif
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
     /* notebook control event types */
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
    wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
};


class wxNotebookEvent : public wxNotifyEvent {
public:
    wxNotebookEvent(wxEventType commandType = wxEVT_NULL, int id = 0,
                    int nSel = -1, int nOldSel = -1);

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
               char* name = "notebook");
    %name(wxPreNotebook)wxNotebook();

    bool Create(wxWindow *parent,
               wxWindowID id,
               const wxPoint& pos = wxDefaultPosition,
               const wxSize& size = wxDefaultSize,
               long style = 0,
               char* name = "notebook");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreNotebook:val._setOORInfo(val)"

    int GetPageCount();
    int SetSelection(int nPage);
    void AdvanceSelection(bool bForward = TRUE);
    int GetSelection();
    bool SetPageText(int nPage, const wxString& strText);
    wxString GetPageText(int nPage) const;

    void SetImageList(wxImageList* imageList);
    void AssignImageList(wxImageList *imageList) ;
    %pragma(python) addtomethod = "AssignImageList:_args[0].thisown = 0"

    wxImageList* GetImageList();
    int  GetPageImage(int nPage);
    bool SetPageImage(int nPage, int nImage);
    int GetRowCount();

    void SetPageSize(const wxSize& size);
    void SetPadding(const wxSize& padding);
    bool DeletePage(int nPage);
    bool RemovePage(int nPage);
    bool DeleteAllPages();
    bool AddPage(/*wxNotebookPage*/ wxWindow *pPage,
                 const wxString& strText,
                 int bSelect = FALSE,
                 int imageId = -1);
    bool InsertPage(int nPage,
                    /*wxNotebookPage*/ wxWindow *pPage,
                    const wxString& strText,
                    bool bSelect = FALSE,
                    int imageId = -1);
    /*wxNotebookPage*/ wxWindow *GetPage(int nPage);

    %addmethods {
        void ResizeChildren() {
            wxSizeEvent evt(self->GetClientSize());
            self->GetEventHandler()->ProcessEvent(evt);
        }
    }


};

//---------------------------------------------------------------------------


enum {
    /* splitter window events */
    wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGING,
    wxEVT_COMMAND_SPLITTER_SASH_POS_CHANGED,
    wxEVT_COMMAND_SPLITTER_UNSPLIT,
    wxEVT_COMMAND_SPLITTER_DOUBLECLICKED,
};


enum
{
    wxSPLIT_HORIZONTAL,
    wxSPLIT_VERTICAL,
    wxSPLIT_DRAG_NONE,
    wxSPLIT_DRAG_DRAGGING,
    wxSPLIT_DRAG_LEFT_DOWN
};


class wxSplitterEvent : public wxCommandEvent {
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
                     char* name = "splitterWindow");
    %name(wxPreSplitterWindow)wxSplitterWindow();

    bool Create(wxWindow* parent, wxWindowID id,
                const wxPoint& point = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style=wxSP_3D|wxCLIP_CHILDREN,
                char* name = "splitterWindow");

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

};

//---------------------------------------------------------------------------

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

    bool SetIcon(const wxIcon& icon, const char* tooltip = "");
    bool RemoveIcon(void);
    bool PopupMenu(wxMenu *menu);
    bool IsIconInstalled();
    bool IsOK();
};
#endif

//---------------------------------------------------------------------------
