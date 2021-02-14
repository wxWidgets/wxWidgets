/////////////////////////////////////////////////////////////////////////////
// Name:        samples/docview/docview.cpp
// Purpose:     Document/view demo
// Author:      Julian Smart
// Modified by: Vadim Zeitlin: merge with the MDI version and general cleanup
// Created:     04/01/98
// Copyright:   (c) 1998 Julian Smart
//              (c) 2008 Vadim Zeitlin
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/*
    This sample show document/view support in wxWidgets.

    It can be run in several ways:
        * With "--mdi" command line option to use multiple MDI child frames
          for the multiple documents (this is the default).
        * With "--sdi" command line option to use multiple top level windows
          for the multiple documents
        * With "--single" command line option to support opening a single
          document only

    Notice that doing it like this somewhat complicates the code, you could
    make things much simpler in your own programs by using either
    wxDocParentFrame or wxDocMDIParentFrame unconditionally (and never using
    the single mode) instead of supporting all of them as this sample does.
 */

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
    #include "wx/stockitem.h"
#endif

#if !wxUSE_DOC_VIEW_ARCHITECTURE
    #error You must set wxUSE_DOC_VIEW_ARCHITECTURE to 1 in setup.h!
#endif

#include "wx/docview.h"
#include "wx/docmdi.h"

#if wxUSE_AUI
    #include "wx/aui/tabmdi.h"
#endif // wxUSE_AUI

#include "docview.h"
#include "doc.h"
#include "view.h"

#include "wx/cmdline.h"
#include "wx/config.h"

#ifdef __WXMAC__
    #include "wx/filename.h"
#endif

#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "doc.xpm"
    #include "chart.xpm"
    #include "notepad.xpm"
#endif

// ----------------------------------------------------------------------------
// MyApp implementation
// ----------------------------------------------------------------------------

wxIMPLEMENT_APP(MyApp);

wxBEGIN_EVENT_TABLE(MyApp, wxApp)
    EVT_MENU(wxID_ABOUT, MyApp::OnAbout)
wxEND_EVENT_TABLE()

MyApp::MyApp()
{
#if wxUSE_MDI_ARCHITECTURE
    m_mode = Mode_MDI;
#else
    m_mode = Mode_SDI;
#endif

    m_canvas = NULL;
    m_menuEdit = NULL;
}

// constants for the command line options names
namespace CmdLineOption
{

#if wxUSE_MDI_ARCHITECTURE
const char * const MDI = "mdi";
#endif // wxUSE_MDI_ARCHITECTURE
#if wxUSE_AUI
const char * const AUI = "aui";
#endif // wxUSE_AUI
const char * const SDI = "sdi";
const char * const SINGLE = "single";

} // namespace CmdLineOption

void MyApp::OnInitCmdLine(wxCmdLineParser& parser)
{
    wxApp::OnInitCmdLine(parser);

#if wxUSE_MDI_ARCHITECTURE
    parser.AddSwitch("", CmdLineOption::MDI,
                     "run in MDI mode: multiple documents, single window");
#endif // wxUSE_MDI_ARCHITECTURE
#if wxUSE_AUI
    parser.AddSwitch("", CmdLineOption::AUI,
                     "run in MDI mode using AUI: multiple documents, single window");
#endif // wxUSE_AUI
    parser.AddSwitch("", CmdLineOption::SDI,
                     "run in SDI mode: multiple documents, multiple windows");
    parser.AddSwitch("", CmdLineOption::SINGLE,
                     "run in single document mode");

    parser.AddParam("document-file",
                    wxCMD_LINE_VAL_STRING,
                    wxCMD_LINE_PARAM_MULTIPLE | wxCMD_LINE_PARAM_OPTIONAL);
}

bool MyApp::OnCmdLineParsed(wxCmdLineParser& parser)
{
    int numModeOptions = 0;

#if wxUSE_MDI_ARCHITECTURE
    if ( parser.Found(CmdLineOption::MDI) )
    {
        m_mode = Mode_MDI;
        numModeOptions++;
    }
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
    if ( parser.Found(CmdLineOption::AUI) )
    {
        m_mode = Mode_AUI;
        numModeOptions++;
    }
#endif // wxUSE_AUI

    if ( parser.Found(CmdLineOption::SDI) )
    {
        m_mode = Mode_SDI;
        numModeOptions++;
    }

    if ( parser.Found(CmdLineOption::SINGLE) )
    {
        m_mode = Mode_Single;
        numModeOptions++;
    }

    if ( numModeOptions > 1 )
    {
        wxLogError("Only a single option choosing the mode can be given.");
        return false;
    }

    // save any files given on the command line: we'll open them in OnInit()
    // later, after creating the frame
    for ( size_t i = 0; i != parser.GetParamCount(); ++i )
        m_filesFromCmdLine.push_back(parser.GetParam(i));

    return wxApp::OnCmdLineParsed(parser);
}

