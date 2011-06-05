/////////////////////////////////////////////////////////////////////////////
// Name:        src/mobile/generic/mo_simulator_g.cpp
// Purpose:     Simulator classes
// Author:      Julian Smart
// Modified by:
// Created:     12/05/2009 08:59:11
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "wx/display.h"
#include "wx/aboutdlg.h"
#include "wx/config.h"
#include "wx/mstream.h"

////@begin includes
////@end includes

#include "wx/mobile/generic/simulator.h"
#include "wx/mobile/generic/keyboard.h"
#include "wx/mobile/generic/bitmaps/iphone.inc"
#include "wx/mobile/generic/bitmaps/iphone_icon.xpm"
#include "wx/mobile/generic/bitmaps/iphonebattery.xpm"

#if 0
////@begin XPM images
#include "../../include/wx/mobile/generic/bitmaps/rotate_clock.xpm"
#include "../../include/wx/mobile/generic/bitmaps/rotate_anticlock.xpm"
////@end XPM images
#endif

#include "wx/mobile/generic/bitmaps/rotate_clock.xpm"
#include "wx/mobile/generic/bitmaps/rotate_anticlock.xpm"

/*
 * wxMoSimulatorFrame type definition
 */

IMPLEMENT_CLASS( wxMoSimulatorFrame, wxFrame )


/*
 * wxMoSimulatorFrame event table definition
 */

BEGIN_EVENT_TABLE( wxMoSimulatorFrame, wxFrame )

////@begin wxMoSimulatorFrame event table entries
    EVT_CLOSE( wxMoSimulatorFrame::OnCloseWindow )

#if defined(__WXMAC__)
    EVT_MENU( wxID_ABOUT, wxMoSimulatorFrame::OnAboutClick )
#endif

    EVT_MENU( ID_NAVIGATION_ROTATE_RIGHT, wxMoSimulatorFrame::OnNavigationRotateRightClick )

    EVT_MENU( ID_NAVIGATION_ROTATE_LEFT, wxMoSimulatorFrame::OnNavigationRotateLeftClick )

    EVT_MENU( ID_NAVIGATION_HOME_SCREEN, wxMoSimulatorFrame::OnNavigationHomeScreenClick )

    EVT_MENU( ID_VIEW_SHOW_SKIN, wxMoSimulatorFrame::OnViewShowSkinClick )
    EVT_UPDATE_UI( ID_VIEW_SHOW_SKIN, wxMoSimulatorFrame::OnViewShowSkinUpdate )

#if defined(__WXGTK__) || defined(__WXMSW__)
    EVT_MENU( wxID_ABOUT, wxMoSimulatorFrame::OnAboutClick )
#endif

////@end wxMoSimulatorFrame event table entries

END_EVENT_TABLE()


/*
 * wxMoSimulatorFrame constructors
 */

wxMoSimulatorFrame::wxMoSimulatorFrame()
{
    Init();
}

wxMoSimulatorFrame::wxMoSimulatorFrame( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    Init();
    Create( parent, id, caption, pos, size, style );
}


/*
 * wxMoSimulatorFrame creator
 */

bool wxMoSimulatorFrame::Create( wxWindow* parent, wxWindowID id, const wxString& caption, const wxPoint& pos, const wxSize& size, long style )
{
    wxFrame::Create( parent, id, caption, pos, size, style );

    CreateControls();
    LoadSettings();

    SetIcon(GetIconResource(wxT("../../include/wx/mobile/generic/iphone_icon.xpm")));
    Centre();

    m_simulatorPanel->SetRotation(0);


    return true;
}


/*
 * wxMoSimulatorFrame destructor
 */

wxMoSimulatorFrame* wxMoSimulatorFrame::sm_simulatorFrame = NULL;

wxMoSimulatorFrame::~wxMoSimulatorFrame()
{
    wxMoSimulatorFrame::SetSimulatorFrame(NULL);

////@begin wxMoSimulatorFrame destruction
////@end wxMoSimulatorFrame destruction
}


/*
 * Member initialisation
 */

void wxMoSimulatorFrame::Init()
{
    wxMoSimulatorFrame::SetSimulatorFrame(this);

////@begin wxMoSimulatorFrame member initialisation
    m_scrolledWindow = NULL;
    m_simulatorPanel = NULL;
////@end wxMoSimulatorFrame member initialisation
}


/*
 * Control creation for wxMoSimulatorFrame
 */

