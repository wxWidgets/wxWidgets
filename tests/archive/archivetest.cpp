///////////////////////////////////////////////////////////////////////////////
// Name:        tests/archive/archive.cpp
// Purpose:     Test the archive classes
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#if wxUSE_STREAMS

#define WX_TEST_ARCHIVE_ITERATOR

#include "wx/zipstrm.h"
#include "wx/mstream.h"
#include "wx/wfstream.h"
#include "wx/dir.h"
#include "wx/cppunit.h"
#include <string>
#include <list>
#include <sys/stat.h>

// Check whether member templates can be used
//
#if defined __GNUC__ && \
    (__GNUC__ >= 3 || (__GNUC__ == 2 && __GNUC_MINOR__ >= 95))
#   define WXARC_MEMBER_TEMPLATES
#endif
#if defined _MSC_VER && _MSC_VER >= 1310
#   define WXARC_MEMBER_TEMPLATES
#endif
#if defined __BORLANDC__ && __BORLANDC__ >= 0x530
#   define WXARC_MEMBER_TEMPLATES
#endif
#if defined __DMC__ && __DMC__ >= 0x832
#   define WXARC_MEMBER_TEMPLATES
#endif
#if defined __MWERKS__ && __MWERKS__ >= 0x2200
#   define WXARC_MEMBER_TEMPLATES
#endif
#if defined __HP_aCC && __HP_aCC > 33300
#   define WXARC_MEMBER_TEMPLATES
#endif
#if defined __SUNPRO_CC && __SUNPRO_CC > 0x500
#   define WXARC_MEMBER_TEMPLATES
#endif


///////////////////////////////////////////////////////////////////////////////
// Bit flags for options for the tests

enum Options
{
    PipeIn      = 0x01,     // input streams are non-seekable
    PipeOut     = 0x02,     // output streams are non-seekable
    Stub        = 0x04,     // the archive should be appended to a stub
    AllOptions  = 0x07
};


///////////////////////////////////////////////////////////////////////////////
// These structs are passed as the template parameter of the test case to
// specify a set of classes to use in the test. This allows either the generic
// wxArchiveXXX interface to be exercised or the specific interface for a
// particular archive format e.g. wxZipXXX.

struct ArchiveClasses
{
    typedef wxArchiveEntry EntryT;
    typedef wxArchiveInputStream InputStreamT;
    typedef wxArchiveOutputStream OutputStreamT;
    typedef wxArchiveClassFactory ClassFactoryT;
    typedef wxArchiveNotifier NotifierT;
    typedef wxArchiveIter IterT;
    typedef wxArchivePairIter PairIterT;
};

struct ZipClasses
{
    typedef wxZipEntry EntryT;
    typedef wxZipInputStream InputStreamT;
    typedef wxZipOutputStream OutputStreamT;
    typedef wxZipClassFactory ClassFactoryT;
    typedef wxZipNotifier NotifierT;
    typedef wxZipIter IterT;
    typedef wxZipPairIter PairIterT;
};


///////////////////////////////////////////////////////////////////////////////
// A class to hold a test entry

class TestEntry
{
public:
    TestEntry(const wxDateTime& dt, int len, const char *data);
    ~TestEntry() { delete [] m_data; }

    wxDateTime GetDateTime() const  { return m_dt; }
    wxFileOffset GetLength() const  { return m_len; }
    size_t GetSize() const          { return m_len; }
    const char *GetData() const     { return m_data; }
    wxString GetComment() const     { return m_comment; }
    bool IsText() const             { return m_isText; }

    void SetComment(const wxString& comment) { m_comment = comment; }
    void SetDateTime(const wxDateTime& dt)   { m_dt = dt; }

private:
    wxDateTime m_dt;
    size_t m_len;
    const char *m_data;
    wxString m_comment;
    bool m_isText;
};

TestEntry::TestEntry(const wxDateTime& dt, int len, const char *data)
  : m_dt(dt),
    m_len(len),
    m_isText(len > 0)
{
    char *d = new char[len];
    memcpy(d, data, len);
    m_data = d;

    for (int i = 0; i < len && m_isText; i++)
        m_isText = (signed char)m_data[i] > 0;
}


///////////////////////////////////////////////////////////////////////////////
// TestOutputStream and TestInputStream are memory streams which can be
// seekable or non-seekable.

class TestOutputStream : public wxOutputStream
{
public:
    TestOutputStream(int options);

    ~TestOutputStream() { delete [] m_data; }

    int GetOptions() const { return m_options; }
    wxFileOffset GetLength() const { return m_size; }

    // gives away the data, this stream is then empty, and can be reused
    void GetData(const char*& data, size_t& size);

    enum { STUB_SIZE = 2048, INITIAL_SIZE = 0x18000, SEEK_LIMIT = 0x100000 };

private:
    void Init();

    wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
    wxFileOffset OnSysTell() const;
    size_t OnSysWrite(const void *buffer, size_t size);

    int m_options;
    size_t m_pos;
    size_t m_capacity;
    size_t m_size;
    char *m_data;
};

TestOutputStream::TestOutputStream(int options)
  : m_options(options)
{
    Init();
}

void TestOutputStream::Init()
{
    m_data = NULL;
    m_size = 0;
    m_capacity = 0;
    m_pos = 0;

    if (m_options & Stub) {
        wxCharBuffer buf(STUB_SIZE);
        memset(buf.data(), 0, STUB_SIZE);
        Write(buf, STUB_SIZE);
    }
}

wxFileOffset TestOutputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
    if ((m_options & PipeOut) == 0) {
        switch (mode) {
            case wxFromStart:   break;
            case wxFromCurrent: pos += m_pos; break;
            case wxFromEnd:     pos += m_size; break;
        }
        if (pos < 0 || pos > SEEK_LIMIT)
            return wxInvalidOffset;
        m_pos = pos;
        return m_pos;
    }
    return wxInvalidOffset;
}

wxFileOffset TestOutputStream::OnSysTell() const
{
    return (m_options & PipeOut) == 0 ? (wxFileOffset)m_pos : wxInvalidOffset;
}

size_t TestOutputStream::OnSysWrite(const void *buffer, size_t size)
{
    if (!IsOk() || !size)
        return 0;
    m_lasterror = wxSTREAM_WRITE_ERROR;

    size_t newsize = m_pos + size;
    wxCHECK(newsize > m_pos, 0);

    if (m_capacity < newsize) {
        size_t capacity = m_capacity ? m_capacity : INITIAL_SIZE;

        while (capacity < newsize) {
            capacity <<= 1;
            wxCHECK(capacity > m_capacity, 0);
        }

        char *buf = new char[capacity];
        if (m_data)
            memcpy(buf, m_data, m_capacity);
        delete [] m_data;
        m_data = buf;
        m_capacity = capacity;
    }

    memcpy(m_data + m_pos, buffer, size);
    m_pos += size;
    if (m_pos > m_size)
        m_size = m_pos;
    m_lasterror = wxSTREAM_NO_ERROR;

    return size;
}

