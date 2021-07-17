///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/regex.cpp
// Purpose:     regular expression matching
// Author:      Karsten Ballueder and Vadim Zeitlin
// Modified by:
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

#include <regex.h>

// WXREGEX_USING_BUILTIN    defined when using the built-in regex lib
// WXREGEX_USING_RE_SEARCH  defined when using re_search in the GNU regex lib
// WXREGEX_CONVERT_TO_MB    defined when the regex lib is using chars and
//                          wxChar is wide, so conversion to UTF-8 must be done
//
#ifdef __REG_NOFRONT
#   define WXREGEX_USING_BUILTIN
#else
#   ifdef HAVE_RE_SEARCH
#       define WXREGEX_USING_RE_SEARCH
#   else
        // We can't use length, so just drop it in this wrapper.
        inline int
        wx_regexec(const regex_t* preg, const char* string, size_t,
                   size_t nmatch, regmatch_t* pmatch, int eflags)
        {
            return regexec(preg, string, nmatch, pmatch, eflags);
        }
#   endif
#   if wxUSE_UNICODE
#       define WXREGEX_CONVERT_TO_MB
#   endif
#   define wx_regcomp regcomp
#   define wx_regfree regfree
#   define wx_regerror regerror
#endif

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

#ifndef WXREGEX_USING_RE_SEARCH

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

#else // WXREGEX_USING_RE_SEARCH

// the array of offsets for the matches, the struct used by the GNU lib
class wxRegExMatches
{
public:
    typedef re_registers *match_type;

    wxRegExMatches(size_t n)
    {
        m_matches.num_regs = n;
        m_matches.start = new regoff_t[n];
        m_matches.end = new regoff_t[n];
    }

    ~wxRegExMatches()
    {
        delete [] m_matches.start;
        delete [] m_matches.end;
    }

    size_t Start(size_t n) const    { return m_matches.start[n]; }
    size_t End(size_t n) const      { return m_matches.end[n]; }

    re_registers *get()             { return &m_matches; }

private:
    re_registers m_matches;
};

#endif // WXREGEX_USING_RE_SEARCH

// the character type used by the regular expression engine
#ifndef WXREGEX_CONVERT_TO_MB
typedef wxChar wxRegChar;
#else
typedef char wxRegChar;
#endif

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
    bool Compile(const wxString& expr, int flags = 0);
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
        m_Matches = NULL;
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
    int len = wx_regerror(errorcode, &m_RegEx, NULL, 0);
    if ( len > 0 )
    {
        wxCharBuffer errbuf(len);

        (void)wx_regerror(errorcode, &m_RegEx, errbuf.data(), errbuf.length());

        szError = wxConvLibc.cMB2WX(errbuf);
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

bool wxRegExImpl::Compile(const wxString& expr, int flags)
{
    Reinit();

#ifdef WX_NO_REGEX_ADVANCED
#   define FLAVORS wxRE_BASIC
#else
#   define FLAVORS (wxRE_ADVANCED | wxRE_BASIC)
    wxASSERT_MSG( (flags & FLAVORS) != FLAVORS,
                  wxT("incompatible flags in wxRegEx::Compile") );
#endif
    wxASSERT_MSG( !(flags & ~(FLAVORS | wxRE_ICASE | wxRE_NOSUB | wxRE_NEWLINE)),
                  wxT("unrecognized flags in wxRegEx::Compile") );

    // translate our flags to regcomp() ones
    int flagsRE = 0;
    if ( !(flags & wxRE_BASIC) )
    {
#ifndef WX_NO_REGEX_ADVANCED
        if (flags & wxRE_ADVANCED)
            flagsRE |= REG_ADVANCED;
        else
#endif
            flagsRE |= REG_EXTENDED;
    }
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
#ifdef WXREGEX_USING_BUILTIN
    int errorcode = wx_re_comp(&m_RegEx, exprstr, expr.length(), flagsRE);
#else
    int errorcode = wx_regcomp(&m_RegEx, exprstr, flagsRE);
#endif

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
                    if ( cptr[1] != wxT('?') )
                        m_nMatches++;
                }
            }
        }

        m_isCompiled = true;
    }

    return IsValid();
}

#ifdef WXREGEX_USING_RE_SEARCH

// On GNU, regexec is implemented as a wrapper around re_search. re_search
// requires a length parameter which the POSIX regexec does not have,
// therefore regexec must do a strlen on the search text each time it is
// called. This can drastically affect performance when matching is done in
// a loop along a string, such as during a search and replace. Therefore if
// re_search is detected by configure, it is used directly.
//
static int ReSearch(const regex_t *preg,
                    const char *text,
                    size_t len,
                    re_registers *matches,
                    int eflags)
{
    regex_t *pattern = const_cast<regex_t*>(preg);

    pattern->not_bol = (eflags & REG_NOTBOL) != 0;
    pattern->not_eol = (eflags & REG_NOTEOL) != 0;
    pattern->regs_allocated = REGS_FIXED;

    int ret = re_search(pattern, text, len, 0, len, matches);
    return ret >= 0 ? 0 : REG_NOMATCH;
}

#endif // WXREGEX_USING_RE_SEARCH

bool wxRegExImpl::Matches(const wxRegChar *str,
                          int flags,
                          size_t len) const
{
    wxCHECK_MSG( IsValid(), false, wxT("must successfully Compile() first") );

    // translate our flags to regexec() ones
    wxASSERT_MSG( !(flags & ~(wxRE_NOTBOL | wxRE_NOTEOL)),
                  wxT("unrecognized flags in wxRegEx::Matches") );

    int flagsRE = 0;
    if ( flags & wxRE_NOTBOL )
        flagsRE |= REG_NOTBOL;
    if ( flags & wxRE_NOTEOL )
        flagsRE |= REG_NOTEOL;

    // allocate matches array if needed
    wxRegExImpl *self = wxConstCast(this, wxRegExImpl);
    if ( !m_Matches && m_nMatches )
    {
        self->m_Matches = new wxRegExMatches(m_nMatches);
    }

    wxRegExMatches::match_type matches = m_Matches ? m_Matches->get() : NULL;

    // do match it
#if defined WXREGEX_USING_BUILTIN
    int rc = wx_re_exec(&self->m_RegEx, str, len, NULL, m_nMatches, matches, flagsRE);
#elif defined WXREGEX_USING_RE_SEARCH
    int rc = ReSearch(&self->m_RegEx, str, len, matches, flagsRE);
#else
    int rc = wx_regexec(&self->m_RegEx, str, len, m_nMatches, matches, flagsRE);
#endif

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
    wxCHECK_MSG( text, wxNOT_FOUND, wxT("NULL text in wxRegEx::Replace") );
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
                        textNew += wxString(textstr + matchStart + start,
                                            wxConvUTF8, len);

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

        result.append(wxString(textstr + matchStart, wxConvUTF8, start));
        matchStart += start;
        result.append(textNew);

        countRepl++;

        matchStart += len;
    }

    result.append(wxString(textstr + matchStart, wxConvUTF8));
    *text = result;

    return countRepl;
}

// ----------------------------------------------------------------------------
// wxRegEx: all methods are mostly forwarded to wxRegExImpl
// ----------------------------------------------------------------------------

void wxRegEx::Init()
{
    m_impl = NULL;
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

#endif // wxUSE_REGEX
