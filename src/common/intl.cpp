/////////////////////////////////////////////////////////////////////////////
// Name:        intl.cpp
// Purpose:     Internationalization and localisation for wxWindows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declaration
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "intl.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_INTL

// standard headers
#include  <locale.h>
#include  <ctype.h>

// wxWindows
#include "wx/defs.h"
#include "wx/string.h"
#include "wx/intl.h"
#include "wx/file.h"
#include "wx/log.h"
#include "wx/debug.h"
#include "wx/utils.h"

#include <stdlib.h>

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------

// this should *not* be wxChar, this type must have exactly 8 bits!
typedef unsigned char size_t8;

#ifdef __WXMSW__
    #if defined(__WIN16__)
        typedef unsigned long size_t32;
    #elif defined(__WIN32__)
        typedef unsigned int size_t32;
    #else
        // Win64 will have different type sizes
        #error "Please define a 32 bit type"
    #endif
#else // !Windows
    // SIZEOF_XXX are defined by configure
    #if defined(SIZEOF_INT) && (SIZEOF_INT == 4)
        typedef unsigned int size_t32;
    #elif defined(SIZEOF_LONG) && (SIZEOF_LONG == 4)
        typedef unsigned long size_t32;
    #else
        // assume sizeof(int) == 4 - what else can we do
        typedef unsigned int size_t32;

        // ... but at least check it during run time
        static class IntSizeChecker
        {
        public:
            IntSizeChecker()
            {
                // Asserting a sizeof directly causes some compilers to
                // issue a "using constant in a conditional expression" warning
                size_t                   intsize = sizeof(int);

                wxASSERT_MSG( intsize == 4,
                              "size_t32 is incorrectly defined!" );
            }
        } intsizechecker;
    #endif
#endif // Win/!Win

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// magic number identifying the .mo format file
const size_t32 MSGCATALOG_MAGIC    = 0x950412de;
const size_t32 MSGCATALOG_MAGIC_SW = 0xde120495;

// extension of ".mo" files
#define MSGCATALOG_EXTENSION  ".mo"

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// suppress further error messages about missing translations
// (if you don't have one catalog file, you wouldn't like to see the
//  error message for each string in it, so normally it's given only
//  once)
void wxSuppressTransErrors();

// restore the logging
void wxRestoreTransErrors();

// get the current state
bool wxIsLoggingTransErrors();

static wxLocale *wxSetLocale(wxLocale *pLocale);

// ----------------------------------------------------------------------------
// wxMsgCatalog corresponds to one disk-file message catalog.
//
// This is a "low-level" class and is used only by wxLocale (that's why
// it's designed to be stored in a linked list)
// ----------------------------------------------------------------------------

class wxMsgCatalog
{
public:
  // ctor & dtor
  wxMsgCatalog();
 ~wxMsgCatalog();

  // load the catalog from disk (szDirPrefix corresponds to language)
  bool Load(const wxChar *szDirPrefix, const wxChar *szName, bool bConvertEncoding = FALSE);
  bool IsLoaded() const { return m_pData != NULL; }

  // get name of the catalog
  const wxChar *GetName() const { return m_pszName; }

  // get the translated string: returns NULL if not found
  const char *GetString(const char *sz) const;

  // public variable pointing to the next element in a linked list (or NULL)
  wxMsgCatalog *m_pNext;

private:
  // this implementation is binary compatible with GNU gettext() version 0.10

  // an entry in the string table
  struct wxMsgTableEntry
  {
    size_t32   nLen;           // length of the string
    size_t32   ofsString;      // pointer to the string
  };

  // header of a .mo file
  struct wxMsgCatalogHeader
  {
    size_t32  magic,          // offset +00:  magic id
              revision,       //        +04:  revision
              numStrings;     //        +08:  number of strings in the file
    size_t32  ofsOrigTable,   //        +0C:  start of original string table
              ofsTransTable;  //        +10:  start of translated string table
    size_t32  nHashSize,      //        +14:  hash table size
              ofsHashTable;   //        +18:  offset of hash table start
  };

  // all data is stored here, NULL if no data loaded
  size_t8 *m_pData;

  // data description
  size_t32          m_numStrings,   // number of strings in this domain
                    m_nHashSize;    // number of entries in hash table
  size_t32         *m_pHashTable;   // pointer to hash table
  wxMsgTableEntry  *m_pOrigTable,   // pointer to original   strings
                   *m_pTransTable;  //            translated

