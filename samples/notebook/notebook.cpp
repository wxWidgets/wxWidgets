/////////////////////////////////////////////////////////////////////////////
// Name:        samples/notebook/notebook.cpp
// Purpose:     a sample demonstrating notebook usage
// Author:      Julian Smart
// Modified by: Dimitri Schoolwerth
// Created:     26/10/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2002 wxWindows team
// License:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/imaglist.h"
#include "wx/artprov.h"
#include "notebook.h"

// Name of the Veto page
#define VETO_PAGE_NAME wxT("Veto")

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    // Create the main window
    MyFrame *frame = new MyFrame( wxT("Notebook sample") );

    // Problem with generic wxNotebook implementation whereby it doesn't size
    // properly unless you set the size again
#if defined(__WIN16__) || defined(__WXMOTIF__)
    int width, height;
    frame->GetSize(& width, & height);
    frame->SetSize(-1, -1, width, height);
#endif

    frame->Show();

    return TRUE;
}

MyNotebook::MyNotebook(wxWindow *parent, wxWindowID id,
    const wxPoint& pos, const wxSize& size, long style)
    : wxNotebook(parent, id, pos, size, style)
{
    // Empty
}

void MyNotebook::CreateInitialPages()
{
    wxPanel *panel = (wxPanel *) NULL;
    wxBoxSizer *sizerPanel = (wxBoxSizer *) NULL;

    // Create and add some panels to the notebook


    // Panel 1
    panel = new wxPanel(this);

    sizerPanel = new wxBoxSizer(wxVERTICAL);
    sizerPanel->Add( new wxButton( panel, -1, wxT("Button") ) );
    sizerPanel->Add( new wxTextCtrl(panel, -1, wxT("1234"),
        wxDefaultPosition, wxSize(120, 150) ) );
    panel->SetSizer(sizerPanel);

    AddPage( panel, wxT("Controls without sizer"), TRUE, GetIconIndex() );


    // Panel 2
    panel = new wxPanel(this);

    wxString animals[] = { wxT("Fox"), wxT("Hare"), wxT("Rabbit"),
        wxT("Sabre-toothed tiger"), wxT("T Rex") };
    wxRadioBox *radiobox1 = new wxRadioBox(panel, -1, wxT("Choose one"),
        wxDefaultPosition, wxDefaultSize, 5, animals, 2, wxRA_SPECIFY_ROWS);

    wxString computers[] = { wxT("Amiga"), wxT("Commodore 64"), wxT("PET"),
        wxT("Another") };
    wxRadioBox *radiobox2 = new wxRadioBox(panel, -1,
        wxT("Choose your favourite"), wxDefaultPosition, wxDefaultSize,
        4, computers, 0, wxRA_SPECIFY_COLS);

    sizerPanel = new wxBoxSizer(wxVERTICAL);
    sizerPanel->Add(radiobox1, 2, wxEXPAND);
    sizerPanel->Add(radiobox2, 1, wxEXPAND);
    panel->SetSizer(sizerPanel);

    AddPage( panel, wxT("Radiobuttons"), FALSE, GetIconIndex() );


    // Panel 3
    panel = new wxPanel(this);
    (void) new wxStaticText( panel, -1,
        wxT("This page intentionally left blank"), wxPoint(10, 10) );
    AddPage( panel, VETO_PAGE_NAME, FALSE, GetIconIndex() );


    // Panel 4
    panel = new wxPanel(this);
    wxButton *buttonBig = new wxButton( panel, -1, wxT("Big button"),
        wxPoint(0, 0), wxSize(480, 360) );

    sizerPanel = new wxBoxSizer(wxVERTICAL);
    sizerPanel->Add(buttonBig, 1, wxEXPAND);
    panel->SetSizer(sizerPanel);

    AddPage( panel, wxT("Big button"), FALSE, GetIconIndex() );


    // Panel 5
    panel = new wxPanel(this);
    panel->SetBackgroundColour( wxColour( wxT("MAROON") ) );
    (void) new wxStaticText( panel, -1,
        wxT("This page has been inserted, not added."), wxPoint(10, 10) );
    InsertPage( 0, panel, wxT("Inserted"), FALSE, GetIconIndex() );


    SetSelection(2);

}

