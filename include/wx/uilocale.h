///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uilocale.h
// Purpose:     wxUILocale class declaration.
// Author:      Vadim Zeitlin
// Created:     2021-07-31
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UILOCALE_H_
#define _WX_UILOCALE_H_

#include "wx/defs.h"

#if wxUSE_INTL

#include "wx/datetime.h"
#include "wx/localedefs.h"
#include "wx/string.h"
#include "wx/vector.h"

class wxUILocaleImpl;

// Flags for wxUILocale::CompareStrings().
enum
{
    wxCompare_CaseSensitive   = 0,
    wxCompare_CaseInsensitive = 1
};

// Flags for wxLocaleIdent::RemoveLikelySubtags().
enum
{
    wxSubtags_FavourRegion = 0,
    wxSubtags_FavourScript = 1
};

// ----------------------------------------------------------------------------
// wxLocaleIdent: allows to fully identify a locale under all platforms
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxLocaleIdent
{
public:
    // Create the object from BCP 47-like language tag: the string must contain
    // at least the language part (2 or 3 ASCII letters) and may contain script
    // and region separated by dashes.
    static wxLocaleIdent FromTag(const wxString& tag);

    // Default ctor creates an empty, invalid identifier.
    wxLocaleIdent() = default;

    // Set language
    wxLocaleIdent& Language(const wxString& language);

    // Set region
    wxLocaleIdent& Region(const wxString& region);

    // Set script (not supported and ignored under Unix)
    wxLocaleIdent& Script(const wxString& script);

    // Set charset (only supported under Unix)
    wxLocaleIdent& Charset(const wxString& charset);

    // Set modifier (only supported under Unix)
    wxLocaleIdent& Modifier(const wxString& modifier);

    // Set extension (only supported under Windows)
    wxLocaleIdent& Extension(const wxString& extension);

    // Set sort order (only supported under Windows)
    wxLocaleIdent& SortOrder(const wxString& sortorder);

    // Accessors for the individual fields.
    const wxString& GetLanguage() const { return m_language; }
    const wxString& GetRegion() const { return m_region; }
    const wxString& GetScript() const { return m_script; }
    const wxString& GetCharset() const { return m_charset; }
    const wxString& GetModifier() const { return m_modifier; }
    const wxString& GetExtension() const { return m_extension; }
    const wxString& GetSortorder() const { return m_sortorder; }

    // Construct platform dependent name
    wxString GetName() const;

    // Get the language tag: for the objects created with FromTag() returns the
    // string passed to it directly, otherwise reconstructs this string from
    // the components.
    wxString GetTag(wxLocaleTagType tagType = wxLOCALE_TAGTYPE_DEFAULT) const;

    // Empty locale identifier is invalid. at least Language() must be called.
    bool IsEmpty() const
    {
        return m_language.empty();
    }

    // Methods for internal use only
    // Find the best match between desired and supported languages/locales.
    static wxString GetBestMatch(const wxVector<wxString>& desired, const wxVector<wxString>& supported);
    static wxString GetBestMatch(const wxString& desired, const wxVector<wxString>& supported);

private:
    wxString m_tag;

    wxString m_language;
    wxString m_region;
    wxString m_script;
    wxString m_charset;
    wxString m_modifier;
    wxString m_extension;
    wxString m_sortorder;

    // Add likely subtags to a given locale identifier.
    static wxLocaleIdent AddLikelySubtags(const wxLocaleIdent& localeIdent);

    // Remove likely subtags from a given locale identifier, favor region.
    static wxLocaleIdent RemoveLikelySubtags(const wxLocaleIdent& localeIdent, int subtagsFavour = wxSubtags_FavourRegion);
};

