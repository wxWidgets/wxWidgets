/////////////////////////////////////////////////////////////////////////////
// Name:        wxib.cpp
// Purpose:     wxInstall Builder
// Author:      Julian Smart
// Author:      Brian Smith
// Modified by:
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

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

#if USE_GENERIC_TBAR
    #if !wxUSE_TOOLBAR_SIMPLE
        #error wxToolBarSimple is not compiled in, set wxUSE_TOOLBAR_SIMPLE \
               to 1 in setup.h and recompile the library.
    #else
        #include <wx/tbarsmpl.h>
    #endif
#endif // USE_GENERIC_TBAR

// ----------------------------------------------------------------------------
// resources
// ----------------------------------------------------------------------------

#if defined(__WXGTK__) || defined(__WXMOTIF__)
    #include "mondrian.xpm"
    #include "bitmaps/new.xpm"
    #include "bitmaps/open.xpm"
    #include "bitmaps/save.xpm"
    #include "bitmaps/copy.xpm"
    #include "bitmaps/cut.xpm"
    #include "bitmaps/preview.xpm"  // paste XPM
    #include "bitmaps/print.xpm"
    #include "bitmaps/help.xpm"
#endif // GTK or Motif

char appName[] = "wxInstall Builder";

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
            wxWindowID id = -1,
            const wxString& title = "wxInstall Builder",
            const wxPoint& pos = wxDefaultPosition,
            const wxSize& size = wxDefaultSize,
            long style = wxDEFAULT_FRAME_STYLE);

    void RecreateToolbar();

    void OnQuit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

	void OnNewScript(wxCommandEvent& event);
	void OnOpenScript(wxCommandEvent& event);
	void OnSaveScript(wxCommandEvent& event);
	void OnNewProject(wxCommandEvent& event);
	void OnOpenProject(wxCommandEvent& event);
	void OnSaveProject(wxCommandEvent& event);

    void OnCut(wxCommandEvent& event);
    void OnCopy(wxCommandEvent& event);
    void OnPaste(wxCommandEvent& event);

    void OnListBoxDoubleClick(wxCommandEvent& event);

#if USE_GENERIC_TBAR
    virtual wxToolBar *OnCreateToolBar(long style,
                                       wxWindowID id,
                                       const wxString& name );
#endif // USE_GENERIC_TBAR

private:
    bool                m_smallToolbar,
                        m_horzToolbar;
    size_t              m_rows;             // 1 or 2 only

    wxTextCtrl*         m_textWindow;
	wxListBox*          m_listBox;

    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

const int ID_TOOLBAR = 500;

enum
{
	ID_COMBO = 1000,
	ID_LISTBOX,
    wxID_NEW_SCRIPT,
	wxID_SAVE_SCRIPT,
	wxID_OPEN_SCRIPT,
	wxID_NEW_PROJECT,
	wxID_SAVE_PROJECT,
    wxID_OPEN_PROJECT
};

// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

// Notice that wxID_HELP will be processed for the 'About' menu and the toolbar
// help button.

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(wxID_EXIT, MyFrame::OnQuit)
    EVT_MENU(wxID_HELP, MyFrame::OnAbout)

    EVT_MENU(wxID_NEW_SCRIPT, MyFrame::OnNewScript)
    EVT_MENU(wxID_OPEN_SCRIPT, MyFrame::OnOpenScript)
    EVT_MENU(wxID_SAVE_SCRIPT, MyFrame::OnSaveScript)
    EVT_MENU(wxID_NEW_PROJECT, MyFrame::OnNewProject)
    EVT_MENU(wxID_OPEN_PROJECT, MyFrame::OnOpenProject)
    EVT_MENU(wxID_SAVE_PROJECT, MyFrame::OnSaveProject)

    EVT_MENU(wxID_CUT, MyFrame::OnCut)
    EVT_MENU(wxID_COPY, MyFrame::OnCopy)
    EVT_MENU(wxID_PASTE, MyFrame::OnPaste)

    EVT_LISTBOX_DCLICK(ID_LISTBOX, MyFrame::OnListBoxDoubleClick)
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
    MyFrame* frame = new MyFrame((wxFrame *) NULL, -1,
                                 "wxInstall Builder",
                                 wxPoint(100, 100), wxSize(450, 300));

	frame->SetAutoLayout(TRUE);

    frame->Show(TRUE);

    frame->SetStatusText("Welcome to wxWindows Install Builder");

    SetTopWindow(frame);

    return TRUE;
}

