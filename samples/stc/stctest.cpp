//////////////////////////////////////////////////////////////////////////////
// File:        contrib/samples/stc/stctest.cpp
// Purpose:     STC test application
// Maintainer:  Otto Wyss
// Created:     2003-09-01
// Copyright:   (c) wxGuide
// Licence:     wxWindows licence
//////////////////////////////////////////////////////////////////////////////

//----------------------------------------------------------------------------
// headers
//----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// for all others, include the necessary headers (this file is usually all you
// need because it includes almost all 'standard' wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

//! wxWidgets headers
#include "wx/config.h"   // configuration support
#include "wx/filedlg.h"  // file dialog support
#include "wx/filename.h" // filename support
#include "wx/notebook.h" // notebook support
#include "wx/settings.h" // system settings
#include "wx/string.h"   // strings support
#include "wx/image.h"    // images support
#if wxUSE_PRINTING_ARCHITECTURE
#include "wx/paper.h"
#endif // wxUSE_PRINTING_ARCHITECTURE

//! application headers
#include "defsext.h"     // Additional definitions
#include "edit.h"        // Edit module
#include "prefs.h"       // Prefs

// Used by wxStyledTextCtrlMap
#include "wx/dcgraph.h"
#include "wx/overlay.h"
#include "wx/splitter.h"

//----------------------------------------------------------------------------
// resources
//----------------------------------------------------------------------------

// the application icon (under Windows it is in resources)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

//============================================================================
// declarations
//============================================================================

#define APP_NAME "STC-Test"
#define APP_DESCR "See http://wxguide.sourceforge.net/"

#define APP_MAINT "Otto Wyss"
#define APP_VENDOR "wxWidgets"
#define APP_COPYRIGTH "(C) 2003 Otto Wyss"
#define APP_LICENCE "wxWidgets"

#define APP_VERSION "0.1.alpha"

class AppBook;


//----------------------------------------------------------------------------
//! global application name
wxString *g_appname = nullptr;

#if wxUSE_PRINTING_ARCHITECTURE

//! global print data, to remember settings during the session
wxPrintData *g_printData = nullptr;
wxPageSetupDialogData *g_pageSetupData = nullptr;

#endif // wxUSE_PRINTING_ARCHITECTURE


class AppFrame;

//----------------------------------------------------------------------------
//! application APP_VENDOR-APP_NAME.
class App: public wxApp {
    friend class AppFrame;

public:
    //! the main function called during application start
    virtual bool OnInit () override;

    //! application exit function
    virtual int OnExit () override;

private:
    //! frame window
    AppFrame* m_frame;

    wxFrame* MinimalEditor();
    void ShowDocumentMap(wxWindow* parent);

protected:
    void OnMinimalEditor(wxCommandEvent&);
    void OnDocumentMap(wxCommandEvent&);
    wxDECLARE_EVENT_TABLE();
};

// created dynamically by wxWidgets
wxDECLARE_APP(App);

//----------------------------------------------------------------------------
//! frame of the application APP_VENDOR-APP_NAME.
class AppFrame: public wxFrame {
    friend class App;
    friend class AppBook;
    friend class AppAbout;

public:
    //! constructor
    AppFrame (const wxString &title);

    //! destructor
    ~AppFrame ();

    //! event handlers
    //! common
    void OnClose (wxCloseEvent &event);
    void OnAbout (wxCommandEvent &event);
    void OnExit (wxCommandEvent &event);
    //! file
    void OnFileOpen (wxCommandEvent &event);
    void OnFileSave (wxCommandEvent &event);
    void OnFileSaveAs (wxCommandEvent &event);
    void OnFileClose (wxCommandEvent &event);
    //! properties
    void OnProperties (wxCommandEvent &event);
    //! print
    void OnPrintSetup (wxCommandEvent &event);
    void OnPrintPreview (wxCommandEvent &event);
    void OnPrint (wxCommandEvent &event);
    //! edit events
    void OnEdit (wxCommandEvent &event);
    void OnContextMenu(wxContextMenuEvent& evt);

private:
    // edit object
    Edit *m_edit;
    void FileOpen (wxString fname);

    //! creates the application menu bar
    wxMenuBar *m_menuBar;
    void CreateMenu ();

    // print preview position and size
    wxRect DeterminePrintSize ();

    wxDECLARE_EVENT_TABLE();
};

//----------------------------------------------------------------------------
//! about box of the application APP_VENDOR-APP_NAME
class AppAbout: public wxDialog {

public:
    //! constructor
    AppAbout (wxWindow *parent,
              int milliseconds = 0,
              long style = 0);

    //! destructor
    ~AppAbout ();

    // event handlers
    void OnTimerEvent (wxTimerEvent &event);

private:
    // timer
    wxTimer *m_timer;

    wxDECLARE_EVENT_TABLE();
};


//============================================================================
// implementation
//============================================================================

wxIMPLEMENT_APP(App);


wxBEGIN_EVENT_TABLE(App, wxApp)
EVT_MENU(myID_WINDOW_MINIMAL, App::OnMinimalEditor)
EVT_MENU(myID_WINDOW_DOCMAP, App::OnDocumentMap)
wxEND_EVENT_TABLE()

//----------------------------------------------------------------------------
// App
//----------------------------------------------------------------------------

bool App::OnInit () {

    wxInitAllImageHandlers();

    // set application and vendor name
    SetAppName (APP_NAME);
    SetVendorName (APP_VENDOR);
    g_appname = new wxString ();
    g_appname->Append (APP_VENDOR);
    g_appname->Append ("-");
    g_appname->Append (APP_NAME);

#if wxUSE_PRINTING_ARCHITECTURE
    // initialize print data and setup
    g_printData = new wxPrintData;
    wxPrintPaperType *paper = wxThePrintPaperDatabase->FindPaperType(wxPAPER_A4);
    g_printData->SetPaperId(paper->GetId());
    g_printData->SetPaperSize(paper->GetSize());
    g_printData->SetOrientation(wxPORTRAIT);

    g_pageSetupData = new wxPageSetupDialogData;
    // copy over initial paper size from print record
    (*g_pageSetupData) = *g_printData;
#endif // wxUSE_PRINTING_ARCHITECTURE

    // create application frame
    m_frame = new AppFrame (*g_appname);

    // open application frame
    m_frame->Layout ();
    m_frame->Show (true);

    return true;
}

int App::OnExit () {

    // delete global appname
    delete g_appname;

#if wxUSE_PRINTING_ARCHITECTURE
    // delete global print data and setup
    if (g_printData) delete g_printData;
    if (g_pageSetupData) delete g_pageSetupData;
#endif // wxUSE_PRINTING_ARCHITECTURE

    return 0;
}

