///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/regex.cpp
// Purpose:     regular expression matching
// Author:      Karsten Ballueder and Vadim Zeitlin
// Created:     13.07.01
// Copyright:   (c) 2000 Karsten Ballueder <ballueder@gmx.net>
//                  2001 Vadim Zeitlin <vadim@wxwidgets.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#if wxUSE_REGEX

#include "wx/regex.h"

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/log.h"
    #include "wx/intl.h"
    #include "wx/crt.h"
#endif //WX_PRECOMP

// At least FreeBSD requires this.
#if defined(__UNIX__)
#   include <sys/types.h>
#endif

// WXREGEX_CONVERT_TO_MB    defined when the regex lib is using chars and
//                          wxChar is wide, so conversion to UTF-8 must be done
// wxRegChar                the character type used by the regular expression engine
//

// Use the same code unit width for PCRE as we use for wxString.
#if wxUSE_UNICODE_UTF8
#    define PCRE2_CODE_UNIT_WIDTH 8
     typedef char wxRegChar;
#elif wxUSE_UNICODE_UTF16
#    define PCRE2_CODE_UNIT_WIDTH 16
     typedef wchar_t wxRegChar;
#else
#    define PCRE2_CODE_UNIT_WIDTH 32
     typedef wchar_t wxRegChar;
#endif

typedef wxRegChar wxRegErrorChar;

// We currently always use PCRE as a static library under MSW.
#ifdef __WINDOWS__
#    define PCRE2_STATIC
#endif

#include <pcre2.h>

#if wxUSE_UNICODE_UTF8
#    define WXREGEX_CONVERT_TO_MB
#endif

// There is an existing pcre2posix library which provides regxxx()
// implementations, but we don't use it because:
//
//  0. The plan is to stop using POSIX API soon anyhow.
//  1. It's yet another system library to depend on.
//  2. We can add non-standard "len" parameter to regexec().
//  3. We want to use PCRE2_ALT_BSUX for compatibility, but we can't
//     set it using just the POSIX API.
//
// So implement these functions ourselves.
namespace
{

// Define POSIX constants and structs ourselves too.

#define REG_EXTENDED  0         // Unused, for compatibility only.

#define REG_ICASE     0x0001    // Same as PCRE2_CASELESS.
#define REG_NEWLINE   0x0002    // Same as PCRE2_MULTILINE.
#define REG_NOTBOL    0x0004    // Same as PCRE2_NOTBOL.
#define REG_NOTEOL    0x0008    // Same as PCRE2_NOTEOL.
#define REG_NOSUB     0x0020    // Don't return matches.
#define REG_NOTEMPTY  0x0100    // Same as PCRE2_NOTEMPTY.

enum
{
    REG_NOERROR = 0,    // Must be 0.
    REG_NOMATCH,        // Returned from regexec().
    REG_BADPAT,         // Catch-all error returned from regcomp().
    REG_ESPACE          // Catch-all errir returned from regexec().
};

typedef size_t regoff_t;

struct regex_t
{
    // This is the only "public" field -- not that it really matters anyhow for
    // this private struct.
    size_t re_nsub;

    pcre2_code* code;
    pcre2_match_data* match_data;

