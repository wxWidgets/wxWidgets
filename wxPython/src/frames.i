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

%{
    // Put some wx default wxChar* values into wxStrings.
    DECLARE_DEF_STRING(FrameNameStr);
    DECLARE_DEF_STRING(DialogNameStr);
    DECLARE_DEF_STRING(StatusLineNameStr);
    DECLARE_DEF_STRING(ToolBarNameStr);
%}

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
                     const wxString& name = wxPyFrameNameStr);
    %name(wxPreTopLevelWindow)wxTopLevelWindow();

    bool Create(wxWindow *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxPyFrameNameStr);

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

    // set the frame icons
    virtual void SetIcons(const wxIconBundle& icons );

    // maximize the window to cover entire screen
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);

    // return TRUE if the frame is in fullscreen mode
    virtual bool IsFullScreen() const;

    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;

    // Set the shape of the window to the given region.
    // Returns TRUE if the platform supports this feature (and the operation
    // is successful.)
    virtual bool SetShape(const wxRegion& region);

};

//----------------------------------------------------------------------


class wxFrame : public wxTopLevelWindow {
public:
    wxFrame(wxWindow* parent, const wxWindowID id, const wxString& title,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxPyFrameNameStr);
    %name(wxPreFrame)wxFrame();

    bool Create(wxWindow* parent, const wxWindowID id, const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxPyFrameNameStr);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"
    %pragma(python) addtomethod = "wxPreFrame:val._setOORInfo(val)"

    wxPoint GetClientAreaOrigin();

    void SetMenuBar(wxMenuBar *menubar);
    wxMenuBar *GetMenuBar();


    // process menu command: returns TRUE if processed
    bool ProcessCommand(int id);
    %pragma(python) addtoclass = "Command = ProcessCommand"

    // create the main status bar
    wxStatusBar* CreateStatusBar(int number = 1,
                                 long style = wxST_SIZEGRIP,
                                 wxWindowID id = -1,
                                 const wxString& name = wxPyStatusLineNameStr);

    // get the main status bar
    wxStatusBar *GetStatusBar();

    // sets the main status bar
    void SetStatusBar(wxStatusBar *statBar);

    // forward these to status bar
    virtual void SetStatusText(const wxString &text, int number = 0);
    virtual void SetStatusWidths(int LCOUNT, int* choices); // uses typemap
    void PushStatusText(const wxString &text, int number = 0);
    void PopStatusText(int number = 0);

    // show help text (typically in the statusbar); show is FALSE
    // if you are hiding the help, TRUE otherwise
    virtual void DoGiveHelp(const wxString& text, bool show);

    // set the status bar pane the help will be shown in
    void SetStatusBarPane(int n);
    int GetStatusBarPane() const;


    // create main toolbar
    virtual wxToolBar* CreateToolBar(long style = -1, //wxNO_BORDER|wxTB_HORIZONTAL,
                                     wxWindowID id = -1,
                                     const wxString& name = wxPyToolBarNameStr);

    // get/set the main toolbar
    virtual wxToolBar *GetToolBar() const { return m_frameToolBar; }
    virtual void SetToolBar(wxToolBar *toolbar) { m_frameToolBar = toolbar; }

    // sends a size event to the window using its current size -- this has an
    // effect of refreshing the window layout
    virtual void SendSizeEvent();


    // send wxUpdateUIEvents for all menu items in the menubar,
    // or just for menu if non-NULL
    void DoMenuUpdates(wxMenu* menu = NULL);

    // do the UI update processing for this window
    virtual void UpdateWindowUI(long flags = wxUPDATE_UI_NONE);
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
             const wxString& name = wxPyDialogNameStr);
    %name(wxPreDialog)wxDialog();

    bool Create(wxWindow* parent,
                const wxWindowID id,
                const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxPyDialogNameStr);

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
                const wxString& name = wxPyFrameNameStr);
    %name(wxPreMiniFrame)wxMiniFrame();

    bool Create(wxWindow* parent, const wxWindowID id, const wxString& title,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_FRAME_STYLE,
                const wxString& name = wxPyFrameNameStr);

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

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    void SetBitmap(const wxBitmap& bitmap);
    wxBitmap& GetBitmap();
};


class wxSplashScreen : public wxFrame {
public:
    wxSplashScreen(const wxBitmap& bitmap, long splashStyle, int milliseconds,
                   wxWindow* parent, wxWindowID id,
                   const wxPoint& pos = wxDefaultPosition,
                   const wxSize& size = wxDefaultSize,
                   long style = wxSIMPLE_BORDER|wxFRAME_NO_TASKBAR|wxSTAY_ON_TOP);

    %pragma(python) addtomethod = "__init__:self._setOORInfo(self)"

    long GetSplashStyle() const;
    wxSplashScreenWindow* GetSplashWindow() const;
    int GetTimeout() const;
};


//---------------------------------------------------------------------------