//----------------------------------------------------------------------------
// AppFrame
//----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE (AppFrame, wxFrame)
    // common
    EVT_CLOSE (                      AppFrame::OnClose)
    // file
    EVT_MENU (wxID_OPEN,             AppFrame::OnFileOpen)
    EVT_MENU (wxID_SAVE,             AppFrame::OnFileSave)
    EVT_MENU (wxID_SAVEAS,           AppFrame::OnFileSaveAs)
    EVT_MENU (wxID_CLOSE,            AppFrame::OnFileClose)
    // properties
    EVT_MENU (myID_PROPERTIES,       AppFrame::OnProperties)
    // print and exit
    EVT_MENU (wxID_PRINT_SETUP,      AppFrame::OnPrintSetup)
    EVT_MENU (wxID_PREVIEW,          AppFrame::OnPrintPreview)
    EVT_MENU (wxID_PRINT,            AppFrame::OnPrint)
    EVT_MENU (wxID_EXIT,             AppFrame::OnExit)
    // Menu items with standard IDs forwarded to the editor.
    EVT_MENU (wxID_CLEAR,            AppFrame::OnEdit)
    EVT_MENU (wxID_CUT,              AppFrame::OnEdit)
    EVT_MENU (wxID_COPY,             AppFrame::OnEdit)
    EVT_MENU (wxID_PASTE,            AppFrame::OnEdit)
    EVT_MENU (wxID_SELECTALL,        AppFrame::OnEdit)
    EVT_MENU (wxID_REDO,             AppFrame::OnEdit)
    EVT_MENU (wxID_UNDO,             AppFrame::OnEdit)
    EVT_MENU (wxID_FIND,             AppFrame::OnEdit)
    // And all our edit-related menu commands.
    EVT_MENU_RANGE (myID_EDIT_FIRST, myID_EDIT_LAST,
                                     AppFrame::OnEdit)
    // help
    EVT_MENU (wxID_ABOUT,            AppFrame::OnAbout)
    EVT_CONTEXT_MENU(                AppFrame::OnContextMenu)
wxEND_EVENT_TABLE ()

AppFrame::AppFrame (const wxString &title)
        : wxFrame (nullptr, wxID_ANY, title, wxDefaultPosition, wxSize(750,550))
{
    SetIcon(wxICON(sample));

    // initialize important variables
    m_edit = nullptr;

    // set icon and background
    SetTitle (*g_appname);
    SetBackgroundColour ("WHITE");

    // create menu
    m_menuBar = new wxMenuBar;
    CreateMenu ();

    // open first page
    m_edit = new Edit (this, wxID_ANY);
    m_edit->SetFocus();

    FileOpen ("stctest.cpp");
}

AppFrame::~AppFrame () {
}

// common event handlers
void AppFrame::OnClose (wxCloseEvent &event) {
    wxCommandEvent evt;
    OnFileClose (evt);
    if (m_edit && m_edit->Modified()) {
        if (event.CanVeto()) event.Veto (true);
        return;
    }
    Destroy();
}

void AppFrame::OnAbout (wxCommandEvent &WXUNUSED(event)) {
    AppAbout dlg(this);
}

void AppFrame::OnExit (wxCommandEvent &WXUNUSED(event)) {
    Close (true);
}

// file event handlers
void AppFrame::OnFileOpen (wxCommandEvent &WXUNUSED(event)) {
    if (!m_edit) return;
#if wxUSE_FILEDLG
    wxString fname;
    wxFileDialog dlg (this, "Open file", wxEmptyString, wxEmptyString, "Any file (*)|*",
                      wxFD_OPEN | wxFD_FILE_MUST_EXIST | wxFD_CHANGE_DIR);
    if (dlg.ShowModal() != wxID_OK) return;
    fname = dlg.GetPath ();
    FileOpen (fname);
#endif // wxUSE_FILEDLG
}

void AppFrame::OnFileSave (wxCommandEvent &WXUNUSED(event)) {
    if (!m_edit) return;
    if (!m_edit->Modified()) {
        wxMessageBox (_("There is nothing to save!"), _("Save file"),
                      wxOK | wxICON_EXCLAMATION);
        return;
    }
    m_edit->SaveFile ();
}

void AppFrame::OnFileSaveAs (wxCommandEvent &WXUNUSED(event)) {
    if (!m_edit) return;
#if wxUSE_FILEDLG
    wxString filename;
    wxFileDialog dlg (this, "Save file", wxEmptyString, wxEmptyString, "Any file (*)|*", wxFD_SAVE|wxFD_OVERWRITE_PROMPT);
    if (dlg.ShowModal() != wxID_OK) return;
    filename = dlg.GetPath();
    m_edit->SaveFile (filename);
#endif // wxUSE_FILEDLG
}

void AppFrame::OnFileClose (wxCommandEvent &WXUNUSED(event)) {
    if (!m_edit) return;
    if (m_edit->Modified()) {
        if (wxMessageBox (_("Text is not saved, save before closing?"), _("Close"),
                          wxYES_NO | wxICON_QUESTION) == wxYES) {
            m_edit->SaveFile();
            if (m_edit->Modified()) {
                wxMessageBox (_("Text could not be saved!"), _("Close abort"),
                              wxOK | wxICON_EXCLAMATION);
                return;
            }
        }
    }
    m_edit->SetFilename (wxEmptyString);
    m_edit->ClearAll();
    m_edit->SetSavePoint();
}

// properties event handlers
void AppFrame::OnProperties (wxCommandEvent &WXUNUSED(event)) {
    if (!m_edit) return;
    EditProperties dlg(m_edit, 0);
}

