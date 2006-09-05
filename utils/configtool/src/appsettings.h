/////////////////////////////////////////////////////////////////////////////
// Name:        appsettings.h
// Purpose:     Settings-related classes
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#ifndef _AP_APPSETTINGS_H_
#define _AP_APPSETTINGS_H_

#include "wx/notebook.h"
#include "wx/dialog.h"
#include "wx/datetime.h"

#include "wxconfigtool.h"

// Frame status
#define ctSHOW_STATUS_NORMAL        0x01
#define ctSHOW_STATUS_MINIMIZED     0x02
#define ctSHOW_STATUS_MAXIMIZED     0x03

/*!
 * \brief ctSettings holds all the settings that can be altered
 * by the user (and probably some that can't).
 */

class ctSettings: public wxObject
{
DECLARE_DYNAMIC_CLASS(ctSettings)
public:
    /// Default constructor.
    ctSettings();

    /// Copy constructor.
    ctSettings(const ctSettings& settings);

    /// Destructor.
    virtual ~ctSettings(){}

// Operations

    /// Assignment operator.
    void operator = (const ctSettings& settings);

    /// Copy function.
    void Copy (const ctSettings& settings);

    /// Loads configuration information from the registry or a file.
    bool LoadConfig();

    /// Saves configuration information to the registry or a file.
    bool SaveConfig();

    /// Initialisation before LoadConfig is called.
    bool Init();

    /// Shows the settings dialog.
    void ShowSettingsDialog(const wxString& page = wxEmptyString);

    /// Generates a new document filename.
    wxString GenerateFilename(const wxString& rootName);

// Accessors

    /// Returns the long application name. This name is used
    /// for the registry key and main frame titlebar.
    wxString GetAppName() const { return m_appName; }

    /// Returns the short application name.
    wxString GetShortAppName() const { return m_appNameShort; }

    /// Gets the name of the last filename to be loaded.
    /// May not be needed in this application.
    wxString& GetLastFilename() { return m_lastFilename; }

    /// Returns true if this is the first time the application
    /// has been run.
    bool GetFirstTimeRun() const { return m_firstTimeRun; }

public:
    wxString                m_lastSetupSaveDir; // Not yet used
    wxString                m_lastDocument;       // Last document
    bool                    m_showToolBar;
    bool                    m_smallToolbar;
    wxRect                  m_frameSize;
    // wxColour                m_backgroundColour; // background colour
    wxFont                  m_editWindowFont;
    int                     m_noUses; // Number of times the app was invoked

    wxString                m_appName;     // The current name of the app...
    wxString                m_appNameShort;     // The short name of the app...
    bool                    m_showSplashScreen; // Show the splash screen
    wxString                m_userName;
    wxString                m_lastFilename; // So we can auto-generate sensible filenames
    bool                    m_loadLastDocument;
    bool                    m_useToolTips;          // Use tooltips on dialogs
    int                     m_frameStatus;
    wxString                m_exportDir;         // Where files are exported to

    wxString                m_frameworkDir;         // Where the wxWin hierarchy is
    bool                    m_useEnvironmentVariable; // Use WXWIN

    /// This isn't explicitly held in the registry
    /// but deduced from whether there are values there
    bool                    m_firstTimeRun;
    bool                    m_showWelcomeDialog; // Show opening helpful dialog

    int                     m_mainSashSize;
    bool                    m_showTrayIcon;
    bool                    m_trayIconIsShown;

    // "Setup file" or "Configure script"
    wxString                m_defaultFileKind;

    // Search settings
    bool                    m_matchCase;
    bool                    m_matchWholeWord;
};

#endif
    // _AP_APPSETTINGS_H_

