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
#include "wx/tokenzr.h"
#include "wx/intl.h"
#include "wx/file.h"
#include "wx/log.h"
#include "wx/debug.h"
#include "wx/utils.h"
#include "wx/dynarray.h"
#ifdef __WIN32__
#include "wx/msw/private.h"
#endif


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
                size_t intsize = sizeof(int);

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
#define MSGCATALOG_EXTENSION  _T(".mo")

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

#ifdef __WXDEBUG__

// small class to suppress the translation erros until exit from current scope
class NoTransErr
{
public:
    NoTransErr() { ms_suppressCount++; }
   ~NoTransErr() { ms_suppressCount--;  }

   static bool Suppress() { return ms_suppressCount > 0; }

private:
   static size_t ms_suppressCount;
};

size_t NoTransErr::ms_suppressCount = 0;

#else // !Debug

class NoTransErr
{
public:
    NoTransErr() { }
   ~NoTransErr() { }
};

#endif // Debug/!Debug

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

    // LC_PATH is a standard env var containing the search path for the .mo
    // files
    const wxChar *pszLcPath = wxGetenv(wxT("LC_PATH"));
    if ( pszLcPath != NULL )
        searchPath << GetAllMsgCatalogSubdirs(pszLcPath, lang);

    // then take the current directory
    // FIXME it should be the directory of the executable
    searchPath << GetAllMsgCatalogSubdirs(wxT("."), lang);

    // and finally add some standard ones
    searchPath
        << GetAllMsgCatalogSubdirs(wxT("/usr/share/locale"), lang)
        << GetAllMsgCatalogSubdirs(wxT("/usr/lib/locale"), lang)
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
  wxLogVerbose(_("looking for catalog '%s' in path '%s'."),
               szName.c_str(), searchPath.c_str());

  wxString strFullName;
  if ( !wxFindFileInPath(&strFullName, searchPath, strFile) ) {
    wxLogVerbose(_("catalog file for domain '%s' not found."), szName.c_str());
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

  if (bConvertEncoding)
      ConvertEncoding();

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
    const char *hdr = StringAtOfs(m_pOrigTable, 0);
    if (hdr == NULL) return; // not supported by this catalog, does not have non-fuzzy header
    if (hdr[0] != 0) return; // ditto

    /* we support catalogs with header (msgid "") that is _not_ marked as "#, fuzzy" (otherwise
       the string would not be included into compiled catalog) */
    wxString header(StringAtOfs(m_pTransTable, 0));
    wxString charset;
    int pos = header.Find(wxT("Content-Type: text/plain; charset="));
    if (pos == wxNOT_FOUND)
        return; // incorrectly filled Content-Type header
    size_t n = pos + 34; /*strlen("Content-Type: text/plain; charset=")*/
    while (header[n] != wxT('\n'))
        charset << header[n++];

    enc = wxTheFontMapper->CharsetToEncoding(charset, FALSE);
    if ( enc == wxFONTENCODING_SYSTEM )
        return; // unknown encoding

    wxFontEncodingArray a = wxEncodingConverter::GetPlatformEquivalents(enc);
    if (a[0] == enc)
        return; // no conversion needed, locale uses native encoding

    if (a.GetCount() == 0)
        return; // we don't know common equiv. under this platform

    wxEncodingConverter converter;

    converter.Init(enc, a[0]);
    for (size_t i = 0; i < m_numStrings; i++)
        converter.Convert((char*)StringAtOfs(m_pTransTable, i));
#endif
}


// ----------------------------------------------------------------------------
// wxLocale
// ----------------------------------------------------------------------------

#include "wx/arrimpl.cpp"
WX_DECLARE_EXPORTED_OBJARRAY(wxLanguageInfo, wxLanguageInfoArray);
WX_DEFINE_OBJARRAY(wxLanguageInfoArray);



wxLocale::wxLocale()
{
  m_pszOldLocale = NULL;
  m_pMsgCat = NULL;
  m_languagesDB = NULL;
  m_language = wxLANGUAGE_UNKNOWN;
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
  m_language = wxLANGUAGE_UNKNOWN;

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



bool wxLocale::Init(int language, int flags)
{
    wxLanguageInfo *info = NULL;
    int lang = language;

    if (m_languagesDB == NULL)
    {
        m_languagesDB = new wxLanguageInfoArray;
        InitLanguagesDB();
    }

    if (lang == wxLANGUAGE_DEFAULT) lang = GetSystemLanguage();  
    if (lang != wxLANGUAGE_UNKNOWN)
    {
        for (size_t i = 0; i < m_languagesDB->GetCount(); i++)
        {
            if (m_languagesDB->Item(i).Language == lang)
            {
                info = &m_languagesDB->Item(i);
                break;
            }
        }
    }

    // We failed to detect system language, so we will use English:
    if (lang == wxLANGUAGE_UNKNOWN)
    {
       return FALSE;
    }
    // Unknown language:
    if (info == NULL)
    {
        wxLogError(wxT("Unknown language %i."), lang);
        return FALSE;
    }

    wxString name = info->Description;
    wxString canonical = info->CanonicalName;
    wxString locale;
    wxChar *retloc;
    
    // Set the locale:
#ifdef __UNIX__
    if (language == wxLANGUAGE_DEFAULT) locale = wxEmptyString;
    else locale = info->CanonicalName;

    retloc = wxSetlocale(LC_ALL, locale);

    if (retloc == NULL)
    {
        // Some C libraries don't like xx_YY form and require xx only
        retloc = wxSetlocale(LC_ALL, locale.Mid(0,2));
    }
    if (retloc == NULL)
    {
        // Some C libraries (namely glibc) still use old ISO 639,
        // so will translate the abbrev for them
        wxString mid = locale.Mid(0,2);
        if (mid == wxT("he")) locale = wxT("iw") + locale.Mid(3);
        else if (mid == wxT("id")) locale = wxT("in") + locale.Mid(3);
        else if (mid == wxT("yi")) locale = wxT("ji") + locale.Mid(3);
        retloc = wxSetlocale(LC_ALL, locale);
    }
    if (retloc == NULL)
    {
        // (This time, we changed locale in previous if-branch, so try again.)
        // Some C libraries don't like xx_YY form and require xx only
        retloc = wxSetlocale(LC_ALL, locale.Mid(0,2));
    }
    if (retloc == NULL)
    {
        wxLogError(wxT("Cannot set locale to '%s'."), locale.c_str());
        return FALSE;
    }

#elif defined(__WIN32__)
    if (language != wxLANGUAGE_DEFAULT)
    {
        wxUint32 lcid = MAKELCID(MAKELANGID(info->WinLang, info->WinSublang), 
                                 SORT_DEFAULT);
        if (SetThreadLocale(lcid))
            retloc = wxSetlocale(LC_ALL, wxEmptyString);
        else
        {
            // Windows9X doesn't support SetThreadLocale, so we must
            // translate LCID to CRT's setlocale string ourselves
            locale.Empty();
            if (GetLastError() == ERROR_CALL_NOT_IMPLEMENTED)
            {
                wxChar buffer[256];
                buffer[0] = wxT('\0');
                GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, buffer, 256);
                locale << buffer;
                buffer[0] = wxT('\0');
                GetLocaleInfo(lcid, LOCALE_SENGCOUNTRY, buffer, 256);
            }
            if (locale.IsEmpty())
            {
                wxLogLastError(wxT("SetThreadLocale"));
                wxLogError(wxT("Cannot set locale to language %s."), name.c_str());
                return FALSE;
            }
            else
                retloc = wxSetlocale(LC_ALL, locale);
        }
    }
    else
        retloc = wxSetlocale(LC_ALL, wxEmptyString);

    if (retloc == NULL)
    {
        wxLogError(wxT("Cannot set locale to language %s."), name.c_str());
        return FALSE;
    }
    
#else
    return FALSE;
#endif
    
    return Init(name, canonical, wxString(retloc),
                (flags & wxLOCALE_LOAD_DEFAULT) != 0,
                (flags & wxLOCALE_CONV_ENCODING) != 0);
}



