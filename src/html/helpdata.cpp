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
#pragma implementation "helpdata.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"

#if wxUSE_HTML && wxUSE_STREAMS

#ifndef WXPRECOMP
    #include "wx/intl.h"
    #include "wx/log.h"
#endif

#include <ctype.h>

#include "wx/html/helpdata.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/busyinfo.h"
#include "wx/encconv.h"
#include "wx/fontmap.h"
#include "wx/log.h"
#include "wx/html/htmlpars.h"
#include "wx/html/htmldefs.h"
#include "wx/html/htmlfilt.h"
#include "wx/filename.h"

#include "wx/arrimpl.cpp"
WX_DEFINE_OBJARRAY(wxHtmlBookRecArray)

//-----------------------------------------------------------------------------
// static helper functions
//-----------------------------------------------------------------------------

// Reads one line, stores it into buf and returns pointer to new line or NULL.
static const wxChar* ReadLine(const wxChar *line, wxChar *buf, size_t bufsize)
{
    wxChar *writeptr = buf;
    wxChar *endptr = buf + bufsize - 1;
    const wxChar *readptr = line;

    while (*readptr != 0 && *readptr != _T('\r') && *readptr != _T('\n') &&
           writeptr != endptr) 
        *(writeptr++) = *(readptr++);
    *writeptr = 0;
    while (*readptr == _T('\r') || *readptr == _T('\n'))
        readptr++;
    if (*readptr == 0)
        return NULL;
    else 
        return readptr;
}



extern "C" int LINKAGEMODE
wxHtmlHelpIndexCompareFunc(const void *a, const void *b)
{
    return wxStricmp(((wxHtmlContentsItem*)a)->m_Name, ((wxHtmlContentsItem*)b)->m_Name);
}


//-----------------------------------------------------------------------------
// HP_Parser
//-----------------------------------------------------------------------------

class HP_Parser : public wxHtmlParser
{
public:
    wxObject* GetProduct() { return NULL; }
protected:
    virtual void AddText(const wxChar* WXUNUSED(txt)) {}
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
        HP_TagHandler(wxHtmlBookRecord *b) : wxHtmlTagHandler()
            { m_Book = b; m_Items = NULL; m_ItemsCnt = 0; m_Name = m_Page = wxEmptyString;
              m_Level = 0; m_ID = -1; }
        wxString GetSupportedTags() { return wxT("UL,OBJECT,PARAM"); }
        bool HandleTag(const wxHtmlTag& tag);
        void WriteOut(wxHtmlContentsItem*& array, int& size);
        void ReadIn(wxHtmlContentsItem* array, int size);
};


bool HP_TagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == wxT("UL"))
    {
        m_Level++;
        ParseInner(tag);
        m_Level--;
        return TRUE;
    }
    else if (tag.GetName() == wxT("OBJECT"))
    {
        m_Name = m_Page = wxEmptyString;
        ParseInner(tag);

#if 0
         if (!m_Page.IsEmpty())
        /* Valid HHW's file may contain only two object tags:

           <OBJECT type="text/site properties">
               <param name="ImageType" value="Folder">
           </OBJECT>

           or

           <OBJECT type="text/sitemap">
               <param name="Name" value="main page">
               <param name="Local" value="another.htm">
           </OBJECT>

           We're interested in the latter. !m_Page.IsEmpty() is valid
           condition because text/site properties does not contain Local param
        */
#endif
        if (tag.GetParam(wxT("TYPE")) == wxT("text/sitemap"))
        {
            if (m_ItemsCnt % wxHTML_REALLOC_STEP == 0)
                m_Items = (wxHtmlContentsItem*) realloc(m_Items,
                                       (m_ItemsCnt + wxHTML_REALLOC_STEP) *
                                       sizeof(wxHtmlContentsItem));

            m_Items[m_ItemsCnt].m_Level = m_Level;
            m_Items[m_ItemsCnt].m_ID = m_ID;
            m_Items[m_ItemsCnt].m_Page = new wxChar[m_Page.Length() + 1];
            wxStrcpy(m_Items[m_ItemsCnt].m_Page, m_Page.c_str());
            m_Items[m_ItemsCnt].m_Name = new wxChar [m_Name.Length() + 1];
            wxStrcpy(m_Items[m_ItemsCnt].m_Name, m_Name.c_str());
            m_Items[m_ItemsCnt].m_Book = m_Book;
            m_ItemsCnt++;
        }

        return TRUE;
    }
    else
    { // "PARAM"
        if (m_Name == wxEmptyString && tag.GetParam(wxT("NAME")) == wxT("Name"))
            m_Name = tag.GetParam(wxT("VALUE"));
        if (tag.GetParam(wxT("NAME")) == wxT("Local"))
            m_Page = tag.GetParam(wxT("VALUE"));
        if (tag.GetParam(wxT("NAME")) == wxT("ID"))
            tag.GetParamAsInt(wxT("VALUE"), &m_ID);
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

wxString wxHtmlBookRecord::GetFullPath(const wxString &page) const
{
    if (wxIsAbsolutePath(page))
        return page;
    else
        return m_BasePath + page;
}



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
    if (m_Contents)
    {
        for (i = 0; i < m_ContentsCnt; i++)
        {
            delete[] m_Contents[i].m_Page;
            delete[] m_Contents[i].m_Name;
        }
        free(m_Contents);
    }
    if (m_Index)
    {
        for (i = 0; i < m_IndexCnt; i++)
        {
            delete[] m_Index[i].m_Page;
            delete[] m_Index[i].m_Name;
        }
        free(m_Index);
    }
}

