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

#ifdef __GNUG__
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

    // return TRUE if Compile() had been called successfully
    bool IsValid() const { return m_isCompiled; }

    // RE operations
    bool Compile(const wxString& expr, int flags);
    bool Matches(const wxString& str, int flags) const;
    bool GetMatch(size_t *start, size_t *len, size_t index) const;
    int Replace(wxString *pattern, const wxString& replacement) const;

private:
    // return the string containing the error message for the given err code
    wxString GetErrorMsg(int errorcode) const;

    // free the RE if compiled
    void Free()
    {
        if ( IsValid() )
        {
            regfree(&m_RegEx);

            m_isCompiled = FALSE;
        }
    }

    // compiled RE
    regex_t     m_RegEx;

    // the subexpressions data
    regmatch_t *m_Matches;
    size_t      m_nMatches;

    // TRUE if m_RegEx is valid
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
    m_isCompiled = FALSE;
    m_Matches = NULL;
}

wxRegExImpl::~wxRegExImpl()
{
    Free();

    delete [] m_Matches;
}

wxString wxRegExImpl::GetErrorMsg(int errorcode) const
{
    wxString msg;

    // first get the string length needed
    int len = regerror(errorcode, &m_RegEx, NULL, 0);
    if ( len > 0 )
    {
        len++;

        (void)regerror(errorcode, &m_RegEx, msg.GetWriteBuf(len), len);

        msg.UngetWriteBuf();
    }
    else // regerror() returned 0
    {
        msg = _("unknown error");
    }

    return msg;
}

bool wxRegExImpl::Compile(const wxString& expr, int flags)
{
    Free();

    // translate our flags to regcomp() ones
    wxASSERT_MSG( !(flags &
                        ~(wxRE_BASIC | wxRE_ICASE | wxRE_NOSUB | wxRE_NEWLINE)),
                  _T("unrecognized flags in wxRegEx::Compile") );

    int flagsRE = 0;
    if ( !(flags & wxRE_BASIC) )
        flagsRE |= REG_EXTENDED;
    if ( flags & wxRE_ICASE )
        flagsRE |= REG_ICASE;
    if ( flags & wxRE_NOSUB )
        flagsRE |= REG_NOSUB;
    if ( flags & wxRE_NEWLINE )
        flagsRE |= REG_NEWLINE;

    // compile it
    int errorcode = regcomp(&m_RegEx, expr, flagsRE);
    if ( errorcode )
    {
        wxLogError(_("Invalid regular expression '%s': %s"),
                   expr.c_str(), GetErrorMsg(errorcode).c_str());

        m_isCompiled = FALSE;
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
            // will alloc later
            m_nMatches = WX_REGEX_MAXMATCHES;
        }

        m_isCompiled = TRUE;
    }

    return IsValid();
}

bool wxRegExImpl::Matches(const wxString& str, int flags) const
{
    wxCHECK_MSG( IsValid(), FALSE, _T("must successfully Compile() first") );

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
    int rc = regexec(&self->m_RegEx, str, m_nMatches, m_Matches, flagsRE);

    switch ( rc )
    {
        case 0:
            // matched successfully
            return TRUE;

        default:
            // an error occured
            wxLogError(_("Failed to match '%s' in regular expression: %s"),
                       str.c_str(), GetErrorMsg(rc).c_str());
            // fall through

        case REG_NOMATCH:
            // no match
            return FALSE;
    }
}

bool wxRegExImpl::GetMatch(size_t *start, size_t *len, size_t index) const
{
    wxCHECK_MSG( IsValid(), FALSE, _T("must successfully Compile() first") );
    wxCHECK_MSG( m_Matches, FALSE, _T("can't use with wxRE_NOSUB") );
    wxCHECK_MSG( index < m_nMatches, FALSE, _T("invalid match index") );

    const regmatch_t& match = m_Matches[index];
    if ( match.rm_so == -1 )
        return FALSE;

    if ( start )
        *start = match.rm_so;
    if ( len )
        *len = match.rm_eo - match.rm_so;

    return TRUE;
}

int wxRegExImpl::Replace(wxString *pattern, const wxString& replacement) const
{
    wxCHECK_MSG( pattern, -1, _T("NULL pattern in wxRegEx::Replace") );

    wxCHECK_MSG( IsValid(), FALSE, _T("must successfully Compile() first") );

    int replaced = 0;
    size_t lastpos = 0;
    wxString newstring;

    for ( size_t idx = 0;
          m_Matches[idx].rm_so != -1 && idx < m_nMatches;
          idx++ )
    {
        // copy non-matching bits:
        newstring << pattern->Mid(lastpos, m_Matches[idx].rm_so - lastpos);
        // copy replacement:
        newstring << replacement;
        // remember how far we got:
        lastpos = m_Matches[idx].rm_eo;
        replaced ++;
    }
    if(replaced > 0)
        *pattern = newstring;
    return replaced;
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

        return FALSE;
    }

    return TRUE;
}

bool wxRegEx::Matches(const wxString& str, int flags) const
{
    wxCHECK_MSG( IsValid(), FALSE, _T("must successfully Compile() first") );

    return m_impl->Matches(str, flags);
}

bool wxRegEx::GetMatch(size_t *start, size_t *len, size_t index) const
{
    wxCHECK_MSG( IsValid(), -1, _T("must successfully Compile() first") );

    return m_impl->GetMatch(start, len, index);
}

wxString wxRegEx::GetMatch(const wxString& text, size_t index) const
{
    size_t start, len;
    if ( !GetMatch(&start, &len, index) )
        return wxEmptyString;

    return text.Mid(start, len);
}

int wxRegEx::Replace(wxString *pattern, const wxString& replacement) const
{
    wxCHECK_MSG( IsValid(), -1, _T("must successfully Compile() first") );

    return m_impl->Replace(pattern, replacement);
}

#endif // wxUSE_REGEX