#ifdef __WXMAC__
void MyApp::MacNewFile()
{
    wxDocManager::GetDocumentManager()->CreateNewDocument();
}
#endif // __WXMAC__

bool MyApp::OnInit()
{
    if ( !wxApp::OnInit() )
        return false;

    ::wxInitAllImageHandlers();

    // Fill in the application information fields before creating wxConfig.
    SetVendorName("wxWidgets");
    SetAppName("wx_docview_sample");
    SetAppDisplayName("wxWidgets DocView Sample");

    //// Create a document manager
    wxDocManager *docManager = new wxDocManager;

    //// Create a template relating drawing documents to their views
    new wxDocTemplate(docManager, "Drawing", "*.drw", "", "drw",
                      "Drawing Doc", "Drawing View",
                      CLASSINFO(DrawingDocument), CLASSINFO(DrawingView));

    if ( m_mode == Mode_Single )
    {
        // If we've only got one window, we only get to edit one document at a
        // time. Therefore no text editing, just doodling.
        docManager->SetMaxDocsOpen(1);
    }
    else // multiple documents mode: allow documents of different types
    {
        // Create a template relating text documents to their views
        new wxDocTemplate(docManager, "Text", "*.txt;*.text", "", "txt;text",
                          "Text Doc", "Text View",
                          CLASSINFO(TextEditDocument), CLASSINFO(TextEditView));
        // Create a template relating image documents to their views
        new wxDocTemplate(docManager, "Image", "*.png;*.jpg", "", "png;jpg",
                          "Image Doc", "Image View",
                          CLASSINFO(ImageDocument), CLASSINFO(ImageView));
    }

    // create the main frame window
    wxFrame *frame = NULL;
    switch ( m_mode )
    {
#if wxUSE_MDI_ARCHITECTURE
        case Mode_MDI:
            frame = new wxDocMDIParentFrame(docManager, NULL, wxID_ANY,
                                            GetAppDisplayName(),
                                            wxDefaultPosition,
                                            wxSize(500, 400));
            break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
        case Mode_AUI:
            frame = new wxDocParentFrameAny<wxAuiMDIParentFrame>
                        (
                            docManager, NULL, wxID_ANY,
                            GetAppDisplayName(),
                            wxDefaultPosition,
                            wxSize(500, 400)
                        );
            break;
#endif // wxUSE_AUI

        case Mode_SDI:
        case Mode_Single:
            frame = new wxDocParentFrame(docManager, NULL, wxID_ANY,
                                         GetAppDisplayName(),
                                         wxDefaultPosition,
                                         wxSize(500, 400));
            break;
    }

    // and its menu bar
    wxMenu *menuFile = new wxMenu;

    menuFile->Append(wxID_NEW);
    menuFile->Append(wxID_OPEN);

    if ( m_mode == Mode_Single )
        AppendDocumentFileCommands(menuFile, true);

    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    // A nice touch: a history of files visited. Use this menu.
    docManager->FileHistoryUseMenu(menuFile);
#if wxUSE_CONFIG
    docManager->FileHistoryLoad(*wxConfig::Get());
#endif // wxUSE_CONFIG


    if ( m_mode == Mode_Single )
    {
        m_canvas = new MyCanvas(NULL, frame);
        m_menuEdit = CreateDrawingEditMenu();
    }

    CreateMenuBarForFrame(frame, menuFile, m_menuEdit);

    frame->SetIcon(wxICON(doc));
    frame->Centre();
    frame->Show();

    if ( m_filesFromCmdLine.empty() )
    {
        // on macOS the dialog will be shown by MacNewFile
#ifndef __WXMAC__
        docManager->CreateNewDocument();
#endif
    }
    else // we have files to open on command line
    {
        for ( size_t i = 0; i != m_filesFromCmdLine.size(); ++i )
            docManager->CreateDocument(m_filesFromCmdLine[i], wxDOC_SILENT);
    }

    return true;
}

int MyApp::OnExit()
{
    wxDocManager * const manager = wxDocManager::GetDocumentManager();
#if wxUSE_CONFIG
    manager->FileHistorySave(*wxConfig::Get());
#endif // wxUSE_CONFIG
    delete manager;

    return wxApp::OnExit();
}