void wxLocale::AddCatalogLookupPathPrefix(const wxString& prefix)
{
    if ( s_searchPrefixes.Index(prefix) == wxNOT_FOUND )
    {
        s_searchPrefixes.Add(prefix);
    }
    //else: already have it
}


int wxLocale::GetSystemLanguage()
{
    int wxlang = wxLANGUAGE_UNKNOWN;
    size_t i;

    wxASSERT_MSG(m_languagesDB != NULL, "Languages DB not initialized, call wxLocale::Init!");
    
#if defined(__UNIX__)
    wxString lang;
    if (!wxGetEnv(wxT("LC_ALL"), &lang) && 
        !wxGetEnv(wxT("LC_MESSAGES"), &lang) &&
        !wxGetEnv(wxT("LANG"), &lang))         
        return wxLANGUAGE_UNKNOWN;

    bool is_abbrev = lang.Len() == 2 || 
                     (lang.Len() == 5 && lang[2] == wxT('_'));
        
    // 0. Make sure the abbrev is according to latest ISO 639
    //    (this is neccessary because glibc uses iw and in instead
    //    of he and id respectively).
    if (is_abbrev)
    {
       wxString mid = lang.Mid(0,2);
       if (mid == wxT("iw")) lang = wxT("he") + lang.Mid(3);
       else if (mid == wxT("in")) lang = wxT("id") + lang.Mid(3);
       else if (mid == wxT("ji")) lang = wxT("yi") + lang.Mid(3);
    }

    // 1. Try to find the lang as is:
    if (is_abbrev)
    {
        for (i = 0; i < m_languagesDB->GetCount(); i++)
        {
            if (m_languagesDB->Item(i).CanonicalName == lang)
            {
                wxlang = m_languagesDB->Item(i).Language;
                break;
            }
        }
    }

    // 2. If lang is of the form xx_YY, try to find xx:
    if (wxlang == wxLANGUAGE_UNKNOWN && is_abbrev && lang.Len() == 5)
    {
        wxString lang2 = lang.Mid(0,2);
        for (i = 0; i < m_languagesDB->GetCount(); i++)
        {
            if (m_languagesDB->Item(i).CanonicalName == lang2)
            {
                wxlang = m_languagesDB->Item(i).Language;
                break;
            }
        }
    }

    // 3. If lang is of the form xx, try to find any xx_YY record:
    if (wxlang == wxLANGUAGE_UNKNOWN && is_abbrev && lang.Len() == 2)
    {
        for (i = 0; i < m_languagesDB->GetCount(); i++)
        {
            if (m_languagesDB->Item(i).CanonicalName.Mid(0,2) == lang)
            {
                wxlang = m_languagesDB->Item(i).Language;
                break;
            }
        }
    }

    // 4. If everything failed, try to find the name in verbose description
    //    (SuSE is known to use LANG="german"):
    if (wxlang == wxLANGUAGE_UNKNOWN && !is_abbrev)
    {
        for (i = 0; i < m_languagesDB->GetCount(); i++)
        {
            if (m_languagesDB->Item(i).Description.CmpNoCase(lang) == 0)
            {
                wxlang = m_languagesDB->Item(i).Language;
                break;
            }
        }
    }

#elif defined(__WIN32__)
    LCID lcid = GetUserDefaultLCID();
    if (lcid == 0) return wxLANGUAGE_UNKNOWN;
    wxUint32 lang = PRIMARYLANGID(LANGIDFROMLCID(lcid));
    wxUint32 sublang = SUBLANGID(LANGIDFROMLCID(lcid));

    for (i = 0; i < m_languagesDB->GetCount(); i++)
    {
        if (m_languagesDB->Item(i).WinLang == lang &&
            m_languagesDB->Item(i).WinSublang == sublang)
        {
            wxlang = m_languagesDB->Item(i).Language;
            break;
        }
    }
#endif

    return wxlang;
}



void wxLocale::AddLanguage(const wxLanguageInfo& info)
{
    wxASSERT_MSG(m_languagesDB != NULL, "Languages DB not initialized, call wxLocale::Init!");
    m_languagesDB->Add(info);
}



