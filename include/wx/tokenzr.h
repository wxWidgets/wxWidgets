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

class wxStringTokenizer : wxObject {
public:
  wxStringTokenizer(const wxString& to_tokenize,
                    const wxString& delims = " \t\r\n",
                    bool ret_delim = FALSE);
  ~wxStringTokenizer();

  int CountTokens();
  bool HasMoreToken();
  wxString NextToken();
  wxString GetString() { return m_string; }
protected:
  off_t FindDelims(const wxString& str, const wxString& delims);
protected:
  wxString m_string, m_delims;
  bool m_retdelims;
};

#endif