void MyApp::AppendDocumentFileCommands(wxMenu *menu, bool supportsPrinting)
{
    menu->Append(wxID_CLOSE);
    menu->Append(wxID_SAVE);
    menu->Append(wxID_SAVEAS);
    menu->Append(wxID_REVERT, _("Re&vert..."));

    if ( supportsPrinting )
    {
        menu->AppendSeparator();
        menu->Append(wxID_PRINT);
        menu->Append(wxID_PRINT_SETUP, "Print &Setup...");
        menu->Append(wxID_PREVIEW);
    }
}

wxMenu *MyApp::CreateDrawingEditMenu()
{
    wxMenu * const menu = new wxMenu;
    menu->Append(wxID_UNDO);
    menu->Append(wxID_REDO);
    menu->AppendSeparator();
    menu->Append(wxID_CUT, "&Cut last segment");

    return menu;
}

void MyApp::CreateMenuBarForFrame(wxFrame *frame, wxMenu *file, wxMenu *edit)
{
    wxMenuBar *menubar = new wxMenuBar;

    menubar->Append(file, wxGetStockLabel(wxID_FILE));

    if ( edit )
        menubar->Append(edit, wxGetStockLabel(wxID_EDIT));

    wxMenu *help= new wxMenu;
    help->Append(wxID_ABOUT);
    menubar->Append(help, wxGetStockLabel(wxID_HELP));

    frame->SetMenuBar(menubar);
}

wxFrame *MyApp::CreateChildFrame(wxView *view, bool isCanvas)
{
    // create a child frame of appropriate class for the current mode
    wxFrame *subframe = NULL;
    wxDocument *doc = view->GetDocument();
    switch ( GetMode() )
#if wxUSE_MDI_ARCHITECTURE
    {
        case Mode_MDI:
            subframe = new wxDocMDIChildFrame
                           (
                                doc,
                                view,
                                wxStaticCast(GetTopWindow(), wxDocMDIParentFrame),
                                wxID_ANY,
                                "Child Frame",
                                wxDefaultPosition,
                                wxSize(300, 300)
                           );
            break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
        case Mode_AUI:
            subframe = new wxDocChildFrameAny<wxAuiMDIChildFrame, wxAuiMDIParentFrame>
                           (
                                doc,
                                view,
                                wxStaticCast(GetTopWindow(), wxAuiMDIParentFrame),
                                wxID_ANY,
                                "Child Frame",
                                wxDefaultPosition,
                                wxSize(300, 300)
                           );
            break;
#endif // wxUSE_AUI

        case Mode_SDI:
        case Mode_Single:
            subframe = new wxDocChildFrame
                           (
                                doc,
                                view,
                                wxStaticCast(GetTopWindow(), wxDocParentFrame),
                                wxID_ANY,
                                "Child Frame",
                                wxDefaultPosition,
                                wxSize(300, 300)
                           );

            subframe->Centre();
            break;
    }

    wxMenu *menuFile = new wxMenu;

    menuFile->Append(wxID_NEW);
    menuFile->Append(wxID_OPEN);
    AppendDocumentFileCommands(menuFile, isCanvas);
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuEdit;
    if ( isCanvas )
    {
        menuEdit = CreateDrawingEditMenu();

        doc->GetCommandProcessor()->SetEditMenu(menuEdit);
        doc->GetCommandProcessor()->Initialize();
    }
    else // text frame
    {
        menuEdit = new wxMenu;
        menuEdit->Append(wxID_COPY);
        menuEdit->Append(wxID_PASTE);
        menuEdit->Append(wxID_SELECTALL);
    }

    CreateMenuBarForFrame(subframe, menuFile, menuEdit);

    subframe->SetIcon(isCanvas ? wxICON(chrt) : wxICON(notepad));

    return subframe;
}

void MyApp::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString modeName;
    switch ( m_mode )
    {
#if wxUSE_MDI_ARCHITECTURE
        case Mode_MDI:
            modeName = "MDI";
            break;
#endif // wxUSE_MDI_ARCHITECTURE

#if wxUSE_AUI
        case Mode_AUI:
            modeName = "AUI";
            break;
#endif // wxUSE_AUI

        case Mode_SDI:
            modeName = "SDI";
            break;

        case Mode_Single:
            modeName = "single document";
            break;

        default:
            wxFAIL_MSG( "unknown mode ");
    }

    const int docsCount =
        wxDocManager::GetDocumentManager()->GetDocumentsVector().size();

    wxLogMessage
    (
        "This is the wxWidgets Document/View Sample\n"
        "running in %s mode.\n"
        "%d open documents.\n"
        "\n"
        "Authors: Julian Smart, Vadim Zeitlin\n"
        "\n"
        "Usage: docview [--{mdi,aui,sdi,single}]",
        modeName,
        docsCount
    );
}