bool wxHtmlHelpData::LoadMSProject(wxHtmlBookRecord *book, wxFileSystem& fsys, const wxString& indexfile, const wxString& contentsfile)
{
    wxFSFile *f;
    wxHtmlFilterHTML filter;
    wxString buf;
    wxString string;

    HP_Parser parser;
    HP_TagHandler *handler = new HP_TagHandler(book);
    parser.AddTagHandler(handler);

    f = ( contentsfile.IsEmpty() ? (wxFSFile*) NULL : fsys.OpenFile(contentsfile) );
    if (f)
    {
        buf.clear();
        buf = filter.ReadFile(*f);
        delete f;
        handler->ReadIn(m_Contents, m_ContentsCnt);
        parser.Parse(buf);
        handler->WriteOut(m_Contents, m_ContentsCnt);
    }
    else
    {
        wxLogError(_("Cannot open contents file: %s"), contentsfile.c_str());
    }

    f = ( indexfile.IsEmpty() ? (wxFSFile*) NULL : fsys.OpenFile(indexfile) );
    if (f)
    {
        buf.clear();
        buf = filter.ReadFile(*f);
        delete f;
        handler->ReadIn(m_Index, m_IndexCnt);
        parser.Parse(buf);
        handler->WriteOut(m_Index, m_IndexCnt);
    }
    else if (!indexfile.IsEmpty())
    {
        wxLogError(_("Cannot open index file: %s"), indexfile.c_str());
    }
    return TRUE;
}



inline static void CacheWriteInt32(wxOutputStream *f, wxInt32 value)
{
    wxInt32 x = wxINT32_SWAP_ON_BE(value);
    f->Write(&x, sizeof(x));
}

inline static wxInt32 CacheReadInt32(wxInputStream *f)
{
    wxInt32 x;
    f->Read(&x, sizeof(x));
    return wxINT32_SWAP_ON_BE(x);
}

inline static void CacheWriteString(wxOutputStream *f, const wxChar *str)
{    
#if wxUSE_UNICODE
    wxWX2MBbuf mbstr(wxConvUTF8.cWX2MB(str));
#else
    const wxChar *mbstr = str;
#endif
    size_t len = strlen(mbstr)+1;
    CacheWriteInt32(f, len);
    f->Write(mbstr, len);
}

inline static wxChar* CacheReadString(wxInputStream *f)
{
    char *str;
    size_t len = (size_t)CacheReadInt32(f);
    str = new char[len];
    f->Read(str, len);
#if !wxUSE_UNICODE
    return str;
#else
    wxMB2WXbuf wxstr(wxConvUTF8.cMB2WX(str));
    wxChar *outstr = new wxChar[wxStrlen(wxstr)+1];
    wxStrcpy(outstr, wxstr);
    return outstr;
#endif
}

#define CURRENT_CACHED_BOOK_VERSION     4

// Additional flags to detect incompatibilities of the runtime environment:
#define CACHED_BOOK_FORMAT_FLAGS \
                     (wxUSE_UNICODE << 0)