void wxMoSimulatorFrame::CreateControls()
{
////@begin wxMoSimulatorFrame content construction
    wxMoSimulatorFrame* itemFrame1 = this;

    wxMenuBar* menuBar = new wxMenuBar;
    wxMenu* itemMenu7 = new wxMenu;
    itemMenu7->Append(wxID_EXIT, _("&Exit"), wxEmptyString, wxITEM_NORMAL);
#if defined(__WXMAC__)
    itemMenu7->Append(wxID_ABOUT, _("&About"), wxEmptyString, wxITEM_NORMAL);
#endif
    menuBar->Append(itemMenu7, _("&File"));
    wxMenu* itemMenu10 = new wxMenu;
    itemMenu10->Append(ID_NAVIGATION_ROTATE_RIGHT, _("Rotate &Right\tCtrl+Right"), wxEmptyString, wxITEM_NORMAL);
    itemMenu10->Append(ID_NAVIGATION_ROTATE_LEFT, _("Rotate &Left\tCtrl+Left"), wxEmptyString, wxITEM_NORMAL);
    itemMenu10->Append(ID_NAVIGATION_HOME_SCREEN, _("&Home Screen\tCtrl+Home"), wxEmptyString, wxITEM_NORMAL);
    itemMenu10->AppendSeparator();
    itemMenu10->Append(ID_VIEW_SHOW_SKIN, _("Show &Skin"), _("Check to show the phone skin."), wxITEM_CHECK);
    menuBar->Append(itemMenu10, _("&View"));
#if defined(__WXGTK__) || defined(__WXMSW__)
    wxMenu* itemMenu16 = new wxMenu;
#if defined(__WXGTK__) || defined(__WXMSW__)
    itemMenu16->Append(wxID_ABOUT, _("&About"), wxEmptyString, wxITEM_NORMAL);
#endif
    menuBar->Append(itemMenu16, _("&Help"));
#endif
    itemFrame1->SetMenuBar(menuBar);

    wxBoxSizer* itemBoxSizer2 = new wxBoxSizer(wxVERTICAL);
    itemFrame1->SetSizer(itemBoxSizer2);

    m_scrolledWindow = new wxScrolledWindow( itemFrame1, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxSIMPLE_BORDER|wxHSCROLL|wxVSCROLL );
    itemBoxSizer2->Add(m_scrolledWindow, 1, wxGROW, 5);
    m_scrolledWindow->SetScrollbars(1, 1, 0, 0);
    wxBoxSizer* itemBoxSizer4 = new wxBoxSizer(wxVERTICAL);
    m_scrolledWindow->SetSizer(itemBoxSizer4);

    m_simulatorPanel = new wxMoSimulatorPanel( m_scrolledWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNO_BORDER );
    m_simulatorPanel->SetBackgroundColour(wxColour(0, 0, 0));
    itemBoxSizer4->Add(m_simulatorPanel, 1, wxGROW, 5);

    m_scrolledWindow->SetMinSize(wxSize(100, 100));

    wxToolBar* itemToolBar18 = CreateToolBar( wxTB_FLAT|wxTB_HORIZONTAL, wxID_ANY );
    itemToolBar18->SetToolBitmapSize(wxSize(24, 24));
    wxBitmap itemtool19Bitmap(itemFrame1->GetBitmapResource(wxT("../../include/wx/mobile/generic/bitmaps/rotate_clock.png")));
    wxBitmap itemtool19BitmapDisabled;
    itemToolBar18->AddTool(ID_NAVIGATION_ROTATE_RIGHT, wxEmptyString, itemtool19Bitmap, itemtool19BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    wxBitmap itemtool20Bitmap(itemFrame1->GetBitmapResource(wxT("../../include/wx/mobile/generic/bitmaps/rotate_anticlock.png")));
    wxBitmap itemtool20BitmapDisabled;
    itemToolBar18->AddTool(ID_NAVIGATION_ROTATE_LEFT, wxEmptyString, itemtool20Bitmap, itemtool20BitmapDisabled, wxITEM_NORMAL, wxEmptyString, wxEmptyString);
    itemToolBar18->Realize();
    itemFrame1->SetToolBar(itemToolBar18);

////@end wxMoSimulatorFrame content construction
}


/*
 * Should we show tooltips?
 */

bool wxMoSimulatorFrame::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap wxMoSimulatorFrame::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxMoSimulatorFrame bitmap retrieval
    wxUnusedVar(name);
    if (name == _T("../../include/wx/mobile/generic/bitmaps/rotate_clock.png"))
    {
        wxBitmap bitmap(rotate_clock_xpm);
        return bitmap;
    }
    else if (name == _T("../../include/wx/mobile/generic/bitmaps/rotate_anticlock.png"))
    {
        wxBitmap bitmap(rotate_anticlock_xpm);
        return bitmap;
    }
    return wxNullBitmap;
////@end wxMoSimulatorFrame bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon wxMoSimulatorFrame::GetIconResource( const wxString& WXUNUSED(name) )
{
    // Icon retrieval
    wxIcon icon(iphone_icon_xpm);
    return icon;
////@end wxMoSimulatorFrame icon retrieval
}


/*
 * wxMoSimulatorPanel type definition
 */

IMPLEMENT_DYNAMIC_CLASS( wxMoSimulatorPanel, wxWindow )


/*
 * wxMoSimulatorPanel event table definition
 */

BEGIN_EVENT_TABLE( wxMoSimulatorPanel, wxWindow )

////@begin wxMoSimulatorPanel event table entries
    EVT_SIZE( wxMoSimulatorPanel::OnSize )
    EVT_PAINT( wxMoSimulatorPanel::OnPaint )

////@end wxMoSimulatorPanel event table entries

END_EVENT_TABLE()


/*
 * wxMoSimulatorPanel constructors
 */

wxMoSimulatorPanel::wxMoSimulatorPanel()
{
    Init();
}

wxMoSimulatorPanel::wxMoSimulatorPanel(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}


/*
 * wxMoSimulatorPanel creator
 */

bool wxMoSimulatorPanel::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
////@begin wxMoSimulatorPanel creation
    wxWindow::Create(parent, id, pos, size, style);
    CreateControls();
////@end wxMoSimulatorPanel creation
    return true;
}


