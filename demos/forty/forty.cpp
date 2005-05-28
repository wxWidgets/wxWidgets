/////////////////////////////////////////////////////////////////////////////
// Name:        forty.cpp
// Purpose:     Forty Thieves patience game
// Author:      Chris Breeze
// Modified by:
// Created:     21/07/97
// RCS-ID:      $Id$
// Copyright:   (c) 1993-1998 Chris Breeze
// Licence:   	wxWindows licence
//---------------------------------------------------------------------------
// Last modified: 22nd July 1998 - ported to wxWindows 2.0
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#pragma interface
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP
#include "wx/wx.h"
#endif

#include "canvas.h"
#include "forty.h"
#include "card.h"
#include "scoredg.h"

#if wxUSE_HTML
#include "wx/file.h"
#include "wx/html/htmlwin.h"
#endif

BEGIN_EVENT_TABLE(FortyFrame, wxFrame)
	EVT_MENU(NEW_GAME, FortyFrame::NewGame)
	EVT_MENU(EXIT, FortyFrame::Exit)
	EVT_MENU(ABOUT, FortyFrame::About)
	EVT_MENU(UNDO, FortyFrame::Undo)
	EVT_MENU(REDO, FortyFrame::Redo)
	EVT_MENU(SCORES, FortyFrame::Scores)
	EVT_MENU(RIGHT_BUTTON_UNDO, FortyFrame::ToggleRightButtonUndo)
	EVT_MENU(HELPING_HAND, FortyFrame::ToggleHelpingHand)
        EVT_MENU(LARGE_CARDS, FortyFrame::ToggleCardSize)
    EVT_CLOSE(FortyFrame::OnCloseWindow)
END_EVENT_TABLE()

// Create a new application object
IMPLEMENT_APP	(FortyApp)

wxColour* FortyApp::m_backgroundColour = 0;
wxColour* FortyApp::m_textColour = 0;
wxBrush*  FortyApp::m_backgroundBrush = 0;

FortyApp::FortyApp()
{
}

FortyApp::~FortyApp()
{
    delete m_backgroundColour;
    delete m_textColour;
    delete m_backgroundBrush;
    delete Card::m_symbolBmap;
    delete Card::m_pictureBmap;

}

bool FortyApp::OnInit()
{
        bool largecards = FALSE;
        wxSize size(668,510);

        if ((argc > 1) && (!wxStrcmp(argv[1],_T("-L"))))
        {
            largecards = TRUE;
            size = wxSize(1000,750);
        }

	FortyFrame* frame = new FortyFrame(
			0,
			_T("Forty Thieves"),
                        -1, -1, size.x, size.y,largecards
			);

 	// Show the frame
	frame->Show(TRUE);

        frame->GetCanvas()->ShowPlayerDialog();

	return TRUE;
}

const wxColour& FortyApp::BackgroundColour()
{
	if (!m_backgroundColour)
	{
		m_backgroundColour = new wxColour(0, 128, 0);
	}

	return *m_backgroundColour;
}

const wxBrush& FortyApp::BackgroundBrush()
{
	if (!m_backgroundBrush)
	{
		m_backgroundBrush = new wxBrush(BackgroundColour(), wxSOLID);
	}

	return *m_backgroundBrush;
}

const wxColour& FortyApp::TextColour()
{
	if (!m_textColour)
	{
		m_textColour = new wxColour(_T("BLACK"));
	}

	return *m_textColour;
}

