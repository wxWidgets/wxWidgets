/////////////////////////////////////////////////////////////////////////////
// Name:        intl.h
// Purpose:     interface of wxLocale
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

/**
    @class wxLocale

    wxLocale class encapsulates all language-dependent settings and is a
    generalization of the C locale concept.

    In wxWidgets this class manages message catalogs which contain the translations
    of the strings used to the current language.

    @b wxPerl note: In wxPerl you can't use the '_' function name, so
    the @c Wx::Locale module can export the @c gettext and
    @c gettext_noop under any given name.

    @code
    # this imports gettext ( equivalent to Wx::GetTranslation
      # and gettext_noop ( a noop )
      # into your module
      use Wx::Locale qw(:default);

      # ....

      # use the functions
      print gettext( "Panic!" );

      button = Wx::Button-new( window, -1, gettext( "Label" ) );
    @endcode

    If you need to translate a lot of strings, then adding gettext( ) around
    each one is a long task ( that is why _( ) was introduced ), so just choose
    a shorter name for gettext:

    @code
    #
      use Wx::Locale 'gettext' = 't',
                     'gettext_noop' = 'gettext_noop';

      # ...

      # use the functions
      print t( "Panic!!" );

      # ...
    @endcode

    @library{wxbase}
    @category{FIXME}

    @see @ref overview_internationalization, @ref overview_sampleinternat "Internat
    sample", wxXLocale
*/
class wxLocale
{
public:
    //@{
    /**
        See Init() for parameters description.
        The call of this function has several global side effects which you should
        understand: first of all, the application locale is changed - note that this
        will affect many of standard C library functions such as printf() or strftime().
        Second, this wxLocale object becomes the new current global locale for the
        application and so all subsequent calls to wxGetTranslation() will try to
        translate the messages using the message catalogs for this locale.
    */
    wxLocale();
    wxLocale(int language,
             int flags =
                 wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);
    wxLocale(const wxString& name,
             const wxString& short = wxEmptyString,
             const wxString& locale = wxEmptyString,
             bool bLoadDefault = true,
             bool bConvertEncoding = false);
    //@}

    /**
        The destructor, like the constructor, also has global side effects: the
        previously
        set locale is restored and so the changes described in
        Init() documentation are rolled back.
    */
    virtual ~wxLocale();

    //@{
    /**
        Add a catalog for use with the current locale: it is searched for in standard
        places (current directory first, then the system one), but you may also prepend
        additional directories to the search path with
        AddCatalogLookupPathPrefix().
        All loaded catalogs will be used for message lookup by
        GetString() for the current locale.
        Returns @true if catalog was successfully loaded, @false otherwise (which might
        mean that the catalog is not found or that it isn't in the correct format).
        The second form of this method takes two additional arguments,
        @a msgIdLanguage and @e msgIdCharset.
        @a msgIdLanguage specifies the language of "msgid" strings in source code
        (i.e. arguments to GetString(),
        wxGetTranslation() and the
        _() macro). It is used if AddCatalog cannot find any
        catalog for current language: if the language is same as source code language,
        then strings from source code are used instead.
        @a msgIdCharset lets you specify the charset used for msgids in sources
        in case they use 8-bit characters (e.g. German or French strings).  This
        argument has no effect in Unicode build, because literals in sources are
        Unicode strings; you have to use compiler-specific method of setting the right
        charset when compiling with Unicode.
        By default (i.e. when you use the first form), msgid strings are assumed
        to be in English and written only using 7-bit ASCII characters.
        If you have to deal with non-English strings or 8-bit characters in the source
        code, see the instructions in
        @ref overview_nonenglishoverview "Writing non-English applications".
    */
    bool AddCatalog(const wxString& domain);
    bool AddCatalog(const wxString& domain,
                    wxLanguage msgIdLanguage,
                    const wxString& msgIdCharset);
    //@}

    /**
        Add a prefix to the catalog lookup path: the message catalog files will be
        looked up under prefix/lang/LC_MESSAGES, prefix/lang and prefix
        (in this order).
        This only applies to subsequent invocations of AddCatalog().
    */
    static void AddCatalogLookupPathPrefix(const wxString& prefix);

