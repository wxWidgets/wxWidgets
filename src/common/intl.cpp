/////////////////////////////////////////////////////////////////////////////
// Name:        intl.cpp
// Purpose:     Internationalization and localisation for wxWindows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
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

// wxWindows
#ifndef  WX_PRECOMP
  #include "wx/defs.h"
  #include "wx/string.h"
#endif //WX_PRECOMP

#include "wx/intl.h"
#include "wx/file.h"
#include "wx/log.h"
#include "wx/utils.h"

// standard headers
#include <locale.h>
#include <stdlib.h>

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// magic number identifying the .mo format file
const uint32 MSGCATALOG_MAGIC    = 0x950412de;
const uint32 MSGCATALOG_MAGIC_SW = 0xde120495;

// extension of ".mo" files
#define MSGCATALOG_EXTENSION  ".mo"

// ----------------------------------------------------------------------------
// global functions (private to this module)
// ----------------------------------------------------------------------------

// suppress further error messages about missing translations
// (if you don't have one catalog file, you wouldn't like to see the
//  error message for each string in it, so normally it's given only
//  once)
static void wxSuppressTransErrors();

// restore the logging
static void wxRestoreTransErrors();

// get the current state
static bool wxIsLoggingTransErrors();

// get the current locale object (@@ may be NULL!)
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
  bool Load(const char *szDirPrefix, const char *szName);
  bool IsLoaded() const { return m_pData != NULL; }

  // get name of the catalog
  const char *GetName() const { return m_pszName; }

  // get the translated string: returns NULL if not found
  const char *GetString(const char *sz) const;

  // public variable pointing to the next element in a linked list (or NULL)
  wxMsgCatalog *m_pNext;
  
private:
  // this implementation is binary compatible with GNU gettext() version 0.10

  // an entry in the string table
  struct wxMsgTableEntry
  {
    uint32   nLen;           // length of the string
    uint32   ofsString;      // pointer to the string
  };

  // header of a .mo file
  struct wxMsgCatalogHeader
  {
    uint32  magic,          // offset +00:  magic id
            revision,       //        +04:  revision
            numStrings;     //        +08:  number of strings in the file
    uint32  ofsOrigTable,   //        +0C:  start of original string table
            ofsTransTable;  //        +10:  start of translated string table
    uint32  nHashSize,      //        +14:  hash table size
            ofsHashTable;   //        +18:  offset of hash table start
  };                     
  
  // all data is stored here, NULL if no data loaded
  uint8 *m_pData;
  
  // data description
  uint32            m_numStrings,   // number of strings in this domain
                    m_nHashSize;    // number of entries in hash table
  uint32           *m_pHashTable;   // pointer to hash table
  wxMsgTableEntry  *m_pOrigTable,   // pointer to original   strings
                   *m_pTransTable;  //            translated

  const char *StringAtOfs(wxMsgTableEntry *pTable, uint32 index) const
    { return (const char *)(m_pData + Swap(pTable[index].ofsString)); }

  // utility functions
    // calculate the hash value of given string
  static inline uint32 GetHash(const char *sz);
    // big<->little endian
  inline uint32 Swap(uint32 ui) const;

  // internal state
  bool HasHashTable() const // true if hash table is present
    { return m_nHashSize > 2 && m_pHashTable != NULL; }

  bool          m_bSwapped;   // wrong endianness?

  char         *m_pszName;    // name of the domain
};

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxMsgCatalog class
// ----------------------------------------------------------------------------

// calculate hash value using the so called hashpjw function by P.J. Weinberger
// [see Aho/Sethi/Ullman, COMPILERS: Principles, Techniques and Tools]
uint32 wxMsgCatalog::GetHash(const char *sz)
{
  #define HASHWORDBITS 32     // the length of uint32

  uint32 hval = 0;
  uint32 g;
  while ( *sz != '\0' ) {
    hval <<= 4;
    hval += (uint32)*sz++;
    g = hval & ((uint32)0xf << (HASHWORDBITS - 4));
    if ( g != 0 ) {
      hval ^= g >> (HASHWORDBITS - 8);
      hval ^= g;
    }
  }

  return hval;
}

// swap the 2 halves of 32 bit integer if needed
uint32 wxMsgCatalog::Swap(uint32 ui) const
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
  DELETEA(m_pData); 
  DELETEA(m_pszName); 
}

