/////////////////////////////////////////////////////////////////////////////
// Name:        samples/notebook/notebook.cpp
// Purpose:     a sample demonstrating notebook usage
// Author:      Julian Smart
// Modified by: Dimitri Schoolwerth
// Created:     26/10/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998-2002 wxWidgets team
// Licence:     wxWindows licence
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
#include "wx/cshelp.h"
#include "wx/utils.h"
#include "notebook.h"

#if !defined(__WXMSW__) && !defined(__WXPM__)
    #include "../sample.xpm"
#endif

//-----------------------------------------------------------------------------
// MyApp
//-----------------------------------------------------------------------------

IMPLEMENT_APP(MyApp)

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

#if wxUSE_HELP
    wxHelpProvider::Set( new wxSimpleHelpProvider );
#endif

    // Create the main window
    MyFrame *frame = new MyFrame();

    // Problem with generic wxNotebook implementation whereby it doesn't size
    // properly unless you set the size again
#if defined(__WXMOTIF__)
    int width, height;
    frame->GetSize(& width, & height);
    frame->SetSize(wxDefaultCoord, wxDefaultCoord, width, height);
#endif

    frame->Show();

    return true;
}


//-----------------------------------------------------------------------------
// Creation functions
//-----------------------------------------------------------------------------

wxPanel *CreateUserCreatedPage(wxBookCtrlBase *parent)
{
    wxPanel *panel = new wxPanel(parent);

#if wxUSE_HELP
    panel->SetHelpText( wxT( "Panel with a Button" ) );
#endif

    (void) new wxButton( panel, wxID_ANY, wxT("Button"),
                         wxPoint(10, 10), wxDefaultSize );

    return panel;
}

wxPanel *CreateRadioButtonsPage(wxBookCtrlBase *parent)
{
    wxPanel *panel = new wxPanel(parent);

#if wxUSE_HELP
    panel->SetHelpText( wxT( "Panel with some Radio Buttons" ) );
#endif

    wxString animals[] =
      { wxT("Fox"), wxT("Hare"), wxT("Rabbit"),
        wxT("Sabre-toothed tiger"), wxT("T Rex") };

    wxRadioBox *radiobox1 = new wxRadioBox(panel, wxID_ANY, wxT("Choose one"),
        wxDefaultPosition, wxDefaultSize, 5, animals, 2, wxRA_SPECIFY_ROWS);

    wxString computers[] =
      { wxT("Amiga"), wxT("Commodore 64"), wxT("PET"),
        wxT("Another") };

    wxRadioBox *radiobox2 = new wxRadioBox(panel, wxID_ANY,
        wxT("Choose your favourite"), wxDefaultPosition, wxDefaultSize,
        4, computers, 0, wxRA_SPECIFY_COLS);

    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
    sizerPanel->Add(radiobox1, 2, wxEXPAND);
    sizerPanel->Add(radiobox2, 1, wxEXPAND);
    panel->SetSizer(sizerPanel);

    return panel;
}

wxPanel *CreateVetoPage(wxBookCtrlBase *parent)
{
    wxPanel *panel = new wxPanel(parent);

#if wxUSE_HELP
    panel->SetHelpText( wxT( "An empty panel" ) );
#endif

    (void) new wxStaticText( panel, wxID_ANY,
                             wxT("This page intentionally left blank"),
                             wxPoint(10, 10) );

    return panel;
}

wxPanel *CreateBigButtonPage(wxBookCtrlBase *parent)
{
    wxPanel *panel = new wxPanel(parent);

#if wxUSE_HELP
    panel->SetHelpText( wxT( "Panel with a maximized button" ) );
#endif

    wxButton *buttonBig = new wxButton(panel, wxID_ANY, wxT("Maximized button"));

    wxBoxSizer *sizerPanel = new wxBoxSizer(wxVERTICAL);
    sizerPanel->Add(buttonBig, 1, wxEXPAND);
    panel->SetSizer(sizerPanel);

    return panel;
}

wxPanel *CreateInsertPage(wxBookCtrlBase *parent)
{
    wxPanel *panel = new wxPanel(parent);

#if wxUSE_HELP
    panel->SetHelpText( wxT( "Maroon panel" ) );
#endif

    panel->SetBackgroundColour( wxColour( wxT("MAROON") ) );
    (void) new wxStaticText( panel, wxID_ANY,
                             wxT("This page has been inserted, not added."),
                             wxPoint(10, 10) );

    return panel;
}

