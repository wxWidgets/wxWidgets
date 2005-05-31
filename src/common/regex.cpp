///////////////////////////////////////////////////////////////////////////////
// Name:        src/common/regex.cpp
// Purpose:     regular expression matching
// Author:      Karsten Ballüder and Vadim Zeitlin
// Modified by:
// Created:     13.07.01
// RCS-ID:      $Id$
// Copyright:   (c) 2000 Karsten Ballüder <ballueder@gmx.net>
//                  2001 Vadim Zeitlin <vadim@wxwindows.org>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#if defined(__GNUG__) && !defined(NO_GCC_PRAGMA)
    #pragma implementation "regex.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_REGEX

#ifndef WX_PRECOMP
    #include "wx/object.h"
    #include "wx/string.h"
    #include "wx/log.h"
    #include "wx/intl.h"
#endif //WX_PRECOMP

// FreeBSD, Watcom and DMars require this, CW doesn't have nor need it.
// Others also don't seem to need it. If you have an error related to
// (not) including <sys/types.h> please report details to
// wx-dev@lists.wxwindows.org
#if defined(__UNIX__) || defined(__WATCOMC__) || defined(__DIGITALMARS__)
#   include <sys/types.h>
#endif

#include <regex.h>
#include "wx/regex.h"

// ----------------------------------------------------------------------------
// private classes
// ----------------------------------------------------------------------------

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
    bool Matches(const wxChar *str, int flags = 0) const;
    bool GetMatch(size_t *start, size_t *len, size_t index = 0) const;
    size_t GetMatchCount() const;
    int Replace(wxString *pattern, const wxString& replacement,
                size_t maxMatches = 0) const;

private:
    // return the string containing the error message for the given err code
    wxString GetErrorMsg(int errorcode, bool badconv) const;

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
            regfree(&m_RegEx);
        }

        delete [] m_Matches;
    }

    // free the RE if any and reinit the members
    void Reinit()
    {
        Free();
        Init();
    }


    // compiled RE
    regex_t     m_RegEx;

    // the subexpressions data
    regmatch_t *m_Matches;
    size_t      m_nMatches;

    // true if m_RegEx is valid
    bool        m_isCompiled;
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

wxString wxRegExImpl::GetErrorMsg(int errorcode, bool badconv) const
{
#if wxUSE_UNICODE && !defined(__REG_NOFRONT)
    // currently only needed when using system library in Unicode mode
    if ( badconv )
    {
        return _("conversion to 8-bit encoding failed");
    }
#else
    // 'use' badconv to avoid a compiler warning
    (void)badconv;
#endif

    wxString szError;

    // first get the string length needed
    int len = regerror(errorcode, &m_RegEx, NULL, 0);
    if ( len > 0 )
    {
        char* szcmbError = new char[++len];

        (void)regerror(errorcode, &m_RegEx, szcmbError, len);

        szError = wxConvertMB2WX(szcmbError);
        delete [] szcmbError;
    }
    else // regerror() returned 0
    {
        szError = _("unknown error");
    }

    return szError;
}

bool wxRegExImpl::Compile(const wxString& expr, int flags)
{
    Reinit();

#ifdef WX_NO_REGEX_ADVANCED
#   define FLAVORS wxRE_BASIC
#else
#   define FLAVORS (wxRE_ADVANCED | wxRE_BASIC)
    wxASSERT_MSG( (flags & FLAVORS) != FLAVORS,
                  _T("incompatible flags in wxRegEx::Compile") );
#endif
    wxASSERT_MSG( !(flags & ~(FLAVORS | wxRE_ICASE | wxRE_NOSUB | wxRE_NEWLINE)),
                  _T("unrecognized flags in wxRegEx::Compile") );

    // translate our flags to regcomp() ones
    int flagsRE = 0;
    if ( !(flags & wxRE_BASIC) )
#ifndef WX_NO_REGEX_ADVANCED
        if (flags & wxRE_ADVANCED)
            flagsRE |= REG_ADVANCED;
        else
#endif
            flagsRE |= REG_EXTENDED;
    if ( flags & wxRE_ICASE )
        flagsRE |= REG_ICASE;
    if ( flags & wxRE_NOSUB )
        flagsRE |= REG_NOSUB;
    if ( flags & wxRE_NEWLINE )
        flagsRE |= REG_NEWLINE;

    // compile it
#ifdef __REG_NOFRONT
    bool conv = true;
    int errorcode = wx_re_comp(&m_RegEx, expr, expr.length(), flagsRE);
#else
    const wxWX2MBbuf conv = expr.mbc_str();
    int errorcode = conv ? regcomp(&m_RegEx, conv, flagsRE) : REG_BADPAT;
#endif

    if ( errorcode )
    {
        wxLogError(_("Invalid regular expression '%s': %s"),
                   expr.c_str(), GetErrorMsg(errorcode, !conv).c_str());

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
                if ( *cptr == _T('\\') )
                {
                    // in basic RE syntax groups are inside \(...\)
                    if ( *++cptr == _T('(') && (flags & wxRE_BASIC) )
                    {
                        m_nMatches++;
                    }
                }
                else if ( *cptr == _T('(') && !(flags & wxRE_BASIC) )
                {
                    // we know that the previous character is not an unquoted
                    // backslash because it would have been eaten above, so we
                    // have a bare '(' and this indicates a group start for the
                    // extended syntax. '(?' is used for extensions by perl-
                    // like REs (e.g. advanced), and is not valid for POSIX
                    // extended, so ignore them always.
                    if ( cptr[1] != _T('?') )
                        m_nMatches++;
                }
            }
        }

        m_isCompiled = true;
    }

    return IsValid();
}

