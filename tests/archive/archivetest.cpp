///////////////////////////////////////////////////////////////////////////////
// Name:        tests/archive/archive.cpp
// Purpose:     Test the archive classes
// Author:      Mike Wetherell
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef WX_PRECOMP
#   include "wx/wx.h"
#endif

#if wxUSE_STREAMS && wxUSE_ARCHIVE_STREAMS

#include "archivetest.h"
#include "wx/dir.h"
#include <string>
#include <list>
#include <map>
#include <sys/stat.h>

using std::string;


///////////////////////////////////////////////////////////////////////////////
// A class to hold a test entry

TestEntry::TestEntry(const wxDateTime& dt, int len, const char *data)
  : m_dt(dt),
    m_len(len),
    m_isText(len > 0)
{
    m_data = new char[len];
    memcpy(m_data, data, len);

    for (int i = 0; i < len && m_isText; i++)
        m_isText = (signed char)m_data[i] > 0;
}


///////////////////////////////////////////////////////////////////////////////
// TestOutputStream and TestInputStream are memory streams which can be
// seekable or non-seekable.

const size_t STUB_SIZE = 2048;
const size_t INITIAL_SIZE = 0x18000;
const wxFileOffset SEEK_LIMIT = 0x100000;

TestOutputStream::TestOutputStream(int options)
  : m_options(options)
{
    Init();
}

