/////////////////////////////////////////////////////////////////////////////
// Name:        helpext.cpp
// Purpose:     an external help controller for wxWindows
// Author:      Karsten Ballueder
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballueder
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#   pragma implementation "wxexthlp.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/list.h"
    #include "wx/intl.h"
#endif

#include "wx/helpbase.h"
#include "wx/generic/helpext.h"

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

#ifndef __WINDOWS__
    #include   <unistd.h>
#endif

IMPLEMENT_CLASS(wxExtHelpController, wxHTMLHelpControllerBase)
   
/**
   This class implements help via an external browser.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.
*/

wxExtHelpController::wxExtHelpController(void)
{
   m_BrowserName = WXEXTHELP_DEFAULTBROWSER;
   m_BrowserIsNetscape = WXEXTHELP_DEFAULTBROWSER_IS_NETSCAPE;

   char *browser = getenv(WXEXTHELP_ENVVAR_BROWSER);
   if(browser)
   {
      m_BrowserName = browser;
      browser = getenv(WXEXTHELP_ENVVAR_BROWSERISNETSCAPE);
      m_BrowserIsNetscape = browser && (atoi(browser) != 0);
   }
}


void
wxExtHelpController::SetBrowser(wxString const & browsername, bool isNetscape)
{
   m_BrowserName = browsername;
   m_BrowserIsNetscape = isNetscape;
}

// Set viewer: new, generic name for SetBrowser
void wxExtHelpController::SetViewer(const wxString& viewer, long flags)
{
    SetBrowser(viewer, ((flags & wxHELP_NETSCAPE) == wxHELP_NETSCAPE));
}

bool
wxExtHelpController::DisplayHelp(wxString const &relativeURL)
{
   wxBusyCursor b; // display a busy cursor


#ifdef __WXMSW__
   wxString url;
   url << m_MapFile << '\\' << relativeURL.BeforeFirst('#');
   bool bOk = (int)ShellExecute(NULL, "open", url,
                                NULL, NULL, SW_SHOWNORMAL ) > 32;
   if ( !bOk )
   {
      wxLogSysError(_("Cannot open URL '%s'"), relativeURL.c_str());
      return false;
   }
   else
      return true;
#else
   // assume UNIX
   wxString command;

   if(m_BrowserIsNetscape) // try re-loading first
   {
      wxString lockfile;
      wxGetHomeDir(&lockfile);
      lockfile << WXEXTHELP_SEPARATOR << _T(".netscape/lock");
      struct stat statbuf;
      if(lstat(lockfile.fn_str(), &statbuf) == 0)
      // cannot use wxFileExists, because it's a link pointing to a
      // non-existing location      if(wxFileExists(lockfile))
      {
         long success;
         command << m_BrowserName << _T(" -remote openURL(")
                 << _T("file://") << m_MapFile
                 << WXEXTHELP_SEPARATOR << relativeURL << _T(")"); 
         success = wxExecute(command);
         if(success != 0 ) // returns PID on success
            return TRUE;
      }
   }
   command = m_BrowserName;
   command << _T(" file://")
           << m_MapFile << WXEXTHELP_SEPARATOR << relativeURL; 
   return wxExecute(command) != 0; 
#endif
}


