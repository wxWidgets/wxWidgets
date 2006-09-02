/////////////////////////////////////////////////////////////////////////////
// Name:        _toplvl.i
// Purpose:     SWIG definitions for wxTopLevelWindow, wxFrame, wxDialog and etc.
//
// Author:      Robin Dunn
//
// Created:     27-Aug-1998
// RCS-ID:      $Id$
// Copyright:   (c) 2003 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// Not a %module


//---------------------------------------------------------------------------

MAKE_CONST_WXSTRING(FrameNameStr);
MAKE_CONST_WXSTRING(DialogNameStr);
MAKE_CONST_WXSTRING(StatusLineNameStr);
MAKE_CONST_WXSTRING(ToolBarNameStr);

//---------------------------------------------------------------------------
%newgroup

enum
{
    wxSTAY_ON_TOP,
    wxICONIZE,
    wxMINIMIZE,
    wxMAXIMIZE,
    wxCLOSE_BOX,
    wxTHICK_FRAME,
    wxSYSTEM_MENU,
    wxMINIMIZE_BOX,
    wxMAXIMIZE_BOX,
    wxTINY_CAPTION_HORIZ,
    wxTINY_CAPTION_VERT,
    wxRESIZE_BOX,
    wxRESIZE_BORDER,

    wxDIALOG_NO_PARENT,
    
    wxDEFAULT_FRAME_STYLE,
    wxDEFAULT_DIALOG_STYLE,

    wxFRAME_TOOL_WINDOW,
    wxFRAME_FLOAT_ON_PARENT,
    wxFRAME_NO_WINDOW_MENU,
    wxFRAME_NO_TASKBAR,
    wxFRAME_SHAPED,
    wxFRAME_DRAWER,

    wxFRAME_EX_METAL,
    wxDIALOG_EX_METAL,
    wxWS_EX_CONTEXTHELP,
    
    // Obsolete
    wxDIALOG_MODAL,
    wxDIALOG_MODELESS,
    wxUSER_COLOURS,
    wxNO_3D,

    wxFRAME_EX_CONTEXTHELP,
    wxDIALOG_EX_CONTEXTHELP,
};    


enum
{
    wxFULLSCREEN_NOMENUBAR,
    wxFULLSCREEN_NOTOOLBAR,
    wxFULLSCREEN_NOSTATUSBAR,
    wxFULLSCREEN_NOBORDER,
    wxFULLSCREEN_NOCAPTION,
    wxFULLSCREEN_ALL,

    wxTOPLEVEL_EX_DIALOG,
};

// Styles for RequestUserAttention
enum
{
    wxUSER_ATTENTION_INFO = 1,
    wxUSER_ATTENTION_ERROR = 2
};

//---------------------------------------------------------------------------

class  wxTopLevelWindow : public wxWindow
{
public:

    // No constructor as it can not be used directly from Python

    // maximize = True => maximize, otherwise - restore
    virtual void Maximize(bool maximize = true);

    // undo Maximize() or Iconize()
    virtual void Restore();

    // iconize = True => iconize, otherwise - restore
    virtual void Iconize(bool iconize = true);

    // return True if the frame is maximized
    virtual bool IsMaximized() const;

    // return true if the frame is always maximized
    // due to native guidelines or current policy
    virtual bool IsAlwaysMaximized() const;
    
    // return True if the frame is iconized
    virtual bool IsIconized() const;

    // get the frame icon
    wxIcon GetIcon() const;

    // set the frame icon
    virtual void SetIcon(const wxIcon& icon);

    // set the frame icons
    virtual void SetIcons(const wxIconBundle& icons);

    // maximize the window to cover entire screen
    virtual bool ShowFullScreen(bool show, long style = wxFULLSCREEN_ALL);

    // return True if the frame is in fullscreen mode
    virtual bool IsFullScreen() const;

    virtual void SetTitle(const wxString& title);
    virtual wxString GetTitle() const;

    // Set the shape of the window to the given region.
    // Returns True if the platform supports this feature
    // (and the operation is successful.)
    virtual bool SetShape(const wxRegion& region);

    // Attracts the users attention to this window if the application is inactive
    // (should be called when a background event occurs)
    virtual void RequestUserAttention(int flags = wxUSER_ATTENTION_INFO);

    // Is this the active frame (highlighted in the taskbar)?
    virtual bool IsActive();

#ifdef __WXMAC__
    %extend {
        void MacSetMetalAppearance( bool on ) {
            int style = self->GetExtraStyle();
            if ( on )
                style |= wxFRAME_EX_METAL;
            else
                style &= ~wxFRAME_EX_METAL;
            self->SetExtraStyle(style);
        }
    }
    bool MacGetMetalAppearance() const;
#else
    %extend
    {
        // TODO: Should they raise not implemented or just NOP???
        void MacSetMetalAppearance( bool on ) { /*wxPyRaiseNotImplemented();*/ }
        bool MacGetMetalAppearance() const    { /*wxPyRaiseNotImplemented();*/ return false; }
    }
#endif

