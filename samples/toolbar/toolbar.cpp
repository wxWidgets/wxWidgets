/////////////////////////////////////////////////////////////////////////////
// Name:        toolbar.cpp
// Purpose:     wxToolBar sample
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

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

#include <wx/toolbar.h>
#include <wx/log.h>
#include <wx/image.h>

// define this to 1 to use wxToolBarSimple instead of the native one
#define USE_GENERIC_TBAR 0

// define this to use XPMs everywhere (by default, BMPs are used under Win)
// BMPs use less space, but aren't compiled into the executable on other platforms
#ifdef __WXMSW__
    #define USE_XPM_BITMAPS 0
#else
    #define USE_XPM_BITMAPS 1
#endif

#if USE_GENERIC_TBAR
    #if !wxUSE_TOOLBAR_SIMPLE
        #error wxToolBarSimple is not compiled in, set wxUSE_TOOLBAR_SIMPLE \
               to 1 in setup.h and recompile the library.
    #else
        #include <wx/tbarsmpl.h>
    #endif
#endif // USE_GENERIC_TBAR

#if USE_XPM_BITMAPS && defined(__WXMSW__) && !wxUSE_XPM_IN_MSW
    #error You need to enable XPM support to use XPM bitmaps with toolbar!
#endif // USE_XPM_BITMAPS

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#if USE_XPM_BITMAPS
    #include "mondrian.xpm"
    #include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/preview.xpm"  // paste XPM
    #include "bitmaps/print.xpm"
    #include "bitmaps/help.xpm"
#endif // USE_XPM_BITMAPS

// ----------------------------------------------------------------------------
// classes
// ----------------------------------------------------------------------------

// Define a new application
class MyApp : public wxApp
{
public:
    bool OnInit();
};

// Define a new frame
class MyFrame: public wxFrame
{
public:
    MyFrame(wxFrame *parent,
            wxWindowID id = wxID_ANY,
            const wxString& title = _T("wxToolBar Sample"),
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE|wxCLIP_CHILDREN|wxNO_FULL_REPAINT_ON_RESIZE);

    void RecreateToolbar();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    void OnSize(wxSizeEvent& event);

    void OnToggleToolbar(wxCommandEvent& event);
    void OnToggleAnotherToolbar(wxCommandEvent& event);
    void OnToggleHorizontalText(wxCommandEvent& WXUNUSED(event));

    void OnToggleToolbarSize(wxCommandEvent& event);
    void OnToggleToolbarOrient(wxCommandEvent& event);
    void OnToggleToolbarRows(wxCommandEvent& event);

    void OnEnablePrint(wxCommandEvent& WXUNUSED(event)) { DoEnablePrint(); }
    void OnDeletePrint(wxCommandEvent& WXUNUSED(event)) { DoDeletePrint(); }
    void OnInsertPrint(wxCommandEvent& event);
    void OnChangeToolTip(wxCommandEvent& event);
    void OnToggleHelp(wxCommandEvent& WXUNUSED(event)) { DoToggleHelp(); }
    void OnToggleRadioBtn(wxCommandEvent& event);

    void OnToolbarStyle(wxCommandEvent& event);

    void OnToolLeftClick(wxCommandEvent& event);
    void OnToolEnter(wxCommandEvent& event);

    void OnCombo(wxCommandEvent& event);

    void OnUpdateCopyAndCut(wxUpdateUIEvent& event);
    void OnUpdateToggleHorzText(wxUpdateUIEvent& event);
    void OnUpdateToggleRadioBtn(wxUpdateUIEvent& event)
        { event.Enable( m_tbar != NULL ); }

#if USE_GENERIC_TBAR
    virtual wxToolBar *OnCreateToolBar(long style,
                                       wxWindowID id,
                                       const wxString& name );
#endif // USE_GENERIC_TBAR

private:
    void DoEnablePrint();
    void DoDeletePrint();
    void DoToggleHelp();

    void LayoutChildren();

    bool                m_smallToolbar,
                        m_horzToolbar,
                        m_horzText;
    size_t              m_rows;             // 1 or 2 only

    // the number of print buttons we have (they're added/removed dynamically)
    size_t              m_nPrint;