    /**
        Adds custom, user-defined language to the database of known languages. This
        database is used in conjunction with the first form of
        Init().
        wxLanguageInfo is defined as follows:
        @e Language should be greater than wxLANGUAGE_USER_DEFINED.
        Wx::LanguageInfo-new( language, canonicalName, WinLang, WinSubLang, Description
        )
    */
    static void AddLanguage(const wxLanguageInfo& info);

    /**
        This function may be used to find the language description structure for the
        given locale, specified either as a two letter ISO language code (for example,
        "pt"), a language code followed by the country code ("pt_BR") or a full, human
        readable, language description ("Portuguese-Brazil").
        Returns the information for the given language or @NULL if this language
        is unknown. Note that even if the returned pointer is valid, the caller should
        @e not delete it.

        @see GetLanguageInfo()
    */
    static wxLanguageInfo* FindLanguageInfo(const wxString& locale);

    /**
        Returns the canonical form of current locale name. Canonical form is the
        one that is used on UNIX systems: it is a two- or five-letter string in xx or
        xx_YY format, where xx is ISO 639 code of language and YY is ISO 3166 code of
        the country. Examples are "en", "en_GB", "en_US" or "fr_FR".
        This form is internally used when looking up message catalogs.
        Compare GetSysName().
    */
    wxString GetCanonicalName() const;

    /**
        Returns the header value for header @e header. The search for @a header is case
        sensitive. If an @e domain
        is passed, this domain is searched. Else all domains will be searched until a
        header has been found.
        The return value is the value of the header if found. Else this will be empty.
    */
    wxString GetHeaderValue(const wxString& header,
                            const wxString& domain = wxEmptyString) const;

    /**
        Returns wxLanguage() constant of current language.
        Note that you can call this function only if you used the form of
        Init() that takes wxLanguage argument.
    */
    int GetLanguage() const;

    /**
        Returns a pointer to wxLanguageInfo structure containing information about the
        given language or @NULL if this language is unknown. Note that even if the
        returned pointer is valid, the caller should @e not delete it.
        See AddLanguage() for the wxLanguageInfo
        description.
        As with Init(), @c wxLANGUAGE_DEFAULT has the
        special meaning if passed as an argument to this function and in this case the
        result of GetSystemLanguage() is used.
    */
    static wxLanguageInfo* GetLanguageInfo(int lang) const;

    /**
        Returns English name of the given language or empty string if this
        language is unknown.
        See GetLanguageInfo() for a remark about
        special meaning of @c wxLANGUAGE_DEFAULT.
    */
    static wxString GetLanguageName(int lang) const;

    /**
        Returns the locale name as passed to the constructor or
        Init(). This is full, human-readable name,
        e.g. "English" or "French".
    */
    const wxString GetLocale() const;

    /**
        Returns the current short name for the locale (as given to the constructor or
        the Init() function).
    */
    const wxString GetName() const;

    //@{
    /**
        Retrieves the translation for a string in all loaded domains unless the szDomain
        parameter is specified (and then only this catalog/domain is searched).
        Returns original string if translation is not available
        (in this case an error message is generated the first time
        a string is not found; use wxLogNull to suppress it).
        The second form is used when retrieving translation of string that has
        different singular and plural form in English or different plural forms in some
        other language. It takes two extra arguments: @e origString
        parameter must contain the singular form of the string to be converted.
        It is also used as the key for the search in the catalog.
        The @a origString2 parameter is the plural form (in English).
        The parameter @a n is used to determine the plural form.  If no
        message catalog is found @a origString is returned if 'n == 1',
        otherwise @e origString2.
        See GNU gettext manual for additional information on plural forms handling.
        This method is called by the wxGetTranslation()
        function and _() macro.

        @remarks Domains are searched in the last to first order, i.e. catalogs
                 added later override those added before.
    */
    const wxString GetString(const wxString& origString,
                             const wxString& domain = wxEmptyString) const;
    const const wxString&   GetString(const wxString& origString,
                                      const wxString& origString2,
                                      size_t n,
                                      const wxString& domain = NULL) const;
    //@}

    /**
        Returns current platform-specific locale name as passed to setlocale().
        Compare GetCanonicalName().
    */
    wxString GetSysName() const;

    /**
        Tries to detect the user's default font encoding.
        Returns wxFontEncoding() value or
        @b wxFONTENCODING_SYSTEM if it couldn't be determined.
    */
    static wxFontEncoding GetSystemEncoding() const;

