/////////////////////////////////////////////////////////////////////////////
// Name:        frames.i
// Purpose:     SWIG definitions of various window classes
//
// Author:      Robin Dunn
//
// Created:     8/27/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////


%module frames

%{
#include "helpers.h"
#include <wx/minifram.h>
#include <wx/splash.h>
%}

//----------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

// Import some definitions of other classes, etc.
%import _defs.i
%import misc.i
%import gdi.i
%import windows.i
%import stattool.i

%pragma(python) code = "import wx"

//----------------------------------------------------------------------

enum {
    wxFULLSCREEN_NOMENUBAR,
    wxFULLSCREEN_NOTOOLBAR,
    wxFULLSCREEN_NOSTATUSBAR,
    wxFULLSCREEN_NOBORDER,
    wxFULLSCREEN_NOCAPTION,
    wxFULLSCREEN_ALL,

    wxTOPLEVEL_EX_DIALOG,
};


//----------------------------------------------------------------------


class  wxTopLevelWindow : public wxWindow
{
public:
    // construction
    wxTopLevelWindow(wxWindow *parent,
                     wxWindowID id,
                     const wxString& title,
                     const wxPoint& pos = wxDefaultPosition,
                     const wxSize& size = wxDefaultSize,
                     long style = wxDEFAULT_FRAME_STYLE,
                     const char* name = "frame");
    %name(wxPreTopLevelWindow)wxTopLevelWindow();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const char* name = "frame");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreTopLevelWindow:val._setOORInfo(val)"

    // maximize = TRUE => maximize, otherwise - restore
    virtual void Maximize(bool maximize = TRUE);

    // undo Maximize() or Iconize()
    virtual void Restore();

    // iconize = TRUE => iconize, otherwise - restore
    virtual void Iconize(bool iconize = TRUE);

    // return TRUE if the frame is maximized
    virtual bool IsMaximized() const;

    // return TRUE if the frame is iconized
    virtual bool IsIconized() const;

    // get the frame icon
    wxIcon GetIcon() const;

    // set the frame icon
    virtual void SetIcon(const wxIcon& icon);


    // maximize the window to cover entire screen
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);

    // return TRUE if the frame is in fullscreen mode
    virtual bool IsFullScreen() const;

    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;
};

//----------------------------------------------------------------------


class wxFrame : public wxTopLevelWindow {
public:
    wxFrame(wxWindow* parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            char* name = "frame");
    %name(wxPreFrame)wxFrame();

    bool Create(wxWindow* parent, const wxWindowID id, const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                char* name = "frame");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreFrame:val._setOORInfo(val)"

    wxPoint GetClientAreaOrigin();

    void SetMenuBar(wxMenuBar *menubar);
    wxMenuBar *GetMenuBar();


    // call this to simulate a menu command
    bool Command(int id);

    // process menu command: returns TRUE if processed
    bool ProcessCommand(int id);

    // create the main status bar
    wxStatusBar* CreateStatusBar(int number = 1,
                                 long style = wxST_SIZEGRIP,
                                 wxWindowID id = -1,
                                 char* name = "statusBar");

    // get the main status bar
    wxStatusBar *GetStatusBar();

    // sets the main status bar
    void SetStatusBar(wxStatusBar *statBar);

    // forward these to status bar
    virtual void SetStatusText(const wxString &text, int number = 0);
    virtual void SetStatusWidths(int LCOUNT, int* choices); // uses typemap


    // create main toolbar
    virtual wxToolBar* CreateToolBar(long style = wxNO_BORDER|wxTB_HORIZONTAL,
                                     wxWindowID id = -1,
                                     const char* name = "toolBar");

    // get/set the main toolbar
    virtual wxToolBar *GetToolBar() const { return m_frameToolBar; }
    virtual void SetToolBar(wxToolBar *toolbar) { m_frameToolBar = toolbar; }

};

//---------------------------------------------------------------------------

class wxDialog : public wxTopLevelWindow {
public:
    wxDialog(wxWindow* parent,
             const wxWindowID id,
             const wxString& title,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const char* name = "dialogBox");
    %name(wxPreDialog)wxDialog();

    bool Create(wxWindow* parent,
                const wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const char* name = "dialogBox");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreDialog:val._setOORInfo(val)"

    void Centre(int direction = wxBOTH);
    void EndModal(int retCode);
    void SetModal(bool flag);
    bool IsModal();
    int ShowModal();

    int  GetReturnCode();
    void SetReturnCode(int retCode);

    wxSizer* CreateTextSizer( const wxString &message );
    wxSizer* CreateButtonSizer( long flags );

};


//---------------------------------------------------------------------------

class wxMiniFrame : public wxFrame {
public:
    wxMiniFrame(wxWindow* parent, const wxWindowID id, const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                char* name = "frame");
    %name(wxPreMiniFrame)wxMiniFrame();

    bool Create(wxWindow* parent, const wxWindowID id, const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                char* name = "frame");

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreMiniFrame:val._setOORInfo(val)"
};


//---------------------------------------------------------------------------

enum {
    wxSPLASH_CENTRE_ON_PARENT,
    wxSPLASH_CENTRE_ON_SCREEN,
    wxSPLASH_NO_CENTRE,
    wxSPLASH_TIMEOUT,
    wxSPLASH_NO_TIMEOUT,
};


class wxSplashScreenWindow: public wxWindow
{
public:
    wxSplashScreenWindow(const wxBitmap& bitmap,
                         wxWindow* parent, wxWindowID id,
                         const wxPoint& pos = wxDefaultPosition,
                         const wxSize& size = wxDefaultSize,
                         long style = wxNO_BORDER);

    void SetBitmap(const wxBitmap& bitmap);
    wxBitmap& GetBitmap();
};


class wxSplashScreen : public wxFrame {
public:
    wxSplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds,
                   wxWindow* parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxFRAME_FLOAT_ON_PARENT);

    long GetSplashStyle() const;
    wxSplashScreenWindow* GetSplashWindow() const;
    int GetTimeout() const;
};


//---------------------------------------------------------------------------