void TestOutputStream::GetData(const char*& data, size_t& size)
{
    data = m_data;
    size = m_size;

    if (m_options & Stub) {
        char *d = m_data;
        size += STUB_SIZE;

        if (size > m_capacity) {
            d = new char[size];
            memcpy(d + STUB_SIZE, m_data, m_size);
            delete [] m_data;
        }
        else {
            memmove(d + STUB_SIZE, d, m_size);
        }

        memset(d, 0, STUB_SIZE);
        data = d;
    }

    Init();
    Reset();
}

class TestInputStream : public wxInputStream
{
public:
    // ctor takes the data from the output stream, which is then empty
    TestInputStream(TestOutputStream& out) : m_data(NULL) { SetData(out); }
    // this ctor 'dups'
    TestInputStream(const TestInputStream& in);
    ~TestInputStream() { delete [] m_data; }

    void Rewind();
    wxFileOffset GetLength() const { return m_size; }
    void SetData(TestOutputStream& out);

private:
    wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode);
    wxFileOffset OnSysTell() const;
    size_t OnSysRead(void *buffer, size_t size);

    int m_options;
    size_t m_pos;
    size_t m_size;
    const char *m_data;
};

TestInputStream::TestInputStream(const TestInputStream& in)
  : m_options(in.m_options),
    m_pos(in.m_pos),
    m_size(in.m_size)
{
    char *p = new char[m_size];
    memcpy(p, in.m_data, m_size);
    m_data = p;
}

void TestInputStream::Rewind()
{
    if ((m_options & Stub) && (m_options & PipeIn))
        m_pos = TestOutputStream::STUB_SIZE * 2;
    else
        m_pos = 0;

    if (m_wbacksize) {
        free(m_wback);
        m_wback = NULL;
        m_wbacksize = 0;
        m_wbackcur = 0;
    }
}

void TestInputStream::SetData(TestOutputStream& out)
{
    delete [] m_data;
    m_options = out.GetOptions();
    out.GetData(m_data, m_size);
    Rewind();
    Reset();
}

wxFileOffset TestInputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
    if ((m_options & PipeIn) == 0) {
        switch (mode) {
            case wxFromStart:   break;
            case wxFromCurrent: pos += m_pos; break;
            case wxFromEnd:     pos += m_size; break;
        }
        if (pos < 0 || pos > TestOutputStream::SEEK_LIMIT)
            return wxInvalidOffset;
        m_pos = pos;
        return m_pos;
    }
    return wxInvalidOffset;
}

wxFileOffset TestInputStream::OnSysTell() const
{
    return (m_options & PipeIn) == 0 ? (wxFileOffset)m_pos : wxInvalidOffset;
}

size_t TestInputStream::OnSysRead(void *buffer, size_t size)
{
    if (!IsOk() || !size)
        return 0;
    if (m_size <= m_pos) {
        m_lasterror = wxSTREAM_EOF;
        return 0;
    }

    if (m_size - m_pos < size)
        size = m_size - m_pos;
    memcpy(buffer, m_data + m_pos, size);
    m_pos += size;
    return size;
}


///////////////////////////////////////////////////////////////////////////////
// minimal non-intrusive reference counting pointer for testing the iterators

template <class T> class Ptr
{
public:
    explicit Ptr(T* p = NULL) : m_p(p), m_count(new int) { *m_count = 1; }
    Ptr(const Ptr& sp) : m_p(sp.m_p), m_count(sp.m_count) { ++*m_count; }
    ~Ptr() { Free(); }

    Ptr& operator =(const Ptr& sp) {
        if (&sp != this) {
            Free();
            m_p = sp.m_p;
            m_count = sp.m_count;
            ++*m_count;
        }
        return *this;
    }

    T* get() const { return m_p; }
    T* operator->() const { return m_p; }
    T& operator*() const { return *m_p; }

private:
    void Free() {
        if (--*m_count == 0) {
            delete m_p;
            delete m_count;
        }
    }

    T *m_p;
    int *m_count;
};


///////////////////////////////////////////////////////////////////////////////
// Clean-up for temp directory

class TempDir
{
public:
    TempDir();
    ~TempDir();
    wxString GetName() const { return m_tmp; }

private:
    void RemoveDir(wxString& path);
    wxString m_tmp;
    wxString m_original;
};

TempDir::TempDir()
{
    wxString tmp = wxFileName::CreateTempFileName(_T("arctest-"));
    if (tmp != wxEmptyString) {
        wxRemoveFile(tmp);
        m_original = wxGetCwd();
        CPPUNIT_ASSERT(wxMkdir(tmp, 0700));
        m_tmp = tmp;
        CPPUNIT_ASSERT(wxSetWorkingDirectory(tmp));
    }
}

TempDir::~TempDir()
{
    if (m_tmp != wxEmptyString) {
        wxSetWorkingDirectory(m_original);
        RemoveDir(m_tmp);
    }
}

void TempDir::RemoveDir(wxString& path)
{
    wxCHECK_RET(!m_tmp.empty() && path.substr(0, m_tmp.length()) == m_tmp,
                _T("remove '") + path + _T("' fails safety check"));

    const wxChar *files[] = {
        _T("text/empty"),
        _T("text/small"),
        _T("bin/bin1000"),
        _T("bin/bin4095"),
        _T("bin/bin4096"),
        _T("bin/bin4097"),
        _T("bin/bin16384"),
        _T("zero/zero5"),
        _T("zero/zero1024"),
        _T("zero/zero32768"),
        _T("zero/zero16385"),
        _T("zero/newname"),
        _T("newfile"),
    };

    const wxChar *dirs[] = {
        _T("text/"), _T("bin/"), _T("zero/"), _T("empty/")
    };

    wxString tmp = m_tmp + wxFileName::GetPathSeparator();
    size_t i;

    for (i = 0; i < WXSIZEOF(files); i++)
        wxRemoveFile(tmp + wxFileName(files[i], wxPATH_UNIX).GetFullPath());

    for (i = 0; i < WXSIZEOF(dirs); i++)
        wxRmdir(tmp + wxFileName(dirs[i], wxPATH_UNIX).GetFullPath());

    if (!wxRmdir(m_tmp))
        wxLogSysError(_T("can't remove temporary dir '%s'"), m_tmp.c_str());
}


