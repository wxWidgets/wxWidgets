/////////////////////////////////////////////////////////////////////////////
// Name:        wx/translation.h
// Purpose:     Internationalization and localisation for wxWidgets
// Author:      Vadim Zeitlin, Vaclav Slavik,
//              Michael N. Filippov <michael@idisys.iae.nsk.su>
// Created:     2010-04-23
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
//              (c) 2010 Vaclav Slavik <vslavik@fastmail.fm>
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TRANSLATION_H_
#define _WX_TRANSLATION_H_

#include "wx/defs.h"
#include "wx/string.h"

#if wxUSE_INTL

#include "wx/buffer.h"
#include "wx/language.h"
#include "wx/strconv.h"

// This is a hack, but this header used to include wx/hashmap.h which, in turn,
// included wx/wxcrt.h and it turns out quite some existing code relied on it
// by using the CRT wrapper functions declared there without explicitly
// including that header, so keep including it from here to let it continue to
// compile.
#include "wx/wxcrt.h"

#include <memory>
#include <unordered_map>

using wxTranslationsHashMap = std::unordered_map<wxString, wxString>;

// ============================================================================
// global decls
// ============================================================================

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// gettext() style macros (notice that xgettext should be invoked with
// --keyword="_" --keyword="wxPLURAL:1,2" options
// to extract the strings from the sources)
#ifndef WXINTL_NO_GETTEXT_MACRO
    #define _(s)                               wxUnderscoreWrapper((s))
#endif

#define wxPLURAL(sing, plur, n)                wxPluralWrapper((sing), (plur), n)

// wx-specific macro for translating strings in the given context: if you use
// them, you need to also add
// --keyword="wxGETTEXT_IN_CONTEXT:1c,2" --keyword="wxGETTEXT_IN_CONTEXT_PLURAL:1c,2,3"
// options to xgettext invocation.
#define wxGETTEXT_IN_CONTEXT(c, s) \
    wxGettextInContextWrapper((c), (s))
#define wxGETTEXT_IN_CONTEXT_PLURAL(c, sing, plur, n) \
    wxGettextInContextPluralWrapper((c), (sing), (plur), (n))

// another one which just marks the strings for extraction, but doesn't
// perform the translation (use -kwxTRANSLATE with xgettext!)
#define wxTRANSLATE(str) str

// another one which just marks the strings, with a context, for extraction,
// but doesn't perform the translation (use -kwxTRANSLATE_IN_CONTEXT:1c,2 with
// xgettext!)
#define wxTRANSLATE_IN_CONTEXT(c, str) str

// ----------------------------------------------------------------------------
// forward decls
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_FWD_BASE wxArrayString;
class WXDLLIMPEXP_FWD_BASE wxTranslationsLoader;
class WXDLLIMPEXP_FWD_BASE wxLocale;

class wxPluralFormsCalculator;
using wxPluralFormsCalculatorPtr = std::unique_ptr<wxPluralFormsCalculator>;

// ----------------------------------------------------------------------------
// wxMsgCatalog corresponds to one loaded message catalog.
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_BASE wxMsgCatalog
{
public:
    // Ctor is protected, because CreateFromXXX functions must be used,
    // but destruction should be unrestricted
    ~wxMsgCatalog();

    // load the catalog from disk or from data; caller is responsible for
    // deleting them if not null
    static wxMsgCatalog *CreateFromFile(const wxString& filename,
                                        const wxString& domain);

    static wxMsgCatalog *CreateFromData(const wxScopedCharBuffer& data,
                                        const wxString& domain);

    // get name of the catalog
    wxString GetDomain() const { return m_domain; }

    // get the translated string: returns nullptr if not found
    const wxString *GetString(const wxString& sz, unsigned n = UINT_MAX, const wxString& ct = wxEmptyString) const;

protected:
    wxMsgCatalog(const wxString& domain);

private:
    // variable pointing to the next element in a linked list (or nullptr)
    wxMsgCatalog *m_pNext;
    friend class wxTranslations;

    wxTranslationsHashMap   m_messages; // all messages in the catalog
    wxString                m_domain;   // name of the domain

    wxPluralFormsCalculatorPtr m_pluralFormsCalculator;
};

// ----------------------------------------------------------------------------
// wxTranslations: message catalogs
// ----------------------------------------------------------------------------

// this class allows to get translations for strings
class WXDLLIMPEXP_BASE wxTranslations
{
public:
    wxTranslations();
    ~wxTranslations();

    // returns current translations object, may return nullptr
    static wxTranslations *Get();
    // sets current translations object (takes ownership; may be null)
    static void Set(wxTranslations *t);

