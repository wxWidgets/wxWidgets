/////////////////////////////////////////////////////////////////////////////
// Name:        wx/tokenzr.h
// Purpose:     String tokenizer - a C++ replacement for strtok(3)
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

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// default: delimiters are usual white space characters
#define wxDEFAULT_DELIMITERS (_T(" \t\r\n"))

// wxStringTokenizer mode flags which determine its behaviour
enum wxStringTokenizerMode
{
    wxTOKEN_INVALID = -1,   // set by def ctor until SetString() is called
    wxTOKEN_DEFAULT,        // strtok() for whitespace delims, RET_EMPTY else
    wxTOKEN_RET_EMPTY,      // return empty token in the middle of the string
    wxTOKEN_RET_EMPTY_ALL,  // return trailing empty tokens too
    wxTOKEN_RET_DELIMS,     // return the delim with token (implies RET_EMPTY)
    wxTOKEN_STRTOK          // behave exactly like strtok(3)
};

// ----------------------------------------------------------------------------
// wxStringTokenizer: replaces infamous strtok() and has some other features
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxStringTokenizer : public wxObject
{
public:
    // ctors and initializers
        // default ctor, call SetString() later
    wxStringTokenizer() { m_mode = wxTOKEN_INVALID; }
        // ctor which gives us the string
    wxStringTokenizer(const wxString& str,
                      const wxString& delims = wxDEFAULT_DELIMITERS,
                      wxStringTokenizerMode mode = wxTOKEN_DEFAULT);

        // args are same as for the non default ctor above
    void SetString(const wxString& str,
                   const wxString& delims = wxDEFAULT_DELIMITERS,
                   wxStringTokenizerMode mode = wxTOKEN_DEFAULT);

        // reinitialize the tokenizer with the same delimiters/mode
    void Reinit(const wxString& str);

    // tokens access
        // count them
    size_t CountTokens() const;
        // did we reach the end of the string?
    bool HasMoreTokens() const;
        // get the next token, will return empty string if !HasMoreTokens()
    wxString GetNextToken();

    // get current tokenizer state
        // returns the part of the string which remains to tokenize (*not* the
        // initial string)
    wxString GetString() const { return m_string; }

        // returns the current position (i.e. one index after the last
        // returned token or 0 if GetNextToken() has never been called) in the
        // original string
    size_t GetPosition() const { return m_pos; }

    // misc
        // get the current mode - can be different from the one passed to the
        // ctor if it was wxTOKEN_DEFAULT
    wxStringTokenizerMode GetMode() const { return m_mode; }

    // backwards compatibility section from now on
    // -------------------------------------------

    // for compatibility only, use GetNextToken() instead
    wxString NextToken() { return GetNextToken(); }

    // compatibility only, don't use
    void SetString(const wxString& to_tokenize,
                   const wxString& delims,
                   bool ret_delim)
    {
        SetString(to_tokenize, delims, wxTOKEN_RET_DELIMS);
    }

    wxStringTokenizer(const wxString& to_tokenize,
                      const wxString& delims,
                      bool ret_delim)
    {
        SetString(to_tokenize, delims, ret_delim);
    }

protected:
    bool IsOk() const { return m_mode != wxTOKEN_INVALID; }

    wxString m_string,              // the (rest of) string to tokenize
             m_delims;              // all delimiters

    size_t   m_pos;                 // the position in the original string

    wxStringTokenizerMode m_mode;   // see wxTOKEN_XXX values

    bool     m_hasMore;             // do we have more (possible empty) tokens?
};

#endif // _WX_TOKENZRH
