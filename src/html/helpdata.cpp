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

#if wxUSE_HTML && wxUSE_STREAMS

#ifndef WXPRECOMP
#include "wx/wx.h"
#endif

#include "wx/html/helpdata.h"
#include "wx/tokenzr.h"
#include "wx/wfstream.h"
#include "wx/busyinfo.h"
#include "wx/encconv.h"
#include "wx/fontmap.h"
#include "wx/log.h"
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



static int LINKAGEMODE IndexCompareFunc(const void *a, const void *b)
{
    return wxStrcmp(((wxHtmlContentsItem*)a) -> m_Name, ((wxHtmlContentsItem*)b) -> m_Name);
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
        HP_TagHandler(wxHtmlBookRecord *b) : wxHtmlTagHandler() {m_Book = b; m_Items = NULL; m_ItemsCnt = 0; m_Name = m_Page = wxEmptyString; m_Level = 0; m_ID = -1; }
        wxString GetSupportedTags() { return wxT("UL,OBJECT,PARAM"); }
        bool HandleTag(const wxHtmlTag& tag);
        void WriteOut(wxHtmlContentsItem*& array, int& size);
        void ReadIn(wxHtmlContentsItem* array, int size);
};


bool HP_TagHandler::HandleTag(const wxHtmlTag& tag)
{
    if (tag.GetName() == wxT("UL")) {
        m_Level++;
        ParseInner(tag);
        m_Level--;
        return TRUE;
    }
    else if (tag.GetName() == wxT("OBJECT")) {
        m_Name = m_Page = wxEmptyString;
        ParseInner(tag);

        if (!m_Page.IsEmpty())
        /* should be 'if (tag.GetParam("TYPE") == "text/sitemap")'
           but this works fine. Valid HHW's file may contain only two
           object tags:
           
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
        {
            if (m_ItemsCnt % wxHTML_REALLOC_STEP == 0)
                m_Items = (wxHtmlContentsItem*) realloc(m_Items, (m_ItemsCnt + wxHTML_REALLOC_STEP) * sizeof(wxHtmlContentsItem));
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
    else { // "PARAM"
        if (m_Name == wxEmptyString && tag.GetParam(wxT("NAME")) == wxT("Name")) 
        {
            m_Name = tag.GetParam(wxT("VALUE"));
            if (m_Name.Find(wxT('&')) != -1) 
            {
        #define ESCSEQ(escape, subst)  \
                          { _T("&") _T(escape) _T(";"), _T("&") _T(escape) _T(" "), _T(subst) } 
                static wxChar* substitutions[][3] = 
                        {
                        ESCSEQ("quot", "\""),
                        ESCSEQ("lt", "<"),
                        ESCSEQ("gt", ">"),

                        ESCSEQ("nbsp", " "),
                        ESCSEQ("iexcl", "!"),
                        ESCSEQ("cent", "\242"/*¢*/),

                        ESCSEQ("yen", " "),
                        ESCSEQ("brkbar", " "),
                        ESCSEQ("sect", " "),
                        ESCSEQ("uml", " "),

                        ESCSEQ("copy", "(c)"),
                        ESCSEQ("ordf", " "),
                        ESCSEQ("laquo", " "),
                        ESCSEQ("not", " "),

                        ESCSEQ("reg", "(r)"),

                        ESCSEQ("deg", " "),
                        ESCSEQ("plusm", " "),

                        ESCSEQ("acute", " "),
                        ESCSEQ("micro", " "),
                        ESCSEQ("para", " "),

                        ESCSEQ("ordm", " "),
                        ESCSEQ("raquo", " "),

                        ESCSEQ("iquest", " "),
                        ESCSEQ("Agrave", "\300"/*À*/),

                        ESCSEQ("Acirc", "\302"/*Â*/),
                        ESCSEQ("Atilde", "\303"/*Ã*/),
                        ESCSEQ("Auml", "\304"/*Ä*/),
                        ESCSEQ("Aring", " "),
                        ESCSEQ("AElig", " "),
                        ESCSEQ("Ccedil", "\347"/*ç*/),
                        ESCSEQ("Egrave", "\310"/*È*/),
                        ESCSEQ("Eacute", "\311"/*É*/),
                        ESCSEQ("Ecirc", "\312"/*Ê*/),
                        ESCSEQ("Euml", "\313"/*Ë*/),
                        ESCSEQ("Igrave", "\314"/*Ì*/),

                        ESCSEQ("Icirc", "\316"/*Î*/),
                        ESCSEQ("Iuml", "\317"/*Ï*/),

                        ESCSEQ("Ntilde", "\321"/*Ñ*/),
                        ESCSEQ("Ograve", "\322"/*Ò*/),

                        ESCSEQ("Ocirc", "\324"/*Ô*/),
                        ESCSEQ("Otilde", "\325"/*Õ*/),
                        ESCSEQ("Ouml", "\326"/*Ö*/),

                        ESCSEQ("Oslash", " "),
                        ESCSEQ("Ugrave", "\331"/*Ù*/),

                        ESCSEQ("Ucirc", " "),
                        ESCSEQ("Uuml", "\334"/*Ü*/),

                        ESCSEQ("szlig", "\247"/*§*/),
                        ESCSEQ("agrave;","à"),
                        ESCSEQ("aacute", "\341"/*á*/),
                        ESCSEQ("acirc", "\342"/*â*/),
                        ESCSEQ("atilde", "\343"/*ã*/),
                        ESCSEQ("auml", "\344"/*ä*/),
                        ESCSEQ("aring", "a"),
                        ESCSEQ("aelig", "ae"),
                        ESCSEQ("ccedil", "\347"/*ç*/),
                        ESCSEQ("egrave", "\350"/*è*/),
                        ESCSEQ("eacute", "\351"/*é*/),
                        ESCSEQ("ecirc", "\352"/*ê*/),
                        ESCSEQ("euml", "\353"/*ë*/),
                        ESCSEQ("igrave", "\354"/*ì*/),
                        ESCSEQ("iacute", "\355"/*í*/),
                        ESCSEQ("icirc", " "),
                        ESCSEQ("iuml", "\357"/*ï*/),
                        ESCSEQ("eth", " "),
                        ESCSEQ("ntilde", "\361"/*ñ*/),
                        ESCSEQ("ograve", "\362"/*ò*/),
                        ESCSEQ("oacute", "\363"/*ó*/),
                        ESCSEQ("ocirc", "\364"/*ô*/),
                        ESCSEQ("otilde", "\365"/*õ*/),
                        ESCSEQ("ouml", "\366"/*ö*/),
                        ESCSEQ("divide", " "),
                        ESCSEQ("oslash", " "),
                        ESCSEQ("ugrave", "\371"/*ù*/),
                        ESCSEQ("uacute", "\372"/*ú*/),
                        ESCSEQ("ucirc", "\373"/*û*/),
                        ESCSEQ("uuml", "\374"/*ü*/),

                        ESCSEQ("yuml", ""),

                        /* this one should ALWAYS stay the last one!!! */
                        ESCSEQ("amp", "&"),

                        { NULL, NULL, NULL }
                        };

                for (int i = 0; substitutions[i][0] != NULL; i++) 
                {
                    m_Name.Replace(substitutions[i][0], substitutions[i][2], TRUE);
                    m_Name.Replace(substitutions[i][1], substitutions[i][2], TRUE);
                }
            }
        }
        if (tag.GetParam(wxT("NAME")) == wxT("Local")) m_Page = tag.GetParam(wxT("VALUE"));
        if (tag.GetParam(wxT("NAME")) == wxT("ID")) tag.ScanParam(wxT("VALUE"), wxT("%i"), &m_ID);
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

    f = ( contentsfile.IsEmpty() ? (wxFSFile*) NULL : fsys.OpenFile(contentsfile) );
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
    else
        wxLogError(_("Cannot open contents file: %s"), contentsfile.c_str());

    f = ( indexfile.IsEmpty() ? (wxFSFile*) NULL : fsys.OpenFile(indexfile) );
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
    else if (!indexfile.IsEmpty())
        wxLogError(_("Cannot open index file: %s"), indexfile.c_str());
    return TRUE;
}