int GetIconIndex(wxBookCtrlBase* bookCtrl)
{
    if (bookCtrl && bookCtrl->GetImageList())
    {
       int nImages = bookCtrl->GetImageList()->GetImageCount();
       if (nImages > 0)
       {
           return bookCtrl->GetPageCount() % nImages;
       }
    }

    return -1;
}

void CreateInitialPages(wxBookCtrlBase *parent)
{
    // Create and add some panels to the notebook

    wxPanel *panel = CreateRadioButtonsPage(parent);
    parent->AddPage( panel, RADIOBUTTONS_PAGE_NAME, false, GetIconIndex(parent) );

    panel = CreateVetoPage(parent);
    parent->AddPage( panel, VETO_PAGE_NAME, false, GetIconIndex(parent) );

    panel = CreateBigButtonPage(parent);
    parent->AddPage( panel, MAXIMIZED_BUTTON_PAGE_NAME, false, GetIconIndex(parent) );

    panel = CreateInsertPage(parent);
    parent->InsertPage( 0, panel, I_WAS_INSERTED_PAGE_NAME, false, GetIconIndex(parent) );

    parent->SetSelection(1);
}

wxPanel *CreatePage(wxBookCtrlBase *parent, const wxString&pageName)
{
    if ( pageName.Contains(INSERTED_PAGE_NAME) ||
            pageName.Contains(ADDED_PAGE_NAME) ||
                pageName.Contains(ADDED_SUB_PAGE_NAME) ||
                    pageName.Contains(ADDED_PAGE_NAME_BEFORE) )
        return CreateUserCreatedPage(parent);

    if ( pageName == I_WAS_INSERTED_PAGE_NAME )
        return CreateInsertPage(parent);

    if ( pageName == VETO_PAGE_NAME )
        return CreateVetoPage(parent);

    if ( pageName == RADIOBUTTONS_PAGE_NAME )
        return CreateRadioButtonsPage(parent);

    if ( pageName == MAXIMIZED_BUTTON_PAGE_NAME )
        return CreateBigButtonPage(parent);

    wxFAIL_MSG( wxT("unknown page name") );

    return NULL;
}


//-----------------------------------------------------------------------------
// MyFrame
//-----------------------------------------------------------------------------

BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    // File menu
    EVT_MENU_RANGE(ID_BOOK_NOTEBOOK, ID_BOOK_MAX, MyFrame::OnType)
    EVT_MENU_RANGE(ID_ORIENT_DEFAULT, ID_ORIENT_MAX, MyFrame::OnOrient)
    EVT_MENU(ID_SHOW_IMAGES, MyFrame::OnShowImages)
    EVT_MENU_RANGE(ID_FIXEDWIDTH, ID_HORZ_LAYOUT, MyFrame::OnStyle)
    EVT_MENU(wxID_EXIT, MyFrame::OnExit)

    // Operations menu
    EVT_MENU(ID_ADD_PAGE, MyFrame::OnAddPage)
    EVT_MENU(ID_ADD_PAGE_NO_SELECT, MyFrame::OnAddPageNoSelect)
    EVT_MENU(ID_INSERT_PAGE, MyFrame::OnInsertPage)
    EVT_MENU(ID_DELETE_CUR_PAGE, MyFrame::OnDeleteCurPage)
    EVT_MENU(ID_DELETE_LAST_PAGE, MyFrame::OnDeleteLastPage)
    EVT_MENU(ID_NEXT_PAGE, MyFrame::OnNextPage)
    EVT_MENU(ID_CHANGE_SELECTION, MyFrame::OnChangeSelection)
    EVT_MENU(ID_SET_SELECTION, MyFrame::OnSetSelection)

#if wxUSE_HELP
    EVT_MENU(ID_CONTEXT_HELP, MyFrame::OnContextHelp)
#endif // wxUSE_HELP
    EVT_MENU(ID_HITTEST, MyFrame::OnHitTest)

    // Book controls
#if wxUSE_NOTEBOOK
    EVT_NOTEBOOK_PAGE_CHANGED(wxID_ANY, MyFrame::OnNotebook)
    EVT_NOTEBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnNotebook)
#endif
#if wxUSE_LISTBOOK
    EVT_LISTBOOK_PAGE_CHANGED(wxID_ANY, MyFrame::OnListbook)
    EVT_LISTBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnListbook)
