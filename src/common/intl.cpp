/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/intl.cpp
// Purpose:     Internationalization and localisation for wxWidgets
// Author:      Vadim Zeitlin
// Modified by: Michael N. Filippov <michael@idisys.iae.nsk.su>
//              (2003/09/30 - PluralForms support)
// Created:     29/01/98
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declaration
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_INTL

#ifndef WX_PRECOMP
    #include "wx/string.h"
    #include "wx/intl.h"
    #include "wx/log.h"
    #include "wx/utils.h"
    #include "wx/app.h"
    #include "wx/hashmap.h"
    #include "wx/module.h"
#endif // WX_PRECOMP

#include <locale.h>

// standard headers
#include <ctype.h>
#include <stdlib.h>
#ifdef HAVE_LANGINFO_H
    #include <langinfo.h>
#endif

#include "wx/file.h"
#include "wx/filename.h"
#include "wx/tokenzr.h"
#include "wx/fontmap.h"
#include "wx/scopedptr.h"
#include "wx/apptrait.h"
#include "wx/stdpaths.h"
#include "wx/hashset.h"
#include "wx/uilocale.h"

#include "wx/private/uilocale.h"

#ifdef __WIN32__
    #include "wx/msw/private/uilocale.h"
#elif defined(__WXOSX__)
    #include "wx/osx/core/cfref.h"
    #include "wx/osx/core/cfstring.h"
    #include <CoreFoundation/CFLocale.h>
    #include <CoreFoundation/CFDateFormatter.h>
    #include <CoreFoundation/CFString.h>
#elif defined(__UNIX__)
    #include "wx/unix/private/uilocale.h"
#endif

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

#define TRACE_I18N wxS("i18n")

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

static wxLocale *wxSetLocale(wxLocale *pLocale);

// ----------------------------------------------------------------------------
// wxLanguageInfo
// ----------------------------------------------------------------------------

#ifdef __WINDOWS__

// helper used by wxLanguageInfo::GetLocaleName() and elsewhere to determine
// whether the locale is Unicode-only (it is if this function returns empty
// string)
static wxString wxGetANSICodePageForLocale(LCID lcid)
{
    wxString cp;

    wxChar buffer[16];
    if ( ::GetLocaleInfo(lcid, LOCALE_IDEFAULTANSICODEPAGE,
                        buffer, WXSIZEOF(buffer)) > 0 )
    {
        if ( buffer[0] != wxT('0') || buffer[1] != wxT('\0') )
            cp = buffer;
        //else: this locale doesn't use ANSI code page
    }

    return cp;
}

wxUint32 wxLanguageInfo::GetLCID() const
{
    return MAKELCID(MAKELANGID(WinLang, WinSublang), SORT_DEFAULT);
}

const char* wxLanguageInfo::TrySetLocale() const
{
    wxString locale;

    const LCID lcid = GetLCID();

    wxChar buffer[256];
    buffer[0] = wxT('\0');

    // Prefer to use the new (Vista and later) locale names instead of locale
    // identifiers if supported, both at the OS level (LOCALE_SNAME) and by the
    // CRT (check by calling setlocale()).
    if ( wxGetWinVersion() >= wxWinVersion_Vista )
    {
        locale = LocaleTag;
        const char* const retloc = wxSetlocale(LC_ALL, locale);
        if ( retloc )
            return retloc;
        //else: fall back to LOCALE_SENGLANGUAGE
    }

    if ( !::GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, buffer, WXSIZEOF(buffer)) )
    {
        wxLogLastError(wxT("GetLocaleInfo(LOCALE_SENGLANGUAGE)"));
        return NULL;
    }

    locale = buffer;
    if ( ::GetLocaleInfo(lcid, LOCALE_SENGCOUNTRY,
                        buffer, WXSIZEOF(buffer)) > 0 )
    {
        locale << wxT('_') << buffer;
    }

    const wxString cp = wxGetANSICodePageForLocale(lcid);
    if ( !cp.empty() )
    {
        locale << wxT('.') << cp;
    }

    return wxSetlocale(LC_ALL, locale);
}

#else // !__WINDOWS__

const char* wxLanguageInfo::TrySetLocale() const
{
    return wxSetlocale(LC_ALL, CanonicalRef.empty() ? CanonicalName : CanonicalRef);
}

#endif // __WINDOWS__/!__WINDOWS__

wxString wxLanguageInfo::GetLocaleName() const
{
    const char* const orig = wxSetlocale(LC_ALL, NULL);

    const char* const ret = TrySetLocale();
    wxString retval;
    if ( ret )
    {
        // Note that we must copy the returned value before calling setlocale()
        // again as the string "ret" points to can (and, at least under Linux
        // with glibc, actually always will) be changed by this call.
        retval = ret;
        wxSetlocale(LC_ALL, orig);
    }

    return retval;
}

// ----------------------------------------------------------------------------
// wxUILocale / wxLocale
// ----------------------------------------------------------------------------

static wxLanguageInfos gs_languagesDB;
static bool gs_languagesDBInitialized = false;

const wxLanguageInfos& wxGetLanguageInfos()
{
    wxUILocale::CreateLanguagesDB();

    return gs_languagesDB;
}

/*static*/
void wxUILocale::CreateLanguagesDB()
{
    if (!gs_languagesDBInitialized)
    {
        gs_languagesDBInitialized = true;

        InitLanguagesDB();
    }
}

/*static*/
void wxUILocale::DestroyLanguagesDB()
{
    if (gs_languagesDBInitialized)
    {
        gs_languagesDB.clear();
        gs_languagesDBInitialized = false;
    }
}

namespace
{

#if defined(__UNIX__) && !defined(__WXOSX__)
    // Small helper function: get the value of the given environment variable and
    // return true only if the variable was found and has non-empty value.
    inline bool wxGetNonEmptyEnvVar(const wxString& name, wxString* value)
    {
        return wxGetEnv(name, value) && !value->empty();
    }
#endif

} // anonymous namespace

