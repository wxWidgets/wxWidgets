/////////////////////////////////////////////////////////////////////////////
// Name:        appsettings.cpp
// Purpose:     Implements settings-related functionality
// Author:      Julian Smart
// Modified by:
// Created:     2002-09-04
// RCS-ID:      $Id$
// Copyright:   (c) Julian Smart
// Licence:
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#pragma implementation "appsettings.h"
#endif

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#ifndef WX_PRECOMP

#include "wx/datstrm.h"
#include "wx/dir.h"
#include "wx/colordlg.h"
#include "wx/wxhtml.h"
#include "wx/effects.h"
#include "wx/spinctrl.h"
#include "wx/tooltip.h"

#endif

#include "wx/wfstream.h"
#include "wx/config.h"
#include "wx/fileconf.h"
#include "wx/valgen.h"
#include "utils.h"
#include "wxconfigtool.h"
#include "appsettings.h"
#include "mainframe.h"
#include "symbols.h"
#include "settingsdialog.h"

/*
 * User-changeable settings
 */

IMPLEMENT_DYNAMIC_CLASS(ctSettings, wxObject)

ctSettings::ctSettings()
{
    m_noUses = 0;
    m_showToolBar = true;
    m_showWelcomeDialog = true;
    m_exportDir = wxEmptyString;

    m_frameSize = wxRect(10, 10, 600, 500);
    // m_backgroundColour = wxColour(140, 172, 179); // blue-grey
    m_editWindowFont = wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT);

    m_appName = wxT("wxWidgets Configuration Tool");
    m_appNameShort = wxT("Configuration Tool");
    m_showSplashScreen = false;
    m_userName = wxEmptyString;
    m_frameStatus = ctSHOW_STATUS_NORMAL;
    m_loadLastDocument = true;
    m_firstTimeRun = true;
    m_smallToolbar = true;
    m_mainSashSize = 200;
    m_useToolTips = true;
    m_showTrayIcon = true;
    m_trayIconIsShown = false;
    m_useEnvironmentVariable = true;
    m_frameworkDir = wxEmptyString;
    m_matchWholeWord = false;
    m_matchCase = false;
    m_defaultFileKind = wxT("Setup file");
}

// Copy constructor
ctSettings::ctSettings(const ctSettings& settings)
{
    Copy(settings);
}


void ctSettings::operator = (const ctSettings& settings)
{
    Copy(settings);
}

void ctSettings::Copy (const ctSettings& settings)
{
    m_lastSetupSaveDir = settings.m_lastSetupSaveDir;
    m_lastDocument = settings.m_lastDocument;
    m_showToolBar = settings.m_showToolBar;
    m_frameSize = settings.m_frameSize;
    m_editWindowFont = settings.m_editWindowFont;
    m_showSplashScreen = settings.m_showSplashScreen;
    m_userName = settings.m_userName;
    m_loadLastDocument = settings.m_loadLastDocument;
    m_exportDir = settings.m_exportDir;
    m_firstTimeRun = settings.m_firstTimeRun;
    m_noUses = settings.m_noUses;
    m_smallToolbar = settings.m_smallToolbar;
    m_useToolTips = settings.m_useToolTips;
    m_showWelcomeDialog = settings.m_showWelcomeDialog;
    m_mainSashSize = settings.m_mainSashSize;

    m_showTrayIcon = settings.m_showTrayIcon;
    m_trayIconIsShown = settings.m_trayIconIsShown;

    m_useEnvironmentVariable = settings.m_useEnvironmentVariable;
    m_frameworkDir = settings.m_frameworkDir;
    m_matchWholeWord = settings.m_matchWholeWord;
    m_matchCase = settings.m_matchCase;
    m_defaultFileKind  = settings.m_defaultFileKind ;
}

// Do some initialisation within stApp::OnInit
bool ctSettings::Init()
{
    m_lastSetupSaveDir = wxEmptyString;
    if (m_userName.empty())
        m_userName = wxGetUserName();

    return true;
}

// Create new filename
wxString ctSettings::GenerateFilename(const wxString& rootName)
{
    wxString path;
    if (!m_lastFilename.empty())
        path = wxPathOnly(m_lastFilename);
    else
        path = wxGetApp().GetAppDir();

    wxString filename(path);
    if (filename.Last() != wxFILE_SEP_PATH )
        filename += wxFILE_SEP_PATH;
    filename += rootName;

    wxString fullFilename = filename + wxT(".wxs");
    int i = 0;
    wxString postfixStr;
    while (wxFileExists(fullFilename))
    {
        i ++;
        postfixStr.Printf(_T("%d"), i);
        fullFilename = filename + postfixStr + wxT(".wxs");
    }

    m_lastFilename = fullFilename;
    return fullFilename;
}

