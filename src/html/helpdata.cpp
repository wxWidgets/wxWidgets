/////////////////////////////////////////////////////////////////////////////
// Name:        helpdata.cpp
// Purpose:     wxHtmlHelpData
// Notes:       Based on htmlhelp.cpp, implementing a monolithic
//              HTML Help controller class,  by Vaclav Slavik
// Author:      Harm van der Heijden and Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Harm van der Heijden and Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_HTML

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/helpdata.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/busyinfo.h"
#include "wx/html/htmlpars.h"
#include "wx/html/htmldefs.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxHtmlBookRecArray)

//-----------------------------------------------------------------------------
// static helper functions
//-----------------------------------------------------------------------------

// Reads one line, stores it into buf and returns pointer to new line or NULL.
static char* ReadLine(char *line, char *buf)
{
    char *writeptr = buf, *readptr = line;

    while (*readptr != 0 && *readptr != '\r' && *readptr != '\n') *(writeptr++) = *(readptr++);
    *writeptr = 0;
    while (*readptr == '\r' || *readptr == '\n') readptr++;
    if (*readptr == 0) return NULL;
    else return readptr;
}


static wxString SafeFileName(const wxString& s)
{
    wxString res = s;
    res.Replace(":", "_", TRUE);
    res.Replace(" ", "_", TRUE);
    res.Replace("/", "_", TRUE);
    res.Replace("\\", "_", TRUE);
    res.Replace("#", "_", TRUE);
    res.Replace(".", "_", TRUE);
    return res;
}


static int LINKAGEMODE IndexCompareFunc(const void *a, const void *b)
{
    return strcmp(((wxHtmlContentsItem*)a) -> m_Name, ((wxHtmlContentsItem*)b) -> m_Name);
}


//-----------------------------------------------------------------------------
// HP_Parser
//-----------------------------------------------------------------------------

class HP_Parser : public wxHtmlParser
{
    public:
        void AddText(const char* WXUNUSED(text)) { }
        wxObject* GetProduct() { return NULL; }
};


//-----------------------------------------------------------------------------
// HP_TagHandler
//-----------------------------------------------------------------------------

class HP_TagHandler : public wxHtmlTagHandler
{
    private:
        wxString m_Name, m_Page;
        int m_Level;
        int m_ID;
        int m_Index;
        wxHtmlContentsItem *m_Items;
        int m_ItemsCnt;
        wxHtmlBookRecord *m_Book;

