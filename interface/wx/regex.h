/////////////////////////////////////////////////////////////////////////////
// Name:        regex.h
// Purpose:     interface of wxRegEx
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**
    @anchor wxRE_FLAGS

    Flags for regex compilation to be used with wxRegEx::Compile().
*/
enum
{
    /**
        Use extended regex syntax.

        This is the default and doesn't need to be specified.
     */
    wxRE_EXTENDED = 0,

    /**
        Use advanced regex syntax.

        This flag is synonym for wxRE_EXTENDED and doesn't need to be specified
        as this is the default syntax.
     */
    wxRE_ADVANCED = 1,

    /**
        Use basic regex syntax.

        Use basic regular expression syntax, close to its POSIX definition,
        but with some extensions still available.

        The word start/end boundary assertions `\<` and `\>` are only
        available when using basic syntax, use `[[:<:]]` and `[[:>:]]` or
        just more general word boundary assertion `\b` when not using it.
     */
    wxRE_BASIC    = 2,

    /** Ignore case in match. */
    wxRE_ICASE    = 4,

    /** Only check match, don't set back references. */
    wxRE_NOSUB    = 8,

    /**
        If not set, treat `\n` as an ordinary character.

        Otherwise `\n` is special: it is not matched by `.` and `^` and `$`
        always match after/before it regardless of the setting of
        ::wxRE_NOTBOL and ::wxRE_NOTEOL.
    */
    wxRE_NEWLINE  = 16,

    /** Default flags.*/
    wxRE_DEFAULT  = wxRE_EXTENDED
};

/**
    @anchor wxRE_NOT_FLAGS

    Flags for regex matching to be used with wxRegEx::Matches().
    These flags are mainly useful when doing several matches in a long string
    to prevent erroneous matches for '^' and '$':
*/
enum
{
    /** '^' doesn't match at the start of line. */
    wxRE_NOTBOL = 32,

    /** '$' doesn't match at the end of line. */
    wxRE_NOTEOL = 64,

    /**
        Don't accept empty string as a valid match.

        If the regex matches an empty string, try alternatives, if there are
        any, or fail.

        This flag is not supported if PCRE support is turned off.

        @since 3.1.6
     */
    wxRE_NOTEMPTY = 128
};