// a helper class which suppresses all translation error messages
// from the moment of it's creation until it's destruction
class NoTransErr
{
  public:
    NoTransErr() { wxSuppressTransErrors(); }
   ~NoTransErr() { wxRestoreTransErrors();  }
};
    
// open disk file and read in it's contents
bool wxMsgCatalog::Load(const char *szDirPrefix, const char *szName)
{
  // search order (assume language 'lang') is
  // 1) $LC_PATH/lang/LC_MESSAGES          (if LC_PATH set)
  // 2) ./lang/LC_MESSAGES
  // 3) ./lang
  // 4) . (Added by JACS)
  //
  // under UNIX we search also in:
  // 5) /usr/share/locale/lang/LC_MESSAGES (Linux)
  // 6) /usr/lib/locale/lang/LC_MESSAGES   (Solaris)
  #define MSG_PATH FILE_SEP_PATH + "LC_MESSAGES" PATH_SEP
          
  wxString strPath("");
  const char *pszLcPath = getenv("LC_PATH");
  if ( pszLcPath != NULL )
      strPath += pszLcPath + wxString(szDirPrefix) + MSG_PATH;        // (1)
  
  // NB: '<<' is unneeded between too literal strings: 
  //     they are concatenated at compile time
  strPath << "./" << wxString(szDirPrefix) + MSG_PATH                 // (2)
          << "./" << szDirPrefix << FILE_SEP_PATH << PATH_SEP         // (3)
		      << "." << PATH_SEP                                          // (4)
  #ifdef  __UNIX__
             "/usr/share/locale/" << szDirPrefix << MSG_PATH          // (5)
             "/usr/lib/locale/"  << szDirPrefix << MSG_PATH           // (6)
  #endif  //UNIX
          ;
  
  wxString strFile = szName;
  strFile += MSGCATALOG_EXTENSION;

  // don't give translation errors here because the wxstd catalog might
  // not yet be loaded (and it's normal)
  //
  // (we're using an object because we have several return paths)
  NoTransErr noTransErr;

  wxLogVerbose(_("looking for catalog '%s' in path '%s'..."),
               szName, strPath.c_str());

  wxString strFullName;
  if ( !wxFindFileInPath(&strFullName, strPath, strFile) ) {
    wxLogWarning(_("catalog file for domain '%s' not found."), szName);
    return FALSE;
  }

  // open file
  wxLogVerbose(_("catalog '%s' found in '%s'."), szName, strFullName.c_str());
  
  // declare these vars here because we're using goto further down
  bool bValid;
  off_t nSize;

  wxFile fileMsg(strFullName);
  if ( !fileMsg.IsOpened() )
    goto error;

  // get the file size
  nSize = fileMsg.Length();
  if ( nSize == ofsInvalid )
    goto error;

  // read the whole file in memory
  m_pData = new uint8[nSize];
  if ( fileMsg.Read(m_pData, nSize) != nSize ) {
    DELETEA(m_pData);
    m_pData = NULL;
    goto error;
  }
    
  // examine header
  bValid = (size_t)nSize > sizeof(wxMsgCatalogHeader);
  
  wxMsgCatalogHeader *pHeader;
  if ( bValid ) {
    pHeader = (wxMsgCatalogHeader *)m_pData;

    // we'll have to swap all the integers if it's true
    m_bSwapped = pHeader->magic == MSGCATALOG_MAGIC_SW;

    // check the magic number
    bValid = m_bSwapped || pHeader->magic == MSGCATALOG_MAGIC;
  }
  
  if ( !bValid ) {
    // it's either too short or has incorrect magic number
    wxLogWarning(_("'%s' is not a valid message catalog."),
                 strFullName.c_str());
    
    DELETEA(m_pData);
    m_pData = NULL;
    return FALSE;
  }
      
  // initialize
  m_numStrings  = Swap(pHeader->numStrings);
  m_pOrigTable  = (wxMsgTableEntry *)(m_pData + Swap(pHeader->ofsOrigTable));
  m_pTransTable = (wxMsgTableEntry *)(m_pData + Swap(pHeader->ofsTransTable));

  m_nHashSize   = Swap(pHeader->nHashSize);
  m_pHashTable  = (uint32 *)(m_pData + Swap(pHeader->ofsHashTable));

  m_pszName = new char[strlen(szName) + 1];
  strcpy(m_pszName, szName);

  // everything is fine
  return TRUE;

error:
  wxLogError(_("error opening message catalog '%s', not loaded."),
             strFullName.c_str());
  return FALSE;
}