// print event handlers
void AppFrame::OnPrintSetup (wxCommandEvent &WXUNUSED(event)) {
#if wxUSE_PRINTING_ARCHITECTURE
    (*g_pageSetupData) = * g_printData;
    wxPageSetupDialog pageSetupDialog(this, g_pageSetupData);
    pageSetupDialog.ShowModal();
    (*g_printData) = pageSetupDialog.GetPageSetupData().GetPrintData();
    (*g_pageSetupData) = pageSetupDialog.GetPageSetupData();
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void AppFrame::OnPrintPreview (wxCommandEvent &WXUNUSED(event)) {
#if wxUSE_PRINTING_ARCHITECTURE
    wxPrintDialogData printDialogData( *g_printData);
    wxPrintPreview *preview =
        new wxPrintPreview (new EditPrint (m_edit),
                            new EditPrint (m_edit),
                            &printDialogData);
    if (!preview->IsOk()) {
        delete preview;
        wxMessageBox (_("There was a problem with previewing.\n\
                         Perhaps your current printer is not correctly?"),
                      _("Previewing"), wxOK);
        return;
    }
    wxRect rect = DeterminePrintSize();
    wxPreviewFrame *frame = new wxPreviewFrame (preview, this, _("Print Preview"));
    frame->SetSize (rect);
    frame->Centre(wxBOTH);
    frame->Initialize();
    frame->Show(true);
#endif // wxUSE_PRINTING_ARCHITECTURE
}

void AppFrame::OnPrint (wxCommandEvent &WXUNUSED(event)) {
#if wxUSE_PRINTING_ARCHITECTURE
    wxPrintDialogData printDialogData( *g_printData);
    wxPrinter printer (&printDialogData);
    EditPrint printout (m_edit);
    if (!printer.Print (this, &printout, true)) {
        if (wxPrinter::GetLastError() == wxPRINTER_ERROR) {
        wxMessageBox (_("There was a problem with printing.\n\
                         Perhaps your current printer is not correctly?"),
                      _("Previewing"), wxOK);
            return;
        }
    }
    (*g_printData) = printer.GetPrintDialogData().GetPrintData();
#endif // wxUSE_PRINTING_ARCHITECTURE
}

// edit events
void AppFrame::OnEdit (wxCommandEvent &event) {
    if (m_edit) m_edit->GetEventHandler()->ProcessEvent (event);
}

void AppFrame::OnContextMenu(wxContextMenuEvent& evt)
{
    wxPoint point = evt.GetPosition();
    // If from keyboard
    if ( point.x == -1 && point.y == -1 )
    {
        wxSize size = GetSize();
        point.x = size.x / 2;
        point.y = size.y / 2;
    }
    else
    {
        point = ScreenToClient(point);
    }

    wxMenu menu;
    menu.Append(wxID_ABOUT, "&About");
    menu.Append(wxID_EXIT, "E&xit");
    PopupMenu(&menu, point);
}

// private functions
void AppFrame::CreateMenu ()
{
    // File menu
    wxMenu *menuFile = new wxMenu;
    menuFile->Append (wxID_OPEN, _("&Open ..\tCtrl+O"));
    menuFile->Append (wxID_SAVE, _("&Save\tCtrl+S"));
    menuFile->Append (wxID_SAVEAS, _("Save &as ..\tCtrl+Shift+S"));
    menuFile->Append (wxID_CLOSE, _("&Close\tCtrl+W"));
    menuFile->AppendSeparator();
    menuFile->Append (myID_PROPERTIES, _("Proper&ties ..\tCtrl+Shift+T"));
    menuFile->AppendSeparator();
    menuFile->Append (wxID_PRINT_SETUP, _("Print Set&up .."));
    menuFile->Append (wxID_PREVIEW, _("Print Pre&view\tCtrl+Shift+P"));
    menuFile->Append (wxID_PRINT, _("&Print ..\tCtrl+P"));
    menuFile->AppendSeparator();
    menuFile->Append (wxID_EXIT, _("&Quit\tCtrl+Q"));

    // Edit menu
    wxMenu *menuEdit = new wxMenu;
    menuEdit->Append (wxID_UNDO, _("&Undo\tCtrl+Z"));
    menuEdit->Append (wxID_REDO, _("&Redo\tCtrl+Shift+Z"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_CUT, _("Cu&t\tCtrl+X"));
    menuEdit->Append (wxID_COPY, _("&Copy\tCtrl+C"));
    menuEdit->Append (wxID_PASTE, _("&Paste\tCtrl+V"));
    menuEdit->Append (wxID_CLEAR, _("&Delete\tDel"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_FIND, _("&Find\tCtrl+F"));
    menuEdit->Enable (wxID_FIND, false);
    menuEdit->Append (myID_FINDNEXT, _("Find &next\tF3"));
    menuEdit->Enable (myID_FINDNEXT, false);
    menuEdit->Append (myID_REPLACE, _("&Replace\tCtrl+H"));
    menuEdit->Enable (myID_REPLACE, false);
    menuEdit->Append (myID_REPLACENEXT, _("Replace &again\tShift+F4"));
    menuEdit->Enable (myID_REPLACENEXT, false);
    menuEdit->AppendSeparator();
    menuEdit->Append (myID_BRACEMATCH, _("&Match brace\tCtrl+M"));
    menuEdit->Append (myID_GOTO, _("&Goto\tCtrl+G"));
    menuEdit->Enable (myID_GOTO, false);
    menuEdit->AppendSeparator();
    menuEdit->Append (myID_INDENTINC, _("&Indent increase\tCtrl+I"));
    menuEdit->Append (myID_INDENTRED, _("I&ndent reduce\tShift+Ctrl+I"));
    menuEdit->AppendSeparator();
    menuEdit->Append (wxID_SELECTALL, _("&Select all\tCtrl+A"));
    menuEdit->Append (myID_SELECTLINE, _("Select &line\tCtrl+L"));

    // highlight submenu
    wxMenu *menuHighlight = new wxMenu;
    int Nr;
    for (Nr = 0; Nr < g_LanguagePrefsSize; Nr++) {
        menuHighlight->Append (myID_HIGHLIGHTFIRST + Nr,
                             g_LanguagePrefs [Nr].name);
    }

    // charset submenu
    wxMenu *menuCharset = new wxMenu;
    menuCharset->Append (myID_CHARSETANSI, _("&ANSI (Windows)"));
    menuCharset->Append (myID_CHARSETMAC, _("&MAC (Macintosh)"));

    // View menu
    wxMenu *menuView = new wxMenu;
    menuView->Append (myID_HIGHLIGHTLANG, _("&Highlight language .."), menuHighlight);
    menuView->AppendSeparator();
    menuView->AppendCheckItem (myID_FOLDTOGGLE, _("&Toggle current fold\tCtrl+T"));
    menuView->AppendCheckItem (myID_OVERTYPE, _("&Overwrite mode\tIns"));
    menuView->AppendCheckItem (myID_WRAPMODEON, _("&Wrap mode\tCtrl+U"));
    menuView->AppendSeparator();
    menuView->AppendCheckItem (myID_DISPLAYEOL, _("Show line &endings"));
    menuView->AppendCheckItem (myID_INDENTGUIDE, _("Show &indent guides"));
    menuView->AppendCheckItem (myID_LINENUMBER, _("Show line &numbers"));
    menuView->AppendCheckItem (myID_LONGLINEON, _("Show &long line marker"));
    menuView->AppendCheckItem (myID_WHITESPACE, _("Show white&space"));
    menuView->AppendSeparator();
    menuView->Append (myID_USECHARSET, _("Use &code page of .."), menuCharset);

    // Annotations menu
    wxMenu* menuAnnotations = new wxMenu;
    menuAnnotations->Append(myID_ANNOTATION_ADD, _("&Add or edit an annotation..."),
                            _("Add an annotation for the current line"));
    menuAnnotations->Append(myID_ANNOTATION_REMOVE, _("&Remove annotation"),
                            _("Remove the annotation for the current line"));
    menuAnnotations->Append(myID_ANNOTATION_CLEAR, _("&Clear all annotations"));

    wxMenu* menuAnnotationsStyle = new wxMenu;
    menuAnnotationsStyle->AppendRadioItem(myID_ANNOTATION_STYLE_HIDDEN, _("&Hidden"));
    menuAnnotationsStyle->AppendRadioItem(myID_ANNOTATION_STYLE_STANDARD, _("&Standard"));
    menuAnnotationsStyle->AppendRadioItem(myID_ANNOTATION_STYLE_BOXED, _("&Boxed"));
    menuAnnotations->AppendSubMenu(menuAnnotationsStyle, "&Style");

    // Indicators menu
    wxMenu* menuIndicators = new wxMenu;
    menuIndicators->Append(myID_INDICATOR_FILL, _("&Add indicator for selection"));
    menuIndicators->Append(myID_INDICATOR_CLEAR, _("&Clear indicator for selection"));

    wxMenu* menuIndicatorStyle = new wxMenu;
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_PLAIN, "Plain");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_SQUIGGLE, "Squiggle");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_TT, "TT");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_DIAGONAL, "Diagonal");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_STRIKE, "Strike");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_HIDDEN, "Hidden");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_BOX, "Box");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_ROUNDBOX, "Round box");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_STRAIGHTBOX, "Straight box");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_DASH, "Dash");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_DOTS, "Dots");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_SQUIGGLELOW, "Squiggle low");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_DOTBOX, "Dot box");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_SQUIGGLEPIXMAP, "Squiggle pixmap");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_COMPOSITIONTHICK, "Composition thick");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_COMPOSITIONTHIN, "Composition thin");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_FULLBOX, "Full box");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_TEXTFORE, "Text fore");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_POINT, "Point");
    menuIndicatorStyle->AppendRadioItem(myID_INDICATOR_STYLE_POINTCHARACTER, "Point character");
    menuIndicators->AppendSubMenu(menuIndicatorStyle, "&Style");

    // change case submenu
    wxMenu *menuChangeCase = new wxMenu;
    menuChangeCase->Append (myID_CHANGEUPPER, _("&Upper case"));
    menuChangeCase->Append (myID_CHANGELOWER, _("&Lower case"));

    // convert EOL submenu
    wxMenu *menuConvertEOL = new wxMenu;
    menuConvertEOL->Append (myID_CONVERTCR, _("CR (&Linux)"));
    menuConvertEOL->Append (myID_CONVERTCRLF, _("CR+LF (&Windows)"));
    menuConvertEOL->Append (myID_CONVERTLF, _("LF (&Macintosh)"));

    // Extra menu
    wxMenu *menuExtra = new wxMenu;
    menuExtra->AppendCheckItem (myID_READONLY, _("&Readonly mode"));
    menuExtra->AppendSeparator();
    menuExtra->Append (myID_CHANGECASE, _("Change &case to .."), menuChangeCase);
    menuExtra->AppendSeparator();
    menuExtra->Append (myID_CONVERTEOL, _("Convert line &endings to .."), menuConvertEOL);
    menuExtra->AppendCheckItem(myID_MULTIPLE_SELECTIONS, _("Toggle &multiple selections"));
    menuExtra->AppendCheckItem(myID_MULTI_PASTE, _("Toggle multi-&paste"));
    menuExtra->AppendCheckItem(myID_MULTIPLE_SELECTIONS_TYPING, _("Toggle t&yping on multiple selections"));
    menuExtra->AppendSeparator();