wxString wxLocale::GetSysName() const
{
    return wxSetlocale(LC_ALL, NULL);
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

    delete m_languagesDB;

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
#ifdef __WXDEBUG__
    if ( !NoTransErr::Suppress() ) {
      NoTransErr noTransErr;

      if ( szDomain != NULL )
      {
        wxLogDebug(_T("string '%s' not found in domain '%s' for locale '%s'."),
                     szOrigString, szDomain, m_strLocale.c_str());
      }
      else
      {
        wxLogDebug(_T("string '%s' not found in locale '%s'."),
                   szOrigString, m_strLocale.c_str());
      }
    }
#endif // __WXDEBUG__

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




// ----------------------------------------------------------------------------
// default languages table & initialization
// ----------------------------------------------------------------------------

// This table is generated by misc/languages/genlang.py
// When making changes, please put them into misc/languages/langtabl.txt

#ifdef __WXMSW__
#define SETWINLANG(info,lang,sublang) \
    info.WinLang = lang, info.WinSublang = sublang;
#else
#define SETWINLANG(info,lang,sublang)
#endif

#define LNG(wxlang, canonical, winlang, winsublang, desc) \
    info.Language = wxlang;                               \
    info.CanonicalName = wxT(canonical);                  \
    info.Description = desc;                              \
    SETWINLANG(info, winlang, winsublang)                 \
    AddLanguage(info);

void wxLocale::InitLanguagesDB()
{
   wxLanguageInfo info;
   wxStringTokenizer tkn;
   
   LNG(wxLANGUAGE_ABKHAZIAN,                  "ab"   , 0              , 0                                 , "Abkhazian")
   LNG(wxLANGUAGE_AFAR,                       "aa"   , 0              , 0                                 , "Afar")
   LNG(wxLANGUAGE_AFRIKAANS,                  "af_ZA", LANG_AFRIKAANS , SUBLANG_DEFAULT                   , "Afrikaans")
   LNG(wxLANGUAGE_ALBANIAN,                   "sq_AL", LANG_ALBANIAN  , SUBLANG_DEFAULT                   , "Albanian")
   LNG(wxLANGUAGE_AMHARIC,                    "am"   , 0              , 0                                 , "Amharic")
   LNG(wxLANGUAGE_ARABIC,                     "ar"   , LANG_ARABIC    , SUBLANG_DEFAULT                   , "Arabic")
   LNG(wxLANGUAGE_ARABIC_ALGERIA,             "ar_DZ", LANG_ARABIC    , SUBLANG_ARABIC_ALGERIA            , "Arabic (Algeria)")
   LNG(wxLANGUAGE_ARABIC_BAHRAIN,             "ar_BH", LANG_ARABIC    , SUBLANG_ARABIC_BAHRAIN            , "Arabic (Bahrain)")
   LNG(wxLANGUAGE_ARABIC_EGYPT,               "ar_EG", LANG_ARABIC    , SUBLANG_ARABIC_EGYPT              , "Arabic (Egypt)")
   LNG(wxLANGUAGE_ARABIC_IRAQ,                "ar_IQ", LANG_ARABIC    , SUBLANG_ARABIC_IRAQ               , "Arabic (Iraq)")
   LNG(wxLANGUAGE_ARABIC_JORDAN,              "ar_JO", LANG_ARABIC    , SUBLANG_ARABIC_JORDAN             , "Arabic (Jordan)")
   LNG(wxLANGUAGE_ARABIC_KUWAIT,              "ar_KW", LANG_ARABIC    , SUBLANG_ARABIC_KUWAIT             , "Arabic (Kuwait)")
   LNG(wxLANGUAGE_ARABIC_LEBANON,             "ar_LB", LANG_ARABIC    , SUBLANG_ARABIC_LEBANON            , "Arabic (Lebanon)")
   LNG(wxLANGUAGE_ARABIC_LIBYA,               "ar_LY", LANG_ARABIC    , SUBLANG_ARABIC_LIBYA              , "Arabic (Libya)")
   LNG(wxLANGUAGE_ARABIC_MOROCCO,             "ar_MA", LANG_ARABIC    , SUBLANG_ARABIC_MOROCCO            , "Arabic (Morocco)")
   LNG(wxLANGUAGE_ARABIC_OMAN,                "ar_OM", LANG_ARABIC    , SUBLANG_ARABIC_OMAN               , "Arabic (Oman)")
   LNG(wxLANGUAGE_ARABIC_QATAR,               "ar_QA", LANG_ARABIC    , SUBLANG_ARABIC_QATAR              , "Arabic (Qatar)")
   LNG(wxLANGUAGE_ARABIC_SAUDI_ARABIA,        "ar_SA", LANG_ARABIC    , SUBLANG_ARABIC_SAUDI_ARABIA       , "Arabic (Saudi Arabia)")
   LNG(wxLANGUAGE_ARABIC_SUDAN,               "ar_SD", 0              , 0                                 , "Arabic (Sudan)")
   LNG(wxLANGUAGE_ARABIC_SYRIA,               "ar_SY", LANG_ARABIC    , SUBLANG_ARABIC_SYRIA              , "Arabic (Syria)")
   LNG(wxLANGUAGE_ARABIC_TUNISIA,             "ar_TN", LANG_ARABIC    , SUBLANG_ARABIC_TUNISIA            , "Arabic (Tunisia)")
   LNG(wxLANGUAGE_ARABIC_UAE,                 "ar_AE", LANG_ARABIC    , SUBLANG_ARABIC_UAE                , "Arabic (Uae)")
   LNG(wxLANGUAGE_ARABIC_YEMEN,               "ar_YE", LANG_ARABIC    , SUBLANG_ARABIC_YEMEN              , "Arabic (Yemen)")
   LNG(wxLANGUAGE_ARMENIAN,                   "hy"   , LANG_ARMENIAN  , SUBLANG_DEFAULT                   , "Armenian")
   LNG(wxLANGUAGE_ASSAMESE,                   "as"   , LANG_ASSAMESE  , SUBLANG_DEFAULT                   , "Assamese")
   LNG(wxLANGUAGE_AYMARA,                     "ay"   , 0              , 0                                 , "Aymara")
   LNG(wxLANGUAGE_AZERI,                      "az"   , LANG_AZERI     , SUBLANG_DEFAULT                   , "Azeri")
   LNG(wxLANGUAGE_AZERI_CYRILLIC,             "az"   , LANG_AZERI     , SUBLANG_AZERI_CYRILLIC            , "Azeri (Cyrillic)")
   LNG(wxLANGUAGE_AZERI_LATIN,                "az"   , LANG_AZERI     , SUBLANG_AZERI_LATIN               , "Azeri (Latin)")
   LNG(wxLANGUAGE_BASHKIR,                    "ba"   , 0              , 0                                 , "Bashkir")
   LNG(wxLANGUAGE_BASQUE,                     "eu_ES", LANG_BASQUE    , SUBLANG_DEFAULT                   , "Basque")
   LNG(wxLANGUAGE_BELARUSIAN,                 "be_BY", LANG_BELARUSIAN, SUBLANG_DEFAULT                   , "Belarusian")
   LNG(wxLANGUAGE_BENGALI,                    "bn"   , LANG_BENGALI   , SUBLANG_DEFAULT                   , "Bengali")
   LNG(wxLANGUAGE_BHUTANI,                    "dz"   , 0              , 0                                 , "Bhutani")
   LNG(wxLANGUAGE_BIHARI,                     "bh"   , 0              , 0                                 , "Bihari")
   LNG(wxLANGUAGE_BISLAMA,                    "bi"   , 0              , 0                                 , "Bislama")
   LNG(wxLANGUAGE_BRETON,                     "br"   , 0              , 0                                 , "Breton")
   LNG(wxLANGUAGE_BULGARIAN,                  "bg_BG", LANG_BULGARIAN , SUBLANG_DEFAULT                   , "Bulgarian")
   LNG(wxLANGUAGE_BURMESE,                    "my"   , 0              , 0                                 , "Burmese")
   LNG(wxLANGUAGE_CAMBODIAN,                  "km"   , 0              , 0                                 , "Cambodian")
   LNG(wxLANGUAGE_CATALAN,                    "ca_ES", LANG_CATALAN   , SUBLANG_DEFAULT                   , "Catalan")
   LNG(wxLANGUAGE_CHINESE,                    "zh_CN", LANG_CHINESE   , SUBLANG_DEFAULT                   , "Chinese")
   LNG(wxLANGUAGE_CHINESE_SIMPLIFIED,         "zh_CN", LANG_CHINESE   , SUBLANG_CHINESE_SIMPLIFIED        , "Chinese (Simplified)")
   LNG(wxLANGUAGE_CHINESE_TRADITIONAL,        "zh_CN", LANG_CHINESE   , SUBLANG_CHINESE_TRADITIONAL       , "Chinese (Traditional)")
   LNG(wxLANGUAGE_CHINESE_HONGKONG,           "zh_HK", LANG_CHINESE   , SUBLANG_CHINESE_HONGKONG          , "Chinese (Hongkong)")
   LNG(wxLANGUAGE_CHINESE_MACAU,              "zh_MO", LANG_CHINESE   , SUBLANG_CHINESE_MACAU             , "Chinese (Macau)")
   LNG(wxLANGUAGE_CHINESE_SINGAPORE,          "zh_SG", LANG_CHINESE   , SUBLANG_CHINESE_SINGAPORE         , "Chinese (Singapore)")
   LNG(wxLANGUAGE_CHINESE_TAIWAN,             "zh_TW", 0              , 0                                 , "Chinese (Taiwan)")
   LNG(wxLANGUAGE_CORSICAN,                   "co"   , 0              , 0                                 , "Corsican")
   LNG(wxLANGUAGE_CROATIAN,                   "hr_HR", LANG_CROATIAN  , SUBLANG_DEFAULT                   , "Croatian")
   LNG(wxLANGUAGE_CZECH,                      "cs_CZ", LANG_CZECH     , SUBLANG_DEFAULT                   , "Czech")
   LNG(wxLANGUAGE_DANISH,                     "da_DK", LANG_DANISH    , SUBLANG_DEFAULT                   , "Danish")
   LNG(wxLANGUAGE_DUTCH,                      "nl_NL", LANG_DUTCH     , SUBLANG_DUTCH                     , "Dutch")
   LNG(wxLANGUAGE_DUTCH_BELGIAN,              "nl_BE", LANG_DUTCH     , SUBLANG_DUTCH_BELGIAN             , "Dutch (Belgian)")
   LNG(wxLANGUAGE_ENGLISH,                    "en_GB", LANG_ENGLISH   , SUBLANG_ENGLISH_UK                , "English")
   LNG(wxLANGUAGE_ENGLISH_UK,                 "en_GB", LANG_ENGLISH   , SUBLANG_ENGLISH_UK                , "English (U.K.)")
   LNG(wxLANGUAGE_ENGLISH_US,                 "en_US", LANG_ENGLISH   , SUBLANG_ENGLISH_US                , "English (U.S.)")
   LNG(wxLANGUAGE_ENGLISH_AUSTRALIA,          "en_AU", LANG_ENGLISH   , SUBLANG_ENGLISH_AUS               , "English (Australia)")
   LNG(wxLANGUAGE_ENGLISH_BELIZE,             "en_BZ", LANG_ENGLISH   , SUBLANG_ENGLISH_BELIZE            , "English (Belize)")
   LNG(wxLANGUAGE_ENGLISH_BOTSWANA,           "en_BW", 0              , 0                                 , "English (Botswana)")
   LNG(wxLANGUAGE_ENGLISH_CANADA,             "en_CA", LANG_ENGLISH   , SUBLANG_ENGLISH_CAN               , "English (Canada)")
   LNG(wxLANGUAGE_ENGLISH_CARIBBEAN,          "en_CB", LANG_ENGLISH   , SUBLANG_ENGLISH_CARIBBEAN         , "English (Caribbean)")
   LNG(wxLANGUAGE_ENGLISH_DENMARK,            "en_DK", 0              , 0                                 , "English (Denmark)")
   LNG(wxLANGUAGE_ENGLISH_EIRE,               "en_IE", LANG_ENGLISH   , SUBLANG_ENGLISH_EIRE              , "English (Eire)")
   LNG(wxLANGUAGE_ENGLISH_JAMAICA,            "en_JM", LANG_ENGLISH   , SUBLANG_ENGLISH_JAMAICA           , "English (Jamaica)")
   LNG(wxLANGUAGE_ENGLISH_NEW_ZEALAND,        "en_NZ", LANG_ENGLISH   , SUBLANG_ENGLISH_NZ                , "English (New Zealand)")
   LNG(wxLANGUAGE_ENGLISH_PHILIPPINES,        "en_PH", LANG_ENGLISH   , SUBLANG_ENGLISH_PHILIPPINES       , "English (Philippines)")
   LNG(wxLANGUAGE_ENGLISH_SOUTH_AFRICA,       "en_ZA", LANG_ENGLISH   , SUBLANG_ENGLISH_SOUTH_AFRICA      , "English (South Africa)")
   LNG(wxLANGUAGE_ENGLISH_TRINIDAD,           "en_TT", LANG_ENGLISH   , SUBLANG_ENGLISH_TRINIDAD          , "English (Trinidad)")
   LNG(wxLANGUAGE_ENGLISH_ZIMBABWE,           "en_ZW", LANG_ENGLISH   , SUBLANG_ENGLISH_ZIMBABWE          , "English (Zimbabwe)")
   LNG(wxLANGUAGE_ESPERANTO,                  "eo"   , 0              , 0                                 , "Esperanto")
   LNG(wxLANGUAGE_ESTONIAN,                   "et_EE", LANG_ESTONIAN  , SUBLANG_DEFAULT                   , "Estonian")
   LNG(wxLANGUAGE_FAEROESE,                   "fo_FO", LANG_FAEROESE  , SUBLANG_DEFAULT                   , "Faeroese")
   LNG(wxLANGUAGE_FARSI,                      ""     , LANG_FARSI     , SUBLANG_DEFAULT                   , "Farsi")
   LNG(wxLANGUAGE_FIJI,                       "fj"   , 0              , 0                                 , "Fiji")
   LNG(wxLANGUAGE_FINNISH,                    "fi_FI", LANG_FINNISH   , SUBLANG_DEFAULT                   , "Finnish")
   LNG(wxLANGUAGE_FRENCH,                     "fr_FR", LANG_FRENCH    , SUBLANG_FRENCH                    , "French")
   LNG(wxLANGUAGE_FRENCH_BELGIAN,             "fr_BE", LANG_FRENCH    , SUBLANG_FRENCH_BELGIAN            , "French (Belgian)")
   LNG(wxLANGUAGE_FRENCH_CANADIAN,            "fr_CA", LANG_FRENCH    , SUBLANG_FRENCH_CANADIAN           , "French (Canadian)")
   LNG(wxLANGUAGE_FRENCH_LUXEMBOURG,          "fr_LU", LANG_FRENCH    , SUBLANG_FRENCH_LUXEMBOURG         , "French (Luxembourg)")
   LNG(wxLANGUAGE_FRENCH_MONACO,              "fr_MC", LANG_FRENCH    , SUBLANG_FRENCH_MONACO             , "French (Monaco)")
   LNG(wxLANGUAGE_FRENCH_SWISS,               "fr_CH", LANG_FRENCH    , SUBLANG_FRENCH_SWISS              , "French (Swiss)")
   LNG(wxLANGUAGE_FRISIAN,                    "fy"   , 0              , 0                                 , "Frisian")
   LNG(wxLANGUAGE_GALICIAN,                   "gl_ES", 0              , 0                                 , "Galician")
   LNG(wxLANGUAGE_GEORGIAN,                   "ka"   , LANG_GEORGIAN  , SUBLANG_DEFAULT                   , "Georgian")
   LNG(wxLANGUAGE_GERMAN,                     "de_DE", LANG_GERMAN    , SUBLANG_GERMAN                    , "German")
   LNG(wxLANGUAGE_GERMAN_AUSTRIAN,            "de_AT", LANG_GERMAN    , SUBLANG_GERMAN_AUSTRIAN           , "German (Austrian)")
   LNG(wxLANGUAGE_GERMAN_BELGIUM,             "de_BE", 0              , 0                                 , "German (Belgium)")
   LNG(wxLANGUAGE_GERMAN_LIECHTENSTEIN,       "de_LI", LANG_GERMAN    , SUBLANG_GERMAN_LIECHTENSTEIN      , "German (Liechtenstein)")
   LNG(wxLANGUAGE_GERMAN_LUXEMBOURG,          "de_LU", LANG_GERMAN    , SUBLANG_GERMAN_LUXEMBOURG         , "German (Luxembourg)")
   LNG(wxLANGUAGE_GERMAN_SWISS,               "de_CH", LANG_GERMAN    , SUBLANG_GERMAN_SWISS              , "German (Swiss)")
   LNG(wxLANGUAGE_GREEK,                      "el_GR", LANG_GREEK     , SUBLANG_DEFAULT                   , "Greek")
   LNG(wxLANGUAGE_GREENLANDIC,                "kl_GL", 0              , 0                                 , "Greenlandic")
   LNG(wxLANGUAGE_GUARANI,                    "gn"   , 0              , 0                                 , "Guarani")
   LNG(wxLANGUAGE_GUJARATI,                   "gu"   , LANG_GUJARATI  , SUBLANG_DEFAULT                   , "Gujarati")
   LNG(wxLANGUAGE_HAUSA,                      "ha"   , 0              , 0                                 , "Hausa")
   LNG(wxLANGUAGE_HEBREW,                     "he_IL", LANG_HEBREW    , SUBLANG_DEFAULT                   , "Hebrew")
   LNG(wxLANGUAGE_HINDI,                      "hi_IN", LANG_HINDI     , SUBLANG_DEFAULT                   , "Hindi")
   LNG(wxLANGUAGE_HUNGARIAN,                  "hu_HU", LANG_HUNGARIAN , SUBLANG_DEFAULT                   , "Hungarian")
   LNG(wxLANGUAGE_ICELANDIC,                  "is_IS", LANG_ICELANDIC , SUBLANG_DEFAULT                   , "Icelandic")
   LNG(wxLANGUAGE_INDONESIAN,                 "id_ID", LANG_INDONESIAN, SUBLANG_DEFAULT                   , "Indonesian")
   LNG(wxLANGUAGE_INTERLINGUA,                "ia"   , 0              , 0                                 , "Interlingua")
   LNG(wxLANGUAGE_INTERLINGUE,                "ie"   , 0              , 0                                 , "Interlingue")
   LNG(wxLANGUAGE_INUKTITUT,                  "iu"   , 0              , 0                                 , "Inuktitut")
   LNG(wxLANGUAGE_INUPIAK,                    "ik"   , 0              , 0                                 , "Inupiak")
   LNG(wxLANGUAGE_IRISH,                      "ga_IE", 0              , 0                                 , "Irish")
   LNG(wxLANGUAGE_ITALIAN,                    "it_IT", LANG_ITALIAN   , SUBLANG_ITALIAN                   , "Italian")
   LNG(wxLANGUAGE_ITALIAN_SWISS,              "it_CH", LANG_ITALIAN   , SUBLANG_ITALIAN_SWISS             , "Italian (Swiss)")
   LNG(wxLANGUAGE_JAPANESE,                   "ja_JP", LANG_JAPANESE  , SUBLANG_DEFAULT                   , "Japanese")
   LNG(wxLANGUAGE_JAVANESE,                   "jw"   , 0              , 0                                 , "Javanese")
   LNG(wxLANGUAGE_KANNADA,                    "kn"   , LANG_KANNADA   , SUBLANG_DEFAULT                   , "Kannada")
   LNG(wxLANGUAGE_KASHMIRI,                   "ks"   , LANG_KASHMIRI  , SUBLANG_DEFAULT                   , "Kashmiri")
   LNG(wxLANGUAGE_KASHMIRI_INDIA,             "ks_IN", LANG_KASHMIRI  , SUBLANG_KASHMIRI_INDIA            , "Kashmiri (India)")
   LNG(wxLANGUAGE_KAZAKH,                     "kk"   , LANG_KAZAK     , SUBLANG_DEFAULT                   , "Kazakh")
   LNG(wxLANGUAGE_KERNEWEK,                   "kw_GB", 0              , 0                                 , "Kernewek")
   LNG(wxLANGUAGE_KINYARWANDA,                "rw"   , 0              , 0                                 , "Kinyarwanda")
   LNG(wxLANGUAGE_KIRGHIZ,                    "ky"   , 0              , 0                                 , "Kirghiz")
   LNG(wxLANGUAGE_KIRUNDI,                    "rn"   , 0              , 0                                 , "Kirundi")
   LNG(wxLANGUAGE_KONKANI,                    ""     , LANG_KONKANI   , SUBLANG_DEFAULT                   , "Konkani")
   LNG(wxLANGUAGE_KOREAN,                     "ko_KR", LANG_KOREAN    , SUBLANG_KOREAN                    , "Korean")
   LNG(wxLANGUAGE_KURDISH,                    "ku"   , 0              , 0                                 , "Kurdish")
   LNG(wxLANGUAGE_LAOTHIAN,                   "lo"   , 0              , 0                                 , "Laothian")
   LNG(wxLANGUAGE_LATIN,                      "la"   , 0              , 0                                 , "Latin")
   LNG(wxLANGUAGE_LATVIAN,                    "lv_LV", LANG_LATVIAN   , SUBLANG_DEFAULT                   , "Latvian")
   LNG(wxLANGUAGE_LINGALA,                    "ln"   , 0              , 0                                 , "Lingala")
   LNG(wxLANGUAGE_LITHUANIAN,                 "lt_LT", LANG_LITHUANIAN, SUBLANG_LITHUANIAN                , "Lithuanian")
   LNG(wxLANGUAGE_MACEDONIAN,                 "mk_MK", LANG_MACEDONIAN, SUBLANG_DEFAULT                   , "Macedonian")
   LNG(wxLANGUAGE_MALAGASY,                   "mg"   , 0              , 0                                 , "Malagasy")
   LNG(wxLANGUAGE_MALAY,                      "ms_MY", LANG_MALAY     , SUBLANG_DEFAULT                   , "Malay")
   LNG(wxLANGUAGE_MALAYALAM,                  "ml"   , LANG_MALAYALAM , SUBLANG_DEFAULT                   , "Malayalam")
   LNG(wxLANGUAGE_MALAY_BRUNEI_DARUSSALAM,    "ms_BN", LANG_MALAY     , SUBLANG_MALAY_BRUNEI_DARUSSALAM   , "Malay (Brunei Darussalam)")
   LNG(wxLANGUAGE_MALAY_MALAYSIA,             "ms_MY", LANG_MALAY     , SUBLANG_MALAY_MALAYSIA            , "Malay (Malaysia)")
   LNG(wxLANGUAGE_MALTESE,                    "mt_MT", 0              , 0                                 , "Maltese")
   LNG(wxLANGUAGE_MANIPURI,                   ""     , LANG_MANIPURI  , SUBLANG_DEFAULT                   , "Manipuri")
   LNG(wxLANGUAGE_MAORI,                      "mi"   , 0              , 0                                 , "Maori")
   LNG(wxLANGUAGE_MARATHI,                    "mr_IN", LANG_MARATHI   , SUBLANG_DEFAULT                   , "Marathi")
   LNG(wxLANGUAGE_MOLDAVIAN,                  "mo"   , 0              , 0                                 , "Moldavian")
   LNG(wxLANGUAGE_MONGOLIAN,                  "mn"   , 0              , 0                                 , "Mongolian")
   LNG(wxLANGUAGE_NAURU,                      "na"   , 0              , 0                                 , "Nauru")
   LNG(wxLANGUAGE_NEPALI,                     "ne"   , LANG_NEPALI    , SUBLANG_DEFAULT                   , "Nepali")
   LNG(wxLANGUAGE_NEPALI_INDIA,               "ne_IN", LANG_NEPALI    , SUBLANG_NEPALI_INDIA              , "Nepali (India)")
   LNG(wxLANGUAGE_NORWEGIAN_BOKMAL,           "no_NO", LANG_NORWEGIAN , SUBLANG_NORWEGIAN_BOKMAL          , "Norwegian (Bokmal)")
   LNG(wxLANGUAGE_NORWEGIAN_NYNORSK,          "nn_NO", LANG_NORWEGIAN , SUBLANG_NORWEGIAN_NYNORSK         , "Norwegian (Nynorsk)")
   LNG(wxLANGUAGE_OCCITAN,                    "oc"   , 0              , 0                                 , "Occitan")
   LNG(wxLANGUAGE_ORIYA,                      "or"   , LANG_ORIYA     , SUBLANG_DEFAULT                   , "Oriya")
   LNG(wxLANGUAGE_OROMO,                      "om"   , 0              , 0                                 , "(Afan) Oromo")
   LNG(wxLANGUAGE_PASHTO,                     "ps"   , 0              , 0                                 , "Pashto, Pushto")
   LNG(wxLANGUAGE_PERSIAN,                    "fa_IR", 0              , 0                                 , "Persian")
   LNG(wxLANGUAGE_POLISH,                     "pl_PL", LANG_POLISH    , SUBLANG_DEFAULT                   , "Polish")
   LNG(wxLANGUAGE_PORTUGUESE,                 "pt_PT", LANG_PORTUGUESE, SUBLANG_PORTUGUESE                , "Portuguese")
   LNG(wxLANGUAGE_PORTUGUESE_BRAZILIAN,       "pt_BR", LANG_PORTUGUESE, SUBLANG_PORTUGUESE_BRAZILIAN      , "Portuguese (Brazilian)")
   LNG(wxLANGUAGE_PUNJABI,                    "pa"   , LANG_PUNJABI   , SUBLANG_DEFAULT                   , "Punjabi")
   LNG(wxLANGUAGE_QUECHUA,                    "qu"   , 0              , 0                                 , "Quechua")
   LNG(wxLANGUAGE_RHAETO_ROMANCE,             "rm"   , 0              , 0                                 , "Rhaeto-Romance")
   LNG(wxLANGUAGE_ROMANIAN,                   "ro_RO", LANG_ROMANIAN  , SUBLANG_DEFAULT                   , "Romanian")
   LNG(wxLANGUAGE_RUSSIAN,                    "ru_RU", LANG_RUSSIAN   , SUBLANG_DEFAULT                   , "Russian")
   LNG(wxLANGUAGE_RUSSIAN_UKRAINE,            "ru_UA", 0              , 0                                 , "Russian (Ukraine)")
   LNG(wxLANGUAGE_SAMOAN,                     "sm"   , 0              , 0                                 , "Samoan")
   LNG(wxLANGUAGE_SANGHO,                     "sg"   , 0              , 0                                 , "Sangho")
   LNG(wxLANGUAGE_SANSKRIT,                   "sa"   , LANG_SANSKRIT  , SUBLANG_DEFAULT                   , "Sanskrit")
   LNG(wxLANGUAGE_SCOTS_GAELIC,               "gd"   , 0              , 0                                 , "Scots Gaelic")
   LNG(wxLANGUAGE_SERBIAN,                    "sr_YU", LANG_SERBIAN   , SUBLANG_DEFAULT                   , "Serbian")
   LNG(wxLANGUAGE_SERBIAN_CYRILLIC,           "sr_YU", LANG_SERBIAN   , SUBLANG_SERBIAN_CYRILLIC          , "Serbian (Cyrillic)")
   LNG(wxLANGUAGE_SERBIAN_LATIN,              "sr_YU", LANG_SERBIAN   , SUBLANG_SERBIAN_LATIN             , "Serbian (Latin)")
   LNG(wxLANGUAGE_SERBO_CROATIAN,             "sh"   , 0              , 0                                 , "Serbo-Croatian")
   LNG(wxLANGUAGE_SESOTHO,                    "st"   , 0              , 0                                 , "Sesotho")
   LNG(wxLANGUAGE_SETSWANA,                   "tn"   , 0              , 0                                 , "Setswana")
   LNG(wxLANGUAGE_SHONA,                      "sn"   , 0              , 0                                 , "Shona")
   LNG(wxLANGUAGE_SINDHI,                     "sd"   , LANG_SINDHI    , SUBLANG_DEFAULT                   , "Sindhi")
   LNG(wxLANGUAGE_SINHALESE,                  "si"   , 0              , 0                                 , "Sinhalese")
   LNG(wxLANGUAGE_SISWATI,                    "ss"   , 0              , 0                                 , "Siswati")
   LNG(wxLANGUAGE_SLOVAK,                     "sk_SK", LANG_SLOVAK    , SUBLANG_DEFAULT                   , "Slovak")
   LNG(wxLANGUAGE_SLOVENIAN,                  "sl_SI", LANG_SLOVENIAN , SUBLANG_DEFAULT                   , "Slovenian")
   LNG(wxLANGUAGE_SOMALI,                     "so"   , 0              , 0                                 , "Somali")
   LNG(wxLANGUAGE_SPANISH,                    "es_ES", LANG_SPANISH   , SUBLANG_SPANISH                   , "Spanish")
   LNG(wxLANGUAGE_SPANISH_ARGENTINA,          "es_AR", LANG_SPANISH   , SUBLANG_SPANISH_ARGENTINA         , "Spanish (Argentina)")
   LNG(wxLANGUAGE_SPANISH_BOLIVIA,            "es_BO", LANG_SPANISH   , SUBLANG_SPANISH_BOLIVIA           , "Spanish (Bolivia)")
   LNG(wxLANGUAGE_SPANISH_CHILE,              "es_CL", LANG_SPANISH   , SUBLANG_SPANISH_CHILE             , "Spanish (Chile)")
   LNG(wxLANGUAGE_SPANISH_COLOMBIA,           "es_CO", LANG_SPANISH   , SUBLANG_SPANISH_COLOMBIA          , "Spanish (Colombia)")
   LNG(wxLANGUAGE_SPANISH_COSTA_RICA,         "es_CR", LANG_SPANISH   , SUBLANG_SPANISH_COSTA_RICA        , "Spanish (Costa Rica)")
   LNG(wxLANGUAGE_SPANISH_DOMINICAN_REPUBLIC, "es_DO", LANG_SPANISH   , SUBLANG_SPANISH_DOMINICAN_REPUBLIC, "Spanish (Dominican republic)")
   LNG(wxLANGUAGE_SPANISH_ECUADOR,            "es_EC", LANG_SPANISH   , SUBLANG_SPANISH_ECUADOR           , "Spanish (Ecuador)")
   LNG(wxLANGUAGE_SPANISH_EL_SALVADOR,        "es_SV", LANG_SPANISH   , SUBLANG_SPANISH_EL_SALVADOR       , "Spanish (El Salvador)")
   LNG(wxLANGUAGE_SPANISH_GUATEMALA,          "es_GT", LANG_SPANISH   , SUBLANG_SPANISH_GUATEMALA         , "Spanish (Guatemala)")
   LNG(wxLANGUAGE_SPANISH_HONDURAS,           "es_HN", LANG_SPANISH   , SUBLANG_SPANISH_HONDURAS          , "Spanish (Honduras)")
   LNG(wxLANGUAGE_SPANISH_MEXICAN,            "es_MX", LANG_SPANISH   , SUBLANG_SPANISH_MEXICAN           , "Spanish (Mexican)")
   LNG(wxLANGUAGE_SPANISH_MODERN,             "es_ES", LANG_SPANISH   , SUBLANG_SPANISH_MODERN            , "Spanish (Modern)")
   LNG(wxLANGUAGE_SPANISH_NICARAGUA,          "es_NI", LANG_SPANISH   , SUBLANG_SPANISH_NICARAGUA         , "Spanish (Nicaragua)")
   LNG(wxLANGUAGE_SPANISH_PANAMA,             "es_PA", LANG_SPANISH   , SUBLANG_SPANISH_PANAMA            , "Spanish (Panama)")
   LNG(wxLANGUAGE_SPANISH_PARAGUAY,           "es_PY", LANG_SPANISH   , SUBLANG_SPANISH_PARAGUAY          , "Spanish (Paraguay)")
   LNG(wxLANGUAGE_SPANISH_PERU,               "es_PE", LANG_SPANISH   , SUBLANG_SPANISH_PERU              , "Spanish (Peru)")
   LNG(wxLANGUAGE_SPANISH_PUERTO_RICO,        "es_PR", LANG_SPANISH   , SUBLANG_SPANISH_PUERTO_RICO       , "Spanish (Puerto Rico)")
   LNG(wxLANGUAGE_SPANISH_URUGUAY,            "es_UY", LANG_SPANISH   , SUBLANG_SPANISH_URUGUAY           , "Spanish (Uruguay)")
   LNG(wxLANGUAGE_SPANISH_US,                 "es_US", 0              , 0                                 , "Spanish (U.S.)")
   LNG(wxLANGUAGE_SPANISH_VENEZUELA,          "es_VE", LANG_SPANISH   , SUBLANG_SPANISH_VENEZUELA         , "Spanish (Venezuela)")
   LNG(wxLANGUAGE_SUNDANESE,                  "su"   , 0              , 0                                 , "Sundanese")
   LNG(wxLANGUAGE_SWAHILI,                    "sw_KE", LANG_SWAHILI   , SUBLANG_DEFAULT                   , "Swahili")
   LNG(wxLANGUAGE_SWEDISH,                    "sv_SE", LANG_SWEDISH   , SUBLANG_SWEDISH                   , "Swedish")
   LNG(wxLANGUAGE_SWEDISH_FINLAND,            "sv_FI", LANG_SWEDISH   , SUBLANG_SWEDISH_FINLAND           , "Swedish (Finland)")
   LNG(wxLANGUAGE_TAGALOG,                    "tl"   , 0              , 0                                 , "Tagalog")
   LNG(wxLANGUAGE_TAJIK,                      "tg"   , 0              , 0                                 , "Tajik")
   LNG(wxLANGUAGE_TAMIL,                      "ta"   , LANG_TAMIL     , SUBLANG_DEFAULT                   , "Tamil")
   LNG(wxLANGUAGE_TATAR,                      "tt"   , LANG_TATAR     , SUBLANG_DEFAULT                   , "Tatar")
   LNG(wxLANGUAGE_TELUGU,                     "te"   , LANG_TELUGU    , SUBLANG_DEFAULT                   , "Telugu")
   LNG(wxLANGUAGE_THAI,                       "th_TH", LANG_THAI      , SUBLANG_DEFAULT                   , "Thai")
   LNG(wxLANGUAGE_TIBETAN,                    "bo"   , 0              , 0                                 , "Tibetan")
   LNG(wxLANGUAGE_TIGRINYA,                   "ti"   , 0              , 0                                 , "Tigrinya")
   LNG(wxLANGUAGE_TONGA,                      "to"   , 0              , 0                                 , "Tonga")
   LNG(wxLANGUAGE_TSONGA,                     "ts"   , 0              , 0                                 , "Tsonga")
   LNG(wxLANGUAGE_TURKISH,                    "tr_TR", LANG_TURKISH   , SUBLANG_DEFAULT                   , "Turkish")
   LNG(wxLANGUAGE_TURKMEN,                    "tk"   , 0              , 0                                 , "Turkmen")
   LNG(wxLANGUAGE_TWI,                        "tw"   , 0              , 0                                 , "Twi")
   LNG(wxLANGUAGE_UIGHUR,                     "ug"   , 0              , 0                                 , "Uighur")
   LNG(wxLANGUAGE_UKRAINIAN,                  "uk_UA", LANG_UKRAINIAN , SUBLANG_DEFAULT                   , "Ukrainian")
   LNG(wxLANGUAGE_URDU,                       "ur"   , LANG_URDU      , SUBLANG_DEFAULT                   , "Urdu")
   LNG(wxLANGUAGE_URDU_INDIA,                 "ur_IN", LANG_URDU      , SUBLANG_URDU_INDIA                , "Urdu (India)")
   LNG(wxLANGUAGE_URDU_PAKISTAN,              "ur_PK", LANG_URDU      , SUBLANG_URDU_PAKISTAN             , "Urdu (Pakistan)")
   LNG(wxLANGUAGE_UZBEK,                      "uz"   , LANG_UZBEK     , SUBLANG_DEFAULT                   , "Uzbek")
   LNG(wxLANGUAGE_UZBEK_CYRILLIC,             "uz"   , LANG_UZBEK     , SUBLANG_UZBEK_CYRILLIC            , "Uzbek (Cyrillic)")
   LNG(wxLANGUAGE_UZBEK_LATIN,                "uz"   , LANG_UZBEK     , SUBLANG_UZBEK_LATIN               , "Uzbek (Latin)")
   LNG(wxLANGUAGE_VIETNAMESE,                 "vi_VN", LANG_VIETNAMESE, SUBLANG_DEFAULT                   , "Vietnamese")
   LNG(wxLANGUAGE_VOLAPUK,                    "vo"   , 0              , 0                                 , "Volapuk")
   LNG(wxLANGUAGE_WELSH,                      "cy"   , 0              , 0                                 , "Welsh")
   LNG(wxLANGUAGE_WOLOF,                      "wo"   , 0              , 0                                 , "Wolof")
   LNG(wxLANGUAGE_XHOSA,                      "xh"   , 0              , 0                                 , "Xhosa")
   LNG(wxLANGUAGE_YIDDISH,                    "yi"   , 0              , 0                                 , "Yiddish")
   LNG(wxLANGUAGE_YORUBA,                     "yo"   , 0              , 0                                 , "Yoruba")
   LNG(wxLANGUAGE_ZHUANG,                     "za"   , 0              , 0                                 , "Zhuang")
   LNG(wxLANGUAGE_ZULU,                       "zu"   , 0              , 0                                 , "Zulu")
};
#undef LNG



#endif // wxUSE_INTL

