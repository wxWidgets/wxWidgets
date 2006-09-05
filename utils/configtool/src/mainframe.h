/////////////////////////////////////////////////////////////////////////////
// Name:        mainframe.h
// Purpose:     Main frame class
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _AP_MAINFRAME_H_
#define _AP_MAINFRAME_H_

#include "wx/imaglist.h"
#include "wx/docview.h"
#include "wx/notebook.h"
#include "wx/textctrl.h"

// #define USE_CONFIG_BROWSER_PAGE

class WXDLLEXPORT wxHtmlWindow;
class WXDLLEXPORT wxSplitterWindow;
class WXDLLEXPORT wxNotebookEvent;

class ctConfigTreeCtrl;
class ctPropertyEditor;
class ctOutputWindow;
class ctFindReplaceDialog;
#ifdef USE_CONFIG_BROWSER_PAGE
class ctConfigurationBrowserWindow;
#endif

/*!
 * \brief The main window of the application.
 */

class ctMainFrame: public wxDocParentFrame
{
    DECLARE_CLASS(ctMainFrame)
public:
    /// Constructor.
    ctMainFrame(wxDocManager *manager, wxFrame *parent, wxWindowID id, const wxString& title, const wxPoint& pos, const wxSize& size, long style);

// Event handlers

    /// Handles the close window event.
    void OnCloseWindow(wxCloseEvent& event);

    /// Handles the About menu event.
    void OnAbout(wxCommandEvent& event);

    /// Handles the frame activation event.
    void OnActivate(wxActivateEvent& event);

    /// Handles the File Open menu event.
    void OnOpen(wxCommandEvent& event);

    /// Handles the File New menu event.
    void OnNew(wxCommandEvent& event);

    /// Handles the Exit menu event.
    void OnExit(wxCommandEvent& event);

    /// Handles the Settings menu event.
    void OnSettings(wxCommandEvent& event);

    /// Handles the Show Toolbar menu event.
    void OnShowToolbar(wxCommandEvent& event);

    /// Handles the Help Contents menu event.
    void OnHelp(wxCommandEvent& event);

    /// Handles context help
    void OnContextHelp(wxCommandEvent& event);

    /// Handles the Help Contents menu event for the reference manual.
    void OnReferenceHelp(wxCommandEvent& event);

    /// General disabler
    void OnUpdateDisable(wxUpdateUIEvent& event);

// Operations

    /// Creates the main frame subwindows.
    bool CreateWindows(wxWindow* parent);

    /// Initialises the toolbar.
    void InitToolBar(wxToolBar* toolbar);

    /// Creates the menubar.
    wxMenuBar* CreateMenuBar();

    /// Resizes the main frame according to the
    /// state of the toolbar
    void ResizeFrame();

    /// Update the frame title.
    void UpdateFrameTitle();

// Accessors

    /// Returns the tree control.
    ctConfigTreeCtrl* GetConfigTreeCtrl() const { return m_configTreeCtrl; }

    /// Returns the property editor window.
    ctPropertyEditor* GetPropertyEditor() const { return m_propertyEditor; }

    /// Returns the document for this frame.
    ctConfigToolDoc* GetDocument() const { return m_document; }

    /// Sets the document for this frame.
    void SetDocument(ctConfigToolDoc* doc) { m_document = doc; }

    /// Returns the edit menu.
    wxMenu* GetEditMenu() const { return m_editMenu; }

    /// Returns the setup page window
    ctOutputWindow* GetSetupPage() const { return m_setupPage; }

    /// Returns the configure page window
    ctOutputWindow* GetConfigurePage() const { return m_configurePage; }

    /// Returns the main notebook containing editor and text tabs
    wxNotebook* GetMainNotebook() const { return m_mainNotebook; }

    /// Sets the find dialog for future closing
    void SetFindDialog(ctFindReplaceDialog* findDialog) { m_findDialog = findDialog; }

    /// Gets the find dialog
    ctFindReplaceDialog* GetFindDialog() const { return m_findDialog ; }

DECLARE_EVENT_TABLE()

protected:
    wxImageList             m_imageList;

    /// The splitter between the tree and the property window.
    wxSplitterWindow*       m_treeSplitterWindow;

    /// The config tree window.
    ctConfigTreeCtrl*       m_configTreeCtrl;

    /// The property editor window.
    ctPropertyEditor*       m_propertyEditor;

    /// The document for this frame.
    ctConfigToolDoc*        m_document;

    /// The edit menu.
    wxMenu*                 m_editMenu;

    /// The notebook with property editor and setup.h/configure
    /// views
    wxNotebook*             m_mainNotebook;
    ctOutputWindow*         m_setupPage;
    ctOutputWindow*         m_configurePage;

    // The control panel for browsing, adding and removing
    // configurations.
#ifdef USE_CONFIG_BROWSER_PAGE
    ctConfigurationBrowserWindow*   m_configBrowserPage;
#endif

    ctFindReplaceDialog*    m_findDialog;
};

/*!
 * ctOutputWindow represents a page showing a setup.h file or config command.
 */

class ctOutputWindow: public wxPanel
{
DECLARE_CLASS(ctOutputWindow)
public:
    ctOutputWindow(wxWindow* parent, wxWindowID id,
        const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, long style = 0);
    virtual ~ctOutputWindow(){}

    /// Initialise the windows.
    void CreateWindows();

    /// Copies the text to the clipboard.
#if wxUSE_CLIPBOARD
    void OnCopyToClipboard(wxCommandEvent& event);
#endif // wxUSE_CLIPBOARD
    void OnUpdateCopy(wxUpdateUIEvent& event);

    /// Regenerates setup.h/configure command
    void OnRegenerate(wxCommandEvent& event);
    void OnUpdateRegenerate(wxUpdateUIEvent& event);

    /// Saves the file.
    void OnSaveText(wxCommandEvent& event);
    void OnUpdateSaveText(wxUpdateUIEvent& event);

    /// Sets the code in the text control.
    void SetText(const wxString& text);

    /// Sets the filename.
    void SetFilename(const wxString& filename);

    /// Sets the document
    void SetDocument(ctConfigToolDoc* doc) { m_doc = doc; }

    /// Get text control
    wxTextCtrl* GetCodeCtrl() const { return m_codeCtrl; }

    /// Get filename control
    wxTextCtrl* GetFilenameCtrl() const { return m_filenameCtrl; }

protected:
    wxTextCtrl* m_codeCtrl;
    wxTextCtrl* m_filenameCtrl;
    ctConfigToolDoc* m_doc;

DECLARE_EVENT_TABLE()
};


#endif
  // _AP_MAINFRAME_H_