#if defined(__WXMSW__) && wxUSE_GRAPHICS_DIRECT2D
    wxMenu* menuTechnology = new wxMenu;
    menuTechnology->AppendRadioItem(myID_TECHNOLOGY_DEFAULT, _("&Default"));
    menuTechnology->AppendRadioItem(myID_TECHNOLOGY_DIRECTWRITE, _("Direct&Write"));
    menuExtra->AppendSubMenu(menuTechnology, _("&Technology"));
    menuExtra->AppendSeparator();
#endif
    menuExtra->AppendCheckItem (myID_CUSTOM_POPUP, _("C&ustom context menu"));

    // Window menu
    wxMenu *menuWindow = new wxMenu;
    menuWindow->Append(myID_WINDOW_MINIMAL, _("&Minimal editor"));
    menuWindow->Append(myID_WINDOW_DOCMAP, _("Document &map\tF2"));

    // Help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append (wxID_ABOUT, _("&About ..\tCtrl+D"));

    // construct menu
    m_menuBar->Append (menuFile, _("&File"));
    m_menuBar->Append (menuEdit, _("&Edit"));
    m_menuBar->Append (menuView, _("&View"));
    m_menuBar->Append (menuAnnotations, _("&Annotations"));
    m_menuBar->Append (menuIndicators, _("&Indicators"));
    m_menuBar->Append (menuExtra, _("E&xtra"));
    m_menuBar->Append (menuWindow, _("&Window"));
    m_menuBar->Append (menuHelp, _("&Help"));
    SetMenuBar (m_menuBar);

    m_menuBar->Check(myID_ANNOTATION_STYLE_BOXED, true);
}

void AppFrame::FileOpen (wxString fname)
{
    m_edit->LoadFile (wxFileName(fname).GetAbsolutePath());
    m_edit->SelectNone();
}

wxRect AppFrame::DeterminePrintSize () {

    wxSize scr = wxGetDisplaySize();

    // determine position and size (shifting 16 left and down)
    wxRect rect = GetRect();
    rect.x += 16;
    rect.y += 16;
    rect.width = wxMin (rect.width, (scr.x - rect.x));
    rect.height = wxMin (rect.height, (scr.x - rect.y));

    return rect;
}


//----------------------------------------------------------------------------
// AppAbout
//----------------------------------------------------------------------------

wxBEGIN_EVENT_TABLE (AppAbout, wxDialog)
    EVT_TIMER (myID_ABOUTTIMER, AppAbout::OnTimerEvent)
wxEND_EVENT_TABLE ()

AppAbout::AppAbout (wxWindow *parent,
                    int milliseconds,
                    long style)
        : wxDialog (parent, wxID_ANY, wxEmptyString,
                    wxDefaultPosition, wxDefaultSize,
                    style | wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER) {

    // set timer if any
    m_timer = nullptr;
    if (milliseconds > 0) {
        m_timer = new wxTimer (this, myID_ABOUTTIMER);
        m_timer->StartOnce(milliseconds);
    }

    // Get version of Scintilla
    wxString versionInfo = wxString::Format("%s (%s, %s)",
        APP_VERSION,
        wxStyledTextCtrl::GetLibraryVersionInfo().GetVersionString(),
        wxStyledTextCtrl::GetLexerVersionInfo().GetVersionString());

    // sets the application title
    SetTitle (_("About .."));

    // about info
    wxGridSizer *aboutinfo = new wxGridSizer (2, 0, 2);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Written by: ")),
                    0, wxALIGN_LEFT);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, APP_MAINT),
                    1, wxEXPAND | wxALIGN_LEFT);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Version: ")),
                    0, wxALIGN_LEFT);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, versionInfo),
                    1, wxEXPAND | wxALIGN_LEFT);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Licence type: ")),
                    0, wxALIGN_LEFT);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, APP_LICENCE),
                    1, wxEXPAND | wxALIGN_LEFT);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, _("Copyright: ")),
                    0, wxALIGN_LEFT);
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, APP_COPYRIGTH),
                    1, wxEXPAND | wxALIGN_LEFT);

    // about icontitle//info
    wxBoxSizer *aboutpane = new wxBoxSizer (wxHORIZONTAL);
    wxBitmap bitmap = wxBitmap(wxICON (sample));
    aboutpane->Add (new wxStaticBitmap (this, wxID_ANY, bitmap),
                    0, wxALIGN_LEFT|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT, 20);
    aboutpane->Add (aboutinfo, 1, wxEXPAND);
    aboutpane->Add (60, 0);

    // about complete
    wxBoxSizer *totalpane = new wxBoxSizer (wxVERTICAL);
    totalpane->Add (0, 20);
    wxStaticText *appname = new wxStaticText(this, wxID_ANY, *g_appname);
    appname->SetFont (wxFontInfo(24).Bold());
    totalpane->Add (appname, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT, 40);
    totalpane->Add (0, 10);
    totalpane->Add (aboutpane, 0, wxEXPAND | wxALL, 4);
    totalpane->Add (new wxStaticText(this, wxID_ANY, APP_DESCR),
                    0, wxALIGN_CENTER | wxALL, 10);
    wxButton *okButton = new wxButton (this, wxID_OK, _("OK"));
    okButton->SetDefault();
    totalpane->Add (okButton, 0, wxALIGN_CENTER | wxLEFT | wxRIGHT | wxBOTTOM, 10);

    SetSizerAndFit (totalpane);

    CenterOnScreen();
    ShowModal();
}

AppAbout::~AppAbout () {
    wxDELETE(m_timer);
}

//----------------------------------------------------------------------------
// event handlers
void AppAbout::OnTimerEvent (wxTimerEvent &WXUNUSED(event)) {
    wxDELETE(m_timer);
    EndModal (wxID_OK);
}