/**
    @class wxRegEx

    wxRegEx represents a regular expression.  This class provides support
    for regular expressions matching and also replacement.

    In wxWidgets 3.1.6 or later, it is built on top of PCRE library
    (https://www.pcre.org/). In the previous versions of wxWidgets, this class
    uses Henry Spencer's library and behaved slightly differently, see below
    for the discussion of the changes if you're upgrading from an older
    version.

    Note that while C++11 and later provides @c std::regex and related classes,
    this class is still useful as it provides the following important
    advantages:

    - Support for richer regular expressions syntax.
    - Much better performance in many common cases, by a factor of 10-100.
    - Consistent behaviour, including performance, on all platforms.

    @library{wxbase}
    @category{data}

    Example:

    A (bad) example of processing some text containing email addresses (the example
    is bad because the real email addresses can have more complicated form than
    @c user@host.net):

    @code
    wxString originalText = "This is some text with foo@example.com and bar@example.com";

    // Regex. to match an email address and extract its subparts.
    wxRegEx reEmail("([^@ -]+)@([[:alnum:]_]+).([[:alnum:]]{2,4})");

    wxString processText = originalText;
    while ( reEmail.Matches(processText) )
    {
        // Find the size of the first match and print it.
        size_t start, len;
        reEmail.GetMatch(&start, &len, 0);
        std::cout << "Email: " << reEmail.GetMatch(processText, 0) << std::endl;

        // Print the submatches.
        std::cout << "Name: " << reEmail.GetMatch(processText, 1) << std::endl;
        std::cout << "Domain: " << reEmail.GetMatch(processText, 2) << std::endl;
        std::cout << "TLD: " << reEmail.GetMatch(processText, 3) << std::endl;

        // Process the remainder of the text if there is any.
        processText = processText.Mid (start + len);
    }

    // Or this will replace all names with "HIDDEN".
    size_t count = reEmail.ReplaceAll(&originalText, "HIDDEN@\\2.\\3");
    std::cout << "text now contains " << count << " hidden addresses" << std::endl;
    std::cout << originalText << std::endl;
    @endcode


    @section regex_pcre_changes Changes in the PCRE-based version

    This section describes the difference in regex syntax in the new PCRE-based
    wxRegEx version compared to the previously used version which implemented
    POSIX regex support.

    The main change is that both extended (::wxRE_EXTENDED) and advanced
    (::wxRE_ADVANCED) regex syntax is now the same as PCRE syntax described at
    https://www.pcre.org/current/doc/html/pcre2syntax.html

    Basic regular expressions (::wxRE_BASIC) are still different, but their
    use is deprecated and PCRE extensions are still accepted in them, please
    avoid using them.

    Other changes are:

    - Negated character classes, i.e. @c [^....], now always match newline
      character, regardless of whether ::wxRE_NEWLINE was used or not. The dot
      metacharacter still has the same meaning, i.e. it matches newline by
      default but not when ::wxRE_NEWLINE is specified.

    - Previously POSIX-specified behaviour of handling unmatched right
      parenthesis @c ')' as a literal character was implemented, but now this
      is a (regex) compilation error.

    - Empty alternation branches were previously ignored, i.e. matching @c a||b
      worked the same as matching just @c a|b, but now actually matches an
      empty string. The new ::wxRE_NOTEMPTY flag can be used to disable empty
      matches.

    - Using @c \\U to embed Unicode code points into the pattern is not
      supported any more, use the still supported @c \\u, followed by exactly
      four hexadecimal digits, or @c \\x, followed by exactly two hexadecimal
      digits, instead.

    - POSIX collating elements inside square brackets, i.e. @c [.XXX.] and
      @c [:XXXX:] are not supported by PCRE and result in regex compilation
      errors.

    - Backslash can be used to escape the character following it even inside
      square brackets now, while it loses its special meaning in POSIX regexes
      when it occurs inside square brackets. In particular, @c "\\]" escapes
      the special meaning of the closing bracket, and so does @e not close the
      character class. Please use @c "\\\\]" instead.

    - Closing parenthesis without a matching open parenthesis is now a syntax
      error instead of just being treated as a literal. To fix possible errors
      due to it, escape parenthesis that are supposed to be taken literally
      with a backslash, i.e. use @c "\\)" in C strings.

    - For completeness, PCRE syntax which previously resulted in errors, e.g.
      @c "(?:...)" and similar constructs, are now accepted and behave as
      expected. Other regexes syntactically invalid according to POSIX are
      re-interpreted as sequences of literal characters with PCRE, e.g. @c "{1"
      is just a sequence of two literal characters now, where it previously was
      a compilation error.
*/
class wxRegEx
{
public:

    /**
        Default constructor: use Compile() later.
    */
    wxRegEx();

    /**
        Create and compile the regular expression, use
        IsValid() to test for compilation errors.

        As for the flags, please see @ref wxRE_FLAGS.
    */
    wxRegEx(const wxString& expr, int flags = wxRE_DEFAULT);


    /**
        Destructor. It's not virtual, don't derive from this class.
    */
    ~wxRegEx();

    /**
        Compile the string into regular expression, return @true if ok or @false
        if string has a syntax error.

        As for the flags, please see @ref wxRE_FLAGS.
    */
    bool Compile(const wxString& pattern, int flags = wxRE_DEFAULT);

    /**
        Get the start index and the length of the match of the expression
        (if @a index is 0) or a bracketed subexpression (@a index different from 0).

        May only be called after successful call to Matches() and only if @c wxRE_NOSUB
        was @b not used in Compile().

        This function only returns @false if the regex didn't match at all or
        one of the arguments is invalid (e.g. @a index is greater or equal than
        the number of captures) and returns @true in all the other cases, even
        if the corresponding capture group didn't match anything, which can be
        the case when using captures in different alternation (@c "|"). In this
        case the returned @a len is @c 0 and @a start is @c -1.
    */
    bool GetMatch(size_t* start, size_t* len, size_t index = 0) const;

    /**
        Returns the part of string corresponding to the match where index is interpreted
        as above. Empty string is returned if match failed.

        May only be called after successful call to Matches() and only if @c wxRE_NOSUB
        was @b not used in Compile().
    */
    wxString  GetMatch(const wxString& text, size_t index = 0) const;