  const char *StringAtOfs(wxMsgTableEntry *pTable, size_t32 index) const
    { return (const char *)(m_pData + Swap(pTable[index].ofsString)); }
    
  // convert encoding to platform native one, if neccessary
  void ConvertEncoding();

  // utility functions
    // calculate the hash value of given string
  static inline size_t32 GetHash(const char *sz);
    // big<->little endian
  inline size_t32 Swap(size_t32 ui) const;

  // internal state
  bool HasHashTable() const // true if hash table is present
    { return m_nHashSize > 2 && m_pHashTable != NULL; }

  bool          m_bSwapped;   // wrong endianness?

  wxChar       *m_pszName;    // name of the domain
};

// ----------------------------------------------------------------------------
// global variables
// ----------------------------------------------------------------------------

// the list of the directories to search for message catalog files
static wxArrayString s_searchPrefixes;

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMsgCatalog class
// ----------------------------------------------------------------------------

// calculate hash value using the so called hashpjw function by P.J. Weinberger
// [see Aho/Sethi/Ullman, COMPILERS: Principles, Techniques and Tools]
size_t32 wxMsgCatalog::GetHash(const char *sz)
{
  #define HASHWORDBITS 32     // the length of size_t32

  size_t32 hval = 0;
  size_t32 g;
  while ( *sz != '\0' ) {
    hval <<= 4;
    hval += (size_t32)*sz++;
    g = hval & ((size_t32)0xf << (HASHWORDBITS - 4));
    if ( g != 0 ) {
      hval ^= g >> (HASHWORDBITS - 8);
      hval ^= g;
    }
  }

  return hval;
}

// swap the 2 halves of 32 bit integer if needed
size_t32 wxMsgCatalog::Swap(size_t32 ui) const
{
  return m_bSwapped ? (ui << 24) | ((ui & 0xff00) << 8) |
                      ((ui >> 8) & 0xff00) | (ui >> 24)
                    : ui;
}

wxMsgCatalog::wxMsgCatalog()
{
  m_pData   = NULL;
  m_pszName = NULL;
}

wxMsgCatalog::~wxMsgCatalog()
{
  wxDELETEA(m_pData);
  wxDELETEA(m_pszName);
}

// small class to suppress the translation erros until exit from current scope
class NoTransErr
{
public:
    NoTransErr() { wxSuppressTransErrors(); }
   ~NoTransErr() { wxRestoreTransErrors();  }
};

// return all directories to search for given prefix
static wxString GetAllMsgCatalogSubdirs(const wxChar *prefix,
                                        const wxChar *lang)
{
    wxString searchPath;

    // search first in prefix/fr/LC_MESSAGES, then in prefix/fr and finally in
    // prefix (assuming the language is 'fr')
    searchPath << prefix << wxFILE_SEP_PATH << lang << wxFILE_SEP_PATH
                         << wxT("LC_MESSAGES") << wxPATH_SEP
               << prefix << wxFILE_SEP_PATH << lang << wxPATH_SEP
               << prefix << wxPATH_SEP;

    return searchPath;
}

// construct the search path for the given language
static wxString GetFullSearchPath(const wxChar *lang)
{
    wxString searchPath;

    // first take the entries explicitly added by the program
    size_t count = s_searchPrefixes.Count();
    for ( size_t n = 0; n < count; n++ )
    {
        searchPath << GetAllMsgCatalogSubdirs(s_searchPrefixes[n], lang)
                   << wxPATH_SEP;
    }

    // then take the current directory
    // FIXME it should be the directory of the executable
    searchPath << GetAllMsgCatalogSubdirs(wxT("."), lang) << wxPATH_SEP;

    // and finally add some standard ones
    searchPath
        << GetAllMsgCatalogSubdirs(wxT("/usr/share/locale"), lang) << wxPATH_SEP
        << GetAllMsgCatalogSubdirs(wxT("/usr/lib/locale"), lang) << wxPATH_SEP
        << GetAllMsgCatalogSubdirs(wxT("/usr/local/share/locale"), lang);

    return searchPath;
}