// My frame constructor
FortyFrame::FortyFrame(wxFrame* frame, const wxString& title, int x, int y, int w, int h,bool largecards):
	wxFrame(frame, -1, title, wxPoint(x, y), wxSize(w, h))
{
#ifdef __WXMAC__
	// we need this in order to allow the about menu relocation, since ABOUT is not the default id of the about menu 
	wxApp::s_macAboutMenuItemId = ABOUT ;
#endif
	// set the icon
#ifdef __WXMSW__
	SetIcon(wxIcon(_T("CardsIcon")));
#else
#ifdef GTK_TBD
	SetIcon(wxIcon(Cards_bits, Cards_width, Cards_height));
#endif
#endif

	// Make a menu bar
	wxMenu* gameMenu = new wxMenu;
	gameMenu->Append(NEW_GAME, _T("&New"), _T("Start a new game"));
	gameMenu->Append(SCORES, _T("&Scores..."), _T("Displays scores"));
	gameMenu->Append(EXIT, _T("E&xit"), _T("Exits Forty Thieves"));

	wxMenu* editMenu = new wxMenu;
	editMenu->Append(UNDO, _T("&Undo"), _T("Undo the last move"));
	editMenu->Append(REDO, _T("&Redo"), _T("Redo a move that has been undone"));

	wxMenu*	optionsMenu = new wxMenu;
	optionsMenu->Append(RIGHT_BUTTON_UNDO,
			_T("&Right button undo"),
			_T("Enables/disables right mouse button undo and redo"),
			TRUE
			);
	optionsMenu->Append(HELPING_HAND,
			_T("&Helping hand"),
			_T("Enables/disables hand cursor when a card can be moved"),
			TRUE
			);
        optionsMenu->Append(LARGE_CARDS,
                        _T("&Large cards"),
                        _T("Enables/disables large cards for high resolution displays"),
                        TRUE
                        );
	optionsMenu->Check(HELPING_HAND, TRUE);
	optionsMenu->Check(RIGHT_BUTTON_UNDO, TRUE);
        optionsMenu->Check(LARGE_CARDS, largecards ? TRUE : FALSE);

	wxMenu* helpMenu = new wxMenu;
	helpMenu->Append(ABOUT, _T("&About..."), _T("Displays information about the game"));

	m_menuBar = new wxMenuBar;
	m_menuBar->Append(gameMenu,    _T("&Game"));
	m_menuBar->Append(editMenu,    _T("&Edit"));
	m_menuBar->Append(optionsMenu, _T("&Options"));
	m_menuBar->Append(helpMenu,    _T("&Help"));

	SetMenuBar(m_menuBar);

        if (largecards)
            Card::SetScale(1.3);

	m_canvas = new FortyCanvas(this, 0, 0, 400, 400);
	wxLayoutConstraints* constr = new wxLayoutConstraints;
	constr->left.SameAs(this, wxLeft);
	constr->top.SameAs(this, wxTop);
	constr->right.SameAs(this, wxRight);
	constr->height.SameAs(this, wxHeight);
	m_canvas->SetConstraints(constr);

	CreateStatusBar();
}

FortyFrame::~FortyFrame()
{
}

void FortyFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (m_canvas->OnCloseCanvas() )
    {
        this->Destroy();
    }
    else
        event.Veto();
}

void
FortyFrame::NewGame(wxCommandEvent&)
{
	m_canvas->NewGame();
}

void
FortyFrame::Exit(wxCommandEvent&)
{
#ifdef __WXGTK__
	// wxGTK doesn't call OnClose() so we do it here
//	if (OnClose())
#endif
	Close(TRUE);
}

void
FortyFrame::About(wxCommandEvent&)
{
#if wxUSE_HTML
    if (wxFileExists(wxT("about.htm")))
    {
        FortyAboutDialog dialog(this, -1, wxT("About Forty Thieves"));
        if (dialog.ShowModal() == wxID_OK)
        {
        }
    }
    else
#endif
    {
        wxMessageBox(
            _T("Forty Thieves\n\n")
            _T("A freeware program using the wxWindows\n")
            _T("portable C++ GUI toolkit.\n")
            _T("http://www.wxwindows.org\n")
            _T("http://www.freiburg.linux.de/~wxxt\n\n")
            _T("Author: Chris Breeze (c) 1992-1998\n")
            _T("email: chris.breeze@iname.com"),
            _T("About Forty Thieves"),
            wxOK, this
            );
    }
}

void
FortyFrame::Undo(wxCommandEvent&)
{
	m_canvas->Undo();
}