    wxTextCtrl         *m_textWindow;

    wxToolBar          *m_tbar;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = 500;

static const long TOOLBAR_STYLE = wxTB_FLAT | wxTB_DOCKABLE | wxTB_TEXT;

enum
{
    IDM_TOOLBAR_TOGGLETOOLBARSIZE = 200,
    IDM_TOOLBAR_TOGGLETOOLBARORIENT,
    IDM_TOOLBAR_TOGGLETOOLBARROWS,
    IDM_TOOLBAR_ENABLEPRINT,
    IDM_TOOLBAR_DELETEPRINT,
    IDM_TOOLBAR_INSERTPRINT,
    IDM_TOOLBAR_TOGGLEHELP,
    IDM_TOOLBAR_TOGGLERADIOBTN1,
    IDM_TOOLBAR_TOGGLERADIOBTN2,
    IDM_TOOLBAR_TOGGLERADIOBTN3,
    IDM_TOOLBAR_TOGGLE_TOOLBAR,
    IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
    IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
    IDM_TOOLBAR_CHANGE_TOOLTIP,
    IDM_TOOLBAR_SHOW_TEXT,
    IDM_TOOLBAR_SHOW_ICONS,
    IDM_TOOLBAR_SHOW_BOTH,

    IDM_TOOLBAR_OTHER_1,
    IDM_TOOLBAR_OTHER_2,
    IDM_TOOLBAR_OTHER_3,

    ID_COMBO = 1000
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

// Notice that wxID_HELP will be processed for the 'About' menu and the toolbar
// help button.

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_SIZE(MyFrame::OnSize)

    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_HELP, MyFrame::OnAbout)

    EVT_MENU(IDM_TOOLBAR_TOGGLE_TOOLBAR, MyFrame::OnToggleToolbar)
    EVT_MENU(IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR, MyFrame::OnToggleAnotherToolbar)
    EVT_MENU(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT, MyFrame::OnToggleHorizontalText)

    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBARSIZE, MyFrame::OnToggleToolbarSize)
    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBARORIENT, MyFrame::OnToggleToolbarOrient)
    EVT_MENU(IDM_TOOLBAR_TOGGLETOOLBARROWS, MyFrame::OnToggleToolbarRows)

    EVT_MENU(IDM_TOOLBAR_ENABLEPRINT, MyFrame::OnEnablePrint)
    EVT_MENU(IDM_TOOLBAR_DELETEPRINT, MyFrame::OnDeletePrint)
    EVT_MENU(IDM_TOOLBAR_INSERTPRINT, MyFrame::OnInsertPrint)
    EVT_MENU(IDM_TOOLBAR_TOGGLEHELP, MyFrame::OnToggleHelp)
    EVT_MENU_RANGE(IDM_TOOLBAR_TOGGLERADIOBTN1, IDM_TOOLBAR_TOGGLERADIOBTN3,
                   MyFrame::OnToggleRadioBtn)
    EVT_MENU(IDM_TOOLBAR_CHANGE_TOOLTIP, MyFrame::OnChangeToolTip)

    EVT_MENU_RANGE(IDM_TOOLBAR_SHOW_TEXT, IDM_TOOLBAR_SHOW_BOTH,
                   MyFrame::OnToolbarStyle)

    EVT_MENU(wxID_ANY, MyFrame::OnToolLeftClick)

    EVT_COMBOBOX(ID_COMBO, MyFrame::OnCombo)

    EVT_TOOL_ENTER(ID_TOOLBAR, MyFrame::OnToolEnter)

    EVT_UPDATE_UI(wxID_COPY, MyFrame::OnUpdateCopyAndCut)
    EVT_UPDATE_UI(wxID_CUT, MyFrame::OnUpdateCopyAndCut)

    EVT_UPDATE_UI_RANGE(IDM_TOOLBAR_TOGGLERADIOBTN1,
                        IDM_TOOLBAR_TOGGLERADIOBTN3,
                        MyFrame::OnUpdateToggleRadioBtn)
    EVT_UPDATE_UI(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
                  MyFrame::OnUpdateToggleHorzText)
END_EVENT_TABLE()

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