/*static*/
int wxUILocale::GetSystemLanguage()
{
    CreateLanguagesDB();

    // init i to avoid compiler warning
    size_t i = 0,
        count = gs_languagesDB.size();

#ifdef __WXOSX__
    wxCFRef<CFLocaleRef> userLocaleRef(CFLocaleCopyCurrent());

    // because the locale identifier (kCFLocaleIdentifier) is formatted a little bit differently, eg
    // az_Cyrl_AZ@calendar=buddhist;currency=JPY we just recreate the base info as expected by wx here

    wxCFStringRef str(wxCFRetain((CFStringRef)CFLocaleGetValue(userLocaleRef, kCFLocaleLanguageCode)));
    const wxString langPrefix = str.AsString() + "_";

    str.reset(wxCFRetain((CFStringRef)CFLocaleGetValue(userLocaleRef, kCFLocaleCountryCode)));
    const wxString langFull = langPrefix + str.AsString();

    int langOnlyMatchIndex = wxNOT_FOUND;
    for (i = 0; i < count; i++)
    {
        const wxString& fullname = gs_languagesDB[i].CanonicalName;
        if (langFull == fullname)
        {
            // Exact match, no need to look any further.
            break;
        }

        if (fullname.StartsWith(langPrefix))
        {
            // Matched just the language, keep looking, but we'll keep this if
            // we don't find an exact match later.
            langOnlyMatchIndex = i;
        }
    }

    if (i == count && langOnlyMatchIndex != wxNOT_FOUND)
        i = langOnlyMatchIndex;
#elif defined(__UNIX__)
    // first get the string identifying the language from the environment
    wxString langFull;
    if (!wxGetNonEmptyEnvVar(wxS("LC_ALL"), &langFull) &&
        !wxGetNonEmptyEnvVar(wxS("LC_MESSAGES"), &langFull) &&
        !wxGetNonEmptyEnvVar(wxS("LANG"), &langFull))
    {
        // no language specified, treat it as English
        return wxLANGUAGE_ENGLISH_US;
    }

    // the language string has the following form
    //
    //      lang[_LANG][.encoding][@modifier]
    //
    // (see environ(5) in the Open Unix specification)
    //
    // where lang is the primary language, LANG is a sublang/territory,
    // encoding is the charset to use and modifier "allows the user to select
    // a specific instance of localization data within a single category"
    //
    // for example, the following strings are valid:
    //      fr
    //      fr_FR
    //      de_DE.iso88591
    //      de_DE@euro
    //      de_DE.iso88591@euro

    // for now we don't use the encoding, although we probably should (doing
    // translations of the msg catalogs on the fly as required) (TODO)
    //
    // we need the modified for languages like Valencian: ca_ES@valencia
    // though, remember it
    wxString modifier;
    size_t posModifier = langFull.find_first_of(wxS("@"));
    if (posModifier != wxString::npos)
        modifier = langFull.Mid(posModifier);

    size_t posEndLang = langFull.find_first_of(wxS("@."));
    if (posEndLang != wxString::npos)
    {
        langFull.Truncate(posEndLang);
    }

    if (langFull == wxS("C") || langFull == wxS("POSIX"))
    {
        // default C locale is English too
        return wxLANGUAGE_ENGLISH_US;
    }

    // do we have just the language (or sublang too)?
    const bool justLang = langFull.find('_') == wxString::npos;

    // 0. Make sure the lang is according to latest ISO 639
    //    (this is necessary because glibc uses iw and in instead
    //    of he and id respectively).

    // the language itself (second part is the dialect/sublang)
    wxString langOrig = ExtractLang(langFull);

    wxString lang;
    if (langOrig == wxS("iw"))
        lang = wxS("he");
    else if (langOrig == wxS("in"))
        lang = wxS("id");
    else if (langOrig == wxS("ji"))
        lang = wxS("yi");
    else if (langOrig == wxS("no_NO"))
        lang = wxS("nb_NO");
    else if (langOrig == wxS("no_NY"))
        lang = wxS("nn_NO");
    else if (langOrig == wxS("no"))
        lang = wxS("nb_NO");
    else
        lang = langOrig;

    // did we change it?
    if (lang != langOrig)
    {
        langFull = lang + ExtractNotLang(langFull);
    }

    // 1. Try to find the language either as is:
    // a) With modifier if set
    if (!modifier.empty())
    {
        wxString langFullWithModifier = langFull + modifier;
        for (i = 0; i < count; i++)
        {
            if (gs_languagesDB[i].CanonicalName == langFullWithModifier)
                break;
        }
    }

    // b) Without modifier
    if (modifier.empty() || i == count)
    {
        for (i = 0; i < count; i++)
        {
            if (gs_languagesDB[i].CanonicalName == langFull)
                break;
        }
    }

    // 2. If langFull is of the form xx_YY, try to find xx:
    if (i == count && !justLang)
    {
        for (i = 0; i < count; i++)
        {
            if (ExtractLang(gs_languagesDB[i].CanonicalName) == lang)
            {
                break;
            }
        }
    }

    // 3. If langFull is of the form xx, try to find any xx_YY record:
    if (i == count && justLang)
    {
        for (i = 0; i < count; i++)
        {
            if (ExtractLang(gs_languagesDB[i].CanonicalName) == langFull)
            {
                break;
            }
        }
    }


    if (i == count)
    {
        // In addition to the format above, we also can have full language
        // names in LANG env var - for example, SuSE is known to use
        // LANG="german" - so check for use of non-standard format and try to
        // find the name in verbose description.
        for (i = 0; i < count; i++)
        {
            if (gs_languagesDB[i].Description.CmpNoCase(langFull) == 0)
            {
                break;
            }
        }
    }
#elif defined(__WIN32__)
    const LANGID langid = ::GetUserDefaultUILanguage();
    if (langid != LOCALE_CUSTOM_UI_DEFAULT)
    {
        wxUint32 lang = PRIMARYLANGID(langid);
        wxUint32 sublang = SUBLANGID(langid);

        for (i = 0; i < count; i++)
        {
            if (gs_languagesDB[i].WinLang == lang &&
                gs_languagesDB[i].WinSublang == sublang)
            {
                break;
            }
        }
    }
    //else: leave wxlang == wxLANGUAGE_UNKNOWN
#endif // Unix/Win32

    if (i < count)
    {
        // we did find a matching entry, use it
        return gs_languagesDB[i].Language;
    }

    // no info about this language in the database
    return wxLANGUAGE_UNKNOWN;
}

/* static */
void wxUILocale::AddLanguage(const wxLanguageInfo& info)
{
    CreateLanguagesDB();
    gs_languagesDB.push_back(info);
}

/* static */
const wxLanguageInfo* wxUILocale::GetLanguageInfo(int lang)
{
    CreateLanguagesDB();

    // calling GetLanguageInfo(wxLANGUAGE_DEFAULT) is a natural thing to do, so
    // make it work
    if (lang == wxLANGUAGE_DEFAULT)
        lang = GetSystemLanguage();

    if (lang == wxLANGUAGE_UNKNOWN)
        return NULL;

    const size_t count = gs_languagesDB.size();
    for (size_t i = 0; i < count; i++)
    {
        if (gs_languagesDB[i].Language == lang)
            return &gs_languagesDB[i];
    }

    return NULL;
}

/* static */
wxString wxUILocale::GetLanguageName(int lang)
{
    wxString string;

    if (lang == wxLANGUAGE_DEFAULT || lang == wxLANGUAGE_UNKNOWN)
        return string;

    const wxLanguageInfo* info = GetLanguageInfo(lang);
    if (info)
        string = info->Description;

    return string;
}