    DocDeclStr(
        void , CenterOnScreen(int dir = wxBOTH),
        "Center the window on screen", "");
    %pythoncode { CentreOnScreen = CenterOnScreen }

#ifdef __WXMSW__
    bool EnableCloseButton(bool enable = true);
#else
    %extend {
        bool EnableCloseButton(bool enable = true) { return false; }
    }
#endif

    
    
    DocDeclStr(
        virtual wxWindow *, GetDefaultItem() const,
        "Get the default child of this parent, i.e. the one which is activated
by pressing <Enter> such as the OK button on a wx.Dialog.", "");
    
    DocDeclStr(
        virtual wxWindow *, SetDefaultItem(wxWindow * child),
        "Set this child as default, return the old default.", "");
    
    DocDeclStr(
        virtual void , SetTmpDefaultItem(wxWindow * win),
        "Set this child as temporary default", "");

    DocDeclStr(
        virtual wxWindow *, GetTmpDefaultItem() const,
        "Return the temporary default item, which can be None.", "");
       
};


//---------------------------------------------------------------------------
%newgroup

// wxFrame is a top-level window with optional menubar, statusbar and toolbar
//
// For each of *bars, a frame may have several of them, but only one is
// managed by the frame, i.e. resized/moved when the frame is and whose size
// is accounted for in client size calculations - all others should be taken
// care of manually.

MustHaveApp(wxFrame);

class wxFrame : public wxTopLevelWindow
{
public:
    %pythonAppend wxFrame         "self._setOORInfo(self)"
    %pythonAppend wxFrame()       ""
    %typemap(out) wxFrame*;    // turn off this typemap

    wxFrame(wxWindow* parent,
            const wxWindowID id = -1,
            const wxString& title = wxPyEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxPyFrameNameStr);
    %RenameCtor(PreFrame, wxFrame());

    // Turn it back on again
    %typemap(out) wxFrame* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxWindow* parent,
            const wxWindowID id = -1,
            const wxString& title = wxPyEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxPyFrameNameStr);

    // frame state
    // -----------

    // get the origin of the client area (which may be different from (0, 0)
    // if the frame has a toolbar) in client coordinates
    virtual wxPoint GetClientAreaOrigin() const;

    // sends a size event to the window using its current size:
    //  this has a side effect of refreshing the window layout
    virtual void SendSizeEvent();

    // menu bar functions
    // ------------------

    virtual void SetMenuBar(wxMenuBar *menubar);
    virtual wxMenuBar *GetMenuBar() const;

    // process menu command: returns True if processed
    bool ProcessCommand(int winid);
    %pythoncode { Command = ProcessCommand }

    // status bar functions
    // --------------------

    // create the main status bar by calling OnCreateStatusBar()
    virtual wxStatusBar* CreateStatusBar(int number = 1,
                                         long style = wxDEFAULT_STATUSBAR_STYLE,
                                         wxWindowID winid = 0,
                                         const wxString& name = wxPyStatusLineNameStr);

// TODO: with directors?
//     // return a new status bar
//     virtual wxStatusBar *OnCreateStatusBar(int number,
//                                            long style,
//                                            wxWindowID winid,
//                                            const wxString& name);

    // get the main status bar
    virtual wxStatusBar *GetStatusBar() const;

    // sets the main status bar
    void SetStatusBar(wxStatusBar *statBar);

    // forward these to status bar
    virtual void SetStatusText(const wxString &text, int number = 0);
    virtual void SetStatusWidths(int widths, const int* widths_field); // uses typemap above
    void PushStatusText(const wxString &text, int number = 0);
    void PopStatusText(int number = 0);

    // set the status bar pane the help will be shown in
    void SetStatusBarPane(int n);
    int GetStatusBarPane() const;

    // toolbar functions
    // -----------------

    // create main toolbar bycalling OnCreateToolBar()
    virtual wxToolBar* CreateToolBar(long style = -1,
                                     wxWindowID winid = -1,
                                     const wxString& name = wxPyToolBarNameStr);

// TODO: with directors?
//     // return a new toolbar
//     virtual wxToolBar *OnCreateToolBar(long style,
//                                        wxWindowID winid,
//                                        const wxString& name );

    // get/set the main toolbar
    virtual wxToolBar *GetToolBar() const;
    virtual void SetToolBar(wxToolBar *toolbar);

    // show help text (typically in the statusbar); show is False
    // if you are hiding the help, True otherwise
    virtual void DoGiveHelp(const wxString& text, bool show);

    // send wxUpdateUIEvents for all menu items in the menubar,
    // or just for menu if non-NULL
    void DoMenuUpdates(wxMenu* menu = NULL);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);
};

//---------------------------------------------------------------------------
%newgroup

MustHaveApp(wxDialog);

class wxDialog : public wxTopLevelWindow
{
public:
    enum
    {
        // all flags allowed in wxDialogBase::CreateButtonSizer()
        ButtonSizerFlags = wxOK | wxCANCEL | wxYES | wxNO | wxHELP | wxNO_DEFAULT
    };