///////////////////////////////////////////////////////////////////////////////
// wxFFile streams for piping to/from an external program

#if defined __UNIX__ || defined __MINGW32__
#   define WXARC_popen popen
#   define WXARC_pclose pclose
#elif defined _MSC_VER || defined __BORLANDC__
#   define WXARC_popen _popen
#   define WXARC_pclose _pclose
#else
#   define WXARC_NO_POPEN
#   define WXARC_popen(cmd, type) NULL
#   define WXARC_pclose(fp)
#endif

#ifdef __WXMSW__
#   define WXARC_b "b"
#else
#   define WXARC_b
#endif

class PFileInputStream : public wxFFileInputStream
{
public:
    PFileInputStream(const wxString& cmd) :
        wxFFileInputStream(WXARC_popen(cmd.mb_str(), "r" WXARC_b)) { }
    ~PFileInputStream()
        { WXARC_pclose(m_file->fp()); m_file->Detach(); }
};

class PFileOutputStream : public wxFFileOutputStream
{
public:
    PFileOutputStream(const wxString& cmd) :
        wxFFileOutputStream(WXARC_popen(cmd.mb_str(), "w" WXARC_b)) { }
    ~PFileOutputStream()
        { WXARC_pclose(m_file->fp()); m_file->Detach(); }
};


///////////////////////////////////////////////////////////////////////////////
// The test case

template <class Classes>
class ArchiveTestCase : public CppUnit::TestCase
{
public:
    ArchiveTestCase(const wxString& name,
                    int id,
                    typename Classes::ClassFactoryT *factory,
                    int options,
                    const wxString& archiver = wxEmptyString,
                    const wxString& unarchiver = wxEmptyString);

    ~ArchiveTestCase();

protected:
    // the classes to test
    typedef typename Classes::EntryT EntryT;
    typedef typename Classes::InputStreamT InputStreamT;
    typedef typename Classes::OutputStreamT OutputStreamT;
    typedef typename Classes::ClassFactoryT ClassFactoryT;
    typedef typename Classes::NotifierT NotifierT;
    typedef typename Classes::IterT IterT;
    typedef typename Classes::PairIterT PairIterT;

    // the entry point for the test
    void runTest();

    // create the test data
    void CreateTestData();
    TestEntry& Add(const char *name, const char *data, int len = -1);
    TestEntry& Add(const char *name, int len = 0, int value = EOF);

    // 'archive up' the test data
    void CreateArchive(wxOutputStream& out);
    void CreateArchive(wxOutputStream& out, const wxString& archiver);

    // perform various modifications on the archive
    void ModifyArchive(wxInputStream& in, wxOutputStream& out);

    // extract the archive and verify its contents
    void ExtractArchive(wxInputStream& in);
    void ExtractArchive(wxInputStream& in, const wxString& unarchiver);
    void VerifyDir(wxString& path, size_t rootlen = 0);

    // tests for the iterators
    void TestIterator(wxInputStream& in);
    void TestPairIterator(wxInputStream& in);
    void TestSmartIterator(wxInputStream& in);
    void TestSmartPairIterator(wxInputStream& in);

    // try reading two entries at the same time
    void ReadSimultaneous(TestInputStream& in);

    // overridables
    virtual void OnCreateArchive(OutputStreamT& WXUNUSED(arc)) { }
    virtual void OnSetNotifier(EntryT& entry);

    virtual void OnArchiveExtracted(InputStreamT& WXUNUSED(arc),
                                    int WXUNUSED(expectedTotal)) { }

    virtual void OnCreateEntry(     OutputStreamT& WXUNUSED(arc),
                                    TestEntry& WXUNUSED(testEntry),
                                    EntryT *entry = NULL) { (void)entry; }

    virtual void OnEntryExtracted(  EntryT& WXUNUSED(entry),
                                    const TestEntry& WXUNUSED(testEntry),
                                    InputStreamT *arc = NULL) { (void)arc; }

    typedef std::map<wxString, TestEntry*> TestEntries;
    TestEntries m_testEntries;              // test data
    std::auto_ptr<ClassFactoryT> m_factory; // factory to make classes
    int m_options;                          // test options
    wxDateTime m_timeStamp;                 // timestamp to give test entries
    int m_id;                               // select between the possibilites
    wxString m_archiver;                    // external archiver
    wxString m_unarchiver;                  // external unarchiver
};

template <class Classes>
ArchiveTestCase<Classes>::ArchiveTestCase(const wxString& name,
                                          int id,
                                          ClassFactoryT *factory,
                                          int options,
                                          const wxString& archiver,
                                          const wxString& unarchiver)
  : CppUnit::TestCase(std::string(name.mb_str())),
    m_factory(factory),
    m_options(options),
    m_timeStamp(1, wxDateTime::Mar, 2005, 12, 0),
    m_id(id),
    m_archiver(archiver),
    m_unarchiver(unarchiver)
{
}

template <class Classes>
ArchiveTestCase<Classes>::~ArchiveTestCase()
{
    TestEntries::iterator it;
    for (it = m_testEntries.begin(); it != m_testEntries.end(); ++it)
        delete it->second;
}

template <class Classes>
void ArchiveTestCase<Classes>::runTest()
{
    TestOutputStream out(m_options);

    CreateTestData();

    if (m_archiver.empty())
        CreateArchive(out);
    else
        CreateArchive(out, m_archiver);

    // check archive could be created
    CPPUNIT_ASSERT(out.GetLength() > 0);

    TestInputStream in(out);

    TestIterator(in);
    in.Rewind();
    TestPairIterator(in);
    in.Rewind();
    TestSmartIterator(in);
    in.Rewind();
    TestSmartPairIterator(in);
    in.Rewind();

    if ((m_options & PipeIn) == 0) {
        ReadSimultaneous(in);
        in.Rewind();
    }

    ModifyArchive(in, out);
    in.SetData(out);

    if (m_unarchiver.empty())
        ExtractArchive(in);
    else
        ExtractArchive(in, m_unarchiver);
    
    // check that all the test entries were found in the archive
    CPPUNIT_ASSERT(m_testEntries.empty());
}

template <class Classes>
void ArchiveTestCase<Classes>::CreateTestData()
{
    Add("text/");
    Add("text/empty", "");
    Add("text/small", "Small text file for testing\n"
                      "archive streams in wxWidgets\n");

    Add("bin/");
    Add("bin/bin1000", 1000);
    Add("bin/bin4095", 4095);
    Add("bin/bin4096", 4096);
    Add("bin/bin4097", 4097);
    Add("bin/bin16384", 16384);

    Add("zero/");
    Add("zero/zero5", 5, 0);
    Add("zero/zero1024", 1024, 109);
    Add("zero/zero32768", 32768, 106);
    Add("zero/zero16385", 16385, 119);

    Add("empty/");
}