bool wxHtmlHelpData::LoadCachedBook(wxHtmlBookRecord *book, wxInputStream *f)
{
    int i, st;
    wxInt32 version;

    /* load header - version info : */
    version = CacheReadInt32(f);
    
    if (version != CURRENT_CACHED_BOOK_VERSION)
    {
        // NB: We can just silently return FALSE here and don't worry about
        //     it anymore, because AddBookParam will load the MS project in
        //     absence of (properly versioned) .cached file and automatically 
        //     create new .cached file immediately afterward.
        return FALSE;
    }

    if (CacheReadInt32(f) != CACHED_BOOK_FORMAT_FLAGS)
        return FALSE;

    /* load contents : */
    st = m_ContentsCnt;
    m_ContentsCnt += CacheReadInt32(f);
    m_Contents = (wxHtmlContentsItem*) realloc(m_Contents,
                                               (m_ContentsCnt / wxHTML_REALLOC_STEP + 1) *
                                               wxHTML_REALLOC_STEP * sizeof(wxHtmlContentsItem));
    for (i = st; i < m_ContentsCnt; i++)
    {
        m_Contents[i].m_Level = CacheReadInt32(f);
        m_Contents[i].m_ID = CacheReadInt32(f);
        m_Contents[i].m_Name = CacheReadString(f);
        m_Contents[i].m_Page = CacheReadString(f);
        m_Contents[i].m_Book = book;
    }

    /* load index : */
    st = m_IndexCnt;
    m_IndexCnt += CacheReadInt32(f);
    m_Index = (wxHtmlContentsItem*) realloc(m_Index, (m_IndexCnt / wxHTML_REALLOC_STEP + 1) *
                                                     wxHTML_REALLOC_STEP * sizeof(wxHtmlContentsItem));
    for (i = st; i < m_IndexCnt; i++)
    {
        m_Index[i].m_Name = CacheReadString(f);
        m_Index[i].m_Page = CacheReadString(f);
        m_Index[i].m_Book = book;
    }
    return TRUE;
}


bool wxHtmlHelpData::SaveCachedBook(wxHtmlBookRecord *book, wxOutputStream *f)
{
    int i;
    wxInt32 cnt;

    /* save header - version info : */
    CacheWriteInt32(f, CURRENT_CACHED_BOOK_VERSION);
    CacheWriteInt32(f, CACHED_BOOK_FORMAT_FLAGS);

    /* save contents : */
    for (cnt = 0, i = 0; i < m_ContentsCnt; i++) 
        if (m_Contents[i].m_Book == book && m_Contents[i].m_Level > 0)
            cnt++;
    CacheWriteInt32(f, cnt);

    for (i = 0; i < m_ContentsCnt; i++)
    {
        if (m_Contents[i].m_Book != book || m_Contents[i].m_Level == 0) 
            continue;
        CacheWriteInt32(f, m_Contents[i].m_Level);
        CacheWriteInt32(f, m_Contents[i].m_ID);
        CacheWriteString(f, m_Contents[i].m_Name);
        CacheWriteString(f, m_Contents[i].m_Page);
    }

    /* save index : */
    for (cnt = 0, i = 0; i < m_IndexCnt; i++) 
        if (m_Index[i].m_Book == book && m_Index[i].m_Level > 0) 
            cnt++;
    CacheWriteInt32(f, cnt);

    for (i = 0; i < m_IndexCnt; i++)
    {
        if (m_Index[i].m_Book != book || m_Index[i].m_Level == 0) 
            continue;
        CacheWriteString(f, m_Index[i].m_Name);
        CacheWriteString(f, m_Index[i].m_Page);
    }
    return TRUE;
}


void wxHtmlHelpData::SetTempDir(const wxString& path)
{
    if (path == wxEmptyString) m_TempPath = path;
    else
    {
        if (wxIsAbsolutePath(path)) m_TempPath = path;
        else m_TempPath = wxGetCwd() + _T("/") + path;

        if (m_TempPath[m_TempPath.Length() - 1] != _T('/'))
            m_TempPath << _T('/');
    }
}



static wxString SafeFileName(const wxString& s)
{
    wxString res(s);
    res.Replace(wxT("#"), wxT("_"));
    res.Replace(wxT(":"), wxT("_"));
    res.Replace(wxT("\\"), wxT("_"));
    res.Replace(wxT("/"), wxT("_"));
    return res;
}

