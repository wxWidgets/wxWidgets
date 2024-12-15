///////////////////////////////////////////////////////////////////////////////
// Name:        wx/uilocale.h
// Purpose:     Interface of wxUILocale
// Author:      Vadim Zeitlin
// Created:     2021-08-01
// Copyright:   (c) 2021 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

/**
    Flags for wxUILocale::CompareStrings() function.

    @since 3.1.6
 */
enum
{
    /// Compare strings case-sensitively, this is the default.
    wxCompare_CaseSensitive   = 0,

    /**
        Ignore strings case when comparing.

        Note that this flag is not supported under POSIX systems, where it is
        simply ignored.
     */
    wxCompare_CaseInsensitive = 1
};

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
        Configure the UI to use the locale corresponding to the given locale name tag.

        If localized applications use this function instead of the recommended
        function wxUILocale::UseDefault(), it should be called as early as possible
        during the program startup, e.g. in the very beginning of the overridden
        wxApp::OnInit().

        @param localeName
            The locale name tag for which the corresponding locale should be created.
            Example: "de_DE.UTF-8" - German locale name in POSIX notation.
            See wxLocaleIdent::FromTag() for more information about the syntax of
            the @a locale tag string.

        Note that under most Unix systems (but not macOS) this function changes
        the C locale to the locale specified by the environment variables and
        so affects the results of calling C functions such as @c sprintf() etc
        which can use comma, rather than period, as decimal separator. The
        wxString::ToCDouble() and wxString::FromCDouble() functions can be used
        for parsing and formatting floating point numbers using period as
        decimal separator independently of the current locale.

        @return @true on success or @false if the locale with the given name
        couldn't be set

        @note If an application tries to configure the UI to use a locale other
              than the default user locale of the system, it can't be guaranteed
              that really @em all controls will obey the locale setting. For
              example, under Windows the calendar control will always use the
              default user locale, no matter which locale was set by this function.
              Under MacOS several standard dialogs like the file selection dialog
              will use at least partially the default user locale.
     */
    static bool UseLocaleName(const wxString& localeName);

    /**
        Creates the local corresponding to the given language tag.

        This is exactly equivalent to using wxUILocale constructor with
        the locale identifier returned by wxLocaleIdent::FromTag(), but
        shorter.

        See wxLocaleIdent::FromTag() for more information about the syntax of
        the @a tag string.
     */
    static wxUILocale FromTag(const wxString& tag);

    /**
        Creates the locale corresponding to the given locale identifier.

        In the simplest case, this can be used as following:
        @code
            const wxUILocale loc(wxLocaleIdent().Language("fr"));
        @endcode
        see wxLocaleIdent description for more details, including other ways of
        specifying the locale.

        If @a localeId is not recognized or not supported, default ("C") locale
        is used instead. Additionally, if @a localeId is empty (see
        wxLocaleIdent::IsEmpty()), an assertion failure is triggered.
     */
    explicit wxUILocale(const wxLocaleIdent& localeId);

    /**
        Compares two strings using comparison rules of this locale.

        This function is useful for sorting the strings in the order expected
        by the user, e.g. by correctly sorting "ä" in the same way as "a" when
        using German locale, but not when using Swedish one, in which "ä" is
        sorted after "z".

        It can be used both with the current locale, and with any other
        supported locale, even under systems not supporting changing the UI
        locale, such as macOS.

        @param lhs
            First comparing string.
        @param rhs
            Second comparing string.
        @param flags
            Can be used to specify whether to compare strings case-sensitively
            (default) or not, by specifying ::wxCompare_CaseInsensitive (note
            that this flag only works under MSW and Mac and is simply ignored
            under the other platforms).
        @return
            -1 if @a lhs is less than @a rhs.
            0 if @a lhs is equal to @a rhs.
            1 if @a lhs is greater than @a rhs.
     */
    int CompareStrings(const wxString& lhs, const wxString& rhs,
                       int flags = wxCompare_CaseSensitive) const;

    /**
        Get the platform-dependent name of the current locale.

        This name can be used in diagnostic messages.
     */
    wxString GetName() const;

    /**
        Get the locale id from which the current locale was instantiated.
     */
    wxLocaleIdent GetLocaleId() const;

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

    /**
        Query the locale for the specified localized name.

        @param name
            One of the elements of wxLocaleName enum.
        @param form
            The representation form requested.
        @return
            The localized name value or empty string if the function failed.
     */
    wxString GetLocalizedName(wxLocaleName name, wxLocaleForm form) const;

    /**
        Gets the full (default), abbreviated or shortest name of the given month.

        This function returns the name in the current locale, use
        wxDateTime::GetEnglishMonthName() to get the untranslated name if necessary.

        @param month
            One of wxDateTime::Jan, ..., wxDateTime::Dec values.
        @param form
            Name form consisting of the flags (Name_Full, Name_Abbr, or Name_Shortest)
            and the context (Context_Formatting or Context_Standalone)
            The default is Name_Full in Context_Formatting.
            Example: wxNameForm().Abbr().Standalone()

        @see GetWeekDayName()
        @since 3.3.0
    */
    wxString GetMonthName(wxDateTime::Month month, wxDateTime::NameForm form = {});

    /**
        Gets the full (default), abbreviated or shortest name of the given week day.

        This function returns the name in the current locale, use
        wxDateTime::GetEnglishWeekDayName() to get the untranslated name if necessary.

        @param weekday
            One of wxDateTime::Sun, ..., wxDateTime::Sat values.
        @param form
            Name form consisting of the flags (Name_Full, Name_Abbr, or Name_Shortest)
            and the context (Context_Formatting or Context_Standalone)
            The default is Name_Full in Context_Formatting.
            Example: wxNameForm().Abbr().Standalone()

        @see GetMonthName()
    */
    wxString GetWeekDayName(wxDateTime::WeekDay weekday, wxDateTime::NameForm form = {});

    /**
        Query the layout direction of the current locale.

        @return
            The layout direction or wxLayout_Default if the function failed.
     */
    wxLayoutDirection GetLayoutDirection() const;

    /**
        Return true if locale is supported on the current system.

        If this function returns @a false, the other functions of this class,
        such as GetInfo() and CompareStrings(), behave as in "C" locale, i.e.
        it's still safe to call them, but their results don't reflect the rules
        for the locale in question, but just use the default (i.e. US English)
        conventions.
     */
    bool IsSupported() const;

    /**
        Adds custom, user-defined language to the database of known languages.
    */
    static void AddLanguage(const wxLanguageInfo& info);

    /**
        This function may be used to find the language description structure for the
        given locale, specified either as a two letter ISO language code (for example,
        "pt"), a language code followed by the country code ("pt_BR") or a full, human
        readable, language description ("Portuguese_Brazil"). Please note that only
        the underscore character is supported as the separator between language and
        region codes.

        Returns the information for the given language or @NULL if this language
        is unknown. Note that even if the returned pointer is valid, the caller
        should @e not delete it.

        @see GetLanguageInfo()
    */
    static const wxLanguageInfo* FindLanguageInfo(const wxString& locale);

    /**
        This function may be used to find the language description structure for the
        given locale, specified as a locale identifier.

        Returns the information for the given language or @NULL if this language
        is unknown. Note that even if the returned pointer is valid, the caller
        should @e not delete it.

        @see GetLanguageInfo()
    */
    static const wxLanguageInfo* FindLanguageInfo(const wxLocaleIdent& localeId);

    /**
        Returns a pointer to wxLanguageInfo structure containing information about
        the given language or @NULL if this language is unknown. Note that even if
        the returned pointer is valid, the caller should @e not delete it.

        See AddLanguage() for the wxLanguageInfo description.
        As with Init(), @c wxLANGUAGE_DEFAULT has the special meaning if passed
        as an argument to this function and in this case the result of
        GetSystemLanguage() is used.
    */
    static const wxLanguageInfo* GetLanguageInfo(int lang);

    /**
        Returns English name of the given language or empty string if this
        language is unknown.

        See GetLanguageInfo() for a remark about special meaning of @c wxLANGUAGE_DEFAULT.
    */
    static wxString GetLanguageName(int lang);

    /**
        Returns canonical name of the given language or empty string if this
        language is unknown.

        See GetLanguageInfo() for a remark about special meaning of @c wxLANGUAGE_DEFAULT.
    */
    static wxString GetLanguageCanonicalName(int lang);

    /**
        Tries to detect the user's default user interface language setting.

        Returns the ::wxLanguage value or @c wxLANGUAGE_UNKNOWN if the language-guessing
        algorithm failed.

        @note Where possible this function returns the user's preferred UI @em language.
              This may be, and usually is, the same as the user's default locale, but it's
              not the same thing. If retrieving the preferred UI language is not supported
              by the operating system (for example, Windows 7 and below), the user's
              default @em locale will be used.

        @see wxTranslations::GetBestTranslation(), GetSystemLocaleId().
    */
    static int GetSystemLanguage();

    /**
        Tries to detect the user's default locale setting.

        Returns the ::wxLanguage value or @c wxLANGUAGE_UNKNOWN if the locale-guessing
        algorithm failed or if the locale can't be described using solely a
        language constant. Consider using GetSystemLocaleId() in this case.

        @note This function works with @em locales and returns the user's default
              locale. This may be, and usually is, the same as their preferred UI
              language, but it's not the same thing. Use wxTranslation to obtain
              @em language information.

        @since 3.1.7

        @see wxTranslations::GetBestTranslation().
    */
    static int GetSystemLocale();

    /**
        Return the description of the default system locale.

        This function can always represent the system locale, even when using
        a language and region pair that doesn't correspond to any of the
        predefined ::wxLanguage constants, such as e.g. "fr-DE", which means
        French language used with German locale settings.

        @since 3.2.2
     */
    static wxLocaleIdent GetSystemLocaleId();
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
    Represents a locale in a portable way.

    There are two possible ways to construct wxLocaleIdent:

    - You can either use FromTag() to create it from a string in the form
      `language ["-" script] ["-" region]`, corresponding to
      the subset of BCP 47 (https://www.rfc-editor.org/rfc/bcp/bcp47.txt)
      syntax.
    - Or you can create it from the different parts of this string by using
      the default constructor and then chaining calls to Language(),
      Region(), Script() and other methods.

    The first method is useful for interoperating with the other software using
    BCP 47 language tags, while the second one may result in more readable
    code and allows to specify Unix-specific locale description parts such as
    charset and modifier that are not part of the BCP 47 strings.

    Example of using wxLocaleIdent in the second way:
    @code
      auto loc = wxLocaleIdent().Language("fr").Region("BE").Modifier("euro");
    #if defined(__WINDOWS__) || defined(__WXOSX__)
      wxASSERT( loc.GetName() == "fr_BE" );
    #elif defined(__UNIX__)
      wxASSERT( loc.GetName() == "fr_BE@euro" );
    #endif
    @endcode

    For the first way, it is enough to just write
    @code
      auto loc = wxLocaleIdent::FromTag("fr-BE"); // Dash, not underscore!
    @endcode

    @since 3.1.6
*/
class wxLocaleIdent
{
public:
    /**
        Return the locale identifier corresponding to the given locale tag.

        This method accepts locale tags in various formats:

        - BCP-47,
        - Windows,
        - POSIX,
        - macOS. and
        - MSVC CRT.

        See section 2.01 of https://www.rfc-editor.org/rfc/bcp/bcp47.txt for the
        full BCP-47 syntax. Here we fully support just the subset we're interested in:

        - Normal language tags (not private use or grandfathered ones),
        - Script, and
        - Region.

        Additionally platform-specific tags are supported:

        - Extensions (without validity checks) (Windows only),
        - Sortorder (Windows only)
        - Charset (POSIX only), and
        - Modifier (POSIX only).

        Only language, script, and region are supported across all platforms.
        The script tag is mapped to the modifier tag for POSIX platforms.
        The script tag takes precedence, if a modifier is also specified.

        The following tag syntax is accepted:

        - BCP-47:   \<language\>[-\<script\>][-\<region\>][-\<extension\>]
        - Windows:  \<language\>[-\<script\>][-\<region\>][-\<extension\>][_\<sortorder\>]
        - POSIX:    \<language\>[_\<region\>][.\<charset\>][@@\<modifier\>]
        - macOS:    \<language\>[-\<script\>][_\<region\>]
        - MSVC CRT: \<language\>[_\<region\>][.\<charset\>]

        The string must contain at least the language part (2 or 3 ASCII
        letters) and may contain script and region separated by dashes, i.e.
        all of the following are valid:

        - "mn"
        - "mn-MN"
        - "mn-Cyrl-MN"

        Note that while BCP 47 extlangs, variants, extensions, private use and
        grandfathered tags are currently not directly supported, they may still
        work for creating wxUILocale on platforms with native support for BCP
        47 strings.

        If the input argument uses an unrecognized syntax (e.g. is empty), an
        empty wxLocaleIdent is returned. Of course, even if this function
        returns a non-empty object, the resulting locale may still be invalid
        or unsupported, use wxUILocale::IsSupported() to check for this.

        Note that the format "MSVC CRT" (Microsoft Visual C++ C RunTime) is
        only supported as an input format, so that locale names as returned
        by the CRT function setlocale can be handled, mainly for compatibility
        with wxLocale.
     */
    static wxLocaleIdent FromTag(const wxString& tag);

    /**
        Default constructor creates an empty and invalid locale identifier.

        At least Language() must be called to make the identifier valid.
    */
    wxLocaleIdent();

    /**
        Set language.

        Return reference to `this` for method chaining.

        See https://www.loc.gov/standards/iso639-2/php/English_list.php for the
        list of all language codes.

        @param language
            A two-letter ISO 639-1 language code or a three-letter ISO 639-2
            code for the languages without ISO 639-1 codes.
    */
    wxLocaleIdent& Language(const wxString& language);

    /**
        Set region.
        Return reference to `this` for method chaining.

        @param region
            It specifies an uppercase ISO 3166-1 country/region identifier.
    */
    wxLocaleIdent& Region(const wxString& region);

    /**
        Set script.

        Note that under Unix systems the script value is currently mapped
        to the modifier attribute using the script alias name, if the latter
        is known. Otherwise it is ignored.

        Return reference to `this` for method chaining.

        @param script
            It is an initial-uppercase ISO 15924 script code.
    */
    wxLocaleIdent& Script(const wxString& script);

    /**
        Set charset.

        Note that this value is only used under Unix systems and simply ignored
        under the other ones.

        Return reference to `this` for method chaining.

        @param charset
            Charset is a string such as "UTF-8", "ISO855915" or "KOI8R".
            Supported charsets depend on the implementation and installation.
    */
    wxLocaleIdent& Charset(const wxString& charset);

    /**
        Set modifier.

        Note that this value is only used under Unix systems and simply ignored
        under the other ones.
        Note that under Unix systems the modifier value may represent a script
        value. If the value corresponds to a valid script alias it is mapped
        to the associated script tag.

        Return reference to `this` for method chaining.

        @param modifier
            Modifier is a free-form text string.
    */
    wxLocaleIdent& Modifier(const wxString& modifier);

    /**
        Set extension.

        Note that this value is only used under Windows systems and simply ignored
        under the other ones.

        Return reference to `this` for method chaining.

        @param extension
            Extension identifiers allow to support custom Windows locales.
            They are usually not portable, not even from one Windows system
            to the other.
    */
    wxLocaleIdent& Extension(const wxString& extension);

    /**
        Set sortorder.

        Note that this value is only used under Windows systems and simply ignored
        under the other ones.

        Return reference to `this` for method chaining.

        @param sortorder
            Sortorder identifiers are defined in the Windows Development documentation:
            https://docs.microsoft.com/en-us/windows/win32/intl/sort-order-identifiers.
    */
    wxLocaleIdent& Sortorder(const wxString& sortorder);

    /// Return the language part of the locale identifier.
    const wxString& GetLanguage() const;

    /// Return the region part of the locale identifier.
    const wxString& GetRegion() const;

    /// Return the script part of the locale identifier.
    const wxString& GetScript() const;

    /// Return the charset part of the locale identifier.
    const wxString& GetCharset() const;

    /// Return the modifier part of the locale identifier.
    const wxString& GetModifier() const;

    /// Return the extension part of the locale identifier.
    const wxString& GetExtension() const;

    /// Return the sortorder part of the locale identifier.
    const wxString& GetSortorder() const;

    /**
        Construct platform dependent name.

        Format:
        - Windows: \<language\>-\<script\>-\<REGION\>-\<extension\>_\<sortorder\>
        - Unix:    \<language\>_\<REGION\>.\<charset\>@@{\<modifier\>\|\<scriptalias\>}
        - MacOS:   \<language\>-\<script\>_\<REGION\>
    */
    wxString GetName() const;

    /**
        Construct name in specified format.

        Format:
        - Default: name as used in wxLocaleIdent::FromTag() or system format
        - System:  name in platform-dependent format
        - Windows: \<language\>-\<script\>-\<REGION\>-\<extension\>_\<sortorder\>
        - Unix:    \<language\>_\<REGION\>.\<charset\>@@\<modifier\>
        - MacOS:   \<language\>-\<script\>_\<REGION\>
        - BCP 47:  \<language\>-\<script\>-\<REGION\>-\<extension\>

        @param tagType
            Value from wxLocaleTagType enum.
    */
    wxString GetTag(wxLocaleTagType tagType = wxLOCALE_TAGTYPE_DEFAULT) const;

    /**
        Check if the locale is empty.

        @return @true if this is an empty, invalid object.
    */
    bool IsEmpty() const;
};