/*
 * wxMoSimulatorPanel destructor
 */

wxMoSimulatorPanel::~wxMoSimulatorPanel()
{
////@begin wxMoSimulatorPanel destruction
////@end wxMoSimulatorPanel destruction
}


/*
 * Member initialisation
 */

void wxMoSimulatorPanel::Init()
{
    m_useSkin = true;
    m_currentImage = 0;
    m_screenWindow = NULL;

////@begin wxMoSimulatorPanel member initialisation
////@end wxMoSimulatorPanel member initialisation
}

/*
 * Control creation for wxMoSimulatorPanel
 */

void wxMoSimulatorPanel::CreateControls()
{
////@begin wxMoSimulatorPanel content construction
    this->SetBackgroundColour(wxColour(0, 0, 0));
////@end wxMoSimulatorPanel content construction

    m_screenWindow = new wxMoSimulatorScreenWindow(this, wxID_ANY);

    LoadImages();
}


/*
 * Should we show tooltips?
 */

bool wxMoSimulatorPanel::ShowToolTips()
{
    return true;
}

/*
 * Get bitmap resources
 */

wxBitmap wxMoSimulatorPanel::GetBitmapResource( const wxString& name )
{
    // Bitmap retrieval
////@begin wxMoSimulatorPanel bitmap retrieval
    wxUnusedVar(name);
    return wxNullBitmap;
////@end wxMoSimulatorPanel bitmap retrieval
}

/*
 * Get icon resources
 */

wxIcon wxMoSimulatorPanel::GetIconResource( const wxString& name )
{
    // Icon retrieval
////@begin wxMoSimulatorPanel icon retrieval
    wxUnusedVar(name);
    return wxNullIcon;
////@end wxMoSimulatorPanel icon retrieval
}


/*
 * wxEVT_SIZE event handler for wxID_ANY
 */

void wxMoSimulatorPanel::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    DoLayout();
}

// Do layout
void wxMoSimulatorPanel::DoLayout()
{
    if (m_screenWindow)
    {
        if (GetUseSkin() && m_images[m_currentImage].Ok())
            m_screenWindow->SetSize(m_screenRects[m_currentImage]);
        else
            m_screenWindow->SetSize(GetClientSize());
        m_screenWindow->SizeApplicationWindow();
    }
}


/*
 * wxEVT_PAINT event handler for wxID_ANY
 */

void wxMoSimulatorPanel::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);

    wxBitmap& bitmap = m_images[m_currentImage];

    if (bitmap.Ok())
        dc.DrawBitmap(bitmap, 0, 0);
}


