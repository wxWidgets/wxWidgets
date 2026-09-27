/////////////////////////////////////////////////////////////////////////////
// Name:        taborder.cpp
// Purpose:     Sample for testing TAB navigation
// Author:      Vadim Zeitlin
// Copyright:   (c) 2007 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/log.h"
    #include "wx/frame.h"
    #include "wx/menu.h"
    #include "wx/sizer.h"

    #include "wx/panel.h"
    #include "wx/msgdlg.h"

    #include "wx/button.h"
    #include "wx/listbox.h"
    #include "wx/stattext.h"
    #include "wx/textctrl.h"
#endif

#include "wx/notebook.h"
#include "wx/scrolwin.h"

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif


// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// menu commands and controls ids
enum
{
    // file menu
    TabOrder_Quit = wxID_EXIT,
    TabOrder_About = wxID_ABOUT,

    // navigation menu
    TabOrder_TabForward = 200,
    TabOrder_TabBackward,

    TabOrder_Max
};

// status panes: first one is for temporary messages, the second one shows
// current focus
enum
{
    StatusPane_Default,
    StatusPane_Focus,
    StatusPane_Max
};

// ----------------------------------------------------------------------------
// declarations of the classes used in this sample
// ----------------------------------------------------------------------------

// the main application class
class MyApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

// and the main sample window
class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnAbout(wxCommandEvent& event);
    void OnQuit(wxCommandEvent& event);

    void OnTabForward(wxCommandEvent& event);
    void OnTabBackward(wxCommandEvent& event);

    void OnIdle(wxIdleEvent& event);

    void DoNavigate(int flags)
    {
        if ( m_panel->NavigateIn(flags) )
        {
            wxLogStatus(this, "Navigation event processed");
        }
        else
        {
            wxLogStatus(this, "Navigation event ignored");
        }
    }

    wxPanel *m_panel;

    wxDECLARE_EVENT_TABLE();
};

// and the panel taking up MyFrame client area
class MyPanel : public wxPanel
{
public:
    MyPanel(wxWindow *parent);

private:
    wxWindow *CreateButtonPage(wxWindow *parent);
    wxWindow *CreateTextPage(wxWindow *parent);
    wxWindow *CreateContainersPage(wxWindow *parent);
};

// a text control which checks if processing Tab presses in controls with
// wxTE_PROCESS_TAB style really works
class MyTabTextCtrl : public wxTextCtrl
{
public:
    MyTabTextCtrl(wxWindow *parent, const wxString& value, int flags = 0)
        : wxTextCtrl(parent, wxID_ANY, value,
                     wxDefaultPosition, wxDefaultSize,
                     flags)
    {
        Bind(wxEVT_KEY_DOWN, &MyTabTextCtrl::OnKeyDown, this);
    }

private:
    void OnKeyDown(wxKeyEvent& event)
    {
        if ( event.GetKeyCode() == WXK_TAB &&
                wxMessageBox
                (
                    "Let the Tab be used for navigation?",
                    "wxWidgets TabOrder sample: Tab key pressed",
                    wxICON_QUESTION | wxYES_NO,
                    this
                ) != wxYES )
        {
            // skip Skip() below: we consume the Tab press ourselves and so the
            // focus shouldn't change
            return;
        }

        event.Skip();
    }
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// MyApp
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    MyFrame *frame = new MyFrame;
    frame->Show(true);

    return true;
}

// ----------------------------------------------------------------------------
// MyFrame
// ----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(TabOrder_Quit,   MyFrame::OnQuit)
    EVT_MENU(TabOrder_About,  MyFrame::OnAbout)

    EVT_MENU(TabOrder_TabForward, MyFrame::OnTabForward)
    EVT_MENU(TabOrder_TabBackward, MyFrame::OnTabBackward)

    EVT_IDLE(MyFrame::OnIdle)
wxEND_EVENT_TABLE()

MyFrame::MyFrame()
       : wxFrame(nullptr, wxID_ANY, "TabOrder wxWidgets Sample")
{
    SetClientSize(FromDIP(wxSize(700, 700)));

    SetIcon(wxICON(sample));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(TabOrder_About);
    menuFile->AppendSeparator();
    menuFile->Append(TabOrder_Quit);

    wxMenu *menuNav = new wxMenu;
    menuNav->Append(TabOrder_TabForward, "Tab &forward\tCtrl-F",
                    "Emulate a <Tab> press");
    menuNav->Append(TabOrder_TabBackward, "Tab &backward\tCtrl-B",
                    "Emulate a <Shift-Tab> press");

    wxMenuBar *mbar = new wxMenuBar;
    mbar->Append(menuFile, "&File");
    mbar->Append(menuNav, "&Navigate");

    SetMenuBar(mbar);

    m_panel = new MyPanel(this);

    CreateStatusBar(StatusPane_Max);
}