    %pythonAppend wxDialog   "self._setOORInfo(self)"
    %pythonAppend wxDialog() ""
    %typemap(out) wxDialog*;    // turn off this typemap

    wxDialog(wxWindow* parent,
             const wxWindowID id = -1,
             const wxString& title = wxPyEmptyString,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxDEFAULT_DIALOG_STYLE,
             const wxString& name = wxPyDialogNameStr);
    %RenameCtor(PreDialog, wxDialog());

    // Turn it back on again
    %typemap(out) wxDialog* { $result = wxPyMake_wxObject($1, $owner); }

    bool Create(wxWindow* parent,
                const wxWindowID id = -1,
                const wxString& title = wxPyEmptyString,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxDEFAULT_DIALOG_STYLE,
                const wxString& name = wxPyDialogNameStr);

    // the modal dialogs have a return code - usually the ID of the last
    // pressed button
    void SetReturnCode(int returnCode);
    int GetReturnCode() const;

    // Set the identifier for the affirmative button: this button will close
    // the dialog after validating data and calling TransferDataFromWindow()
    void SetAffirmativeId(int affirmativeId);
    int GetAffirmativeId() const;

    // Set identifier for Esc key translation: the button with this id will
    // close the dialog without doing anything else; special value wxID_NONE
    // means to not handle Esc at all while wxID_ANY means to map Esc to
    // wxID_CANCEL if present and GetAffirmativeId() otherwise
    void SetEscapeId(int escapeId);
    int GetEscapeId() const;

    // splits text up at newlines and places the
    // lines into a vertical wxBoxSizer
    wxSizer* CreateTextSizer( const wxString &message );

    // places buttons into a horizontal wxBoxSizer
    wxSizer* CreateButtonSizer( long flags,
                                bool separated = false,
                                wxCoord distance = 0 );
    wxStdDialogButtonSizer* CreateStdDialogButtonSizer( long flags );

    //void SetModal( bool flag );

    // is the dialog in modal state right now?
    virtual bool IsModal() const;

    // Shows the dialog and starts a nested event loop that returns when
    // EndModal is called.
    virtual int ShowModal();

    // may be called to terminate the dialog with the given return code
    virtual void EndModal(int retCode);

    static wxVisualAttributes
    GetClassDefaultAttributes(wxWindowVariant variant = wxWINDOW_VARIANT_NORMAL);

    %property(AffirmativeId, GetAffirmativeId, SetAffirmativeId, doc="See `GetAffirmativeId` and `SetAffirmativeId`");
    %property(EscapeId, GetEscapeId, SetEscapeId, doc="See `GetEscapeId` and `SetEscapeId`");
    %property(ReturnCode, GetReturnCode, SetReturnCode, doc="See `GetReturnCode` and `SetReturnCode`");
};

//---------------------------------------------------------------------------
%newgroup


MustHaveApp(wxMiniFrame);

class wxMiniFrame : public wxFrame
{
public:
    %pythonAppend wxMiniFrame         "self._setOORInfo(self)"
    %pythonAppend wxMiniFrame()       ""

    wxMiniFrame(wxWindow* parent,
            const wxWindowID id = -1,
            const wxString& title = wxPyEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxPyFrameNameStr);
    %RenameCtor(PreMiniFrame, wxMiniFrame());

    bool Create(wxWindow* parent,
            const wxWindowID id = -1,
            const wxString& title = wxPyEmptyString,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE,
            const wxString& name = wxPyFrameNameStr);
};


//---------------------------------------------------------------------------
%newgroup


enum
{
    wxSPLASH_CENTRE_ON_PARENT,
    wxSPLASH_CENTRE_ON_SCREEN,
    wxSPLASH_NO_CENTRE,
    wxSPLASH_TIMEOUT,
    wxSPLASH_NO_TIMEOUT,
};


MustHaveApp(wxSplashScreenWindow);

class wxSplashScreenWindow: public wxWindow
{
public:
    %pythonAppend wxSplashScreenWindow         "self._setOORInfo(self)"

    wxSplashScreenWindow(const wxBitmap& bitmap,
             wxWindow* parent,
             wxWindowID id,
             const wxPoint& pos = wxDefaultPosition,
             const wxSize& size = wxDefaultSize,
             long style = wxNO_BORDER);

    void SetBitmap(const wxBitmap& bitmap);
    wxBitmap& GetBitmap();
};


MustHaveApp(wxSplashScreen);

class wxSplashScreen : public wxFrame
{
public:
    %pythonAppend wxSplashScreen         "self._setOORInfo(self)"

    wxSplashScreen(const wxBitmap& bitmap,
            long splashStyle, int milliseconds,
            wxWindow* parent,
            wxWindowID id = -1,
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxSIMPLE_BORDER | wxFRAME_NO_TASKBAR | wxSTAY_ON_TOP);

    long GetSplashStyle() const;
    wxSplashScreenWindow* GetSplashWindow() const;
    int GetTimeout() const;
};


//---------------------------------------------------------------------------