// Set current image (0 - 3)
bool wxMoSimulatorPanel::SetRotation(int image, bool refresh)
{
    m_currentImage = image;
    if (refresh && m_images[m_currentImage].Ok())
    {
        wxBitmap bitmap;
        wxSize simulatorSize;

        if (GetUseSkin())
        {
            bitmap = m_images[m_currentImage];
            simulatorSize = wxSize(bitmap.GetWidth(), bitmap.GetHeight());
            if (m_screenWindow)
                m_screenWindow->SetSize(m_screenRects[m_currentImage]);
        }
        else
        {
            simulatorSize = m_noSkinRects[m_currentImage].GetSize();
            if (m_screenWindow)
                m_screenWindow->SetSize(m_noSkinRects[m_currentImage]);
        }

        SetSize(simulatorSize);
        SetMinSize(simulatorSize);

        wxDisplay display;
        wxSize displaySize(display.GetClientArea().GetSize());

        wxWindow* topLevelWindow = wxGetTopLevelParent(this);

        wxRect frameRect = topLevelWindow->GetRect();
        wxRect frameClientRect = topLevelWindow->GetClientRect();
        int maxWidth = displaySize.x - (frameRect.x - frameClientRect.x);
        int maxHeight = displaySize.y - (frameRect.y - frameClientRect.y);

        int w = wxMin(simulatorSize.x, maxWidth);
        int h = wxMin(simulatorSize.y, maxHeight);

        bool smallerWidth = (w != simulatorSize.x);
        bool smallerHeight = (h != simulatorSize.y);

        // Add a bit for scrollbars
        if (smallerHeight)
            w += 22;
        if (smallerWidth)
            h += 22;

        topLevelWindow->SetClientSize(w+2, h+2);

        wxScrolledWindow* parent = wxDynamicCast(GetParent(), wxScrolledWindow);
        if (parent)
        {
            if (smallerWidth || smallerHeight)
            {
                parent->SetScrollRate(1, 1);
                parent->FitInside();
            }
            else
            {
                parent->SetScrollRate(0, 0);
            }
        }

        Refresh();

        if (wxMoKeyboard::IsKeyboardShowing())
            wxMoKeyboard::GetKeyboard()->FitCurrentKeyboard();

    }
    return true;
}

// Load images
bool wxMoSimulatorPanel::LoadImages()
{
    wxImage image1;
    {
        wxMemoryInputStream is(iphone_png, sizeof(iphone_png));
        image1 = wxImage(is, wxBITMAP_TYPE_PNG, -1);
    }

    wxImage image2 = image1.Rotate90();
    wxImage image3 = image2.Rotate90();
    wxImage image4 = image3.Rotate90();

    m_images[0] = wxBitmap(image1);
    m_images[1] = wxBitmap(image2);
    m_images[2] = wxBitmap(image3);
    m_images[3] = wxBitmap(image4);

    wxSize szVert = wxSize(320, 480);
    wxSize szHoriz = wxSize(480, 320);

    wxPoint pt1(36, 133); // 1st position
    wxPoint pt2(119, 38);
    wxPoint pt3(39, 119);
    wxPoint pt4(130, 39); // 1st anticlockwise

    m_screenRects[0] = wxRect(pt1, szVert);
    m_screenRects[1] = wxRect(pt2, szHoriz);
    m_screenRects[2] = wxRect(pt3, szVert);
    m_screenRects[3] = wxRect(pt4, szHoriz);

    m_noSkinRects[0] = wxRect(wxPoint(0, 0), szVert);
    m_noSkinRects[1] = wxRect(wxPoint(0, 0), szHoriz);
    m_noSkinRects[2] = wxRect(wxPoint(0, 0), szVert);
    m_noSkinRects[3] = wxRect(wxPoint(0, 0), szHoriz);

    return true;
}

// Rotate right
bool wxMoSimulatorPanel::RotateRight()
{
    m_currentImage ++;
    if (m_currentImage > 3)
        m_currentImage = 0;
    SetRotation(m_currentImage);
    return true;
}

// Rotate left
bool wxMoSimulatorPanel::RotateLeft()
{
    m_currentImage --;
    if (m_currentImage == -1)
        m_currentImage = 3;
    SetRotation(m_currentImage);
    return true;
}

// Go home
bool wxMoSimulatorPanel::GoHome()
{
    SetRotation(0);
    return true;
}

