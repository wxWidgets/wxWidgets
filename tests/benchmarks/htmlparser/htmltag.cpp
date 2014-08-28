/////////////////////////////////////////////////////////////////////////////
// Name:        src/html/htmltag.cpp
// Purpose:     wx28HtmlTag class (represents single tag)
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "htmltag.h"

#include "htmlpars.h"
#include <stdio.h> // for vsscanf
#include <stdarg.h>


//-----------------------------------------------------------------------------
// wx28HtmlTagsCache
//-----------------------------------------------------------------------------

struct wx28HtmlCacheItem
{
    // this is "pos" value passed to wx28HtmlTag's constructor.
    // it is position of '<' character of the tag
    int Key;

    // end positions for the tag:
    // end1 is '<' of ending tag,
    // end2 is '>' or both are
    // -1 if there is no ending tag for this one...
    // or -2 if this is ending tag  </...>
    int End1, End2;

    // name of this tag
    wxChar *Name;
};


IMPLEMENT_CLASS(wx28HtmlTagsCache,wxObject)

#define CACHE_INCREMENT  64

bool wxIsCDATAElement(const wxChar *tag)
{
    return (wxStrcmp(tag, wxT("SCRIPT")) == 0) ||
           (wxStrcmp(tag, wxT("STYLE")) == 0);
}

wx28HtmlTagsCache::wx28HtmlTagsCache(const wxString& source)
{
    const wxChar *src = source.c_str();
    int lng = source.length();
    wxChar tagBuffer[256];

    m_Cache = NULL;
    m_CacheSize = 0;
    m_CachePos = 0;

    int pos = 0;
    while (pos < lng)
    {
        if (src[pos] == wxT('<'))   // tag found:
        {
            if (m_CacheSize % CACHE_INCREMENT == 0)
                m_Cache = (wx28HtmlCacheItem*) realloc(m_Cache, (m_CacheSize + CACHE_INCREMENT) * sizeof(wx28HtmlCacheItem));
            int tg = m_CacheSize++;
            int stpos = pos++;
            m_Cache[tg].Key = stpos;

            int i;
            for ( i = 0;
                  pos < lng && i < (int)WXSIZEOF(tagBuffer) - 1 &&
                  src[pos] != wxT('>') && !wxIsspace(src[pos]);
                  i++, pos++ )
            {
                tagBuffer[i] = (wxChar)wxToupper(src[pos]);
            }
            tagBuffer[i] = wxT('\0');

            m_Cache[tg].Name = new wxChar[i+1];
            memcpy(m_Cache[tg].Name, tagBuffer, (i+1)*sizeof(wxChar));

            while (pos < lng && src[pos] != wxT('>')) pos++;

            if (src[stpos+1] == wxT('/')) // ending tag:
            {
                m_Cache[tg].End1 = m_Cache[tg].End2 = -2;
                // find matching begin tag:
                for (i = tg; i >= 0; i--)
                    if ((m_Cache[i].End1 == -1) && (wxStrcmp(m_Cache[i].Name, tagBuffer+1) == 0))
                    {
                        m_Cache[i].End1 = stpos;
                        m_Cache[i].End2 = pos + 1;
                        break;
                    }
            }
            else
            {
                m_Cache[tg].End1 = m_Cache[tg].End2 = -1;

                if (wxIsCDATAElement(tagBuffer))
                {
                    // store the orig pos in case we are missing the closing
                    // tag (see below)
                    wxInt32 old_pos = pos;
                    bool foundCloseTag = false;

                    // find next matching tag
                    int tag_len = wxStrlen(tagBuffer);
                    while (pos < lng)
                    {
                        // find the ending tag
                        while (pos + 1 < lng &&
                               (src[pos] != '<' || src[pos+1] != '/'))
                            ++pos;
                        if (src[pos] == '<')
                            ++pos;

                        // see if it matches
                        int match_pos = 0;
                        while (pos < lng && match_pos < tag_len && src[pos] != '>' && src[pos] != '<') {
                            // cast to wxChar needed to suppress warning in
                            // Unicode build
                            if ((wxChar)wxToupper(src[pos]) == tagBuffer[match_pos]) {
                                ++match_pos;
                            }
                            else if (src[pos] == wxT(' ') || src[pos] == wxT('\n') ||
                                src[pos] == wxT('\r') || src[pos] == wxT('\t')) {
                                // need to skip over these
                            }
                            else {
                                match_pos = 0;
                            }
                            ++pos;
                        }

                        // found a match
                        if (match_pos == tag_len)
                        {
                            pos = pos - tag_len - 3;
                            foundCloseTag = true;
                            break;
                        }
                        else // keep looking for the closing tag
                        {
                            ++pos;
                        }
                    }
                    if (!foundCloseTag)
                    {
                        // we didn't find closing tag; this means the markup
                        // is incorrect and the best thing we can do is to
                        // ignore the unclosed tag and continue parsing as if
                        // it didn't exist:
                        pos = old_pos;
                    }
                }
            }
        }

        pos++;
    }

    // ok, we're done, now we'll free .Name members of cache - we don't need it anymore:
    for (int i = 0; i < m_CacheSize; i++)
    {
        delete[] m_Cache[i].Name;
        m_Cache[i].Name = NULL;
    }
}

