/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.h
// Purpose:     String tokenizer
// Author:      Guilhem Lavaux
// Modified by: Gregory Pietsch
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
#include "wx/filefn.h"

class WXDLLEXPORT wxStringTokenizer : public wxObject
{
public:
    wxStringTokenizer(const wxString& to_tokenize,
                      const wxString& delims = " \t\r\n",
                      bool ret_delim = FALSE);
    wxStringTokenizer() { m_retdelims = FALSE; m_pos = 0; }
    virtual ~wxStringTokenizer();

    int CountTokens() const;
    bool HasMoreTokens();

    wxString NextToken();
    wxString GetNextToken() { return NextToken(); };

    wxString GetString() const { return m_string; }
    // One note about GetString -- it returns the string
    // remaining after the previous tokens have been removed,
    // not the original string

    void SetString(const wxString& to_tokenize,
                   const wxString& delims = " \t\r\n",
                   bool ret_delim = FALSE)
    {
        m_string = to_tokenize;
        m_delims = delims;
        m_retdelims = ret_delim;
        m_pos = 0;
    }

    // Here's the desired function.  It returns the position
    // of the next token in the original string by keeping track
    // of everything that's been deleted by GetNextToken.
    wxUint32 GetPosition() { return m_pos; }

protected:
    wxString m_string, m_delims;
    bool m_retdelims;
    wxUint32 m_pos;     // the position
};

#endif // _WX_TOKENZRH
