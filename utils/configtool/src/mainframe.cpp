/////////////////////////////////////////////////////////////////////////////
// Name:        mainframe.cpp
// Purpose:     Main frame
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "mainframe.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/splitter.h"

#endif

#include "wx/cshelp.h"
#include "wx/html/htmlwin.h"
#include "wx/notebook.h"
#include "wx/dataobj.h"
#include "wx/clipbrd.h"
#include "wx/stockitem.h"
#include "wxconfigtool.h"
#include "mainframe.h"
#include "appsettings.h"
#include "symbols.h"
#include "configtree.h"
#include "propeditor.h"
#include "configtooldoc.h"
#include "configtoolview.h"
#include "configbrowser.h"

#include "bitmaps/wxconfigtool.xpm"

#include "bitmaps/copy.xpm"
#include "bitmaps/cut.xpm"
#include "bitmaps/paste.xpm"
#include "bitmaps/open.xpm"
#include "bitmaps/save.xpm"
#include "bitmaps/new.xpm"
#include "bitmaps/help.xpm"
#include "bitmaps/undo.xpm"
#include "bitmaps/redo.xpm"
#include "bitmaps/helpcs.xpm"
#include "bitmaps/go.xpm"

IMPLEMENT_CLASS(ctMainFrame, wxDocParentFrame)

