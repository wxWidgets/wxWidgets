/////////////////////////////////////////////////////////////////////////////
// Name:        emulator.cpp
// Purpose:     Emulator wxWindows sample
// Author:      Julian Smart
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

#ifdef __GNUG__
    #pragma implementation "emulator.h"
#endif

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWindows headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "emulator.h"

#ifdef __WXX11__    
#include "wx/x11/reparent.h"
#endif

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources)
#if defined(__WXGTK__) || defined(__WXMOTIF__) || defined(__WXMAC__) || defined(__WXMGL__) || defined(__WXX11__)
    #include "mondrian.xpm"
#endif

// ----------------------------------------------------------------------------
// event tables and other macros for wxWindows
// ----------------------------------------------------------------------------

// the event tables connect the wxWindows events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
BEGIN_EVENT_TABLE(wxEmulatorFrame, wxFrame)
    EVT_MENU(Emulator_Quit,  wxEmulatorFrame::OnQuit)
    EVT_MENU(Emulator_About, wxEmulatorFrame::OnAbout)
END_EVENT_TABLE()

// Create a new application object: this macro will allow wxWindows to create
// the application object during program execution (it's better than using a
// static object for many reasons) and also declares the accessor function
// wxGetApp() which will return the reference of the right type (i.e. wxEmulatorApp and
// not wxApp)
IMPLEMENT_APP(wxEmulatorApp)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// the application class
// ----------------------------------------------------------------------------

wxEmulatorApp::wxEmulatorApp()
{
    m_xnestWindow = NULL;
    m_containerWindow = NULL;
}

// 'Main program' equivalent: the program execution "starts" here
bool wxEmulatorApp::OnInit()
{
    wxInitAllImageHandlers();

    // create the main application window
    wxEmulatorFrame *frame = new wxEmulatorFrame(_T("wxEmulator"),
                                 wxPoint(50, 50), wxSize(450, 340));
                                
    m_containerWindow = new wxEmulatorContainer(frame, -1);

    // Load the emulation info
    if (!LoadEmulator())
    {
        frame->Destroy();
        wxMessageBox(wxT("Sorry, could not load this emulator. Please check bitmaps are valid."));
        return FALSE;        
    }
    
    if (m_emulatorInfo.m_emulatorBackgroundBitmap.Ok())
        frame->SetClientSize(m_emulatorInfo.m_emulatorBackgroundBitmap.GetWidth(),
                             m_emulatorInfo.m_emulatorBackgroundBitmap.GetHeight());
    
    // and show it (the frames, unlike simple controls, are not shown when
    // created initially)
    frame->Show(TRUE);
    
#ifdef __WXX11__    
    m_xnestWindow = new wxAdoptedWindow;

    wxString cmd;
    cmd.Printf(wxT("Xnest :100 -geometry %dx%d+50+50"),
        (int) m_emulatorScreenSize.x, (int) m_emulatorScreenSize.y);

    // Asynchronously executes Xnest    
    if (0 == wxExecute(cmd))
    {
        frame->Destroy();
        wxMessageBox(wxT("Sorry, could not run Xnest. Please check your PATH."));
        return FALSE;
    }
    
    wxReparenter reparenter;
    if (!reparenter.WaitAndReparent(m_containerWindow, m_xnestWindow, wxT("Xnest")))
    {
        wxMessageBox(wxT("Sorry, could not reparent Xnest.."));
        frame->Destroy();
        return FALSE;
    }
#endif

    // success: wxApp::OnRun() will be called which will enter the main message
    // loop and the application will run. If we returned FALSE here, the
    // application would exit immediately.
    return TRUE;
}

// Load the specified emulator.
// For now, hard-wired. TODO: make this configurable
bool wxEmulatorApp::LoadEmulator()
{
    m_emulatorInfo.m_emulatorTitle = wxT("iPAQ Emulator");

    m_emulatorInfo.m_emulatorDescription = wxT("No description yet");

    // The offset from the top-left of the main emulator
    // bitmap and the virtual screen (where Xnest is
    // positioned)
    m_emulatorInfo.m_emulatorScreenPosition = wxPoint(45, 57);

    // The emulated screen size
    m_emulatorInfo.m_emulatorScreenSize = wxSize(240, 320);

    m_emulatorInfo.m_emulatorBackgroundBitmapName = wxT("ipaq01.jpg");

    m_emulatorInfo.m_emulatorBackgroundColour = * wxBLACK;
    
    return m_emulatorInfo.Load();
}

// ----------------------------------------------------------------------------
// main frame
// ----------------------------------------------------------------------------

// frame constructor
wxEmulatorFrame::wxEmulatorFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
       : wxFrame(NULL, -1, title, pos, size)
{
    // set the frame icon
    SetIcon(wxICON(mondrian));

#if wxUSE_MENUS
    // create a menu bar
    wxMenu *menuFile = new wxMenu;

    // the "About" item should be in the help menu
    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(Emulator_About, _T("&About...\tF1"), _T("Show about dialog"));

    menuFile->Append(Emulator_Quit, _T("E&xit\tAlt-X"), _T("Quit this program"));

    // now append the freshly created menu to the menu bar...
    wxMenuBar *menuBar = new wxMenuBar();
    menuBar->Append(menuFile, _T("&File"));
    menuBar->Append(helpMenu, _T("&Help"));

    // ... and attach this menu bar to the frame
    SetMenuBar(menuBar);
#endif // wxUSE_MENUS

#if wxUSE_STATUSBAR
    // create a status bar just for fun (by default with 1 pane only)
    CreateStatusBar(2);
    SetStatusText(_T("Welcome to wxWindows!"));
#endif // wxUSE_STATUSBAR
}


