/////////////////////////////////////////////////////////////////////////////
// Name:        intl.h
// Purpose:     Internationalization and localisation for wxWindows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:   	wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   __INTLH__
#define   __INTLH__

#ifdef __GNUG__
#pragma interface "intl.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"

// ============================================================================
// global decls
// ============================================================================

// ----------------------------------------------------------------------------
// simple types
// ----------------------------------------------------------------------------

// # adjust if necessary
typedef unsigned char size_t8;
typedef unsigned long size_t32;

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// gettext() style macro
#define   _(str)  wxGetTranslation(str)

// ----------------------------------------------------------------------------
// forward decls
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxLocale;
class WXDLLEXPORT wxMsgCatalog;
extern WXDLLEXPORT_DATA(wxLocale *) g_pLocale;

// ============================================================================
// locale support
// ============================================================================

// ----------------------------------------------------------------------------
// wxLocale: encapsulates all language dependent settings, including current
//           message catalogs, date, time and currency formats (#### to do) &c
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxLocale
{
public:
  // ctor & dtor
    // call Init() if you use this ctor
  wxLocale();
    // the ctor has a side effect of changing current locale
  wxLocale(const char *szName,              // name (for messages)
           const char *szShort = (const char *) NULL,      // dir prefix (for msg files)
           const char *szLocale = (const char *) NULL,     // locale (for setlocale)
           bool bLoadDefault = TRUE)        // preload wxstd.mo?
    { Init(szName, szShort, szLocale, bLoadDefault); }
    // the same as a function (returns TRUE on success)
  bool Init(const char *szName,
            const char *szShort = (const char *) NULL,
            const char *szLocale = (const char *) NULL,
            bool bLoadDefault = TRUE);
    // restores old locale
 ~wxLocale();

  // returns locale name
  const char *GetLocale() const { return m_strLocale; }

  // add a catalog: it's searched for in standard places (current directory 
  // first, system one after). It will be used for message lookup by
  // GetString().
  //
  // Returns 'true' if it was successfully loaded
  bool AddCatalog(const char *szDomain);
  
  // check if the given catalog is loaded
  bool IsLoaded(const char *szDomain) const;
  
  // retrieve the translation for a string in all loaded domains unless
  // the szDomain parameter is specified (and then only this domain is
  // searched)
  //
  // return original string if translation is not available
  // (in this case an error message is generated the first time
  //  a string is not found; use wxLogNull to suppress it)
  //
  // domains are searched in the last to first order, i.e. catalogs
  // added later override those added before.
  const char *GetString(const char *szOrigString, 
                        const char *szDomain = (const char *) NULL) const;

private:
  // find catalog by name in a linked list, return NULL if !found
  wxMsgCatalog  *FindCatalog(const char *szDomain) const;

  wxString       m_strLocale,     // this locale name
                 m_strShort;      // short name for the locale
  
  const char    *m_pszOldLocale;  // previous locale from setlocale()
  wxLocale      *m_pOldLocale;    // previous wxLocale
  
  wxMsgCatalog  *m_pMsgCat;       // pointer to linked list of catalogs
};

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------
WXDLLEXPORT wxLocale* wxGetLocale();

// get the translation of the string in the current locale  
inline WXDLLEXPORT const char *wxGetTranslation(const char *sz)
{
  wxLocale *pLoc = wxGetLocale();
  return pLoc == (wxLocale *) NULL ? sz : pLoc->GetString(sz);
}

#endif
	// _WX_INTLH__