/////////////////////////////////////////////////////////////////////////////
// Minimal editor added by Troels K 2008-04-08
// Thanks to geralds for SetLexerXml() - http://wxforum.shadonet.com/viewtopic.php?t=7155

class MinimalEditor : public wxStyledTextCtrl
{
    enum
    {
        margin_id_lineno,
        margin_id_fold,
    };

public:
    MinimalEditor(wxWindow* parent, wxWindowID id = wxID_ANY) : wxStyledTextCtrl(parent, id)
    {
        SetLexerXml();

        SetProperty("fold", "1");
        SetProperty("fold.comment", "1");
        SetProperty("fold.compact", "1");
        SetProperty("fold.preprocessor", "1");
        SetProperty("fold.html", "1");
        SetProperty("fold.html.preprocessor", "1");

        SetMarginType(margin_id_lineno, wxSTC_MARGIN_NUMBER);
        SetMarginWidth(margin_id_lineno, 32);

        // We intentionally invert foreground and background colours here.
        const wxColour colFg = StyleGetForeground(wxSTC_STYLE_DEFAULT);
        const wxColour colBg = StyleGetBackground(wxSTC_STYLE_DEFAULT);
        MarkerDefine(wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_BOXPLUS,           colBg, colFg);
        MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_BOXMINUS,          colBg, colFg);
        MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE,             colBg, colFg);
        MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_BOXPLUSCONNECTED,  colBg, colFg);
        MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, colBg, colFg);
        MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER,           colBg, colFg);
        MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNER,           colBg, colFg);

        SetMarginMask(margin_id_fold, wxSTC_MASK_FOLDERS);
        SetMarginWidth(margin_id_fold, 32);
        SetMarginSensitive(margin_id_fold, true);

        SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

        SetTabWidth(4);
        SetUseTabs(false);
        SetWrapMode(wxSTC_WRAP_WORD);
        SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);
    }
    virtual bool SetFont(const wxFont& font) override
    {
        StyleSetFont(wxSTC_STYLE_DEFAULT, font);
        return wxStyledTextCtrl::SetFont(font);
    }
    void SetLexerXml()
    {
        const wxColour colTag = wxSystemSettings::SelectLightDark(*wxBLUE, *wxCYAN);
        const wxColour colAttr = wxSystemSettings::SelectLightDark(*wxRED, "PINK");

        SetLexer(wxSTC_LEX_XML);

        // Ensure the correct default background is used for all styles.
        StyleClearAll();

        StyleSetForeground(wxSTC_H_TAG, colTag);
        StyleSetForeground(wxSTC_H_TAGUNKNOWN, colTag);
        StyleSetForeground(wxSTC_H_ATTRIBUTE, colAttr);
        StyleSetForeground(wxSTC_H_ATTRIBUTEUNKNOWN, colAttr);
        StyleSetBold(wxSTC_H_ATTRIBUTEUNKNOWN, true);
        StyleSetForeground(wxSTC_H_OTHER, colTag);
        StyleSetForeground(wxSTC_H_COMMENT, wxColour("GREY"));
        StyleSetForeground(wxSTC_H_ENTITY, colAttr);
        StyleSetBold(wxSTC_H_ENTITY, true);
        StyleSetForeground(wxSTC_H_TAGEND, colTag);
        StyleSetForeground(wxSTC_H_XMLSTART, colTag);
        StyleSetForeground(wxSTC_H_XMLEND, colTag);
        StyleSetForeground(wxSTC_H_CDATA, colAttr);
    }
protected:
    void OnMarginClick(wxStyledTextEvent&);
    void OnText(wxStyledTextEvent&);
    wxDECLARE_EVENT_TABLE();
};

wxBEGIN_EVENT_TABLE(MinimalEditor, wxStyledTextCtrl)
    EVT_STC_MARGINCLICK(wxID_ANY, MinimalEditor::OnMarginClick)
    EVT_STC_CHANGE(wxID_ANY, MinimalEditor::OnText)
wxEND_EVENT_TABLE()

void MinimalEditor::OnMarginClick(wxStyledTextEvent &event)
{
    if (event.GetMargin() == margin_id_fold)
    {
        int lineClick = LineFromPosition(event.GetPosition());
        int levelClick = GetFoldLevel(lineClick);
        if ((levelClick & wxSTC_FOLDLEVELHEADERFLAG) > 0)
        {
            ToggleFold(lineClick);
        }
    }
}

void MinimalEditor::OnText(wxStyledTextEvent& event)
{
    wxLogDebug("Modified");
    event.Skip();
}

class MinimalEditorFrame : public wxFrame
{
public:
    MinimalEditorFrame() : wxFrame(nullptr, wxID_ANY, _("Minimal Editor"))
    {
        MinimalEditor* editor = new MinimalEditor(this);
        editor->SetFont(wxFontInfo().Family(wxFONTFAMILY_TELETYPE));
        wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
        sizer->Add(editor, 1, wxEXPAND);
        SetSizer(sizer);
        editor->SetText(
           "<xml>\n"
           "   <text>\n"
           "      This is xml with syntax highlighting, line numbers, folding, word wrap and context menu\n"
           "   </text>\n"
           "</xml>"
           );
    }
};

wxFrame* App::MinimalEditor()
{
    MinimalEditorFrame* frame = new MinimalEditorFrame;
    frame->Show();
    return frame;
}

void App::OnMinimalEditor(wxCommandEvent& WXUNUSED(event))
{
    MinimalEditor();
}

// Editor with a document map.

constexpr const char* wxTRACE_STC_MAP = "stcmap";

#include <functional>