    // changes loader to non-default one; takes ownership of 'loader'
    void SetLoader(wxTranslationsLoader *loader);

    void SetLanguage(wxLanguage lang);
    void SetLanguage(const wxString& lang);

    // get languages available for this app
    wxArrayString GetAvailableTranslations(const wxString& domain) const;

    // find best available translation language for given domain
    wxString GetBestAvailableTranslation(const wxString& domain);

    wxString GetBestTranslation(const wxString& domain, wxLanguage msgIdLanguage);
    wxString GetBestTranslation(const wxString& domain,
                                const wxString& msgIdLanguage = wxASCII_STR("en"));

    // add catalog for the given domain returning true if it could be found by
    // wxTranslationsLoader
    bool AddAvailableCatalog(const wxString& domain, wxLanguage msgIdLanguage = wxLANGUAGE_ENGLISH_US);

    // add standard wxWidgets catalog ("wxstd")
    bool AddStdCatalog();

    // add catalog with given domain name and language, looking it up via
    // wxTranslationsLoader -- unlike AddAvailableCatalog(), this function also
    // returns true if this catalog is not needed at all because msgIdLanguage
    // is an acceptable language to use directly
    bool AddCatalog(const wxString& domain,
                    wxLanguage msgIdLanguage = wxLANGUAGE_ENGLISH_US);

    // check if the given catalog is loaded
    bool IsLoaded(const wxString& domain) const;

    // access to translations
    const wxString *GetTranslatedString(const wxString& origString,
                                        const wxString& domain = wxEmptyString,
                                        const wxString& context = wxEmptyString) const;
    const wxString *GetTranslatedString(const wxString& origString,
                                        unsigned n,
                                        const wxString& domain = wxEmptyString,
                                        const wxString& context = wxEmptyString) const;

    wxString GetHeaderValue(const wxString& header,
                            const wxString& domain = wxEmptyString) const;


    wxDEPRECATED_INLINE(
        static const wxString& GetUntranslatedString(const wxString& str),
        return str;
    )

private:
    enum class Translations
    {
      NotNeeded = -1,
      NotFound = 0,
      Found = 1
    };

    Translations DoAddCatalog(const wxString& domain, wxLanguage msgIdLanguage);

    // perform loading of the catalog via m_loader
    bool LoadCatalog(const wxString& domain, const wxString& lang);

    // find catalog by name in a linked list, return nullptr if !found
    wxMsgCatalog *FindCatalog(const wxString& domain) const;

    // same as Set(), without taking ownership; only for wxLocale
    static void SetNonOwned(wxTranslations *t);
    friend class wxLocale;

    wxString DoGetBestAvailableTranslation(const wxString& domain, const wxString& additionalAvailableLanguage);

private:
    wxString m_lang;
    wxTranslationsLoader *m_loader;

    wxMsgCatalog *m_pMsgCat; // pointer to linked list of catalogs

    // In addition to keeping all the catalogs in the linked list, we also
    // store them in a hash map indexed by the domain name to allow finding
    // them by name efficiently.
    using wxMsgCatalogMap = std::unordered_map<wxString, wxMsgCatalog*>;
    wxMsgCatalogMap m_catalogMap;
};


// abstraction of translations discovery and loading
class WXDLLIMPEXP_BASE wxTranslationsLoader
{
public:
    wxTranslationsLoader() = default;
    virtual ~wxTranslationsLoader() = default;

    virtual wxMsgCatalog *LoadCatalog(const wxString& domain,
                                      const wxString& lang) = 0;

    virtual wxArrayString GetAvailableTranslations(const wxString& domain) const = 0;
};


// standard wxTranslationsLoader implementation, using filesystem
class WXDLLIMPEXP_BASE wxFileTranslationsLoader
    : public wxTranslationsLoader
{
public:
    static void AddCatalogLookupPathPrefix(const wxString& prefix);

    virtual wxMsgCatalog *LoadCatalog(const wxString& domain,
                                      const wxString& lang) override;

    virtual wxArrayString GetAvailableTranslations(const wxString& domain) const override;
};


#ifdef __WINDOWS__
// loads translations from win32 resources
class WXDLLIMPEXP_BASE wxResourceTranslationsLoader
    : public wxTranslationsLoader
{
public:
    virtual wxMsgCatalog *LoadCatalog(const wxString& domain,
                                      const wxString& lang) override;

    virtual wxArrayString GetAvailableTranslations(const wxString& domain) const override;

protected:
    // returns resource type to use for translations
    virtual wxString GetResourceType() const { return wxASCII_STR("MOFILE"); }

    // returns module to load resources from
    virtual WXHINSTANCE GetModule() const { return nullptr; }
};
#endif // __WINDOWS__