// open disk file and read in it's contents
bool wxMsgCatalog::Load(const wxChar *szDirPrefix, const wxChar *szName0, bool bConvertEncoding)
{
   /* We need to handle locales like  de_AT.iso-8859-1
      For this we first chop off the .CHARSET specifier and ignore it.
      FIXME: UNICODE SUPPORT: must use CHARSET specifier!
   */
   wxString szName = szName0;
   if(szName.Find(wxT('.')) != -1) // contains a dot
      szName = szName.Left(szName.Find(wxT('.')));

  // FIXME VZ: I forgot the exact meaning of LC_PATH - anyone to remind me?
  // KB: search path where to find the mo files, probably : delimited
#if 0
  const wxChar *pszLcPath = wxGetenv("LC_PATH");
  if ( pszLcPath != NULL )
      strPath += pszLcPath + wxString(szDirPrefix) + MSG_PATH;
#endif // 0

  wxString searchPath = GetFullSearchPath(szDirPrefix);
  const wxChar *sublocale = wxStrchr(szDirPrefix, wxT('_'));
  if ( sublocale )
  {
      // also add just base locale name: for things like "fr_BE" (belgium
      // french) we should use "fr" if no belgium specific message catalogs
      // exist
      searchPath << GetFullSearchPath(wxString(szDirPrefix).
                                      Left((size_t)(sublocale - szDirPrefix)))
                 << wxPATH_SEP;
  }

  wxString strFile = szName;
  strFile += MSGCATALOG_EXTENSION;

  // don't give translation errors here because the wxstd catalog might
  // not yet be loaded (and it's normal)
  //
  // (we're using an object because we have several return paths)

  NoTransErr noTransErr;
  wxLogVerbose(wxT("looking for catalog '%s' in path '%s'."),
               szName.c_str(), searchPath.c_str());

  wxString strFullName;
  if ( !wxFindFileInPath(&strFullName, searchPath, strFile) ) {
    wxLogWarning(_("catalog file for domain '%s' not found."), szName.c_str());
    return FALSE;
  }

  // open file
  wxLogVerbose(_("using catalog '%s' from '%s'."),
             szName.c_str(), strFullName.c_str());

  wxFile fileMsg(strFullName);
  if ( !fileMsg.IsOpened() )
    return FALSE;

  // get the file size
  off_t nSize = fileMsg.Length();
  if ( nSize == wxInvalidOffset )
    return FALSE;

  // read the whole file in memory
  m_pData = new size_t8[nSize];
  if ( fileMsg.Read(m_pData, nSize) != nSize ) {
    wxDELETEA(m_pData);
    return FALSE;
  }

  // examine header
  bool bValid = (size_t)nSize > sizeof(wxMsgCatalogHeader);

  wxMsgCatalogHeader *pHeader = (wxMsgCatalogHeader *)m_pData;
  if ( bValid ) {
    // we'll have to swap all the integers if it's true
    m_bSwapped = pHeader->magic == MSGCATALOG_MAGIC_SW;

    // check the magic number
    bValid = m_bSwapped || pHeader->magic == MSGCATALOG_MAGIC;
  }

  if ( !bValid ) {
    // it's either too short or has incorrect magic number
    wxLogWarning(_("'%s' is not a valid message catalog."), strFullName.c_str());

    wxDELETEA(m_pData);
    return FALSE;
  }

  // initialize
  m_numStrings  = Swap(pHeader->numStrings);
  m_pOrigTable  = (wxMsgTableEntry *)(m_pData +
                   Swap(pHeader->ofsOrigTable));
  m_pTransTable = (wxMsgTableEntry *)(m_pData +
                   Swap(pHeader->ofsTransTable));

  m_nHashSize   = Swap(pHeader->nHashSize);
  m_pHashTable  = (size_t32 *)(m_pData + Swap(pHeader->ofsHashTable));

  m_pszName = new wxChar[wxStrlen(szName) + 1];
  wxStrcpy(m_pszName, szName);

  if (bConvertEncoding) ConvertEncoding();

  // everything is fine
  return TRUE;
}