#if wxUSE_UNICODE

#define READ_STRING(f, s, lng) { char tmpc; for (int i = 0; i < lng; i++) { f -> Read(&tmpc, 1); s[i] = (wxChar)tmpc;} }
#define WRITE_STRING(f, s, lng) { char tmpc; for (int i = 0; i < lng; i++) { tmpc = (char)s[i]; f -> Write(&tmpc, 1);} }

#else

#define READ_STRING(f, s, lng) f -> Read(s, lng * sizeof(char));
#define WRITE_STRING(f, s, lng) f -> Write(s, lng * sizeof(char));

#endif


#define CURRENT_CACHED_BOOK_VERSION     1

bool wxHtmlHelpData::LoadCachedBook(wxHtmlBookRecord *book, wxInputStream *f)
{
    int i, st;
    wxInt32 x;
    wxInt32 version;

    /* load header - version info : */

    f -> Read(&x, sizeof(x));
    version = wxINT32_SWAP_ON_BE(x);
    
    if (version != CURRENT_CACHED_BOOK_VERSION) 
    {
        wxLogError(_("Incorrect version of HTML help book"));
        return FALSE;
        // NOTE: when adding new version, please ensure backward compatibility!
    }
    
    /* load contents : */

    f -> Read(&x, sizeof(x));
    st = m_ContentsCnt;
    m_ContentsCnt += wxINT32_SWAP_ON_BE(x);
    m_Contents = (wxHtmlContentsItem*) realloc(m_Contents, 
                                               (m_ContentsCnt / wxHTML_REALLOC_STEP + 1) * 
                                               wxHTML_REALLOC_STEP * sizeof(wxHtmlContentsItem));
    for (i = st; i < m_ContentsCnt; i++) {
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_Level = wxINT32_SWAP_ON_BE(x);
        f -> Read(&x, sizeof(x));
        m_Contents[i].m_ID = wxINT32_SWAP_ON_BE(x);
        f -> Read(&x, sizeof(x)); x = wxINT32_SWAP_ON_BE(x);
        m_Contents[i].m_Name = new wxChar[x];
        READ_STRING(f, m_Contents[i].m_Name, x);
        f -> Read(&x, sizeof(x)); x = wxINT32_SWAP_ON_BE(x);
        m_Contents[i].m_Page = new wxChar[x];
        READ_STRING(f, m_Contents[i].m_Page, x);
        m_Contents[i].m_Book = book;
    }

    /* load index : */

    f -> Read(&x, sizeof(x));
    st = m_IndexCnt;
    m_IndexCnt += wxINT32_SWAP_ON_BE(x);
    m_Index = (wxHtmlContentsItem*) realloc(m_Index, (m_IndexCnt / wxHTML_REALLOC_STEP + 1) * 
                                                     wxHTML_REALLOC_STEP * sizeof(wxHtmlContentsItem));
    for (i = st; i < m_IndexCnt; i++) {
        f -> Read(&x, sizeof(x)); x = wxINT32_SWAP_ON_BE(x);
        m_Index[i].m_Name = new wxChar[x];
        READ_STRING(f, m_Index[i].m_Name, x);
        f -> Read(&x, sizeof(x)); x = wxINT32_SWAP_ON_BE(x);
        m_Index[i].m_Page = new wxChar[x];
        READ_STRING(f, m_Index[i].m_Page, x);
        m_Index[i].m_Book = book;
    }
    return TRUE;
}