bool wxHtmlHelpData::AddBookParam(const wxFSFile& bookfile,
                                  wxFontEncoding encoding,
                                  const wxString& title, const wxString& contfile,
                                  const wxString& indexfile, const wxString& deftopic,
                                  const wxString& path)
{
    wxFileSystem fsys;
    wxFSFile *fi;
    wxHtmlBookRecord *bookr;

    int IndexOld = m_IndexCnt,
        ContentsOld = m_ContentsCnt;

    if (!path.IsEmpty())
        fsys.ChangePathTo(path, TRUE);

    size_t booksCnt = m_BookRecords.GetCount();
    for (size_t i = 0; i < booksCnt; i++)
    {
        if ( m_BookRecords[i].GetBookFile() == bookfile.GetLocation() )
            return TRUE; // book is (was) loaded
    }

    bookr = new wxHtmlBookRecord(bookfile.GetLocation(), fsys.GetPath(), title, deftopic);
    
    if (m_ContentsCnt % wxHTML_REALLOC_STEP == 0)
        m_Contents = (wxHtmlContentsItem*) realloc(m_Contents, (m_ContentsCnt + wxHTML_REALLOC_STEP) * sizeof(wxHtmlContentsItem));
    m_Contents[m_ContentsCnt].m_Level = 0;
    m_Contents[m_ContentsCnt].m_ID = 0;
    m_Contents[m_ContentsCnt].m_Page = new wxChar[deftopic.Length() + 1];
    wxStrcpy(m_Contents[m_ContentsCnt].m_Page, deftopic.c_str());
    m_Contents[m_ContentsCnt].m_Name = new wxChar [title.Length() + 1];
    wxStrcpy(m_Contents[m_ContentsCnt].m_Name, title.c_str());
    m_Contents[m_ContentsCnt].m_Book = bookr;

    // store the contents index for later
    int cont_start = m_ContentsCnt++;

    // Try to find cached binary versions:
    // 1. save file as book, but with .hhp.cached extension
    // 2. same as 1. but in temp path
    // 3. otherwise or if cache load failed, load it from MS.

    fi = fsys.OpenFile(bookfile.GetLocation() + wxT(".cached"));

    if (fi == NULL ||
          fi->GetModificationTime() < bookfile.GetModificationTime() ||
          !LoadCachedBook(bookr, fi->GetStream()))
    {
        if (fi != NULL) delete fi;
        fi = fsys.OpenFile(m_TempPath + wxFileNameFromPath(bookfile.GetLocation()) + wxT(".cached"));
        if (m_TempPath == wxEmptyString || fi == NULL ||
            fi->GetModificationTime() < bookfile.GetModificationTime() ||
            !LoadCachedBook(bookr, fi->GetStream()))
        {
            LoadMSProject(bookr, fsys, indexfile, contfile);
            if (m_TempPath != wxEmptyString)
            {
                wxFileOutputStream *outs = new wxFileOutputStream(m_TempPath +
                                                  SafeFileName(wxFileNameFromPath(bookfile.GetLocation())) + wxT(".cached"));
                SaveCachedBook(bookr, outs);
                delete outs;
            }
        }
    }

    if (fi != NULL) delete fi;

    // Now store the contents range
    bookr->SetContentsRange(cont_start, m_ContentsCnt);

    // Convert encoding, if neccessary:
    if (encoding != wxFONTENCODING_SYSTEM)
    {
        wxFontEncodingArray a = wxEncodingConverter::GetPlatformEquivalents(encoding);
        if (a.GetCount() != 0 && a[0] != encoding)
        {
            int i;
            wxEncodingConverter conv;
            conv.Init(encoding, a[0]);

            for (i = IndexOld; i < m_IndexCnt; i++)
                conv.Convert(m_Index[i].m_Name);
            for (i = ContentsOld; i < m_ContentsCnt; i++)
                conv.Convert(m_Contents[i].m_Name);
        }
    }

    m_BookRecords.Add(bookr);
    if (m_IndexCnt > 0)
        qsort(m_Index, m_IndexCnt, sizeof(wxHtmlContentsItem), wxHtmlHelpIndexCompareFunc);

    return TRUE;
}