// ----------------------------------------------------------------------------
// global functions
// ----------------------------------------------------------------------------

// get the translation of the string in the current locale
inline wxString wxGetTranslation(const wxString& str,
                                 const wxString& domain = wxString(),
                                 const wxString& context = wxString())
{
    wxTranslations *trans = wxTranslations::Get();
    const wxString *transStr = trans ? trans->GetTranslatedString(str, domain, context)
                                     : nullptr;
    if ( transStr )
        return *transStr;
    else
        return str;
}

inline wxString wxGetTranslation(const wxString& str1,
                                 const wxString& str2,
                                 unsigned n,
                                 const wxString& domain = wxString(),
                                 const wxString& context = wxString())
{
    wxTranslations *trans = wxTranslations::Get();
    const wxString *transStr = trans ? trans->GetTranslatedString(str1, n, domain, context)
                                     : nullptr;
    if ( transStr )
        return *transStr;
    else
        return n == 1 ? str1 : str2;
}

#ifdef wxNO_IMPLICIT_WXSTRING_ENCODING

/*
 * It must always be possible to call wxGetTranslation() with const
 * char* arguments.
 */

inline wxString wxGetTranslation(const char *str,
                                 const char *domain = "",
                                 const char *context = "") {
    const wxMBConv &conv = wxConvWhateverWorks;
    return wxGetTranslation(wxString(str, conv), wxString(domain, conv),
                            wxString(context, conv));
}

inline wxString wxGetTranslation(const char *str1,
                                 const char *str2,
                                 unsigned n,
                                 const char *domain = "",
                                 const char *context = "") {
    const wxMBConv &conv = wxConvWhateverWorks;
    return wxGetTranslation(wxString(str1, conv), wxString(str2, conv), n,
                            wxString(domain, conv),
                            wxString(context, conv));
}

// We can't construct wxString implicitly in this case, so use a helper.
inline wxString wxTRANS_INPUT_STR(const char* s)
{
    return wxString::FromAscii(s);
}

inline wxString wxTRANS_INPUT_STR(const wchar_t* s)
{
    return wxString(s);
}
#else // !wxNO_IMPLICIT_WXSTRING_ENCODING
    // We can rely on implicit conversion, so don't bother with the helper.
    #define wxTRANS_INPUT_STR(s) s
#endif // wxNO_IMPLICIT_WXSTRING_ENCODING

#ifndef wxNO_REQUIRE_LITERAL_MSGIDS

// Wrapper functions that only accept string literals as arguments,
// not variables, not char* pointers, and define the fall backs only in
// order to point out to the comment below:

/*
    *** LITERALS-MSGID comment marker ***

    This comment explains the reason for static assert failures below. Please
    see https://wxwidgets.org/help/msgid-literals for a more detailed and
    possibly more up-to-date version of this comment.

    If you get a compile error when using any of the translation macros, i.e.
    _(), wxPLURAL() etc, it means that you're passing something other than a
    literal string, i.e. just simple "whatever", to wx translation functions.
    This most likely indicates a bug in your program which is now detected when
    it was silently ignored before and should be fixed by changing the code to
    use string literals. For example, existing code doing

        _(wxString::Format("Hello %s", who))

    wouldn't work properly even if it were allowed to compile and should be
    changed to

        wxString::Format(_("Hello %s"), who))

    However if you can't do this, for some reason, you may choose to predefine
    wxNO_REQUIRE_LITERAL_MSGIDS which disables these checks. Please note that
    this is *NOT* recommended, as the problematic strings still won't be
    translated, because they won't have been extracted by xgettext in the first
    place and the right thing to do remains to fix the code instead.
*/

template<size_t N, typename T>
wxString wxUnderscoreWrapper(const T (&msg)[N])
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg));
}

template <typename T>
wxString wxUnderscoreWrapper(T)
{
    static_assert(!sizeof(T), "See https://wxwidgets.org/help/msgid-literals or LITERALS-MSGID comment above.");
    return {};
}

template<size_t M, size_t N, typename T>
wxString wxPluralWrapper(const T (&msg)[M],
                         const T (&plural)[N],
                         int count)
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg), wxTRANS_INPUT_STR(plural),
                            count);
}

template <typename T, typename U>
wxString wxPluralWrapper(T, U, int)
{
    static_assert(!sizeof(T), "See https://wxwidgets.org/help/msgid-literals or LITERALS-MSGID comment above.");
    return {};
}