    /**
        Tries to detect the name of the user's default font encoding. This string isn't
        particularly useful for the application as its form is platform-dependent and
        so you should probably use
        GetSystemEncoding() instead.
        Returns a user-readable string value or an empty string if it couldn't be
        determined.
    */
    static wxString GetSystemEncodingName() const;

    /**
        Tries to detect the user's default language setting.
        Returns wxLanguage() value or
         @b wxLANGUAGE_UNKNOWN if the language-guessing algorithm failed.
    */
    static int GetSystemLanguage() const;

    //@{
    /**
        The second form is deprecated, use the first one unless you know what you are
        doing.

        @param language
            wxLanguage identifier of the locale.
            wxLANGUAGE_DEFAULT has special meaning -- wxLocale will use system's
        default
            language (see GetSystemLanguage).
        @param flags
            Combination of the following:







            wxLOCALE_LOAD_DEFAULT




            Load the message catalog
            for the given locale containing the translations of standard wxWidgets
        messages
            automatically.





            wxLOCALE_CONV_ENCODING




            Automatically convert message
            catalogs to platform's default encoding. Note that it will do only basic
            conversion between well-known pair like iso8859-1 and windows-1252 or
            iso8859-2 and windows-1250. See Writing non-English applications for
        detailed
            description of this behaviour. Note that this flag is meaningless in
        Unicode build.
        @param name
            The name of the locale. Only used in diagnostic messages.
        @param short
            The standard 2 letter locale abbreviation; it is used as the
            directory prefix when looking for the message catalog files.
        @param locale
            The parameter for the call to setlocale(). Note that it is
            platform-specific.
        @param bLoadDefault
            May be set to @false to prevent loading of the message catalog
            for the given locale containing the translations of standard wxWidgets
        messages.
            This parameter would be rarely used in normal circumstances.
        @param bConvertEncoding
            May be set to @true to do automatic conversion of message
            catalogs to platform's native encoding. Note that it will do only basic
            conversion between well-known pair like iso8859-1 and windows-1252 or
            iso8859-2 and windows-1250.
            See Writing non-English applications for detailed
            description of this behaviour.
    */
    bool Init(int language = wxLANGUAGE_DEFAULT,
              int flags =
                  wxLOCALE_LOAD_DEFAULT | wxLOCALE_CONV_ENCODING);
    bool Init(const wxString& name,
              const wxString& short = wxEmptyString,
              const wxString& locale = wxEmptyString,
              bool bLoadDefault = true,
              bool bConvertEncoding = false);
    //@}

    /**
        Check whether the operating system and/or C run time environment supports
        this locale. For example in Windows 2000 and Windows XP, support for many
        locales is not installed by default. Returns @true if the locale is
        supported.
        The argument @a lang is the wxLanguage identifier. To obtain this for a
        given a two letter ISO language code, use
        FindLanguageInfo() to obtain its
        wxLanguageInfo structure. See AddLanguage() for
        the wxLanguageInfo description.

        @since 2.7.1.
    */
    static bool IsAvailable(int lang);

    /**
        Check if the given catalog is loaded, and returns @true if it is.
        According to GNU gettext tradition, each catalog
        normally corresponds to 'domain' which is more or less the application name.
        See also: AddCatalog()
    */
    bool IsLoaded(const char* domain) const;

    /**
        Returns @true if the locale could be set successfully.
    */
    bool IsOk() const;

    /**
        See @ref overview_languagecodes "list of recognized language constants".
        These constants may be used to specify the language
        in Init() and are returned by
        GetSystemLanguage():
    */
};