bool wxRegExImpl::Matches(const wxChar *str, int flags) const
{
    wxCHECK_MSG( IsValid(), false, _T("must successfully Compile() first") );

    // translate our flags to regexec() ones
    wxASSERT_MSG( !(flags & ~(wxRE_NOTBOL | wxRE_NOTEOL)),
                  _T("unrecognized flags in wxRegEx::Matches") );

    int flagsRE = 0;
    if ( flags & wxRE_NOTBOL )
        flagsRE |= REG_NOTBOL;
    if ( flags & wxRE_NOTEOL )
        flagsRE |= REG_NOTEOL;

    // allocate matches array if needed
    wxRegExImpl *self = wxConstCast(this, wxRegExImpl);
    if ( !m_Matches && m_nMatches )
    {
        self->m_Matches = new regmatch_t[m_nMatches];
    }

    // do match it
#ifdef __REG_NOFRONT
    bool conv = true;
    int rc = wx_re_exec(&self->m_RegEx, str, wxStrlen(str), NULL, m_nMatches, m_Matches, flagsRE);
#else
    const wxWX2MBbuf conv = wxConvertWX2MB(str);
    int rc = conv ? regexec(&self->m_RegEx, conv, m_nMatches, m_Matches, flagsRE) : REG_BADPAT;
#endif

    switch ( rc )
    {
        case 0:
            // matched successfully
            return true;

        default:
            // an error occurred
            wxLogError(_("Failed to match '%s' in regular expression: %s"),
                       str, GetErrorMsg(rc, !conv).c_str());
            // fall through

        case REG_NOMATCH:
            // no match
            return false;
    }
}

bool wxRegExImpl::GetMatch(size_t *start, size_t *len, size_t index) const
{
    wxCHECK_MSG( IsValid(), false, _T("must successfully Compile() first") );
    wxCHECK_MSG( m_nMatches, false, _T("can't use with wxRE_NOSUB") );
    wxCHECK_MSG( m_Matches, false, _T("must call Matches() first") );
    wxCHECK_MSG( index < m_nMatches, false, _T("invalid match index") );

    const regmatch_t& match = m_Matches[index];

    if ( start )
        *start = match.rm_so;
    if ( len )
        *len = match.rm_eo - match.rm_so;

    return true;
}

size_t wxRegExImpl::GetMatchCount() const
{
    wxCHECK_MSG( IsValid(), 0, _T("must successfully Compile() first") );
    wxCHECK_MSG( m_nMatches, 0, _T("can't use with wxRE_NOSUB") );

    return m_nMatches;
}

int wxRegExImpl::Replace(wxString *text,
                         const wxString& replacement,
                         size_t maxMatches) const
{
    wxCHECK_MSG( text, wxNOT_FOUND, _T("NULL text in wxRegEx::Replace") );
    wxCHECK_MSG( IsValid(), wxNOT_FOUND, _T("must successfully Compile() first") );

    // the replacement text
    wxString textNew;

    // attempt at optimization: don't iterate over the string if it doesn't
    // contain back references at all
    bool mayHaveBackrefs =
        replacement.find_first_of(_T("\\&")) != wxString::npos;

    if ( !mayHaveBackrefs )
    {
        textNew = replacement;
    }

    // the position where we start looking for the match
    //
    // NB: initial version had a nasty bug because it used a wxChar* instead of
    //     an index but the problem is that replace() in the loop invalidates
    //     all pointers into the string so we have to use indices instead
    size_t matchStart = 0;

    // number of replacement made: we won't make more than maxMatches of them
    // (unless maxMatches is 0 which doesn't limit the number of replacements)
    size_t countRepl = 0;

    // note that "^" shouldn't match after the first call to Matches() so we
    // use wxRE_NOTBOL to prevent it from happening
    while ( (!maxMatches || countRepl < maxMatches) &&
            Matches(text->c_str() + matchStart, countRepl ? wxRE_NOTBOL : 0) )
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

                if ( *p == _T('\\') )
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
                else if ( *p == _T('&') )
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
                        wxFAIL_MSG( _T("invalid back reference") );

                        // just eat it...
                    }
                    else
                    {
                        textNew += wxString(text->c_str() + matchStart + start,
                                            len);

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
            wxFAIL_MSG( _T("internal logic error in wxRegEx::Replace") );

            return wxNOT_FOUND;
        }

        matchStart += start;
        text->replace(matchStart, len, textNew);

        countRepl++;

        matchStart += textNew.length();
    }

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
        delete m_impl;
        m_impl = NULL;

        return false;
    }

    return true;
}

bool wxRegEx::Matches(const wxChar *str, int flags) const
{
    wxCHECK_MSG( IsValid(), false, _T("must successfully Compile() first") );

    return m_impl->Matches(str, flags);
}

bool wxRegEx::GetMatch(size_t *start, size_t *len, size_t index) const
{
    wxCHECK_MSG( IsValid(), false, _T("must successfully Compile() first") );

    return m_impl->GetMatch(start, len, index);
}

wxString wxRegEx::GetMatch(const wxString& text, size_t index) const
{
    size_t start, len;
    if ( !GetMatch(&start, &len, index) )
        return wxEmptyString;

    return text.Mid(start, len);
}

size_t wxRegEx::GetMatchCount() const
{
    wxCHECK_MSG( IsValid(), 0, _T("must successfully Compile() first") );

    return m_impl->GetMatchCount();
}

int wxRegEx::Replace(wxString *pattern,
                     const wxString& replacement,
                     size_t maxMatches) const
{
    wxCHECK_MSG( IsValid(), wxNOT_FOUND, _T("must successfully Compile() first") );

    return m_impl->Replace(pattern, replacement, maxMatches);
}

#endif // wxUSE_REGEX