// The `main program' equivalent, creating the windows and returning the
// main frame
bool MyApp::OnInit()
{
    // Create the main frame window
    MyFrame* frame = new MyFrame((wxFrame *) NULL, wxID_ANY,
                                 _T("wxToolBar Sample"),
#ifdef __WXWINCE__
                                 wxDefaultPosition, wxDefaultSize
#else
                                 wxPoint(100, 100), wxSize(550, 300)
#endif
                                 );

    frame->Show(true);

#if wxUSE_STATUSBAR
    frame->SetStatusText(_T("Hello, wxWidgets"));
#endif

    SetTopWindow(frame);

    return true;
}

void MyFrame::RecreateToolbar()
{
#ifdef __WXWINCE__
    // On Windows CE, we should not delete the
    // previous toolbar in case it contains the menubar.
    // We'll try to accomodate this usage in due course.
    wxToolBar* toolBar = CreateToolBar();
#else
    // delete and recreate the toolbar
    wxToolBarBase *toolBar = GetToolBar();
    long style = toolBar ? toolBar->GetWindowStyle() : TOOLBAR_STYLE;

    delete toolBar;

    SetToolBar(NULL);

    style &= ~(wxTB_HORIZONTAL | wxTB_VERTICAL | wxTB_HORZ_LAYOUT);
    style |= m_horzToolbar ? wxTB_HORIZONTAL : wxTB_VERTICAL;

    if ( style & wxTB_TEXT && !(style & wxTB_NOICONS) && m_horzText )
        style |= wxTB_HORZ_LAYOUT;

    toolBar = CreateToolBar(style, ID_TOOLBAR);
#endif

    // Set up toolbar
    wxBitmap toolBarBitmaps[8];

#if USE_XPM_BITMAPS
    toolBarBitmaps[0] = wxBitmap(new_xpm);
    toolBarBitmaps[1] = wxBitmap(open_xpm);
    toolBarBitmaps[2] = wxBitmap(save_xpm);
    toolBarBitmaps[3] = wxBitmap(copy_xpm);
    toolBarBitmaps[4] = wxBitmap(cut_xpm);
    toolBarBitmaps[5] = wxBitmap(paste_xpm);
    toolBarBitmaps[6] = wxBitmap(print_xpm);
    toolBarBitmaps[7] = wxBitmap(help_xpm);
#else // !USE_XPM_BITMAPS
    toolBarBitmaps[0] = wxBITMAP(new);
    toolBarBitmaps[1] = wxBITMAP(open);
    toolBarBitmaps[2] = wxBITMAP(save);
    toolBarBitmaps[3] = wxBITMAP(copy);
    toolBarBitmaps[4] = wxBITMAP(cut);
    toolBarBitmaps[5] = wxBITMAP(paste);
    toolBarBitmaps[6] = wxBITMAP(print);
    toolBarBitmaps[7] = wxBITMAP(help);
#endif // USE_XPM_BITMAPS/!USE_XPM_BITMAPS

    if ( !m_smallToolbar )
    {
        int w = 2*toolBarBitmaps[0].GetWidth(),
            h = 2*toolBarBitmaps[0].GetHeight();
        for ( size_t n = 0; n < WXSIZEOF(toolBarBitmaps); n++ )
        {
            toolBarBitmaps[n] =
                wxBitmap(toolBarBitmaps[n].ConvertToImage().Scale(w, h));
        }

        toolBar->SetToolBitmapSize(wxSize(w, h));
    }

    toolBar->AddTool(wxID_NEW, _T("New"), toolBarBitmaps[0], _T("New file"));
    toolBar->AddTool(wxID_OPEN, _T("Open"), toolBarBitmaps[1], _T("Open file"));

    // the generic toolbar doesn't really support this
#if (wxUSE_TOOLBAR_NATIVE && !USE_GENERIC_TBAR) && !defined(__WXX11__) || defined(__WXUNIVERSAL__)
    // adding a combo to a vertical toolbar is not very smart
    if ( m_horzToolbar )
    {
        wxComboBox *combo = new wxComboBox(toolBar, ID_COMBO, _T(""), wxDefaultPosition, wxSize(200,wxDefaultCoord) );
        combo->Append(_T("This"));
        combo->Append(_T("is a"));
        combo->Append(_T("combobox"));
        combo->Append(_T("in a"));
        combo->Append(_T("toolbar"));
        toolBar->AddControl(combo);
    }
#endif // toolbars which don't support controls

    toolBar->AddTool(wxID_SAVE, _T("Save"), toolBarBitmaps[2], _T("Toggle button 1"), wxITEM_CHECK);
    toolBar->AddTool(wxID_COPY, _T("Copy"), toolBarBitmaps[3], _T("Toggle button 2"), wxITEM_CHECK);
    toolBar->AddTool(wxID_CUT, _T("Cut"), toolBarBitmaps[4], _T("Toggle/Untoggle help button"));
    toolBar->AddTool(wxID_PASTE, _T("Paste"), toolBarBitmaps[5], _T("Paste"));
    toolBar->AddTool(wxID_PRINT, _T("Print"), toolBarBitmaps[6], _T("Delete this tool. This is a very long tooltip to test whether it does the right thing when the tooltip is more than Windows can cope with."));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_HELP, _T("Help"), toolBarBitmaps[7], _T("Help button"), wxITEM_CHECK);

    // after adding the buttons to the toolbar, must call Realize() to reflect
    // the changes
    toolBar->Realize();

    toolBar->SetRows(m_horzToolbar ? m_rows : 10 / m_rows);
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