void wx28HtmlTagsCache::QueryTag(int at, int* end1, int* end2)
{
    if (m_Cache == NULL) return;
    if (m_Cache[m_CachePos].Key != at)
    {
        int delta = (at < m_Cache[m_CachePos].Key) ? -1 : 1;
        do
        {
            if ( m_CachePos < 0 || m_CachePos == m_CacheSize )
            {
                // something is very wrong with HTML, give up by returning an
                // impossibly large value which is going to be ignored by the
                // caller
                *end1 =
                *end2 = INT_MAX;
                return;
            }

            m_CachePos += delta;
        }
        while (m_Cache[m_CachePos].Key != at);
    }
    *end1 = m_Cache[m_CachePos].End1;
    *end2 = m_Cache[m_CachePos].End2;
}




//-----------------------------------------------------------------------------
// wx28HtmlTag
//-----------------------------------------------------------------------------

IMPLEMENT_CLASS(wx28HtmlTag,wxObject)

wx28HtmlTag::wx28HtmlTag(wx28HtmlTag *parent,
                     const wxString& source, int pos, int end_pos,
                     wx28HtmlTagsCache *cache,
                     wx28HtmlEntitiesParser *entParser) : wxObject()
{
    /* Setup DOM relations */

    m_Next = NULL;
    m_FirstChild = m_LastChild = NULL;
    m_Parent = parent;
    if (parent)
    {
        m_Prev = m_Parent->m_LastChild;
        if (m_Prev == NULL)
            m_Parent->m_FirstChild = this;
        else
            m_Prev->m_Next = this;
        m_Parent->m_LastChild = this;
    }
    else
        m_Prev = NULL;

    /* Find parameters and their values: */

    int i;
    wxChar c;

    // fill-in name, params and begin pos:
    i = pos+1;

    // find tag's name and convert it to uppercase:
    while ((i < end_pos) &&
           ((c = source[i++]) != wxT(' ') && c != wxT('\r') &&
             c != wxT('\n') && c != wxT('\t') &&
             c != wxT('>')))
    {
        if ((c >= wxT('a')) && (c <= wxT('z')))
            c -= (wxT('a') - wxT('A'));
        m_Name << c;
    }

    // if the tag has parameters, read them and "normalize" them,
    // i.e. convert to uppercase, replace whitespaces by spaces and
    // remove whitespaces around '=':
    if (source[i-1] != wxT('>'))
    {
        #define IS_WHITE(c) (c == wxT(' ') || c == wxT('\r') || \
                             c == wxT('\n') || c == wxT('\t'))
        wxString pname, pvalue;
        wxChar quote;
        enum
        {
            ST_BEFORE_NAME = 1,
            ST_NAME,
            ST_BEFORE_EQ,
            ST_BEFORE_VALUE,
            ST_VALUE
        } state;

        quote = 0;
        state = ST_BEFORE_NAME;
        while (i < end_pos)
        {
            c = source[i++];

            if (c == wxT('>') && !(state == ST_VALUE && quote != 0))
            {
                if (state == ST_BEFORE_EQ || state == ST_NAME)
                {
                    m_ParamNames.Add(pname);
                    m_ParamValues.Add(wxEmptyString);
                }
                else if (state == ST_VALUE && quote == 0)
                {
                    m_ParamNames.Add(pname);
                    if (entParser)
                        m_ParamValues.Add(entParser->Parse(pvalue));
                    else
                        m_ParamValues.Add(pvalue);
                }
                break;
            }
            switch (state)
            {
                case ST_BEFORE_NAME:
                    if (!IS_WHITE(c))
                    {
                        pname = c;
                        state = ST_NAME;
                    }
                    break;
                case ST_NAME:
                    if (IS_WHITE(c))
                        state = ST_BEFORE_EQ;
                    else if (c == wxT('='))
                        state = ST_BEFORE_VALUE;
                    else
                        pname << c;
                    break;
                case ST_BEFORE_EQ:
                    if (c == wxT('='))
                        state = ST_BEFORE_VALUE;
                    else if (!IS_WHITE(c))
                    {
                        m_ParamNames.Add(pname);
                        m_ParamValues.Add(wxEmptyString);
                        pname = c;
                        state = ST_NAME;
                    }
                    break;
                case ST_BEFORE_VALUE:
                    if (!IS_WHITE(c))
                    {
                        if (c == wxT('"') || c == wxT('\''))
                            quote = c, pvalue = wxEmptyString;
                        else
                            quote = 0, pvalue = c;
                        state = ST_VALUE;
                    }
                    break;
                case ST_VALUE:
                    if ((quote != 0 && c == quote) ||
                        (quote == 0 && IS_WHITE(c)))
                    {
                        m_ParamNames.Add(pname);
                        if (quote == 0)
                        {
                            // VS: backward compatibility, no real reason,
                            //     but wxHTML code relies on this... :(
                            pvalue.MakeUpper();
                        }
                        if (entParser)
                            m_ParamValues.Add(entParser->Parse(pvalue));
                        else
                            m_ParamValues.Add(pvalue);
                        state = ST_BEFORE_NAME;
                    }
                    else
                        pvalue << c;
                    break;
            }
        }

        #undef IS_WHITE
    }
    m_Begin = i;

    cache->QueryTag(pos, &m_End1, &m_End2);
    if (m_End1 > end_pos) m_End1 = end_pos;
    if (m_End2 > end_pos) m_End2 = end_pos;
}