template <class Classes>
TestEntry& ArchiveTestCase<Classes>::Add(const char *name,
                                         const char *data,
                                         int len /*=-1*/)
{
    if (len == -1)
        len = strlen(data);
    TestEntry*& entry = m_testEntries[wxString(name, *wxConvCurrent)];
    wxASSERT(entry == NULL);
    entry = new TestEntry(m_timeStamp, len, data);
    m_timeStamp += wxTimeSpan(0, 1, 30);
    return *entry;
}

template <class Classes>
TestEntry& ArchiveTestCase<Classes>::Add(const char *name,
                                         int len /*=0*/,
                                         int value /*=EOF*/)
{
    wxCharBuffer buf(len);
    for (int i = 0; i < len; i++)
        buf.data()[i] = value == EOF ? rand() : value;
    return Add(name, buf, len);
}

// Create an archive using the wx archive classes, write it to 'out'
//
template <class Classes>
void ArchiveTestCase<Classes>::CreateArchive(wxOutputStream& out)
{
    std::auto_ptr<OutputStreamT> arc(m_factory->NewStream(out));
    TestEntries::iterator it;

    OnCreateArchive(*arc);

    // We want to try creating entries in various different ways, 'choices'
    // is just a number used to select between all the various possibilities.
    int choices = m_id;

    for (it = m_testEntries.begin(); it != m_testEntries.end(); ++it) {
        choices += 5;
        TestEntry& testEntry = *it->second;
        wxString name = it->first;

        // It should be possible to create a directory entry just by supplying
        // a name that looks like a directory, or alternatively any old name
        // can be identified as a directory using SetIsDir or PutNextDirEntry
        bool setIsDir = name.Last() == _T('/') && (choices & 1);
        if (setIsDir)
            name.erase(name.length() - 1);

        // provide some context for the error message so that we know which
        // iteration of the loop we were on
        std::string error_entry((_T(" '") + name + _T("'")).mb_str());
        std::string error_context(" failed for entry" + error_entry);

        if ((choices & 2) || testEntry.IsText()) {
            // try PutNextEntry(EntryT *pEntry)
            std::auto_ptr<EntryT> entry(m_factory->NewEntry());
            entry->SetName(name, wxPATH_UNIX);
            if (setIsDir)
                entry->SetIsDir();
            entry->SetDateTime(testEntry.GetDateTime());
            entry->SetSize(testEntry.GetLength());
            OnCreateEntry(*arc, testEntry, entry.get());
            CPPUNIT_ASSERT_MESSAGE("PutNextEntry" + error_context,
                                   arc->PutNextEntry(entry.release()));
        }
        else {
            // try the convenience methods
            OnCreateEntry(*arc, testEntry);
            if (setIsDir)
                CPPUNIT_ASSERT_MESSAGE("PutNextDirEntry" + error_context,
                    arc->PutNextDirEntry(name, testEntry.GetDateTime()));
            else
                CPPUNIT_ASSERT_MESSAGE("PutNextEntry" + error_context,
                    arc->PutNextEntry(name, testEntry.GetDateTime(),
                                      testEntry.GetLength()));
        }

        if (name.Last() != _T('/')) {
            // for non-dirs write the data
            arc->Write(testEntry.GetData(), testEntry.GetSize());
            CPPUNIT_ASSERT_MESSAGE("LastWrite check" + error_context,
                arc->LastWrite() == testEntry.GetSize());
            // should work with or without explicit CloseEntry
            if (choices & 3)
                CPPUNIT_ASSERT_MESSAGE("CloseEntry" + error_context,
                    arc->CloseEntry());
        }

        CPPUNIT_ASSERT_MESSAGE("IsOk" + error_context, arc->IsOk());
    }

    // should work with or without explicit Close
    if (m_id % 2)
        CPPUNIT_ASSERT(arc->Close());
}

// Create an archive using an external archive program
//
template <class Classes>
void ArchiveTestCase<Classes>::CreateArchive(wxOutputStream& out,
                                             const wxString& archiver)
{
    // for an external archiver the test data need to be written to
    // temp files
    TempDir tmpdir;

    // write the files
    TestEntries::iterator i;
    for (i = m_testEntries.begin(); i != m_testEntries.end(); ++i) {
        wxFileName fn(i->first, wxPATH_UNIX);
        TestEntry& entry = *i->second;

        if (fn.IsDir()) {
            fn.Mkdir(0777, wxPATH_MKDIR_FULL);
        } else {
            wxFileName::Mkdir(fn.GetPath(), 0777, wxPATH_MKDIR_FULL);
            wxFFileOutputStream fileout(fn.GetFullPath());
            fileout.Write(entry.GetData(), entry.GetSize());
        }
    }

    for (i = m_testEntries.begin(); i != m_testEntries.end(); ++i) {
        wxFileName fn(i->first, wxPATH_UNIX);
        TestEntry& entry = *i->second;
        wxDateTime dt = entry.GetDateTime();
#ifdef __WXMSW__
        if (fn.IsDir())
            entry.SetDateTime(wxDateTime());
        else
#endif
            fn.SetTimes(NULL, &dt, NULL);
    }

    if ((m_options & PipeOut) == 0) {
        wxFileName fn(tmpdir.GetName());
        fn.SetExt(_T("arc"));
        wxString tmparc = fn.GetFullPath();

        // call the archiver to create an archive file
        system(wxString::Format(archiver, tmparc.c_str()).mb_str());

        // then load the archive file
        {
            wxFFileInputStream in(tmparc);
            if (in.Ok())
                out.Write(in);
        }

        wxRemoveFile(tmparc);
    }
    else {
        // for the non-seekable test, have the archiver output to "-"
        // and read the archive via a pipe
        PFileInputStream in(wxString::Format(archiver, _T("-")));
        if (in.Ok())
            out.Write(in);
    }
}