    public:
        HP_TagHandler(wxHtmlBookRecord *b) : wxHtmlTagHandler() {m_Book = b; m_Items = NULL; m_ItemsCnt = 0; m_Name = m_Page = wxEmptyString; m_Level = 0; }
        wxString GetSupportedTags() { return "UL,OBJECT,PARAM"; }
        bool HandleTag(const wxHtmlTag& tag);
        void WriteOut(wxHtmlContentsItem*& array, int& size);
        void ReadIn(wxHtmlContentsItem* array, int size);
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
            if (m_ItemsCnt % wxHTML_REALLOC_STEP == 0)
                m_Items = (wxHtmlContentsItem*) realloc(m_Items, (m_ItemsCnt + wxHTML_REALLOC_STEP) * sizeof(wxHtmlContentsItem));
            m_Items[m_ItemsCnt].m_Level = m_Level;
            m_Items[m_ItemsCnt].m_ID = m_ID;
            m_Items[m_ItemsCnt].m_Page = new char[m_Page.Length() + 1];
            strcpy(m_Items[m_ItemsCnt].m_Page, m_Page.c_str());
            m_Items[m_ItemsCnt].m_Name = new char [m_Name.Length() + 1];
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



void HP_TagHandler::WriteOut(wxHtmlContentsItem*& array, int& size)
{
    array = m_Items;
    size = m_ItemsCnt;
    m_Items = NULL;
    m_ItemsCnt = 0;
}

void HP_TagHandler::ReadIn(wxHtmlContentsItem* array, int size)
{
    m_Items = array;
    m_ItemsCnt = size;
}




//-----------------------------------------------------------------------------
// wxHtmlHelpData
//-----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxHtmlHelpData, wxObject)

wxHtmlHelpData::wxHtmlHelpData()
{
    m_TempPath = wxEmptyString;

    m_Contents = NULL;
    m_ContentsCnt = 0;
    m_Index = NULL;
    m_IndexCnt = 0;
}

wxHtmlHelpData::~wxHtmlHelpData()
{
    int i;

    m_BookRecords.Empty();
    if (m_Contents) {
        for (i = 0; i < m_ContentsCnt; i++) {
            delete[] m_Contents[i].m_Page;
            delete[] m_Contents[i].m_Name;
        }
        free(m_Contents);
    }
    if (m_Index) {
        for (i = 0; i < m_IndexCnt; i++) {
            delete[] m_Index[i].m_Page;
            delete[] m_Index[i].m_Name;
        }
        free(m_Index);
    }
}

bool wxHtmlHelpData::LoadMSProject(wxHtmlBookRecord *book, wxFileSystem& fsys, const wxString& indexfile, const wxString& contentsfile)
{
    wxFSFile *f;
    char *buf;
    int sz;
    wxString string;

    HP_Parser parser;
    HP_TagHandler *handler = new HP_TagHandler(book);
    parser.AddTagHandler(handler);

    f = ( contentsfile.IsEmpty() ? 0 : fsys.OpenFile(contentsfile) );
    if (f) {
        sz = f -> GetStream() -> GetSize();
        buf = new char[sz + 1];
        buf[sz] = 0;
        f -> GetStream() -> Read(buf, sz);
        delete f;
        handler -> ReadIn(m_Contents, m_ContentsCnt);
        parser.Parse(buf);
        handler -> WriteOut(m_Contents, m_ContentsCnt);
        delete[] buf;
    }

    f = ( indexfile.IsEmpty() ? 0 : fsys.OpenFile(indexfile) );
    if (f) {
        sz = f -> GetStream() -> GetSize();
        buf = new char[sz + 1];
        buf[sz] = 0;
        f -> GetStream() -> Read(buf, sz);
        delete f;
        handler -> ReadIn(m_Index, m_IndexCnt);
        parser.Parse(buf);
        handler -> WriteOut(m_Index, m_IndexCnt);
        delete[] buf;
    }
    return TRUE;
}


bool wxHtmlHelpData::LoadCachedBook(wxHtmlBookRecord *book, wxInputStream *f)
{
    int i, st;
    int x;

    /* load contents : */

    f -> Read(&x, sizeof(x));
    st = m_ContentsCnt;
    m_ContentsCnt += x;
    m_Contents = (wxHtmlContentsItem*) realloc(m_Contents, (m_ContentsCnt / wxHTML_REALLOC_STEP + 1) * wxHTML_REALLOC_STEP * sizeof(wxHtmlContentsItem));
    for (i = st; i < m_ContentsCnt; i++) {
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_Level = x;
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_ID = x;
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_Name = new char[x];
        f -> Read(m_Contents[i].m_Name, x);
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_Page = new char[x];
        f -> Read(m_Contents[i].m_Page, x);
        m_Contents[i].m_Book = book;
    }

    /* load index : */

    f -> Read(&x, sizeof(x));
    st = m_IndexCnt;
    m_IndexCnt += x;
    m_Index = (wxHtmlContentsItem*) realloc(m_Index, (m_IndexCnt / wxHTML_REALLOC_STEP + 1) * wxHTML_REALLOC_STEP * sizeof(wxHtmlContentsItem));
    for (i = st; i < m_IndexCnt; i++) {
        f -> Read(&x, sizeof(x));
        m_Index[i].m_Name = new char[x];
        f -> Read(m_Index[i].m_Name, x);
        f -> Read(&x, sizeof(x));
        m_Index[i].m_Page = new char[x];
        f -> Read(m_Index[i].m_Page, x);
        m_Index[i].m_Book = book;
    }
    return TRUE;
}


bool wxHtmlHelpData::SaveCachedBook(wxHtmlBookRecord *book, wxOutputStream *f)
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
    return TRUE;
}


void wxHtmlHelpData::SetTempDir(const wxString& path)
{
    if (path == wxEmptyString) m_TempPath = path;
    else {
        if (wxIsAbsolutePath(path)) m_TempPath = path;
        else m_TempPath = wxGetCwd() + "/" + path;

        if (m_TempPath[m_TempPath.Length() - 1] != '/')
            m_TempPath << "/";
    }
}


