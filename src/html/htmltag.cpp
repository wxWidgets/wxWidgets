/////////////////////////////////////////////////////////////////////////////
// Name:        htmltag.cpp
// Purpose:     wxHtmlTag class (represents single tag)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include <wx/wxprec.h>

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif

#include <wx/html/htmltag.h>
#include <stdarg.h>




//-----------------------------------------------------------------------------
// wxHtmlTagsCache
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxHtmlTagsCache,wxObject)

#define CACHE_INCREMENT  64

wxHtmlTagsCache::wxHtmlTagsCache(const wxString& source)
{
    const char *src = source.c_str();
    int i, tg, pos, stpos;
    int lng = source.Length();
    char dummy[256];

    m_Cache = NULL;
    m_CacheSize = 0;
    m_CachePos = 0;

    pos = 0;
    while (pos < lng) {
        if (src[pos] == '<') {  // tag found:
            if (m_CacheSize % CACHE_INCREMENT == 0)
                m_Cache = (sCacheItem*) realloc(m_Cache, (m_CacheSize + CACHE_INCREMENT) * sizeof(sCacheItem));
            tg = m_CacheSize++;
            m_Cache[tg].Key = stpos = pos++;
            dummy[0] = 0; i = 0;
            while ((src[pos] != '>') && (src[pos] != ' ')) {
                dummy[i] = src[pos++];
                if ((dummy[i] >= 'a') && (dummy[i] <= 'z')) dummy[i] -= ('a' - 'A');
                i++;
            }
            dummy[i] = 0;
            m_Cache[tg].Name = (char*) malloc(i+1);
            memcpy(m_Cache[tg].Name, dummy, i+1);

            while (src[pos] != '>') pos++;

            if (src[stpos+1] == '/') { // ending tag:
                m_Cache[tg].End1 = m_Cache[tg].End2 = -2;
                // find matching begin tag:
                for (i = tg; i >= 0; i--)
                    if ((m_Cache[i].End1 == -1) && (strcmp(m_Cache[i].Name, dummy+1) == 0)) {
                        m_Cache[i].End1 = stpos;
                        m_Cache[i].End2 = pos + 1;
                        break;
                    }
            }
            else {
                m_Cache[tg].End1 = m_Cache[tg].End2 = -1;
            }
        }

        pos++;
    }

    // ok, we're done, now we'll free .Name members of cache - we don't need it anymore:
    for (i = 0; i < m_CacheSize; i++) {
        free(m_Cache[i].Name);
        m_Cache[i].Name = NULL;
    }
}



void wxHtmlTagsCache::QueryTag(int at, int* end1, int* end2)
{
    if (m_Cache == NULL) return;
    if (m_Cache[m_CachePos].Key != at) {
        int delta = (at < m_Cache[m_CachePos].Key) ? -1 : 1;
        do {m_CachePos += delta;} while (m_Cache[m_CachePos].Key != at);
    }
    *end1 = m_Cache[m_CachePos].End1;
    *end2 = m_Cache[m_CachePos].End2;
}




//-----------------------------------------------------------------------------
// wxHtmlTag
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wxHtmlTag,wxObject)

wxHtmlTag::wxHtmlTag(const wxString& source, int pos, int end_pos, wxHtmlTagsCache* cache) : wxObject()
{
    int i;
    char c;

    // fill-in name, params and begin pos:
    m_Name = m_Params = wxEmptyString;
    i = pos+1;
    if (source[i] == '/') {m_Ending = TRUE; i++;}
    else m_Ending = FALSE;

    while ((i < end_pos) && ((c = source[i++]) != ' ') && (c != '>')) {
        if ((c >= 'a') && (c <= 'z')) c -= ('a' - 'A');
        m_Name += c;
    }

    if (source[i-1] != '>')
        while ((i < end_pos) && ((c = source[i++]) != '>')) {
            if ((c >= 'a') && (c <= 'z')) c -= ('a' - 'A');
            m_Params += c;
            if (c == '"') {
                while ((i < end_pos) && ((c = source[i++]) != '"')) m_Params += c;
                m_Params += c;
            }
        }
   m_Begin = i;

   cache -> QueryTag(pos, &m_End1, &m_End2);
   if (m_End1 > end_pos) m_End1 = end_pos;
   if (m_End2 > end_pos) m_End2 = end_pos;
}

    

bool wxHtmlTag::HasParam(const wxString& par) const
{
    const char *st = m_Params, *p = par;
    const char *st2, *p2;

    if (*st == 0) return FALSE;
    if (*p == 0) return FALSE;
    for (st2 = st, p2 = p; ; st2++) {
        if (*p2 == 0) return TRUE;
        if (*st2 == 0) return FALSE;
        if (*p2 != *st2) p2 = p;
        if (*p2 == *st2) p2++;
        if (*st2 == ' ') p2 = p;
        else if (*st2 == '=') {
            p2 = p;
            while (*st2 != ' ') {
                if (*st2 == '"') {
                    st2++;
                    while (*st2 != '"') st2++;
                }
                st2++;
                if (*st2 == 0) return FALSE;
            }
        }
    }
}



wxString wxHtmlTag::GetParam(const wxString& par, bool with_commas) const
{
    const char *st = m_Params, *p = par;
    const char *st2, *p2;
    bool comma;

    if (*st == 0) return "";
    if (*p == 0) return "";
    for (st2 = st, p2 = p; ; st2++) {
        if (*p2 == 0) { // found
            wxString fnd = "";
            st2++; // '=' character
            comma = FALSE;
            if (!with_commas && (*(st2) == '"')) {st2++; comma = TRUE;}
            while (*st2 != 0) {
                if (*st2 == '"') comma = !comma;
                else if ((*st2 == ' ') && (!comma)) break;
                fnd += (*(st2++));
            }
            if (!with_commas && (*(st2-1) == '"')) fnd.RemoveLast();
            return fnd;
        }
        if (*st2 == 0) return "";
        if (*p2 != *st2) p2 = p;
        if (*p2 == *st2) p2++;
        if (*st2 == ' ') p2 = p;
        else if (*st2 == '=') {
            p2 = p;
            while (*st2 != ' ') {
                if (*st2 == '"') {
                    st2++;
                    while (*st2 != '"') st2++;
                }
                st2++;
            }
        }
    }
}



void wxHtmlTag::ScanParam(const wxString& par, char *format, ...) const
{
    va_list argptr;
    wxString parval = GetParam(par);

    va_start(argptr, format);

#if defined(__MINGW32__) || defined(__CYGWIN__) || defined(__VISUALC__)
    sscanf((const char*)parval, format, va_arg(argptr, void *));
#else
    vsscanf((const char*)parval, format, argptr);
#endif

/*
        --- vsscanf is not defined under Cygwin or Mingw32 or M$ Visual C++ environment
            if this module doesn't compile with your compiler,
            modify the def statement and let me know. Thanks...
        
            So far wxHtml functions are scanning only _one_ value
            so I workarounded this by supposing that there is only
            one ...-parameter 
*/

    va_end(argptr);
}

#endif