// Show skin
bool wxMoSimulatorPanel::ShowSkin(bool showSkin)
{
    m_useSkin = showSkin;
    SetRotation(m_currentImage);
    return true;
}


/*
 * wxMoSimulatorScreenWindow class declaration
 * This is the panel representing the screen window, parent for the application window.
 */

IMPLEMENT_DYNAMIC_CLASS(wxMoSimulatorScreenWindow, wxWindow)

BEGIN_EVENT_TABLE(wxMoSimulatorScreenWindow, wxWindow)
    EVT_SIZE(wxMoSimulatorScreenWindow::OnSize)
END_EVENT_TABLE()

wxMoSimulatorScreenWindow* wxMoSimulatorScreenWindow::sm_currentScreenWindow = NULL;

wxMoSimulatorScreenWindow::wxMoSimulatorScreenWindow()
{
    Init();
}

wxMoSimulatorScreenWindow::wxMoSimulatorScreenWindow(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}

// Creation
bool wxMoSimulatorScreenWindow::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    wxWindow::Create(parent, id, pos, size, (style|wxBORDER_NONE) & ~wxTAB_TRAVERSAL);

    SetBackgroundColour(wxColour(68, 72, 250));

    m_statusBar = new wxMoSimulatorStatusBar(this, wxID_ANY, wxPoint(0, 0), wxSize(size.x, 20), wxNO_BORDER);

    return true;
}

// Destructor
wxMoSimulatorScreenWindow::~wxMoSimulatorScreenWindow()
{
    if (sm_currentScreenWindow == this)
        sm_currentScreenWindow = NULL;
}

// Initialises member variables
void wxMoSimulatorScreenWindow::Init()
{
    m_applicationWindow = NULL;
    m_statusBar = NULL;

    sm_currentScreenWindow = this;
}

// Finds the simulator screen window parent of the given window.
wxMoSimulatorScreenWindow* wxMoSimulatorScreenWindow::FindSimulatorScreenWindow(wxWindow* child)
{
    if (!child)
    {
        return sm_currentScreenWindow;
    }

    wxASSERT( child != NULL );

    wxWindow* p = child;
    while (p)
    {
        wxMoSimulatorScreenWindow* simulatorWindow = wxDynamicCast(p, wxMoSimulatorScreenWindow);

        if (simulatorWindow)
            return simulatorWindow;

        p = p->GetParent();
    }

    return NULL;
}

void wxMoSimulatorScreenWindow::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    SizeApplicationWindow();
}

// Set the application window
void wxMoSimulatorScreenWindow::SizeApplicationWindow()
{
    int x = 0;
    int y = 0;
    int width = GetSize().x;
    int height = GetSize().y;

    if (m_statusBar && m_statusBar->IsShown())
    {
        m_statusBar->SetSize(x, y, width, 20);
        y += 20;
        height -= 20;
    }

    if (m_applicationWindow)
        m_applicationWindow->SetSize(x, y, width, height);
}

/**
    wxMoSimulatorStatusBar
 */

IMPLEMENT_DYNAMIC_CLASS( wxMoSimulatorStatusBar, wxWindow )

BEGIN_EVENT_TABLE( wxMoSimulatorStatusBar, wxWindow )
    EVT_SIZE(wxMoSimulatorStatusBar::OnSize)
    EVT_PAINT(wxMoSimulatorStatusBar::OnPaint)
END_EVENT_TABLE()

wxMoSimulatorStatusBar::wxMoSimulatorStatusBar()
{
    Init();
}

// Destructor
wxMoSimulatorStatusBar::~wxMoSimulatorStatusBar()
{
}

wxMoSimulatorStatusBar::wxMoSimulatorStatusBar(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    Init();
    Create(parent, id, pos, size, style);
}

void wxMoSimulatorStatusBar::Init()
{
}