    /**
        Returns the size of the array of matches, i.e.\ the number of bracketed
        subexpressions plus one for the expression itself, or 0 on error.

        May only be called after successful call to Compile().
        and only if @c wxRE_NOSUB was @b not used.
    */
    size_t GetMatchCount() const;

    /**
        Return @true if this is a valid compiled regular expression, @false
        otherwise.
    */
    bool IsValid() const;

    ///@{
    /**
        Matches the precompiled regular expression against the string @a text,
        returns @true if matches and @false otherwise.

        @e Flags may be combination of @c wxRE_NOTBOL and @c wxRE_NOTEOL, see
        @ref wxRE_NOT_FLAGS.

        Some regex libraries assume that the text given is null terminated, while
        others require the length be given as a separate parameter. Therefore for
        maximum portability assume that @a text cannot contain embedded nulls.

        When the <b>Matches(const wxChar *text, int flags = 0)</b> form is used,
        a wxStrlen() will be done internally if the regex library requires the
        length. When using Matches() in a loop the <b>Matches(text, flags, len)</b>
        form can be used instead, making it possible to avoid a wxStrlen() inside
        the loop.

        May only be called after successful call to Compile().
    */
    bool Matches(const wxChar* text, int flags = 0) const;
    bool Matches(const wxChar* text, int flags, size_t len) const;
    ///@}

    /**
        Matches the precompiled regular expression against the string @a text,
        returns @true if matches and @false otherwise.

        @e Flags may be combination of @c wxRE_NOTBOL and @c wxRE_NOTEOL, see
        @ref wxRE_NOT_FLAGS.

        May only be called after successful call to Compile().
    */
    bool Matches(const wxString& text, int flags = 0) const;

    /**
        Replaces the current regular expression in the string pointed to by
        @a text, with the text in @a replacement and return number of matches
        replaced (maybe 0 if none found) or -1 on error.

        The replacement text may contain back references @c \\number which will be
        replaced with the value of the corresponding subexpression in the
        pattern match. @c \\0 corresponds to the entire match and @c \& is a
        synonym for it. Backslash may be used to quote itself or @c \& character.

        @a maxMatches may be used to limit the number of replacements made, setting
        it to 1, for example, will only replace first occurrence (if any) of the
        pattern in the text while default value of 0 means replace all.
    */
    int Replace(wxString* text, const wxString& replacement,
                size_t maxMatches = 0) const;

    /**
        Replace all occurrences: this is actually a synonym for
        Replace().

        @see ReplaceFirst()
    */
    int ReplaceAll(wxString* text, const wxString& replacement) const;

    /**
        Replace the first occurrence.
    */
    int ReplaceFirst(wxString* text, const wxString& replacement) const;

    /**
        Escapes any of the characters having special meaning for wxRegEx.

        Currently the following characters are special: \\, ^, $, ., |, ?, *,
        +, (, ), [, ], { and }. All occurrences of any of these characters in
        the passed string are escaped, i.e. a backslash is inserted before
        them, to remove their special meaning.

        For example:
        @code
            wxString quoted = wxRegEx::QuoteMeta("foo.*bar");
            assert( quoted == R"(foo\.\*bar)" );
        @endcode

        This function can be useful when using wxRegEx to search for a literal
        string entered by user, for example.

        @param str
            A string that may contain metacharacters to escape.

        @return A string with all metacharacters escaped.

        @since 3.1.3
    */
    static wxString QuoteMeta(const wxString& str);

    /**
        Converts a basic regular expression to an extended regex syntax.

        This function can be used to convert @a bre using deprecated wxRE_BASIC
        syntax to default (extended) syntax.

        @since 3.1.6
     */
    static wxString ConvertFromBasic(const wxString& bre);

    /**
        Return the version of PCRE used.

        The returned wxVersionInfo object currently always has its micro
        version component set to 0, as PCRE uses only major and minor version
        components. Its description component contains the version number,
        release date and, for pre-release PCRE versions, a mention of it.

        @since 3.1.6
     */
    static wxVersionInfo GetLibraryVersionInfo();
};