// Do a standard set of modification on an archive, delete an entry,
// rename an entry and add an entry
//
template <class Classes>
void ArchiveTestCase<Classes>::ModifyArchive(wxInputStream& in,
                                             wxOutputStream& out)
{
    std::auto_ptr<InputStreamT> arcIn(m_factory->NewStream(in));
    std::auto_ptr<OutputStreamT> arcOut(m_factory->NewStream(out));
    std::auto_ptr<EntryT> entry;

    const wxString deleteName = _T("bin/bin1000");
    const wxString renameFrom = _T("zero/zero1024");
    const wxString renameTo   = _T("zero/newname");
    const wxString newName    = _T("newfile");
    const char *newData       = "New file added as a test\n";

    arcOut->CopyArchiveMetaData(*arcIn);

    while (entry.reset(arcIn->GetNextEntry()), entry.get() != NULL) {
        OnSetNotifier(*entry);
        wxString name = entry->GetName(wxPATH_UNIX);

        // provide some context for the error message so that we know which
        // iteration of the loop we were on
        std::string error_entry((_T(" '") + name + _T("'")).mb_str());
        std::string error_context(" failed for entry" + error_entry);

        if (name == deleteName) {
            TestEntries::iterator it = m_testEntries.find(name);
            CPPUNIT_ASSERT_MESSAGE(
                "deletion failed (already deleted?) for" + error_entry,
                it != m_testEntries.end());
            TestEntry *p = it->second;
            m_testEntries.erase(it);
            delete p;
        }
        else {
            if (name == renameFrom) {
                entry->SetName(renameTo);
                TestEntries::iterator it = m_testEntries.find(renameFrom);
                CPPUNIT_ASSERT_MESSAGE(
                    "rename failed (already renamed?) for" + error_entry,
                    it != m_testEntries.end());
                TestEntry *p = it->second;
                m_testEntries.erase(it);
                m_testEntries[renameTo] = p;
            }

            CPPUNIT_ASSERT_MESSAGE("CopyEntry" + error_context,
                arcOut->CopyEntry(entry.release(), *arcIn));
        }
    }

    // check that the deletion and rename were done
    CPPUNIT_ASSERT(m_testEntries.count(deleteName) == 0);
    CPPUNIT_ASSERT(m_testEntries.count(renameFrom) == 0);
    CPPUNIT_ASSERT(m_testEntries.count(renameTo) == 1);

    // check that the end of the input archive was reached without error
    CPPUNIT_ASSERT(arcIn->Eof());

    // try adding a new entry
    TestEntry& testEntry = Add(newName.mb_str(), newData);
    entry.reset(m_factory->NewEntry());
    entry->SetName(newName);
    entry->SetDateTime(testEntry.GetDateTime());
    entry->SetSize(testEntry.GetLength());
    OnCreateEntry(*arcOut, testEntry, entry.get());
    OnSetNotifier(*entry);
    CPPUNIT_ASSERT(arcOut->PutNextEntry(entry.release()));
    CPPUNIT_ASSERT(arcOut->Write(newData, strlen(newData)).IsOk());

    // should work with or without explicit Close
    if (m_id % 2)
        CPPUNIT_ASSERT(arcOut->Close());
}

// Extract an archive using the wx archive classes
//
template <class Classes>
void ArchiveTestCase<Classes>::ExtractArchive(wxInputStream& in)
{
    typedef Ptr<EntryT> EntryPtr;
    typedef std::list<EntryPtr> Entries;
    typedef typename Entries::iterator EntryIter;

    std::auto_ptr<InputStreamT> arc(m_factory->NewStream(in));
    int expectedTotal = m_testEntries.size();
    EntryPtr entry;
    Entries entries;

    if ((m_options & PipeIn) == 0)
        OnArchiveExtracted(*arc, expectedTotal);

    while (entry = EntryPtr(arc->GetNextEntry()), entry.get() != NULL) {
        wxString name = entry->GetName(wxPATH_UNIX);

        // provide some context for the error message so that we know which
        // iteration of the loop we were on
        std::string error_entry((_T(" '") + name + _T("'")).mb_str());
        std::string error_context(" failed for entry" + error_entry);

        TestEntries::iterator it = m_testEntries.find(name);
        CPPUNIT_ASSERT_MESSAGE(
            "archive contains an entry that shouldn't be there" + error_entry,
            it != m_testEntries.end());

        const TestEntry& testEntry = *it->second;

        wxDateTime dt = testEntry.GetDateTime();
        if (dt.IsValid())
            CPPUNIT_ASSERT_MESSAGE("timestamp check" + error_context,
                                   dt == entry->GetDateTime());

        // non-seekable entries are allowed to have GetSize == wxInvalidOffset
        // until the end of the entry's data has been read past
        CPPUNIT_ASSERT_MESSAGE("entry size check" + error_context,
            testEntry.GetLength() == entry->GetSize() ||
            ((m_options & PipeIn) != 0 && entry->GetSize() == wxInvalidOffset));
        CPPUNIT_ASSERT_MESSAGE(
            "arc->GetLength() == entry->GetSize()" + error_context,
            arc->GetLength() == entry->GetSize());

        if (name.Last() != _T('/'))
        {
            CPPUNIT_ASSERT_MESSAGE("!IsDir" + error_context,
                !entry->IsDir());
            wxCharBuffer buf(testEntry.GetSize() + 1);
            CPPUNIT_ASSERT_MESSAGE("Read until Eof" + error_context,
                arc->Read(buf.data(), testEntry.GetSize() + 1).Eof());
            CPPUNIT_ASSERT_MESSAGE("LastRead check" + error_context,
                arc->LastRead() == testEntry.GetSize());
            CPPUNIT_ASSERT_MESSAGE("data compare" + error_context,
                !memcmp(buf.data(), testEntry.GetData(), testEntry.GetSize()));
        } else {
            CPPUNIT_ASSERT_MESSAGE("IsDir" + error_context, entry->IsDir());
        }

        // GetSize() must return the right result in all cases after all the
        // data has been read
        CPPUNIT_ASSERT_MESSAGE("entry size check" + error_context,
            testEntry.GetLength() == entry->GetSize());
        CPPUNIT_ASSERT_MESSAGE(
            "arc->GetLength() == entry->GetSize()" + error_context,
            arc->GetLength() == entry->GetSize());

        if ((m_options & PipeIn) == 0) {
            OnEntryExtracted(*entry, testEntry, arc.get());
            delete it->second;
            m_testEntries.erase(it);
        } else {
            entries.push_back(entry);
        }
    }

    // check that the end of the input archive was reached without error
    CPPUNIT_ASSERT(arc->Eof());

    // for non-seekable streams these data are only guaranteed to be
    // available once the end of the archive has been reached
    if (m_options & PipeIn) {
        for (EntryIter i = entries.begin(); i != entries.end(); ++i) {
            wxString name = (*i)->GetName(wxPATH_UNIX);
            TestEntries::iterator j = m_testEntries.find(name);
            OnEntryExtracted(**i, *j->second);
            delete j->second;
            m_testEntries.erase(j);
        }
        OnArchiveExtracted(*arc, expectedTotal);
    }
}