class wxStyledTextCtrlMap : public wxStyledTextCtrl
{
public:
    wxStyledTextCtrlMap(wxWindow* parent, wxStyledTextCtrl* edit)
        : wxStyledTextCtrl(parent),
          m_edit(edit)
    {
        // Use the same document as the main editor.
        auto* const doc = edit->GetDocPointer();
        edit->AddRefDocument(doc);
        SetDocPointer(doc);

        // The map is used as as the scrollbar, so the editor doesn't need it.
        edit->SetUseVerticalScrollBar(false);

        // Making the map read-only apparently makes the document read-only, so
        // undo this for the original control (doing nothing if it already was
        // read-only anyhow).
        auto const wasReadOnly = edit->GetReadOnly();
        SetReadOnly(true);
        edit->SetReadOnly(wasReadOnly);

        // Copy the main editor attributes.

        // Do NOT set the lexer: this somehow breaks syntax highlighting and
        // folding in the main editor itself and the map gets syntax
        // highlighting even without it anyhow.

        for ( int style = 0; style < wxSTC_STYLE_MAX; ++style )
        {
            // There is probably no need to set the font for the map: at such
            // size, all fonts are indistinguishable.
            StyleSetForeground(style, edit->StyleGetForeground(style));
            StyleSetBackground(style, edit->StyleGetBackground(style));
            StyleSetBold(style, edit->StyleGetBold(style));
            StyleSetItalic(style, edit->StyleGetItalic(style));
            StyleSetUnderline(style, edit->StyleGetUnderline(style));
            StyleSetCase(style, edit->StyleGetCase(style));
        }

        SetWrapMode(edit->GetWrapMode());
        SetWrapVisualFlags(edit->GetWrapVisualFlags());
        SetWrapVisualFlagsLocation(edit->GetWrapVisualFlagsLocation());
        SetWrapIndentMode(edit->GetWrapIndentMode());
        SetWrapStartIndent(edit->GetWrapStartIndent());

        // Configure the map appearance.
        SetZoom(-10 /* maximum zoom out level */);
        SetExtraDescent(-1); // Compress the lines even more.
        SetCaretStyle(wxSTC_CARETSTYLE_INVISIBLE);
        SetMarginCount(0);
        SetIndentationGuides(wxSTC_IV_NONE);
        UsePopUp(wxSTC_POPUP_NEVER);
        SetUseHorizontalScrollBar(false);
        SetCursor(wxCURSOR_ARROW);

        // Ensure that folds in the map are synchronized with the main editor.
        edit->SetMirrorFoldingCtrl(this);

        // Scroll the editor when the map is scrolled.
        Bind(wxEVT_STC_UPDATEUI, &wxStyledTextCtrlMap::OnMapUpdate, this);

        // But also update the map when the editor changes.
        edit->Bind(wxEVT_STC_UPDATEUI, &wxStyledTextCtrlMap::OnEditUpdate, this);

        // And also when the height changes.
        edit->Bind(wxEVT_SIZE, [this](wxSizeEvent& event) {
            wxLogTrace(wxTRACE_STC_MAP,
                       "Edit resized to %dx%d, line info: %s",
                       event.GetSize().x, event.GetSize().y,
                       m_lines.Dump());

            // We try to update the map immediately as we may not need to do
            // anything else, but we need to also handle the number of
            // displayed lines changing later due to wrapping performed in the
            // background by Scintilla, so we also do it from wxEVT_PAINT
            // handler below.
            if ( UpdateLineInfo() )
            {
                wxLogTrace(wxTRACE_STC_MAP, "Sync map from size: %s",
                           m_lines.Dump());
                SyncMapPosition();
            }

            event.Skip();
        });

        edit->Bind(wxEVT_PAINT, [this](wxPaintEvent& event) {
            // The number of displayed lines may have changed due to
            // rewrapping, in which case we need to update the map.
            if ( UpdateLineInfo() )
            {
                wxLogTrace(wxTRACE_STC_MAP, "Sync map from paint: %s",
                            m_lines.Dump());
                SyncMapPosition();
            }

            event.Skip();
        });

        // Handle mouse events for dragging the visible zone indicator.
        Bind(wxEVT_LEFT_DOWN, [this](wxMouseEvent& event) {
            HandleMouseClick(event);
        });

        Bind(wxEVT_MOTION, [this](wxMouseEvent& event) {
            if ( event.LeftIsDown() && IsDragging() )
                DoDrag(event.GetPosition());
        });

        Bind(wxEVT_LEFT_UP, [this](wxMouseEvent& WXUNUSED(event)) {
            if ( IsDragging() )
                DoStopDragging();
        });

        Bind(wxEVT_LEFT_DCLICK, [this](wxMouseEvent& event) {
            // Don't let Scintilla get the double click, handle it as a simple
            // click instead.
            HandleMouseClick(event);
        });

        Bind(wxEVT_MOUSEWHEEL, [this](wxMouseEvent& event) {
            // Don't allow zooming into the map.
            if ( event.ControlDown() )
                return;

            // But do allow to scroll in it using the wheel.
            event.Skip();
        });

        Bind(wxEVT_MOUSE_CAPTURE_LOST, [this](wxMouseCaptureLostEvent& WXUNUSED(event)) {
            DoStopDragging();
        });

        // Draw our overlay over the map.
        Bind(wxEVT_PAINT, [this](wxPaintEvent& event) {
            // Normally we shouldn't call the base class event handler directly
            // like this, but here we really want to draw the text before
            // calling our DrawVisibleZone().
            wxStyledTextCtrl::OnPaint(event);

            // While dragging the visible zone is shown in an overlay.
            if ( !IsDragging() )
            {
                wxPaintDC dc(this);

                // We need to use wxGraphicsContext to draw the translucent
                // rectangle.
                wxGCDC gcdc(dc);
                DrawVisibleZone(gcdc);
            }
        });

        // Note that the line index doesn't matter, as the height is the same
        // for all of them.
        m_mapLineHeight = TextHeight(0);

        UpdateLineInfo();
        SyncMapPosition();
        SyncSelection();

        wxLogTrace(wxTRACE_STC_MAP, "Edit line height: %d, map: %d",
                   m_edit->TextHeight(0), TextHeight(0));
    }

    virtual ~wxStyledTextCtrlMap()
    {
        if ( IsDragging() )
            DoStopDragging();
    }

private:
    // Check if the cached information about visible and total number of lines
    // has changed and update it and return true if it did.
    bool UpdateLineInfo()
    {
        LinesInfo lineInfo;
        lineInfo.mapVisible = LinesOnScreen();
        lineInfo.editVisible = m_edit->LinesOnScreen();

        auto const mapDisplay = GetDisplayLineCount();
        lineInfo.mapMax = mapDisplay > lineInfo.mapVisible
            ? mapDisplay - lineInfo.mapVisible
            : 0;

        auto const editDisplay = m_edit->GetDisplayLineCount();
        lineInfo.editMax = editDisplay > lineInfo.editVisible
            ? editDisplay - lineInfo.editVisible
            : 0;

        // The last thumb position corresponds to the difference between the
        // map line corresponding to the first visible line in the editor and
        // the first visible line in the map itself.
        lineInfo.thumbMax =
            VisibleFromDocLine(m_edit->DocLineFromVisible(lineInfo.editMax)) -
                lineInfo.mapMax;

        if ( lineInfo == m_lines )
            return false;

        m_lines = lineInfo;
        return true;
    }

    // Get document line at the given mouse position in the map.
    int GetDocLineAtMapPoint(const wxPoint& pos) const
    {
        return LineFromPosition(PositionFromPoint(pos));
    }

    // Get the valid editor first line corresponding to the desired one but
    // adjusted for bounds.
    int GetEditValidFirstLine(int firstLine) const
    {
        if ( firstLine < 0 )
            return 0;

        if ( firstLine > m_lines.editMax )
            return m_lines.editMax;

        return firstLine;
    }

    // Set the editor first visible line without triggering the matching map
    // update.
    void SetEditFirstVisibleLine(int firstLine)
    {
        m_lastSetEditFirst = firstLine;

        m_edit->SetFirstVisibleLine(firstLine);
    }

    // Set the first visible line in the map without triggering the matching
    // editor update.
    void SetMapFirstVisibleLine(int firstLine)
    {
        m_lastSetMapFirst = firstLine;

        SetFirstVisibleLine(firstLine);
    }

    // We want the physical position of the thumb in the map to show the
    // position of the currently visible part of the document, just as a
    // normal scrollbar thumb does. So the thumb position is proportional to
    // the first visible line in the editor:
    //
    //          thumbPos = thumbMax / editMax * editFirst
    //
    // Where the coefficient of proportionality is given by considering
    // that the last possible values of the editor first line must be
    // mapped to the last valid position of the thumb.
    //
    // But thumb position (in map line units) is also given by the difference
    // between the map line corresponding to the first visible line in the
    // editor and the first visible line in the map itself:
    //
    //          thumbPos = VisibleFromDocLine(editDocFirst) - mapFirst
    //
    // where
    //
    //          editDocFirst = m_edit->DocLineFromVisible(editFirst)
    //
    // This gives us the expression for the map first visible line as a
    // function of the editor first visible line:
    //
    //          mapFirst = VisibleFromDocLine(editDocFirst) -
    //                          thumbMax / editMax * editFirst
    //
    // which can be used to set the map position when the editor position
    // changes and is returned by the function below.

