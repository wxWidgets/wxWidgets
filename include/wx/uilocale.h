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

#include "wx/localedefs.h"
#include "wx/string.h"

class wxUILocaleImpl;

// Flags for wxUILocale::CompareStrings().
enum
{
    wxCompare_CaseSensitive   = 0,
    wxCompare_CaseInsensitive = 1
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
    wxLocaleIdent() { }

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

    // Accessors for the individual fields.
    const wxString& GetLanguage() const { return m_language; }
    const wxString& GetRegion() const { return m_region; }
    const wxString& GetScript() const { return m_script; }
    const wxString& GetCharset() const { return m_charset; }
    const wxString& GetModifier() const { return m_modifier; }

    // Construct platform dependent name
    wxString GetName() const;

    // Get the language tag: for the objects created with FromTag() returns the
    // string passed to it directly, otherwise reconstructs this string from
    // the components.
    wxString GetTag() const;

    // Empty locale identifier is invalid. at least Language() must be called.
    bool IsEmpty() const
    {
        return m_language.empty();
    }

private:
    wxString m_tag;

    wxString m_language;
    wxString m_region;
    wxString m_script;
    wxString m_charset;
    wxString m_modifier;
};

// ----------------------------------------------------------------------------
// wxUILocale allows to use the default UI locale and get information about it
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxUILocale
{
public:
    // Configure the UI to use the default user locale.
    static bool UseDefault();

    // Use the locale corresponding to the given language.
    //
    // This is a compatibility function used by wxWidgets itself, don't use it
    // in the new code.
    static bool UseLanguage(const wxLanguageInfo& info);

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

    // Query the locale for the specified information.
    wxString GetInfo(wxLocaleInfo index,
                     wxLocaleCategory cat = wxLOCALE_CAT_DEFAULT) const;

    // Compares two strings in the order defined by this locale.
    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags = wxCompare_CaseSensitive) const;

    // Note that this class is not supposed to be used polymorphically, hence
    // its dtor is not virtual.
    ~wxUILocale();

    // Try to get user's (or OS's) preferred language setting.
    // Return wxLANGUAGE_UNKNOWN if language-guessing algorithm failed
    static int GetSystemLanguage();

    // Retrieve the language info struct for the given language
    //
    // Returns NULL if no info found, pointer must *not* be deleted by caller
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
    // Returns NULL if no info found, pointer must *not* be deleted by caller
    static const wxLanguageInfo* FindLanguageInfo(const wxString& locale);

    // Add custom language to the list of known languages.
    // Notes: 1) wxLanguageInfo contains platform-specific data
    //        2) must be called before Init to have effect
    static void AddLanguage(const wxLanguageInfo& info);

    // These two methods are for internal use only. First one creates the
    // global language database if it doesn't already exist, second one destroys
    // it.
    static void CreateLanguagesDB();
    static void DestroyLanguagesDB();

    // These two methods are for internal use only.
    // wxLocaleIdent expects script identifiers as listed in ISO 15924.
    // However, directory names for translation catalogs follow the
    // Unix convention, using script aliases as listed  in ISO 15924.
    // First one converts a script name to its alias, second converts
    // a script alias to its corresponding script name.
    // Both methods return empty strings, if the script name or alias
    // couldn't be found.
    static wxString GetScriptAliasFromName(const wxString& scriptName);
    static wxString GetScriptNameFromAlias(const wxString& scriptAlias);

private:
    // This ctor is private and exists only for implementation reasons.
    // It takes ownership of the provided pointer.
    explicit wxUILocale(wxUILocaleImpl* impl = NULL) : m_impl(impl) { }

    // Creates the global tables of languages and scripts called by CreateLanguagesDB
    static void InitLanguagesDB();

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