// Extract an archive using an external unarchive program
//
template <class Classes>
void ArchiveTestCase<Classes>::ExtractArchive(wxInputStream& in,
                                              const wxString& unarchiver)
{
    // for an external unarchiver, unarchive to a tempdir
    TempDir tmpdir;

    if ((m_options & PipeIn) == 0) {
        wxFileName fn(tmpdir.GetName());
        fn.SetExt(_T("arc"));
        wxString tmparc = fn.GetFullPath();
        
        if (m_options & Stub)
            in.SeekI(TestOutputStream::STUB_SIZE * 2);

        // write the archive to a temporary file
        {
            wxFFileOutputStream out(tmparc);
            if (out.Ok())
                out.Write(in);
        }

        // call unarchiver
        system(wxString::Format(unarchiver, tmparc.c_str()).mb_str());
        wxRemoveFile(tmparc);
    }
    else {
        // for the non-seekable test, have the archiver extract "-" and
        // feed it the archive via a pipe
        PFileOutputStream out(wxString::Format(unarchiver, _T("-")));
        if (out.Ok())
            out.Write(in);
    }

    wxString dir = tmpdir.GetName();
    VerifyDir(dir);
}

// Verifies the files produced by an external unarchiver are as expected
//
template <class Classes>
void ArchiveTestCase<Classes>::VerifyDir(wxString& path, size_t rootlen /*=0*/)
{
    wxDir dir;
    path += wxFileName::GetPathSeparator();
    int pos = path.length();
    wxString name;

    if (!rootlen)
        rootlen = pos;

    if (dir.Open(path) && dir.GetFirst(&name)) {
        do {
            path.replace(pos, wxString::npos, name);
            name = m_factory->GetInternalName(
                    path.substr(rootlen, wxString::npos));

            bool isDir = wxDirExists(path);
            if (isDir)
                name += _T("/");

            // provide some context for the error message so that we know which
            // iteration of the loop we were on
            std::string error_entry((_T(" '") + name + _T("'")).mb_str());
            std::string error_context(" failed for entry" + error_entry);

            TestEntries::iterator it = m_testEntries.find(name);
            CPPUNIT_ASSERT_MESSAGE(
                "archive contains an entry that shouldn't be there"
                    + error_entry,
                it != m_testEntries.end());

            const TestEntry& testEntry = *it->second;

#ifndef __WXMSW__
            CPPUNIT_ASSERT_MESSAGE("timestamp check" + error_context,
                                   testEntry.GetDateTime() ==
                                   wxFileName(path).GetModificationTime());
#endif
            if (!isDir) {
                wxFFileInputStream in(path);
                CPPUNIT_ASSERT_MESSAGE(
                    "entry not found in archive" + error_entry, in.Ok());

                size_t size = in.GetLength();
                wxCharBuffer buf(size);
                CPPUNIT_ASSERT_MESSAGE("Read" + error_context,
                    in.Read(buf.data(), size).LastRead() == size);
                CPPUNIT_ASSERT_MESSAGE("size check" + error_context,
                    testEntry.GetSize() == size);
                CPPUNIT_ASSERT_MESSAGE("data compare" + error_context,
                    memcmp(buf.data(), testEntry.GetData(), size) == 0);
            }
            else {
                VerifyDir(path, rootlen);
            }

            delete it->second;
            m_testEntries.erase(it);
        }
        while (dir.GetNext(&name));
    }
}

// test the simple iterators that give away ownership of an entry
//
template <class Classes>
void ArchiveTestCase<Classes>::TestIterator(wxInputStream& in)
{
    typedef std::list<EntryT*> ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;

    std::auto_ptr<InputStreamT> arc(m_factory->NewStream(in));
    size_t count = 0;

#ifdef WXARC_MEMBER_TEMPLATES
    ArchiveCatalog cat((IterT)*arc, IterT());
#else
    ArchiveCatalog cat;
    for (IterT i(*arc); i != IterT(); ++i)
        cat.push_back(*i);
#endif

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it) {
        std::auto_ptr<EntryT> entry(*it);
        count += m_testEntries.count(entry->GetName(wxPATH_UNIX));
    }

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());
    CPPUNIT_ASSERT(count == cat.size());
}

// test the pair iterators that can be used to load a std::map or wxHashMap
// these also give away ownership of entries
//
template <class Classes>
void ArchiveTestCase<Classes>::TestPairIterator(wxInputStream& in)
{
    typedef std::map<wxString, EntryT*> ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;

    std::auto_ptr<InputStreamT> arc(m_factory->NewStream(in));
    size_t count = 0;

#ifdef WXARC_MEMBER_TEMPLATES
    ArchiveCatalog cat((PairIterT)*arc, PairIterT());
#else
    ArchiveCatalog cat;
    for (PairIterT i(*arc); i != PairIterT(); ++i)
        cat.push_back(*i);
#endif

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it) {
        std::auto_ptr<EntryT> entry(it->second);
        count += m_testEntries.count(entry->GetName(wxPATH_UNIX));
    }

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());
    CPPUNIT_ASSERT(count == cat.size());
}

// simple iterators using smart pointers, no need to worry about ownership
//
template <class Classes>
void ArchiveTestCase<Classes>::TestSmartIterator(wxInputStream& in)
{
    typedef std::list<Ptr<EntryT> > ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;
    typedef wxArchiveIterator<InputStreamT, Ptr<EntryT> > Iter;

    std::auto_ptr<InputStreamT> arc(m_factory->NewStream(in));

#ifdef WXARC_MEMBER_TEMPLATES
    ArchiveCatalog cat((Iter)*arc, Iter());
#else
    ArchiveCatalog cat;
    for (Iter i(*arc); i != Iter(); ++i)
        cat.push_back(*i);
#endif

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it)
        CPPUNIT_ASSERT(m_testEntries.count((*it)->GetName(wxPATH_UNIX)));
}

// pair iterator using smart pointers
//
template <class Classes>
void ArchiveTestCase<Classes>::TestSmartPairIterator(wxInputStream& in)
{
    typedef std::map<wxString, Ptr<EntryT> > ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;
    typedef wxArchiveIterator<InputStreamT,
                std::pair<wxString, Ptr<EntryT> > > PairIter;

    std::auto_ptr<InputStreamT> arc(m_factory->NewStream(in));

#ifdef WXARC_MEMBER_TEMPLATES
    ArchiveCatalog cat((PairIter)*arc, PairIter());
#else
    ArchiveCatalog cat;
    for (PairIter i(*arc); i != PairIter(); ++i)
        cat.push_back(*i);
#endif

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it)
        CPPUNIT_ASSERT(m_testEntries.count(it->second->GetName(wxPATH_UNIX)));
}