void
FortyFrame::Redo(wxCommandEvent&)
{
	m_canvas->Redo();
}

void
FortyFrame::Scores(wxCommandEvent&)
{
	m_canvas->UpdateScores();
	ScoreDialog scores(this, m_canvas->GetScoreFile());
	scores.Display();
}

void
FortyFrame::ToggleRightButtonUndo(wxCommandEvent& event)
{
	bool checked = m_menuBar->IsChecked(event.GetId());
	m_canvas->EnableRightButtonUndo(checked);
}

void
FortyFrame::ToggleHelpingHand(wxCommandEvent& event)
{
	bool checked = m_menuBar->IsChecked(event.GetId());
	m_canvas->EnableHelpingHand(checked);
}

void
FortyFrame::ToggleCardSize(wxCommandEvent& event)
{
        bool checked = m_menuBar->IsChecked(event.GetId());
        Card::SetScale(checked ? 1.3 : 1);
        m_canvas->LayoutGame();
        m_canvas->Refresh();
}

//----------------------------------------------------------------------------
// stAboutDialog
//----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(FortyAboutDialog,wxDialog)
END_EVENT_TABLE()

FortyAboutDialog::FortyAboutDialog( wxWindow *parent, wxWindowID id, const wxString &title,
    const wxPoint &position, const wxSize& size, long style ) :
    wxDialog( parent, id, title, position, size, style )
{
    AddControls(this);

    Centre(wxBOTH);
}

bool FortyAboutDialog::AddControls(wxWindow* parent)
{
#if wxUSE_HTML
    wxString htmlText;
    wxString htmlFile(wxT("about.htm"));

    //if (!wxGetApp().GetMemoryTextResource(wxT("about.htm"), htmlText))
    {
//        wxSetWorkingDirectory(wxGetApp().GetAppDir());
//        wxString htmlFile(wxGetApp().GetFullAppPath(wxT("about.htm")));
        
        if (wxFileExists(htmlFile))
        {
            wxFile file;
            file.Open(htmlFile, wxFile::read);
            long len = file.Length();
            wxChar* buf = htmlText.GetWriteBuf(len + 1);
            file.Read(buf, len);
            buf[len] = 0;
            htmlText.UngetWriteBuf();
        }
    }

    if (htmlText.IsEmpty())
    {
        htmlText.Printf(wxT("<html><head><title>Warning</title></head><body><P>Sorry, could not find resource for About dialog<P></body></html>"));
    }

    // Customize the HTML
#if 0
    wxString verString;
    verString.Printf("%.2f", stVERSION_NUMBER);
    htmlText.Replace(wxT("$VERSION$"), verString);
#endif
    htmlText.Replace(wxT("$DATE$"), _T(__DATE__));

    wxSize htmlSize(400, 290);

    // Note: in later versions of wxWin this will be fixed so wxRAISED_BORDER
    // does the right thing. Meanwhile, this is a workaround.
#ifdef __WXMSW__
    long borderStyle = wxDOUBLE_BORDER;
#else
    long borderStyle = wxRAISED_BORDER;
#endif

    wxHtmlWindow* html = new wxHtmlWindow(this, ID_ABOUT_HTML_WINDOW, wxDefaultPosition, htmlSize, borderStyle);
    html -> SetBorders(10);
    html -> SetPage(htmlText);
        
    //// Start of sizer-based control creation

    wxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxWindow *item1 = parent->FindWindow( ID_ABOUT_HTML_WINDOW );
    wxASSERT( item1 );
    item0->Add( item1, 0, wxALIGN_CENTRE|wxALL, 5 );

    wxButton *item2 = new wxButton( parent, wxID_CANCEL, _T("&Close"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->SetDefault();
    item2->SetFocus();

    item0->Add( item2, 0, wxALIGN_RIGHT|wxALL, 5 );

    parent->SetAutoLayout( TRUE );
    parent->SetSizer( item0 );
    parent->Layout();
    item0->Fit( parent );
    item0->SetSizeHints( parent );
#endif

    return TRUE;
}