wx28HtmlTag::~wx28HtmlTag()
{
    wx28HtmlTag *t1, *t2;
    t1 = m_FirstChild;
    while (t1)
    {
        t2 = t1->GetNextSibling();
        delete t1;
        t1 = t2;
    }
}

bool wx28HtmlTag::HasParam(const wxString& par) const
{
    return (m_ParamNames.Index(par, false) != wxNOT_FOUND);
}

wxString wx28HtmlTag::GetParam(const wxString& par, bool with_commas) const
{
    int index = m_ParamNames.Index(par, false);
    if (index == wxNOT_FOUND)
        return wxEmptyString;
    if (with_commas)
    {
        // VS: backward compatibility, seems to be never used by wxHTML...
        wxString s;
        s << wxT('"') << m_ParamValues[index] << wxT('"');
        return s;
    }
    else
        return m_ParamValues[index];
}

int wx28HtmlTag::ScanParam(const wxString& par,
                         const wxChar *format,
                         void *param) const
{
    wxString parval = GetParam(par);
    return wxSscanf(parval, format, param);
}

bool wx28HtmlTag::GetParamAsInt(const wxString& par, int *clr) const
{
    if ( !HasParam(par) )
        return false;

    long i;
    if ( !GetParam(par).ToLong(&i) )
        return false;

    *clr = (int)i;
    return true;
}

wxString wx28HtmlTag::GetAllParams() const
{
    // VS: this function is for backward compatibility only,
    //     never used by wxHTML
    wxString s;
    size_t cnt = m_ParamNames.GetCount();
    for (size_t i = 0; i < cnt; i++)
    {
        s << m_ParamNames[i];
        s << wxT('=');
        if (m_ParamValues[i].Find(wxT('"')) != wxNOT_FOUND)
            s << wxT('\'') << m_ParamValues[i] << wxT('\'');
        else
            s << wxT('"') << m_ParamValues[i] << wxT('"');
    }
    return s;
}

wx28HtmlTag *wx28HtmlTag::GetFirstSibling() const
{
    if (m_Parent)
        return m_Parent->m_FirstChild;
    else
    {
        wx28HtmlTag *cur = (wx28HtmlTag*)this;
        while (cur->m_Prev)
            cur = cur->m_Prev;
        return cur;
    }
}

wx28HtmlTag *wx28HtmlTag::GetLastSibling() const
{
    if (m_Parent)
        return m_Parent->m_LastChild;
    else
    {
        wx28HtmlTag *cur = (wx28HtmlTag*)this;
        while (cur->m_Next)
            cur = cur->m_Next;
        return cur;
    }
}

wx28HtmlTag *wx28HtmlTag::GetNextTag() const
{
    if (m_FirstChild) return m_FirstChild;
    if (m_Next) return m_Next;
    wx28HtmlTag *cur = m_Parent;
    if (!cur) return NULL;
    while (cur->m_Parent && !cur->m_Next)
        cur = cur->m_Parent;
    return cur->m_Next;
}