#endif
#if wxUSE_CHOICEBOOK
    EVT_CHOICEBOOK_PAGE_CHANGED(wxID_ANY, MyFrame::OnChoicebook)
    EVT_CHOICEBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnChoicebook)
#endif
#if wxUSE_TREEBOOK
    EVT_TREEBOOK_PAGE_CHANGED(wxID_ANY, MyFrame::OnTreebook)
    EVT_TREEBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnTreebook)

    EVT_MENU(ID_ADD_SUB_PAGE, MyFrame::OnAddSubPage)
    EVT_MENU(ID_ADD_PAGE_BEFORE, MyFrame::OnAddPageBefore)
    EVT_UPDATE_UI_RANGE(ID_ADD_PAGE_BEFORE, ID_ADD_SUB_PAGE,
                            MyFrame::OnUpdateTreeMenu)
#endif
#if wxUSE_TOOLBOOK
    EVT_TOOLBOOK_PAGE_CHANGED(wxID_ANY, MyFrame::OnToolbook)
    EVT_TOOLBOOK_PAGE_CHANGING(wxID_ANY, MyFrame::OnToolbook)
#endif

    // Update title in idle time
    EVT_IDLE(MyFrame::OnIdle)
END_EVENT_TABLE()

MyFrame::MyFrame()
    : wxFrame(NULL, wxID_ANY, wxString(wxT("wxWidgets book controls sample")))
{
#if wxUSE_HELP
    SetExtraStyle(wxFRAME_EX_CONTEXTHELP);
#endif // wxUSE_HELP

#if wxUSE_NOTEBOOK
    m_type = Type_Notebook;
#elif wxUSE_CHOICEBOOK
    m_type = Type_Choicebook;
#elif wxUSE_LISTBOOK
    m_type = Type_Listbook;
#elif wxUSE_TREEBOOK
    m_type = Type_Treebook;
#elif wxUSE_TOOLBOOK
    m_type = Type_Toolbook;
#else
    #error "Don't use Notebook sample without any book enabled in wxWidgets build!"
#endif

    m_orient = ID_ORIENT_DEFAULT;
    m_chkShowImages = true;
    m_fixedWidth = false;
    m_multi = false;
    m_noPageTheme = false;
    m_buttonBar = false;
    m_horzLayout = false;

    SetIcon(wxICON(sample));

    // menu of the sample
    wxMenu *menuType = new wxMenu;
#if wxUSE_NOTEBOOK
    menuType->AppendRadioItem(ID_BOOK_NOTEBOOK,   wxT("&Notebook\tCtrl-1"));
#endif
#if wxUSE_LISTBOOK
    menuType->AppendRadioItem(ID_BOOK_LISTBOOK,   wxT("&Listbook\tCtrl-2"));
#endif
#if wxUSE_CHOICEBOOK
    menuType->AppendRadioItem(ID_BOOK_CHOICEBOOK, wxT("&Choicebook\tCtrl-3"));
#endif
#if wxUSE_TREEBOOK
    menuType->AppendRadioItem(ID_BOOK_TREEBOOK,   wxT("&Treebook\tCtrl-4"));
#endif
#if wxUSE_TOOLBOOK
    menuType->AppendRadioItem(ID_BOOK_TOOLBOOK,   wxT("T&oolbook\tCtrl-5"));
#endif

    menuType->Check(ID_BOOK_NOTEBOOK + m_type, true);

    wxMenu *menuOrient = new wxMenu;
    menuOrient->AppendRadioItem(ID_ORIENT_DEFAULT, wxT("&Default\tAlt-0"));
    menuOrient->AppendRadioItem(ID_ORIENT_TOP,     wxT("&Top\tAlt-1"));
    menuOrient->AppendRadioItem(ID_ORIENT_BOTTOM,  wxT("&Bottom\tAlt-2"));
    menuOrient->AppendRadioItem(ID_ORIENT_LEFT,    wxT("&Left\tAlt-3"));
    menuOrient->AppendRadioItem(ID_ORIENT_RIGHT,   wxT("&Right\tAlt-4"));

    wxMenu *menuStyle = new wxMenu;
#if wxUSE_NOTEBOOK
    menuStyle->AppendCheckItem(ID_FIXEDWIDTH, wxT("&Fixed Width (wxNotebook)"));
    menuStyle->AppendCheckItem(ID_MULTI, wxT("&Multiple lines (wxNotebook)"));
    menuStyle->AppendCheckItem(ID_NOPAGETHEME, wxT("&No Page Theme (wxNotebook)"));
#endif
#if wxUSE_TOOLBOOK
    menuStyle->AppendCheckItem(ID_BUTTONBAR, wxT("&Button Bar (wxToolbook)"));
    menuStyle->AppendCheckItem(ID_HORZ_LAYOUT, wxT("&Horizontal layout (wxToolbook)"));
#endif

    wxMenu *menuPageOperations = new wxMenu;
    menuPageOperations->Append(ID_ADD_PAGE, wxT("&Add page\tAlt-A"));
    menuPageOperations->Append(ID_ADD_PAGE_NO_SELECT, wxT("&Add page (don't select)\tAlt-B"));
    menuPageOperations->Append(ID_INSERT_PAGE, wxT("&Insert page\tAlt-I"));
    menuPageOperations->Append(ID_DELETE_CUR_PAGE, wxT("&Delete current page\tAlt-D"));
    menuPageOperations->Append(ID_DELETE_LAST_PAGE, wxT("D&elete last page\tAlt-L"));
    menuPageOperations->Append(ID_NEXT_PAGE, wxT("&Next page\tAlt-N"));
#if wxUSE_TREEBOOK
    menuPageOperations->AppendSeparator();
    menuPageOperations->Append(ID_ADD_PAGE_BEFORE, wxT("Insert page &before\tAlt-B"));
    menuPageOperations->Append(ID_ADD_SUB_PAGE, wxT("Add s&ub page\tAlt-U"));
#endif
    menuPageOperations->AppendSeparator();
    menuPageOperations->Append(ID_CHANGE_SELECTION, wxT("&Change selection to 0\tCtrl-0"));
    menuPageOperations->Append(ID_SET_SELECTION, wxT("&Set selection to 0\tShift-Ctrl-0"));

    wxMenu *menuOperations = new wxMenu;
#if wxUSE_HELP
    menuOperations->Append(ID_CONTEXT_HELP, wxT("&Context help\tCtrl-F1"));
#endif // wxUSE_HELP
    menuOperations->Append(ID_HITTEST, wxT("&Hit test\tCtrl-H"));

    wxMenu *menuFile = new wxMenu;
    menuFile->Append(wxID_ANY, wxT("&Type"), menuType, wxT("Type of control"));
    menuFile->Append(wxID_ANY, wxT("&Orientation"), menuOrient, wxT("Orientation of control"));
    menuFile->AppendCheckItem(ID_SHOW_IMAGES, wxT("&Show images\tAlt-S"));
    menuFile->Append(wxID_ANY, wxT("&Style"), menuStyle, wxT("Style of control"));
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT, wxT("E&xit"), wxT("Quits the application"));
    menuFile->Check(ID_SHOW_IMAGES, m_chkShowImages);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, wxT("&File"));
    menuBar->Append(menuPageOperations, wxT("&Pages"));
    menuBar->Append(menuOperations, wxT("&Operations"));
    SetMenuBar(menuBar);

    // books creation
    m_panel    = NULL;
    m_bookCtrl = NULL;

    // create a dummy image list with a few icons
    const wxSize imageSize(32, 32);

    m_imageList = new wxImageList(imageSize.GetWidth(), imageSize.GetHeight());
    m_imageList->
        Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, imageSize));
    m_imageList->
        Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, imageSize));
    m_imageList->
        Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, imageSize));
    m_imageList->
        Add(wxArtProvider::GetIcon(wxART_ERROR, wxART_OTHER, imageSize));

    m_panel = new wxPanel(this);

