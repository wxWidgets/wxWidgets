/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.cpp
// Purpose:     String tokenizer
// Author:      Guilhem Lavaux
// Modified by: Gregory Pietsch
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
    #pragma implementation "tokenzr.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/tokenzr.h"

wxStringTokenizer::wxStringTokenizer(const wxString& to_tokenize,
                                     const wxString& delims,
                                     bool ret_delims)
{
    m_string = to_tokenize;
    m_delims = delims;
    m_retdelims = ret_delims;
    m_pos = 0;
}

wxStringTokenizer::~wxStringTokenizer()
{
}

int wxStringTokenizer::CountTokens() const
{
    size_t pos = 0;
    int count = 0;
    bool at_delim;

    while (pos < m_string.length()) {
        // while we're still counting ...
        at_delim = (m_delims.find(m_string.at(pos)) < m_delims.length());
        // are we at a delimiter? if so, move to the next nondelimiter;
        // if not, move to the next delimiter.  If the find_first_of
        // and find_first_not_of methods fail, pos will be assigned
        // npos (0xFFFFFFFF) which will terminate the loop on the next
        // go-round unless we have a really long string, which is unlikely
        pos = at_delim ? m_string.find_first_not_of(m_delims, pos)
                       : m_string.find_first_of(m_delims, pos);
        if (m_retdelims)
        {
            // if we're retaining delimiters, increment count
            count++;
        }
        else
        {
            // if we're not retaining delimiters and at a token, inc count
            count += (!at_delim);
        }
    }
    return count;
}

bool wxStringTokenizer::HasMoreTokens()
{
    return (m_retdelims
            ? !m_string.IsEmpty()
            : m_string.find_first_not_of(m_delims) < m_string.length());
}

wxString wxStringTokenizer::NextToken()
{
    size_t pos;
    wxString r_string;

    if ( m_string.IsEmpty() )
        return m_string;
        pos = m_string.find_first_not_of(m_delims);
    if ( m_retdelims ) {
        // we're retaining delimiters (unusual behavior, IMHO)
        if (pos == 0)
            // first char is a non-delimiter
            pos = m_string.find_first_of(m_delims);
    } else {
        // we're not retaining delimiters
        m_string.erase(0, pos);
        m_pos += pos;
        if (m_string.IsEmpty())
            return m_string;
        pos = m_string.find_first_of(m_delims);
    }
    if (pos <= m_string.length()) {
        r_string = m_string.substr(0, pos);
        m_string.erase(0, pos);
        m_pos += pos;
    } else {
        r_string = m_string;
        m_pos += m_string.length();
        m_string.Empty();
    }
    return r_string;
}