// ----------------------------------------------------------------------------
// wxUILocale allows to use the default UI locale and get information about it
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxUILocale
{
public:
    // Configure the UI to use the default user locale.
    static bool UseDefault();

    // Use the locale corresponding to the given POSIX locale, e.g. "de_DE.UTF-8".
    //
    // This is a compatibility function used by wxWidgets itself, don't use it
    // in the new code.
    static bool UseLocaleName(const wxString& localeName);

    // Get the object corresponding to the currently used locale.
    static const wxUILocale& GetCurrent();

    // A helper just to avoid writing wxUILocale(wxLocaleIdent::FromTag(...)).
    static wxUILocale FromTag(const wxString& tag)
    {
        return wxUILocale(wxLocaleIdent::FromTag(tag));
    }

    // Create the object corresponding to the given locale.
    explicit wxUILocale(const wxLocaleIdent& localeId);

    // Objects of this class can be (cheaply) copied.
    wxUILocale(const wxUILocale& loc);
    wxUILocale& operator=(const wxUILocale& loc);

    // Check if the locale is actually supported by the current system: if it's
    // not supported, the other functions will behave as for the "C" locale.
    bool IsSupported() const;

    // Get the platform-dependent name of the current locale.
    wxString GetName() const;

    // Get the locale id from which the current locale was instantiated.
    wxLocaleIdent GetLocaleId() const;

    // Query the locale for the specified information.
    wxString GetInfo(wxLocaleInfo index,
                     wxLocaleCategory cat = wxLOCALE_CAT_DEFAULT) const;

    // Query the locale for the specified localized name.
    wxString GetLocalizedName(wxLocaleName name, wxLocaleForm form) const;

#if wxUSE_DATETIME
    // Get the full (default) or abbreviated localized month name
    // returns empty string on error
    wxString GetMonthName(wxDateTime::Month month,
                          wxDateTime::NameForm form = {}) const;

    // Get the full (default) or abbreviated localized weekday name
    // returns empty string on error
    wxString GetWeekDayName(wxDateTime::WeekDay weekday,
                            wxDateTime::NameForm form = {}) const;
#endif // wxUSE_DATETIME

    // Query the layout direction of the current locale.
    wxLayoutDirection GetLayoutDirection() const;

    // Compares two strings in the order defined by this locale.
    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags = wxCompare_CaseSensitive) const;

    // Note that this class is not supposed to be used polymorphically, hence
    // its dtor is not virtual.
    ~wxUILocale();

    // Return the locale ID representing the default system locale, which would
    // be set is UseDefault() is called.
    static wxLocaleIdent GetSystemLocaleId();

    // Try to get user's (or OS's) preferred language setting.
    // Return wxLANGUAGE_UNKNOWN if the language-guessing algorithm failed
    // Prefer using GetSystemLocaleId() above.
    static int GetSystemLanguage();

    // Try to get user's (or OS's) default locale setting.
    // Return wxLANGUAGE_UNKNOWN if the locale-guessing algorithm failed
    // Prefer using GetSystemLocaleId() above.
    static int GetSystemLocale();

    // Try to retrieve a list of user's (or OS's) preferred UI languages.
    // Return empty list if language-guessing algorithm failed
    static wxVector<wxString> GetPreferredUILanguages();

    // Retrieve the language info struct for the given language
    //
    // Returns nullptr if no info found, pointer must *not* be deleted by caller
    static const wxLanguageInfo* GetLanguageInfo(int lang);

    // Returns language name in English or empty string if the language
    // is not in database
    static wxString GetLanguageName(int lang);

    // Returns ISO code ("canonical name") of language or empty string if the
    // language is not in database
    static wxString GetLanguageCanonicalName(int lang);

    // Find the language for the given locale string which may be either a
    // canonical ISO 2 letter language code ("xx"), a language code followed by
    // the country code ("xx_XX") or a Windows full language name ("Xxxxx...")
    //
    // Returns nullptr if no info found, pointer must *not* be deleted by caller
    static const wxLanguageInfo* FindLanguageInfo(const wxString& locale);

    // Find the language for the given locale string which may be either a
    // canonical ISO 2 letter language code ("xx"), a language code followed by
    // the country code ("xx_XX") or a Windows full language name ("Xxxxx...")
    //
    // Returns nullptr if no info found, pointer must *not* be deleted by caller
    static const wxLanguageInfo* FindLanguageInfo(const wxLocaleIdent& locId);

    // Add custom language to the list of known languages.
    // Notes: 1) wxLanguageInfo contains platform-specific data
    //        2) must be called before Init to have effect
    static void AddLanguage(const wxLanguageInfo& info);

private:
    // This ctor is private and exists only for implementation reasons.
    // It takes ownership of the provided pointer.
    explicit wxUILocale(wxUILocaleImpl* impl = nullptr) : m_impl(impl) { }

    static wxUILocale ms_current;

    wxUILocaleImpl* m_impl;
};

inline wxString wxGetUIDateFormat()
{
    return wxUILocale::GetCurrent().GetInfo(wxLOCALE_SHORT_DATE_FMT);
}

#else // !wxUSE_INTL

inline wxString wxGetUIDateFormat()
{
    return wxString(wxS("%x"));
}

#endif // wxUSE_INTL/!wxUSE_INTL

#endif // _WX_UILOCALE_H_