// try reading two entries at the same time
//
template <class Classes>
void ArchiveTestCase<Classes>::ReadSimultaneous(TestInputStream& in)
{
    typedef std::map<wxString, Ptr<EntryT> > ArchiveCatalog;
    typedef wxArchiveIterator<InputStreamT,
                std::pair<wxString, Ptr<EntryT> > > PairIter;

    // create two archive input streams
    TestInputStream in2(in);
    std::auto_ptr<InputStreamT> arc(m_factory->NewStream(in));
    std::auto_ptr<InputStreamT> arc2(m_factory->NewStream(in2));

    // load the catalog
#ifdef WXARC_MEMBER_TEMPLATES
    ArchiveCatalog cat((PairIter)*arc, PairIter());
#else
    ArchiveCatalog cat;
    for (PairIter i(*arc); i != PairIter(); ++i)
        cat.push_back(*i);
#endif

    // the names of two entries to read
    const wxChar *name = _T("text/small");
    const wxChar *name2 = _T("bin/bin1000");

    // open them
    typename ArchiveCatalog::iterator j;
    CPPUNIT_ASSERT((j = cat.find(name)) != cat.end());
    CPPUNIT_ASSERT(arc->OpenEntry(*j->second));
    CPPUNIT_ASSERT((j = cat.find(name2)) != cat.end());
    CPPUNIT_ASSERT(arc2->OpenEntry(*j->second));

    // get pointers to the expected data
    TestEntries::iterator k;
    CPPUNIT_ASSERT((k = m_testEntries.find(name)) != m_testEntries.end());
    TestEntry *entry = k->second;
    CPPUNIT_ASSERT((k = m_testEntries.find(name2)) != m_testEntries.end());
    TestEntry *entry2 = k->second;

    size_t count = 0, count2 = 0;
    size_t size = entry->GetSize(), size2 = entry2->GetSize();
    const char *data = entry->GetData(), *data2 = entry2->GetData();

    // read and check the two entries in parallel, character by character
    while (arc->IsOk() || arc2->IsOk()) {
        char ch = arc->GetC();
        if (arc->LastRead() == 1) {
            CPPUNIT_ASSERT(count < size);
            CPPUNIT_ASSERT(ch == data[count++]);
        }
        char ch2 = arc2->GetC();
        if (arc2->LastRead() == 1) {
            CPPUNIT_ASSERT(count2 < size2);
            CPPUNIT_ASSERT(ch2 == data2[count2++]);
        }
    }

    CPPUNIT_ASSERT(arc->Eof());
    CPPUNIT_ASSERT(arc2->Eof());
    CPPUNIT_ASSERT(count == size);
    CPPUNIT_ASSERT(count2 == size2);
}

// Nothing useful can be done with a generic notifier yet, so just test one
// can be set
//
template <class NotifierT, class EntryT>
class ArchiveNotifier : public NotifierT
{
public:
    void OnEntryUpdated(EntryT& WXUNUSED(entry)) { }
};

template <class Classes>
void ArchiveTestCase<Classes>::OnSetNotifier(EntryT& entry)
{
    static ArchiveNotifier<NotifierT, EntryT> notifier;
    entry.SetNotifier(notifier);
}


///////////////////////////////////////////////////////////////////////////////
// ArchiveTestCase<ZipClasses> could be used directly, but instead this
// derived class is used so that zip specific features can be tested.

class ZipTestCase : public ArchiveTestCase<ZipClasses>
{
public:
    ZipTestCase(const wxString& name,
                int id,
                int options,
                const wxString& archiver = wxEmptyString,
                const wxString& unarchiver = wxEmptyString)
    :
        ArchiveTestCase<ZipClasses>(name, id, new wxZipClassFactory,
                                    options, archiver, unarchiver),
        m_count(0)
    { }

protected:
    void OnCreateArchive(wxZipOutputStream& zip);
    
    void OnArchiveExtracted(wxZipInputStream& zip, int expectedTotal);
    
    void OnCreateEntry(wxZipOutputStream& zip,
                       TestEntry& testEntry,
                       wxZipEntry *entry);
    
    void OnEntryExtracted(wxZipEntry& entry,
                          const TestEntry& testEntry,
                          wxZipInputStream *arc);

    void OnSetNotifier(EntryT& entry);

    int m_count;
    wxString m_comment;
};

void ZipTestCase::OnCreateArchive(wxZipOutputStream& zip)
{
    m_comment << _T("Comment for test ") << m_id;
    zip.SetComment(m_comment);
}

void ZipTestCase::OnArchiveExtracted(wxZipInputStream& zip, int expectedTotal)
{
    CPPUNIT_ASSERT(zip.GetComment() == m_comment);
    CPPUNIT_ASSERT(zip.GetTotalEntries() == expectedTotal);
}

void ZipTestCase::OnCreateEntry(wxZipOutputStream& zip,
                                TestEntry& testEntry,
                                wxZipEntry *entry)
{
    zip.SetLevel((m_id + m_count) % 10);

    if (entry) {
        switch ((m_id + m_count) % 5) {
            case 0:
            {
                wxString comment = _T("Comment for ") + entry->GetName();
                entry->SetComment(comment);
                // lowercase the expected result, and the notifier should do
                // the same for the zip entries when ModifyArchive() runs
                testEntry.SetComment(comment.Lower());
                break;
            }
            case 2:
                entry->SetMethod(wxZIP_METHOD_STORE);
                break;
            case 4:
                entry->SetMethod(wxZIP_METHOD_DEFLATE);
                break;
        }
        entry->SetIsText(testEntry.IsText());
    }

    m_count++;
}

void ZipTestCase::OnEntryExtracted(wxZipEntry& entry,
                                   const TestEntry& testEntry,
                                   wxZipInputStream *arc)
{
    // provide some context for the error message so that we know which
    // iteration of the loop we were on
    std::string error_entry((_T(" '") + entry.GetName() + _T("'")).mb_str());
    std::string error_context(" failed for entry" + error_entry);

    CPPUNIT_ASSERT_MESSAGE("GetComment" + error_context,
        entry.GetComment() == testEntry.GetComment());

    // for seekable streams, GetNextEntry() doesn't read the local header so
    // call OpenEntry() to do it
    if (arc && (m_options & PipeIn) == 0 && entry.IsDir())
        arc->OpenEntry(entry);

    CPPUNIT_ASSERT_MESSAGE("IsText" + error_context,
                           entry.IsText() == testEntry.IsText());

    CPPUNIT_ASSERT_MESSAGE("Extra/LocalExtra mismatch for entry" + error_entry,
        (entry.GetExtraLen() != 0 && entry.GetLocalExtraLen() != 0) ||
        (entry.GetExtraLen() == 0 && entry.GetLocalExtraLen() == 0));
}

