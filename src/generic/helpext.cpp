/*-*- c++ -*-********************************************************
 * helpext.cpp - an external help controller for wxWindows          *
 *                                                                  *
 * (C) 1999 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/
#ifdef __GNUG__
#   pragma implementation "wxexthlp.h"
#endif

#include   "wx/setup.h"
#include   "wx/helpbase.h"
#include   "wx/generic/helpext.h"
#include   "wx/string.h"
#include   "wx/utils.h"
#include   "wx/list.h"
#include   <stdio.h>
#include   <ctype.h>
#include   <sys/stat.h>
#include   <unistd.h>

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
}


