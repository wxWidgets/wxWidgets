/*-*- c++ -*-********************************************************
 * exthlp.h - an external help controller for wxWindows             *
 *                                                                  *
 * (C) 1998 by Karsten Ballüder (Ballueder@usa.net)                 *
 *                                                                  *
 * $Id$
 *******************************************************************/
#ifndef WXXHELP_H
#define WXXHELP_H

#if wxUSE_HELP

#ifdef __GNUG__
#   pragma interface "wxexthlp.h"
#endif

/// Name for map file.
#define WXEXTHELP_MAPFILE   "wxhelp.map"
/// Path separator.
#define WXEXTHELP_SEPARATOR '/'
#ifndef WXEXTHELP_DEFAULTBROWSER
/// Default browser name.
#   define WXEXTHELP_DEFAULTBROWSER "kdehelp"
/// Is default browse a variant of netscape?
#   define WXEXTHELP_DEFAULTBROWSER_IS_NETSCAPE false
#endif
/// Name of environment variable to set help browser.
#define   WXEXTHELP_ENVVAR_BROWSER   "WX_HELPBROWSER"
/// Is browser a netscape browser?
#define   WXEXTHELP_ENVVAR_BROWSERISNETSCAPE "WX_HELPBROWSER_NS"
/// Maximum line length in map file.
#define WXEXTHELP_BUFLEN 512
/// Character introducing comments/documentation field in map file.
#define WXEXTHELP_COMMENTCHAR   ';'

class wxExtHelpMapList;


/**
   This class implements help via an external browser.
   It requires the name of a directory containing the documentation
   and a file mapping numerical Section numbers to relative URLS.

   The map file contains two or three fields per line:
   numeric_id  relative_URL  [; comment/documentation]

   The numeric_id is the id used to look up the entry in
   DisplaySection()/DisplayBlock(). The relative_URL is a filename of
   an html file, relative to the help directory. The optional
   comment/documentation field (after a ';') is used for keyword
   searches, so some meaningful text here does not hurt.
   If the documentation itself contains a ';', only the part before
   that will be displayed in the listbox, but all of it used for search.

   Lines starting with ';' will be ignored.
*/

class wxExtHelpController : public wxHelpControllerBase
{      
DECLARE_CLASS(wxExtHelpController)
   public:
   wxExtHelpController(void);
   virtual ~wxExtHelpController(void);

   /** This must be called to tell the controller where to find the
       documentation.
       @param file - NOT a filename, but a directory name.
       @return true on success
   */
   virtual bool Initialize(const wxString& file, int WXUNUSED(server))
      { return Initialize(file); }
      
   /** This must be called to tell the controller where to find the
       documentation.
       @param file - NOT a filename, but a directory name.
       @return true on success
   */
   virtual bool Initialize(const wxString& file);
  
   /** If file is "", reloads file given in Initialize.
       @file Name of help directory.
       @return true on success
   */
   virtual bool LoadFile(const wxString& file = "");

   /** Display list of all help entries.
       @return true on success
   */
   virtual bool DisplayContents(void);
   /** Display help for id sectionNo.
       @return true on success
   */
   virtual bool DisplaySection(int sectionNo);
   /** Display help for id sectionNo -- identical with DisplaySection().
       @return true on success
   */
   virtual bool DisplayBlock(long blockNo);
   /** Search comment/documentation fields in map file and present a
       list to chose from.
       @key k string to search for, empty string will list all entries
       @return true on success
   */
   virtual bool KeywordSearch(const wxString& k);

   /// does nothing
   virtual bool Quit(void);
   /// does nothing
   virtual void OnQuit(void);

   /** Tell it which browser to use.
       The Netscape support will check whether Netscape is already
       running (by looking at the .netscape/lock file in the user's
       home directory) and tell it to load the page into the existing
       window. 
       @param browsername The command to call a browser/html viewer.
       @param isNetscape Set this to TRUE if the browser is some variant of Netscape.
   */
   void SetBrowser(wxString const & browsername = WXEXTHELP_DEFAULTBROWSER,
                   bool isNetscape = WXEXTHELP_DEFAULTBROWSER_IS_NETSCAPE);
 private:
   /// Filename of currently active map file.
   wxString         m_MapFile;
   /// How many entries do we have in the map file?
   int              m_NumOfEntries;
   /// A list containing all id,url,documentation triples.
   wxExtHelpMapList *m_MapList;
   /// How to call the html viewer.
   wxString         m_BrowserName;
   /// Is the viewer a variant of netscape?
   bool             m_BrowserIsNetscape;
   /// Call the browser using a relative URL.
   bool CallBrowser(wxString const &);
};

#endif  
#endif
