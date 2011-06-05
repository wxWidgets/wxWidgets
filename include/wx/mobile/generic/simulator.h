/////////////////////////////////////////////////////////////////////////////
// Name:        wx/mobile/generic/simulator.h
// Purpose:     Simulator classes
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWidgets Licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_MOBILE_GENERIC_SIMULATOR_H_
#define _WX_MOBILE_GENERIC_SIMULATOR_H_

/*!
 * Includes
 */

////@begin includes
#include "wx/frame.h"
#include "wx/toolbar.h"
////@end includes

#include "wx/scrolwin.h"

/*
 * Forward declarations
 */

////@begin forward declarations
class wxMoSimulatorPanel;
////@end forward declarations

/*
 * Control identifiers
 */

////@begin control identifiers
#define ID_NAVIGATION_ROTATE_RIGHT 10000
#define ID_NAVIGATION_ROTATE_LEFT 10001
#define ID_NAVIGATION_HOME_SCREEN 10002
#define ID_VIEW_SHOW_SKIN 10003
#define SYMBOL_WXIPSIMULATORFRAME_STYLE wxCAPTION|wxRESIZE_BORDER|wxSYSTEM_MENU|wxMINIMIZE_BOX|wxMAXIMIZE_BOX|wxCLOSE_BOX
#define SYMBOL_WXIPSIMULATORFRAME_TITLE _("iPhoni")
#define SYMBOL_WXIPSIMULATORFRAME_IDNAME wxID_ANY
#define SYMBOL_WXIPSIMULATORFRAME_SIZE wxSize(500, 820)
#define SYMBOL_WXIPSIMULATORFRAME_POSITION wxDefaultPosition
#define SYMBOL_WXIPSIMULATORPANEL_STYLE wxNO_BORDER
#define SYMBOL_WXIPSIMULATORPANEL_IDNAME wxID_ANY
#define SYMBOL_WXIPSIMULATORPANEL_SIZE wxDefaultSize
#define SYMBOL_WXIPSIMULATORPANEL_POSITION wxDefaultPosition
////@end control identifiers


/**
    @class wxMoSimulatorFrame
    The frame representing the overall simulator.

    @category{wxMobileImplementation}
 */

class wxMoSimulatorFrame: public wxFrame
{
    DECLARE_CLASS( wxMoSimulatorFrame )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxMoSimulatorFrame();
    wxMoSimulatorFrame( wxWindow* parent, wxWindowID id = SYMBOL_WXIPSIMULATORFRAME_IDNAME, const wxString& caption = SYMBOL_WXIPSIMULATORFRAME_TITLE, const wxPoint& pos = SYMBOL_WXIPSIMULATORFRAME_POSITION, const wxSize& size = SYMBOL_WXIPSIMULATORFRAME_SIZE, long style = SYMBOL_WXIPSIMULATORFRAME_STYLE );

    bool Create( wxWindow* parent, wxWindowID id = SYMBOL_WXIPSIMULATORFRAME_IDNAME, const wxString& caption = SYMBOL_WXIPSIMULATORFRAME_TITLE, const wxPoint& pos = SYMBOL_WXIPSIMULATORFRAME_POSITION, const wxSize& size = SYMBOL_WXIPSIMULATORFRAME_SIZE, long style = SYMBOL_WXIPSIMULATORFRAME_STYLE );

    /// Destructor
    ~wxMoSimulatorFrame();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    /// Returns the simulator panel
    wxMoSimulatorPanel* GetSimulatorPanel() const { return m_simulatorPanel; }

    bool ShowSkin(bool show);

    bool LoadSettings(const wxString& appName = wxT("iPhoni"));
    bool SaveSettings(const wxString& appName = wxT("iPhoni"));

    /// Get simulator frame
    static wxMoSimulatorFrame* GetSimulatorFrame() { return sm_simulatorFrame; }

    /// Set simulator frame
    static void SetSimulatorFrame(wxMoSimulatorFrame* frame) { sm_simulatorFrame = frame; }

////@begin wxMoSimulatorFrame event handler declarations

    /// wxEVT_CLOSE_WINDOW event handler for wxID_ANY
    void OnCloseWindow( wxCloseEvent& event );

#if defined(__WXMAC__)
    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_ABOUT
    void OnAboutClick( wxCommandEvent& event );

#endif
    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_NAVIGATION_ROTATE_RIGHT
    void OnNavigationRotateRightClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_NAVIGATION_ROTATE_LEFT
    void OnNavigationRotateLeftClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_NAVIGATION_HOME_SCREEN
    void OnNavigationHomeScreenClick( wxCommandEvent& event );

    /// wxEVT_COMMAND_MENU_SELECTED event handler for ID_VIEW_SHOW_SKIN
    void OnViewShowSkinClick( wxCommandEvent& event );

    /// wxEVT_UPDATE_UI event handler for ID_VIEW_SHOW_SKIN
    void OnViewShowSkinUpdate( wxUpdateUIEvent& event );

#if defined(__WXGTK__) || defined(__WXMSW__)
    /// wxEVT_COMMAND_MENU_SELECTED event handler for wxID_ABOUT
    void OnAboutClick( wxCommandEvent& event );

#endif
////@end wxMoSimulatorFrame event handler declarations

////@begin wxMoSimulatorFrame member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxMoSimulatorFrame member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxMoSimulatorFrame member variables
    wxScrolledWindow* m_scrolledWindow;
    wxMoSimulatorPanel* m_simulatorPanel;
////@end wxMoSimulatorFrame member variables

