/*-*- c++ -*-********************************************************
 * wxexthlp.cpp - an external help controller for wxWindows         *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
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



class wxExtHelpMapEntry : public wxObject
{
public:
   int      id;
   wxString url;
   wxString doc;
   wxExtHelpMapEntry(int iid, wxString const &iurl, wxString const &idoc)
      { id = iid; url = iurl; doc = idoc; }
};


struct wxBusyCursor
{
   wxBusyCursor() { wxBeginBusyCursor(); }
   ~wxBusyCursor() { wxEndBusyCursor(); }
};

IMPLEMENT_CLASS(wxExtHelpController, wxHelpControllerBase)
   
/**
   This class implements help via an external browser.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.
*/

wxExtHelpController::wxExtHelpController(void)
{
   m_MapList = NULL;
   m_BrowserName = WXEXTHELP_DEFAULTBROWSER;
   m_BrowserIsNetscape = WXEXTHELP_DEFAULTBROWSER_IS_NETSCAPE;
   m_NumOfEntries = 0;

   char *browser = getenv(WXEXTHELP_ENVVAR_BROWSER);
   if(browser)
   {
      m_BrowserName = browser;
      browser = getenv(WXEXTHELP_ENVVAR_BROWSERISNETSCAPE);
      m_BrowserIsNetscape = browser && (atoi(browser) != 0);
   }
}

void
wxExtHelpController::DeleteList(void)
{
   if(m_MapList)
   {
      wxNode *node = m_MapList->First();
      while (node)
      {
         delete (wxExtHelpMapEntry *)node->Data();
         delete node;
         node = m_MapList->First();
      }
      delete m_MapList;
      m_MapList = NULL;
   }
}

wxExtHelpController::~wxExtHelpController(void)
{
   DeleteList();
}

void
wxExtHelpController::SetBrowser(wxString const & browsername, bool isNetscape)
{
   m_BrowserName = browsername;
   m_BrowserIsNetscape = isNetscape;
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

  
bool
wxExtHelpController::LoadFile(const wxString& ifile)
{
   wxString mapFile, file, url, doc;
   int id,i,len;
   char buffer[WXEXTHELP_BUFLEN];
   
   wxBusyCursor b; // display a busy cursor

   if(! ifile.IsEmpty())
   {
      file = ifile;
      if(! wxIsAbsolutePath(file))
      {
         char* f = wxGetWorkingDirectory();
         file = f;
         delete[] f; // wxGetWorkingDirectory returns new memory
         file << WXEXTHELP_SEPARATOR << ifile;
      }
      else
         file = ifile;
   
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
   
   FILE *input = fopen(mapFile.c_str(),"rt");
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
         url = "";
         while(buffer[i] && ! isspace(buffer[i]) && buffer[i] !=
               WXEXTHELP_COMMENTCHAR)
            url << buffer[i++];
         while(buffer[i] && buffer[i] != WXEXTHELP_COMMENTCHAR)
            i++;
         doc = "";
         if(buffer[i])
            doc = (buffer + i + 1); // skip the comment character
         m_MapList->Append(new wxExtHelpMapEntry(id,url,doc));
         m_NumOfEntries++;
      }
   }while(! feof(input));
   fclose(input);
   
   m_MapFile = file; // now it's valid
   return true;
}

bool
wxExtHelpController::CallBrowser(wxString const &relativeURL)
{
   wxBusyCursor b; // display a busy cursor
   wxString command;

   if(m_BrowserIsNetscape) // try re-loading first
   {
      wxString lockfile;
      wxGetHomeDir(&lockfile);
      lockfile << WXEXTHELP_SEPARATOR << ".netscape/lock";
      struct stat statbuf;
      if(lstat(lockfile.c_str(), &statbuf) == 0)
      // cannot use wxFileExists, because it's a link pointing to a
      // non-existing location      if(wxFileExists(lockfile))
      {
         long success;
         command << m_BrowserName << " -remote openURL("
                 << "file://" << m_MapFile
                 << WXEXTHELP_SEPARATOR << relativeURL << ")"; 
         success = wxExecute(command);
         if(success != 0 ) // returns PID on success
            return true;
      }
   }
   command = m_BrowserName;
   command << " file://"
           << m_MapFile << WXEXTHELP_SEPARATOR << relativeURL; 
   return wxExecute(command) != 0; 
}

bool
wxExtHelpController::DisplayContents(void)
{
   if(! m_NumOfEntries)
      return false;
   wxBusyCursor b; // display a busy cursor
   return KeywordSearch("");
}
      
bool
wxExtHelpController::DisplaySection(int sectionNo)
{
   if(! m_NumOfEntries)
      return false;

   wxBusyCursor b; // display a busy cursor
   wxNode *node = m_MapList->First();
   wxExtHelpMapEntry *entry;
   while(node)
   {
      entry = (wxExtHelpMapEntry *)node->Data();
      if(entry->id == sectionNo)
         return CallBrowser(entry->url);
      node = node->Next();
   }
   return false;
}

bool
wxExtHelpController::DisplayBlock(long blockNo)
{
   return DisplaySection((int)blockNo);
}

bool
wxExtHelpController::KeywordSearch(const wxString& k)
{
   if(! m_NumOfEntries)
      return false;

   wxBusyCursor b; // display a busy cursor
   wxString     *choices = new wxString[m_NumOfEntries];
   wxString     *urls = new wxString[m_NumOfEntries];
   wxString compA, compB;
   
   int          idx = 0, j;
   bool         rc;
   bool         showAll = k.IsEmpty();
   wxNode       *node = m_MapList->First();
   wxExtHelpMapEntry *entry;
   
   compA = k; compA.LowerCase(); // we compare case insensitive
   while(node)
   {
      entry = (wxExtHelpMapEntry *)node->Data();
      compB = entry->doc; compB.LowerCase();
      if((showAll || compB.Contains(k)) && ! compB.IsEmpty()) 
      {
         urls[idx] = entry->url;
         // doesn't work:
         // choices[idx] = (**i).doc.Contains((**i).doc.Before(WXEXTHELP_COMMENTCHAR));
         //if(choices[idx].IsEmpty()) // didn't contain the ';'
         //   choices[idx] = (**i).doc;
         choices[idx] = "";
         for(j=0;entry->doc.c_str()[j]
                && entry->doc.c_str()[j] != WXEXTHELP_COMMENTCHAR; j++)
            choices[idx] << entry->doc.c_str()[j];
         idx++;
      }
      node = node->Next();
   }

   if(idx == 1)
      rc = CallBrowser(urls[0]);
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
         rc = CallBrowser(urls[idx]);
      else
         rc = false;
   }
   delete[] urls;
   delete[] choices;
   
   return rc;
}


bool
wxExtHelpController::Quit(void)
{
   return true;
}

void
wxExtHelpController::OnQuit(void)
{
}