// Creation
bool wxMoSimulatorStatusBar::Create(wxWindow* parent, wxWindowID id, const wxPoint& pos, const wxSize& size, long style)
{
    wxWindow::Create(parent, id, pos, size, style);

    //SetBackgroundColour(wxColour(63, 63, 63));
    SetBackgroundColour(wxColour(220, 220, 220));

    LoadImages();

    SetFont(wxFont(8, wxSWISS, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    return true;
}

void wxMoSimulatorStatusBar::OnSize( wxSizeEvent& WXUNUSED(event) )
{
    Refresh();
}

void wxMoSimulatorStatusBar::OnPaint( wxPaintEvent& WXUNUSED(event) )
{
    wxPaintDC dc(this);
    //dc.SetBackground(wxBrush(*wxRED));
    //dc.Clear();

    if (m_batteryBitmap.Ok())
    {
        int spacing = 2;
        int verticalOffset = 1; // compensate for rule at bottom
        int x = GetSize().x - m_batteryBitmap.GetWidth() - spacing;
        int y = (GetSize().y - m_batteryBitmap.GetHeight())/2 - verticalOffset;
        dc.DrawBitmap(m_batteryBitmap, x, y, true);

        wxString machine = wxT("iPhoni");

        dc.SetFont(GetFont());

        int textW, textH;
        dc.GetTextExtent(machine, & textW, & textH);
        int textX = spacing*2;
        int textY = (GetSize().y - textH)/2 - verticalOffset;

        dc.SetTextForeground(*wxWHITE);
        dc.DrawText(machine, textX, textY+1);
        dc.SetTextForeground(*wxBLACK);
        dc.DrawText(machine, textX, textY);

        dc.SetPen(wxPen(wxColour(128, 128, 128)));
        dc.DrawLine(0, 18, GetSize().x, 18);
    }
}

bool wxMoSimulatorStatusBar::LoadImages()
{
#if 0
    m_logoBitmap = wxBitmap(iphonelogo_xpm);
#endif
    m_batteryBitmap = wxBitmap(iphonebattery_xpm);

    return true;
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_NAVIGATION_ROTATE_RIGHT
 */

void wxMoSimulatorFrame::OnNavigationRotateRightClick( wxCommandEvent& WXUNUSED(event) )
{
    m_simulatorPanel->RotateRight();
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_NAVIGATION_ROTATE_LEFT
 */

void wxMoSimulatorFrame::OnNavigationRotateLeftClick( wxCommandEvent& WXUNUSED(event) )
{
    m_simulatorPanel->RotateLeft();
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_NAVIGATION_HOME_SCREEN
 */

void wxMoSimulatorFrame::OnNavigationHomeScreenClick( wxCommandEvent& WXUNUSED(event) )
{
    m_simulatorPanel->GoHome();
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for wxID_ABOUT
 */

void wxMoSimulatorFrame::OnAboutClick( wxCommandEvent& WXUNUSED(event) )
{
    wxAboutDialogInfo info;
    info.SetName(_("iPhoni"));
    info.SetDescription(_("A mobile phone simulator, for developing mobile GUIs on a variety of platforms."));
    info.AddDeveloper(_("Julian Smart"));
    info.SetLicence(_("wxWindows Licence"));
    info.SetCopyright(_("(c) Julian Smart"));
    info.SetVersion(_("1.0"));
    info.SetWebSite(_("http://www.wxwidgets.org"));

    wxAboutBox(info);
}


/*
 * wxEVT_COMMAND_MENU_SELECTED event handler for ID_VIEW_SHOW_SKIN
 */

void wxMoSimulatorFrame::OnViewShowSkinClick( wxCommandEvent& WXUNUSED(event) )
{
    ShowSkin(!m_simulatorPanel->GetUseSkin());
}

bool wxMoSimulatorFrame::ShowSkin(bool show)
{
    if (m_simulatorPanel)
        m_simulatorPanel->ShowSkin(show);
    return true;
}

/*
 * wxEVT_UPDATE_UI event handler for ID_VIEW_SHOW_SKIN
 */

void wxMoSimulatorFrame::OnViewShowSkinUpdate( wxUpdateUIEvent& event )
{
    event.Check(m_simulatorPanel->GetUseSkin());
}

bool wxMoSimulatorFrame::LoadSettings(const wxString& appName)
{
    wxConfig config(appName);

    bool useSkin;
    if (config.Read(wxT("UseSkin"), & useSkin))
    {
        m_simulatorPanel->SetUseSkin(useSkin);
    }

    return true;
}

bool wxMoSimulatorFrame::SaveSettings(const wxString& appName)
{
    wxConfig config(appName);
    config.Write(wxT("UseSkin"), m_simulatorPanel->GetUseSkin());

    return true;
}


/*
 * wxEVT_CLOSE_WINDOW event handler for wxID_ANY
 */

void wxMoSimulatorFrame::OnCloseWindow( wxCloseEvent& WXUNUSED(event) )
{
    SaveSettings();

    wxMoKeyboard::DestroyKeyboard();
    Destroy();
}

