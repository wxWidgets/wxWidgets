/////////////////////////////////////////////////////////////////////////////
// Name:        intl.h
// Purpose:     Internationalization and localisation for wxWindows
// Author:      Vadim Zeitlin
// Modified by:
// Created:     29/01/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#ifndef   __INTLH__
#define   __INTLH__

#ifdef __GNUG__
    #pragma interface "intl.h"
#endif

#include "wx/defs.h"
#include "wx/string.h"

#if wxUSE_INTL

// ============================================================================
// global decls
// ============================================================================

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// gettext() style macro (notice that xgettext should be invoked with "-k_"
// option to extract the strings inside _() from the sources)
#ifndef WXINTL_NO_GETTEXT_MACRO
    #define   _(str)  wxGetTranslation(wxT(str))
#endif

// ----------------------------------------------------------------------------
// forward decls
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxLocale;
class WXDLLEXPORT wxMsgCatalog;

// ============================================================================
// locale support
// ============================================================================

// ----------------------------------------------------------------------------
// wxLocale: encapsulates all language dependent settings, including current
//           message catalogs, date, time and currency formats (TODO) &c
// ----------------------------------------------------------------------------
class WXDLLEXPORT wxLocale
{
public:
    // ctor & dtor
    // -----------

        // call Init() if you use this ctor
    wxLocale();
        // the ctor has a side effect of changing current locale
    wxLocale(const wxChar *szName,              // name (for messages)
             const wxChar *szShort = (const wxChar *) NULL,      // dir prefix (for msg files)
             const wxChar *szLocale = (const wxChar *) NULL,     // locale (for setlocale)
             bool bLoadDefault = TRUE)        // preload wxstd.mo?
        { Init(szName, szShort, szLocale, bLoadDefault); }
        // the same as a function (returns TRUE on success)
    bool Init(const wxChar *szName,
              const wxChar *szShort = (const wxChar *) NULL,
              const wxChar *szLocale = (const wxChar *) NULL,
              bool bLoadDefault = TRUE);
        // restores old locale
    ~wxLocale();

    // returns locale name
    const wxChar *GetLocale() const { return m_strLocale; }

    // add a prefix to the catalog lookup path: the message catalog files will be
    // looked up under prefix/<lang>/LC_MESSAGES, prefix/LC_MESSAGES and prefix
    // (in this order).
    //
    // This only applies to subsequent invocations of AddCatalog()!
    static void AddCatalogLookupPathPrefix(const wxString& prefix);

    // add a catalog: it's searched for in standard places (current directory
    // first, system one after), but the you may prepend additional directories to
    // the search path with AddCatalogLookupPathPrefix().
    //
    // The loaded catalog will be used for message lookup by GetString().
    //
    // Returns 'true' if it was successfully loaded
    bool AddCatalog(const wxChar *szDomain);

    // check if the given catalog is loaded
    bool IsLoaded(const wxChar *szDomain) const;

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
    const wxMB2WXbuf GetString(const wxChar *szOrigString,
			       const wxChar *szDomain = (const wxChar *) NULL) const;

    // Returns the current short name for the locale
    const wxString& GetName() const { return m_strShort; }

private:
    // find catalog by name in a linked list, return NULL if !found
    wxMsgCatalog  *FindCatalog(const wxChar *szDomain) const;

    wxString       m_strLocale,     // this locale name
                   m_strShort;      // short name for the locale

    const wxChar  *m_pszOldLocale;  // previous locale from setlocale()
    wxLocale      *m_pOldLocale;    // previous wxLocale

    wxMsgCatalog  *m_pMsgCat;       // pointer to linked list of catalogs
};

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// get the current locale object (note that it may be NULL!)
extern WXDLLEXPORT wxLocale* wxGetLocale();

// get the translation of the string in the current locale
inline const wxMB2WXbuf wxGetTranslation(const wxChar *sz)
{
    wxLocale *pLoc = wxGetLocale();
    if (pLoc)
        return pLoc->GetString(sz);
    else
        return (const wxMB2WXbuf)sz;
}

#else // !wxUSE_INTL

#ifndef WXINTL_NO_GETTEXT_MACRO
    #define   _(str)  (str)
#endif

inline const wxChar *wxGetTranslation(const wxChar *sz) { return sz; }

#endif // wxUSE_INTL/!wxUSE_INTL

#endif
    // _WX_INTLH__