#if USE_LOG
    m_text = new wxTextCtrl(m_panel, wxID_ANY, wxEmptyString,
                            wxDefaultPosition, wxDefaultSize,
                            wxTE_MULTILINE | wxTE_READONLY);

    m_logTargetOld = wxLog::SetActiveTarget( new wxLogTextCtrl(m_text) );
#endif // USE_LOG

    // Set sizers
    m_sizerFrame = new wxBoxSizer(wxVERTICAL);

#if USE_LOG
    m_sizerFrame->Add(m_text, 1, wxEXPAND);
#endif // USE_LOG

    RecreateBook();

    m_panel->SetSizer(m_sizerFrame);
    m_panel->Layout();

    wxBoxSizer *sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add(m_panel, wxSizerFlags(1).Expand());
    SetSizerAndFit(sizer);
}

MyFrame::~MyFrame()
{
#if USE_LOG
    delete wxLog::SetActiveTarget(m_logTargetOld);
#endif // USE_LOG

    delete m_imageList;
}

// DISPATCH_ON_TYPE() macro is an ugly way to write the "same" code for
// different wxBookCtrlBase-derived classes without duplicating code and
// without using templates, it expands into "before <xxx> after" where "xxx"
// part is control class-specific
#if wxUSE_NOTEBOOK
    #define CASE_NOTEBOOK(x) case Type_Notebook: x; break;
