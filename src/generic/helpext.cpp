/////////////////////////////////////////////////////////////////////////////
// Name:        helpext.cpp
// Purpose:     an external help controller for wxWidgets
// Author:      Karsten Ballueder
// Modified by:
// Created:     04/01/98
// RCS-ID:      $Id$
// Copyright:   (c) Karsten Ballueder
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
#   pragma implementation "wxexthlp.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_HELP && !defined(__WXWINCE__)

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/string.h"
    #include "wx/utils.h"
    #include "wx/list.h"
    #include "wx/intl.h"
    #include "wx/msgdlg.h"
    #include "wx/choicdlg.h"
    #include "wx/log.h"
#endif

#include "wx/helpbase.h"
#include "wx/generic/helpext.h"

#include <stdio.h>
#include <ctype.h>
#include <sys/stat.h>

#if !defined(__WINDOWS__) && !defined(__OS2__)
    #include   <unistd.h>
#endif

#ifdef __WXMSW__
#include <windows.h>
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

/// Name for map file.
#define WXEXTHELP_MAPFILE   _T("wxhelp.map")
/// Maximum line length in map file.
#define WXEXTHELP_BUFLEN 512
/// Character introducing comments/documentation field in map file.
#define WXEXTHELP_COMMENTCHAR   ';'

#define CONTENTS_ID   0

IMPLEMENT_CLASS(wxExtHelpController, wxHelpControllerBase)

/// Name of environment variable to set help browser.
#define   WXEXTHELP_ENVVAR_BROWSER   wxT("WX_HELPBROWSER")
/// Is browser a netscape browser?
#define   WXEXTHELP_ENVVAR_BROWSERISNETSCAPE wxT("WX_HELPBROWSER_NS")

/**
   This class implements help via an external browser.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.
*/

wxExtHelpController::wxExtHelpController()
{
   m_MapList = (wxList*) NULL;
   m_NumOfEntries = 0;
   m_BrowserName = WXEXTHELP_DEFAULTBROWSER;
   m_BrowserIsNetscape = WXEXTHELP_DEFAULTBROWSER_IS_NETSCAPE;

   wxChar *browser = wxGetenv(WXEXTHELP_ENVVAR_BROWSER);
   if(browser)
   {
      m_BrowserName = browser;
      browser = wxGetenv(WXEXTHELP_ENVVAR_BROWSERISNETSCAPE);
      m_BrowserIsNetscape = browser && (wxAtoi(browser) != 0);
   }
}

wxExtHelpController::~wxExtHelpController()
{
   DeleteList();
}

void wxExtHelpController::SetBrowser(const wxString& browsername, bool isNetscape)
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
wxExtHelpController::DisplayHelp(const wxString &relativeURL)
{
   wxBusyCursor b; // display a busy cursor


#if defined(__WXMSW__)
   wxString url;
   url << m_MapFile << '\\' << relativeURL.BeforeFirst('#');
   bool bOk = (int)ShellExecute(NULL, wxT("open"), url.c_str(),
                                NULL, NULL, SW_SHOWNORMAL ) > 32;
   if ( !bOk )
   {
      wxLogSysError(_("Cannot open URL '%s'"), relativeURL.c_str());
      return false;
   }

   return true;
#elif  defined(__WXPM__)

   wxString url;
   url << m_MapFile << '\\' << relativeURL.BeforeFirst('#');
//   will have to fix for OS/2, later.....DW
//   bool bOk = (int)ShellExecute(NULL, "open", url,
//                                NULL, NULL, SW_SHOWNORMAL ) > 32;
//   if ( !bOk )
//   {
//      wxLogSysError(_("Cannot open URL '%s'"), relativeURL.c_str());
//      return false;
//   }
//   else
      return true;

#elif defined(__DOS__)

   wxString command;
   command = m_BrowserName;
   command << wxT(" file://")
           << m_MapFile << WXEXTHELP_SEPARATOR << relativeURL;
   return wxExecute(command) != 0;

#else // UNIX
   wxString command;

#ifndef __EMX__
   if(m_BrowserIsNetscape) // try re-loading first
   {
      wxString lockfile;
      wxGetHomeDir(&lockfile);
#ifdef __VMS__
      lockfile << WXEXTHELP_SEPARATOR << wxT(".netscape]lock.");
      struct stat statbuf;
      if(stat(lockfile.fn_str(), &statbuf) == 0)
#else
      lockfile << WXEXTHELP_SEPARATOR << wxT(".netscape/lock");
      struct stat statbuf;
      if(lstat(lockfile.fn_str(), &statbuf) == 0)
      // cannot use wxFileExists, because it's a link pointing to a
      // non-existing location      if(wxFileExists(lockfile))
#endif
      {
         long success;
         command << m_BrowserName << wxT(" -remote openURL(")
                 << wxT("file://") << m_MapFile
                 << WXEXTHELP_SEPARATOR << relativeURL << wxT(")");
         success = wxExecute(command);
         if(success != 0 ) // returns PID on success
            return true;
      }
   }
#endif
   command = m_BrowserName;
   command << wxT(" file://")
           << m_MapFile << WXEXTHELP_SEPARATOR << relativeURL;
   return wxExecute(command) != 0;
#endif
}