void MyFrame::OnQuit(wxCommandEvent& WXUNUSED(event))
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxMessageBox("Tab navigation sample\n(c) 2007 Vadim Zeitlin",
                 "About TabOrder wxWidgets Sample", wxOK, this);
}

void MyFrame::OnTabForward(wxCommandEvent& WXUNUSED(event))
{
    DoNavigate(wxNavigationKeyEvent::IsForward | wxNavigationKeyEvent::FromTab);
}

void MyFrame::OnTabBackward(wxCommandEvent& WXUNUSED(event))
{
    DoNavigate(wxNavigationKeyEvent::IsBackward | wxNavigationKeyEvent::FromTab);
}

void MyFrame::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    // track the window which has the focus in the status bar
    static wxWindow *s_windowFocus = nullptr;
    wxWindow *focus = wxWindow::FindFocus();
    if ( focus != s_windowFocus )
    {
        s_windowFocus = focus;

        wxString msg;
        if ( focus )
        {
            msg.Printf("Focus is at %s", s_windowFocus->GetName());
        }
        else
        {
            msg = "No focus";
        }

        SetStatusText(msg, StatusPane_Focus);
    }
}

// ----------------------------------------------------------------------------
// MyPanel
// ----------------------------------------------------------------------------

MyPanel::MyPanel(wxWindow *parent)
       : wxPanel(parent, wxID_ANY)
{
    wxNotebook *notebook = new wxNotebook(this, wxID_ANY);
    notebook->AddPage(CreateButtonPage(notebook), "Button");
    notebook->AddPage(CreateTextPage(notebook), "Text");
    notebook->AddPage(CreateContainersPage(notebook), "Containers");

    wxSizer *sizerV = new wxBoxSizer(wxVERTICAL);
    sizerV->Add(notebook, wxSizerFlags(1).Expand());

    wxListBox *lbox = new wxListBox(this, wxID_ANY);
    lbox->AppendString("Just a");
    lbox->AppendString("simple");
    lbox->AppendString("listbox");
    sizerV->Add(lbox, wxSizerFlags(1).Expand());

    SetSizerAndFit(sizerV);
}

wxWindow *MyPanel::CreateButtonPage(wxWindow *parent)
{
    wxSizerFlags flagsBorder = wxSizerFlags().Border().Centre();

    wxPanel *page = new wxPanel(parent);
    wxSizer *sizerPage = new wxBoxSizer(wxHORIZONTAL);
    sizerPage->Add(new wxButton(page, wxID_ANY, "&First"), flagsBorder);
    sizerPage->Add(new wxStaticText(page, wxID_ANY, "[st&atic]"),
                   flagsBorder);
    sizerPage->Add(new wxButton(page, wxID_ANY, "&Second"), flagsBorder);

    page->SetSizer(sizerPage);

    return page;
}

wxWindow *MyPanel::CreateTextPage(wxWindow *parent)
{
    auto* const sizerPage = new wxFlexGridSizer(2, FromDIP(wxSize(5, 5)));
    sizerPage->AddGrowableCol(1);
    wxPanel *page = new wxPanel(parent);

    sizerPage->Add(new wxStaticText(page, wxID_ANY, "&Label:"),
                   wxSizerFlags().Right().CentreVertical());
    sizerPage->Add(new MyTabTextCtrl(page, "TAB ignored here"),
                   wxSizerFlags(1).Expand());

    sizerPage->Add(new wxStaticText(page, wxID_ANY, "&Another one:"),
                   wxSizerFlags().Right().CentreVertical());
    sizerPage->Add(new MyTabTextCtrl(page, "press Tab here", wxTE_PROCESS_TAB),
                    wxSizerFlags(1).Expand());

    page->SetSizer(sizerPage);

    return page;
}

