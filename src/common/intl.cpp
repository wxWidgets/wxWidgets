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
#include "wx/uilocale.h"

#include "wx/private/localeset.h"
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
    // Prefer to use the locale names instead of locale identifiers if
    // supported, both at the OS level (LOCALE_SNAME) and by the CRT (check by
    // calling setlocale()).
    const char* const retloc = wxSetlocale(LC_ALL, GetCanonicalWithRegion());
    if ( retloc )
        return retloc;

    // Fall back to LOCALE_SENGLANGUAGE
    const LCID lcid = GetLCID();

    wxChar buffer[256];
    buffer[0] = wxT('\0');

    if ( !::GetLocaleInfo(lcid, LOCALE_SENGLANGUAGE, buffer, WXSIZEOF(buffer)) )
    {
        wxLogLastError(wxT("GetLocaleInfo(LOCALE_SENGLANGUAGE)"));
        return nullptr;
    }

    wxString locale = buffer;
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
    return wxSetlocale(LC_ALL, GetCanonicalWithRegion());
}

#endif // __WINDOWS__/!__WINDOWS__

wxString wxLanguageInfo::GetLocaleName() const
{
    wxString localeId = GetCanonicalWithRegion();
    wxUILocale uiLocale = wxUILocale::FromTag(localeId);
    wxString localeName = uiLocale.IsSupported() ? uiLocale.GetName() : wxString();
    return localeName;
}

wxString wxLanguageInfo::GetCanonicalWithRegion() const
{
    return CanonicalRef.empty() ? CanonicalName : CanonicalRef;
}

// ----------------------------------------------------------------------------
// wxUILocale
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

/* static */
void wxUILocale::AddLanguage(const wxLanguageInfo& info)
{
    CreateLanguagesDB();
    gs_languagesDB.push_back(info);
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

    m_pszOldLocale = nullptr;
    m_pOldLocale = nullptr;

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
                    )
{
    wxString strName(name);
    wxString strShort(shortName);

    // change current locale (default: same as long name)
    wxString szLocale(locale);
    if ( szLocale.empty() )
    {
        // the argument to setlocale()
        szLocale = shortName;

        wxCHECK_MSG( !szLocale.empty(), false,
                    wxS("no locale to set in wxLocale::Init()") );
    }

    int languageId = wxLANGUAGE_UNKNOWN;
    wxLocaleIdent localeId = wxLocaleIdent::FromTag(szLocale);
    if ( const wxLanguageInfo* langInfo = wxUILocale::FindLanguageInfo(localeId) )
    {
        // Prefer to use Init(wxLanguage) overload if possible as it will
        // correctly set our m_language and also set the locale correctly under
        // MSW, where just calling wxSetLocale() as we do below is not enough.
        //
        // However don't do it if
        //   1. the locale is not empty, or
        //   2. the parameters are incompatible with this language,
        //      e.g. if we are called with something like ("French", "de")
        //      to use French locale but German translations:
        //      this seems unlikely to happen but, in principle, it could.
        if ( locale.empty() && langInfo->CanonicalName.StartsWith(shortName) )
        {
            return Init(langInfo->Language,
                        bLoadDefault ? wxLOCALE_LOAD_DEFAULT : 0);
        }
        else
        {
            strName = langInfo->Description;
            strShort = langInfo->GetCanonicalWithRegion();
            languageId = langInfo->Language;
        }
    }

    // the short name will be used to look for catalog files as well,
    // so we need something here
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

    DoInit(strName, strShort, languageId);

#if defined(__UNIX__) || defined(__WIN32__)
    const wxString oldUILocale = wxUILocale::GetCurrent().GetName();
    bool ok = wxUILocale::UseLocaleName(szLocale);
    if (ok)
    {
        m_oldUILocale = oldUILocale;
    }

    // Under (non-Darwin) Unix wxUILocale already set the C locale, but under
    // the other platforms we still have to do it here.
#if defined(__WIN32__) || defined(__WXOSX__)
    ok = wxSetlocale(LC_ALL, szLocale) != nullptr;
#endif // __WIN32__

#else
    bool ok = false;
#endif

    return DoCommonPostInit(ok, szLocale, strShort, bLoadDefault);
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
    m_pszOldLocale = wxSetlocale(LC_ALL, nullptr);
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
        m_pszOldLocale = nullptr;

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

    // Do this again here in case LC_CTYPE was changed by setlocale().
    wxEnsureLocaleIsCompatibleWithCRT();

    return success;
}

bool wxLocale::Init(int lang, int flags)
{
    wxCHECK_MSG( lang != wxLANGUAGE_UNKNOWN, false,
                 wxS("Initializing unknown locale doesn't make sense, did you ")
                 wxS("mean to use wxLANGUAGE_DEFAULT perhaps?") );

    wxString name, shortName;

    const wxLanguageInfo *info = GetLanguageInfo(lang);

    // Unknown language:
    if (info == nullptr)
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
        shortName = info->GetCanonicalWithRegion();
    }

    DoInit(name, shortName, lang);

    // Set the locale:

#if defined(__UNIX__) || defined(__WIN32__)
    const wxString oldUILocale = wxUILocale::GetCurrent().GetName();

    bool ok = lang == wxLANGUAGE_DEFAULT ? wxUILocale::UseDefault()
                                         : wxUILocale::UseLocaleName(shortName);
    if (ok)
    {
        m_oldUILocale = oldUILocale;
    }

    // Under (non-Darwn) Unix wxUILocale already set the C locale, but under
    // the other platforms we still have to do it here.
#if defined(__WIN32__) || defined(__WXOSX__)

    // We prefer letting the CRT to set its locale on its own when using
    // default locale, as it does a better job of it than we do. We also have
    // to do this when we didn't recognize the default language at all.
    const char *retloc = lang == wxLANGUAGE_DEFAULT ? wxSetlocale(LC_ALL, "")
                                                    : info->TrySetLocale();

#if defined(__VISUALC__) || defined(__MINGW32__)
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
    // Despite the method name wxLocale always determines the system language
    // based on the default user locale (and not the preferred UI language).
    // Therefore we need to call wxUILocale::GetSystemLocale() here.
    return wxUILocale::GetSystemLocale();
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
    {
        TempLocaleSetter setDefautLocale(LC_CTYPE);

        encname = wxString::FromAscii(nl_langinfo(CODESET));
    }

    if (encname.empty())
#endif // HAVE_LANGINFO_H
    {
        // if we can't get at the character set directly, try to see if it's in
        // the environment variables (in most cases this won't work, but I was
        // out of ideas)
        char* lang = getenv("LC_ALL");
        char* dot = lang ? strchr(lang, '.') : nullptr;
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
    // We need to explicitly handle the case "lang == wxLANGUAGE_DEFAULT" here,
    // because wxUILocale::GetLanguageInfo() determines the system language
    // based on the preferred UI language while wxLocale uses the default
    // user locale for that purpose.
    //
    // Note that even though wxUILocale::GetLanguageInfo() seems to do the same
    // thing as we do here, it actually does _not_ because we're calling our
    // GetSystemLanguage() which maps to wxUILocale::GetSystemLocale() and not
    // the function with the same name in that class. This is incredibly
    // confusing but necessary for backwards compatibility.
    if (lang == wxLANGUAGE_DEFAULT)
        lang = GetSystemLanguage();
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
    return wxSetlocale(LC_ALL, nullptr);
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
            wxTranslations::Set(nullptr);
    }

    // restore old locale pointer
    wxSetLocale(m_pOldLocale);

    // and old current wxUILocale
    if (!m_oldUILocale.empty())
    {
        wxUILocale::UseLocaleName(m_oldUILocale);
    }

    if ( m_pszOldLocale )
    {
        wxSetlocale(LC_ALL, m_pszOldLocale);
        free(const_cast<char *>(m_pszOldLocale));
    }
}


// check if the given locale is provided by OS and C run time
/* static */
bool wxLocale::IsAvailable(int lang)
{
    const wxLanguageInfo *info = wxLocale::GetLanguageInfo(lang);
    if ( !info )
    {
        // This must be wxLANGUAGE_DEFAULT as otherwise we should have found
        // the matching entry.
        wxCHECK_MSG( lang == wxLANGUAGE_DEFAULT, false,
                     wxS("No info for a valid language?") );

        // For this one, we need to check whether using it later is going to
        // actually work, i.e. if the CRT supports it.
        const char* const origLocale = wxSetlocale(LC_ALL, nullptr);
        if ( !origLocale )
        {
            // This is not supposed to happen, we should always be able to
            // query the current locale, but don't crash if it does.
            return false;
        }

        // Make a copy of the string because wxSetlocale() call below may
        // change the buffer to which it points.
        const wxString origLocaleStr = wxString::FromUTF8(origLocale);

        if ( !wxSetlocale(LC_ALL, "") )
        {
            // Locale wasn't changed, so nothing else to do.
            return false;
        }

        // We support this locale, but restore the original one before
        // returning.
        wxSetlocale(LC_ALL, origLocaleStr.utf8_str());

        return true;
    }

    wxString localeTag = info->GetCanonicalWithRegion();
    wxUILocale uiLocale(wxLocaleIdent::FromTag(localeTag));

    return uiLocale.IsSupported();
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
    wxUnusedVar(msgIdCharset);
    return t->AddCatalog(szDomain, msgIdLanguage);
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

/* static */
wxString wxLocale::GetInfo(wxLocaleInfo index, wxLocaleCategory cat)
{
    return wxUILocale::GetCurrent().GetInfo(index, cat);
}

/* static */
wxString wxLocale::GetOSInfo(wxLocaleInfo index, wxLocaleCategory cat)
{
    return wxUILocale::GetCurrent().GetInfo(index, cat);
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
                    (nullptr, cfloc, dateStyle, timeStyle));
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
static wxLocale *g_pLocale = nullptr;

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

        bool OnInit() override
        {
            return true;
        }

        void OnExit() override
        {
            wxLocale::DestroyLanguagesDB();
        }
};

wxIMPLEMENT_DYNAMIC_CLASS(wxLocaleModule, wxModule);

#endif // wxUSE_INTL