// Define my frame constructor
MyFrame::MyFrame(wxFrame* parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style)
       : wxFrame(parent, id, title, pos, size, style)
{
    m_tbar = NULL;

    m_smallToolbar = true;
    m_horzToolbar = true;
    m_horzText = false;
    m_rows = 1;
    m_nPrint = 1;

#if wxUSE_STATUSBAR
    // Give it a status line
    CreateStatusBar();
#endif

    // Give it an icon
    SetIcon(wxICON(mondrian));

    // Make a menubar
    wxMenu *tbarMenu = new wxMenu;
    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_TOOLBAR,
                              _T("Toggle &toolbar\tCtrl-Z"),
                              _T("Show or hide the toolbar"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_ANOTHER_TOOLBAR,
                              _T("Toggle &another toolbar\tCtrl-A"),
                              _T("Show/hide another test toolbar"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLE_HORIZONTAL_TEXT,
                              _T("Toggle hori&zontal text\tCtrl-H"),
                              _T("Show text under/alongside the icon"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLBARSIZE,
                              _T("&Toggle toolbar size\tCtrl-S"),
                              _T("Toggle between big/small toolbar"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLBARORIENT,
                              _T("Toggle toolbar &orientation\tCtrl-O"),
                              _T("Toggle toolbar orientation"));

    tbarMenu->AppendCheckItem(IDM_TOOLBAR_TOGGLETOOLBARROWS,
                              _T("Toggle number of &rows\tCtrl-R"),
                              _T("Toggle number of toolbar rows between 1 and 2"));

    tbarMenu->AppendSeparator();

    tbarMenu->Append(IDM_TOOLBAR_ENABLEPRINT, _T("&Enable print button\tCtrl-E"), _T(""));
    tbarMenu->Append(IDM_TOOLBAR_DELETEPRINT, _T("&Delete print button\tCtrl-D"), _T(""));
    tbarMenu->Append(IDM_TOOLBAR_INSERTPRINT, _T("&Insert print button\tCtrl-I"), _T(""));
    tbarMenu->Append(IDM_TOOLBAR_TOGGLEHELP, _T("Toggle &help button\tCtrl-T"), _T(""));
    tbarMenu->AppendSeparator();
    tbarMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN1, _T("Toggle &1st radio button\tCtrl-1"), _T(""));
    tbarMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN2, _T("Toggle &2nd radio button\tCtrl-2"), _T(""));
    tbarMenu->Append(IDM_TOOLBAR_TOGGLERADIOBTN3, _T("Toggle &3rd radio button\tCtrl-3"), _T(""));
    tbarMenu->AppendSeparator();
    tbarMenu->Append(IDM_TOOLBAR_CHANGE_TOOLTIP, _T("Change tool tip"), _T(""));
    tbarMenu->AppendSeparator();
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_TEXT, _T("Show &text\tAlt-T"));
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_ICONS, _T("Show &icons\tAlt-I"));
    tbarMenu->AppendRadioItem(IDM_TOOLBAR_SHOW_BOTH, _T("Show &both\tAlt-B"));

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_EXIT, _T("E&xit\tAlt-X"), _T("Quit toolbar sample") );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, _T("&About"), _T("About toolbar sample"));

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, _T("&File"));
    menuBar->Append(tbarMenu, _T("&Toolbar"));
    menuBar->Append(helpMenu, _T("&Help"));

    // Associate the menu bar with the frame
    SetMenuBar(menuBar);

    menuBar->Check(IDM_TOOLBAR_SHOW_BOTH, true);

    // Create the toolbar
    RecreateToolbar();

    m_textWindow = new wxTextCtrl(this, wxID_ANY, _T(""), wxPoint(0, 0), wxDefaultSize, wxTE_MULTILINE);
}