class wxExtHelpMapEntry : public wxObject
{
public:
   int      id;
   wxString url;
   wxString doc;
   wxExtHelpMapEntry(int iid, wxString const &iurl, wxString const &idoc)
      { id = iid; url = iurl; doc = idoc; }
};

void wxExtHelpController::DeleteList()
{
   if(m_MapList)
   {
      wxList::compatibility_iterator node = m_MapList->GetFirst();
      while (node)
      {
         delete (wxExtHelpMapEntry *)node->GetData();
         m_MapList->Erase(node);
         node = m_MapList->GetFirst();
      }
      delete m_MapList;
      m_MapList = (wxList*) NULL;
   }
}

/** This must be called to tell the controller where to find the
    documentation.
    @param file - NOT a filename, but a directory name.
    @return true on success
*/
bool
wxExtHelpController::Initialize(const wxString& file)
{
   return LoadFile(file);
}


// ifile is the name of the base help directory
bool wxExtHelpController::LoadFile(const wxString& ifile)
{
   wxString mapFile, file, url, doc;
   int id,i,len;
   char buffer[WXEXTHELP_BUFLEN];

   wxBusyCursor b; // display a busy cursor

   if(! ifile.empty())
   {
      file = ifile;
      if(! wxIsAbsolutePath(file))
      {
         wxChar* f = wxGetWorkingDirectory();
         file = f;
         delete[] f; // wxGetWorkingDirectory returns new memory
#ifdef __WXMAC__
         file << ifile;
#else
         file << WXEXTHELP_SEPARATOR << ifile;
#endif
      }
      else
         file = ifile;

#if wxUSE_INTL
      // If a locale is set, look in file/localename, i.e.
      // If passed "/usr/local/myapp/help" and the current wxLocale is
      // set to be "de", then look in "/usr/local/myapp/help/de/"
      // first and fall back to "/usr/local/myapp/help" if that
      // doesn't exist.
      if(wxGetLocale() && !wxGetLocale()->GetName().empty())
      {
         wxString newfile;
         newfile << WXEXTHELP_SEPARATOR << wxGetLocale()->GetName();
         if(wxDirExists(newfile))
            file = newfile;
         else
         {
            newfile = WXEXTHELP_SEPARATOR;
            const wxChar *cptr = wxGetLocale()->GetName().c_str();
            while(*cptr && *cptr != wxT('_'))
               newfile << *(cptr++);
            if(wxDirExists(newfile))
               file = newfile;
         }
      }
#endif

      if(! wxDirExists(file))
         return false;

      mapFile << file << WXEXTHELP_SEPARATOR << WXEXTHELP_MAPFILE;
   }
   else // try to reload old file
      mapFile = m_MapFile;

   if(! wxFileExists(mapFile))
      return false;

   DeleteList();
   m_MapList = new wxList;
   m_NumOfEntries = 0;

   FILE *input = wxFopen(mapFile.fn_str(),wxT("rt"));
   if(! input)
      return false;
   do
   {
      if(fgets(buffer,WXEXTHELP_BUFLEN,input) && *buffer != WXEXTHELP_COMMENTCHAR)
      {
         len = strlen(buffer);
         if(buffer[len-1] == '\n')
            buffer[len-1] = '\0'; // cut of trailing newline
         if(sscanf(buffer,"%d", &id) != 1)
            break; // error
         for(i=0; isdigit(buffer[i])||isspace(buffer[i])||buffer[i]=='-'; i++)
            ; // find begin of URL
         url = wxEmptyString;
         while(buffer[i] && ! isspace(buffer[i]) && buffer[i] !=
               WXEXTHELP_COMMENTCHAR)
            url << (wxChar) buffer[i++];
         while(buffer[i] && buffer[i] != WXEXTHELP_COMMENTCHAR)
            i++;
         doc = wxEmptyString;
         if(buffer[i])
            doc = wxString::FromAscii( (buffer + i + 1) ); // skip the comment character
         m_MapList->Append(new wxExtHelpMapEntry(id,url,doc));
         m_NumOfEntries++;
      }
   }while(! feof(input));
   fclose(input);

   m_MapFile = file; // now it's valid
   return true;
}


