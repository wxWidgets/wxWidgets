///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uilocale.h
// Purpose:     Interface of wxUILocale
// Author:      Vadim Zeitlin
// Created:     2021-08-01
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Query and modify locale used for the UI by the current platform.

    UI locale determines all culture-dependent conventions used in the user
    interface, including numbers, currencies and dates formatting. It also
    determines the language used by the native dialogs, such as wxFileDialog,
    where different labels use the language corresponding to the current UI
    locale.

    The UI locale is, in general, different from C locale set by the standard
    @c setlocale() function and affecting C standard library functions such as
    @c printf(), @c scanf(), @c strftime() and many others. Unfortunately, the
    relationship between C and UI locales is not the same depending on the
    platform: with wxGTK they must be the same, but under macOS C locale must
    not be changed, as doing this exposes bugs in the system. Because of this,
    applications can't generally count on C locale being set to any particular
    value and it is best to avoid using it, including implicitly via the
    standard C functions, in portable code. Instead, consider using
    wxNumberFormatter for parsing and formatting numbers according to the
    current UI locale or wxString::FromCDouble() and wxString::ToCDouble()
    functions for doing it always using period as decimal separator.

    Localized applications should call wxUILocale::UseDefault() on startup to
    explicitly indicate that they opt-in using the current UI locale, even if
    this results in changing the global C locale, as is the case in wxGTK. Note
    that some platforms (MSW and macOS) will use default user locale for their
    standard dialogs even if this function is not called, but it is still
    necessary to call it to use the correct number and date formats and to
    avoid mixing messages in the user language with default formats not
    corresponding to it.

    Please also note that under macOS to really use the user locale, it must be
    listed as a supported language in the application @c Info.plist file under
    @c CFBundleLocalizations key.

    Unlike wxLocale class, this class doesn't affect the translations used by
    the application, see wxTranslations for doing this.

    @library{wxbase}
    @since 3.1.6
 */
class wxUILocale
{
public:
    /**
        Configure the UI to use the default user locale.

        Localized applications should call this functions as early as possible
        during the program startup, e.g. in the very beginning of the
        overridden wxApp::OnInit().

        Note that under most Unix systems (but not macOS) this function changes
        the C locale to the locale specified by the environment variables and
        so affects the results of calling C functions such as @c sprintf() etc
        which can use comma, rather than period, as decimal separator. The
        wxString::ToCDouble() and wxString::FromCDouble() functions can be used
        for parsing and formatting floating point numbers using period as
        decimal separator independently of the current locale.

        @return @true on success or @false if the default locale couldn't be set
     */
    static bool UseDefault();

    /**
        Get the object corresponding to the currently used locale.

        If UseDefault() had been called, this object corresponds to the default
        user locale. Otherwise it corresponds to a generic locale similar to
        "C" locale, i.e. always uses period as decimal separator and m/d/y date
        format.
     */
    static const wxUILocale& GetCurrent();

    /**
        Compares two strings using comparison rules of the given locale.

        @param lhs
            First comparing string.
        @param rhs
            Second comparing string.
        @param localeId
            Represents platform dependent language name.
            @see wxLocaleIdent for details.
        @return
            -1 if lhs less than rhs.
            0 if lhs equal to rhs.
            1 if lhs greater than rhs.
     */
    static int CompareStrings(const wxString& lhs, const wxString& rhs,
                              const wxLocaleIdent& localeId = wxLocaleIdent());

    /**
        Get the platform-dependent name of the current locale.

        This name can be used in diagnostic messages.
     */
    wxString GetName() const;

    /**
        Query the locale for the specified information.

        This function returns the value of the locale-specific option specified
        by the given @a index.

        @param index
            One of the elements of wxLocaleInfo enum.
        @param cat
            The category to use with the given index or wxLOCALE_CAT_DEFAULT if
            the index can only apply to a single category.
        @return
            The option value or empty string if the function failed.
     */
    wxString GetInfo(wxLocaleInfo index,
                     wxLocaleCategory cat = wxLOCALE_CAT_DEFAULT) const;
};

/**
    Return the format to use for formatting user-visible dates.

    This is a simple wrapper function normally calling wxUILocale::GetInfo()
    with wxLOCALE_SHORT_DATE_FMT argument, but which is also available when @c
    wxUSE_INTL==0, i.e. support for internationalization is disabled at
    compile-time, in which case it returns @c %x string, i.e. uses the current
    C locale formatting rather than UI locale.

    @see wxDateTime::Format()

    @since 3.1.6
 */
wxString wxGetUIDateFormat();

/**
    Allows to construct the full locale identifier in a portable way.

    Parts of the locale not supported by the current platform (e.g. modifier under non-Unix platforms) are ignored.
    The remaining parts are used to construct a string uniquely identifying the locale in a platform-specific name.

    Usage example:
    @code
      auto loc = wxLocaleIdent("fr").Region("BE").Modifier("euro");
    #if defined(__WINDOWS__) || defined(__WXOSX__)
      wxASSERT( loc.GetName() == "fr_BE" );
    #elif defined(__UNIX__)
      wxASSERT( loc.GetName() == "fr_BE@euro" );
    #endif
    @endcode
    @since 3.1.6
*/
class wxLocaleIdent
{
public:
    /**
        This is the default constructor and it leaves language empty.
    */
    wxLocaleIdent();

    /**
        Constructor with language.

        @param language
            ISO 639 language code.
            See Language() for more detailed info.
    */
    wxLocaleIdent(const wxString& language);

    /**
        Set language.
        Return reference to @this for method chaining.

        @param language
            It is a lowercase ISO 639 language code.
            The codes from ISO 639-1 are used when available.
            Otherwise, codes from ISO 639-2/T are used.
    */
    wxLocaleIdent& Language(const wxString& language);

    /**
        Set region.
        Return reference to @this for method chaining.

        @param region
            It specifies an uppercase ISO 3166-1 country/region identifier.
    */
    wxLocaleIdent& Region(const wxString& region);

    /**
        Set script.
        Return reference to @this for method chaining.

        @param script
            It is an initial-uppercase ISO 15924 script code.
    */
    wxLocaleIdent& Script(const wxString& script);

    /**
        Set modifier.
        Return reference to @this for method chaining.

        @param modifier
            Modifier is defined by ISO/IEC 15897.
            It is a semi-colon separated list of identifiers, or name=value pairs.
    */
    wxLocaleIdent& Modifier(const wxString& modifier);

    /**
        Construct platform dependent name.
        Format:
        Windows: <language>-<script>-<REGION>
        Unix:    <language>_<REGION>@<modifier>
        MacOS:   <language>-<script>_<REGION>
    */
    wxString GetName() const;

    /**
        Empty language represents user's default language.

        @return @true if language is empty, @false otherwise.
    */
    bool IsDefault() const;
};