wxWindow *MyPanel::CreateContainersPage(wxWindow *parent)
{
    const wxSizerFlags flagsBorder = wxSizerFlags().Expand().Border();

    wxPanel *page = new wxPanel(parent);
    wxSizer *sizerPage = new wxBoxSizer(wxVERTICAL);

    // Create a panel with a border, to make it visible, and optionally a
    // label inside it.
    const auto createPanel = [](wxWindow* parentPanel,
                                const wxString& name,
                                const wxString& label = wxString())
    {
        wxPanel* const panel = new wxPanel(parentPanel, wxID_ANY,
                                           wxDefaultPosition, wxDefaultSize,
                                           wxTAB_TRAVERSAL | wxBORDER_SIMPLE,
                                           name);
        panel->SetSizer(new wxBoxSizer(wxHORIZONTAL));
        if ( !label.empty() )
        {
            panel->GetSizer()->Add(new wxStaticText(panel, wxID_ANY, label),
                                   wxSizerFlags().Border());
        }

        return panel;
    };

    sizerPage->Add(new wxButton(page, wxID_ANY, "&Before"), flagsBorder);

    // TAB shouldn't stop on the panels containing only labels...
    sizerPage->Add(createPanel(page, "label-only panel",
                               "Only a label: TAB should skip this panel"),
                   flagsBorder);

    // ... even if they're nested.
    wxPanel* const outer = createPanel(page, "outer panel");
    outer->GetSizer()->Add(createPanel(outer, "inner panel",
                                       "Nested panel with only a label: "
                                       "TAB should skip both panels"),
                           flagsBorder);
    sizerPage->Add(outer, flagsBorder);

    // Unless they explicitly request it.
    wxPanel* const optIn = createPanel(page, "opt-in panel",
                                       "EnableFocusFromKeyboard() was called: "
                                       "TAB should stop here");
    optIn->EnableFocusFromKeyboard();
    sizerPage->Add(optIn, flagsBorder);

    // Or if they can be scrolled, as this can only be done from keyboard if
    // they have focus.
    auto* const scrolled = new wxScrolledWindow(page, wxID_ANY,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                wxVSCROLL | wxBORDER_SIMPLE,
                                                "scrollable panel");
    auto* const sizerScrolled = new wxBoxSizer(wxVERTICAL);
    for ( int n = 1; n <= 10; n++ )
    {
        sizerScrolled->Add(new wxStaticText(scrolled, wxID_ANY,
            wxString::Format("Scrollable panel line #%d: TAB should stop "
                             "here and arrows should scroll", n)));
    }
    scrolled->SetSizer(sizerScrolled);
    scrolled->SetScrollRate(0, FromDIP(10));
    scrolled->SetMinSize(FromDIP(wxSize(-1, 40)));
    sizerPage->Add(scrolled, flagsBorder);

    // Check that adding focusable children to a panel containing only labels
    // makes TAB stop at them, both when the panel is already shown and when
    // it is shown later (only the first click on the corresponding button
    // tests the latter, subsequent ones just add more text controls).
    wxPanel* const dynamic = createPanel(page, "dynamic panel",
                                         "Initially only a label");
    sizerPage->Add(dynamic, flagsBorder);

    wxPanel* const hidden = createPanel(page, "hidden panel",
                                        "Initially hidden and only a label");
    hidden->Hide();
    sizerPage->Add(hidden, flagsBorder);

    wxSizer* const sizerButtons = new wxBoxSizer(wxHORIZONTAL);

    wxButton* const btnAdd = new wxButton(page, wxID_ANY,
                                          "&Add text to dynamic panel");
    btnAdd->Bind(wxEVT_BUTTON, [this, page, dynamic](wxCommandEvent&)
        {
            dynamic->GetSizer()->Add(new wxTextCtrl(dynamic, wxID_ANY,
                                                    "TAB should stop here"),
                                     wxSizerFlags(1).Border());
            page->InvalidateBestSize();
            Layout();
        });
    sizerButtons->Add(btnAdd, wxSizerFlags().Border(wxRIGHT));

    wxButton* const btnShow = new wxButton(page, wxID_ANY,
                                           "Add text to &hidden panel and show it");
    btnShow->Bind(wxEVT_BUTTON, [this, page, hidden](wxCommandEvent&)
        {
            hidden->GetSizer()->Add(new wxTextCtrl(hidden, wxID_ANY,
                                                   "TAB should stop here too"),
                                    wxSizerFlags(1).Border());
            hidden->Show();
            page->InvalidateBestSize();
            Layout();
        });
    sizerButtons->Add(btnShow);

    sizerPage->Add(sizerButtons, wxSizerFlags().Border());

    sizerPage->Add(new wxButton(page, wxID_ANY, "A&fter"), flagsBorder);

    page->SetSizer(sizerPage);

    return page;
}