#else
    #define CASE_NOTEBOOK(x)
#endif

#if wxUSE_LISTBOOK
    #define CASE_LISTBOOK(x) case Type_Listbook: x; break;
#else
    #define CASE_LISTBOOK(x)
#endif

#if wxUSE_CHOICEBOOK
    #define CASE_CHOICEBOOK(x) case Type_Choicebook: x; break;
#else
    #define CASE_CHOICEBOOK(x)
#endif

#if wxUSE_TREEBOOK
    #define CASE_TREEBOOK(x) case Type_Treebook: x; break;
#else
    #define CASE_TREEBOOK(x)
#endif

#if wxUSE_TOOLBOOK
    #define CASE_TOOLBOOK(x) case Type_Toolbook: x; break;
#else
    #define CASE_TOOLBOOK(x)
#endif

#define DISPATCH_ON_TYPE(before, nb, lb, cb, tb, toolb, after)                \
    switch ( m_type )                                                         \
    {                                                                         \
        CASE_NOTEBOOK(before nb after)                                        \
        CASE_LISTBOOK(before lb after)                                        \
        CASE_CHOICEBOOK(before cb after)                                      \
        CASE_TREEBOOK(before tb after)                                        \
        CASE_TOOLBOOK(before toolb after)                                     \
                                                                              \
        default:                                                              \
            wxFAIL_MSG( wxT("unknown book control type") );                    \
    }

int MyFrame::TranslateBookFlag(int nb, int lb, int chb, int tbk, int toolbk) const
{
    int flag = 0;

    DISPATCH_ON_TYPE(flag =, nb,  lb,  chb,  tbk, toolbk, + 0);

    return flag;
}

void MyFrame::RecreateBook()
{
    int flags;
    switch ( m_orient )
    {
        case ID_ORIENT_TOP:
            flags = wxBK_TOP;
            break;

        case ID_ORIENT_BOTTOM:
            flags = wxBK_BOTTOM;
            break;

        case ID_ORIENT_LEFT:
            flags = wxBK_LEFT;
            break;

        case ID_ORIENT_RIGHT:
            flags = wxBK_RIGHT;
            break;

        default:
            flags = wxBK_DEFAULT;
    }

#if wxUSE_NOTEBOOK
    if ( m_fixedWidth && m_type == Type_Notebook )
        flags |= wxNB_FIXEDWIDTH;
    if ( m_multi && m_type == Type_Notebook )
        flags |= wxNB_MULTILINE;
    if ( m_noPageTheme && m_type == Type_Notebook )
        flags |= wxNB_NOPAGETHEME;
#endif
#if wxUSE_TOOLBOOK
    if ( m_buttonBar && m_type == Type_Toolbook )
        flags |= wxTBK_BUTTONBAR;
    if ( m_horzLayout && m_type == Type_Toolbook )
        flags |= wxTBK_HORZ_LAYOUT;
#endif

    wxBookCtrlBase *oldBook = m_bookCtrl;

    m_bookCtrl = NULL;

    DISPATCH_ON_TYPE(m_bookCtrl = new,
                         wxNotebook,
                         wxListbook,
                         wxChoicebook,
                         wxTreebook,
                         wxToolbook,
                     (m_panel, wxID_ANY, wxDefaultPosition, wxDefaultSize, flags));

    if ( !m_bookCtrl )
        return;

    m_bookCtrl->Hide();

    // wxToolbook doesn't work without icons so always use them for it.
    if ( m_chkShowImages || m_type == Type_Toolbook )
    {
        m_bookCtrl->SetImageList(m_imageList);
    }

    if ( oldBook )
    {
#if wxUSE_TREEBOOK
        // we only need the old treebook if we're recreating another treebook
        wxTreebook *tbkOld = m_type == Type_Treebook
                                ? wxDynamicCast(oldBook, wxTreebook)
                                : NULL;
#endif // wxUSE_TREEBOOK

        const int count = oldBook->GetPageCount();
        for ( int n = 0; n < count; n++ )
        {
            const int image = GetIconIndex(m_bookCtrl);
            const wxString str = oldBook->GetPageText(n);

            wxWindow *page = CreatePage(m_bookCtrl, str);

            // treebook complication: need to account for possible parent page
#if wxUSE_TREEBOOK
            if ( tbkOld )
            {
                const int parent = tbkOld->GetPageParent(n);
                if ( parent != wxNOT_FOUND )
                {
                    wxStaticCast(m_bookCtrl, wxTreebook)->
                        InsertSubPage(parent, page, str, false, image);

                    // skip adding it again below
                    continue;
                }
            }
#endif // wxUSE_TREEBOOK

            m_bookCtrl->AddPage(page, str, false, image);
        }

        const int sel = oldBook->GetSelection();
        if ( sel != wxNOT_FOUND )
            m_bookCtrl->SetSelection(sel);


        m_sizerFrame->Detach(oldBook);
        delete oldBook;
    }
    else // no old book
    {
        CreateInitialPages(m_bookCtrl);
    }

    m_sizerFrame->Insert(0, m_bookCtrl, wxSizerFlags(5).Expand().Border());

    m_sizerFrame->Show(m_bookCtrl);
    m_sizerFrame->Layout();
}