bool
wxExtHelpController::DisplayContents()
{
   if(! m_NumOfEntries)
      return false;

   wxString contents;
   wxList::compatibility_iterator node = m_MapList->GetFirst();
   wxExtHelpMapEntry *entry;
   while(node)
   {
      entry = (wxExtHelpMapEntry *)node->GetData();
      if(entry->id == CONTENTS_ID)
      {
         contents = entry->url;
         break;
      }
      node = node->GetNext();
   }

   bool rc = false;
   wxString file;
   file << m_MapFile << WXEXTHELP_SEPARATOR << contents;
   if(file.Contains(wxT('#')))
      file = file.BeforeLast(wxT('#'));
   if(contents.Length() && wxFileExists(file))
      rc = DisplaySection(CONTENTS_ID);

   // if not found, open homemade toc:
   return rc ? true : KeywordSearch(wxEmptyString);
}

bool
wxExtHelpController::DisplaySection(int sectionNo)
{
   if(! m_NumOfEntries)
      return false;

   wxBusyCursor b; // display a busy cursor
   wxList::compatibility_iterator node = m_MapList->GetFirst();
   wxExtHelpMapEntry *entry;
   while(node)
   {
      entry = (wxExtHelpMapEntry *)node->GetData();
      if(entry->id == sectionNo)
         return DisplayHelp(entry->url);
      node = node->GetNext();
   }
   return false;
}

bool wxExtHelpController::DisplaySection(const wxString& section)
{
    bool isFilename = (section.Find(wxT(".htm")) != -1);

    if (isFilename)
        return DisplayHelp(section);
    else
        return KeywordSearch(section);
}

bool
wxExtHelpController::DisplayBlock(long blockNo)
{
   return DisplaySection((int)blockNo);
}

bool
wxExtHelpController::KeywordSearch(const wxString& k,
                                   wxHelpSearchMode WXUNUSED(mode))
{
   if(! m_NumOfEntries)
      return false;

   wxString     *choices = new wxString[m_NumOfEntries];
   wxString     *urls = new wxString[m_NumOfEntries];
   wxString compA, compB;

   int          idx = 0, j;
   bool         rc;
   bool         showAll = k.empty();
   wxList::compatibility_iterator node = m_MapList->GetFirst();
   wxExtHelpMapEntry *entry;

   {
      wxBusyCursor b; // display a busy cursor
      compA = k; compA.LowerCase(); // we compare case insensitive
      while(node)
      {
         entry = (wxExtHelpMapEntry *)node->GetData();
         compB = entry->doc; compB.LowerCase();
         if((showAll || compB.Contains(k)) && ! compB.empty())
         {
            urls[idx] = entry->url;
            // doesn't work:
            // choices[idx] = (**i).doc.Contains((**i).doc.Before(WXEXTHELP_COMMENTCHAR));
            //if(choices[idx].empty()) // didn't contain the ';'
            //   choices[idx] = (**i).doc;
            choices[idx] = wxEmptyString;
            for(j=0;entry->doc.c_str()[j]
                   && entry->doc.c_str()[j] != WXEXTHELP_COMMENTCHAR; j++)
               choices[idx] << entry->doc.c_str()[j];
            idx++;
         }
         node = node->GetNext();
      }
   }

   if(idx == 1)
      rc = DisplayHelp(urls[0]);
   else if(idx == 0)
   {
      wxMessageBox(_("No entries found."));
      rc = false;
   }
   else
   {
      idx = wxGetSingleChoiceIndex(showAll ? _("Help Index") : _("Relevant entries:"),
                                   showAll ? _("Help Index") : _("Entries found"),
                                   idx,choices);
      if(idx != -1)
         rc = DisplayHelp(urls[idx]);
      else
         rc = false;
   }
   delete[] urls;
   delete[] choices;

   return rc;
}


bool wxExtHelpController::Quit()
{
   return true;
}

void wxExtHelpController::OnQuit()
{
}


#endif // wxUSE_HELP