/* static */
wxString wxUILocale::GetLanguageCanonicalName(int lang)
{
    wxString string;

    if (lang == wxLANGUAGE_DEFAULT || lang == wxLANGUAGE_UNKNOWN)
        return string;

    const wxLanguageInfo* info = GetLanguageInfo(lang);
    if (info)
        string = info->CanonicalName;

    return string;
}

/* static */
const wxLanguageInfo* wxUILocale::FindLanguageInfo(const wxString& locale)
{
    CreateLanguagesDB();

    const wxLanguageInfo* infoRet = NULL;

    const size_t count = gs_languagesDB.size();
    for (size_t i = 0; i < count; i++)
    {
        const wxLanguageInfo* info = &gs_languagesDB[i];

        if (wxStricmp(locale, info->CanonicalName) == 0 ||
            wxStricmp(locale, info->Description) == 0)
        {
            // exact match, stop searching
            infoRet = info;
            break;
        }

        if (wxStricmp(locale, info->CanonicalName.BeforeFirst(wxS('_'))) == 0)
        {
            // a match -- but maybe we'll find an exact one later, so continue
            // looking
            //
            // OTOH, maybe we had already found a language match and in this
            // case don't overwrite it because the entry for the default
            // country always appears first in gs_languagesDB
            if (!infoRet)
                infoRet = info;
        }
    }

    return infoRet;
}

// ----------------------------------------------------------------------------
// wxLocale
// ----------------------------------------------------------------------------

/*static*/
void wxLocale::CreateLanguagesDB()
{
    wxUILocale::CreateLanguagesDB();
}

/*static*/
void wxLocale::DestroyLanguagesDB()
{
    wxUILocale::DestroyLanguagesDB();
}

void wxLocale::DoCommonInit()
{
    m_language = wxLANGUAGE_UNKNOWN;

    m_pszOldLocale = NULL;
    m_pOldLocale = NULL;

#ifdef __WIN32__
    m_oldLCID = 0;
#endif

    m_initialized = false;
}

// NB: this function has (desired) side effect of changing current locale
bool wxLocale::Init(const wxString& name,
                    const wxString& shortName,
                    const wxString& locale,
                    bool            bLoadDefault
#if WXWIN_COMPATIBILITY_2_8
                   ,bool            WXUNUSED_UNLESS_DEBUG(bConvertEncoding)
#endif
                    )
{
#if WXWIN_COMPATIBILITY_2_8
    wxASSERT_MSG( bConvertEncoding,
                  wxS("wxLocale::Init with bConvertEncoding=false is no longer supported, add charset to your catalogs") );
#endif

    // change current locale (default: same as long name)
    wxString szLocale(locale);
    if ( szLocale.empty() )
    {
        // the argument to setlocale()
        szLocale = shortName;

        wxCHECK_MSG( !szLocale.empty(), false,
                    wxS("no locale to set in wxLocale::Init()") );
    }

    if ( const wxLanguageInfo* langInfo = FindLanguageInfo(szLocale) )
    {
        // Prefer to use Init(wxLanguage) overload if possible as it will
        // correctly set our m_language and also set the locale correctly under
        // MSW, where just calling wxSetLocale() as we do below is not enough.
        //
        // However don't do it if the parameters are incompatible with this
        // language, e.g. if we are called with something like ("French", "de")
        // to use French locale but German translations: this seems unlikely to
        // happen but, in principle, it could.
        if ( langInfo->CanonicalName.StartsWith(shortName) )
        {
            return Init(langInfo->Language,
                        bLoadDefault ? wxLOCALE_LOAD_DEFAULT : 0);
        }
    }

    // the short name will be used to look for catalog files as well,
    // so we need something here
    wxString strShort(shortName);
    if ( strShort.empty() ) {
        // FIXME I don't know how these 2 letter abbreviations are formed,
        //       this wild guess is surely wrong
        if ( !szLocale.empty() )
        {
            strShort += (wxChar)wxTolower(szLocale[0]);
            if ( szLocale.length() > 1 )
                strShort += (wxChar)wxTolower(szLocale[1]);
        }
    }

    DoInit(name, strShort, wxLANGUAGE_UNKNOWN);

    const bool ret = wxSetlocale(LC_ALL, szLocale) != NULL;

    return DoCommonPostInit(ret, szLocale, shortName, bLoadDefault);
}

void wxLocale::DoInit(const wxString& name,
                      const wxString& shortName,
                      int language)
{
    wxASSERT_MSG( !m_initialized,
                    wxS("you can't call wxLocale::Init more than once") );

    m_initialized = true;
    m_strLocale = name;
    m_strShort = shortName;
    m_language = language;

    // Store the current locale in order to be able to restore it in the dtor.
    m_pszOldLocale = wxSetlocale(LC_ALL, NULL);
    if ( m_pszOldLocale )
        m_pszOldLocale = wxStrdup(m_pszOldLocale);

#ifdef __WIN32__
    m_oldLCID = ::GetThreadLocale();
#endif

    m_pOldLocale = wxSetLocale(this);

    // Set translations object, but only if the user didn't do so yet.
    // This is to preserve compatibility with wx-2.8 where wxLocale was
    // the only API for translations. wxLocale works as a stack, with
    // latest-created one being the active one:
    //     wxLocale loc_fr(wxLANGUAGE_FRENCH);
    //     // _() returns French
    //     {
    //         wxLocale loc_cs(wxLANGUAGE_CZECH);
    //         // _() returns Czech
    //     }
    //     // _() returns French again
    wxTranslations *oldTrans = wxTranslations::Get();
    if ( !oldTrans ||
         (m_pOldLocale && oldTrans == &m_pOldLocale->m_translations) )
    {
        wxTranslations::SetNonOwned(&m_translations);
    }
}

bool wxLocale::DoCommonPostInit(bool success,
                                const wxString& name,
                                const wxString& shortName,
                                bool bLoadDefault)
{
    if ( !success )
    {
        wxLogWarning(_("Cannot set locale to language \"%s\"."), name);

        // As we failed to change locale, there is no need to restore the
        // previous one: it's still valid.
        free(const_cast<char *>(m_pszOldLocale));
        m_pszOldLocale = NULL;

        // continue nevertheless and try to load at least the translations for
        // this language
    }

    wxTranslations *t = wxTranslations::Get();
    if ( t )
    {
        t->SetLanguage(shortName);

        if ( bLoadDefault )
            t->AddStdCatalog();
    }

    return success;
}

