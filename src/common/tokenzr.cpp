/////////////////////////////////////////////////////////////////////////////
// Name:        tokenzr.cpp
// Purpose:     String tokenizer
// Author:      Guilhem Lavaux
// Modified by:
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

#ifndef WX_PRECOMP
#endif

#include "wx/object.h"
#include "wx/string.h"
#include "wx/tokenzr.h"

wxStringTokenizer::wxStringTokenizer(const wxString& to_tokenize,
                                     const wxString& delims,
				     bool ret_delims)
  : wxObject()
{
  m_string = to_tokenize;
  m_delims = delims;
  m_retdelims = ret_delims;
}

wxStringTokenizer::~wxStringTokenizer()
{
}

off_t wxStringTokenizer::FindDelims(const wxString& str, const wxString& delims)
{
  int i, j;
  register int s_len = str.Length(),
               len = delims.Length();

  for (i=0;i<s_len;i++) {
    register char c = str[i];

    for (j=0;j<len;j++)
      if (delims[j] == c)
        return i;
  }
  return -1;
}

int wxStringTokenizer::CountTokens()
{
  wxString p_string = m_string;
  bool found = TRUE;
  int pos, count = 1; 

  if (p_string.Length() == 0)
    return 0;

  while (found) {
    pos = FindDelims(p_string, m_delims);
    if (pos != -1) {
      count++;
      p_string = p_string(0, pos);
    } else
      found = FALSE;
  }
  return count;
}

bool wxStringTokenizer::HasMoreToken()
{
  return (m_string.Length() != 0);
}

// AVS - added to fix leading whitespace / mult. delims bugs
void wxStringTokenizer::EatLeadingDelims() 
{
  int pos;

  while ((pos=FindDelims(m_string, m_delims))==0) { // while leading delims
     m_string = m_string.Mid((size_t)1);     // trim 'em from the left
  }
}

wxString wxStringTokenizer::NextToken()
{
  register off_t pos, pos2;
  wxString r_string;

  if (m_string.IsNull())
    return m_string;

  if (!m_retdelims)
    EatLeadingDelims(); // AVS - added to fix leading whitespace /
                        // mult. delims bugs

  pos = FindDelims(m_string, m_delims);
  if (pos == -1) {
    r_string = m_string;
    m_string = wxEmptyString;
    
    return r_string;
  }

  if (m_retdelims) {
    if (!pos) {
      pos++;
      pos2 = 1;
    } else
      pos2 = pos;
  } else
      pos2 = pos + 1;
  
  r_string = m_string.Left((size_t)pos);
  m_string = m_string.Mid((size_t)pos2);

  return r_string;
}