// search for a string
const char *wxMsgCatalog::GetString(const char *szOrig) const
{
  if ( szOrig == NULL )
    return NULL;

  if ( HasHashTable() ) {   // use hash table for lookup if possible
    size_t32 nHashVal = GetHash(szOrig);
    size_t32 nIndex   = nHashVal % m_nHashSize;

    size_t32 nIncr = 1 + (nHashVal % (m_nHashSize - 2));

#if defined(__VISAGECPP__)
// VA just can't stand while(1) or while(TRUE)
    bool bOs2var = TRUE;
    while(bOs2var) {
#else
    while (1) {
#endif
      size_t32 nStr = Swap(m_pHashTable[nIndex]);
      if ( nStr == 0 )
        return NULL;

      if ( strcmp(szOrig, StringAtOfs(m_pOrigTable, nStr - 1)) == 0 )
        return StringAtOfs(m_pTransTable, nStr - 1);

      if ( nIndex >= m_nHashSize - nIncr)
        nIndex -= m_nHashSize - nIncr;
      else
        nIndex += nIncr;
    }
  }
  else {                    // no hash table: use default binary search
    size_t32 bottom = 0,
           top    = m_numStrings,
           current;
    while ( bottom < top ) {
      current = (bottom + top) / 2;
      int res = strcmp(szOrig, StringAtOfs(m_pOrigTable, current));
      if ( res < 0 )
        top = current;
      else if ( res > 0 )
        bottom = current + 1;
      else    // found!
        return StringAtOfs(m_pTransTable, current);
    }
  }

  // not found
  return NULL;
}


#if wxUSE_GUI
#include "wx/fontmap.h"
#include "wx/encconv.h"
#endif

void wxMsgCatalog::ConvertEncoding()
{
#if wxUSE_GUI
    wxFontEncoding enc;

    // first, find encoding header:
    const char *hdr = GetString("$ENCODING");   
    if (hdr == NULL) return; // not supported by this catalog
    if ((enc = wxTheFontMapper -> CharsetToEncoding(hdr, FALSE)) == wxFONTENCODING_SYSTEM) return;

    wxFontEncodingArray a = wxEncodingConverter::GetPlatformEquivalents(enc);
    if (a[0] == enc) return; // no conversion needed, locale uses native encoding
    
    if (a.GetCount() == 0) return; // we don't know common equiv. under this platform
    
    wxEncodingConverter converter;
    
    converter.Init(enc, a[0]);
    for (unsigned i = 0; i < m_numStrings; i++)
        converter.Convert((char*)StringAtOfs(m_pTransTable, i));
#endif
}


// ----------------------------------------------------------------------------
// wxLocale
// ----------------------------------------------------------------------------

wxLocale::wxLocale()
{
  m_pszOldLocale = NULL;
  m_pMsgCat = NULL;
}

// NB: this function has (desired) side effect of changing current locale
bool wxLocale::Init(const wxChar *szName,
                    const wxChar *szShort,
                    const wxChar *szLocale,
                    bool        bLoadDefault,
                    bool        bConvertEncoding)
{
  m_strLocale = szName;
  m_strShort = szShort;
  m_bConvertEncoding = bConvertEncoding;

  // change current locale (default: same as long name)
  if ( szLocale == NULL )
  {
    // the argument to setlocale()
    szLocale = szShort;
  }
  m_pszOldLocale = wxSetlocale(LC_ALL, szLocale);
  if ( m_pszOldLocale == NULL )
    wxLogError(_("locale '%s' can not be set."), szLocale);

  // the short name will be used to look for catalog files as well,
  // so we need something here
  if ( m_strShort.IsEmpty() ) {
    // FIXME I don't know how these 2 letter abbreviations are formed,
    //       this wild guess is surely wrong
    m_strShort = tolower(szLocale[0]) + tolower(szLocale[1]);
  }

  // save the old locale to be able to restore it later
  m_pOldLocale = wxSetLocale(this);

  // load the default catalog with wxWindows standard messages
  m_pMsgCat = NULL;
  bool bOk = TRUE;
  if ( bLoadDefault )
    bOk = AddCatalog(wxT("wxstd"));

  return bOk;
}

void wxLocale::AddCatalogLookupPathPrefix(const wxString& prefix)
{
    if ( s_searchPrefixes.Index(prefix) == wxNOT_FOUND )
    {
        s_searchPrefixes.Add(prefix);
    }
    //else: already have it
}

// clean up
wxLocale::~wxLocale()
{
    // free memory
    wxMsgCatalog *pTmpCat;
    while ( m_pMsgCat != NULL ) {
        pTmpCat = m_pMsgCat;
        m_pMsgCat = m_pMsgCat->m_pNext;
        delete pTmpCat;
    }

    // restore old locale
    wxSetLocale(m_pOldLocale);
    wxSetlocale(LC_ALL, m_pszOldLocale);
}

// get the translation of given string in current locale
const wxMB2WXbuf wxLocale::GetString(const wxChar *szOrigString,
                                     const wxChar *szDomain) const
{
  if ( wxIsEmpty(szOrigString) )
      return szDomain;

  const char *pszTrans = NULL;
#if wxUSE_UNICODE
  const wxWX2MBbuf szOrgString = wxConvCurrent->cWX2MB(szOrigString);
#else // ANSI
  #define szOrgString szOrigString
#endif // Unicode/ANSI

  wxMsgCatalog *pMsgCat;
  if ( szDomain != NULL ) {
    pMsgCat = FindCatalog(szDomain);

    // does the catalog exist?
    if ( pMsgCat != NULL )
      pszTrans = pMsgCat->GetString(szOrgString);
  }
  else {
    // search in all domains
    for ( pMsgCat = m_pMsgCat; pMsgCat != NULL; pMsgCat = pMsgCat->m_pNext ) {
      pszTrans = pMsgCat->GetString(szOrgString);
      if ( pszTrans != NULL )   // take the first found
        break;
    }
  }

  if ( pszTrans == NULL ) {
    if ( wxIsLoggingTransErrors() ) {
      // suppress further error messages if we're not debugging: this avoids
      // flooding the user with messages about each and every missing string if,
      // for example, a whole catalog file is missing.

      // do it before calling LogWarning to prevent infinite recursion!
#ifdef __WXDEBUG__
      NoTransErr noTransErr;
#else // !debug
      wxSuppressTransErrors();
#endif // debug/!debug

      if ( szDomain != NULL )
      {
        wxLogWarning(_("string '%s' not found in domain '%s' for locale '%s'."),
                     szOrigString, szDomain, m_strLocale.c_str());
      }
      else
      {
        wxLogWarning(_("string '%s' not found in locale '%s'."),
                     szOrigString, m_strLocale.c_str());
      }
    }

    return (wxMB2WXbuf)(szOrigString);
  }
  else
  {
    return wxConvertMB2WX(pszTrans); // or preferably wxCSConv(charset).cMB2WX(pszTrans) or something,
                                     // a macro similar to wxConvertMB2WX could be written for that
  }

  #undef szOrgString
}

// find catalog by name in a linked list, return NULL if !found
wxMsgCatalog *wxLocale::FindCatalog(const wxChar *szDomain) const
{
// linear search in the linked list
  wxMsgCatalog *pMsgCat;
  for ( pMsgCat = m_pMsgCat; pMsgCat != NULL; pMsgCat = pMsgCat->m_pNext ) {
    if ( wxStricmp(pMsgCat->GetName(), szDomain) == 0 )
      return pMsgCat;
  }

  return NULL;
}

// check if the given catalog is loaded
bool wxLocale::IsLoaded(const wxChar *szDomain) const
{
  return FindCatalog(szDomain) != NULL;
}

// add a catalog to our linked list
bool wxLocale::AddCatalog(const wxChar *szDomain)
{
  wxMsgCatalog *pMsgCat = new wxMsgCatalog;

  if ( pMsgCat->Load(m_strShort, szDomain, m_bConvertEncoding) ) {
    // add it to the head of the list so that in GetString it will
    // be searched before the catalogs added earlier
    pMsgCat->m_pNext = m_pMsgCat;
    m_pMsgCat = pMsgCat;

    return TRUE;
  }
  else {
    // don't add it because it couldn't be loaded anyway
    delete pMsgCat;

    return FALSE;
  }
}

// ----------------------------------------------------------------------------
// global functions and variables
// ----------------------------------------------------------------------------

// translation errors logging
// --------------------------

static bool gs_bGiveTransErrors = TRUE;

void wxSuppressTransErrors()
{
  gs_bGiveTransErrors = FALSE;
}

void wxRestoreTransErrors()
{
  gs_bGiveTransErrors = TRUE;
}

bool wxIsLoggingTransErrors()
{
  return gs_bGiveTransErrors;
}

// retrieve/change current locale
// ------------------------------

// the current locale object
static wxLocale *g_pLocale = NULL;

wxLocale *wxGetLocale()
{
  return g_pLocale;
}

wxLocale *wxSetLocale(wxLocale *pLocale)
{
  wxLocale *pOld = g_pLocale;
  g_pLocale = pLocale;
  return pOld;
}

#endif // wxUSE_INTL