bool wxHtmlHelpData::AddBookParam(const wxString& title, const wxString& contfile,
                                  const wxString& indexfile, const wxString& deftopic,
                                  const wxString& path)
{
    wxFileSystem fsys;
    wxFSFile *fi;
    wxHtmlBookRecord *bookr;
    wxString safetitle;

    if (! path.IsEmpty())
        fsys.ChangePathTo(path, TRUE);

    bookr = new wxHtmlBookRecord(path + '/', title, deftopic);

    if (m_ContentsCnt % wxHTML_REALLOC_STEP == 0)
        m_Contents = (wxHtmlContentsItem*) realloc(m_Contents, (m_ContentsCnt + wxHTML_REALLOC_STEP) * sizeof(wxHtmlContentsItem));
    m_Contents[m_ContentsCnt].m_Level = 0;
    m_Contents[m_ContentsCnt].m_ID = 0;
    m_Contents[m_ContentsCnt].m_Page = new char[deftopic.Length() + 1];
    strcpy(m_Contents[m_ContentsCnt].m_Page, deftopic.c_str());
    m_Contents[m_ContentsCnt].m_Name = new char [title.Length() + 1];
    strcpy(m_Contents[m_ContentsCnt].m_Name, title.c_str());
    m_Contents[m_ContentsCnt].m_Book = bookr;

    // store the contents index for later
    int cont_start = m_ContentsCnt++;

    // Try to find cached binary versions:
    safetitle = SafeFileName(title);
    fi = fsys.OpenFile(safetitle + ".cached");
    if (fi == NULL) fi = fsys.OpenFile(m_TempPath + safetitle + ".cached");
    if ((fi == NULL) || (m_TempPath == wxEmptyString)) {
        LoadMSProject(bookr, fsys, indexfile, contfile);
        if (m_TempPath != wxEmptyString) {
            wxFileOutputStream *outs = new wxFileOutputStream(m_TempPath + safetitle + ".cached");
            SaveCachedBook(bookr, outs);
            delete outs;
        }
    } else {
        LoadCachedBook(bookr, fi -> GetStream());
        delete fi;
    }

    // Now store the contents range
    bookr->SetContentsRange(cont_start, m_ContentsCnt);

    m_BookRecords.Add(bookr);
    if (m_IndexCnt > 0)
        qsort(m_Index, m_IndexCnt, sizeof(wxHtmlContentsItem), IndexCompareFunc);

    return TRUE;
}


bool wxHtmlHelpData::AddBook(const wxString& book)
{
    wxFSFile *fi;
    wxFileSystem fsys;
    wxInputStream *s;
    wxString bookFull;

    int sz;
    char *buff, *lineptr;
    char linebuf[300];

    wxString title = _("noname"),
                     safetitle,
                     start = wxEmptyString,
                             contents = wxEmptyString, index = wxEmptyString;

    if (wxIsAbsolutePath(book)) bookFull = book;
    else bookFull = wxGetCwd() + "/" + book;

    fi = fsys.OpenFile(bookFull);
    if (fi == NULL) return FALSE;
    fsys.ChangePathTo(bookFull);
    s = fi -> GetStream();
    sz = s -> GetSize();
    buff = new char[sz + 1];
    buff[sz] = 0;
    s -> Read(buff, sz);
    lineptr = buff;
    delete fi;

    do {
        lineptr = ReadLine(lineptr, linebuf);

        if (strstr(linebuf, "Title=") == linebuf)
            title = linebuf + strlen("Title=");
        if (strstr(linebuf, "Default topic=") == linebuf)
            start = linebuf + strlen("Default topic=");
        if (strstr(linebuf, "Index file=") == linebuf)
            index = linebuf + strlen("Index file=");
        if (strstr(linebuf, "Contents file=") == linebuf)
            contents = linebuf + strlen("Contents file=");
    } while (lineptr != NULL);
    delete[] buff;

    return AddBookParam(title, contents, index, start, fsys.GetPath());
}

wxString wxHtmlHelpData::FindPageByName(const wxString& x)
{
    int cnt;
    int i;
    wxFileSystem fsys;
    wxFSFile *f;
    wxString url(wxEmptyString);

    /* 1. try to open given file: */

    cnt = m_BookRecords.GetCount();
    for (i = 0; i < cnt; i++) {
        f = fsys.OpenFile(m_BookRecords[i].GetBasePath() + x);
        if (f) {
            url = m_BookRecords[i].GetBasePath() + x;
            delete f;
            return url;
        }
    }


    /* 2. try to find a book: */

    for (i = 0; i < cnt; i++) {
        if (m_BookRecords[i].GetTitle() == x) {
            url = m_BookRecords[i].GetBasePath() + m_BookRecords[i].GetStart();
            return url;
        }
    }

    /* 3. try to find in contents: */

    cnt = m_ContentsCnt;
    for (i = 0; i < cnt; i++) {
        if (strcmp(m_Contents[i].m_Name, x) == 0) {
            url = m_Contents[i].m_Book -> GetBasePath() + m_Contents[i].m_Page;
            return url;
        }
    }


    /* 4. try to find in index: */

    cnt = m_IndexCnt;
    for (i = 0; i < cnt; i++) {
        if (strcmp(m_Index[i].m_Name, x) == 0) {
            url = m_Index[i].m_Book -> GetBasePath() + m_Index[i].m_Page;
            return url;
        }
    }

    return url;
}

