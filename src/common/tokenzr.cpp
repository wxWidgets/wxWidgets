/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.cpp
// Purpose:     String tokenizer
// Author:      Guilhem Lavaux
// Modified by: Vadim Zeitlin
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

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxStringTokenizer construction
// ----------------------------------------------------------------------------

wxStringTokenizer::wxStringTokenizer(const wxString& to_tokenize,
                                     const wxString& delims,
                                     bool ret_delims)
{
    SetString(to_tokenize, delims, ret_delims);
}

void wxStringTokenizer::SetString(const wxString& to_tokenize,
                                  const wxString& delims,
                                  bool ret_delim)
{
    m_string = to_tokenize;
    m_delims = delims;
    m_retdelims = ret_delim;
    m_pos = 0;

    // empty string doesn't have any tokens
    m_hasMore = !m_string.empty();
}

wxStringTokenizer::~wxStringTokenizer()
{
}

// ----------------------------------------------------------------------------
// count the number of tokens in the string
// ----------------------------------------------------------------------------

size_t wxStringTokenizer::CountTokens() const
{
    size_t pos = 0;
    size_t count = 0;
    for ( ;; )
    {
        pos = m_string.find_first_of(m_delims, pos);
        if ( pos == wxString::npos )
            break;

        count++;    // one more token found

        pos++;      // skip delimiter
    }

    // normally, we didn't count the last token in the loop above - so add it
    // unless the string was empty from the very beginning, in which case it
    // still has 0 (and not 1) tokens
    if ( !m_string.empty() )
    {
        count++;
    }

    return count;
}

// ----------------------------------------------------------------------------
// token extraction
// ----------------------------------------------------------------------------

wxString wxStringTokenizer::GetNextToken()
{
    wxString token;
    if ( HasMoreTokens() )
    {
        size_t pos = m_string.find_first_of(m_delims); // end of token
        size_t pos2;                                   // start of the next one
        if ( pos != wxString::npos )
        {
            // return the delimiter too
            pos2 = pos + 1;
        }
        else
        {
            pos2 = m_string.length();

            // no more tokens in this string
            m_hasMore = FALSE;
        }

        token = wxString(m_string, m_retdelims ? pos2 : pos);

        // remove token with the following it delimiter from string
        m_string.erase(0, pos2);

        // keep track of the position in the original string too
        m_pos += pos2;
    }
    //else: no more tokens, return empty token

    return token;
}