bool wxHtmlHelpData::SaveCachedBook(wxHtmlBookRecord *book, wxOutputStream *f)
{
    int i;
    wxInt32 x;

    /* save header - version info : */

    x = wxINT32_SWAP_ON_BE(CURRENT_CACHED_BOOK_VERSION);
    f -> Write(&x, sizeof(x));

    /* save contents : */

    x = 0;
    for (i = 0; i < m_ContentsCnt; i++) if (m_Contents[i].m_Book == book && m_Contents[i].m_Level > 0) x++;
    x = wxINT32_SWAP_ON_BE(x);
    f -> Write(&x, sizeof(x));
    for (i = 0; i < m_ContentsCnt; i++) {
        if (m_Contents[i].m_Book != book || m_Contents[i].m_Level == 0) continue;
        x = wxINT32_SWAP_ON_BE(m_Contents[i].m_Level);
        f -> Write(&x, sizeof(x));
        x = wxINT32_SWAP_ON_BE(m_Contents[i].m_ID);
        f -> Write(&x, sizeof(x));
        x = wxINT32_SWAP_ON_BE(wxStrlen(m_Contents[i].m_Name) + 1);
        f -> Write(&x, sizeof(x));
        WRITE_STRING(f, m_Contents[i].m_Name, x);
        x = wxINT32_SWAP_ON_BE(wxStrlen(m_Contents[i].m_Page) + 1);
        f -> Write(&x, sizeof(x));
        WRITE_STRING(f, m_Contents[i].m_Page, x);
    }

    /* save index : */

    x = 0;
    for (i = 0; i < m_IndexCnt; i++) if (m_Index[i].m_Book == book && m_Index[i].m_Level > 0) x++;
    x = wxINT32_SWAP_ON_BE(x);
    f -> Write(&x, sizeof(x));
    for (i = 0; i < m_IndexCnt; i++) {
        if (m_Index[i].m_Book != book || m_Index[i].m_Level == 0) continue;
        x = wxINT32_SWAP_ON_BE(wxStrlen(m_Index[i].m_Name) + 1);
        f -> Write(&x, sizeof(x));
        WRITE_STRING(f, m_Index[i].m_Name, x);
        x = wxINT32_SWAP_ON_BE(wxStrlen(m_Index[i].m_Page) + 1);
        f -> Write(&x, sizeof(x));
        WRITE_STRING(f, m_Index[i].m_Page, x);
    }
    return TRUE;
}