    int errorcode;
    regoff_t erroroffset;
};

struct regmatch_t
{
    regoff_t rm_so;
    regoff_t rm_eo;
};

int wx_regcomp(regex_t* preg, const wxRegChar* pattern, int cflags)
{
    // PCRE2_UTF is required in order to handle non-ASCII characters when using
    // 8-bit version of the library.
    //
    // Use PCRE2_ALT_BSUX because we want to handle \uXXXX for compatibility
    // with the previously used regex library and because it's useful.
    int options = PCRE2_UTF | PCRE2_ALT_BSUX;

    if ( cflags & REG_ICASE )
        options |= PCRE2_CASELESS;

    // Default behaviour of the old regex library corresponds to DOTALL i.e.
    // dot matches any character, but wxRE_NEWLINE enables both MULTILINE (so
    // that ^/$ match after/before newline in addition to matching at the
    // start/end of string) and disables the special handling of "\n", i.e. we
    // must use DOTALL with it.
    if ( cflags & REG_NEWLINE )
        options |= PCRE2_MULTILINE;
    else
        options |= PCRE2_DOTALL;

    preg->code = pcre2_compile
                 (
                    (PCRE2_SPTR)pattern,
                    PCRE2_ZERO_TERMINATED,
                    options,
                    &preg->errorcode,
                    &preg->erroroffset,
                    nullptr                    // use default context
                 );

    if ( !preg->code )
    {
        // Don't bother translating PCRE error to the most appropriate POSIX
        // error code, there is no way to do it losslessly and the main thing
        // that matters is the error message and not the error code anyhow.
        return REG_BADPAT;
    }

    preg->match_data = pcre2_match_data_create_from_pattern(preg->code, nullptr);

    return REG_NOERROR;
}

int
wx_regexec(const regex_t* preg, const wxRegChar* string, size_t len,
           size_t nmatch, regmatch_t* pmatch, int eflags)
{
    int options = 0;

    if ( eflags & REG_NOTBOL )
        options |= PCRE2_NOTBOL;
    if ( eflags & REG_NOTEOL )
        options |= PCRE2_NOTEOL;
    if ( eflags & REG_NOTEMPTY )
        options |= PCRE2_NOTEMPTY;

    const int rc = pcre2_match
                   (
                        preg->code,
                        (PCRE2_SPTR)string,
                        len,
                        0,                      // start offset
                        options,
                        preg->match_data,
                        nullptr                    // use default context
                   );

    if ( rc == PCRE2_ERROR_NOMATCH )
        return REG_NOMATCH;

    if ( rc < 0 )
        return REG_ESPACE;

    // Successful match, fill in pmatch array if necessary.
    if ( pmatch )
    {
        const PCRE2_SIZE* const
            ovector = pcre2_get_ovector_pointer(preg->match_data);

        const size_t nmatchActual = static_cast<size_t>(rc);
        for ( size_t n = 0; n < nmatch; ++n )
        {
            regmatch_t& m = pmatch[n];

            if ( n < nmatchActual )
            {
                m.rm_so = ovector[n*2] == PCRE2_UNSET ? -1 : ovector[n*2];
                m.rm_eo = ovector[n*2+1] == PCRE2_UNSET ? -1 : ovector[n*2+1];
            }
            else
            {
                m.rm_so =
                m.rm_eo = static_cast<regoff_t>(-1);
            }
        }
    }

    return REG_NOERROR;
}

size_t
wx_regerror(int errcode, const regex_t* preg, wxRegErrorChar* errbuf, size_t errbuf_size)
{
    // We don't use the passed in POSIX error code other than to check that we
    // do have an error but rely on PCRE error code from regex_t.
    wxRegErrorChar buffer[256];
    int len;
    if ( errcode == REG_NOERROR )
        len = wxSnprintf(buffer, WXSIZEOF(buffer), "no error");
    else
        len = pcre2_get_error_message(preg->errorcode, (PCRE2_UCHAR*)buffer, sizeof(buffer));

    if ( len < 0 )
        len = wxSnprintf(buffer, WXSIZEOF(buffer), "PCRE error %d", preg->errorcode);

    if ( errbuf && errbuf_size )
        wxStrlcpy(errbuf, buffer, errbuf_size);

    return len;
}

void wx_regfree(regex_t* preg)
{
    pcre2_match_data_free(preg->match_data);
    pcre2_code_free(preg->code);
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

// the array of offsets for the matches, the usual POSIX regmatch_t array.
class wxRegExMatches
{
public:
    typedef regmatch_t *match_type;

    wxRegExMatches(size_t n)        { m_matches = new regmatch_t[n]; }
    ~wxRegExMatches()               { delete [] m_matches; }

    // we just use casts here because the fields of regmatch_t struct may be 64
    // bit but we're limited to size_t in our public API and are not going to
    // change it because operating on strings longer than 4GB using it is
    // absolutely impractical anyhow
    size_t Start(size_t n) const
    {
        return wx_truncate_cast(size_t, m_matches[n].rm_so);
    }

    size_t End(size_t n) const
    {
        return wx_truncate_cast(size_t, m_matches[n].rm_eo);
    }

    regmatch_t *get() const         { return m_matches; }

private:
    regmatch_t *m_matches;
};

// the real implementation of wxRegEx
class wxRegExImpl
{
public:
    // ctor and dtor
    wxRegExImpl();
    ~wxRegExImpl();

    // return true if Compile() had been called successfully
    bool IsValid() const { return m_isCompiled; }

    // RE operations
    bool Compile(wxString expr, int flags = 0);
    bool Matches(const wxRegChar *str, int flags, size_t len) const;
    bool GetMatch(size_t *start, size_t *len, size_t index = 0) const;
    size_t GetMatchCount() const;
    int Replace(wxString *pattern, const wxString& replacement,
                size_t maxMatches = 0) const;

private:
    // return the string containing the error message for the given err code
    wxString GetErrorMsg(int errorcode) const;

    // init the members
    void Init()
    {
        m_isCompiled = false;
        m_Matches = nullptr;
        m_nMatches = 0;
    }

    // free the RE if compiled
    void Free()
    {
        if ( IsValid() )
        {
            wx_regfree(&m_RegEx);
        }

        delete m_Matches;
    }

    // free the RE if any and reinit the members
    void Reinit()
    {
        Free();
        Init();
    }

    // compiled RE
    regex_t         m_RegEx;

    // the subexpressions data
    wxRegExMatches *m_Matches;
    size_t          m_nMatches;

    // true if m_RegEx is valid
    bool            m_isCompiled;
};


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxRegExImpl
// ----------------------------------------------------------------------------

wxRegExImpl::wxRegExImpl()
{
    Init();
}

wxRegExImpl::~wxRegExImpl()
{
    Free();
}

wxString wxRegExImpl::GetErrorMsg(int errorcode) const
{
    wxString szError;

    // first get the string length needed
    int len = wx_regerror(errorcode, &m_RegEx, nullptr, 0);
    if ( len > 0 )
    {
        wxCharTypeBuffer<wxRegErrorChar> errbuf(len + 1);

        (void)wx_regerror(errorcode, &m_RegEx, errbuf.data(), errbuf.length());

        szError = errbuf;
    }

    if ( szError.empty() ) // regerror() returned 0 or conversion failed
    {
        szError = _("unknown error");
    }

    return szError;
}

// Helper function for processing bracket expressions inside a regex.
//
// Advance the iterator until the closing bracket matching the opening one the
// iterator currently points to, i.e.:
//
// Precondition: *it == '['
// Postcondition: *it == ']' or it == end if failed to find matching ']'
static
wxString::const_iterator
SkipBracketExpression(wxString::const_iterator it, wxString::const_iterator end)
{
    wxASSERT_MSG( *it == '[', "must be at the start of bracket expression" );

    // Initial ']', possibly after the preceding '^', is different because it
    // stands for a literal ']' and not the end of the bracket expression, so
    // check for it first.
    ++it;
    if ( it != end && *it == '^' )
        ++it;
    if ( it != end && *it == ']' )
        ++it;

    // Any ']' from now on ends the bracket expression.
    for ( ; it != end; ++it )
    {
        const wxUniChar c = *it;

        if ( c == ']' )
            break;

        if ( c == '[' )
        {
            // Bare '[' on its own is not special, but collating elements and
            // character classes are, so check for them and advance past them
            // if necessary to avoid misinterpreting the matching closing ']'.
            if ( ++it == end )
                break;

            const wxUniChar c2 = *it;
            if ( c2 == ':' || c2 == '.' || c2 == '=' )
            {
                for ( ++it; it != end; ++it )
                {
                    if ( *it == c2 )
                    {
                        if ( ++it == end )
                            break;

                        if ( *it == ']' )
                            break;
                    }
                }

                if ( it == end )
                    break;
            }
        }
    }

    return it;
}

/* static */
wxString wxRegEx::ConvertFromBasic(const wxString& bre)
{
    /*
        Quoting regex(7):

        Obsolete ("basic") regular expressions differ in several respects.
        '|', '+', and '?' are ordinary characters and there is no equivalent
        for their functionality. The delimiters for bounds are "\{" and "\}",
        with '{' and '}' by themselves ordinary characters. The parentheses
        for nested subexpressions are "\(" and "\)", with '(' and ')' by
        themselves ordinary characters. '^' is an ordinary character except at
        the beginning of the RE or(!) the beginning of a parenthesized
        subexpression, '$' is an ordinary character except at the end of the RE
        or(!) the end of a parenthesized subexpression, and '*' is an ordinary
        character if it appears at the beginning of the RE or the beginning of
        a parenthesized subexpression (after a possible leading '^').

        Finally, there is one new type of atom, a back reference: '\' followed
        by a nonzero decimal digit d matches the same sequence of characters
        matched by the dth parenthesized subexpression [...]
     */
    wxString ere;
    ere.reserve(bre.length());

    enum SinceStart
    {
        SinceStart_None,        // Just at the beginning.
        SinceStart_OnlyCaret,   // Had just "^" since the beginning.
        SinceStart_Some         // Had something else since the beginning.
    };

    struct State
    {
        explicit State(SinceStart sinceStart_)
        {
            isBackslash = false;
            sinceStart = sinceStart_;
        }

        bool isBackslash;
        SinceStart sinceStart;
    };

    State previous(SinceStart_None);
    for ( wxString::const_iterator it = bre.begin(),
                                  end = bre.end();
          it != end;
          ++it )
    {
        const wxUniChar c = *it;

        // What should be done with the current character?
        enum Disposition
        {
            Disposition_Skip,   // Nothing.
            Disposition_Append, // Append to output.
            Disposition_Escape  // ... after escaping it with backslash.
        } disposition = Disposition_Append;

        State current(SinceStart_Some);

        if ( previous.isBackslash )
        {
            // By default, keep the backslash present in the BRE, it's still
            // needed in the ERE too.
            disposition = Disposition_Escape;

            switch ( c.GetValue() )
            {
                case '(':
                    // It's the start of a new subexpression.
                    current.sinceStart = SinceStart_None;
                    wxFALLTHROUGH;

                case ')':
                case '{':
                case '}':
                    // Do not escape to ensure they remain special in the ERE
                    // as the escaped versions were special in the BRE.
                    disposition = Disposition_Append;
                    break;

                case '<':
                case '>':
                    // Map word boundaries extensions to POSIX syntax
                    // understood by PCRE.
                    ere += "[[:";
                    ere += c;
                    ere += ":]]";
                    disposition = Disposition_Skip;
                    break;
            }
        }
        else // This character is not escaped.
        {
            switch ( c.GetValue() )
            {
                case '\\':
                    current.isBackslash = true;

                    // Don't do anything with it yet, we'll deal with it later.
                    disposition = Disposition_Skip;
                    break;

                case '^':
                    // Escape unless it appears at the start.
                    switch ( previous.sinceStart )
                    {
                        case SinceStart_None:
                            // Don't escape, but do update the state.
                            current.sinceStart = SinceStart_OnlyCaret;
                            break;

                        case SinceStart_OnlyCaret:
                        case SinceStart_Some:
                            disposition = Disposition_Escape;
                            break;
                    }
                    break;

                case '*':
                    // Escape unless it appears at the start or right after "^".
                    switch ( previous.sinceStart )
                    {
                        case SinceStart_None:
                        case SinceStart_OnlyCaret:
                            disposition = Disposition_Escape;
                            break;

                        case SinceStart_Some:
                            break;
                    }
                    break;

                case '$':
                    // Escape unless it appears at the end or just before "\)".
                    disposition = Disposition_Escape;
                    {
                        wxString::const_iterator next = it;
                        ++next;
                        if ( next == end )
                        {
                            // It is at the end, so has special meaning.
                            disposition = Disposition_Append;
                        }
                        else // Not at the end, but maybe at subexpression end?
                        {
                            if ( *next == '\\' )
                            {
                                ++next;
                                if ( next != end && *next == ')' )
                                    disposition = Disposition_Append;
                            }
                        }
                    }
                    break;

                case '|':
                case '+':
                case '?':
                case '(':
                case ')':
                case '{':
                case '}':
                    // Escape these characters which are not special in a BRE,
                    // but would be special in a ERE if left unescaped.
                    disposition = Disposition_Escape;
                    break;

                case '[':
                    // Rules are very different for the characters inside the
                    // bracket expressions and we don't have to change anything
                    // for them as the syntax is the same for BREs and EREs, so
                    // just process the entire expression at once.
                    {
                        const wxString::const_iterator start = it;
                        it = SkipBracketExpression(it, end);

                        // Copy everything inside without any changes.
                        ere += wxString(start, it);

                        if ( it == end )
                        {
                            // If we reached the end without finding the
                            // matching ']' there is nothing remaining anyhow.
                            return ere;
                        }

                        // Note that default Disposition_Append here is fine,
                        // we'll append the closing ']' to "ere" below.
                    }
                    break;
            }
        }

        switch ( disposition )
        {
            case Disposition_Skip:
                break;

            case Disposition_Escape:
                ere += '\\';
                wxFALLTHROUGH;

            case Disposition_Append:
                // Note: don't use "c" here, iterator may have been advanced
                // inside the loop.
                ere += *it;
                break;
        }

        previous = current;
    }

    // It's an error if a RE ends with a backslash, but we still need to
    // preserve this error in the resulting RE.
    if ( previous.isBackslash )
        ere += '\\';

    return ere;
}

// Small helper for converting selected PCRE compilation options to string.
static wxString PCREOptionsToString(int opts)
{
    wxString s;

    if ( opts & PCRE2_CASELESS )
        s += 'i';
    if ( opts & PCRE2_MULTILINE )
        s += 'm';
    if ( opts & PCRE2_DOTALL )
        s += 's';
    if ( opts & PCRE2_EXTENDED )
        s += 'x';

    return s;
}

// Convert metasyntax, i.e. directors and embedded options, to PCRE syntax.
//
// See TCL re_syntax man page for more details.
static wxString ConvertMetasyntax(wxString expr, int& flags)
{
    // First check for directors that must occur only at the beginning.
    const int DIRECTOR_PREFIX_LEN = 3;
    if ( expr.length() > DIRECTOR_PREFIX_LEN && expr.StartsWith("***") )
    {
        switch ( expr[DIRECTOR_PREFIX_LEN].GetValue() )
        {
            // "***:" director indicates that the regex uses ARE syntax.
            case ':':
                flags &= ~wxRE_BASIC;
                flags |= wxRE_ADVANCED;
                expr.erase(0, DIRECTOR_PREFIX_LEN + 1);
                break;

            // "***=" director means that the rest is a literal string.
            case '=':
                // We could use PCRE2_LITERAL, but for now just use the "\Q"
                // escape that should result in the same way -- maybe even less
                // efficiently, but we probably don't really care about
                // performance in this very special case.
                flags &= ~(wxRE_BASIC | wxRE_ADVANCED);
                expr.replace(0, DIRECTOR_PREFIX_LEN + 1, "\\Q");
                break;

            default:
                // This is an invalid director that will result in a compile
                // error anyhow, so don't bother special-casing it and just
                // don't do anything to compile it and get an error later.
                break;
        }
    }

    // Then check for the embedded options that may occur at the beginning of
    // an ARE, but possibly after a director (necessarily the "***:" one).
    if ( (flags & wxRE_ADVANCED) && expr.StartsWith("(?") )
    {
        // String with the options: we use this for the options we don't know
        // about.
        wxString optsString;

        // PCRE options to enable or disable.
        int opts = 0,
            negopts = 0;

        // (Last) syntax selected by the options.
        enum Syntax
        {
            Syntax_None,
            Syntax_Basic,
            Syntax_Extended,
            Syntax_Literal
        } syntax = Syntax_None;

        const wxString::iterator end = expr.end();
        const wxString::iterator start = expr.begin() + 2;

        for ( wxString::iterator it = start; it != end; ++it )
        {
            if ( *it == ')' )
            {
                optsString += PCREOptionsToString(opts);

                if ( negopts )
                {
                    optsString += "-";
                    optsString += PCREOptionsToString(negopts);
                }

                size_t posAfterOpts;
                if ( optsString.empty() )
                {
                    expr.erase(expr.begin(), ++it);
                    posAfterOpts = 0;
                }
                else
                {
                    expr.replace(start, it, optsString);
                    posAfterOpts = optsString.length() + 3; // (?opts)
                }

                // Finally deal with the syntax selection.
                flags &= ~wxRE_ADVANCED;

                switch ( syntax )
                {
                    case Syntax_None:
                        flags |= wxRE_ADVANCED;
                        break;

                    case Syntax_Basic:
                        flags |= wxRE_BASIC;
                        break;

                    case Syntax_Extended:
                        flags |= wxRE_EXTENDED;
                        break;

                    case Syntax_Literal:
                        // As above, we could also use the LITERAL option, but
                        // this is simpler.
                        expr.insert(posAfterOpts, "\\Q");
                        break;
                }

                break;
            }

            // Avoid misinterpreting other constructs (non-capturing groups,
            // look ahead assertions etc) as options, which always consist in
            // alphabetic characters only.
            if ( *it < 'a' || *it > 'z' )
                break;

            switch ( (*it).GetValue() )
            {
                case 'b':
                    syntax = Syntax_Basic;
                    break;

                case 'e':
                    syntax = Syntax_Extended;
                    break;

                case 'q':
                    syntax = Syntax_Literal;
                    break;

                case 'm':
                case 'n':
                    // This option corresponds to MULTILINE PCRE option,
                    // without DOTALL, so enable the former and disable the
                    // latter.
                    negopts &= ~PCRE2_MULTILINE;
                    opts |= PCRE2_MULTILINE;
                    wxFALLTHROUGH;

                case 'p':
                    // This option corresponds to the default PCRE behaviour,
                    // but we use DOTALL by default, so turn it off (this might
                    // be unnecessary if wxRE_NEWLINE is also used, but it does
                    // no harm).
                    negopts |= PCRE2_DOTALL;
                    break;

                case 'w':
                    // This option corresponds to using both MULTILINE and
                    // DOTALL with PCRE.
                    negopts &= ~(PCRE2_MULTILINE | PCRE2_DOTALL);
                    opts |= PCRE2_MULTILINE | PCRE2_DOTALL;
                    break;

                case 'c':
                    // Disable case-insensitive matching.
                    negopts |= PCRE2_CASELESS;
                    break;

                case 't':
                    // Disable extended syntax.
                    negopts |= PCRE2_EXTENDED;
                    break;

                case 's':
                    // This option reverts to the default behaviour in the old
                    // regex library or enables DOTALL in PCRE, which is much
                    // more useful and common, so use it with PCRE meaning.
                    negopts &= ~PCRE2_DOTALL;
                    opts |= PCRE2_DOTALL;
                    break;

                    // These options have the same meaning as in PCRE.
                case 'i':
                    negopts &= ~PCRE2_CASELESS;
                    opts |= PCRE2_CASELESS;
                    break;

                case 'x':
                    negopts &= ~PCRE2_EXTENDED;
                    opts |= PCRE2_EXTENDED;
                    break;

                default:
                    // Keep the rest: could be a valid PCRE option or invalid
                    // option for both libraries, in which case we'll get an
                    // error, which is what we want.
                    optsString += *it;
                    break;
            }
        }
    }

    return expr;
}

// Convert "advanced" word boundary assertions to the syntax understood by PCRE.
//
// These extensions (known as "TCL extensions" because TCL uses the same regex
// library previous wx versions used) worked before, so preserve them for
// compatibility.
//
// Note that this does not take into account "\<" and "\>" (GNU extensions) as
// those are only valid when using BREs and so are taken care of above.
static wxString ConvertWordBoundaries(const wxString& expr)
{
    wxString out;
    out.reserve(expr.length());

    for ( wxString::const_iterator it = expr.begin(),
                                  end = expr.end();
          it != end;
          ++it )
    {
        if ( *it == '\\' )
        {
            ++it;
            if ( it == end )
            {
                out.append('\\');
                break;
            }

            const char* replacement = nullptr;
            switch ( (*it).GetValue() )
            {
                case 'm':
                    replacement = "[[:<:]]";
                    break;

                case 'M':
                    replacement = "[[:>:]]";
                    break;

                case 'y':
                    replacement = "\\b";
                    break;

                case 'Y':
                    replacement = "\\B";
                    break;
            }

            if ( replacement )
            {
                out.append(replacement);

                continue;
            }

            out.append('\\');
        }

        out.append(*it);
    }

    return out;
}

bool wxRegExImpl::Compile(wxString expr, int flags)
{
    Reinit();

    wxASSERT_MSG( !(flags & ~(wxRE_ADVANCED | wxRE_BASIC | wxRE_ICASE | wxRE_NOSUB | wxRE_NEWLINE)),
                  wxT("unrecognized flags in wxRegEx::Compile") );

    // Deal with the directors and embedded options first (this can modify
    // flags).
    expr = ConvertMetasyntax(expr, flags);

    // PCRE doesn't support BREs, translate them to EREs.
    if ( flags & wxRE_BASIC )
    {
        expr = wxRegEx::ConvertFromBasic(expr);
        flags &= ~wxRE_BASIC;
    }
    else if ( flags & wxRE_ADVANCED )
    {
        expr = ConvertWordBoundaries(expr);
    }

    // translate our flags to regcomp() ones
    int flagsRE = 0;
    if ( !(flags & wxRE_BASIC) )
        flagsRE |= REG_EXTENDED;
    if ( flags & wxRE_ICASE )
        flagsRE |= REG_ICASE;
    if ( flags & wxRE_NOSUB )
        flagsRE |= REG_NOSUB;
    if ( flags & wxRE_NEWLINE )
        flagsRE |= REG_NEWLINE;

#ifndef WXREGEX_CONVERT_TO_MB
    const wxChar *exprstr = expr.c_str();
#else
    const wxScopedCharBuffer exprbuf = expr.utf8_str();
    const char* const exprstr = exprbuf.data();
#endif

    // compile it
    int errorcode = wx_regcomp(&m_RegEx, exprstr, flagsRE);

    if ( errorcode )
    {
        wxLogError(_("Invalid regular expression '%s': %s"),
                   expr, GetErrorMsg(errorcode));

        m_isCompiled = false;
    }
    else // ok
    {
        // don't allocate the matches array now, but do it later if necessary
        if ( flags & wxRE_NOSUB )
        {
            // we don't need it at all
            m_nMatches = 0;
        }
        else
        {
            // we will alloc the array later (only if really needed) but count
            // the number of sub-expressions in the regex right now

            // there is always one for the whole expression
            m_nMatches = 1;

            // and some more for bracketed subexperessions
            for ( const wxChar *cptr = expr.c_str(); *cptr; cptr++ )
            {
                if ( *cptr == wxT('\\') )
                {
                    // in basic RE syntax groups are inside \(...\)
                    if ( *++cptr == wxT('(') && (flags & wxRE_BASIC) )
                    {
                        m_nMatches++;
                    }
                }
                else if ( *cptr == wxT('(') && !(flags & wxRE_BASIC) )
                {
                    // we know that the previous character is not an unquoted
                    // backslash because it would have been eaten above, so we
                    // have a bare '(' and this indicates a group start for the
                    // extended syntax. '(?' is used for extensions by perl-
                    // like REs (e.g. advanced), and is not valid for POSIX
                    // extended, so ignore them always.
                    if ( cptr[1] != wxT('?')
                        && cptr[1] != wxT('*')
                            )
                        m_nMatches++;
                }
            }
        }

        m_isCompiled = true;
    }

    return IsValid();
}

bool wxRegExImpl::Matches(const wxRegChar *str,
                          int flags,
                          size_t len) const
{
    wxCHECK_MSG( IsValid(), false, wxT("must successfully Compile() first") );

    // translate our flags to regexec() ones
    wxASSERT_MSG( !(flags & ~(wxRE_NOTBOL | wxRE_NOTEOL | wxRE_NOTEMPTY)),
                  wxT("unrecognized flags in wxRegEx::Matches") );

    int flagsRE = 0;
    if ( flags & wxRE_NOTBOL )
        flagsRE |= REG_NOTBOL;
    if ( flags & wxRE_NOTEOL )
        flagsRE |= REG_NOTEOL;
    if ( flags & wxRE_NOTEMPTY )
        flagsRE |= REG_NOTEMPTY;

    // allocate matches array if needed
    wxRegExImpl *self = wxConstCast(this, wxRegExImpl);
    if ( !m_Matches && m_nMatches )
    {
        self->m_Matches = new wxRegExMatches(m_nMatches);
    }

    wxRegExMatches::match_type matches = m_Matches ? m_Matches->get() : nullptr;

    // do match it
    int rc = wx_regexec(&self->m_RegEx, str, len, m_nMatches, matches, flagsRE);

    switch ( rc )
    {
        case 0:
            // matched successfully
            return true;

        default:
            // an error occurred
            wxLogError(_("Failed to find match for regular expression: %s"),
                       GetErrorMsg(rc));
            wxFALLTHROUGH;

        case REG_NOMATCH:
            // no match
            return false;
    }
}

bool wxRegExImpl::GetMatch(size_t *start, size_t *len, size_t index) const
{
    wxCHECK_MSG( IsValid(), false, wxT("must successfully Compile() first") );
    wxCHECK_MSG( m_nMatches, false, wxT("can't use with wxRE_NOSUB") );
    wxCHECK_MSG( m_Matches, false, wxT("must call Matches() first") );
    wxCHECK_MSG( index < m_nMatches, false, wxT("invalid match index") );

    if ( start )
        *start = m_Matches->Start(index);
    if ( len )
        *len = m_Matches->End(index) - m_Matches->Start(index);

    return true;
}

size_t wxRegExImpl::GetMatchCount() const
{
    wxCHECK_MSG( IsValid(), 0, wxT("must successfully Compile() first") );
    wxCHECK_MSG( m_nMatches, 0, wxT("can't use with wxRE_NOSUB") );

    return m_nMatches;
}

int wxRegExImpl::Replace(wxString *text,
                         const wxString& replacement,
                         size_t maxMatches) const
{
    wxCHECK_MSG( text, wxNOT_FOUND, wxT("null text in wxRegEx::Replace") );
    wxCHECK_MSG( IsValid(), wxNOT_FOUND, wxT("must successfully Compile() first") );

    // the input string
#ifndef WXREGEX_CONVERT_TO_MB
    const wxChar *textstr = text->c_str();
    size_t textlen = text->length();
#else
    const wxScopedCharBuffer textbuf = text->utf8_str();
    const char* const textstr = textbuf.data();
    size_t textlen = textbuf.length();
#endif

    // the replacement text
    wxString textNew;

    // the result, allow 25% extra
    wxString result;
    result.reserve(5 * textlen / 4);

    // attempt at optimization: don't iterate over the string if it doesn't
    // contain back references at all
    bool mayHaveBackrefs =
        replacement.find_first_of(wxT("\\&")) != wxString::npos;

    if ( !mayHaveBackrefs )
    {
        textNew = replacement;
    }

    // the position where we start looking for the match
    size_t matchStart = 0;

    // number of replacement made: we won't make more than maxMatches of them
    // (unless maxMatches is 0 which doesn't limit the number of replacements)
    size_t countRepl = 0;

    // note that "^" shouldn't match after the first call to Matches() so we
    // use wxRE_NOTBOL to prevent it from happening
    while ( (!maxMatches || countRepl < maxMatches) &&
             Matches(textstr + matchStart,
                     countRepl ? wxRE_NOTBOL : 0,
                     textlen - matchStart) )
    {
        // the string possibly contains back references: we need to calculate
        // the replacement text anew after each match
        if ( mayHaveBackrefs )
        {
            mayHaveBackrefs = false;
            textNew.clear();
            textNew.reserve(replacement.length());

            for ( const wxChar *p = replacement.c_str(); *p; p++ )
            {
                size_t index = (size_t)-1;

                if ( *p == wxT('\\') )
                {
                    if ( wxIsdigit(*++p) )
                    {
                        // back reference
                        wxChar *end;
                        index = (size_t)wxStrtoul(p, &end, 10);
                        p = end - 1; // -1 to compensate for p++ in the loop
                    }
                    //else: backslash used as escape character
                }
                else if ( *p == wxT('&') )
                {
                    // treat this as "\0" for compatbility with ed and such
                    index = 0;
                }

                // do we have a back reference?
                if ( index != (size_t)-1 )
                {
                    // yes, get its text
                    size_t start, len;
                    if ( !GetMatch(&start, &len, index) )
                    {
                        wxFAIL_MSG( wxT("invalid back reference") );

                        // just eat it...
                    }
                    else
                    {
                        textNew += wxString(textstr + matchStart + start, len);

                        mayHaveBackrefs = true;
                    }
                }
                else // ordinary character
                {
                    textNew += *p;
                }
            }
        }

        size_t start, len;
        if ( !GetMatch(&start, &len) )
        {
            // we did have match as Matches() returned true above!
            wxFAIL_MSG( wxT("internal logic error in wxRegEx::Replace") );

            return wxNOT_FOUND;
        }

        // an insurance against implementations that don't grow exponentially
        // to ensure building the result takes linear time
        if (result.capacity() < result.length() + start + textNew.length())
            result.reserve(2 * result.length());

        result.append(wxString(textstr + matchStart, start));
        matchStart += start;
        result.append(textNew);

        countRepl++;

        matchStart += len;
    }

    result.append(wxString(textstr + matchStart));
    *text = result;

    return countRepl;
}

// ----------------------------------------------------------------------------
// wxRegEx: all methods are mostly forwarded to wxRegExImpl
// ----------------------------------------------------------------------------

void wxRegEx::Init()
{
    m_impl = nullptr;
}

wxRegEx::~wxRegEx()
{
    delete m_impl;
}

bool wxRegEx::Compile(const wxString& expr, int flags)
{
    if ( !m_impl )
    {
        m_impl = new wxRegExImpl;
    }

    if ( !m_impl->Compile(expr, flags) )
    {
        // error message already given in wxRegExImpl::Compile
        wxDELETE(m_impl);

        return false;
    }

    return true;
}

bool wxRegEx::Matches(const wxString& str, int flags) const
{
    wxCHECK_MSG( IsValid(), false, wxT("must successfully Compile() first") );

#ifndef WXREGEX_CONVERT_TO_MB
    const wxChar* const textstr = str.c_str();
    const size_t textlen = str.length();
#else
    const wxScopedCharBuffer textstr = str.utf8_str();
    const size_t textlen = textstr.length();
#endif

    return m_impl->Matches(textstr, flags, textlen);
}

bool wxRegEx::GetMatch(size_t *start, size_t *len, size_t index) const
{
    wxCHECK_MSG( IsValid(), false, wxT("must successfully Compile() first") );

    return m_impl->GetMatch(start, len, index);
}

wxString wxRegEx::GetMatch(const wxString& text, size_t index) const
{
    size_t start, len;
    if ( !GetMatch(&start, &len, index) )
        return wxEmptyString;

#ifndef WXREGEX_CONVERT_TO_MB
    return text.Mid(start, len);
#else
    return wxString::FromUTF8(text.utf8_str().data() + start, len);
#endif
}

size_t wxRegEx::GetMatchCount() const
{
    wxCHECK_MSG( IsValid(), 0, wxT("must successfully Compile() first") );

    return m_impl->GetMatchCount();
}

int wxRegEx::Replace(wxString *pattern,
                     const wxString& replacement,
                     size_t maxMatches) const
{
    wxCHECK_MSG( IsValid(), wxNOT_FOUND, wxT("must successfully Compile() first") );

    return m_impl->Replace(pattern, replacement, maxMatches);
}

wxString wxRegEx::QuoteMeta(const wxString& str)
{
    static const wxString s_strMetaChars = wxS("\\^$.|?*+()[]{}");

    wxString strEscaped;

    // This is the maximal possible length of the resulting string, if every
    // character were escaped.
    strEscaped.reserve(str.length() * 2);

    for ( wxString::const_iterator it = str.begin(); it != str.end(); ++it )
    {
        if ( s_strMetaChars.find(*it) != wxString::npos )
        {
            strEscaped += wxS('\\');
        }

        strEscaped += *it;
    }

    strEscaped.shrink_to_fit();

    return strEscaped;
}

/* static */
wxVersionInfo wxRegEx::GetLibraryVersionInfo()
{
    wxRegChar buf[64];
    pcre2_config(PCRE2_CONFIG_VERSION, buf);

    return wxVersionInfo("PCRE2", PCRE2_MAJOR, PCRE2_MINOR, 0, buf);
}

#endif // wxUSE_REGEX
