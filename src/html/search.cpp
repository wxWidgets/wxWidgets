/////////////////////////////////////////////////////////////////////////////
// Name:        search.cpp
// Purpose:     search engine
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////



#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#include <wx/defs.h>
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include "search.h"



//--------------------------------------------------------------------------------
// wxSearchEngine
//--------------------------------------------------------------------------------

void wxSearchEngine::LookFor(const wxString& keyword)
{
    if (m_Keyword) free(m_Keyword);
    m_Keyword = (char*) malloc(keyword.Length() + 1);
    strcpy(m_Keyword, keyword.c_str());
    for (int i = strlen(m_Keyword) - 1; i >= 0; i--)
        if ((m_Keyword[i] >= 'A') && (m_Keyword[i] <= 'Z'))
            m_Keyword[i] += 'a' - 'A';
}



bool wxSearchEngine::Scan(wxInputStream *stream)
{
    wxASSERT_MSG(m_Keyword != NULL, _("wxSearchEngine::LookFor must be called before scanning!"));

    int i, j;
    int lng = stream -> StreamSize();
    int wrd = strlen(m_Keyword);
    bool found = FALSE;
    char *buf = (char*) malloc(lng + 1);
    stream -> Read(buf, lng);
    buf[lng] = 0;

    for (i = 0; i < lng; i++)
        if ((buf[i] >= 'A') && (buf[i] <= 'Z')) buf[i] += 'a' - 'A';

    for (i = 0; i < lng - wrd; i++) {
        j = 0;
        while ((j < wrd) && (buf[i + j] == m_Keyword[j])) j++;
        if (j == wrd) {found = TRUE; break;}
    }

    free(buf);
    return found;
}

#endif