void MyFrame::AddFlagStrIfFlagPresent(wxString & flagStr, long flags, long flag,
                                      const wxChar * flagName) const
{
    if( (flags & flag) == flag )
    {
        if( !flagStr.empty() )
            flagStr += wxT(" | ");
        flagStr += flagName;
    }
}

wxPanel *MyFrame::CreateNewPage() const
{
    wxPanel *panel = new wxPanel(m_bookCtrl, wxID_ANY );

#if wxUSE_HELP
    panel->SetHelpText( wxT( "Panel with \"First\" and \"Second\" buttons" ) );
#endif

    (void) new wxButton(panel, wxID_ANY, wxT("First button"), wxPoint(10, 30));
    (void) new wxButton(panel, wxID_ANY, wxT("Second button"), wxPoint(150, 30));

    return panel;
}


//-----------------------------------------------------------------------------
// MyFrame - event handlers
//-----------------------------------------------------------------------------

#if wxUSE_HELP

void MyFrame::OnContextHelp(wxCommandEvent& WXUNUSED(event))
{
    // launches local event loop
    wxContextHelp ch( this );
}

#endif // wxUSE_HELP

void MyFrame::OnHitTest(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase * book = GetCurrentBook();
    const wxPoint pt = ::wxGetMousePosition();

    long flags;
    int pagePos = book->HitTest( book->ScreenToClient(pt), &flags );

    wxString flagsStr;

    AddFlagStrIfFlagPresent( flagsStr, flags, wxBK_HITTEST_NOWHERE, wxT("wxBK_HITTEST_NOWHERE") );
    AddFlagStrIfFlagPresent( flagsStr, flags, wxBK_HITTEST_ONICON,  wxT("wxBK_HITTEST_ONICON") );
    AddFlagStrIfFlagPresent( flagsStr, flags, wxBK_HITTEST_ONLABEL, wxT("wxBK_HITTEST_ONLABEL") );
    AddFlagStrIfFlagPresent( flagsStr, flags, wxBK_HITTEST_ONPAGE,  wxT("wxBK_HITTEST_ONPAGE") );

    wxLogMessage(wxT("HitTest at (%d,%d): %d: %s"),
                 pt.x,
                 pt.y,
                 pagePos,
                 flagsStr.c_str());
}

void MyFrame::OnType(wxCommandEvent& event)
{
    m_type = static_cast<BookType>(event.GetId() - ID_BOOK_NOTEBOOK);

    if ( m_bookCtrl )
        m_sizerFrame->Hide(m_bookCtrl);

    RecreateBook();
}

#if wxUSE_TREEBOOK

void MyFrame::OnUpdateTreeMenu(wxUpdateUIEvent& event)
{
    event.Enable(m_type == Type_Treebook);
}

