/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.cpp
// Purpose:     String tokenizer
// Author:      Guilhem Lavaux
// Modified by: Vadim Zeitlin (almost full rewrite)
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "tokenzr.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/tokenzr.h"

// Required for wxIs... functions
#include <ctype.h>

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStringTokenizer construction
// ----------------------------------------------------------------------------

wxStringTokenizer::wxStringTokenizer(const wxString& str,
                                     const wxString& delims,
                                     wxStringTokenizerMode mode)
{
    SetString(str, delims, mode);
}

void wxStringTokenizer::SetString(const wxString& str,
                                  const wxString& delims,
                                  wxStringTokenizerMode mode)
{
    if ( mode == wxTOKEN_DEFAULT )
    {
        // by default, we behave like strtok() if the delimiters are only
        // whitespace characters and as wxTOKEN_RET_EMPTY otherwise (for
        // whitespace delimiters, strtok() behaviour is better because we want
        // to count consecutive spaces as one delimiter)
        const wxChar *p;
        for ( p = delims.c_str(); *p; p++ )
        {
            if ( !wxIsspace(*p) )
                break;
        }

        if ( *p )
        {
            // not whitespace char in delims
            mode = wxTOKEN_RET_EMPTY;
        }
        else
        {
            // only whitespaces
            mode = wxTOKEN_STRTOK;
        }
    }

    m_delims = delims;
    m_mode = mode;

    Reinit(str);
}

void wxStringTokenizer::Reinit(const wxString& str)
{
    wxASSERT_MSG( IsOk(), _T("you should call SetString() first") );

    m_string = str;
    m_pos = 0;

    // empty string doesn't have any tokens
    m_hasMore = !m_string.empty();
}

// ----------------------------------------------------------------------------
// access to the tokens
// ----------------------------------------------------------------------------

// do we have more of them?
bool wxStringTokenizer::HasMoreTokens() const
{
    wxCHECK_MSG( IsOk(), FALSE, _T("you should call SetString() first") );

    if ( m_string.find_first_not_of(m_delims) == wxString::npos )
    {
        // no non empty tokens left, but in 2 cases we still may return TRUE if
        // GetNextToken() wasn't called yet for this empty token:
        //
        //   a) in wxTOKEN_RET_EMPTY_ALL mode we always do it
        //   b) in wxTOKEN_RET_EMPTY mode we do it in the special case of a
        //      string containing only the delimiter: then there is an empty
        //      token just before it
        return (m_mode == wxTOKEN_RET_EMPTY_ALL) ||
               (m_mode == wxTOKEN_RET_EMPTY && m_pos == 0)
                    ? m_hasMore : FALSE;
    }
    else
    {
        // there are non delimiter characters left, hence we do have more
        // tokens
        return TRUE;
    }
}

// count the number of tokens in the string
size_t wxStringTokenizer::CountTokens() const
{
    wxCHECK_MSG( IsOk(), 0, _T("you should call SetString() first") );

    // VZ: this function is IMHO not very useful, so it's probably not very
    //     important if it's implementation here is not as efficient as it
    //     could be - but OTOH like this we're sure to get the correct answer
    //     in all modes
    wxStringTokenizer *self = (wxStringTokenizer *)this;    // const_cast
    wxString stringInitial = m_string;

    size_t count = 0;
    while ( self->HasMoreTokens() )
    {
        count++;

        (void)self->GetNextToken();
    }

    self->Reinit(stringInitial);

    return count;
}

// ----------------------------------------------------------------------------
// token extraction
// ----------------------------------------------------------------------------

wxString wxStringTokenizer::GetNextToken()
{
    // strtok() doesn't return empty tokens, all other modes do
    bool allowEmpty = m_mode != wxTOKEN_STRTOK;

    wxString token;
    do
    {
        if ( !HasMoreTokens() )
        {
            break;
        }
        // find the end of this token
        size_t pos = m_string.find_first_of(m_delims);

        // and the start of the next one
        if ( pos == wxString::npos )
        {
            // no more delimiters, the token is everything till the end of
            // string
            token = m_string;

            m_pos += m_string.length();
            m_string.clear();

            // no more tokens in this string, even in wxTOKEN_RET_EMPTY_ALL
            // mode (we will return the trailing one right now in this case)
            m_hasMore = FALSE;
        }
        else
        {
            size_t pos2 = pos + 1;

            // in wxTOKEN_RET_DELIMS mode we return the delimiter character
            // with token
            token = wxString(m_string, m_mode == wxTOKEN_RET_DELIMS ? pos2
                                                                    : pos);

            // remove token with the following it delimiter from string
            m_string.erase(0, pos2);

            // keep track of the position in the original string too
            m_pos += pos2;
        }
    }
    while ( !allowEmpty && token.empty() );

    return token;
}

// ----------------------------------------------------------------------------
// public functions
// ----------------------------------------------------------------------------

wxArrayString wxStringTokenize(const wxString& str,
                               const wxString& delims,
                               wxStringTokenizerMode mode)
{
    wxArrayString tokens;
    wxStringTokenizer tk(str, delims, mode);
    while ( tk.HasMoreTokens() )
    {
        tokens.Add(tk.GetNextToken());
    }

    return tokens;
}