bool wxHtmlHelpData::AddBook(const wxString& book)
{
    if (book.Right(4).Lower() == wxT(".zip") ||
        book.Right(4).Lower() == wxT(".htb") /*html book*/)
    {
        wxFileSystem fsys;
        wxString s;
        bool rt = FALSE;

        s = fsys.FindFirst(book + wxT("#zip:") + wxT("*.hhp"), wxFILE);
        while (!s.IsEmpty())
        {
            if (AddBook(s)) rt = TRUE;
            s = fsys.FindNext();
        }

        return rt;
    }
    else
    {
        wxFSFile *fi;
        wxFileSystem fsys;

        wxString title = _("noname"),
                 safetitle,
                 start = wxEmptyString,
                 contents = wxEmptyString,
                 index = wxEmptyString,
                 charset = wxEmptyString;

        fi = fsys.OpenFile(book);
        if (fi == NULL)
        {
            wxLogError(_("Cannot open HTML help book: %s"), book.c_str());
            return FALSE;
        }
        fsys.ChangePathTo(book);

        const wxChar *lineptr;
        wxChar linebuf[300];
        wxString tmp;
        wxHtmlFilterPlainText filter;
        tmp = filter.ReadFile(*fi);
        lineptr = tmp.c_str();

        do 
        {
            lineptr = ReadLine(lineptr, linebuf, 300);
            
            for (wxChar *ch = linebuf; *ch != wxT('\0') && *ch != wxT('='); ch++)
               *ch = tolower(*ch);

            if (wxStrstr(linebuf, _T("title=")) == linebuf)
                title = linebuf + wxStrlen(_T("title="));
            if (wxStrstr(linebuf, _T("default topic=")) == linebuf)
                start = linebuf + wxStrlen(_T("default topic="));
            if (wxStrstr(linebuf, _T("index file=")) == linebuf)
                index = linebuf + wxStrlen(_T("index file="));
            if (wxStrstr(linebuf, _T("contents file=")) == linebuf)
                contents = linebuf + wxStrlen(_T("contents file="));
            if (wxStrstr(linebuf, _T("charset=")) == linebuf)
                charset = linebuf + wxStrlen(_T("charset="));
        } while (lineptr != NULL);

        wxFontEncoding enc;
        if (charset == wxEmptyString) enc = wxFONTENCODING_SYSTEM;
        else enc = wxFontMapper::Get()->CharsetToEncoding(charset);
        bool rtval = AddBookParam(*fi, enc,
                                  title, contents, index, start, fsys.GetPath());
        delete fi;
        return rtval;
    }
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
    for (i = 0; i < cnt; i++)
    {
        f = fsys.OpenFile(m_BookRecords[i].GetFullPath(x));
        if (f)
        {
            url = m_BookRecords[i].GetFullPath(x);
            delete f;
            return url;
        }
    }


    /* 2. try to find a book: */

    for (i = 0; i < cnt; i++)
    {
        if (m_BookRecords[i].GetTitle() == x)
        {
            url = m_BookRecords[i].GetFullPath(m_BookRecords[i].GetStart());
            return url;
        }
    }

    /* 3. try to find in contents: */

    cnt = m_ContentsCnt;
    for (i = 0; i < cnt; i++)
    {
        if (wxStrcmp(m_Contents[i].m_Name, x) == 0)
        {
            url = m_Contents[i].GetFullPath();
            return url;
        }
    }


    /* 4. try to find in index: */

    cnt = m_IndexCnt;
    for (i = 0; i < cnt; i++)
    {
        if (wxStrcmp(m_Index[i].m_Name, x) == 0)
        {
            url = m_Index[i].GetFullPath();
            return url;
        }
    }

    return url;
}

wxString wxHtmlHelpData::FindPageById(int id)
{
    int i;
    wxString url(wxEmptyString);

    for (i = 0; i < m_ContentsCnt; i++)
    {
        if (m_Contents[i].m_ID == id)
        {
            url = m_Contents[i].GetFullPath();
            return url;
        }
    }

    return url;
}

//----------------------------------------------------------------------------------
// wxHtmlSearchStatus functions
//----------------------------------------------------------------------------------