wxString wxHtmlHelpData::FindPageById(int id)
{
    int i;
    wxString url(wxEmptyString);

    for (i = 0; i < m_ContentsCnt; i++) {
        if (m_Contents[i].m_ID == id) {
            url = m_Contents[i].m_Book -> GetBasePath() + m_Contents[i].m_Page;
            return url;
        }
    }

    return url;
}

//----------------------------------------------------------------------------------
// wxHtmlSearchStatus functions
//----------------------------------------------------------------------------------

wxHtmlSearchStatus::wxHtmlSearchStatus(wxHtmlHelpData* data, const wxString& keyword,
                                       const wxString& book)
{
    m_Data = data;
    m_Keyword = keyword;
    wxHtmlBookRecord* bookr = NULL;
    if (book != wxEmptyString) {
        // we have to search in a specific book. Find it first
        int i, cnt = data->m_BookRecords.GetCount();
        for (i = 0; i < cnt; i++)
            if (data->m_BookRecords[i].GetTitle() == book) {
                bookr = &(data->m_BookRecords[i]);
                m_CurIndex = bookr->GetContentsStart();
                m_MaxIndex = bookr->GetContentsEnd();
                break;
            }
        // check; we won't crash if the book doesn't exist, but it's Bad Anyway.
        wxASSERT(bookr);
    }
    if (! bookr) {
        // no book specified; search all books
        m_CurIndex = 0;
        m_MaxIndex = m_Data->m_ContentsCnt;
    }
    m_Engine.LookFor(keyword);
    m_Active = (m_CurIndex < m_MaxIndex);
    m_LastPage = wxEmptyString;
}

bool wxHtmlSearchStatus::Search()
{
    wxFileSystem fsys;
    wxFSFile *file;
    int i = m_CurIndex;  // shortcut
    bool found = FALSE;

    if (! m_Active) {
	// sanity check. Illegal use, but we'll try to prevent a crash anyway
#if !defined(__VISAGECPP__)
wxASSERT(0);
#else
wxASSERT(m_Active);
#endif
	return FALSE;
    }

    m_ContentsItem = NULL;
    m_Name = wxEmptyString;

    file = fsys.OpenFile(m_Data->m_Contents[i].m_Book -> GetBasePath() +
                         m_Data->m_Contents[i].m_Page);
    if (file) {
        if (m_LastPage != file->GetLocation()) {
            m_LastPage = file->GetLocation();
            if (m_Engine.Scan(file -> GetStream())) {
                m_Name = m_Data->m_Contents[i].m_Name;
                m_ContentsItem = m_Data->m_Contents + i;
                found = TRUE;
            }
        }
        delete file;
    }
    m_Active = (++m_CurIndex < m_MaxIndex);
    return found;
}








//--------------------------------------------------------------------------------
// wxSearchEngine
//--------------------------------------------------------------------------------

void wxSearchEngine::LookFor(const wxString& keyword)
{
    if (m_Keyword) delete[] m_Keyword;
    m_Keyword = new char[keyword.Length() + 1];
    strcpy(m_Keyword, keyword.c_str());
    for (int i = strlen(m_Keyword) - 1; i >= 0; i--)
        if ((m_Keyword[i] >= 'A') && (m_Keyword[i] <= 'Z'))
            m_Keyword[i] += 'a' - 'A';
}



bool wxSearchEngine::Scan(wxInputStream *stream)
{
    wxASSERT_MSG(m_Keyword != NULL, _("wxSearchEngine::LookFor must be called before scanning!"));

    int i, j;
    int lng = stream ->GetSize();
    int wrd = strlen(m_Keyword);
    bool found = FALSE;
    char *buf = new char[lng + 1];
    stream -> Read(buf, lng);
    buf[lng] = 0;

    for (i = 0; i < lng; i++)
        if ((buf[i] >= 'A') && (buf[i] <= 'Z')) buf[i] += 'a' - 'A';

    for (i = 0; i < lng - wrd; i++) {
        j = 0;
        while ((j < wrd) && (buf[i + j] == m_Keyword[j])) j++;
    if (j == wrd) {found = TRUE; break; }
    }

    delete[] buf;
    return found;
}




#endif