    // Get the map first visible line corresponding to the given editor
    // first visible line.
    int GetMapFirstFromEditFirst(int editFirst) const
    {
        auto const editDocFirst = m_edit->DocLineFromVisible(editFirst);
        auto const thumbPos =
            wxMulDivInt32(editFirst, m_lines.thumbMax, m_lines.editMax);

        return VisibleFromDocLine(editDocFirst) - thumbPos;
    }

    // Scroll the editor to correspond to the current position in the map.
    void SyncEditPosition()
    {
        auto const mapFirst = GetFirstVisibleLine();

        // There is no way to invert the formula above giving mapFirst from
        // editFirst, so we start with the likely candidate for editFirst
        // (which is actually the correct value if no lines are wrapped or
        // folded) and iterate it until we find the value of editFirst giving
        // the desired mapFirst.
        const int editNew = FindClosestEditFirstLine
            (
                wxMulDivInt32(mapFirst, m_lines.editMax, m_lines.mapMax),
                [this](int editNew)
                {
                    return GetMapFirstFromEditFirst(editNew);
                },
                mapFirst
            );

        if ( editNew == m_edit->GetFirstVisibleLine() )
            return;

        SetEditFirstVisibleLine(editNew);
    }

    // Scroll the map to correspond to the current position in the editor.
    void SyncMapPosition()
    {
        auto const editFirst = m_edit->GetFirstVisibleLine();
        auto const mapFirst = GetMapFirstFromEditFirst(editFirst);

        if ( mapFirst == GetFirstVisibleLine() )
            return;

        SetMapFirstVisibleLine(mapFirst);

        Refresh();
    }

    // Helper function returning the visible line of the map corresponding to
    // the given visible line in the main editor.
    int MapVisibleFromEditVisible(int editVisible) const
    {
        auto const docLine = m_edit->DocLineFromVisible(editVisible);
        return VisibleFromDocLine(docLine);
    }

    // This function encapsulates the algorithm iterating over the first
    // visible editor line until we getValueForEditFirst() returns the closest
    // value to the desired one.
    //
    // The initial value is forced to be in the correct range.
    int
    FindClosestEditFirstLine(int editNewInitial,
                             std::function<int (int)> getValueForEditFirst,
                             int valueWanted) const
    {
        int editNew = GetEditValidFirstLine(editNewInitial);

        wxLogTrace(wxTRACE_STC_MAP, "Iterating: initial estimate=%d", editNew);

        // We use direction, which can be -1, 0 or 1 to avoid looping forever
        // if we can't get any closer, so remember which way we're going and
        // stop if overshoot.
        for ( int direction = 0;; )
        {
            auto const valueCurrent = getValueForEditFirst(editNew);

            if ( valueCurrent < valueWanted )
            {
                if ( editNew >= m_lines.editMax )
                {
                    // We can't go any further.
                    break;
                }

                if ( direction < 0 )
                {
                    // We overshot the correct value, we won't get any closer
                    // to it than we were before, so stop now to avoid entering
                    // an infinite loop alternating between the two values
                    // closest to the desired one from above and below.
                    break;
                }

                // Remember that we're going down in case it is the first
                // iteration of the loop.
                direction = 1;

                ++editNew;
            }
            else if ( valueCurrent > valueWanted )
            {
                // See comment in the branch above.
                if ( editNew <= 0 )
                    break;

                if ( direction > 0 )
                    break;

                direction = -1;

                --editNew;
            }
            else // valueCurrent == valueWanted
            {
                break;
            }
        }

        wxLogTrace(wxTRACE_STC_MAP, "Iterating: final line=%d", editNew);

        return editNew;
    }

    struct ThumbInfo
    {
        int pos = 0;    // Position of the top of the thumb in pixels.
        int height = 0; // Height of the thumb in pixels.
    };

    // Get the current thumb position and height.
    ThumbInfo GetThumbInfo() const
    {
        ThumbInfo thumb;

        // Thumb starts at the first line shown in the editor and is offset
        // from the top of the map window by the number of lines between it and
        // the first line shown in the map, multiplied by the height of one
        // line. In the trivial case when display and physical lines are the
        // same, this would be just the difference between the visible lines in
        // the editor and the map respectively, but in the general case we need
        // to convert the editor visible line to the document line first and
        // then convert it to the map visible line.
        auto const editFirst = m_edit->GetFirstVisibleLine();
        auto const mapThumbFirst = MapVisibleFromEditVisible(editFirst);

        thumb.pos = (mapThumbFirst - GetFirstVisibleLine()) * m_mapLineHeight;

        // Thumb size is the number of display lines in the map corresponding
        // to the lines shown in the editor (in the trivial case when display
        // and physical lines are the same, this is just the number of the
        // lines shown in the editor) multiplied by the height of one line.
        auto const editLast = editFirst + m_lines.editVisible;
        auto const mapThumbLast = MapVisibleFromEditVisible(editLast);

        thumb.height = (mapThumbLast - mapThumbFirst) * m_mapLineHeight;

        return thumb;
    }

    void DrawVisibleZone(wxDC& dc)
    {
        // We need to use a different opacity here when not using wxMSW (which
        // sets the opacity for the overlay globally).
        //
        // TODO: Don't hardcode the colour.
        dc.SetBrush(wxColour(0x80, 0x80, 0x80, IsDragging() ? 0x80 : 0x40));
        dc.SetPen(*wxTRANSPARENT_PEN);

        auto const thumb = GetThumbInfo();
        dc.DrawRectangle(0, thumb.pos, GetClientSize().x, thumb.height);
    }

    // Show the same selection in the map as in the editor.
    void SyncSelection()
    {
        SetSelectionStart(m_edit->GetSelectionStart());
        SetSelectionEnd(m_edit->GetSelectionEnd());
    }

    void OnEditUpdate(wxStyledTextEvent& event)
    {
        if ( event.GetUpdated() & wxSTC_UPDATE_V_SCROLL )
        {
            auto const editFirst = m_edit->GetFirstVisibleLine();
            if ( editFirst == m_lastSetEditFirst )
            {
                m_lastSetEditFirst = -1;
                return;
            }

            wxLogTrace(wxTRACE_STC_MAP, "Sync map from scroll: %d",
                       editFirst);

            SyncMapPosition();
        }

        if ( event.GetUpdated() & wxSTC_UPDATE_SELECTION )
            SyncSelection();

        if ( event.GetUpdated() & wxSTC_UPDATE_CONTENT )
            Refresh();
    }

    void OnMapUpdate(wxStyledTextEvent& event)
    {
        // We're only interested in scrolling notifications here.
        if ( event.GetUpdated() & wxSTC_UPDATE_V_SCROLL )
        {
            auto const mapFirst = GetFirstVisibleLine();
            if ( mapFirst == m_lastSetMapFirst )
            {
                m_lastSetMapFirst = -1;
                return;
            }

            wxLogTrace(wxTRACE_STC_MAP, "Sync edit from scroll: %d",
                       mapFirst);

            SyncEditPosition();
        }
    }

    // Return true if the visible zone indicator is currently being dragged.
    bool IsDragging() const
    {
        return m_isDragging;
    }