/**
    @class wxXLocale


    wxXLocale::wxXLocale
    wxXLocale::GetCLocale
    wxXLocale::IsOk


    Introduction

    This class represents a locale object used by so-called xlocale API. Unlike
    wxLocale it doesn't provide any non-trivial operations but
    simply provides a portable wrapper for POSIX @c locale_t type. It exists
    solely to be provided as an argument to various @c wxFoo_l() functions
    which are the extensions of the standard locale-dependent functions (hence the
    name xlocale). These functions do exactly the same thing as the corresponding
    standard @c foo() except that instead of using the global program locale
    they use the provided wxXLocale object. For example, if the user runs the
    program in French locale, the standard @c printf() function will output
    floating point numbers using decimal comma instead of decimal period. If the
    program needs to format a floating-point number in a standard format it can
    use @c wxPrintf_l(wxXLocale::GetCLocale(), "%g", number) to do it.
    Conversely, if a program wanted to output the number in French locale, even if
    the current locale is different, it could use wxXLocale(wxLANGUAGE_FRENCH).


    Availability

    This class is fully implemented only under the platforms where xlocale POSIX
    API or equivalent is available. Currently the xlocale API is available under
    most of the recent Unix systems (including Linux, various BSD and Mac OS X) and
    Microsoft Visual C++ standard library provides a similar API starting from
    version 8 (Visual Studio 2005).

    If neither POSIX API nor Microsoft proprietary equivalent are available, this
    class is still available but works in degraded mode: the only supported locale
    is the C one and attempts to create wxXLocale object for any other locale will
    fail. You can use the preprocessor macro @c wxHAS_XLOCALE_SUPPORT to
    test if full xlocale API is available or only skeleton C locale support is
    present.

    Notice that wxXLocale is new in wxWidgets 2.9.0 and is not compiled in if
    @c wxUSE_XLOCALE was set to 0 during the library compilation.


    Locale-dependent functions

    Currently the following @c _l-functions are available:

         Character classification functions: @c wxIsxxx_l(), e.g.
              @c wxIsalpha_l(), @c wxIslower_l() and all the others.
         Character transformation functions: @c wxTolower_l() and
              @c wxToupper_l()

    We hope to provide many more functions (covering numbers, time and formatted
    IO) in the near future.

    @library{wxbase}
    @category{FIXME}

    @see wxLocale
*/
class wxXLocale
{
public:
    //@{
    /**
        Creates the locale object corresponding to the specified locale string. The
        locale string is system-dependent, use constructor taking wxLanguage for better
        portability.
    */
    wxLocale();
    wxLocale(wxLanguage lang);
    wxLocale(const char* loc);
    //@}

    /**
        This class is fully implemented only under the platforms where xlocale POSIX
        API or equivalent is available. Currently the xlocale API is available under
        most of the recent Unix systems (including Linux, various BSD and Mac OS X) and
        Microsoft Visual C++ standard library provides a similar API starting from
        version 8 (Visual Studio 2005).
        If neither POSIX API nor Microsoft proprietary equivalent are available, this
        class is still available but works in degraded mode: the only supported locale
        is the C one and attempts to create wxXLocale object for any other locale will
        fail. You can use the preprocessor macro @c wxHAS_XLOCALE_SUPPORT to
        test if full xlocale API is available or only skeleton C locale support is
        present.
        Notice that wxXLocale is new in wxWidgets 2.9.0 and is not compiled in if
        @c wxUSE_XLOCALE was set to 0 during the library compilation.
    */


    /**
        Returns the global object representing the "C" locale. For an even shorter
        access to this object a global @c wxCLocale variable (implemented as a
        macro) is provided and can be used instead of calling this method.
    */
    static wxXLocale GetCLocale();

    /**
        This class represents a locale object used by so-called xlocale API. Unlike
        wxLocale it doesn't provide any non-trivial operations but
        simply provides a portable wrapper for POSIX @c locale_t type. It exists
        solely to be provided as an argument to various @c wxFoo_l() functions
        which are the extensions of the standard locale-dependent functions (hence the
        name xlocale). These functions do exactly the same thing as the corresponding
        standard @c foo() except that instead of using the global program locale
        they use the provided wxXLocale object. For example, if the user runs the
        program in French locale, the standard @c printf() function will output
        floating point numbers using decimal comma instead of decimal period. If the
        program needs to format a floating-point number in a standard format it can
        use @c wxPrintf_l(wxXLocale::GetCLocale(), "%g", number) to do it.
        Conversely, if a program wanted to output the number in French locale, even if
        the current locale is different, it could use wxXLocale(wxLANGUAGE_FRENCH).
    */


    /**
        Returns @true if this object is initialized, i.e. represents a valid locale
        or
        @false otherwise.
    */
    bool IsOk() const;