#if USE_GENERIC_TBAR

wxToolBar* MyFrame::OnCreateToolBar(long style,
                                    wxWindowID id,
                                    const wxString& name)
{
    return (wxToolBar *)new wxToolBarSimple(this, id,
                                            wxDefaultPosition, wxDefaultSize,
                                            style, name);
}

#endif // USE_GENERIC_TBAR

void MyFrame::LayoutChildren()
{
    wxSize size = GetClientSize();

    int offset;
    if ( m_tbar )
    {
        m_tbar->SetSize(wxDefaultCoord, size.y);
        m_tbar->Move(0, 0);

        offset = m_tbar->GetSize().x;
    }
    else
    {
        offset = 0;
    }

    m_textWindow->SetSize(offset, 0, size.x - offset, size.y);
}

void MyFrame::OnSize(wxSizeEvent& event)
{
    if ( m_tbar )
    {
        LayoutChildren();
    }
    else
    {
        event.Skip();
    }
}

void MyFrame::OnToggleToolbar(wxCommandEvent& WXUNUSED(event))
{
    wxToolBar *tbar = GetToolBar();

    if ( !tbar )
    {
        RecreateToolbar();
    }
    else
    {
        delete tbar;

        SetToolBar(NULL);
    }
}

void MyFrame::OnToggleHorizontalText(wxCommandEvent& WXUNUSED(event))
{
    m_horzText = !m_horzText;

    RecreateToolbar();
}

void MyFrame::OnToggleAnotherToolbar(wxCommandEvent& WXUNUSED(event))
{
    if ( m_tbar )
    {
        delete m_tbar;
        m_tbar = NULL;
    }
    else
    {
        long style = GetToolBar()->GetWindowStyle();
        style &= ~wxTB_HORIZONTAL;
        style |= wxTB_VERTICAL;

        m_tbar = new wxToolBar(this, wxID_ANY,
                               wxDefaultPosition, wxDefaultSize,
                               style);

        m_tbar->SetMargins(4, 4);

        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_1, _T("First"), wxBITMAP(new));
        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_2, _T("Second"), wxBITMAP(open));
        m_tbar->AddRadioTool(IDM_TOOLBAR_OTHER_3, _T("Third"), wxBITMAP(save));
        m_tbar->AddSeparator();
        m_tbar->AddTool(wxID_HELP, _T("Help"), wxBITMAP(help));

        m_tbar->Realize();
    }

    LayoutChildren();
}

void MyFrame::OnToggleToolbarSize(wxCommandEvent& WXUNUSED(event))
{
    m_smallToolbar = !m_smallToolbar;

    RecreateToolbar();
}

void MyFrame::OnToggleToolbarRows(wxCommandEvent& WXUNUSED(event))
{
    // m_rows may be only 1 or 2
    m_rows = 3 - m_rows;

    GetToolBar()->SetRows(m_horzToolbar ? m_rows : 10 / m_rows);

    //RecreateToolbar(); -- this is unneeded
}