#endif // wxUSE_TREEBOOK

void MyFrame::OnOrient(wxCommandEvent& event)
{
    m_orient = event.GetId();
    RecreateBook();
    m_sizerFrame->Layout();
}

void MyFrame::OnShowImages(wxCommandEvent& event)
{
    m_chkShowImages = event.IsChecked();
    RecreateBook();
    m_sizerFrame->Layout();
}

void MyFrame::OnStyle(wxCommandEvent& event)
{
    bool checked = event.IsChecked();
    switch (event.GetId())
    {
        case ID_FIXEDWIDTH:  m_fixedWidth = checked;  break;
        case ID_MULTI:       m_multi = checked;       break;
        case ID_NOPAGETHEME: m_noPageTheme = checked; break;
        case ID_BUTTONBAR:   m_buttonBar = checked;   break;
        case ID_HORZ_LAYOUT: m_horzLayout = checked;  break;
        default: break; // avoid compiler warning
    }

    RecreateBook();
    m_sizerFrame->Layout();
}

void MyFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void MyFrame::OnAddPage(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
    {
        static unsigned s_pageAdded = 0;
        currBook->AddPage(CreateNewPage(),
                          wxString::Format
                          (
                            ADDED_PAGE_NAME wxT("%u"),
                            ++s_pageAdded
                          ),
                          true,
                          GetIconIndex(currBook));
    }
}

void MyFrame::OnAddPageNoSelect(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
    {
        static unsigned s_pageAdded = 0;
        currBook->AddPage(CreateNewPage(),
                          wxString::Format
                          (
                            ADDED_PAGE_NAME wxT("%u"),
                            ++s_pageAdded
                          ),
                          false,
                          GetIconIndex(currBook));
    }
}

#if wxUSE_TREEBOOK
void MyFrame::OnAddSubPage(wxCommandEvent& WXUNUSED(event))
{
    wxTreebook *currBook = wxDynamicCast(GetCurrentBook(), wxTreebook);
    if ( currBook )
    {
        const int selPos = currBook->GetSelection();
        if ( selPos == wxNOT_FOUND )
        {
            wxLogError(wxT("Select the parent page first!"));
            return;
        }

        static unsigned s_subPageAdded = 0;
        currBook->InsertSubPage
                  (
                    selPos,
                    CreateNewPage(),
                    wxString::Format
                    (
                     ADDED_SUB_PAGE_NAME wxT("%u"),
                     ++s_subPageAdded
                    ),
                    true,
                    GetIconIndex(currBook)
                  );
    }
}

void MyFrame::OnAddPageBefore(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();
    if ( currBook )
    {
        const int selPos = currBook->GetSelection();
        if ( selPos == wxNOT_FOUND )
        {
            wxLogError(wxT("Select the parent page first!"));
            return;
        }

        static unsigned s_subPageAdded = 0;
        currBook->InsertPage(selPos,
                             CreateNewPage(),
                             wxString::Format
                             (
                                ADDED_PAGE_NAME_BEFORE wxT("%u"),
                                ++s_subPageAdded
                             ),
                             true,
                             GetIconIndex(currBook));
    }
}
#endif // wxUSE_TREEBOOK

void MyFrame::OnInsertPage(wxCommandEvent& WXUNUSED(event))
{
    static unsigned s_pageIns = 0;

    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
    {
        wxPanel *panel = CreateUserCreatedPage( currBook );

        currBook->InsertPage( 0, panel,
            wxString::Format(INSERTED_PAGE_NAME wxT("%u"), ++s_pageIns), false,
            GetIconIndex(currBook) );

        currBook->SetSelection(0);
    }
}

void MyFrame::OnDeleteCurPage(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
    {
        int sel = currBook->GetSelection();

        if (sel != wxNOT_FOUND)
        {
            currBook->DeletePage(sel);
        }
    }
}

void MyFrame::OnDeleteLastPage(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
    {
        int page = currBook->GetPageCount();

        if ( page != 0 )
        {
            currBook->DeletePage(page - 1);
        }
    }
}

void MyFrame::OnNextPage(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
    {
        currBook->AdvanceSelection();
    }
}

void MyFrame::OnChangeSelection(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
        currBook->ChangeSelection(0);
}

void MyFrame::OnSetSelection(wxCommandEvent& WXUNUSED(event))
{
    wxBookCtrlBase *currBook = GetCurrentBook();

    if ( currBook )
        currBook->SetSelection(0);
}

