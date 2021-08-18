///////////////////////////////////////////////////////////////////////////////
// Name:        tests/archive/archivetest.h
// Purpose:     Test the archive classes
// Author:      Mike Wetherell
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef WX_ARCHIVETEST_INCLUDED
#define WX_ARCHIVETEST_INCLUDED 1

#define WX_TEST_ARCHIVE_ITERATOR

#include "wx/archive.h"
#include "wx/scopedptr.h"
#include "wx/wfstream.h"

#include <map>
#include <memory>

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
// TestOutputStream and TestInputStream are memory streams which can be
// seekable or non-seekable.

class TestOutputStream : public wxOutputStream
{
public:
    TestOutputStream(int options);

    ~TestOutputStream() { delete [] m_data; }

    int GetOptions() const { return m_options; }
    wxFileOffset GetLength() const wxOVERRIDE { return m_size; }
    bool IsSeekable() const wxOVERRIDE { return (m_options & PipeOut) == 0; }

    // gives away the data, this stream is then empty, and can be reused
    void GetData(char*& data, size_t& size);

private:
    void Init();

    wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) wxOVERRIDE;
    wxFileOffset OnSysTell() const wxOVERRIDE;
    size_t OnSysWrite(const void *buffer, size_t size) wxOVERRIDE;

    int m_options;
    size_t m_pos;
    size_t m_capacity;
    size_t m_size;
    char *m_data;
};

class TestInputStream : public wxInputStream
{
public:
    // various streams have implemented eof differently, so check the archive
    // stream works with all the possibilities (bit flags that can be ORed)
    enum EofTypes {
        AtLast    = 0x01,   // eof before an attempt to read past the last byte
        WithError = 0x02    // give an error instead of eof
    };

    // ctor takes the data from the output stream, which is then empty
    TestInputStream(TestOutputStream& out, int eoftype)
        : m_data(NULL), m_eoftype(eoftype) { SetData(out); }
    // this ctor 'dups'
    TestInputStream(const TestInputStream& in);
    ~TestInputStream() { delete [] m_data; }

    void Rewind();
    wxFileOffset GetLength() const wxOVERRIDE { return m_size; }
    bool IsSeekable() const wxOVERRIDE { return (m_options & PipeIn) == 0; }
    void SetData(TestOutputStream& out);

    void Chop(size_t size) { m_size = size; }
    char& operator [](size_t pos) { return m_data[pos]; }

private:
    wxFileOffset OnSysSeek(wxFileOffset pos, wxSeekMode mode) wxOVERRIDE;
    wxFileOffset OnSysTell() const wxOVERRIDE;
    size_t OnSysRead(void *buffer, size_t size) wxOVERRIDE;

    int m_options;
    size_t m_pos;
    size_t m_size;
    char *m_data;
    int m_eoftype;
};


///////////////////////////////////////////////////////////////////////////////
// wxFFile streams for piping to/from an external program

class PFileInputStream : public wxFFileInputStream
{
public:
    PFileInputStream(const wxString& cmd);
    ~PFileInputStream();
};

class PFileOutputStream : public wxFFileOutputStream
{
public:
    PFileOutputStream(const wxString& cmd);
    ~PFileOutputStream();
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
    char *m_data;
    wxString m_comment;
    bool m_isText;
};


///////////////////////////////////////////////////////////////////////////////
// The test case

template <class ClassFactoryT>
class ArchiveTestCase : public CppUnit::TestCase
{
public:
    ArchiveTestCase(std::string name,
                    ClassFactoryT *factory,
                    int options,
                    const wxString& archiver = wxEmptyString,
                    const wxString& unarchiver = wxEmptyString);

    ~ArchiveTestCase();

protected:
    // the classes to test
    typedef typename ClassFactoryT::entry_type     EntryT;
    typedef typename ClassFactoryT::instream_type  InputStreamT;
    typedef typename ClassFactoryT::outstream_type OutputStreamT;
    typedef typename ClassFactoryT::notifier_type  NotifierT;
    typedef typename ClassFactoryT::iter_type      IterT;
    typedef typename ClassFactoryT::pairiter_type  PairIterT;

    // the entry point for the test
    void runTest() wxOVERRIDE;

    // create the test data
    void CreateTestData();
    TestEntry& Add(const char *name, const char *data, int len = -1);
    TestEntry& Add(const char *name, int len = 0, int value = EOF);

    // 'archive up' the test data
    void CreateArchive(wxOutputStream& out);
#ifndef __WXOSX_IPHONE__
    void CreateArchive(wxOutputStream& out, const wxString& archiver);
#endif

    // perform various modifications on the archive
    void ModifyArchive(wxInputStream& in, wxOutputStream& out);

    // extract the archive and verify its contents
    void ExtractArchive(wxInputStream& in);
#ifndef __WXOSX_IPHONE__
    void ExtractArchive(wxInputStream& in, const wxString& unarchiver);
#endif
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
    wxScopedPtr<ClassFactoryT> m_factory;   // factory to make classes
    int m_options;                          // test options
    wxDateTime m_timeStamp;                 // timestamp to give test entries
    int m_id;                               // select between the possibilites
    wxString m_archiver;                    // external archiver
    wxString m_unarchiver;                  // external unarchiver
};


///////////////////////////////////////////////////////////////////////////////
// Make ids

class TestId
{
public:
    // make a new id and return it as a string
    static std::string MakeId();
    // get the current id
    static int GetId() { return m_seed; }
private:
    // seed for generating the ids
    static int m_seed;
};


///////////////////////////////////////////////////////////////////////////////
// Base class for the archive test suites

class ArchiveTestSuite : public CppUnit::TestSuite
{
public:
    ArchiveTestSuite(std::string name);

protected:
    void DoRunTest();

    virtual CppUnit::Test *makeTest(std::string descr,
                                    int options,
                                    bool genericInterface,
                                    const wxString& archiver,
                                    const wxString& unarchiver);

    void AddArchiver(const wxString& cmd) { AddCmd(m_archivers, cmd); }
    void AddUnArchiver(const wxString &cmd) { AddCmd(m_unarchivers, cmd); }
    bool IsInPath(const wxString& cmd);

    std::string Description(const wxString& type,
                            int options,
                            bool genericInterface = false,
                            const wxString& archiver = wxEmptyString,
                            const wxString& unarchiver = wxEmptyString);

private:
    wxString m_name;
    wxPathList m_path;
    wxArrayString m_archivers;
    wxArrayString m_unarchivers;

    void AddCmd(wxArrayString& cmdlist, const wxString& cmd);
};

#endif
