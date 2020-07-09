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

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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
wxString *g_appname = NULL;

#if wxUSE_PRINTING_ARCHITECTURE

//! global print data, to remember settings during the session
wxPrintData *g_printData = (wxPrintData*) NULL;
wxPageSetupDialogData *g_pageSetupData = (wxPageSetupDialogData*) NULL;

#endif // wxUSE_PRINTING_ARCHITECTURE


class AppFrame;

//----------------------------------------------------------------------------
//! application APP_VENDOR-APP_NAME.
class App: public wxApp {
    friend class AppFrame;

public:
    //! the main function called during application start
    virtual bool OnInit () wxOVERRIDE;

    //! application exit function
    virtual int OnExit () wxOVERRIDE;

private:
    //! frame window
    AppFrame* m_frame;

    wxFrame* MinimalEditor();
protected:
    void OnMinimalEditor(wxCommandEvent&);
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
        : wxFrame ((wxFrame *)NULL, wxID_ANY, title, wxDefaultPosition, wxSize(750,550))
{
    SetIcon(wxICON(sample));

    // initialize important variables
    m_edit = NULL;

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
    menuFile->Append (myID_PROPERTIES, _("Proper&ties ..\tCtrl+I"));
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
    menuEdit->Append (myID_INDENTINC, _("&Indent increase\tTab"));
    menuEdit->Append (myID_INDENTRED, _("I&ndent reduce\tShift+Tab"));
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

    // Help menu
    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append (wxID_ABOUT, _("&About ..\tCtrl+D"));

    // construct menu
    m_menuBar->Append (menuFile, _("&File"));
    m_menuBar->Append (menuEdit, _("&Edit"));
    m_menuBar->Append (menuView, _("&View"));
    m_menuBar->Append (menuAnnotations, _("&Annotations"));
    m_menuBar->Append (menuExtra, _("E&xtra"));
    m_menuBar->Append (menuWindow, _("&Window"));
    m_menuBar->Append (menuHelp, _("&Help"));
    SetMenuBar (m_menuBar);

    m_menuBar->Check(myID_ANNOTATION_STYLE_BOXED, true);
}

void AppFrame::FileOpen (wxString fname)
{
    wxFileName w(fname); w.Normalize(); fname = w.GetFullPath();
    m_edit->LoadFile (fname);
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
    m_timer = NULL;
    if (milliseconds > 0) {
        m_timer = new wxTimer (this, myID_ABOUTTIMER);
        m_timer->Start (milliseconds, wxTIMER_ONE_SHOT);
    }

    // Get version of Scintilla
    wxVersionInfo vi = wxStyledTextCtrl::GetLibraryVersionInfo();

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
    aboutinfo->Add (new wxStaticText(this, wxID_ANY, wxString::Format("%s (%s)", APP_VERSION, vi.GetVersionString())),
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

        MarkerDefine(wxSTC_MARKNUM_FOLDER,        wxSTC_MARK_BOXPLUS, "WHITE", "BLACK");
        MarkerDefine(wxSTC_MARKNUM_FOLDEROPEN,    wxSTC_MARK_BOXMINUS,  "WHITE", "BLACK");
        MarkerDefine(wxSTC_MARKNUM_FOLDERSUB,     wxSTC_MARK_VLINE,     "WHITE", "BLACK");
        MarkerDefine(wxSTC_MARKNUM_FOLDEREND,     wxSTC_MARK_BOXPLUSCONNECTED, "WHITE", "BLACK");
        MarkerDefine(wxSTC_MARKNUM_FOLDEROPENMID, wxSTC_MARK_BOXMINUSCONNECTED, "WHITE", "BLACK");
        MarkerDefine(wxSTC_MARKNUM_FOLDERMIDTAIL, wxSTC_MARK_TCORNER,     "WHITE", "BLACK");
        MarkerDefine(wxSTC_MARKNUM_FOLDERTAIL,    wxSTC_MARK_LCORNER,     "WHITE", "BLACK");

        SetMarginMask(margin_id_fold, wxSTC_MASK_FOLDERS);
        SetMarginWidth(margin_id_fold, 32);
        SetMarginSensitive(margin_id_fold, true);

        SetFoldFlags(wxSTC_FOLDFLAG_LINEBEFORE_CONTRACTED | wxSTC_FOLDFLAG_LINEAFTER_CONTRACTED);

        SetTabWidth(4);
        SetUseTabs(false);
        SetWrapMode(wxSTC_WRAP_WORD);
        SetWrapVisualFlags(wxSTC_WRAPVISUALFLAG_END);
    }
    virtual bool SetFont(const wxFont& font) wxOVERRIDE
    {
        StyleSetFont(wxSTC_STYLE_DEFAULT, font);
        return wxStyledTextCtrl::SetFont(font);
    }
    void SetLexerXml()
    {
        SetLexer(wxSTC_LEX_XML);
        StyleSetForeground(wxSTC_H_DEFAULT, *wxBLACK);
        StyleSetForeground(wxSTC_H_TAG, *wxBLUE);
        StyleSetForeground(wxSTC_H_TAGUNKNOWN, *wxBLUE);
        StyleSetForeground(wxSTC_H_ATTRIBUTE, *wxRED);
        StyleSetForeground(wxSTC_H_ATTRIBUTEUNKNOWN, *wxRED);
        StyleSetBold(wxSTC_H_ATTRIBUTEUNKNOWN, true);
        StyleSetForeground(wxSTC_H_NUMBER, *wxBLACK);
        StyleSetForeground(wxSTC_H_DOUBLESTRING, *wxBLACK);
        StyleSetForeground(wxSTC_H_SINGLESTRING, *wxBLACK);
        StyleSetForeground(wxSTC_H_OTHER, *wxBLUE);
        StyleSetForeground(wxSTC_H_COMMENT, wxColour("GREY"));
        StyleSetForeground(wxSTC_H_ENTITY, *wxRED);
        StyleSetBold(wxSTC_H_ENTITY, true);
        StyleSetForeground(wxSTC_H_TAGEND, *wxBLUE);
        StyleSetForeground(wxSTC_H_XMLSTART, *wxBLUE);
        StyleSetForeground(wxSTC_H_XMLEND, *wxBLUE);
        StyleSetForeground(wxSTC_H_CDATA, *wxRED);
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
    MinimalEditorFrame() : wxFrame(NULL, wxID_ANY, _("Minimal Editor"))
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