// Load config info
bool ctSettings::LoadConfig()
{
    wxConfig config(wxGetApp().GetSettings().GetAppName(), wxT("wxWidgets"));

    config.Read(wxT("Files/LastFile"), & m_lastFilename);
    config.Read(wxT("Files/LastSetupSaveDir"), & m_lastSetupSaveDir);
    config.Read(wxT("Files/ExportDir"), & m_exportDir);
    config.Read(wxT("Files/FrameworkDir"), & m_frameworkDir);
    config.Read(wxT("Files/UseEnvironmentVariable"), (bool*) & m_useEnvironmentVariable);

    config.Read(wxT("Misc/UserName"), & m_userName);
    config.Read(wxT("Misc/FrameStatus"), & m_frameStatus);
    config.Read(wxT("Misc/ShowToolTips"), (bool*) & m_useToolTips);
    config.Read(wxT("Misc/LastDocument"), & m_lastDocument);
    config.Read(wxT("Misc/LoadLastDocument"), (bool*) & m_loadLastDocument);
    config.Read(wxT("Misc/ShowWelcomeDialog"), (bool*) & m_showWelcomeDialog);
    config.Read(wxT("Misc/Ran"), & m_noUses);
    config.Read(wxT("Misc/ShowTrayIcon"), (bool*) & m_showTrayIcon);
    config.Read(wxT("Misc/MatchWholeWord"), (bool*) & m_matchWholeWord);
    config.Read(wxT("Misc/MatchCase"), (bool*) & m_matchCase);
    config.Read(wxT("Misc/BuildMode"), & m_defaultFileKind );

    m_noUses ++;

    config.Read(wxT("Windows/ShowToolBar"), (bool*) & m_showToolBar);

    m_firstTimeRun = !(config.Read(wxT("Windows/WindowX"), & m_frameSize.x));
    config.Read(wxT("Windows/WindowY"), & m_frameSize.y);
    config.Read(wxT("Windows/WindowWidth"), & m_frameSize.width);
    config.Read(wxT("Windows/WindowHeight"), & m_frameSize.height);
    config.Read(wxT("Windows/ShowSplashScreen"), (bool*) & m_showSplashScreen);
    config.Read(wxT("Windows/SmallToolbar"), (bool*) & m_smallToolbar);
    config.Read(wxT("Windows/MainSashSize"), & m_mainSashSize);

    wxString fontSpec;

    fontSpec = wxEmptyString;
    config.Read(wxT("Style/EditWindowFont"), & fontSpec);
    if (!fontSpec.empty())
        m_editWindowFont = apStringToFont(fontSpec);

    // Crash-resistance
    int runningProgram = 0;
    config.Read(wxT("Misc/RunningProgram"), & runningProgram);

#ifndef __WXDEBUG__
    // runningProgram should be zero if all is well. If 1,
    // it crashed during a run, so we should disable the auto-load
    // facility just in case it's trying to load a damaged file.
    if (runningProgram != 0)
    {
        m_loadLastDocument = false;
    }
#endif

    config.Write(wxT("Misc/RunningProgram"), (long) 1);

    return true;
}

// Save config info
bool ctSettings::SaveConfig()
{
    wxConfig config(wxGetApp().GetSettings().GetAppName(), wxT("wxWidgets"));

    config.Write(wxT("Files/LastFile"), m_lastFilename);
    config.Write(wxT("Files/LastSetupSaveDir"), m_lastSetupSaveDir);
    config.Write(wxT("Files/ExportDir"), m_exportDir);
    config.Write(wxT("Files/FrameworkDir"), m_frameworkDir);
    config.Write(wxT("Files/UseEnvironmentVariable"), m_useEnvironmentVariable);

    config.Write(wxT("Misc/UserName"), m_userName);
    config.Write(wxT("Misc/FrameStatus"), (long) m_frameStatus);
    config.Write(wxT("Misc/ShowToolTips"), m_useToolTips);
    config.Write(wxT("Misc/LastDocument"), m_lastDocument);
    config.Write(wxT("Misc/LoadLastDocument"), (long) m_loadLastDocument);
    config.Write(wxT("Misc/ShowWelcomeDialog"), (long) m_showWelcomeDialog);
    config.Write(wxT("Misc/Ran"), m_noUses);
    config.Write(wxT("Misc/ShowTrayIcon"), (long) m_showTrayIcon);
    config.Write(wxT("Misc/MatchWholeWord"), (long) m_matchWholeWord);
    config.Write(wxT("Misc/MatchCase"), (long) m_matchCase);
    config.Write(wxT("Misc/BuildMode"), m_defaultFileKind);

    config.Write(wxT("Windows/ShowToolBar"), m_showToolBar);
    config.Write(wxT("Windows/WindowX"), (long) m_frameSize.x);
    config.Write(wxT("Windows/WindowY"), (long) m_frameSize.y);
    config.Write(wxT("Windows/WindowWidth"), (long) m_frameSize.width);
    config.Write(wxT("Windows/WindowHeight"), (long) m_frameSize.height);

    config.Write(wxT("Windows/ShowSplashScreen"), m_showSplashScreen);
    config.Write(wxT("Windows/SmallToolbar"), (long) m_smallToolbar);
    config.Write(wxT("Windows/MainSashSize"), (long) m_mainSashSize);

    /*
    wxString backgroundColour(apColourToHexString(m_backgroundColour));
    config.Write(wxT("Style/BackgroundColour"), backgroundColour);
    */

    config.Write(wxT("Style/EditWindowFont"), apFontToString(m_editWindowFont));

    // Indicate that we're no longer running, so we know if the program
    // crashed last time around.
    config.Write(wxT("Misc/RunningProgram"), (long) 0);

    {
        config.SetPath(wxT("FileHistory/"));
        wxGetApp().GetDocManager()->FileHistorySave(config);
    }

    return true;
}

void ctSettings::ShowSettingsDialog(const wxString& WXUNUSED(page))
{
    ctSettingsDialog* dialog = new ctSettingsDialog(wxGetApp().GetTopWindow());
//    if (!page.IsEmpty())
//        dialog->GetNotebook()->SetSelection(apFindNotebookPage(dialog->GetNotebook(), page));

    /* int ret = */ dialog->ShowModal();
    dialog->Destroy();
}