int MyNotebook::GetIconIndex() const
{
    if (m_imageList)
    {
       int nImages = m_imageList->GetImageCount();
       if (nImages > 0)
       {
           return GetPageCount() % nImages;
       }
    }

    return -1;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size,
                 long style)
    : wxFrame((wxWindow *) NULL, -1, title, pos, size, style)
{
    m_panel = (wxPanel *) NULL;
    m_notebook = (MyNotebook *) NULL;
    m_imageList = (wxImageList *) NULL;

    m_panel = new wxPanel(this, -1, wxDefaultPosition, wxDefaultSize,
        wxTAB_TRAVERSAL|wxCLIP_CHILDREN|wxNO_BORDER);

    // Create remaining controls

    // must be in sync with Orient enum
    wxString strOrientations[] =
    {
        wxT("&Top"),
        wxT("&Bottom"),
        wxT("&Left"),
        wxT("&Right"),
    };

    wxASSERT_MSG( WXSIZEOF(strOrientations) == ORIENT_MAX,
                  wxT("forgot to update something") );

    m_radioOrient = new wxRadioBox
        (
            m_panel, ID_RADIO_ORIENT,
            wxT("&Tab orientation"),
            wxDefaultPosition, wxDefaultSize,
            WXSIZEOF(strOrientations), strOrientations,
            1, wxRA_SPECIFY_COLS
        );

    m_chkShowImages = new wxCheckBox( m_panel, ID_CHK_SHOWIMAGES,
        wxT("&Show images") );

    m_btnAddPage = new wxButton( m_panel, ID_BTN_ADD_PAGE, wxT("&Add page") );

    m_btnInsertPage = new wxButton( m_panel, ID_BTN_INSERT_PAGE,
        wxT("&Insert page") );

    m_btnDeletePage = new wxButton( m_panel, ID_BTN_DELETE_PAGE,
        wxT("&Delete page") );

    m_btnNextPage = new wxButton( m_panel, ID_BTN_NEXT_PAGE,
        wxT("&Next page") );

    m_btnExit = new wxButton( m_panel, wxID_OK, wxT("&Exit") );
    m_btnExit->SetDefault();

    m_notebook = new MyNotebook(m_panel, ID_NOTEBOOK);

    m_text = new wxTextCtrl(m_panel, -1, wxEmptyString,
        wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY);

    m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(m_text) );

    // Create the notebook's panels
    m_notebook->CreateInitialPages();

    // Set sizers
    m_sizerFrame = new wxBoxSizer(wxVERTICAL);

    m_sizerTop = new wxBoxSizer(wxHORIZONTAL);

    wxBoxSizer *sizerLeft = new wxBoxSizer(wxVERTICAL);
    {
        sizerLeft->Add(m_radioOrient, 0, wxEXPAND);
        sizerLeft->Add(m_chkShowImages, 0, wxEXPAND | wxTOP, 4);

        sizerLeft->Add(0, 0, 1); // Spacer

        sizerLeft->Add(m_btnAddPage, 0, wxEXPAND | (wxTOP | wxBOTTOM), 4);
        sizerLeft->Add(m_btnInsertPage, 0, wxEXPAND | (wxTOP | wxBOTTOM), 4);
        sizerLeft->Add(m_btnDeletePage, 0, wxEXPAND | (wxTOP | wxBOTTOM), 4);
        sizerLeft->Add(m_btnNextPage, 0, wxEXPAND | (wxTOP | wxBOTTOM), 4);

        sizerLeft->Add(0, 0, 1); // Spacer

        sizerLeft->Add(m_btnExit, 0, wxEXPAND);
    }

    m_sizerTop->Add(sizerLeft, 0, wxEXPAND | wxALL, 4);


    m_sizerFrame->Add(m_sizerTop, 1, wxEXPAND);
    m_sizerFrame->Add(m_text, 0, wxEXPAND);

    ReInitNotebook();

    m_panel->SetSizer(m_sizerFrame);

    m_panel->SetAutoLayout(TRUE);

    m_sizerFrame->Fit(this);

    Centre(wxBOTH);

}

MyFrame::~MyFrame()
{
    delete wxLog::SetActiveTarget(m_logTargetOld);
}

void MyFrame::ReInitNotebook()
{
    int flags;

    switch ( m_radioOrient->GetSelection() )
    {
        default:
            wxFAIL_MSG( wxT("unknown notebook orientation") );
            // fall through

        case ORIENT_TOP:
            flags = wxNB_TOP;
            break;

        case ORIENT_BOTTOM:
            flags = wxNB_BOTTOM;
            break;

        case ORIENT_LEFT:
            flags = wxNB_LEFT;
            break;

        case ORIENT_RIGHT:
            flags = wxNB_RIGHT;
            break;
    }

    MyNotebook *notebook = m_notebook;


    m_notebook = new MyNotebook(m_panel, ID_NOTEBOOK,
                                wxDefaultPosition, wxDefaultSize,
                                flags);

    CreateImageList();

    if ( notebook )
    {
        int sel = notebook->GetSelection();

        int count = notebook->GetPageCount();
        for ( int n = 0; n < count; n++ )
        {
            wxNotebookPage *page = notebook->GetPage(0);
            page->Reparent(m_notebook);

            m_notebook->AddPage( page, notebook->GetPageText(0), FALSE,
                m_notebook->GetIconIndex() );

            notebook->RemovePage(0);
        }

        if (m_sizerNotebook)
        {
            m_sizerTop->Remove(m_sizerNotebook);
        }

        delete notebook;

        // restore selection
        if ( sel != -1 )
        {
            m_notebook->SetSelection(sel);
        }
    }

    m_sizerNotebook = new wxNotebookSizer(m_notebook);
    m_sizerTop->Add(m_sizerNotebook, 1, wxEXPAND | wxALL, 4);
    m_sizerTop->Layout();
}

