/////////////////////////////////////////////////////////////////////////////
// Name:        htmlhelp.cpp
// Purpose:     Help controller
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////


//#ifdef __GNUG__
//#pragma implementation "htmlhelp.h"
//#endif
//  --- already in htmlhelp.cpp

#include <wx/wxprec.h>

#include "wx/defs.h"
#if wxUSE_HTML

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#ifndef WXPRECOMP
#include <wx/wx.h>
#endif


#include <wx/wxhtml.h>
#include <wx/busyinfo.h>




class HP_Parser : public wxHtmlParser
{
    public:
        void AddText(const char* text) {}
        wxObject* GetProduct() {return NULL;}
};



class HP_TagHandler : public wxHtmlTagHandler
{
    private:
        wxString m_Name, m_Page;
        int m_Level;
	int m_ID;
        int m_Index;
        HtmlContentsItem *m_Items;
        int m_ItemsCnt;
        HtmlBookRecord *m_Book;

    public:
        HP_TagHandler(HtmlBookRecord *b) : wxHtmlTagHandler() {m_Book = b; m_Items = NULL; m_ItemsCnt = 0; m_Name = m_Page = wxEmptyString; m_Level = 0;}
        wxString GetSupportedTags() {return "UL,OBJECT,PARAM";}
        bool HandleTag(const wxHtmlTag& tag);
        void WriteOut(HtmlContentsItem*& array, int& size);
        void ReadIn(HtmlContentsItem* array, int size);
};


bool HP_TagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == "UL") {
        m_Level++;
        ParseInner(tag);
        m_Level--;
        return TRUE;
    }

    else if (tag.GetName() == "OBJECT") {
        m_Name = m_Page = wxEmptyString;
        ParseInner(tag);
        if (m_Page != wxEmptyString) {
            if (m_ItemsCnt % HTML_REALLOC_STEP == 0)
                m_Items = (HtmlContentsItem*) realloc(m_Items, (m_ItemsCnt + HTML_REALLOC_STEP) * sizeof(HtmlContentsItem));
            m_Items[m_ItemsCnt].m_Level = m_Level;
            m_Items[m_ItemsCnt].m_ID = m_ID;
            m_Items[m_ItemsCnt].m_Page = (char*) malloc(m_Page.Length() + 1);
            strcpy(m_Items[m_ItemsCnt].m_Page, m_Page.c_str());
            m_Items[m_ItemsCnt].m_Name = (char*) malloc(m_Name.Length() + 1);
            strcpy(m_Items[m_ItemsCnt].m_Name, m_Name.c_str());
            m_Items[m_ItemsCnt].m_Book = m_Book;
            m_ItemsCnt++;
        }
        return TRUE;
    }

    else { // "PARAM"
        if (m_Name == wxEmptyString && tag.GetParam("NAME") == "Name") m_Name = tag.GetParam("VALUE");
        if (tag.GetParam("NAME") == "Local") m_Page = tag.GetParam("VALUE");
        if (tag.GetParam("NAME") == "ID") tag.ScanParam("VALUE", "%i", &m_ID); 
        return FALSE;
    }
}



void HP_TagHandler::WriteOut(HtmlContentsItem*& array, int& size)
{
    array = m_Items;
    size = m_ItemsCnt;
    m_Items = NULL;
    m_ItemsCnt = 0;
}

void HP_TagHandler::ReadIn(HtmlContentsItem* array, int size)
{
    m_Items = array;
    m_ItemsCnt = size;
}