void MyFrame::OnToggleToolbarOrient(wxCommandEvent& WXUNUSED(event))
{
    m_horzToolbar = !m_horzToolbar;

    RecreateToolbar();
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    if ( event.IsChecked() )
        m_textWindow->WriteText( _T("Help button down now.\n") );
    else
        m_textWindow->WriteText( _T("Help button up now.\n") );

    (void)wxMessageBox(_T("wxWidgets toolbar sample"), _T("About wxToolBar"));
}

void MyFrame::OnToolLeftClick(wxCommandEvent& event)
{
    wxString str;
    str.Printf( _T("Clicked on tool %d\n"), event.GetId());
    m_textWindow->WriteText( str );

    if (event.GetId() == wxID_COPY)
    {
        DoEnablePrint();
    }

    if (event.GetId() == wxID_CUT)
    {
        DoToggleHelp();
    }

    if (event.GetId() == wxID_PRINT)
    {
        DoDeletePrint();
    }
}

void MyFrame::OnCombo(wxCommandEvent& event)
{
    wxLogStatus(_T("Combobox string '%s' selected"), event.GetString().c_str());
}

void MyFrame::DoEnablePrint()
{
    if ( !m_nPrint )
        return;

    wxToolBarBase *tb = GetToolBar();
    tb->EnableTool(wxID_PRINT, !tb->GetToolEnabled(wxID_PRINT));
}

void MyFrame::DoDeletePrint()
{
    if ( !m_nPrint )
        return;

    wxToolBarBase *tb = GetToolBar();
    tb->DeleteTool( wxID_PRINT );

    m_nPrint--;
}

void MyFrame::DoToggleHelp()
{
    wxToolBarBase *tb = GetToolBar();
    tb->ToggleTool( wxID_HELP, !tb->GetToolState( wxID_HELP ) );
}

void MyFrame::OnUpdateCopyAndCut(wxUpdateUIEvent& event)
{
    event.Enable( m_textWindow->CanCopy() );
}

void MyFrame::OnUpdateToggleHorzText(wxUpdateUIEvent& event)
{
    wxToolBar *tbar = GetToolBar();
    event.Enable( tbar &&
                    tbar->HasFlag(wxTB_TEXT) &&
                        !tbar->HasFlag(wxTB_NOICONS) );
}

void MyFrame::OnChangeToolTip(wxCommandEvent& WXUNUSED(event))
{
    GetToolBar()->SetToolShortHelp(wxID_NEW, _T("New toolbar button"));
}

void MyFrame::OnToolbarStyle(wxCommandEvent& event)
{
    long style = GetToolBar()->GetWindowStyle();
    style &= ~(wxTB_NOICONS | wxTB_TEXT);

    switch ( event.GetId() )
    {
        case IDM_TOOLBAR_SHOW_TEXT:
            style |= wxTB_NOICONS | wxTB_TEXT;
            break;

        case IDM_TOOLBAR_SHOW_ICONS:
            // nothing to do
            break;

        case IDM_TOOLBAR_SHOW_BOTH:
            style |= wxTB_TEXT;
    }

    GetToolBar()->SetWindowStyle(style);
}

void MyFrame::OnInsertPrint(wxCommandEvent& WXUNUSED(event))
{
    m_nPrint++;

    wxToolBarBase *tb = GetToolBar();
    tb->InsertTool(0, wxID_PRINT, _T("New print"),
                   wxBITMAP(print), wxNullBitmap,
                   wxITEM_NORMAL,
                   _T("Delete this tool"),
                   _T("This button was inserted into the toolbar"));

    // must call Realize() after adding a new button
    tb->Realize();
}

void MyFrame::OnToolEnter(wxCommandEvent& event)
{
#if wxUSE_STATUSBAR
    if (event.GetSelection() > -1)
    {
        wxString str;
        str.Printf(_T("This is tool number %d"), event.GetSelection());
        SetStatusText(str);
    }
    else
        SetStatusText(_T(""));
#else
    wxUnusedVar(event);
#endif // wxUSE_STATUSBAR
}

void MyFrame::OnToggleRadioBtn(wxCommandEvent& event)
{
    if ( m_tbar )
    {
        m_tbar->ToggleTool(IDM_TOOLBAR_OTHER_1 +
                            event.GetId() - IDM_TOOLBAR_TOGGLERADIOBTN1, true);
    }
}