bool wxLocale::Init(int lang, int flags)
{
#if WXWIN_COMPATIBILITY_2_8
    wxASSERT_MSG( !(flags & wxLOCALE_CONV_ENCODING),
                  wxS("wxLOCALE_CONV_ENCODING is no longer supported, add charset to your catalogs") );
#endif

    wxCHECK_MSG( lang != wxLANGUAGE_UNKNOWN, false,
                 wxS("Initializing unknown locale doesn't make sense, did you ")
                 wxS("mean to use wxLANGUAGE_DEFAULT perhaps?") );

    wxString name, shortName;

    const wxLanguageInfo *info = GetLanguageInfo(lang);

    // Unknown language:
    if (info == NULL)
    {
        // This could have happened because some concrete language has been
        // requested and we just don't know anything about it. In this case, we
        // have no choice but to simply give up.
        if ( lang != wxLANGUAGE_DEFAULT )
        {
            wxLogError(wxS("Unknown language %i."), lang);
            return false;
        }

        // However in case we didn't recognize the default system language, we
        // can still try to use it, even though we don't know anything about it
        // because setlocale() still might.
    }
    else
    {
        name = info->Description;
        shortName = info->CanonicalName;
    }

    DoInit(name, shortName, lang);

    // Set the locale:

#if defined(__UNIX__) || defined(__WIN32__)

    bool ok = lang == wxLANGUAGE_DEFAULT ? wxUILocale::UseDefault()
                                         : wxUILocale::UseLanguage(*info);

    // Under (non-Darwn) Unix wxUILocale already set the C locale, but under
    // the other platforms we still have to do it here.
#if defined(__WIN32__) || defined(__WXOSX__)

    // We prefer letting the CRT to set its locale on its own when using
    // default locale, as it does a better job of it than we do. We also have
    // to do this when we didn't recognize the default language at all.
    const char *retloc = lang == wxLANGUAGE_DEFAULT ? wxSetlocale(LC_ALL, "")
                                                    : info->TrySetLocale();

#if wxUSE_UNICODE && (defined(__VISUALC__) || defined(__MINGW32__))
    // VC++ setlocale() (also used by Mingw) can't set locale to languages that
    // can only be written using Unicode, therefore wxSetlocale() call fails
    // for such languages but we don't want to report it as an error -- so that
    // at least message catalogs can be used.
    if ( !retloc )
    {
        if ( wxGetANSICodePageForLocale(LOCALE_USER_DEFAULT).empty() )
        {
            // we set the locale to a Unicode-only language, don't treat the
            // inability of CRT to use it as an error
            retloc = "C";
        }
    }
#endif // CRT not handling Unicode-only languages

    if ( !retloc )
        ok = false;

#endif // __WIN32__

    return DoCommonPostInit
           (
                ok,
                name,
                // wxLANGUAGE_DEFAULT needs to be passed to wxTranslations as ""
                // for correct detection of user's preferred language(s)
                lang == wxLANGUAGE_DEFAULT ? wxString() : shortName,
                flags & wxLOCALE_LOAD_DEFAULT
           );
#else // !(__UNIX__ || __WIN32__)
    wxUnusedVar(flags);
    return false;
#endif
}

/*static*/
int wxLocale::GetSystemLanguage()
{
    return wxUILocale::GetSystemLanguage();
}

// ----------------------------------------------------------------------------
// encoding stuff
// ----------------------------------------------------------------------------

// this is a bit strange as under Windows we get the encoding name using its
// numeric value and under Unix we do it the other way round, but this just
// reflects the way different systems provide the encoding info

/* static */
wxString wxLocale::GetSystemEncodingName()
{
    wxString encname;

#if defined(__WIN32__)
    // FIXME: what is the error return value for GetACP()?
    const UINT codepage = ::GetACP();
    switch (codepage)
    {
    case 65001:
        encname = "UTF-8";
        break;

    default:
        encname.Printf(wxS("windows-%u"), codepage);
    }
#elif defined(__WXMAC__)
    encname = wxCFStringRef::AsString(
        CFStringGetNameOfEncoding(CFStringGetSystemEncoding())
    );
#elif defined(__UNIX_LIKE__)

#if defined(HAVE_LANGINFO_H) && defined(CODESET)
    // GNU libc provides current character set this way (this conforms
    // to Unix98)
    char* oldLocale = strdup(setlocale(LC_CTYPE, NULL));
    setlocale(LC_CTYPE, "");
    encname = wxString::FromAscii(nl_langinfo(CODESET));
    setlocale(LC_CTYPE, oldLocale);
    free(oldLocale);

    if (encname.empty())
#endif // HAVE_LANGINFO_H
    {
        // if we can't get at the character set directly, try to see if it's in
        // the environment variables (in most cases this won't work, but I was
        // out of ideas)
        char* lang = getenv("LC_ALL");
        char* dot = lang ? strchr(lang, '.') : NULL;
        if (!dot)
        {
            lang = getenv("LC_CTYPE");
            if (lang)
                dot = strchr(lang, '.');
        }
        if (!dot)
        {
            lang = getenv("LANG");
            if (lang)
                dot = strchr(lang, '.');
        }

        if (dot)
        {
            encname = wxString::FromAscii(dot + 1);
        }
    }
#endif // Win32/Unix

    return encname;
}

/* static */
wxFontEncoding wxLocale::GetSystemEncoding()
{
#if defined(__WIN32__)
    const UINT codepage = ::GetACP();

    switch (codepage)
    {
    case 1250:
    case 1251:
    case 1252:
    case 1253:
    case 1254:
    case 1255:
    case 1256:
    case 1257:
    case 1258:
        return (wxFontEncoding)(wxFONTENCODING_CP1250 + codepage - 1250);

    case 1361:
        return wxFONTENCODING_CP1361;

    case 874:
        return wxFONTENCODING_CP874;

    case 932:
        return wxFONTENCODING_CP932;

    case 936:
        return wxFONTENCODING_CP936;

    case 949:
        return wxFONTENCODING_CP949;

    case 950:
        return wxFONTENCODING_CP950;

    case 65001:
        return wxFONTENCODING_UTF8;
    }
#elif defined(__WXMAC__)
    CFStringEncoding encoding = 0;
    encoding = CFStringGetSystemEncoding();
    return wxMacGetFontEncFromSystemEnc(encoding);
#elif defined(__UNIX_LIKE__) && wxUSE_FONTMAP
    const wxString encname = GetSystemEncodingName();
    if (!encname.empty())
    {
        wxFontEncoding enc = wxFontMapperBase::GetEncodingFromName(encname);

        // on some modern Linux systems (RedHat 8) the default system locale
        // is UTF8 -- but it isn't supported by wxGTK1 in ANSI build at all so
        // don't even try to use it in this case
#if !wxUSE_UNICODE && \
        ((defined(__WXGTK__) && !defined(__WXGTK20__)) || defined(__WXMOTIF__))
        if (enc == wxFONTENCODING_UTF8)
        {
            // the most similar supported encoding...
            enc = wxFONTENCODING_ISO8859_1;
        }
#endif // !wxUSE_UNICODE

        // GetEncodingFromName() returns wxFONTENCODING_DEFAULT for C locale
        // (a.k.a. US-ASCII) which is arguably a bug but keep it like this for
        // backwards compatibility and just take care to not return
        // wxFONTENCODING_DEFAULT from here as this surely doesn't make sense
        if (enc == wxFONTENCODING_DEFAULT)
        {
            // we don't have wxFONTENCODING_ASCII, so use the closest one
            return wxFONTENCODING_ISO8859_1;
        }

        if (enc != wxFONTENCODING_MAX)
        {
            return enc;
        }
        //else: return wxFONTENCODING_SYSTEM below
    }
#endif // Win32/Unix

    return wxFONTENCODING_SYSTEM;
}