template<size_t M, size_t N, typename T>
wxString wxGettextInContextWrapper(const T (&ctx)[M],
                                   const T (&msg)[N])
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg), wxString(),
                            wxTRANS_INPUT_STR(ctx));
}

template <typename T, typename U>
wxString wxGettextInContextWrapper(T, U)
{
    static_assert(!sizeof(T), "See https://wxwidgets.org/help/msgid-literals or LITERALS-MSGID comment above.");
    return {};
}

template<size_t L, size_t M, size_t N, typename T>
wxString wxGettextInContextPluralWrapper(const T (&ctx)[L],
                                         const T (&msg)[M],
                                         const T (&plural)[N],
                                         int count)
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg), wxTRANS_INPUT_STR(plural),
                            count, wxString(), wxTRANS_INPUT_STR(ctx));
}

template <typename T, typename U, typename V>
wxString wxGettextInContextPluralWrapper(T, U, V, int)
{
    static_assert(!sizeof(T), "See https://wxwidgets.org/help/msgid-literals or LITERALS-MSGID comment above.");
    return {};
}

#else // wxNO_REQUIRE_LITERAL_MSGIDS

// Wrapper functions that accept both string literals and variables
// as arguments.
inline wxString wxUnderscoreWrapper(const char *msg)
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg));
}

inline wxString wxPluralWrapper(const char *msg,
                                const char *plural,
                                int count)
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg), wxTRANS_INPUT_STR(plural),
                            count);
}

inline wxString wxGettextInContextWrapper(const char *ctx,
                                          const char *msg)
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg), wxString(),
                            wxTRANS_INPUT_STR(ctx));
}

inline wxString wxGettextInContextPluralWrapper(const char *ctx,
                                                const char *msg,
                                                const char *plural,
                                                int count)
{
    return wxGetTranslation(wxTRANS_INPUT_STR(msg), wxTRANS_INPUT_STR(plural),
                            count, wxString(), wxTRANS_INPUT_STR(ctx));
}

#endif // wxNO_REQUIRE_LITERAL_MSGIDS

#else // !wxUSE_INTL

// the macros should still be defined - otherwise compilation would fail

#if !defined(WXINTL_NO_GETTEXT_MACRO)
    #if !defined(_)
#ifndef wxNO_IMPLICIT_WXSTRING_ENCODING
        #define _(s)                 (s)
#else
        #define _(s)                 wxASCII_STR(s)
#endif
    #endif
    #define wxPLURAL(sing, plur, n)  ((n) == 1 ? (sing) : (plur))
    #define wxGETTEXT_IN_CONTEXT(c, s)                     (s)
    #define wxGETTEXT_IN_CONTEXT_PLURAL(c, sing, plur, n)  wxPLURAL(sing, plur, n)
#endif

#define wxTRANSLATE(str) str
#define wxTRANSLATE_IN_CONTEXT(c, str) str

// NB: we use a template here in order to avoid using
//     wxLocale::GetUntranslatedString() above, which would be required if
//     we returned const wxString&; this way, the compiler should be able to
//     optimize wxGetTranslation() away

template<typename TString>
inline TString wxGetTranslation(TString str)
    { return str; }

template<typename TString, typename TDomain>
inline TString wxGetTranslation(TString str, TDomain WXUNUSED(domain))
    { return str; }

template<typename TString, typename TDomain, typename TContext>
inline TString wxGetTranslation(TString str, TDomain WXUNUSED(domain), TContext WXUNUSED(context))
    { return str; }

template<typename TString, typename TDomain>
inline TString wxGetTranslation(TString str1, TString str2, size_t n)
    { return n == 1 ? str1 : str2; }

template<typename TString, typename TDomain>
inline TString wxGetTranslation(TString str1, TString str2, size_t n,
                                TDomain WXUNUSED(domain))
    { return n == 1 ? str1 : str2; }

template<typename TString, typename TDomain, typename TContext>
inline TString wxGetTranslation(TString str1, TString str2, size_t n,
                                TDomain WXUNUSED(domain),
                                TContext WXUNUSED(context))
    { return n == 1 ? str1 : str2; }

#endif // wxUSE_INTL/!wxUSE_INTL

// define this one just in case it occurs somewhere (instead of preferred
// wxTRANSLATE) too
#if !defined(WXINTL_NO_GETTEXT_MACRO)
    #if !defined(gettext_noop)
        #define gettext_noop(str) (str)
    #endif
    #if !defined(N_)
        #define N_(s)             (s)
    #endif
#endif

#endif // _WX_TRANSLATION_H_
