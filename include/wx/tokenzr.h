/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.h
// Purpose:     String tokenizer
// Author:      Guilhem Lavaux
// Modified by:
// Created:     04/22/98
// RCS-ID:      $Id$
// Copyright:   (c) Guilhem Lavaux
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOKENZRH
#define _WX_TOKENZRH

#ifdef __GNUG__
#pragma interface
#endif

#include "wx/object.h"
#include "wx/string.h"
#include "wx/filefn.h"

class WXDLLEXPORT wxStringTokenizer : public wxObject
{
public:
    wxStringTokenizer(const wxString& to_tokenize,
                      const wxString& delims = " \t\r\n",
                      bool ret_delim = FALSE);
    wxStringTokenizer() { m_retdelims = FALSE;}
    virtual ~wxStringTokenizer();

    int CountTokens() const;
    bool HasMoreTokens();

    wxString NextToken();
    wxString GetNextToken() { return NextToken(); };

    wxString GetString() const { return m_string; }

    void SetString(const wxString& to_tokenize,
                   const wxString& delims = " \t\r\n",
                   bool ret_delim = FALSE)
    {
        m_string = to_tokenize;
        m_delims = delims;
        m_retdelims = ret_delim;
    }

protected:
    off_t FindDelims(const wxString& str, const wxString& delims) const;
    void EatLeadingDelims();

    wxString m_string, m_delims;
    bool m_retdelims;
};

#endif // _WX_TOKENZRH