/* static */
void wxLocale::AddLanguage(const wxLanguageInfo& info)
{
    wxUILocale::AddLanguage(info);
}

/* static */
const wxLanguageInfo* wxLocale::GetLanguageInfo(int lang)
{
    return wxUILocale::GetLanguageInfo(lang);
}

/* static */
wxString wxLocale::GetLanguageName(int lang)
{
    return wxUILocale::GetLanguageName(lang);
}

/* static */
wxString wxLocale::GetLanguageCanonicalName(int lang)
{
    return wxUILocale::GetLanguageCanonicalName(lang);
}

/* static */
const wxLanguageInfo* wxLocale::FindLanguageInfo(const wxString& locale)
{
    return wxUILocale::FindLanguageInfo(locale);
}

wxString wxLocale::GetSysName() const
{
    return wxSetlocale(LC_ALL, NULL);
}

// clean up
wxLocale::~wxLocale()
{
    // Nothing here needs to be done if the object had never been initialized
    // successfully.
    if ( !m_initialized )
        return;

    // Restore old translations object.
    // See DoCommonInit() for explanation of why this is needed for backward
    // compatibility.
    if ( wxTranslations::Get() == &m_translations )
    {
        if ( m_pOldLocale )
            wxTranslations::SetNonOwned(&m_pOldLocale->m_translations);
        else
            wxTranslations::Set(NULL);
    }

    // restore old locale pointer
    wxSetLocale(m_pOldLocale);

    if ( m_pszOldLocale )
    {
        wxSetlocale(LC_ALL, m_pszOldLocale);
        free(const_cast<char *>(m_pszOldLocale));
    }

#ifdef __WIN32__
    wxUseLCID(m_oldLCID);
#endif
}


// check if the given locale is provided by OS and C run time
/* static */
bool wxLocale::IsAvailable(int lang)
{
    const wxLanguageInfo *info = wxLocale::GetLanguageInfo(lang);
    if ( !info )
    {
        // The language is unknown (this normally only happens when we're
        // passed wxLANGUAGE_DEFAULT), so we can't support it.
        wxASSERT_MSG( lang == wxLANGUAGE_DEFAULT,
                      wxS("No info for a valid language?") );
        return false;
    }

#if defined(__WIN32__)
    if ( !info->WinLang )
        return false;

    if ( !::IsValidLocale(info->GetLCID(), LCID_INSTALLED) )
        return false;

#elif defined(__WXOSX__)
    CFLocaleRef
        cfloc = CFLocaleCreate(kCFAllocatorDefault, wxCFStringRef(info->CanonicalName));
    if ( !cfloc )
        return false;

    CFRelease(cfloc);
#elif defined(__UNIX__)

    // Test if setting the locale works, then set it back.
    char * const oldLocale = wxStrdupA(setlocale(LC_ALL, NULL));

    wxLocaleIdent locId;
    wxString region;
    locId.Language(info->CanonicalName.BeforeFirst('_', &region));
    if ( !region.empty() )
    {
        // We never have encoding in our canonical names, but we can have
        // modifiers, so take them into account.
        wxString mod;
        locId.Region(region.BeforeFirst('@', &mod));

        if ( !mod.empty() )
            locId.Modifier(mod);
    }

    const bool available = wxSetlocaleTryAll(LC_ALL, locId);

    // restore the original locale
    wxSetlocale(LC_ALL, oldLocale);

    free(oldLocale);

    if ( !available )
        return false;
#endif

    return true;
}


bool wxLocale::AddCatalog(const wxString& domain)
{
    wxTranslations *t = wxTranslations::Get();
    if ( !t )
        return false;
    return t->AddCatalog(domain);
}

bool wxLocale::AddCatalog(const wxString& domain, wxLanguage msgIdLanguage)
{
    wxTranslations *t = wxTranslations::Get();
    if ( !t )
        return false;
    return t->AddCatalog(domain, msgIdLanguage);
}

// add a catalog to our linked list
bool wxLocale::AddCatalog(const wxString& szDomain,
                        wxLanguage      msgIdLanguage,
                        const wxString& msgIdCharset)
{
    wxTranslations *t = wxTranslations::Get();
    if ( !t )
        return false;
#if wxUSE_UNICODE
    wxUnusedVar(msgIdCharset);
    return t->AddCatalog(szDomain, msgIdLanguage);
#else
    return t->AddCatalog(szDomain, msgIdLanguage, msgIdCharset);
#endif
}

bool wxLocale::IsLoaded(const wxString& domain) const
{
    wxTranslations *t = wxTranslations::Get();
    if ( !t )
        return false;
    return t->IsLoaded(domain);
}

wxString wxLocale::GetHeaderValue(const wxString& header,
                                  const wxString& domain) const
{
    wxTranslations *t = wxTranslations::Get();
    if ( !t )
        return wxEmptyString;
    return t->GetHeaderValue(header, domain);
}

// ----------------------------------------------------------------------------
// accessors for locale-dependent data
// ----------------------------------------------------------------------------

#if defined(__WINDOWS__) || defined(__WXOSX__)

namespace
{

bool IsAtTwoSingleQuotes(const wxString& fmt, wxString::const_iterator p)
{
    if ( p != fmt.end() && *p == '\'')
    {
        ++p;
        if ( p != fmt.end() && *p == '\'')
        {
            return true;
        }
    }

    return false;
}

} // anonymous namespace

// This function translates from Unicode date formats described at
//
//      http://unicode.org/reports/tr35/tr35-6.html#Date_Format_Patterns
//
// to strftime()-like syntax. This translation is not lossless but we try to do
// our best.