    static wxMoSimulatorFrame* sm_simulatorFrame;
};

/**
    @class wxMoSimulatorStatusBar

    A window representing the status bar at the top of the iPhone screen.

    @category{wxMobileImplementation}
 */

class wxMoSimulatorStatusBar: public wxWindow
{
    DECLARE_DYNAMIC_CLASS( wxMoSimulatorStatusBar )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxMoSimulatorStatusBar();
    wxMoSimulatorStatusBar(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER);

    void Init();

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER);

    /// Destructor
    ~wxMoSimulatorStatusBar();

    bool LoadImages();

    void OnSize( wxSizeEvent& event );
    void OnPaint( wxPaintEvent& event );

    virtual bool AcceptsFocus() const { return false; }

protected:
    wxBitmap    m_logoBitmap;
    wxBitmap    m_batteryBitmap;
};


/**
    @class wxMoSimulatorScreenWindow

    This is the panel representing the screen window, the parent for the application window.

    @category{wxMobileImplementation}
 */

class wxMoSimulatorScreenWindow: public wxWindow
{
    DECLARE_DYNAMIC_CLASS( wxMoSimulatorScreenWindow )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxMoSimulatorScreenWindow();
    wxMoSimulatorScreenWindow(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Destructor
    ~wxMoSimulatorScreenWindow();

    void SetApplicationWindow(wxWindow* win) { m_applicationWindow = win; }
    wxWindow* GetApplicationWindow() const { return m_applicationWindow; }

    void SetStatusBar(wxMoSimulatorStatusBar* win) { m_statusBar = win; }
    wxMoSimulatorStatusBar* GetStatusBar() const { return m_statusBar; }

    /// Finds the simulator screen window parent of the given window.
    static wxMoSimulatorScreenWindow* FindSimulatorScreenWindow(wxWindow* child);

    /// The current screen window can be set to different values if the
    /// there are multiple ones in the same app.
    static void SetCurrentScreenWindow(wxMoSimulatorScreenWindow* win) { sm_currentScreenWindow = win; }
    static wxMoSimulatorScreenWindow* GetCurrentScreenWindow() { return sm_currentScreenWindow; }

    /// Initialises member variables
    void Init();

    void OnSize( wxSizeEvent& event );

    /// Set the application window
    void SizeApplicationWindow();

    virtual bool AcceptsFocus() const { return false; }

protected:

    wxWindow*                   m_applicationWindow;
    wxMoSimulatorStatusBar*     m_statusBar;
    static wxMoSimulatorScreenWindow* sm_currentScreenWindow;
};


/**
    @class wxMoSimulatorPanel

    The class containing the simulated screen and the skin images.

    @category{wxMobileImplementation}
 */

class wxMoSimulatorPanel: public wxWindow
{
    DECLARE_DYNAMIC_CLASS( wxMoSimulatorPanel )
    DECLARE_EVENT_TABLE()

public:
    /// Constructors
    wxMoSimulatorPanel();
    wxMoSimulatorPanel(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Creation
    bool Create(wxWindow* parent, wxWindowID id = wxID_ANY, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = wxNO_BORDER|wxTAB_TRAVERSAL);

    /// Destructor
    ~wxMoSimulatorPanel();

    /// Initialises member variables
    void Init();

    /// Creates the controls and sizers
    void CreateControls();

    /// Load images
    bool LoadImages();

    /// Set current image (0 - 3)
    bool SetRotation(int image, bool refresh = true);

    /// Rotate right
    bool RotateRight();

    /// Rotate left
    bool RotateLeft();

    /// Go home
    bool GoHome();

    /// Get the screen window
    wxMoSimulatorScreenWindow* GetScreenWindow() const { return m_screenWindow; }

    /// Do layout
    void DoLayout();

    /// Show skin
    bool ShowSkin(bool showSkin);

    virtual bool AcceptsFocus() const { return false; }

    bool GetUseSkin() const { return m_useSkin; }
    void SetUseSkin(bool useSkin) { m_useSkin = useSkin; }

////@begin wxMoSimulatorPanel event handler declarations

    /// wxEVT_SIZE event handler for wxID_ANY
    void OnSize( wxSizeEvent& event );

    /// wxEVT_PAINT event handler for wxID_ANY
    void OnPaint( wxPaintEvent& event );

////@end wxMoSimulatorPanel event handler declarations

////@begin wxMoSimulatorPanel member function declarations

    /// Retrieves bitmap resources
    wxBitmap GetBitmapResource( const wxString& name );

    /// Retrieves icon resources
    wxIcon GetIconResource( const wxString& name );
////@end wxMoSimulatorPanel member function declarations

    /// Should we show tooltips?
    static bool ShowToolTips();

////@begin wxMoSimulatorPanel member variables
////@end wxMoSimulatorPanel member variables

    // Phone images
    wxBitmap    m_images[4];

    // Screen rects for images
    wxRect      m_screenRects[4];

    // Screen rects for no-skin mode
    wxRect      m_noSkinRects[4];

    // Current image
    int         m_currentImage;

    // Screen window
    wxMoSimulatorScreenWindow*   m_screenWindow;

    bool        m_useSkin;
};

#endif
    // _WX_MOBILE_GENERIC_SIMULATOR_H_