// check the notifier mechanism by using it to fold the entry comments to
// lowercase
//
class ZipNotifier : public wxZipNotifier
{
public:
    void OnEntryUpdated(wxZipEntry& entry);
};

void ZipNotifier::OnEntryUpdated(wxZipEntry& entry)
{
    entry.SetComment(entry.GetComment().Lower());
}

void ZipTestCase::OnSetNotifier(EntryT& entry)
{
    static ZipNotifier notifier;
    entry.SetNotifier(notifier);
}


///////////////////////////////////////////////////////////////////////////////
// 'zip - -' produces local headers without the size field set. This is a
// case not covered by all the other tests, so this class tests it as a
// special case

class ZipPipeTestCase : public CppUnit::TestCase
{
public:
    ZipPipeTestCase(const wxString& name, int options) :
        CppUnit::TestCase(std::string(name.mb_str())), m_options(options) { }

protected:
    void runTest();
    int m_options;
};

void ZipPipeTestCase::runTest()
{
    TestOutputStream out(m_options);

    wxString testdata = _T("test data to pipe through zip");
    wxString cmd = _T("echo ") + testdata + _T(" | zip -q - -");

    {
        PFileInputStream in(cmd);
        if (in.Ok())
            out.Write(in);
    }

    TestInputStream in(out);
    wxZipInputStream zip(in);

    std::auto_ptr<wxZipEntry> entry(zip.GetNextEntry());
    CPPUNIT_ASSERT(entry.get() != NULL);

    if ((m_options & PipeIn) == 0)
        CPPUNIT_ASSERT(entry->GetSize() != wxInvalidOffset);

    char buf[64];
    size_t len = zip.Read(buf, sizeof(buf) - 1).LastRead();

    while (len > 0 && buf[len - 1] <= 32)
        --len;
    buf[len] = 0;

    CPPUNIT_ASSERT(zip.Eof());
    CPPUNIT_ASSERT(wxString(buf, *wxConvCurrent) == testdata);
}


///////////////////////////////////////////////////////////////////////////////
// The suite

class ArchiveTestSuite : public CppUnit::TestSuite
{
public:
    ArchiveTestSuite();
    static CppUnit::Test *suite()
        { return (new ArchiveTestSuite)->makeSuite(); }

private:
    int m_id;
    wxPathList m_path;

    ArchiveTestSuite *makeSuite();
    void AddCmd(wxArrayString& cmdlist, const wxString& cmd);
    bool IsInPath(const wxString& cmd);

    wxString Description(const wxString& type,
                         int options,
                         bool genericInterface = false,
                         const wxString& archiver = wxEmptyString,
                         const wxString& unarchiver = wxEmptyString);
};

ArchiveTestSuite::ArchiveTestSuite()
  : CppUnit::TestSuite("ArchiveTestSuite"),
    m_id(0)
{
    m_path.AddEnvList(_T("PATH"));
}

// add the command for an external archiver to the list, testing for it in
// the path first
//
void ArchiveTestSuite::AddCmd(wxArrayString& cmdlist, const wxString& cmd)
{
    if (cmdlist.empty())
        cmdlist.push_back(_T(""));
    if (IsInPath(cmd))
        cmdlist.push_back(cmd);
}

bool ArchiveTestSuite::IsInPath(const wxString& cmd)
{
    wxString c = cmd.BeforeFirst(_T(' '));
#ifdef __WXMSW__
    c += _T(".exe");
#endif
    return !m_path.FindValidPath(c).empty();
}

// make the test suite
//
ArchiveTestSuite *ArchiveTestSuite::makeSuite()
{
    typedef wxArrayString::iterator Iter;
    wxArrayString zippers;
    wxArrayString unzippers;

    AddCmd(zippers, _T("zip -qr %s *"));
    AddCmd(unzippers, _T("unzip -q %s"));

    for (int genInterface = 0; genInterface < 2; genInterface++)
        for (Iter i = unzippers.begin(); i != unzippers.end(); ++i)
            for (Iter j = zippers.begin(); j != zippers.end(); ++j)
                for (int options = 0; options <= AllOptions; options++)
                {
                    // unzip doesn't support piping in the zip
                    if ((options & PipeIn) && !i->empty())
                        continue;
#ifdef WXARC_NO_POPEN 
                    // if no popen then can use piped output of zip
                    if ((options & PipeOut) && !j->empty())
                        continue;
#endif
                    wxString name = Description(_T("wxZip"), options, 
                                                genInterface != 0, *j, *i);

                    if (genInterface)
                        addTest(new ArchiveTestCase<ArchiveClasses>(
                                    name, m_id,
                                    new wxZipClassFactory,
                                    options, *j, *i));
                    else
                        addTest(new ZipTestCase(name, m_id, options, *j, *i));

                    m_id++;
                }

#ifndef WXARC_NO_POPEN 
    // if have popen then can check the piped output of 'zip - -'
    if (IsInPath(_T("zip")))
        for (int options = 0; options <= PipeIn; options += PipeIn) {
            wxString name = Description(_T("ZipPipeTestCase"), options);
            addTest(new ZipPipeTestCase(name, options));
            m_id++;
        }
#endif

    return this;
}

// make a display string for the option bits
//
wxString ArchiveTestSuite::Description(const wxString& type,
                                       int options,
                                       bool genericInterface,
                                       const wxString& archiver,
                                       const wxString& unarchiver)
{
    wxString descr;
    descr << m_id << _T(" ");
    
    if (genericInterface)
        descr << _T("wxArchive (") << type << _T(")");
    else
        descr << type;

    if (!archiver.empty())
        descr << _T(" ") << archiver.BeforeFirst(_T(' '));
    if (!unarchiver.empty())
        descr << _T(" ") << unarchiver.BeforeFirst(_T(' '));
    
    wxString optstr;

    if ((options & PipeIn) != 0)
        optstr += _T("|PipeIn");
    if ((options & PipeOut) != 0)
        optstr += _T("|PipeOut");
    if ((options & Stub) != 0)
        optstr += _T("|Stub");
    if (!optstr.empty())
        optstr = _T(" (") + optstr.substr(1) + _T(")");

    descr << optstr;

    return descr;
}

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION(ArchiveTestSuite);

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(ArchiveTestSuite, "ArchiveTestSuite");

#endif // wxUSE_STREAMS