// search for a string
const char *wxMsgCatalog::GetString(const char *szOrig) const
{
  if ( szOrig == NULL )
    return NULL;

  if ( HasHashTable() ) {   // use hash table for lookup if possible
    uint32 nHashVal = GetHash(szOrig); 
    uint32 nIndex   = nHashVal % m_nHashSize;

    uint32 nIncr = 1 + (nHashVal % (m_nHashSize - 2));
    
    while ( TRUE ) {
      uint32 nStr = Swap(m_pHashTable[nIndex]);
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
    uint32 bottom = 0,
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

// ----------------------------------------------------------------------------
// wxLocale
// ----------------------------------------------------------------------------

// NB: ctor has (desired) side effect of changing current locale
wxLocale::wxLocale(const char *szName, 
                   const char *szShort, 
                   const char *szLocale,
                   bool        bLoadDefault)
        : m_strLocale(szName), m_strShort(szShort)
{
  // change current locale (default: same as long name)
  if ( szLocale == NULL )
    szLocale = szName;
  m_pszOldLocale = setlocale(LC_ALL, szLocale);
  if ( m_pszOldLocale == NULL )
    wxLogError(_("locale '%s' can not be set."), szLocale);

  // the short name will be used to look for catalog files as well,
  // so we need something here
  if ( m_strShort.IsEmpty() ) {
    // @@@@ I don't know how these 2 letter abbreviations are formed,
    //      this wild guess is almost surely wrong
    m_strShort = wxToLower(szLocale[0]) + wxToLower(szLocale[1]);
  }
  
  // save the old locale to be able to restore it later
	m_pOldLocale = wxSetLocale(this);
  
  // load the default catalog with wxWindows standard messages
  m_pMsgCat = NULL;
  if ( bLoadDefault )
    AddCatalog("wxstd");
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
  setlocale(LC_ALL, m_pszOldLocale);
}

// get the translation of given string in current locale
const char *wxLocale::GetString(const char *szOrigString, 
                                const char *szDomain) const
{
  wxASSERT( szOrigString != NULL ); // would be pretty silly

  const char *pszTrans = NULL;

  wxMsgCatalog *pMsgCat;
  if ( szDomain != NULL ) {
    pMsgCat = FindCatalog(szDomain);
    
    // does the catalog exist?
    if ( pMsgCat != NULL )
      pszTrans = pMsgCat->GetString(szOrigString);
  }
  else {
    // search in all domains
    for ( pMsgCat = m_pMsgCat; pMsgCat != NULL; pMsgCat = pMsgCat->m_pNext ) {
      pszTrans = pMsgCat->GetString(szOrigString);
      if ( pszTrans != NULL )   // take the first found
        break;
    }
  }

  if ( pszTrans == NULL ) {
    if ( wxIsLoggingTransErrors() ) {
      // suppress further error messages
      // (do it before LogWarning to prevent infinite recursion!)
      wxSuppressTransErrors();
      
      if ( szDomain != NULL )
        wxLogWarning(_("string '%s' not found in domain '%s'"
                       " for locale '%s'."),
                     szOrigString, szDomain, m_strLocale.c_str());
      else
        wxLogWarning(_("string '%s' not found in locale '%s'."),
                     szOrigString, m_strLocale.c_str());
    }

    return szOrigString;
  }
  else
    return pszTrans;
}

// find catalog by name in a linked list, return NULL if !found
wxMsgCatalog *wxLocale::FindCatalog(const char *szDomain) const
{
// linear search in the linked list
  wxMsgCatalog *pMsgCat;
  for ( pMsgCat = m_pMsgCat; pMsgCat != NULL; pMsgCat = pMsgCat->m_pNext ) {
    if ( Stricmp(pMsgCat->GetName(), szDomain) == 0 )
      return pMsgCat;
  }
  
  return NULL;
}

// check if the given catalog is loaded
bool wxLocale::IsLoaded(const char *szDomain) const
{
  return FindCatalog(szDomain) != NULL;
}

// add a catalog to our linked list
bool wxLocale::AddCatalog(const char *szDomain)
{
  wxMsgCatalog *pMsgCat = new wxMsgCatalog;
  
  if ( pMsgCat->Load(m_strShort, szDomain) ) {
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
wxLocale *g_pLocale = NULL;

wxLocale *wxSetLocale(wxLocale *pLocale)
{
	wxLocale *pOld = g_pLocale;
	g_pLocale = pLocale;
	return pOld;
}