BEGIN_EVENT_TABLE(ctMainFrame, wxDocParentFrame)
    EVT_CLOSE(ctMainFrame::OnCloseWindow)
    EVT_MENU(wxID_ABOUT, ctMainFrame::OnAbout)
    EVT_MENU(wxID_OPEN, ctMainFrame::OnOpen)
    EVT_MENU(wxID_NEW, ctMainFrame::OnNew)
    EVT_MENU(wxID_EXIT, ctMainFrame::OnExit)
    EVT_MENU(ctID_SETTINGS, ctMainFrame::OnSettings)
    EVT_MENU(ctID_SHOW_TOOLBAR, ctMainFrame::OnShowToolbar)
    EVT_MENU(wxID_HELP, ctMainFrame::OnHelp)
    EVT_MENU(ctID_REFERENCE_CONTENTS, ctMainFrame::OnReferenceHelp)
    EVT_MENU(wxID_HELP_CONTEXT, ctMainFrame::OnContextHelp)

    EVT_UPDATE_UI(ctID_ADD_ITEM_CHECKBOX, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_ADD_ITEM_RADIOBUTTON, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_ADD_ITEM_GROUP, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_ADD_ITEM_CHECK_GROUP, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_ADD_ITEM_RADIO_GROUP, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_ADD_ITEM_STRING, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_DELETE_ITEM, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_RENAME_ITEM, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_COPY, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_CUT, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(wxID_PASTE, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_ITEM_HELP,  ctMainFrame::OnUpdateDisable)

    EVT_UPDATE_UI(ctID_ADD_CUSTOM_PROPERTY, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_EDIT_CUSTOM_PROPERTY, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_DELETE_CUSTOM_PROPERTY, ctMainFrame::OnUpdateDisable)

    EVT_UPDATE_UI(ctID_SAVE_SETUP_FILE, ctMainFrame::OnUpdateDisable)
    EVT_UPDATE_UI(ctID_SAVE_CONFIGURE_COMMAND, ctMainFrame::OnUpdateDisable)

    EVT_UPDATE_UI(wxID_FIND, ctMainFrame::OnUpdateDisable)

    EVT_UPDATE_UI(ctID_GO, ctMainFrame::OnUpdateDisable)
END_EVENT_TABLE()

// Define my frame constructor
ctMainFrame::ctMainFrame(wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style):
  wxDocParentFrame(manager, parent, id, title, pos, size, style)
{
    m_document = NULL;
    m_editMenu = NULL;
    m_configurePage = NULL;
    m_setupPage = NULL;
#ifdef USE_CONFIG_BROWSER_PAGE
    m_configBrowserPage = NULL;
#endif
    m_mainNotebook = NULL;
    m_findDialog = NULL;

    m_treeSplitterWindow = new wxSplitterWindow(this, wxID_ANY, wxDefaultPosition, wxSize(400, 300),
        wxSP_3DSASH|wxSP_3DBORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);

    m_configTreeCtrl = new ctConfigTreeCtrl(m_treeSplitterWindow, wxID_ANY, wxDefaultPosition, wxDefaultSize,
        wxTR_HAS_BUTTONS|wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);

    m_mainNotebook = new wxNotebook(m_treeSplitterWindow, wxID_ANY, wxDefaultPosition, wxSize(300, 300),
        wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);

    m_propertyEditor = new ctPropertyEditor(m_mainNotebook, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
        wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);
    m_setupPage = new ctOutputWindow(m_mainNotebook, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
        wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);
    m_configurePage = new ctOutputWindow(m_mainNotebook, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
        wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);

#ifdef USE_CONFIG_BROWSER_PAGE
    m_configBrowserPage = new ctConfigurationBrowserWindow(m_mainNotebook, wxID_ANY, wxDefaultPosition, wxSize(300, 200),
        wxNO_BORDER|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);
#endif

    m_mainNotebook->AddPage(m_propertyEditor, _T("Properties"));
#ifdef USE_CONFIG_BROWSER_PAGE
    m_mainNotebook->AddPage(m_configBrowserPage, _T("Configuration Browser"));
#endif
    m_mainNotebook->AddPage(m_setupPage, _T("setup.h"));
    m_mainNotebook->AddPage(m_configurePage, _T("configure"));

    // Need to do this afterwards since wxWin sets the XP background
    // in AddPage
    m_propertyEditor->GetDescriptionWindow()->SetBackgroundColour(ctDESCRIPTION_BACKGROUND_COLOUR);

    int sashPos = wxGetApp().GetSettings().m_mainSashSize;
    m_treeSplitterWindow->SplitVertically(m_configTreeCtrl, m_mainNotebook, sashPos);

    SetIcon(wxIcon(wxconfigtool_xpm));

#if wxUSE_STATUSBAR
    CreateStatusBar(2);
#endif // wxUSE_STATUSBAR

    wxMenuBar* menuBar = CreateMenuBar();
    SetMenuBar(menuBar);

    CreateToolBar(wxNO_BORDER|wxTB_FLAT|wxTB_HORIZONTAL|wxNO_FULL_REPAINT_ON_RESIZE|wxCLIP_CHILDREN);
    InitToolBar(GetToolBar());

    if (wxGetApp().GetSettings().m_showToolBar)
    {
        menuBar->Check(ctID_SHOW_TOOLBAR, true);
    }
    else
    {
        menuBar->Check(ctID_SHOW_TOOLBAR, false);
        GetToolBar()->Show(false);
        ResizeFrame();
    }
}

void ctMainFrame::OnCloseWindow(wxCloseEvent& event)
{
    if (GetDocument())
    {
        if (!GetDocument()->DeleteAllViews() && event.CanVeto())
        {
            event.Veto();
            return;
        }
    }

    if (m_findDialog)
    {
        m_findDialog->Destroy();
        m_findDialog = NULL;
    }

    Show(false);

    if (IsMaximized())
        wxGetApp().GetSettings().m_frameStatus = ctSHOW_STATUS_MAXIMIZED ;
    else if (IsIconized())
        wxGetApp().GetSettings().m_frameStatus = ctSHOW_STATUS_MINIMIZED ;
    else
        wxGetApp().GetSettings().m_frameStatus = ctSHOW_STATUS_NORMAL ;

    // Must delete this now since the DLL loading library will be
    // uninitialised by the time the app object is deleted
    wxGetApp().ClearHelpControllers();

    if (!IsMaximized() && !IsIconized())
    {
        wxGetApp().GetSettings().m_frameSize = GetRect();
        wxGetApp().GetSettings().m_mainSashSize = m_treeSplitterWindow->GetSashPosition();
    }

    Destroy();
}

void ctMainFrame::OnAbout(wxCommandEvent& WXUNUSED(event))
{
    wxString msg = wxGetApp().GetSettings().GetAppName() + wxT(" (c) Julian Smart");
    wxString caption = wxT("About ") + wxGetApp().GetSettings().GetAppName();
    wxMessageBox(msg, caption, wxOK|wxICON_INFORMATION);
}

void ctMainFrame::OnOpen(wxCommandEvent& event)
{
    wxGetApp().GetDocManager()->OnFileOpen(event);
}

void ctMainFrame::OnNew(wxCommandEvent& event)
{
    wxGetApp().GetDocManager()->OnFileNew(event);
}

void ctMainFrame::OnExit(wxCommandEvent& WXUNUSED(event))
{
    Close();
}

void ctMainFrame::OnSettings(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().GetSettings().ShowSettingsDialog();
}

void ctMainFrame::InitToolBar(wxToolBar* toolBar)
{
    toolBar->SetHelpText(wxT("The toolbar gives you easy access to commonly used commands."));

    // Set up toolbar
    wxBitmap toolBarBitmaps[20];

    toolBarBitmaps[0] = wxBitmap(new_xpm);
    toolBarBitmaps[1] = wxBitmap(open_xpm);
    toolBarBitmaps[2] = wxBitmap(save_xpm);
    toolBarBitmaps[3] = wxBitmap(copy_xpm);
    toolBarBitmaps[4] = wxBitmap(cut_xpm);
    toolBarBitmaps[5] = wxBitmap(paste_xpm);
    toolBarBitmaps[6] = wxBitmap(go_xpm);
    toolBarBitmaps[7] = wxBitmap(help_xpm);
    toolBarBitmaps[10] = wxBitmap(undo_xpm);
    toolBarBitmaps[11] = wxBitmap(redo_xpm);
    toolBarBitmaps[12] = wxBitmap(help_xpm);
    toolBarBitmaps[13] = wxBitmap(helpcs_xpm);

    toolBar->AddTool(wxID_NEW, toolBarBitmaps[0], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxT("New project"));
    toolBar->AddTool(wxID_OPEN, toolBarBitmaps[1], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxT("Open project"));
    toolBar->AddTool(wxID_SAVE, toolBarBitmaps[2], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxT("Save project"));

    toolBar->AddSeparator();
    toolBar->AddTool(wxID_CUT, toolBarBitmaps[4], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxGetStockLabel(wxID_CUT, false));
    toolBar->AddTool(wxID_COPY, toolBarBitmaps[3], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxGetStockLabel(wxID_COPY, false));
    toolBar->AddTool(wxID_PASTE, toolBarBitmaps[5], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxGetStockLabel(wxID_PASTE, false));
    toolBar->AddSeparator();
    toolBar->AddTool(wxID_UNDO, toolBarBitmaps[10], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxGetStockLabel(wxID_UNDO, false));
    toolBar->AddTool(wxID_REDO, toolBarBitmaps[11], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxGetStockLabel(wxID_REDO, false));
    toolBar->AddSeparator();
    toolBar->AddTool(ctID_GO, toolBarBitmaps[6], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxT("Save setup.h or configurewx.sh"));
    toolBar->AddSeparator();
    toolBar->AddTool(ctID_ITEM_HELP, toolBarBitmaps[12], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxT("Show help for this option"));
    toolBar->AddTool(wxID_HELP_CONTEXT, toolBarBitmaps[13], wxNullBitmap, false, wxDefaultCoord, wxDefaultCoord, (wxObject *) NULL, wxT("Show help on the clicked item"));

    // after adding the buttons to the toolbar, must call Realize() to reflect
    // the changes
    toolBar->Realize();
}

wxMenuBar* ctMainFrame::CreateMenuBar()
{
    // Make a menubar
    wxMenu *fileMenu = new wxMenu;

    wxGetApp().GetFileHistory().UseMenu(fileMenu);

    fileMenu->Append(wxID_NEW, wxGetStockLabel(wxID_NEW, true, wxT("Ctrl+N")), wxT("Create a new settings document"));
    fileMenu->Append(wxID_OPEN, wxGetStockLabel(wxID_OPEN, true, wxT("Ctrl+O")), wxT("Open a settings document"));
    fileMenu->Append(wxID_CLOSE, wxGetStockLabel(wxID_CLOSE, true, wxT("Ctrl+W")), wxT("Close the current settings document"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_SAVE, wxGetStockLabel(wxID_SAVE, true, wxT("Ctrl+S")), wxT("Save the settings document"));
    fileMenu->Append(wxID_SAVEAS, wxGetStockLabel(wxID_SAVEAS), wxT("Save the settings document under a new filename"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ctID_SAVE_SETUP_FILE, wxT("Save Setup.&h...\tCtrl+H"), wxT("Save the setup.h file"));
    fileMenu->Append(ctID_SAVE_CONFIGURE_COMMAND, wxT("Save Configure Script...\tCtrl+G"), wxT("Save the configure script file"));
    fileMenu->AppendSeparator();
    fileMenu->Append(ctID_GO, wxT("&Go\tF5"), wxT("Quick-save the setup.h or configure.sh file"));
    fileMenu->AppendSeparator();
    fileMenu->Append(wxID_EXIT, wxGetStockLabel(wxID_EXIT, true, wxT("Alt+F4")), wxT("Exit the application"));

    wxGetApp().GetDocManager()->FileHistoryUseMenu(fileMenu);

    wxMenu *editMenu = new wxMenu;

    editMenu->Append(wxID_UNDO, wxGetStockLabel(wxID_UNDO, true, wxT("Ctrl+Z")));
    editMenu->Append(wxID_REDO, wxGetStockLabel(wxID_REDO, true, wxT("Ctrl+Y")));
    editMenu->AppendSeparator();
    editMenu->Append(wxID_COPY, wxGetStockLabel(wxID_CLOSE, true, wxT("Ctrl+C")));
    editMenu->Append(wxID_CUT, wxGetStockLabel(wxID_CUT, true, wxT("Ctrl+X")));
    editMenu->Append(wxID_PASTE, wxGetStockLabel(wxID_PASTE, true, wxT("Ctrl+V")));
    editMenu->AppendSeparator();

    wxMenu* itemMenu = new wxMenu;
    itemMenu->Append(ctID_ADD_ITEM_CHECKBOX, _("Add &Checkbox Option"), _("Add a checkbox configuration option"));
    itemMenu->Append(ctID_ADD_ITEM_RADIOBUTTON, _("Add &Radio Button Option"), _("Add a radio button configuration option"));
// TODO. However we can also simply use custom properties;
// but then the C++ code has to be special-cased.
//    itemMenu->Append(ctID_ADD_ITEM_STRING, _("Add &String Option"), _("Add a string configuration option"));
    itemMenu->Append(ctID_ADD_ITEM_GROUP, _("Add &Group Option"), _("Add a group configuration option"));
    itemMenu->Append(ctID_ADD_ITEM_CHECK_GROUP, _("Add Chec&k Group Option"), _("Add a check group configuration option"));
    itemMenu->Append(ctID_ADD_ITEM_RADIO_GROUP, _("Add Rad&io Group Option"), _("Add a radio group configuration option"));

    editMenu->Append(ctID_ADD_ITEM, wxT("Add &Option"), itemMenu, _("Add a configuration option"));
    editMenu->AppendSeparator();

    wxMenu* propertyMenu = new wxMenu;
    propertyMenu->Append(ctID_ADD_CUSTOM_PROPERTY, _("&Add Custom Property"), _("Adds a custom property to the current option"));
    propertyMenu->Append(ctID_EDIT_CUSTOM_PROPERTY, _("&Edit Custom Property"), _("Edits the currently selected custom property"));
    propertyMenu->Append(ctID_DELETE_CUSTOM_PROPERTY, _("&Delete Custom Property"), _("Deletes the currently selected custom property"));
    editMenu->Append(ctID_CUSTOM_PROPERTY, _("Custom P&roperty"), propertyMenu, _("Custom property commands"));

    editMenu->AppendSeparator();
    editMenu->Append(ctID_DELETE_ITEM, _("&Delete Option"), _("Delete a configuration option"));
    editMenu->Append(ctID_RENAME_ITEM, _("&Rename Option"), _("Rename a configuration option"));
    editMenu->AppendSeparator();
    editMenu->Append(wxID_FIND, wxGetStockLabel(wxID_FIND, true, wxT("Ctrl+F")), _("Search for a string in the settings document"));

    // Save for the command processor.
    m_editMenu = editMenu;

    wxMenu *viewMenu = new wxMenu;
    viewMenu->Append(ctID_SHOW_TOOLBAR, wxT("Show &Toolbar"), wxT("Toggle the toolbar on and off"),
        wxITEM_CHECK);
    viewMenu->Append(ctID_SETTINGS, wxT("&Settings...\tCtrl+T"), wxT("Show application settings"));

    wxMenu *helpMenu = new wxMenu;
    helpMenu->Append(wxID_HELP, wxT("&Help Contents"), wxT("Show Configuration Tool help"));
    helpMenu->Append(ctID_REFERENCE_CONTENTS, wxT("&wxWidgets Help Contents"), wxT("Show wxWidgets reference"));
    helpMenu->AppendSeparator();
    helpMenu->Append(ctID_ITEM_HELP, wxT("&Configuration Option Help\tF1"), wxT("Show help for the selected option"));
    helpMenu->Append(wxID_HELP_CONTEXT, wxT("&What's this?"), wxT("Show help on the clicked item"));
    helpMenu->AppendSeparator();
    helpMenu->Append(wxID_ABOUT, wxT("&About..."), wxT("Show details about this application"));

    wxMenuBar *menuBar = new wxMenuBar;

    menuBar->Append(fileMenu, wxT("&File"));
    menuBar->Append(editMenu, wxT("&Edit"));
    menuBar->Append(viewMenu, wxT("&View"));
    menuBar->Append(helpMenu, wxT("&Help"));

    {
        wxConfig config(wxGetApp().GetSettings().GetAppName(), wxT("wxWidgets"));
        config.SetPath(wxT("FileHistory/"));
        wxGetApp().GetDocManager()->FileHistoryLoad(config);
    }

    return menuBar;
}

/// Handles the Show Toolbar menu event.
void ctMainFrame::OnShowToolbar(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().GetSettings().m_showToolBar = !wxGetApp().GetSettings().m_showToolBar;
    GetToolBar()->Show(wxGetApp().GetSettings().m_showToolBar);
    ResizeFrame();
}

/// Handles the Help Contents menu event.
void ctMainFrame::OnHelp(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().GetHelpController().DisplayContents();
}

/// Handles context help
void ctMainFrame::OnContextHelp(wxCommandEvent& WXUNUSED(event))
{
    wxContextHelp contextHelp;
}

/// Handles the Help Contents menu event for the reference manual.
void ctMainFrame::OnReferenceHelp(wxCommandEvent& WXUNUSED(event))
{
    wxGetApp().GetReferenceHelpController().DisplayContents();
}

/// Resizes the main frame according to the
/// state of the toolbar
void ctMainFrame::ResizeFrame()
{
    PositionToolBar();
    wxSizeEvent event(GetSize(), GetId());
    this->ProcessEvent(event);
}

/// Update the frame title.
void ctMainFrame::UpdateFrameTitle()
{
    // TODO
}

// General disabler
void ctMainFrame::OnUpdateDisable(wxUpdateUIEvent& event)
{
    event.Enable( false );
}

/*!
 * ctOutputWindow represents a page showing a setup.h file or config command.
 */

IMPLEMENT_CLASS(ctOutputWindow, wxPanel)

BEGIN_EVENT_TABLE(ctOutputWindow, wxPanel)
    EVT_BUTTON(wxID_COPY, ctOutputWindow::OnCopyToClipboard)
    EVT_BUTTON(wxID_SAVE, ctOutputWindow::OnSaveText)
    EVT_BUTTON(ctID_REGENERATE, ctOutputWindow::OnRegenerate)
    EVT_UPDATE_UI(wxID_SAVE, ctOutputWindow::OnUpdateSaveText)
    EVT_UPDATE_UI(ctID_REGENERATE, ctOutputWindow::OnUpdateRegenerate)
    EVT_UPDATE_UI(wxID_COPY, ctOutputWindow::OnUpdateCopy)
END_EVENT_TABLE()

ctOutputWindow::ctOutputWindow(wxWindow* parent, wxWindowID id,
                                                   const wxPoint& pos, const wxSize& size, long style):
    wxPanel(parent, id, pos, size, style)
{
    m_codeCtrl = NULL;
    m_filenameCtrl = NULL;
    m_doc = NULL;
    CreateWindows();
}

/// Initialise the windows.
void ctOutputWindow::CreateWindows()
{
    wxBoxSizer *item0 = new wxBoxSizer( wxVERTICAL );

    wxBoxSizer *item2 = new wxBoxSizer( wxHORIZONTAL );
    item0->Add( item2, 0, wxGROW|wxALIGN_CENTER_VERTICAL, 5 );

    wxButton *item4 = new wxButton( this, wxID_COPY, _("&Copy"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item4, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxTOP|wxBOTTOM|wxRIGHT, 5 );
    item4->SetHelpText(_("Copies the selection or whole file to the clipboard."));

    wxButton *item5 = new wxButton( this, ctID_REGENERATE, _("Re&generate"), wxDefaultPosition, wxDefaultSize, 0 );
    item2->Add( item5, 0, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
    item5->SetHelpText(_("Regenerates the code."));

#if 0
    m_filenameCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxSUNKEN_BORDER|wxTE_READONLY);
    item2->Add( m_filenameCtrl, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxTOP|wxBOTTOM|wxRIGHT, 5 );
    m_filenameCtrl->SetHelpText(_("Shows the filename where the code is being saved."));
#else
    m_filenameCtrl = NULL;
#endif

    // The code editor
    m_codeCtrl = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(100, 100), wxTE_RICH|wxTE_MULTILINE|wxSUNKEN_BORDER);
    item0->Add( m_codeCtrl, 1, wxGROW|wxALIGN_CENTER_VERTICAL|wxLEFT|wxRIGHT|wxBOTTOM, 5 );

    SetAutoLayout( true );
    SetSizer( item0 );
}

/// Copies the text to the clipboard.
void ctOutputWindow::OnCopyToClipboard(wxCommandEvent& WXUNUSED(event))
{
    // Try to copy the selection first
    long int selFrom, selTo;
    m_codeCtrl->GetSelection(& selFrom, & selTo);
    if (selFrom != selTo)
    {
        m_codeCtrl->Copy();
        return;
    }

    // Copy the whole amount
    if (!wxTheClipboard->Open())
    {
        wxMessageBox(_("Sorry, could not open the clipboard."), _("Clipboard problem"), wxICON_EXCLAMATION|wxOK);
        return;
    }

    wxString value(m_codeCtrl->GetValue());
#ifdef __WXMSW__
    value.Replace(_T("\n"), _T("\r\n"));
#endif
    wxTextDataObject *data = new wxTextDataObject( value );

    if (!wxTheClipboard->SetData( data ))
    {
        wxTheClipboard->Close();
        wxMessageBox(_("Sorry, could not copy to the clipboard."), _("Clipboard problem"), wxICON_EXCLAMATION|wxOK);
    }
    else
    {
        wxTheClipboard->Close();
    }
}

/// Sets the code in the text control.
void ctOutputWindow::SetText(const wxString& text)
{
    if (m_codeCtrl)
        m_codeCtrl->SetValue(text);
}

/// Sets the filename.
void ctOutputWindow::SetFilename(const wxString& filename)
{
    if (m_filenameCtrl)
        m_filenameCtrl->SetValue(filename);
}

/// Saves the file.
void ctOutputWindow::OnSaveText(wxCommandEvent& WXUNUSED(event))
{
    if (m_codeCtrl->IsModified())
    {
        wxString filename(m_filenameCtrl->GetValue());
        if (!filename.empty())
        {
            m_codeCtrl->SaveFile(filename);
            m_codeCtrl->DiscardEdits();
        }
    }
}

void ctOutputWindow::OnUpdateSaveText(wxUpdateUIEvent& event)
{
    event.Enable(m_doc && m_codeCtrl && m_codeCtrl->IsModified());
}

void ctOutputWindow::OnRegenerate(wxCommandEvent& WXUNUSED(event))
{
    if (m_doc)
    {
        ctConfigToolView* view = (ctConfigToolView*) m_doc->GetFirstView();
        view->RegenerateSetup();
    }
}

void ctOutputWindow::OnUpdateRegenerate(wxUpdateUIEvent& event)
{
    event.Enable( m_doc != NULL );
}

void ctOutputWindow::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( m_doc != NULL );
}