void MyFrame::CreateImageList()
{
    if (m_imageList)
    {
        delete m_imageList;
        m_imageList = (wxImageList *) NULL;
    }

    if ( m_chkShowImages->IsChecked() )
    {
        // create a dummy image list with a few icons
        wxSize size(32, 32);

        m_imageList = new wxImageList( size.GetWidth(), size.GetHeight() );
        
        m_imageList->Add
            (
                wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, size)
            );

        m_imageList->Add
            (
                wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, size)
            );

        m_imageList->Add
            (
                wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, size)
            );

        m_imageList->Add
            (
                wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, size)
            );

        m_notebook->SetImageList(m_imageList);
    }
}


BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_RADIOBOX(ID_RADIO_ORIENT, MyFrame::OnCheckOrRadioBox)
    EVT_CHECKBOX(ID_CHK_SHOWIMAGES, MyFrame::OnCheckOrRadioBox)

    EVT_BUTTON(ID_BTN_ADD_PAGE, MyFrame::OnButtonAddPage)
    EVT_BUTTON(ID_BTN_INSERT_PAGE, MyFrame::OnButtonInsertPage)
    EVT_BUTTON(ID_BTN_DELETE_PAGE, MyFrame::OnButtonDeletePage)
    EVT_BUTTON(ID_BTN_NEXT_PAGE, MyFrame::OnButtonNextPage)
    EVT_BUTTON(wxID_OK, MyFrame::OnButtonExit)

    EVT_NOTEBOOK_PAGE_CHANGED(ID_NOTEBOOK, MyFrame::OnNotebook)
    EVT_NOTEBOOK_PAGE_CHANGING(ID_NOTEBOOK, MyFrame::OnNotebook)

    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

void MyFrame::OnCheckOrRadioBox(wxCommandEvent& event)
{
        ReInitNotebook();
}

void MyFrame::OnButtonAddPage( wxCommandEvent& WXUNUSED(event) )
{
    static size_t s_pageAdded = 0;

    wxPanel *panel = new wxPanel( m_notebook, -1 );
    (void) new wxButton( panel, -1, wxT("Button"),
        wxPoint(10, 10), wxSize(-1, -1) );

    m_notebook->AddPage(panel, wxString::Format(wxT("Added %u"),
        ++s_pageAdded), FALSE, m_notebook->GetIconIndex() );
}

void MyFrame::OnButtonInsertPage( wxCommandEvent& WXUNUSED(event) )
{
    static size_t s_pageIns = 0;

    wxPanel *panel = new wxPanel( m_notebook, -1 );
    (void) new wxButton( panel, -1, wxT("Button"),
        wxPoint(10, 10), wxSize(-1, -1) );

    m_notebook->InsertPage( 0, panel,
        wxString::Format(wxT("Inserted %u"), ++s_pageIns), FALSE,
        m_notebook->GetIconIndex() );

    m_notebook->SetSelection(0);
}

void MyFrame::OnButtonDeletePage( wxCommandEvent& WXUNUSED(event) )
{
    int sel = m_notebook->GetSelection();

    if (sel != -1)
    {
        m_notebook->DeletePage(sel);
    }
}

void MyFrame::OnButtonNextPage( wxCommandEvent& WXUNUSED(event) )
{
    m_notebook->AdvanceSelection();
}

void MyFrame::OnButtonExit( wxCommandEvent& WXUNUSED(event) )
{
    Close();
}

void MyFrame::OnNotebook(wxNotebookEvent& event)
{
    wxString str = wxT("Unknown notebook event");

    wxEventType eventType = event.GetEventType();

    if (eventType == wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED)
    {
        str = wxT("Notebook changed");
    }
    else if (eventType == wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING)
    {
        int idx = event.GetOldSelection();
        if ( idx != -1 && m_notebook->GetPageText(idx) == VETO_PAGE_NAME )
        {
            if
            (
                wxMessageBox(
                wxT("Are you sure you want to leave this notebook page?\n")
                wxT("(This demonstrates veto-ing)"),
                          wxT("Notebook sample"),
                          wxICON_QUESTION | wxYES_NO, this) != wxYES )
            {
                event.Veto();

                return;
            }

        }

        str = wxT("Notebook changing");
    }

    static int s_numNotebookEvents = 0;

    wxLogMessage(wxT("Notebook event #%d: %s (%d)"),
        s_numNotebookEvents++, str, eventType);

    m_text->SetInsertionPointEnd();

    event.Skip();
}

void MyFrame::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    static int s_nPages = -1;
    static int s_nSel = -1;

    int nPages = m_notebook->GetPageCount();
    int nSel = m_notebook->GetSelection();
    if ( nPages != s_nPages || nSel != s_nSel )
    {
        s_nPages = nPages;
        s_nSel = nSel;

        wxString title;
        title.Printf(wxT("Notebook (%d pages, selection: %d)"), nPages, nSel);

        SetTitle(title);
    }
}