void TestOutputStream::Init()
{
    m_data = nullptr;
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
        m_pos = (size_t)pos;
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

void TestOutputStream::GetData(char*& data, size_t& size)
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


///////////////////////////////////////////////////////////////////////////////
// TestOutputStream and TestInputStream are memory streams which can be
// seekable or non-seekable.

TestInputStream::TestInputStream(const TestInputStream& in)
  : wxInputStream(),
    m_options(in.m_options),
    m_pos(in.m_pos),
    m_size(in.m_size),
    m_eoftype(in.m_eoftype)
{
    m_data = new char[m_size];
    memcpy(m_data, in.m_data, m_size);
}

void TestInputStream::Rewind()
{
    if ((m_options & Stub) && (m_options & PipeIn))
        m_pos = STUB_SIZE * 2;
    else
        m_pos = 0;

    if (m_wbacksize) {
        free(m_wback);
        m_wback = nullptr;
        m_wbacksize = 0;
        m_wbackcur = 0;
    }

    Reset();
}

void TestInputStream::SetData(TestOutputStream& out)
{
    delete [] m_data;
    m_options = out.GetOptions();
    out.GetData(m_data, m_size);
    Rewind();
}

wxFileOffset TestInputStream::OnSysSeek(wxFileOffset pos, wxSeekMode mode)
{
    if ((m_options & PipeIn) == 0) {
        switch (mode) {
            case wxFromStart:   break;
            case wxFromCurrent: pos += m_pos; break;
            case wxFromEnd:     pos += m_size; break;
        }
        if (pos < 0 || pos > SEEK_LIMIT)
            return wxInvalidOffset;
        m_pos = (size_t)pos;
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

    size_t count;

    if (m_pos >= m_size)
        count = 0;
    else if (m_size - m_pos < size)
        count = m_size - m_pos;
    else
        count = size;

    if (count) {
        memcpy(buffer, m_data + m_pos, count);
        m_pos += count;
    }

    if (((m_eoftype & AtLast) != 0 && m_pos >= m_size) || count < size)
    {
        if ((m_eoftype & WithError) != 0)
            m_lasterror = wxSTREAM_READ_ERROR;
        else
            m_lasterror = wxSTREAM_EOF;
    }

    return count;
}


///////////////////////////////////////////////////////////////////////////////
// minimal non-intrusive reference counting pointer for testing the iterators

template <class T> class Ptr
{
public:
    explicit Ptr(T* p = nullptr) : m_p(p), m_count(new int) { *m_count = 1; }
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
    wxString tmp = wxFileName::CreateTempFileName(wxT("arctest-"));
    if (!tmp.empty()) {
        wxRemoveFile(tmp);
        m_original = wxGetCwd();
        CPPUNIT_ASSERT(wxMkdir(tmp, 0700));
        m_tmp = tmp;
        CPPUNIT_ASSERT(wxSetWorkingDirectory(tmp));
    }
}

TempDir::~TempDir()
{
    if (!m_tmp.empty()) {
        wxSetWorkingDirectory(m_original);
        RemoveDir(m_tmp);
    }
}

void TempDir::RemoveDir(wxString& path)
{
    wxCHECK_RET(!m_tmp.empty() && path.substr(0, m_tmp.length()) == m_tmp,
                wxT("remove '") + path + wxT("' fails safety check"));

    const wxChar *files[] = {
        wxT("text/empty"),
        wxT("text/small"),
        wxT("bin/bin1000"),
        wxT("bin/bin4095"),
        wxT("bin/bin4096"),
        wxT("bin/bin4097"),
        wxT("bin/bin16384"),
        wxT("zero/zero5"),
        wxT("zero/zero1024"),
        wxT("zero/zero32768"),
        wxT("zero/zero16385"),
        wxT("zero/newname"),
        wxT("newfile"),
    };

    const wxChar *dirs[] = {
        wxT("text/"), wxT("bin/"), wxT("zero/"), wxT("empty/")
    };

    wxString tmp = m_tmp + wxFileName::GetPathSeparator();
    size_t i;

    for (i = 0; i < WXSIZEOF(files); i++)
        wxRemoveFile(tmp + wxFileName(files[i], wxPATH_UNIX).GetFullPath());

    for (i = 0; i < WXSIZEOF(dirs); i++)
        wxRmdir(tmp + wxFileName(dirs[i], wxPATH_UNIX).GetFullPath());

    if (!wxRmdir(m_tmp))
    {
        wxLogSysError(wxT("can't remove temporary dir '%s'"), m_tmp);
    }
}


///////////////////////////////////////////////////////////////////////////////
// wxFFile streams for piping to/from an external program

#if defined __UNIX__ || defined __MINGW32__
#   define WXARC_popen popen
#   define WXARC_pclose pclose
#elif defined _MSC_VER
#   define WXARC_popen _popen
#   define WXARC_pclose _pclose
#else
#   define WXARC_NO_POPEN
#   define WXARC_popen(cmd, type) nullptr
#   define WXARC_pclose(fp)
#endif

#ifdef __WINDOWS__
#   define WXARC_b "b"
#else
#   define WXARC_b
#endif

PFileInputStream::PFileInputStream(const wxString& cmd)
  : wxFFileInputStream(WXARC_popen(cmd.mb_str(), "r" WXARC_b))
{
}

PFileInputStream::~PFileInputStream()
{
    WXARC_pclose(m_file->fp()); m_file->Detach();
}

PFileOutputStream::PFileOutputStream(const wxString& cmd)
: wxFFileOutputStream(WXARC_popen(cmd.mb_str(), "w" WXARC_b))
{
}

PFileOutputStream::~PFileOutputStream()
{
    WXARC_pclose(m_file->fp()); m_file->Detach();
}


///////////////////////////////////////////////////////////////////////////////
// The test case

template <class ClassFactoryT>
ArchiveTestCase<ClassFactoryT>::ArchiveTestCase(
    string name,
    ClassFactoryT *factory,
    int options,
    const wxString& archiver,
    const wxString& unarchiver)
  :
    CppUnit::TestCase(TestId::MakeId() + name),
    m_factory(factory),
    m_options(options),
    m_timeStamp(1, wxDateTime::Mar, 2004, 12, 0),
    m_id(TestId::GetId()),
    m_archiver(archiver),
    m_unarchiver(unarchiver)
{
    wxASSERT(m_factory.get() != nullptr);
}

template <class ClassFactoryT>
ArchiveTestCase<ClassFactoryT>::~ArchiveTestCase()
{
    TestEntries::iterator it;
    for (it = m_testEntries.begin(); it != m_testEntries.end(); ++it)
        delete it->second;
}

template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::runTest()
{
    TestOutputStream out(m_options);

    CreateTestData();

    if (m_archiver.empty())
        CreateArchive(out);
    else
    {
#ifndef __WXOSX_IPHONE__
        CreateArchive(out, m_archiver);
#else
        CPPUNIT_FAIL("using external archivers is not supported on iOS");
#endif
    }

    // check archive could be created
    CPPUNIT_ASSERT(out.GetLength() > 0);

    TestInputStream in(out, m_id % ((m_options & PipeIn) ? 4 : 3));

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
    {
#ifndef __WXOSX_IPHONE__
        ExtractArchive(in, m_unarchiver);
#else
        CPPUNIT_FAIL("using external archivers is not supported on iOS");
#endif
    }

    // check that all the test entries were found in the archive
    CPPUNIT_ASSERT(m_testEntries.empty());
}

template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::CreateTestData()
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

template <class ClassFactoryT>
TestEntry& ArchiveTestCase<ClassFactoryT>::Add(const char *name,
                                               const char *data,
                                               int len /*=-1*/)
{
    if (len == -1)
        len = strlen(data);
    TestEntry*& entry = m_testEntries[wxString(name, *wxConvCurrent)];
    wxASSERT(entry == nullptr);
    entry = new TestEntry(m_timeStamp, len, data);
    m_timeStamp += wxTimeSpan(0, 1, 30);
    return *entry;
}

template <class ClassFactoryT>
TestEntry& ArchiveTestCase<ClassFactoryT>::Add(const char *name,
                                               int len /*=0*/,
                                               int value /*=EOF*/)
{
    wxCharBuffer buf(len);
    for (int i = 0; i < len; i++)
        buf.data()[i] = (char)(value == EOF ? rand() : value);
    return Add(name, buf, len);
}

// Create an archive using the wx archive classes, write it to 'out'
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::CreateArchive(wxOutputStream& out)
{
    wxScopedPtr<OutputStreamT> arc(m_factory->NewStream(out));
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
        bool setIsDir = name.Last() == wxT('/') && (choices & 1);
        if (setIsDir)
            name.erase(name.length() - 1);

        // provide some context for the error message so that we know which
        // iteration of the loop we were on
        string error_entry((wxT(" '") + name + wxT("'")).mb_str());
        string error_context(" failed for entry" + error_entry);

        if ((choices & 2) || testEntry.IsText()) {
            // try PutNextEntry(EntryT *pEntry)
            wxScopedPtr<EntryT> entry(m_factory->NewEntry());
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

        if (it->first.Last() != wxT('/')) {
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
#ifndef __WXOSX_IPHONE__
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::CreateArchive(wxOutputStream& out,
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
            wxFileName::Mkdir(fn.GetPath(), 0777, wxPATH_MKDIR_FULL);
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
#ifdef __WINDOWS__
        if (fn.IsDir())
            entry.SetDateTime(wxDateTime());
        else
#endif
            fn.SetTimes(nullptr, &dt, nullptr);
    }

    if ((m_options & PipeOut) == 0) {
        wxFileName fn(tmpdir.GetName());
        fn.SetExt(wxT("arc"));
        wxString tmparc = fn.GetPath(wxPATH_GET_SEPARATOR) + fn.GetFullName();

        // call the archiver to create an archive file
        if ( system(wxString::Format(archiver, tmparc).mb_str()) == -1 )
        {
            wxLogError("Failed to run acrhiver command \"%s\"", archiver);
        }

        // then load the archive file
        {
            wxFFileInputStream in(tmparc);
            if (in.IsOk())
                out.Write(in);
        }

        wxRemoveFile(tmparc);
    }
    else {
        // for the non-seekable test, have the archiver output to "-"
        // and read the archive via a pipe
        PFileInputStream in(wxString::Format(archiver, wxT("-")));
        if (in.IsOk())
            out.Write(in);
    }
}
#endif

// Do a standard set of modification on an archive, delete an entry,
// rename an entry and add an entry
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::ModifyArchive(wxInputStream& in,
                                                   wxOutputStream& out)
{
    wxScopedPtr<InputStreamT> arcIn(m_factory->NewStream(in));
    wxScopedPtr<OutputStreamT> arcOut(m_factory->NewStream(out));
    EntryT *pEntry;

    const wxString deleteName = wxT("bin/bin1000");
    const wxString renameFrom = wxT("zero/zero1024");
    const wxString renameTo   = wxT("zero/newname");
    const wxString newName    = wxT("newfile");
    const char *newData       = "New file added as a test\n";

    arcOut->CopyArchiveMetaData(*arcIn);

    while ((pEntry = arcIn->GetNextEntry()) != nullptr) {
        wxScopedPtr<EntryT> entry(pEntry);
        OnSetNotifier(*entry);
        wxString name = entry->GetName(wxPATH_UNIX);

        // provide some context for the error message so that we know which
        // iteration of the loop we were on
        string error_entry((wxT(" '") + name + wxT("'")).mb_str());
        string error_context(" failed for entry" + error_entry);

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
    wxScopedPtr<EntryT> newentry(m_factory->NewEntry());
    newentry->SetName(newName);
    newentry->SetDateTime(testEntry.GetDateTime());
    newentry->SetSize(testEntry.GetLength());
    OnCreateEntry(*arcOut, testEntry, newentry.get());
    OnSetNotifier(*newentry);
    CPPUNIT_ASSERT(arcOut->PutNextEntry(newentry.release()));
    CPPUNIT_ASSERT(arcOut->Write(newData, strlen(newData)).IsOk());

    // should work with or without explicit Close
    if (m_id % 2)
        CPPUNIT_ASSERT(arcOut->Close());
}

// Extract an archive using the wx archive classes
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::ExtractArchive(wxInputStream& in)
{
    typedef Ptr<EntryT> EntryPtr;
    typedef std::list<EntryPtr> Entries;
    typedef typename Entries::iterator EntryIter;

    wxScopedPtr<InputStreamT> arc(m_factory->NewStream(in));
    int expectedTotal = m_testEntries.size();
    EntryPtr entry;
    Entries entries;

    if ((m_options & PipeIn) == 0)
        OnArchiveExtracted(*arc, expectedTotal);

    while (entry = EntryPtr(arc->GetNextEntry()), entry.get() != nullptr) {
        wxString name = entry->GetName(wxPATH_UNIX);

        // provide some context for the error message so that we know which
        // iteration of the loop we were on
        string error_entry((wxT(" '") + name + wxT("'")).mb_str());
        string error_context(" failed for entry" + error_entry);

        TestEntries::iterator it = m_testEntries.find(name);
        CPPUNIT_ASSERT_MESSAGE(
            "archive contains an entry that shouldn't be there" + error_entry,
            it != m_testEntries.end());

        const TestEntry& testEntry = *it->second;

#ifndef __WINDOWS__
        // On Windows some archivers compensate for Windows DST handling, but
        // other don't, so disable the test for now.
        wxDateTime dt = testEntry.GetDateTime();
        if (dt.IsValid())
            CPPUNIT_ASSERT_MESSAGE("timestamp check" + error_context,
                                   dt == entry->GetDateTime());
#endif

        // non-seekable entries are allowed to have GetSize == wxInvalidOffset
        // until the end of the entry's data has been read past
        CPPUNIT_ASSERT_MESSAGE("entry size check" + error_context,
            (testEntry.GetLength() == entry->GetSize() ||
            ((m_options & PipeIn) != 0 && entry->GetSize() == wxInvalidOffset)));
        CPPUNIT_ASSERT_MESSAGE(
            "arc->GetLength() == entry->GetSize()" + error_context,
            arc->GetLength() == entry->GetSize());

        if (name.Last() != wxT('/'))
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
#ifndef __WXOSX_IPHONE__
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::ExtractArchive(wxInputStream& in,
                                                    const wxString& unarchiver)
{
    // for an external unarchiver, unarchive to a tempdir
    TempDir tmpdir;

    if ((m_options & PipeIn) == 0) {
        wxFileName fn(tmpdir.GetName());
        fn.SetExt(wxT("arc"));
        wxString tmparc = fn.GetPath(wxPATH_GET_SEPARATOR) + fn.GetFullName();

        if (m_options & Stub)
            in.SeekI(STUB_SIZE * 2);

        // write the archive to a temporary file
        {
            wxFFileOutputStream out(tmparc);
            if (out.IsOk())
                out.Write(in);
        }

        // call unarchiver
        if ( system(wxString::Format(unarchiver, tmparc).mb_str()) == -1 )
        {
            wxLogError("Failed to run unarchiver command \"%s\"", unarchiver);
        }

        wxRemoveFile(tmparc);
    }
    else {
        // for the non-seekable test, have the archiver extract "-" and
        // feed it the archive via a pipe
        PFileOutputStream out(wxString::Format(unarchiver, wxT("-")));
        if (out.IsOk())
            out.Write(in);
    }

    wxString dir = tmpdir.GetName();
    VerifyDir(dir);
}
#endif

// Verifies the files produced by an external unarchiver are as expected
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::VerifyDir(wxString& path,
                                               size_t rootlen /*=0*/)
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
                name += wxT("/");

            // provide some context for the error message so that we know which
            // iteration of the loop we were on
            string error_entry((wxT(" '") + name + wxT("'")).mb_str());
            string error_context(" failed for entry" + error_entry);

            TestEntries::iterator it = m_testEntries.find(name);
            CPPUNIT_ASSERT_MESSAGE(
                "archive contains an entry that shouldn't be there"
                    + error_entry,
                it != m_testEntries.end());

            const TestEntry& testEntry = *it->second;

#if 0 //ndef __WINDOWS__
            CPPUNIT_ASSERT_MESSAGE("timestamp check" + error_context,
                                   testEntry.GetDateTime() ==
                                   wxFileName(path).GetModificationTime());
#endif
            if (!isDir) {
                wxFFileInputStream in(path);
                CPPUNIT_ASSERT_MESSAGE(
                    "entry not found in archive" + error_entry, in.IsOk());

                size_t size = (size_t)in.GetLength();
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
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::TestIterator(wxInputStream& in)
{
    typedef std::list<EntryT*> ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;

    wxScopedPtr<InputStreamT> arc(m_factory->NewStream(in));
    size_t count = 0;

    ArchiveCatalog cat((IterT)*arc, IterT());

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it) {
        wxScopedPtr<EntryT> entry(*it);
        count += m_testEntries.count(entry->GetName(wxPATH_UNIX));
    }

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());
    CPPUNIT_ASSERT(count == cat.size());
}

// test the pair iterators that can be used to load a std::map or wxHashMap
// these also give away ownership of entries
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::TestPairIterator(wxInputStream& in)
{
    typedef std::map<wxString, EntryT*> ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;

    wxScopedPtr<InputStreamT> arc(m_factory->NewStream(in));
    size_t count = 0;

    ArchiveCatalog cat((PairIterT)*arc, PairIterT());

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it) {
        wxScopedPtr<EntryT> entry(it->second);
        count += m_testEntries.count(entry->GetName(wxPATH_UNIX));
    }

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());
    CPPUNIT_ASSERT(count == cat.size());
}

// simple iterators using smart pointers, no need to worry about ownership
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::TestSmartIterator(wxInputStream& in)
{
    typedef std::list<Ptr<EntryT> > ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;
    typedef wxArchiveIterator<InputStreamT, Ptr<EntryT> > Iter;

    wxScopedPtr<InputStreamT> arc(m_factory->NewStream(in));

    ArchiveCatalog cat((Iter)*arc, Iter());

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it)
        CPPUNIT_ASSERT(m_testEntries.count((*it)->GetName(wxPATH_UNIX)));
}

// pair iterator using smart pointers
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::TestSmartPairIterator(wxInputStream& in)
{
    typedef std::map<wxString, Ptr<EntryT> > ArchiveCatalog;
    typedef typename ArchiveCatalog::iterator CatalogIter;
    typedef wxArchiveIterator<InputStreamT,
                std::pair<wxString, Ptr<EntryT> > > PairIter;

    wxScopedPtr<InputStreamT> arc(m_factory->NewStream(in));

    ArchiveCatalog cat((PairIter)*arc, PairIter());

    CPPUNIT_ASSERT(m_testEntries.size() == cat.size());

    for (CatalogIter it = cat.begin(); it != cat.end(); ++it)
        CPPUNIT_ASSERT(m_testEntries.count(it->second->GetName(wxPATH_UNIX)));
}

// try reading two entries at the same time
//
template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::ReadSimultaneous(TestInputStream& in)
{
    typedef std::map<wxString, Ptr<EntryT> > ArchiveCatalog;
    typedef wxArchiveIterator<InputStreamT,
                std::pair<wxString, Ptr<EntryT> > > PairIter;

    // create two archive input streams
    TestInputStream in2(in);
    wxScopedPtr<InputStreamT> arc(m_factory->NewStream(in));
    wxScopedPtr<InputStreamT> arc2(m_factory->NewStream(in2));

    // load the catalog
    ArchiveCatalog cat((PairIter)*arc, PairIter());

    // the names of two entries to read
    const wxChar *name = wxT("text/small");
    const wxChar *name2 = wxT("bin/bin1000");

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
    void OnEntryUpdated(EntryT& WXUNUSED(entry)) override { }
};

template <class ClassFactoryT>
void ArchiveTestCase<ClassFactoryT>::OnSetNotifier(EntryT& entry)
{
    static ArchiveNotifier<NotifierT, EntryT> notifier;
    entry.SetNotifier(notifier);
}


///////////////////////////////////////////////////////////////////////////////
// An additional case to check that reading corrupt archives doesn't crash

class CorruptionTestCase : public CppUnit::TestCase
{
public:
    CorruptionTestCase(std::string name,
                       wxArchiveClassFactory *factory,
                       int options)
      : CppUnit::TestCase(TestId::MakeId() + name),
        m_factory(factory),
        m_options(options)
    { }

    // the entry point for the test
    void runTest() override;

protected:
    void CreateArchive(wxOutputStream& out);
    void ExtractArchive(wxInputStream& in);

    wxScopedPtr<wxArchiveClassFactory> m_factory;  // factory to make classes
    int m_options;                              // test options
};

void CorruptionTestCase::runTest()
{
    TestOutputStream out(m_options);
    CreateArchive(out);
    TestInputStream in(out, 0);
    wxFileOffset len = in.GetLength();

    // try flipping one byte in the archive
    int pos;
    for (pos = 0; pos < len; pos++) {
        char n = in[pos];
        in[pos] = ~n;
        ExtractArchive(in);
        in.Rewind();
        in[pos] = n;
    }

    // try zeroing one byte in the archive
    for (pos = 0; pos < len; pos++) {
        char n = in[pos];
        in[pos] = 0;
        ExtractArchive(in);
        in.Rewind();
        in[pos] = n;
    }

    // try chopping the archive off
    for (int size = 1; size <= len; size++) {
        in.Chop(size);
        ExtractArchive(in);
        in.Rewind();
    }
}

void CorruptionTestCase::CreateArchive(wxOutputStream& out)
{
    wxScopedPtr<wxArchiveOutputStream> arc(m_factory->NewStream(out));

    arc->PutNextDirEntry(wxT("dir"));
    arc->PutNextEntry(wxT("file"));
    arc->Write(wxT("foo"), 3);
}

void CorruptionTestCase::ExtractArchive(wxInputStream& in)
{
    wxScopedPtr<wxArchiveInputStream> arc(m_factory->NewStream(in));
    wxScopedPtr<wxArchiveEntry> entry(arc->GetNextEntry());

    while (entry.get() != nullptr) {
        char buf[1024];

        while (arc->IsOk())
            arc->Read(buf, sizeof(buf));

        entry.reset(arc->GetNextEntry());
    }
}


///////////////////////////////////////////////////////////////////////////////
// Make the ids

int TestId::m_seed = 6219;

// static
string TestId::MakeId()
{
    m_seed = (m_seed * 171) % 30269;
    return string(wxString::Format(wxT("%-6d"), m_seed).mb_str());
}


///////////////////////////////////////////////////////////////////////////////
// Suite base

ArchiveTestSuite::ArchiveTestSuite(string name)
  : CppUnit::TestSuite("archive/" + name),
    m_name(name.c_str(), *wxConvCurrent)
{
    m_name = wxT("wx") + m_name.Left(1).Upper() + m_name.Mid(1).Lower();
    m_path.AddEnvList(wxT("PATH"));
    m_archivers.push_back(wxT(""));
    m_unarchivers.push_back(wxT(""));
}

// add the command for an external archiver to the list, testing for it in
// the path first
//
void ArchiveTestSuite::AddCmd(wxArrayString& cmdlist, const wxString& cmd)
{
    if (IsInPath(cmd))
        cmdlist.push_back(cmd);
}

bool ArchiveTestSuite::IsInPath(const wxString& cmd)
{
    wxString c = cmd.BeforeFirst(wxT(' '));
#ifdef __WINDOWS__
    c += wxT(".exe");
#endif
    return !m_path.FindValidPath(c).empty();
}

// run all the tests in the test suite
//
void ArchiveTestSuite::DoRunTest()
{
    typedef wxArrayString::iterator Iter;

    for (int generic = 0; generic < 2; generic++)
        for (Iter i = m_unarchivers.begin(); i != m_unarchivers.end(); ++i)
            for (Iter j = m_archivers.begin(); j != m_archivers.end(); ++j)
                for (int options = 0; options <= AllOptions; options++)
                {
#ifdef WXARC_NO_POPEN
                    // if no popen then can't pipe in/out of archiver
                    if ((options & PipeIn) && !i->empty())
                        continue;
                    if ((options & PipeOut) && !j->empty())
                        continue;
#endif
                    string descr = Description(m_name, options,
                                               generic != 0, *j, *i);

                    CppUnit::Test *test = makeTest(descr, options,
                                                   generic != 0, *j, *i);

                    if (test)
                    {
                        test->runTest();
                        delete test;
                    }
                }

    for (int options = 0; options <= PipeIn; options += PipeIn)
    {
        wxObject *pObj = wxCreateDynamicObject(m_name + wxT("ClassFactory"));
        wxArchiveClassFactory *factory;
        factory = wxDynamicCast(pObj, wxArchiveClassFactory);

        if (factory) {
            string descr(m_name.mb_str());
            descr = "CorruptionTestCase (" + descr + ")";

            if (options)
                descr += " (PipeIn)";

            CorruptionTestCase test(descr, factory, options);
            test.runTest();
        }
    }
}

CppUnit::Test *ArchiveTestSuite::makeTest(
    string WXUNUSED(descr),
    int WXUNUSED(options),
    bool WXUNUSED(genericInterface),
    const wxString& WXUNUSED(archiver),
    const wxString& WXUNUSED(unarchiver))
{
    return nullptr;
}

// make a display string for the option bits
//
string ArchiveTestSuite::Description(const wxString& type,
                                     int options,
                                     bool genericInterface,
                                     const wxString& archiver,
                                     const wxString& unarchiver)
{
    wxString descr;

    if (genericInterface)
        descr << wxT("wxArchive (") << type << wxT(")");
    else
        descr << type;

    if (!archiver.empty()) {
        const wxChar *fn = (options & PipeOut) != 0 ? wxT("-") : wxT("file");
        const wxString cmd = archiver.Contains("%s")
                             ? wxString::Format(archiver, fn)
                             : archiver;
        descr << wxT(" (") << cmd << wxT(")");
    }
    if (!unarchiver.empty()) {
        const wxChar *fn = (options & PipeIn) != 0 ? wxT("-") : wxT("file");
        const wxString cmd = unarchiver.Contains("%s")
                             ? wxString::Format(unarchiver, fn)
                             : unarchiver;
        descr << wxT(" (") << cmd << wxT(")");
    }

    wxString optstr;

    if ((options & PipeIn) != 0)
        optstr += wxT("|PipeIn");
    if ((options & PipeOut) != 0)
        optstr += wxT("|PipeOut");
    if ((options & Stub) != 0)
        optstr += wxT("|Stub");
    if (!optstr.empty())
        optstr = wxT(" (") + optstr.substr(1) + wxT(")");

    descr << optstr;

    return string(descr.mb_str());
}


///////////////////////////////////////////////////////////////////////////////
// Instantiations

template class ArchiveTestCase<wxArchiveClassFactory>;

#if wxUSE_ZIPSTREAM
#include "wx/zipstrm.h"
template class ArchiveTestCase<wxZipClassFactory>;
#endif

#if wxUSE_TARSTREAM
#include "wx/tarstrm.h"
template class ArchiveTestCase<wxTarClassFactory>;
#endif

#endif // wxUSE_STREAMS && wxUSE_ARCHIVE_STREAMS