void MyFrame::OnIdle( wxIdleEvent& WXUNUSED(event) )
{
    static int s_nPages = wxNOT_FOUND;
    static int s_nSel = wxNOT_FOUND;
    static wxBookCtrlBase *s_currBook = NULL;

    wxBookCtrlBase *currBook = GetCurrentBook();

    int nPages = currBook ? currBook->GetPageCount() : 0;
    int nSel = currBook ? currBook->GetSelection() : wxNOT_FOUND;

    if ( nPages != s_nPages || nSel != s_nSel || s_currBook != currBook )
    {
        s_nPages = nPages;
        s_nSel = nSel;
        s_currBook = currBook;

        wxString selection;
        if ( nSel == wxNOT_FOUND )
            selection << wxT("not found");
        else
            selection << nSel;

        wxString title;
        title.Printf(wxT("Notebook and friends (%d pages, selection: %s)"), nPages, selection.c_str());

        SetTitle(title);
    }
}

void MyFrame::OnBookCtrl(wxBookCtrlBaseEvent& event)
{
    static const struct EventInfo
    {
        wxEventType typeChanged,
                    typeChanging;
        const wxChar *name;
    } events[] =
    {
#if wxUSE_NOTEBOOK
        {
            wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGED,
            wxEVT_COMMAND_NOTEBOOK_PAGE_CHANGING,
            wxT("wxNotebook")
        },
#endif // wxUSE_NOTEBOOK
#if wxUSE_LISTBOOK
        {
            wxEVT_COMMAND_LISTBOOK_PAGE_CHANGED,
            wxEVT_COMMAND_LISTBOOK_PAGE_CHANGING,
            wxT("wxListbook")
        },
#endif // wxUSE_LISTBOOK
#if wxUSE_CHOICEBOOK
        {
            wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGED,
            wxEVT_COMMAND_CHOICEBOOK_PAGE_CHANGING,
            wxT("wxChoicebook")
        },
#endif // wxUSE_CHOICEBOOK
#if wxUSE_TREEBOOK
        {
            wxEVT_COMMAND_TREEBOOK_PAGE_CHANGED,
            wxEVT_COMMAND_TREEBOOK_PAGE_CHANGING,
            wxT("wxTreebook")
        },
#endif // wxUSE_TREEBOOK
#if wxUSE_TOOLBOOK
        {
            wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGED,
            wxEVT_COMMAND_TOOLBOOK_PAGE_CHANGING,
            wxT("wxToolbook")
        },
#endif // wxUSE_TOOLBOOK
    };


    wxString nameEvent,
             nameControl,
             veto;
    const wxEventType eventType = event.GetEventType();

    // NB: can't use wxStaticCast here as wxBookCtrlBase is not in
    //     wxRTTI
    const wxBookCtrlBase * const
        book = static_cast<wxBookCtrlBase *>(event.GetEventObject());

    for ( size_t n = 0; n < WXSIZEOF(events); n++ )
    {
        const EventInfo& ei = events[n];
        if ( eventType == ei.typeChanged )
        {
            nameEvent = wxT("Changed");
        }
        else if ( eventType == ei.typeChanging )
        {
            const int idx = event.GetOldSelection();

            if ( idx != wxNOT_FOUND &&
                    book && book->GetPageText(idx) == VETO_PAGE_NAME )
            {
                if ( wxMessageBox
                     (
                      wxT("Are you sure you want to leave this page?\n")
                      wxT("(This demonstrates veto-ing)"),
                      wxT("Notebook sample"),
                      wxICON_QUESTION | wxYES_NO,
                      this
                     ) != wxYES )
                {
                    event.Veto();
                    veto = wxT(" (vetoed)");
                }
            }

            nameEvent = wxT("Changing");
        }
        else // skip end of the loop
        {
            continue;
        }

        nameControl = ei.name;
        break;
    }

    static int s_num = 0;

    wxLogMessage(wxT("Event #%d: %s: %s (%d) new sel %d, old %d, current %d%s"),
                 ++s_num,
                 nameControl.c_str(),
                 nameEvent.c_str(),
                 eventType,
                 event.GetSelection(),
                 event.GetOldSelection(),
                 book->GetSelection(),
                 veto.c_str());

#if USE_LOG
    m_text->SetInsertionPointEnd();
#endif
}