// The function is only exported because it is used in the unit test, it is not
// part of the public API (but it is also used by wxUILocaleImpl).
WXDLLIMPEXP_BASE
wxString wxTranslateFromUnicodeFormat(const wxString& fmt)
{
    wxString fmtWX;
    fmtWX.reserve(fmt.length());

    char chLast = '\0';
    size_t lastCount = 0;

    const char* formatchars =
        "dghHmMsSy"
#ifdef __WINDOWS__
        "t"
#else
        "EcLawD"
#endif
        ;
    for ( wxString::const_iterator p = fmt.begin(); /* end handled inside */; ++p )
    {
        if ( p != fmt.end() )
        {
            if ( *p == chLast )
            {
                lastCount++;
                continue;
            }

            const wxUniChar ch = (*p).GetValue();
            if ( ch.IsAscii() && strchr(formatchars, ch) )
            {
                // these characters come in groups, start counting them
                chLast = ch;
                lastCount = 1;
                continue;
            }
        }

        // interpret any special characters we collected so far
        if ( lastCount )
        {
            switch ( chLast )
            {
                case 'd':
                    switch ( lastCount )
                    {
                        case 1: // d
                        case 2: // dd
                            // these two are the same as we don't distinguish
                            // between 1 and 2 digits for days
                            fmtWX += "%d";
                            break;
#ifdef __WINDOWS__
                        case 3: // ddd
                            fmtWX += "%a";
                            break;

                        case 4: // dddd
                            fmtWX += "%A";
                            break;
#endif
                        default:
                            wxFAIL_MSG( "too many 'd's" );
                    }
                    break;
#ifndef __WINDOWS__
                case 'D':
                    switch ( lastCount )
                    {
                        case 1: // D
                        case 2: // DD
                        case 3: // DDD
                            fmtWX += "%j";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 'D's" );
                    }
                    break;
               case 'w':
                    switch ( lastCount )
                    {
                        case 1: // w
                        case 2: // ww
                            fmtWX += "%W";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 'w's" );
                    }
                    break;
                case 'E':
                   switch ( lastCount )
                    {
                        case 1: // E
                        case 2: // EE
                        case 3: // EEE
                            fmtWX += "%a";
                            break;
                        case 4: // EEEE
                            fmtWX += "%A";
                            break;
                        case 5: // EEEEE
                        case 6: // EEEEEE
                            // no "narrow form" in strftime(), use abbrev.
                            fmtWX += "%a";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 'E's" );
                    }
                    break;
                case 'c':
                    switch ( lastCount )
                {
                    case 1: // c
                        // TODO: unsupported: first day of week as numeric value
                        fmtWX += "1";
                        break;
                    case 3: // ccc
                        fmtWX += "%a";
                        break;
                    case 4: // cccc
                        fmtWX += "%A";
                        break;
                    case 5: // ccccc
                        // no "narrow form" in strftime(), use abbrev.
                        fmtWX += "%a";
                        break;

                    default:
                        wxFAIL_MSG( "wrong number of 'c's" );
                }
                    break;
                case 'L':
                    switch ( lastCount )
                {
                    case 1: // L
                    case 2: // LL
                        fmtWX += "%m";
                        break;

                    case 3: // LLL
                        fmtWX += "%b";
                        break;

                    case 4: // LLLL
                        fmtWX += "%B";
                        break;

                    case 5: // LLLLL
                        // no "narrow form" in strftime(), use abbrev.
                        fmtWX += "%b";
                        break;

                    default:
                        wxFAIL_MSG( "too many 'L's" );
                }
                    break;
#endif
                case 'M':
                    switch ( lastCount )
                    {
                        case 1: // M
                        case 2: // MM
                            // as for 'd' and 'dd' above
                            fmtWX += "%m";
                            break;

                        case 3:
                            fmtWX += "%b";
                            break;

                        case 4:
                            fmtWX += "%B";
                            break;

                        case 5:
                            // no "narrow form" in strftime(), use abbrev.
                            fmtWX += "%b";
                            break;

                        default:
                            wxFAIL_MSG( "too many 'M's" );
                    }
                    break;

                case 'y':
                    switch ( lastCount )
                    {
                        case 1: // y
                        case 2: // yy
                            fmtWX += "%y";
                            break;

                        case 4: // yyyy
                            fmtWX += "%Y";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 'y's" );
                    }
                    break;

                case 'H':
                    switch ( lastCount )
                    {
                        case 1: // H
                        case 2: // HH
                            fmtWX += "%H";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 'H's" );
                    }
                    break;

               case 'h':
                    switch ( lastCount )
                    {
                        case 1: // h
                        case 2: // hh
                            fmtWX += "%I";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 'h's" );
                    }
                    break;

               case 'm':
                    switch ( lastCount )
                    {
                        case 1: // m
                        case 2: // mm
                            fmtWX += "%M";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 'm's" );
                    }
                    break;

               case 's':
                    switch ( lastCount )
                    {
                        case 1: // s
                        case 2: // ss
                            fmtWX += "%S";
                            break;

                        default:
                            wxFAIL_MSG( "wrong number of 's's" );
                    }
                    break;

                case 'g':
                    // strftime() doesn't have era string,
                    // ignore this format
                    wxASSERT_MSG( lastCount <= 2, "too many 'g's" );

                    break;
#ifndef __WINDOWS__
                case 'a':
                    fmtWX += "%p";
                    break;
#endif
#ifdef __WINDOWS__
                case 't':
                    switch ( lastCount )
                    {
                        case 1: // t
                        case 2: // tt
                            fmtWX += "%p";
                            break;

                        default:
                            wxFAIL_MSG( "too many 't's" );
                    }
                    break;
#endif
                default:
                    wxFAIL_MSG( "unreachable" );
            }

            chLast = '\0';
            lastCount = 0;
        }

        if ( p == fmt.end() )
            break;

        /*
        Handle single quotes:
        "Two single quotes represents [sic] a literal single quote, either
        inside or outside single quotes. Text within single quotes is not
        interpreted in any way (except for two adjacent single quotes)."
        */

        if ( IsAtTwoSingleQuotes(fmt, p) )
        {
            fmtWX += '\'';
            ++p; // the 2nd single quote is skipped by the for loop's increment
            continue;
        }

        bool isEndQuote = false;
        if ( *p == '\'' )
        {
            ++p;
            while ( p != fmt.end() )
            {
                if ( IsAtTwoSingleQuotes(fmt, p) )
                {
                    fmtWX += '\'';
                    p += 2;
                    continue;
                }

                if ( *p == '\'' )
                {
                    isEndQuote = true;
                    break;
                }

                fmtWX += *p;
                ++p;
            }
        }

        if ( p == fmt.end() )
            break;

        if ( !isEndQuote )
        {
            // not a special character so must be just a separator, treat as is
            if ( *p == wxT('%') )
            {
                // this one needs to be escaped
                fmtWX += wxT('%');
            }

            fmtWX += *p;
        }
    }

    return fmtWX;
}


#endif // __WINDOWS__ || __WXOSX__