    /**
        Currently the following @c _l-functions are available:
             Character classification functions: @c wxIsxxx_l(), e.g.
                  @c wxIsalpha_l(), @c wxIslower_l() and all the others.
             Character transformation functions: @c wxTolower_l() and
                  @c wxToupper_l()
        We hope to provide many more functions (covering numbers, time and formatted
        IO) in the near future.

        @see wxLocale
    */
};



// ============================================================================
// Global functions/macros
// ============================================================================

/** @ingroup group_funcmacro_string */
//@{

/**
    This macro is identical to _() but for the plural variant of
    wxGetTranslation().

    @return A const wxString.

    @header{wx/intl.h}
*/
#define wxPLURAL(string, plural, n)

/**
    This macro doesn't do anything in the program code -- it simply expands to
    the value of its argument.

    However it does have a purpose which is to mark the literal strings for the
    extraction into the message catalog created by @c xgettext program. Usually
    this is achieved using _() but that macro not only marks the string for
    extraction but also expands into a wxGetTranslation() call which means that
    it cannot be used in some situations, notably for static array
    initialization.

    Here is an example which should make it more clear: suppose that you have a
    static array of strings containing the weekday names and which have to be
    translated (note that it is a bad example, really, as wxDateTime already
    can be used to get the localized week day names already). If you write:

    @code
    static const char * const weekdays[] = { _("Mon"), ..., _("Sun") };
    ...
    // use weekdays[n] as usual
    @endcode

    The code wouldn't compile because the function calls are forbidden in the
    array initializer. So instead you should do this:

    @code
    static const char * const weekdays[] = { wxTRANSLATE("Mon"), ...,
    wxTRANSLATE("Sun") };
    ...
    // use wxGetTranslation(weekdays[n])
    @endcode

    Note that although the code @b would compile if you simply omit
    wxTRANSLATE() in the above, it wouldn't work as expected because there
    would be no translations for the weekday names in the program message
    catalog and wxGetTranslation() wouldn't find them.

    @return A const wxChar*.

    @header{wx/intl.h}
*/
#define wxTRANSLATE(string)

/**
    This function returns the translation of @a string in the current
    @c locale(). If the string is not found in any of the loaded message
    catalogs (see @ref overview_i18n), the original string is returned. In
    debug build, an error message is logged -- this should help to find the
    strings which were not yet translated.  If @a domain is specified then only
    that domain/catalog is searched for a matching string.  As this function is
    used very often, an alternative (and also common in Unix world) syntax is
    provided: the _() macro is defined to do the same thing as
    wxGetTranslation().

    This function calls wxLocale::GetString().

    @note This function is not suitable for literal strings in Unicode builds
          since the literal strings must be enclosed into _T() or wxT() macro
          which makes them unrecognised by @c xgettext, and so they are not
          extracted to the message catalog. Instead, use the _() and wxPLURAL()
          macro for all literal strings.

    @see wxGetTranslation(const wxString&, const wxString&, size_t, const wxString&)

    @header{wx/intl.h}
*/
const wxString wxGetTranslation(const wxString& string,
                                 const wxString& domain = wxEmptyString);

/**
    This is an overloaded version of
    wxGetTranslation(const wxString&, const wxString&), please see its
    documentation for general information.

    This version is used when retrieving translation of string that has
    different singular and plural forms in English or different plural forms in
    some other language. Like wxGetTranslation(const wxString&,const wxString&),
    the @a string parameter must contain the singular form of the string to be
    converted and is used as the key for the search in the catalog. The
    @a plural parameter is the plural form (in English). The parameter @a n is
    used to determine the plural form. If no message catalog is found,
    @a string is returned if "n == 1", otherwise @a plural is returned.

    See GNU gettext Manual for additional information on plural forms handling:
    <http://www.gnu.org/software/gettext/manual/gettext.html#Plural-forms>
    For a shorter alternative see the wxPLURAL() macro.

    This function calls wxLocale::GetString().

    @header{wx/intl.h}
*/
const wxString wxGetTranslation(const wxString& string,
                                 const wxString& plural, size_t n,
                                 const wxString& domain = wxEmptyString);

/**
    This macro expands into a call to wxGetTranslation(), so it marks the
    message for the extraction by @c xgettext just as wxTRANSLATE() does, but
    also returns the translation of the string for the current locale during
    execution.

    Don't confuse this with _T()!

    @header{wx/intl.h}
*/
const wxString _(const wxString& string);

//@}