// event handlers

void wxEmulatorFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    // TRUE is to force the frame to close
    Close(TRUE);
}

void wxEmulatorFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( _T("wxEmulator is an environment for testing embedded X11 apps.\n"));

    wxMessageBox(msg, _T("About wxEmulator"), wxOK | wxICON_INFORMATION, this);
}

IMPLEMENT_CLASS(wxEmulatorContainer, wxWindow)

BEGIN_EVENT_TABLE(wxEmulatorContainer, wxWindow)
    EVT_SIZE(wxEmulatorContainer::OnSize)
    EVT_PAINT(wxEmulatorContainer::OnPaint)
    EVT_ERASE_BACKGROUND(wxEmulatorContainer::OnEraseBackground)
END_EVENT_TABLE()

wxEmulatorContainer::wxEmulatorContainer(wxWindow* parent, wxWindowID id):
    wxWindow(parent, id, wxDefaultPosition, wxDefaultSize)
{
}

void wxEmulatorContainer::OnSize(wxSizeEvent& event)
{
    wxSize sz = GetClientSize();
    if (wxGetApp().m_emulatorInfo.m_emulatorBackgroundBitmap.Ok() &&
        wxGetApp().m_xnestWindow)
    {
        int bitmapWidth = wxGetApp().m_emulatorInfo.m_emulatorBackgroundBitmap.GetWidth();
        int bitmapHeight = wxGetApp().m_emulatorInfo.m_emulatorBackgroundBitmap.GetHeight();
        
        int x = wxMax(0, (sz.x - bitmapWidth)/2.0);
        int y = wxMax(0, (sz.y - bitmapHeight)/2.0);
        
        x += wxGetApp().m_emulatorInfo.m_emulatorScreenPosition.x;
        y += wxGetApp().m_emulatorInfo.m_emulatorScreenPosition.y;
        
        wxGetApp().m_xnestWindow->Move(x, y);
    }
    Refresh();
}

void wxEmulatorContainer::OnPaint(wxPaintEvent& event)
{
    wxPaintDC dc(this);
    
    wxSize sz = GetClientSize();
    if (wxGetApp().m_emulatorInfo.m_emulatorBackgroundBitmap.Ok())
    {
        int bitmapWidth = wxGetApp().m_emulatorInfo.m_emulatorBackgroundBitmap.GetWidth();
        int bitmapHeight = wxGetApp().m_emulatorInfo.m_emulatorBackgroundBitmap.GetHeight();
        
        int x = wxMax(0, (sz.x - bitmapWidth)/2.0);
        int y = wxMax(0, (sz.y - bitmapHeight)/2.0);
        
        dc.DrawBitmap(wxGetApp().m_emulatorInfo.m_emulatorBackgroundBitmap, x, y);
    }
}

void wxEmulatorContainer::OnEraseBackground(wxEraseEvent& event)
{
    wxDC* dc = NULL;
    
    if (event.GetDC())
    {
        dc = event.GetDC();
    }
    else
    {
        dc = new wxClientDC(this);
    }
    
    dc->SetBackground(wxBrush(wxGetApp().m_emulatorInfo.m_emulatorBackgroundColour, wxSOLID));
    dc->Clear();
    
    if (!event.GetDC())
        delete dc;
}

// Information about the emulator decorations

void wxEmulatorInfo::Copy(const wxEmulatorInfo& info)
{
    m_emulatorTitle = info.m_emulatorTitle;
    m_emulatorDescription = info.m_emulatorDescription;
    m_emulatorScreenPosition = info.m_emulatorScreenPosition;
    m_emulatorScreenSize = info.m_emulatorScreenSize;
    m_emulatorBackgroundBitmap = info.m_emulatorBackgroundBitmap;
    m_emulatorBackgroundBitmapName = info.m_emulatorBackgroundBitmapName;
    m_emulatorBackgroundColour = info.m_emulatorBackgroundColour;
}

// Initialisation
void wxEmulatorInfo::Init()
{
}

// Loads bitmaps
bool wxEmulatorInfo::Load()
{
    if (m_emulatorBackgroundBitmapName.IsEmpty())
        return FALSE;

    // TODO: prepend current directory if relative name
    int type = wxDetermineImageType(m_emulatorBackgroundBitmapName);
    if (type == -1)
        return FALSE;
    
    return m_emulatorBackgroundBitmap.LoadFile(m_emulatorBackgroundBitmapName, type);
}

// Returns the image type, or -1, determined from the extension.
int wxDetermineImageType(const wxString& filename)
{
    wxString path, name, ext;

    wxSplitPath(filename, & path, & name, & ext);

    ext.MakeLower();
    if (ext == "jpg" || ext == "jpeg")
        return wxBITMAP_TYPE_JPEG;
    else if (ext == "gif")
        return wxBITMAP_TYPE_GIF;
    else if (ext == "bmp")
        return wxBITMAP_TYPE_BMP;
    else if (ext == "png")
        return wxBITMAP_TYPE_PNG;
    else if (ext == "pcx")
        return wxBITMAP_TYPE_PCX;
    else if (ext == "tif" || ext == "tiff")
        return wxBITMAP_TYPE_TIF;
    else
        return -1;
}


