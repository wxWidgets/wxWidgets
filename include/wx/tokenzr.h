/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.h
// Purpose:     String tokenizer
// Author:      Guilhem Lavaux
// Modified by: Vadim Zeitlin
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOKENZRH
#define _WX_TOKENZRH

#ifdef __GNUG__
    #pragma interface "tokenzr.h"
#endif

#include "wx/object.h"
#include "wx/string.h"

// default: delimiters are usual white space characters
#define wxDEFAULT_DELIMITERS (_T(" \t\r\n"))

class WXDLLEXPORT wxStringTokenizer : public wxObject
{
public:
    // ctors and such
    wxStringTokenizer() { m_retdelims = FALSE; m_pos = 0; }
    wxStringTokenizer(const wxString& to_tokenize,
                      const wxString& delims = wxDEFAULT_DELIMITERS,
                      bool ret_delim = FALSE);
    void SetString(const wxString& to_tokenize,
                   const wxString& delims = wxDEFAULT_DELIMITERS,
                   bool ret_delim = FALSE);
    virtual ~wxStringTokenizer();

    // count tokens/get next token
    size_t CountTokens() const;
    bool HasMoreTokens() { return m_hasMore; }
    wxString GetNextToken();

    // One note about GetString -- it returns the string
    // remaining after the previous tokens have been removed,
    // not the original string
    wxString GetString() const { return m_string; }

    // returns the current position (i.e. one index after the last returned
    // token or 0 if GetNextToken() has never been called) in the original
    // string
    size_t GetPosition() const { return m_pos; }

    // for compatibility only, use GetNextToken() instead
    wxString NextToken() { return GetNextToken(); }

protected:
    wxString m_string,              // the (rest of) string to tokenize
             m_delims;              // all delimiters

    size_t   m_pos;                 // the position in the original string

    bool     m_retdelims;           // if TRUE, return delims with tokens
    bool     m_hasMore;             // do we have more tokens?
};

#endif // _WX_TOKENZRH