wxHtmlSearchStatus::wxHtmlSearchStatus(wxHtmlHelpData* data, const wxString& keyword,
                                       bool case_sensitive, bool whole_words_only,
                                       const wxString& book)
{
    m_Data = data;
    m_Keyword = keyword;
    wxHtmlBookRecord* bookr = NULL;
    if (book != wxEmptyString)
    {
        // we have to search in a specific book. Find it first
        int i, cnt = data->m_BookRecords.GetCount();
        for (i = 0; i < cnt; i++)
            if (data->m_BookRecords[i].GetTitle() == book)
            {
                bookr = &(data->m_BookRecords[i]);
                m_CurIndex = bookr->GetContentsStart();
                m_MaxIndex = bookr->GetContentsEnd();
                break;
            }
        // check; we won't crash if the book doesn't exist, but it's Bad Anyway.
        wxASSERT(bookr);
    }
    if (! bookr)
    {
        // no book specified; search all books
        m_CurIndex = 0;
        m_MaxIndex = m_Data->m_ContentsCnt;
    }
    m_Engine.LookFor(keyword, case_sensitive, whole_words_only);
    m_Active = (m_CurIndex < m_MaxIndex);
    m_LastPage = NULL;
}

bool wxHtmlSearchStatus::Search()
{
    wxFSFile *file;
    int i = m_CurIndex;  // shortcut
    bool found = FALSE;
    wxChar *thepage;

    if (!m_Active)
    {
        // sanity check. Illegal use, but we'll try to prevent a crash anyway
        wxASSERT(m_Active);
        return FALSE;
    }

    m_Name = wxEmptyString;
    m_ContentsItem = NULL;
    thepage = m_Data->m_Contents[i].m_Page;

    m_Active = (++m_CurIndex < m_MaxIndex);
    // check if it is same page with different anchor:
    if (m_LastPage != NULL)
    {
        wxChar *p1, *p2;
        for (p1 = thepage, p2 = m_LastPage;
             *p1 != 0 && *p1 != _T('#') && *p1 == *p2; p1++, p2++) {}

        m_LastPage = thepage;

        if (*p1 == 0 || *p1 == _T('#'))
            return FALSE;
    }
    else m_LastPage = thepage;

    wxFileSystem fsys;
    file = fsys.OpenFile(m_Data->m_Contents[i].m_Book->GetFullPath(thepage));
    if (file)
    {
        if (m_Engine.Scan(*file))
        {
            m_Name = m_Data->m_Contents[i].m_Name;
            m_ContentsItem = m_Data->m_Contents + i;
            found = TRUE;
        }
        delete file;
    }
    return found;
}








//--------------------------------------------------------------------------------
// wxHtmlSearchEngine
//--------------------------------------------------------------------------------

void wxHtmlSearchEngine::LookFor(const wxString& keyword, bool case_sensitive, bool whole_words_only)
{
    m_CaseSensitive = case_sensitive;
    m_WholeWords = whole_words_only;
    if (m_Keyword) delete[] m_Keyword;
    m_Keyword = new wxChar[keyword.Length() + 1];
    wxStrcpy(m_Keyword, keyword.c_str());

    if (!m_CaseSensitive)
    {
        for (int i = wxStrlen(m_Keyword) - 1; i >= 0; i--)
        {
            if ((m_Keyword[i] >= wxT('A')) && (m_Keyword[i] <= wxT('Z')))
                m_Keyword[i] += wxT('a') - wxT('A');
        }
    }
}


static inline bool WHITESPACE(wxChar c)
{
    return c == _T(' ') || c == _T('\n') || c == _T('\r') || c == _T('\t');
}

bool wxHtmlSearchEngine::Scan(const wxFSFile& file)
{
    wxASSERT_MSG(m_Keyword != NULL, wxT("wxHtmlSearchEngine::LookFor must be called before scanning!"));

    int i, j;
    int wrd = wxStrlen(m_Keyword);
    bool found = FALSE;
    wxHtmlFilterHTML filter;
    wxString tmp = filter.ReadFile(file);
    int lng = tmp.length();
    const wxChar *buf = tmp.c_str();

    if (!m_CaseSensitive)
        for (i = 0; i < lng; i++)
            tmp[(size_t)i] = (wxChar)wxTolower(tmp[(size_t)i]);

    if (m_WholeWords)
    {
        for (i = 0; i < lng - wrd; i++)
        {
            if (WHITESPACE(buf[i])) continue;
            j = 0;
            while ((j < wrd) && (buf[i + j] == m_Keyword[j])) j++;
            if (j == wrd && WHITESPACE(buf[i + j])) { found = TRUE; break; }
        }
    }

    else
    {
        for (i = 0; i < lng - wrd; i++)
        {
            j = 0;
            while ((j < wrd) && (buf[i + j] == m_Keyword[j])) j++;
            if (j == wrd) { found = TRUE; break; }
        }
    }

    return found;
}



#endif
