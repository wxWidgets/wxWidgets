/////////////////////////////////////////////////////////////////////////////
// Name:        wxconfigtool.h
// Purpose:     Generic application class
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _AP_WXCONFIGTOOL_H_
#define _AP_WXCONFIGTOOL_H_

#include "wx/docview.h"
#include "wx/help.h"
#include "wx/app.h"

#ifdef __WXMSW__
#include "wx/msw/helpchm.h"
#else
#include "wx/html/helpctrl.h"
#endif

#include "appsettings.h"

class ctMainFrame;
class ctConfigToolDoc;

/*!
 * \brief The application class.
 * The application class controls initialisation,
 * cleanup and other application-wide issues.
 */

class ctApp: public wxApp
{
public:
    /// Constructor.
    ctApp();

    /// Destructor.
    virtual ~ctApp(){}

// Accessors

    /// Returns the application directory.
    wxString GetAppDir() const { return m_appDir; }

    /// Prepends the current app program directory to the name.
    wxString GetFullAppPath(const wxString& filename) const;

    /// Returns an object containing the application settings.
    ctSettings& GetSettings() { return m_settings; }

    /// Returns the file history object.
    wxFileHistory& GetFileHistory() { return m_fileHistory; }

    /// Returns the notebook window.
    wxNotebook* GetNotebookWindow() { return m_notebookWindow; }

    /// Returns true if the application should show tooltips.
    virtual bool UsingTooltips();

    /// Returns the help controller object for the manual.
    wxHelpControllerBase& GetHelpController() { return *m_helpController; }

    /// Returns the help controller object for the wxWidgets reference manual.
    wxHelpControllerBase& GetReferenceHelpController() { return *m_helpControllerReference; }

    /// Returns the document manager object.
    wxDocManager* GetDocManager() const { return m_docManager; }

    /// Returns the main frame.
    ctMainFrame* GetMainFrame();

// Operations

    /// Called on application initialisation.
    bool OnInit(void);

    /// Called on application exit.
    int OnExit(void);

    /// Loads config info from the registry or a file.
    virtual bool LoadConfig();

    /// Saves config info to the registry or a file.
    virtual bool SaveConfig();

    /// The help controller needs to be cleared before wxWidgets
    /// cleanup happens.
    void ClearHelpControllers() ;

protected:
    /// The application directory.
    wxString                m_appDir;

    /// Notebook window.
    wxNotebook*             m_notebookWindow;

    /// The help controller object.
    wxHelpControllerBase*   m_helpController;

    /// The help controller object (reference manual).
    wxHelpControllerBase*   m_helpControllerReference;

    /// The file history.
    wxFileHistory           m_fileHistory;

    /// The configuration data.
    ctSettings              m_settings;

    /// The document manager.
    wxDocManager*           m_docManager;

public:
DECLARE_EVENT_TABLE()
};

DECLARE_APP(ctApp)

/////////////////////////////////////////////////
// Menu ids
/////////////////////////////////////////////////

// File menu ids
#define ctID_SAVE_SETUP_FILE                1001
#define ctID_SAVE_CONFIGURE_COMMAND         1002
#define ctID_GO                             1003

// Edit menu ids
#define ctID_ADD_ITEM                       1030
#define ctID_ADD_ITEM_CHECKBOX              1031
#define ctID_ADD_ITEM_RADIOBUTTON           1032
#define ctID_ADD_ITEM_STRING                1033
#define ctID_ADD_ITEM_GROUP                 1034
#define ctID_ADD_ITEM_CHECK_GROUP           1035
#define ctID_ADD_ITEM_RADIO_GROUP           1036
#define ctID_DELETE_ITEM                    1037
#define ctID_RENAME_ITEM                    1038
#define ctID_CUSTOM_PROPERTY                1039
#define ctID_ADD_CUSTOM_PROPERTY            1040
#define ctID_EDIT_CUSTOM_PROPERTY           1041
#define ctID_DELETE_CUSTOM_PROPERTY         1042

// View menu ids
#define ctID_SETTINGS                       1020
#define ctID_SHOW_TOOLBAR                   1021

// Help menu ids
#define ctID_GOTO_WEBSITE                   1050
#define ctID_ITEM_HELP                      1051
#define ctID_REFERENCE_CONTENTS             1052

// Taskbar menu ids
#define ctID_TASKBAR_EXIT_APP               1202
#define ctID_TASKBAR_SHOW_APP               1203

// Tree context menu
#define ctID_TREE_PASTE_BEFORE              1300
#define ctID_TREE_PASTE_AFTER               1301
#define ctID_TREE_PASTE_AS_CHILD            1302
#define ctID_TREE_COPY                      1303
#define ctID_TREE_CUT                       1304

/////////////////////////////////////////////////
// Window/control ids
/////////////////////////////////////////////////

// Settings dialogs
#define ctID_SETTINGS_GENERAL               1500

// Regenerate setup.h/configure command
#define ctID_REGENERATE                     1600

#endif
  // _AP_WXCONFIGTOOL_H_