void wxHtmlHelpController::LoadMSProject(HtmlBookRecord *book, wxFileSystem& fsys, const wxString& indexfile, const wxString& contentsfile, bool show_wait_msg)
{
    wxFSFile *f;
    char *buf;
    int sz;
    wxString string;
    wxBusyInfo *busyinfo = (show_wait_msg) ?  new wxBusyInfo(_("Importing help file : \n") + book -> m_Title) : NULL;
    
    HP_Parser parser;
    HP_TagHandler *handler = new HP_TagHandler(book);
    parser.AddTagHandler(handler);

    f = fsys.OpenFile(contentsfile);
    if (f) {
        sz = f -> GetStream() -> StreamSize();
        buf = (char*) malloc(sz+1);
        buf[sz] = 0;
        f -> GetStream() -> Read(buf, sz);
        delete f;
        handler -> ReadIn(m_Contents, m_ContentsCnt);
        parser.Parse(buf);
        handler -> WriteOut(m_Contents, m_ContentsCnt);
        free(buf);
    }

    f = fsys.OpenFile(indexfile);
    if (f) {
        sz = f -> GetStream() -> StreamSize();
        buf = (char*) malloc(sz+1);
        buf[sz] = 0;
        f -> GetStream() -> Read(buf, sz);
        delete f;
        handler -> ReadIn(m_Index, m_IndexCnt);
        parser.Parse(buf);
        handler -> WriteOut(m_Index, m_IndexCnt);
        free(buf);
    }
    if (show_wait_msg) delete busyinfo;
}






void wxHtmlHelpController::LoadCachedBook(HtmlBookRecord *book, wxInputStream *f)
{
    int i, st;
    int x;

    /* load contents : */

    f -> Read(&x, sizeof(x));
    st = m_ContentsCnt;
    m_ContentsCnt += x;
    m_Contents = (HtmlContentsItem*) realloc(m_Contents, (m_ContentsCnt / HTML_REALLOC_STEP + 1) * HTML_REALLOC_STEP * sizeof(HtmlContentsItem));
    for (i = st; i < m_ContentsCnt; i++) {
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_Level = x;
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_ID = x;
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_Name = (char*) malloc(x);
        f -> Read(m_Contents[i].m_Name, x);
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_Page = (char*) malloc(x);
        f -> Read(m_Contents[i].m_Page, x);
        m_Contents[i].m_Book = book;
    }

    /* load index : */

    f -> Read(&x, sizeof(x));
    st = m_IndexCnt;
    m_IndexCnt += x;
    m_Index = (HtmlContentsItem*) realloc(m_Index, (m_IndexCnt / HTML_REALLOC_STEP + 1) * HTML_REALLOC_STEP * sizeof(HtmlContentsItem));
    for (i = st; i < m_IndexCnt; i++) {
        f -> Read(&x, sizeof(x));
        m_Index[i].m_Name = (char*) malloc(x);
        f -> Read(m_Index[i].m_Name, x);
        f -> Read(&x, sizeof(x));
        m_Index[i].m_Page = (char*) malloc(x);
        f -> Read(m_Index[i].m_Page, x);
        m_Index[i].m_Book = book;
    }
}






void wxHtmlHelpController::SaveCachedBook(HtmlBookRecord *book, wxOutputStream *f)
{
    int i;
    int x;

    /* save contents : */

    x = 0;
    for (i = 0; i < m_ContentsCnt; i++) if (m_Contents[i].m_Book == book && m_Contents[i].m_Level > 0) x++;
    f -> Write(&x, sizeof(x));
    for (i = 0; i < m_ContentsCnt; i++) {
        if (m_Contents[i].m_Book != book || m_Contents[i].m_Level == 0) continue;
        x = m_Contents[i].m_Level;
        f -> Write(&x, sizeof(x));
        x = m_Contents[i].m_ID;
        f -> Write(&x, sizeof(x));
        x = strlen(m_Contents[i].m_Name) + 1;
        f -> Write(&x, sizeof(x));
        f -> Write(m_Contents[i].m_Name, x);
        x = strlen(m_Contents[i].m_Page) + 1;
        f -> Write(&x, sizeof(x));
        f -> Write(m_Contents[i].m_Page, x);
    }

    /* save index : */

    x = 0;
    for (i = 0; i < m_IndexCnt; i++) if (m_Index[i].m_Book == book && m_Index[i].m_Level > 0) x++;
    f -> Write(&x, sizeof(x));
    for (i = 0; i < m_IndexCnt; i++) {
        if (m_Index[i].m_Book != book || m_Index[i].m_Level == 0) continue;
        x = strlen(m_Index[i].m_Name) + 1;
        f -> Write(&x, sizeof(x));
        f -> Write(m_Index[i].m_Name, x);
        x = strlen(m_Index[i].m_Page) + 1;
        f -> Write(&x, sizeof(x));
        f -> Write(m_Index[i].m_Page, x);
    }
}

#endif