/////////////////////////////////////////////////////////////////////////////
// Name:        htmlpars.cpp
// Purpose:     wxHtmlParser class (generic parser)
// Author:      Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


#ifdef __GNUG__
#pragma implementation
#endif

#include "wx/wxprec.h"

#include "wx/defs.h"
#if wxUSE_HTML && wxUSE_STREAMS

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/url.h"
#include "wx/html/htmldefs.h"
#include "wx/html/htmlpars.h"



//-----------------------------------------------------------------------------
// wxHtmlParser
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlParser,wxObject)


wxObject* wxHtmlParser::Parse(const wxString& source)
{
    wxObject *result;

    InitParser(source);
    DoParsing();
    result = GetProduct();
    DoneParser();
    return result;
}

void wxHtmlParser::InitParser(const wxString& source)
{
    SetSource(source);
}

void wxHtmlParser::DoneParser()
{
    delete m_Cache;
    m_Cache = NULL;
}

void wxHtmlParser::SetSource(const wxString& src)
{
    m_Source = src;
    delete m_Cache;
    m_Cache = new wxHtmlTagsCache(m_Source);
}

void wxHtmlParser::DoParsing(int begin_pos, int end_pos)
{
    if (end_pos <= begin_pos) return;

    char c;
    char *temp = new char[end_pos - begin_pos + 1];
    int i;
    int templen;

    templen = 0;
    i = begin_pos;

    while (i < end_pos) 
    {
        c = m_Source[(unsigned int) i];

        // continue building word:
        if (c != '<') 
	    {
            temp[templen++] = c;
            i++;
        }

        else if (c == '<')  
	    {
            wxHtmlTag tag(m_Source, i, end_pos, m_Cache);

            if (templen) 
	        {
                temp[templen] = 0;
                AddText(temp);
                templen = 0;
            }
            AddTag(tag);
            if (tag.HasEnding()) i = tag.GetEndPos2();
            else i = tag.GetBeginPos();
        }
    }

    if (templen) 
    { // last word of block :-(
        temp[templen] = 0;
        AddText(temp);
    }
    delete[] temp;
}

void wxHtmlParser::AddTag(const wxHtmlTag& tag)
{
    wxHtmlTagHandler *h;
    bool inner = FALSE;

    h = (wxHtmlTagHandler*) m_HandlersHash.Get(tag.GetName());
    if (h)
        inner = h->HandleTag(tag);
    if (!inner) 
    {
        if (tag.HasEnding())
            DoParsing(tag.GetBeginPos(), tag.GetEndPos1());
    }
}

void wxHtmlParser::AddTagHandler(wxHtmlTagHandler *handler)
{
    wxString s(handler->GetSupportedTags());
    wxStringTokenizer tokenizer(s, ", ");

    while (tokenizer.HasMoreTokens())
        m_HandlersHash.Put(tokenizer.NextToken(), handler);

    if (m_HandlersList.IndexOf(handler) == wxNOT_FOUND)
        m_HandlersList.Append(handler);

    handler->SetParser(this);
}

void wxHtmlParser::PushTagHandler(wxHtmlTagHandler *handler, wxString tags)
{
    wxStringTokenizer tokenizer(tags, ", ");
    wxString key;

    if (m_HandlersStack == NULL) 
    {
        m_HandlersStack = new wxList;
        m_HandlersStack->DeleteContents(TRUE);
    }

    m_HandlersStack->Insert(new wxHashTable(m_HandlersHash));

    while (tokenizer.HasMoreTokens()) 
    {
        key = tokenizer.NextToken();
        m_HandlersHash.Delete(key);
        m_HandlersHash.Put(key, handler);
    }
}

void wxHtmlParser::PopTagHandler()
{
    wxNode *first;
    
    if (m_HandlersStack == NULL || 
        (first = m_HandlersStack->GetFirst()) == NULL) 
    {
        wxLogWarning(_("Warning: attempt to remove HTML tag handler from empty stack."));
        return;
    }
    m_HandlersHash = *((wxHashTable*) first->GetData());
    m_HandlersStack->DeleteNode(first);
}

wxHtmlParser::~wxHtmlParser()
{
    if (m_HandlersStack) delete m_HandlersStack;
    m_HandlersHash.Clear();
    m_HandlersList.DeleteContents(TRUE);
    m_HandlersList.Clear();
}

//-----------------------------------------------------------------------------
// wxHtmlTagHandler
//-----------------------------------------------------------------------------

IMPLEMENT_ABSTRACT_CLASS(wxHtmlTagHandler,wxObject)
#endif