wxString wxGetStdCLocaleInfo(wxLocaleInfo index, wxLocaleCategory WXUNUSED(cat))
{
    switch ( index )
    {
        case wxLOCALE_THOUSANDS_SEP:
            return wxString();

        case wxLOCALE_DECIMAL_POINT:
            return ".";

        case wxLOCALE_SHORT_DATE_FMT:
            return "%m/%d/%y";

        case wxLOCALE_LONG_DATE_FMT:
            return "%A, %B %d, %Y";

        case wxLOCALE_TIME_FMT:
            return "%H:%M:%S";

        case wxLOCALE_DATE_TIME_FMT:
            return "%m/%d/%y %H:%M:%S";

        default:
            wxFAIL_MSG( "unknown wxLocaleInfo" );
    }

    return wxString();
}

#if defined(__WINDOWS__)

// These functions are also used by wxUILocaleImpl, so don't make them private.
extern wxString
wxGetInfoFromLCID(LCID lcid, wxLocaleInfo index, wxLocaleCategory cat);

LCTYPE wxGetLCTYPEFormatFromLocalInfo(wxLocaleInfo index)
{
    switch ( index )
    {
        case wxLOCALE_SHORT_DATE_FMT:
            return LOCALE_SSHORTDATE;

        case wxLOCALE_LONG_DATE_FMT:
            return LOCALE_SLONGDATE;

        case wxLOCALE_TIME_FMT:
            return LOCALE_STIMEFORMAT;

        default:
            wxFAIL_MSG( "no matching LCTYPE" );
    }

    return 0;
}

namespace
{

// This private function additionally checks consistency of the decimal
// separator settings between MSW and CRT.
wxString
GetInfoFromLCID(LCID lcid, wxLocaleInfo index, wxLocaleCategory cat)
{
    const wxString str = wxGetInfoFromLCID(lcid, index, cat);

    if ( !str.empty() && index == wxLOCALE_DECIMAL_POINT )
    {
        // As we get our decimal point separator from Win32 and not the
        // CRT there is a possibility of mismatch between them and this
        // can easily happen if the user code called setlocale()
        // instead of using wxLocale to change the locale. And this can
        // result in very strange bugs elsewhere in the code as the
        // assumptions that formatted strings do use the decimal
        // separator actually fail, so check for it here.
        wxASSERT_MSG
        (
            wxString::Format("%.3f", 1.23).find(str) != wxString::npos,
            "Decimal separator mismatch -- did you use setlocale()?"
            "If so, use wxLocale to change the locale instead."
        );
    }

    return str;
}

} // anonymous namespace

// This function is also used by wxUILocaleImpl, so don't make it private.
wxString
wxGetInfoFromLCID(LCID lcid, wxLocaleInfo index, wxLocaleCategory cat)
{
    wxString str;

    wxChar buf[256];
    buf[0] = wxT('\0');

    switch ( index )
    {
        case wxLOCALE_THOUSANDS_SEP:
            if ( ::GetLocaleInfo(lcid, LOCALE_STHOUSAND, buf, WXSIZEOF(buf)) )
                str = buf;
            break;

        case wxLOCALE_DECIMAL_POINT:
            if ( ::GetLocaleInfo(lcid,
                                 cat == wxLOCALE_CAT_MONEY
                                     ? LOCALE_SMONDECIMALSEP
                                     : LOCALE_SDECIMAL,
                                 buf,
                                 WXSIZEOF(buf)) )
            {
                str = buf;
            }
            break;

        case wxLOCALE_SHORT_DATE_FMT:
        case wxLOCALE_LONG_DATE_FMT:
        case wxLOCALE_TIME_FMT:
            if ( ::GetLocaleInfo(lcid, wxGetLCTYPEFormatFromLocalInfo(index),
                                 buf, WXSIZEOF(buf)) )
            {
                return wxTranslateFromUnicodeFormat(buf);
            }
            break;

        case wxLOCALE_DATE_TIME_FMT:
            // there doesn't seem to be any specific setting for this, so just
            // combine date and time ones
            //
            // we use the short date because this is what "%c" uses by default
            // ("%#c" uses long date but we have no way to specify the
            // alternate representation here)
            {
                const wxString
                    datefmt = wxGetInfoFromLCID(lcid, wxLOCALE_SHORT_DATE_FMT, cat);
                if ( datefmt.empty() )
                    break;

                const wxString
                    timefmt = wxGetInfoFromLCID(lcid, wxLOCALE_TIME_FMT, cat);
                if ( timefmt.empty() )
                    break;

                str << datefmt << ' ' << timefmt;
            }
            break;

        default:
            wxFAIL_MSG( "unknown wxLocaleInfo" );
    }

    return str;
}

/* static */
wxString wxLocale::GetInfo(wxLocaleInfo index, wxLocaleCategory cat)
{
    if ( !wxGetLocale() )
    {
        // wxSetLocale() hadn't been called yet of failed, hence CRT must be
        // using "C" locale -- but check it to detect bugs that would happen if
        // this were not the case.
        wxASSERT_MSG( strcmp(setlocale(LC_ALL, NULL), "C") == 0,
                      wxS("You probably called setlocale() directly instead ")
                      wxS("of using wxLocale and now there is a ")
                      wxS("mismatch between C/C++ and Windows locale.\n")
                      wxS("Things are going to break, please only change ")
                      wxS("locale by creating wxLocale objects to avoid this!") );


        // Return the hard coded values for C locale. This is really the right
        // thing to do as there is no LCID we can use in the code below in this
        // case, even LOCALE_INVARIANT is not quite the same as C locale (the
        // only difference is that it uses %Y instead of %y in the date format
        // but this difference is significant enough).
        return wxGetStdCLocaleInfo(index, cat);
    }

    // wxSetLocale() succeeded and so thread locale was set together with CRT one.
    return GetInfoFromLCID(::GetThreadLocale(), index, cat);
}

/* static */
wxString wxLocale::GetOSInfo(wxLocaleInfo index, wxLocaleCategory cat)
{
    return GetInfoFromLCID(::GetThreadLocale(), index, cat);
}

#elif defined(__WXOSX__)