void MyFrame::RecreateToolbar()
{
    // delete and recreate the toolbar
    wxToolBarBase *toolBar = GetToolBar();
    delete toolBar;

    SetToolBar(NULL);

    long style = wxNO_BORDER | wxTB_FLAT | wxTB_DOCKABLE;
    style |= m_horzToolbar ? wxTB_HORIZONTAL : wxTB_VERTICAL;

    toolBar = CreateToolBar(style, ID_TOOLBAR);
    toolBar->SetMargins( 4, 4 );

    // Set up toolbar
    wxBitmap toolBarBitmaps[8];

    toolBarBitmaps[0] = wxBITMAP(new);
    toolBarBitmaps[1] = wxBITMAP(open);
    toolBarBitmaps[2] = wxBITMAP(save);
    toolBarBitmaps[3] = wxBITMAP(copy);
    toolBarBitmaps[4] = wxBITMAP(cut);
    toolBarBitmaps[5] = wxBITMAP(paste);
    toolBarBitmaps[6] = wxBITMAP(print);
    toolBarBitmaps[7] = wxBITMAP(help);

    if ( !m_smallToolbar )
    {
        int w = 2*toolBarBitmaps[0].GetWidth(),
            h = 2*toolBarBitmaps[0].GetHeight();
        for ( size_t n = 0; n < WXSIZEOF(toolBarBitmaps); n++ )
        {
            toolBarBitmaps[n] =
                wxImage(toolBarBitmaps[n]).Scale(w, h).ConvertToBitmap();
        }

        toolBar->SetToolBitmapSize(wxSize(w, h));
    }

#ifdef __WXMSW__
    int width = 24;
#else
    int width = 16;
#endif

    int currentX = 5;

    toolBar->AddTool(wxID_NEW_SCRIPT, toolBarBitmaps[0], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "New script");
    currentX += width + 5;
	toolBar->AddTool(wxID_OPEN_SCRIPT, toolBarBitmaps[1], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Open script");

    // neither the generic nor Motif native toolbars really support this
    currentX += width + 5;
    toolBar->AddTool(wxID_SAVE_SCRIPT, toolBarBitmaps[2], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Save script");
    currentX += width + 5;
    toolBar->AddTool(wxID_COPY, toolBarBitmaps[3], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Copy");
    currentX += width + 5;
    toolBar->AddTool(wxID_CUT, toolBarBitmaps[4], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Cut");
    currentX += width + 5;
    toolBar->AddTool(wxID_PASTE, toolBarBitmaps[5], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Paste");
    currentX += width + 5;
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_HELP, toolBarBitmaps[7], wxNullBitmap, FALSE, currentX, -1, (wxObject *) NULL, "Help button");

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
	m_listBox = new wxListBox(this, ID_LISTBOX, wxPoint(0,0), wxSize(-1, -1));

	m_listBox->Append("loadwxr");
	m_listBox->Append("closeold");
	m_listBox->Append("mleview");
	m_listBox->Append("setbutton");
	m_listBox->Append("getcheck");
	m_listBox->Append("message");
	m_listBox->Append("disable");
	m_listBox->Append("settext");
	m_listBox->Append("gettext");
	m_listBox->Append("grabfile");
	m_listBox->Append("remove");
	m_listBox->Append("system");
	m_listBox->Append("startinst");

	wxLayoutConstraints *b1 = new wxLayoutConstraints;
	b1->left.SameAs       (this, wxLeft, 0);
	b1->top.SameAs        (this, wxTop, 0);
	b1->width.PercentOf   (this, wxWidth, 20);
	b1->bottom.SameAs     (this, wxBottom, 0);
	m_listBox->SetConstraints(b1);

	m_textWindow = new wxTextCtrl(this, -1, "", wxPoint(0,0), wxSize(-1, -1), wxTE_MULTILINE);

	m_textWindow->AppendText("; Script generated by wxInstall Builder\n");

	wxLayoutConstraints *b2 = new wxLayoutConstraints;
	b2->top.SameAs        (this, wxTop, 0);
	b2->left.SameAs       (m_listBox, wxRight, 0);
	b2->width.PercentOf   (this, wxWidth, 80);
	b2->bottom.SameAs     (this, wxBottom, 0);
	m_textWindow->SetConstraints(b2);

	m_smallToolbar = TRUE;
    m_horzToolbar = TRUE;
    m_rows = 1;

    // Give it a status line
    CreateStatusBar();

    // Give it an icon
    SetIcon(wxICON(mondrian));

    wxMenu *fileMenu = new wxMenu;
    fileMenu->Append(wxID_NEW_SCRIPT, "&New Script", "New wxInstall Script" );
    fileMenu->Append(wxID_OPEN_SCRIPT, "&Open Script", "Open wxInstall Script" );
    fileMenu->Append(wxID_SAVE_SCRIPT, "&Save Script", "Save wxInstall Script" );
    fileMenu->AppendSeparator();
    /*fileMenu->Append(wxID_NEW_PROJECT, "N&ew Project", "New wxInstall Project" );
    fileMenu->Append(wxID_OPEN_PROJECT, "O&pen Project", "Open wxInstall Project" );
    fileMenu->Append(wxID_SAVE_PROJECT, "S&ave Project", "Save wxInstall Project" );
    fileMenu->AppendSeparator();*/
    fileMenu->Append(wxID_EXIT, "E&xit", "Quit wxInstall Builder" );

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, "&About", "About wxInstall Builder");

    wxMenuBar* menuBar = new wxMenuBar( wxMB_DOCKABLE );

    menuBar->Append(fileMenu, "&File");
    menuBar->Append(helpMenu, "&Help");

    // Associate the menu bar with the frame
    SetMenuBar(menuBar);

    // Create the toolbar
    RecreateToolbar();
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

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(TRUE);
}

void MyFrame::OnCut(wxCommandEvent& WXUNUSED(event))
{
    m_textWindow->Cut();
}

void MyFrame::OnCopy(wxCommandEvent& WXUNUSED(event))
{
    m_textWindow->Copy();
}

void MyFrame::OnPaste(wxCommandEvent& WXUNUSED(event))
{
    m_textWindow->Paste();
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    (void)wxMessageBox("wxInstall Builder by Brian Smith", "About wxInstall Builder");
}

void MyFrame::OnNewScript(wxCommandEvent& event)
{
    m_textWindow->Clear();
	m_textWindow->AppendText("; Script generated by wxInstall Builder\n");
}

void MyFrame::OnOpenScript(wxCommandEvent& event)
{
	wxFileDialog dialog2(this, "Open Script", "", "", "*.ini", 0);

	if (dialog2.ShowModal() == wxID_OK)
		m_textWindow->LoadFile(dialog2.GetPath());
}

void MyFrame::OnSaveScript(wxCommandEvent& event)
{
	wxFileDialog dialog2(this, "Save Script", "", "", "*.ini", 0);

	if (dialog2.ShowModal() == wxID_OK)
		m_textWindow->SaveFile(dialog2.GetPath());
}

void MyFrame::OnNewProject(wxCommandEvent& event)
{
}

void MyFrame::OnOpenProject(wxCommandEvent& event)
{
}

void MyFrame::OnSaveProject(wxCommandEvent& event)
{
}

void MyFrame::OnListBoxDoubleClick( wxCommandEvent &event )
{
	int item;

	item = m_listBox->GetSelection();

	switch(item)
	{
	case 0:
		{
			wxFileDialog dialog2(this, "Choose wxr filename", "", "", "*.wxr", 0);

			if (dialog2.ShowModal() == wxID_OK)
			{
				char tempbuf[1024] = "DialogEd ";
				wxString tmp = dialog2.GetFilename();

				m_textWindow->WriteText("loadwxr,");
				m_textWindow->WriteText(tmp);
				m_textWindow->WriteText("\n");

				strcat(tempbuf, tmp);
				wxExecute(tempbuf);
			}
		}
		break;
	case 1:
		{
			m_textWindow->WriteText("closeold\n");
		}
		break;
	case 2:
		{
			wxFileDialog dialog2(this, "Choose filename to view", "", "", "*.txt", 0);

			if (dialog2.ShowModal() == wxID_OK)
			{
				wxTextEntryDialog dialog3(this,
										  "Please enter name of the MLE widget",
										  appName,
										  "",
										  wxOK | wxCANCEL);

				if (dialog3.ShowModal() == wxID_OK)
				{
					m_textWindow->WriteText("mleview,");
					m_textWindow->WriteText(dialog3.GetValue());
					m_textWindow->WriteText(",");
					m_textWindow->WriteText(dialog2.GetFilename());
					m_textWindow->WriteText("\n");
				}
			}
		}
		break;
	case 3:
		{
			wxFileDialog dialog2(this, "Choose script to attach to button", "", "", "*.ini", 0);

			if (dialog2.ShowModal() == wxID_OK)
			{
				wxTextEntryDialog dialog3(this,
										  "Please enter ID of the button",
										  appName,
										  "",
										  wxOK | wxCANCEL);

				if (dialog3.ShowModal() == wxID_OK)
				{
					m_textWindow->WriteText("setbutton,");
					m_textWindow->WriteText(dialog3.GetValue());
					m_textWindow->WriteText(",");
					m_textWindow->WriteText(dialog2.GetFilename());
					m_textWindow->WriteText("\n");
				}
			}
		}
		break;
	case 4:
		{
			wxFileDialog dialog2(this, "Choose script to run if not checked", "", "", "*.ini", 0);

			if (dialog2.ShowModal() == wxID_OK)
			{
				wxTextEntryDialog dialog3(this,
										  "Please enter name of the checkbox widget",
										  appName,
										  "",
										  wxOK | wxCANCEL);

				if (dialog3.ShowModal() == wxID_OK)
				{
					m_textWindow->WriteText("getcheck,");
					m_textWindow->WriteText(dialog3.GetValue());
					m_textWindow->WriteText(",");
					m_textWindow->WriteText(dialog2.GetFilename());
					m_textWindow->WriteText("\n");
				}
			}
		}
		break;
	case 5:
		{
			wxTextEntryDialog dialog2(this,
									  "Please enter the message to display to the user",
									  appName,
									  "",
									  wxOK | wxCANCEL);

			if (dialog2.ShowModal() == wxID_OK)
			{
				m_textWindow->WriteText("message,\"");
				m_textWindow->WriteText(dialog2.GetValue());
				m_textWindow->WriteText("\"\n");
			}
		}
		break;
	case 6:
		{
			wxTextEntryDialog dialog2(this,
									  "Please enter the widget name to disable",
									  appName,
									  "",
									  wxOK | wxCANCEL);

			if (dialog2.ShowModal() == wxID_OK)
			{
				m_textWindow->WriteText("disable,");
				m_textWindow->WriteText(dialog2.GetValue());
				m_textWindow->WriteText("\n");
			}
		}
		break;
	case 7:
		{
			wxTextEntryDialog dialog2(this,
									  "Please enter text widget name to set from",
									  appName,
									  "",
									  wxOK | wxCANCEL);

			if (dialog2.ShowModal() == wxID_OK)
			{
				m_textWindow->WriteText("settext,");
				m_textWindow->WriteText(dialog2.GetValue());
				m_textWindow->WriteText("\n");
			}
		}
		break;
	case 8:
		{
			wxTextEntryDialog dialog2(this,
									  "Please enter text widget name to get from",
									  appName,
									  "",
									  wxOK | wxCANCEL);

			if (dialog2.ShowModal() == wxID_OK)
			{
				m_textWindow->WriteText("gettext,");
				m_textWindow->WriteText(dialog2.GetValue());
				m_textWindow->WriteText("\n");
			}
		}
		break;
	case 9:
		{
			wxFileDialog dialog2(this, "Choose file to grab", "", "", "*", 0);

			if (dialog2.ShowModal() == wxID_OK)
			{
				m_textWindow->WriteText("grabfile,");
				m_textWindow->WriteText(dialog2.GetFilename());
				m_textWindow->WriteText("\n");
			}
		}
		break;
	case 10:
		{
			wxFileDialog dialog2(this, "Choose file to remove", "", "", "*", 0);

			if (dialog2.ShowModal() == wxID_OK)
			{
				m_textWindow->WriteText("remove,");
				m_textWindow->WriteText(dialog2.GetFilename());
				m_textWindow->WriteText("\n");
			}
		}
		break;
	case 11:
		{
			wxTextEntryDialog dialog2(this,
									  "Please enter the command to execute",
									  appName,
									  "",
									  wxOK | wxCANCEL);

			if (dialog2.ShowModal() == wxID_OK)
			{
				m_textWindow->WriteText("system,");
				m_textWindow->WriteText(dialog2.GetValue());
				m_textWindow->WriteText("\n");
			}
		}
		break;
	case 12:
		{
			wxFileDialog dialog2(this, "Choose script to run at installation completion", "", "", "*.ini", 0);

			if (dialog2.ShowModal() == wxID_OK)
			{
				wxTextEntryDialog dialog3(this,
										  "Please enter name of the gauge widget",
										  appName,
										  "",
										  wxOK | wxCANCEL);

				if (dialog3.ShowModal() == wxID_OK)
				{
					m_textWindow->WriteText("startinst,");
					m_textWindow->WriteText(dialog3.GetValue());
					m_textWindow->WriteText(",");
					m_textWindow->WriteText(dialog2.GetFilename());
					m_textWindow->WriteText("\n");
				}
			}
		}
		break;
	}
}