    void HandleMouseClick(wxMouseEvent& event)
    {
        auto const posMouse = event.GetPosition();
        auto const docLine = GetDocLineAtMapPoint(posMouse);
        auto const editLine = m_edit->VisibleFromDocLine(docLine);

        wxLogTrace(wxTRACE_STC_MAP, "Clicked doc/editor line %d/%d",
                   docLine, editLine);

        auto const editFirst = m_edit->GetFirstVisibleLine();
        auto const docFirst = m_edit->DocLineFromVisible(editFirst);

        auto const editLast = editFirst + m_lines.editVisible;
        auto const docLast = m_edit->DocLineFromVisible(editLast);

        // If the line is in the visible zone indicator, start dragging it.
        if ( docLine >= docFirst && docLine < docLast )
        {
            StartDragging(posMouse);
            return;
        }

        // The clicked line becomes the center of the visible zone indicator.
        // unless it's too close to the top or bottom of the map.
        auto const
            editNew = GetEditValidFirstLine(editLine - m_lines.editVisible / 2);

        if ( editNew != editFirst )
        {
            wxLogTrace(wxTRACE_STC_MAP, "Editor first %d -> %d",
                       editFirst, editNew);
            SetEditFirstVisibleLine(editNew);
            SyncMapPosition();

            Refresh();
        }
    }

    // Called when the user starts dragging at the given point.
    void StartDragging(const wxPoint& pos)
    {
        m_isDragging = true;

        auto const thumb = GetThumbInfo();
        m_dragOffset = pos.y - (thumb.pos + thumb.height / 2);

        wxLogTrace(wxTRACE_STC_MAP, "Dragging: start at offset=%d",
                   m_dragOffset);

        SetCursor(wxCURSOR_HAND);
        CaptureMouse();

        // This is required by wxMSW to ensure that the map shows from behind
        // the overlay and does nothing elsewhere.
        m_overlay.SetOpacity(0x80);

        wxOverlayDC dc(m_overlay, this);
        dc.Clear();
        DrawVisibleZone(dc);

        Refresh();
    }

    // Called while dragging the visible zone indicator.
    void DoDrag(wxPoint pos)
    {
        // We want the thumb to follow the mouse when dragging, i.e. its middle
        // must remain at the same distance from the mouse pointer as it was
        // when the dragging started.
        pos.y -= m_dragOffset;

        // This is the position at which we want thumb middle to be.
        const int thumbWanted = (pos.y + m_mapLineHeight - 1) / m_mapLineHeight;

        // First visible line in the editor is proportional to the thumb
        // position, but we only have the position of the middle of the thumb
        // and the thumb height depends on the position too, so we need to
        // iterate until we find the right position.
        //
        // Start with the position corresponding to the current thumb height.
        int thumbPos = thumbWanted - GetThumbInfo().height / (2 * m_mapLineHeight);

        // Adjust until the actual position of the middle of the thumb if
        // this were the first visible line in the editor becomes close to
        // the desired position.
        const int editNew = FindClosestEditFirstLine
            (
                wxMulDivInt32(thumbPos, m_lines.editMax, m_lines.thumbMax),
                [this](int editNew)
                {
                    // Thumb top corresponds to the visible line in the map
                    // corresponding to the document line corresponding to the
                    // line editNew in the editor and, similarly, thumb bottom
                    // corresponds to editNew + m_lines.editVisible.
                    return
                        (MapVisibleFromEditVisible(editNew) +
                         MapVisibleFromEditVisible(editNew + m_lines.editVisible))
                        / 2
                        - GetMapFirstFromEditFirst(editNew);
                },
                thumbWanted
            );

        if ( editNew == m_edit->GetFirstVisibleLine() )
            return;

        SetEditFirstVisibleLine(editNew);
        SyncMapPosition();

        wxOverlayDC dc(m_overlay, this);
        dc.Clear();
        DrawVisibleZone(dc);
    }

    // Called when we stop dragging, either because the mouse button was
    // released or because the mouse capture was lost.
    void DoStopDragging()
    {
        m_isDragging = false;
        m_dragOffset = 0;

        SetCursor(wxCURSOR_ARROW);
        ReleaseMouse();

        m_overlay.Reset();

        Refresh();
    }


    // The associated main document control.
    wxStyledTextCtrl* const m_edit;

    // Overlay showing the visible zone indicator being dragged.
    wxOverlay m_overlay;

    // The constant vertical offset between the mouse pointer and the middle of
    // the thumb while dragging. Only valid if m_isDragging is true.
    int m_dragOffset = 0;

    // Height of a single line in the map: this value is cached when creating
    // the control.
    int m_mapLineHeight = 0;

    // The values in this struct change when the window size changes and, for
    // the display line count, may also change later when wrapping is enabled
    // as Scintilla re-wraps lines during idle time.
    struct LinesInfo
    {
        // Number of lines visible in the map.
        int mapVisible = 0;

        // Number of lines visible in the editor.
        int editVisible = 0;

        // Last valid value for the first map line.
        int mapMax = 0;

        // Last valid value for the first editor line.
        int editMax = 0;

        // Last valid value for the thumb position (in map line units).
        int thumbMax = 0;

        LinesInfo() = default;
        LinesInfo(const LinesInfo& other) = default;
        LinesInfo& operator=(const LinesInfo& other) = default;

        bool operator==(const LinesInfo& rhs) const
        {
            return mapVisible == rhs.mapVisible &&
                   editVisible == rhs.editVisible &&
                   mapMax == rhs.mapMax &&
                   editMax == rhs.editMax &&
                   thumbMax == rhs.thumbMax;
        }

        bool operator!=(const LinesInfo& rhs) const
        {
            return !(*this == rhs);
        }

        wxString Dump() const
        {
            return wxString::Format
                   (
                    "map/edit visible: %d/%d, map/edit first line max: %d/%d",
                    mapVisible, editVisible, mapMax, editMax
                   );
        }
    } m_lines;

    // Last values set by this code: we can safely ignore notifications about
    // changes that still use these values.
    //
    // -1 here means "invalid" and is used because it's guaranteed to be
    // different from any valid first line value.
    int m_lastSetEditFirst = -1;
    int m_lastSetMapFirst = -1;

    // Flag set while dragging the thumb.
    bool m_isDragging = false;


    wxDECLARE_NO_COPY_CLASS(wxStyledTextCtrlMap);
};

void App::ShowDocumentMap(wxWindow* parent)
{
    wxDialog dialog(parent, wxID_ANY, "Editor with Document Map",
                    wxDefaultPosition,
                    wxWindow::FromDIP(wxSize(800, 600), m_frame),
                    wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);

    auto* const splitter = new wxSplitterWindow(&dialog, wxID_ANY);

    auto* const edit = new Edit(splitter);
    edit->LoadFile("stctest.cpp");

    // Show line numbers in the margin, which are hidden by default.
    edit->ToggleLineNumbers();

    edit->SetWrapMode(wxSTC_WRAP_WORD);
    edit->SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);

    auto* const map = new wxStyledTextCtrlMap(splitter, edit);

    splitter->SplitVertically(edit, map);
    splitter->SetMinimumPaneSize(dialog.FromDIP(10));

    dialog.Bind(wxEVT_SIZE, [&](wxSizeEvent& event) {
        splitter->SetSashPosition(-dialog.FromDIP(200));
        event.Skip();
    });

    dialog.ShowModal();
}

void App::OnDocumentMap(wxCommandEvent& WXUNUSED(event))
{
    ShowDocumentMap(m_frame);
}