// This function is also used by wxUILocaleImpl, so don't make it private.
extern wxString
wxGetInfoFromCFLocale(CFLocaleRef cfloc, wxLocaleInfo index, wxLocaleCategory WXUNUSED(cat))
{
    CFStringRef cfstr = 0;
    switch ( index )
    {
        case wxLOCALE_THOUSANDS_SEP:
            cfstr = (CFStringRef) CFLocaleGetValue(cfloc, kCFLocaleGroupingSeparator);
            break;

        case wxLOCALE_DECIMAL_POINT:
            cfstr = (CFStringRef) CFLocaleGetValue(cfloc, kCFLocaleDecimalSeparator);
            break;

        case wxLOCALE_SHORT_DATE_FMT:
        case wxLOCALE_LONG_DATE_FMT:
        case wxLOCALE_DATE_TIME_FMT:
        case wxLOCALE_TIME_FMT:
            {
                CFDateFormatterStyle dateStyle = kCFDateFormatterNoStyle;
                CFDateFormatterStyle timeStyle = kCFDateFormatterNoStyle;
                switch (index )
                {
                    case wxLOCALE_SHORT_DATE_FMT:
                        dateStyle = kCFDateFormatterShortStyle;
                        break;
                    case wxLOCALE_LONG_DATE_FMT:
                        dateStyle = kCFDateFormatterFullStyle;
                        break;
                    case wxLOCALE_DATE_TIME_FMT:
                        dateStyle = kCFDateFormatterFullStyle;
                        timeStyle = kCFDateFormatterMediumStyle;
                        break;
                    case wxLOCALE_TIME_FMT:
                        timeStyle = kCFDateFormatterMediumStyle;
                        break;
                    default:
                        wxFAIL_MSG( "unexpected time locale" );
                        return wxString();
                }
                wxCFRef<CFDateFormatterRef> dateFormatter( CFDateFormatterCreate
                    (NULL, cfloc, dateStyle, timeStyle));
                wxCFStringRef cfs = wxCFRetain( CFDateFormatterGetFormat(dateFormatter ));
                wxString format = wxTranslateFromUnicodeFormat(cfs.AsString());
                // we always want full years
                format.Replace("%y","%Y");
                return format;
            }

        default:
            wxFAIL_MSG( "Unknown locale info" );
            return wxString();
    }

    wxCFStringRef str(wxCFRetain(cfstr));
    return str.AsString();
}

/* static */
wxString wxLocale::GetInfo(wxLocaleInfo index, wxLocaleCategory cat)
{
    CFLocaleRef userLocaleRefRaw;
    if ( wxGetLocale() )
    {
        userLocaleRefRaw = CFLocaleCreate
                        (
                                kCFAllocatorDefault,
                                wxCFStringRef(wxGetLocale()->GetCanonicalName())
                        );
    }
    else // no current locale, use the default one
    {
        userLocaleRefRaw = CFLocaleCopyCurrent();
    }

    wxCFRef<CFLocaleRef> userLocaleRef(userLocaleRefRaw);

    return wxGetInfoFromCFLocale(userLocaleRef, index, cat);
}

#else // !__WINDOWS__ && !__WXOSX__, assume generic POSIX

#ifdef HAVE_LANGINFO_H

wxString wxGetDateFormatOnly(const wxString& fmt)
{
    // this is not 100% precise but the idea is that a typical date/time format
    // under POSIX systems is a combination of a long date format with time one
    // so we should be able to get just the long date format by removing all
    // time-specific format specifiers
    static const char *timeFmtSpecs = "HIklMpPrRsSTXzZ";
    static const char *timeSep = " :./-";

    wxString fmtDateOnly;
    const wxString::const_iterator end = fmt.end();
    wxString::const_iterator lastSep = end;
    for ( wxString::const_iterator p = fmt.begin(); p != end; ++p )
    {
        if ( strchr(timeSep, *p) )
        {
            if ( lastSep == end )
                lastSep = p;

            // skip it for now, we'll discard it if it's followed by a time
            // specifier later or add it to fmtDateOnly if it is not
            continue;
        }

        if ( *p == '%' &&
                (p + 1 != end) && strchr(timeFmtSpecs, p[1]) )
        {
            // time specified found: skip it and any preceding separators
            ++p;
            lastSep = end;
            continue;
        }

        if ( lastSep != end )
        {
            fmtDateOnly += wxString(lastSep, p);
            lastSep = end;
        }

        fmtDateOnly += *p;
    }

    return fmtDateOnly;
}

#endif // HAVE_LANGINFO_H/

/* static */
wxString wxLocale::GetInfo(wxLocaleInfo index, wxLocaleCategory cat)
{
    lconv * const lc = localeconv();
    if ( !lc )
        return wxString();

    switch ( index )
    {
        case wxLOCALE_THOUSANDS_SEP:
            switch ( cat )
            {
                case wxLOCALE_CAT_DEFAULT:
                case wxLOCALE_CAT_NUMBER:
                    return lc->thousands_sep;

                case wxLOCALE_CAT_MONEY:
                    return lc->mon_thousands_sep;

                default:
                    wxFAIL_MSG( "invalid wxLocaleCategory" );
            }
            break;


        case wxLOCALE_DECIMAL_POINT:
            switch ( cat )
            {
                case wxLOCALE_CAT_DEFAULT:
                case wxLOCALE_CAT_NUMBER:
                    return lc->decimal_point;

                case wxLOCALE_CAT_MONEY:
                    return lc->mon_decimal_point;

                default:
                    wxFAIL_MSG( "invalid wxLocaleCategory" );
            }
            break;

#ifdef HAVE_LANGINFO_H
        case wxLOCALE_SHORT_DATE_FMT:
            return nl_langinfo(D_FMT);

        case wxLOCALE_DATE_TIME_FMT:
            return nl_langinfo(D_T_FMT);

        case wxLOCALE_TIME_FMT:
            return nl_langinfo(T_FMT);

        case wxLOCALE_LONG_DATE_FMT:
            return wxGetDateFormatOnly(nl_langinfo(D_T_FMT));
#else // !HAVE_LANGINFO_H
        case wxLOCALE_SHORT_DATE_FMT:
        case wxLOCALE_LONG_DATE_FMT:
        case wxLOCALE_DATE_TIME_FMT:
        case wxLOCALE_TIME_FMT:
            // no fallback, let the application deal with unavailability of
            // nl_langinfo() itself as there is no good way for us to do it (well, we
            // could try to reverse engineer the format from strftime() output but this
            // looks like too much trouble considering the relatively small number of
            // systems without nl_langinfo() still in use)
            break;
#endif // HAVE_LANGINFO_H/!HAVE_LANGINFO_H

        default:
            wxFAIL_MSG( "unknown wxLocaleInfo value" );
    }

    return wxString();
}

#endif // platform

#ifndef __WINDOWS__

/* static */
wxString wxLocale::GetOSInfo(wxLocaleInfo index, wxLocaleCategory cat)
{
    return GetInfo(index, cat);
}

#endif // !__WINDOWS__

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
// wxLocale module (for lazy destruction of languagesDB)
// ----------------------------------------------------------------------------

class wxLocaleModule: public wxModule
{
    wxDECLARE_DYNAMIC_CLASS(wxLocaleModule);
    public:
        wxLocaleModule() {}

        bool OnInit() wxOVERRIDE
        {
            return true;
        }

        void OnExit() wxOVERRIDE
        {
            wxLocale::DestroyLanguagesDB();
        }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxLocaleModule, wxModule);

#endif // wxUSE_INTL