void wxHtmlHelpData::SetTempDir(const wxString& path)
{
    if (path == wxEmptyString) m_TempPath = path;
    else {
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

    if (! path.IsEmpty())
        fsys.ChangePathTo(path, TRUE);

    bookr = new wxHtmlBookRecord(fsys.GetPath(), title, deftopic);

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
          fi -> GetModificationTime() < bookfile.GetModificationTime() || 
          !LoadCachedBook(bookr, fi -> GetStream()))
    {
        if (fi != NULL) delete fi;
        fi = fsys.OpenFile(m_TempPath + wxFileNameFromPath(bookfile.GetLocation()) + wxT(".cached"));
        if (m_TempPath == wxEmptyString || fi == NULL || 
            fi -> GetModificationTime() < bookfile.GetModificationTime() || 
            !LoadCachedBook(bookr, fi -> GetStream()))
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
        qsort(m_Index, m_IndexCnt, sizeof(wxHtmlContentsItem), IndexCompareFunc);

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
        wxInputStream *s;
        wxString bookFull;

        int sz;
        char *buff, *lineptr;
        char linebuf[300];

        wxString title = _("noname"),
                 safetitle,
                 start = wxEmptyString,
                 contents = wxEmptyString, 
                 index = wxEmptyString,
                 charset = wxEmptyString;

        if (wxIsAbsolutePath(book)) bookFull = book;
        else bookFull = wxGetCwd() + "/" + book;

        fi = fsys.OpenFile(bookFull);
        if (fi == NULL) 
        {
            wxLogError(_("Cannot open HTML help book: %s"), bookFull.c_str());
            return FALSE;
        }
        fsys.ChangePathTo(bookFull);
        s = fi -> GetStream();
        sz = s -> GetSize();
        buff = new char[sz + 1];
        buff[sz] = 0;
        s -> Read(buff, sz);
        lineptr = buff;

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
            if (strstr(linebuf, "Charset=") == linebuf)
                charset = linebuf + strlen("Charset=");
        } while (lineptr != NULL);
        delete[] buff;
    
        wxFontEncoding enc;
        if (charset == wxEmptyString) enc = wxFONTENCODING_SYSTEM;
        else enc = wxTheFontMapper -> CharsetToEncoding(charset);
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
        if (wxStrcmp(m_Contents[i].m_Name, x) == 0) {
            url = m_Contents[i].m_Book -> GetBasePath() + m_Contents[i].m_Page;
            return url;
        }
    }


    /* 4. try to find in index: */

    cnt = m_IndexCnt;
    for (i = 0; i < cnt; i++) {
        if (wxStrcmp(m_Index[i].m_Name, x) == 0) {
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
                                       bool case_sensitive, bool whole_words_only,
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

    if (!m_Active) {
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
    file = fsys.OpenFile(m_Data->m_Contents[i].m_Book -> GetBasePath() + thepage);
    if (file) 
    {
        if (m_Engine.Scan(file -> GetStream())) {
            m_Name = m_Data->m_Contents[i].m_Name;
            m_ContentsItem = m_Data->m_Contents + i;
            found = TRUE;
        }
        delete file;
    }
    return found;
}








//--------------------------------------------------------------------------------
// wxSearchEngine
//--------------------------------------------------------------------------------

void wxSearchEngine::LookFor(const wxString& keyword, bool case_sensitive, bool whole_words_only)
{
    m_CaseSensitive = case_sensitive;
    m_WholeWords = whole_words_only;
    if (m_Keyword) delete[] m_Keyword;
    m_Keyword = new wxChar[keyword.Length() + 1];
    wxStrcpy(m_Keyword, keyword.c_str());
    
    if (!m_CaseSensitive)
        for (int i = wxStrlen(m_Keyword) - 1; i >= 0; i--)
            if ((m_Keyword[i] >= wxT('A')) && (m_Keyword[i] <= wxT('Z')))
                m_Keyword[i] += wxT('a') - wxT('A');
}



#define WHITESPACE(c)  (c == ' ' || c == '\n' || c == '\r' || c == '\t')

bool wxSearchEngine::Scan(wxInputStream *stream)
{
    wxASSERT_MSG(m_Keyword != NULL, wxT("wxSearchEngine::LookFor must be called before scanning!"));

    int i, j;
    int lng = stream ->GetSize();
    int wrd = wxStrlen(m_Keyword);
    bool found = FALSE;
    char *buf = new char[lng + 1];
    stream -> Read(buf, lng);
    buf[lng] = 0;

    if (!m_CaseSensitive)
        for (i = 0; i < lng; i++)
            if ((buf[i] >= 'A') && (buf[i] <= 'Z')) buf[i] += 'a' - 'A';

    if (m_WholeWords)
    {
        for (i = 0; i < lng - wrd; i++) {
            if (WHITESPACE(buf[i])) continue;
            j = 0;
            while ((j < wrd) && (buf[i + j] == m_Keyword[j])) j++;
            if (j == wrd && WHITESPACE(buf[i + j])) {found = TRUE; break; }
        }
    }
    
    else
    {
        for (i = 0; i < lng - wrd; i++) {
            j = 0;
            while ((j < wrd) && (buf[i + j] == m_Keyword[j])) j++;
            if (j == wrd) {found = TRUE; break; }
        }
    }

    delete[] buf;
    return found;
}




#endif
