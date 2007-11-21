///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/largefile.cpp
// Purpose:     Tests for large file support
// Author:      Mike Wetherell
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

//
// We're interested in what happens around offsets 0x7fffffff and 0xffffffff
// so the test writes a small chunk of test data just before and just after
// these offsets, then reads them back.
//
// The tests can be run with:
//
//  test --verbose largeFile
//
// On systems supporting sparse files they may also be registered in the
// Streams subsuite.
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/wfstream.h"

#ifdef __WXMSW__
#include "wx/msw/wrapwin.h"
#include <winioctl.h>
#endif

using std::auto_ptr;


///////////////////////////////////////////////////////////////////////////////
// Helpers

bool IsFAT(const wxString& path);
void MakeSparse(const wxString& path, int fd);


///////////////////////////////////////////////////////////////////////////////
// Base class for the test cases - common code

class LargeFileTest : public CppUnit::TestCase
{
public:
    LargeFileTest(std::string name) : CppUnit::TestCase(name) { }
    virtual ~LargeFileTest() { }

protected:
    void runTest();

    virtual wxInputStream *MakeInStream(const wxString& name) const = 0;
    virtual wxOutputStream *MakeOutStream(const wxString& name) const = 0;
    virtual bool HasLFS() const = 0;
};

void LargeFileTest::runTest()
{
    // self deleting temp file
    struct TmpFile {
        TmpFile() : m_name(wxFileName::CreateTempFileName(_T("wxlfs-"))) { }
        ~TmpFile() { if (!m_name.empty()) wxRemoveFile(m_name); }
        wxString m_name;
    } tmpfile;

    CPPUNIT_ASSERT(!tmpfile.m_name.empty());

    bool haveLFS = true;
    bool fourGigLimit = false;

    if (!HasLFS()) {
        haveLFS = false;
        wxString n(getName().c_str(), *wxConvCurrent);
        wxLogInfo(n + _T(": No large file support, testing up to 2GB only"));
    }
    else if (IsFAT(tmpfile.m_name)) {
        fourGigLimit = true;
        wxString n(getName().c_str(), *wxConvCurrent);
        wxLogInfo(n + _T(": FAT volumes are limited to 4GB files"));
    }

    // size of the test blocks
    const size_t size = 0x40;

    // the test blocks
    char upto2Gig[size];
    char past2Gig[size];
    char upto4Gig[size];
    char past4Gig[size];
    memset(upto2Gig, 'A', size);
    memset(past2Gig, 'B', size);
    memset(upto4Gig, 'X', size);
    memset(past4Gig, 'Y', size);

    wxFileOffset pos;

    // write a large file
    {
        auto_ptr<wxOutputStream> out(MakeOutStream(tmpfile.m_name));
        
        // write 'A's at [ 0x7fffffbf, 0x7fffffff [
        pos = 0x7fffffff - size;
        CPPUNIT_ASSERT(out->SeekO(pos) == pos);
        CPPUNIT_ASSERT(out->Write(upto2Gig, size).LastWrite() == size);
        pos += size;

        if (haveLFS) {
            // write 'B's at [ 0x7fffffff, 0x8000003f [
            CPPUNIT_ASSERT(out->Write(past2Gig, size).LastWrite() == size);
            pos += size;
            CPPUNIT_ASSERT(out->TellO() == pos);

            // write 'X's at [ 0xffffffbf, 0xffffffff [
            pos = 0xffffffff - size;
            CPPUNIT_ASSERT(out->SeekO(pos) == pos);
            CPPUNIT_ASSERT(out->Write(upto4Gig, size).LastWrite() == size);
            pos += size;

            if (!fourGigLimit) {
                // write 'Y's at [ 0xffffffff, 0x10000003f [
                CPPUNIT_ASSERT(out->Write(past4Gig, size).LastWrite() == size);
                pos += size;
            }
        }

        // check the file seems to be the right length
        CPPUNIT_ASSERT(out->TellO() == pos);
        CPPUNIT_ASSERT(out->GetLength() == pos);
    }

    // read the large file back
    {
        auto_ptr<wxInputStream> in(MakeInStream(tmpfile.m_name));
        char buf[size];

        if (haveLFS) {
            CPPUNIT_ASSERT(in->GetLength() == pos);
            pos = 0xffffffff;

            if (!fourGigLimit) {
                CPPUNIT_ASSERT(in->GetLength() > pos);

                // read back the 'Y's at [ 0xffffffff, 0x10000003f [
                CPPUNIT_ASSERT(in->SeekI(pos) == pos);
                CPPUNIT_ASSERT(in->Read(buf, size).LastRead() == size);
                CPPUNIT_ASSERT(memcmp(buf, past4Gig, size) == 0);

                CPPUNIT_ASSERT(in->TellI() == in->GetLength());
            }

            // read back the 'X's at [ 0xffffffbf, 0xffffffff [
            pos -= size;
            CPPUNIT_ASSERT(in->SeekI(pos) == pos);
            CPPUNIT_ASSERT(in->Read(buf, size).LastRead() == size);
            CPPUNIT_ASSERT(memcmp(buf, upto4Gig, size) == 0);
            pos += size;
            CPPUNIT_ASSERT(in->TellI() == pos);

            // read back the 'B's at [ 0x7fffffff, 0x8000003f [
            pos = 0x7fffffff;
            CPPUNIT_ASSERT(in->SeekI(pos) == pos);
            CPPUNIT_ASSERT(in->Read(buf, size).LastRead() == size);
            CPPUNIT_ASSERT(memcmp(buf, past2Gig, size) == 0);
        }
        else {
            CPPUNIT_ASSERT(in->GetLength() == 0x7fffffff);
            pos = 0x7fffffff;
        }

        // read back the 'A's at [ 0x7fffffbf, 0x7fffffff [
        pos -= size;
        CPPUNIT_ASSERT(in->SeekI(pos) == pos);
        CPPUNIT_ASSERT(in->Read(buf, size).LastRead() == size);
        CPPUNIT_ASSERT(memcmp(buf, upto2Gig, size) == 0);
        pos += size;
        CPPUNIT_ASSERT(in->TellI() == pos);
    }
}


///////////////////////////////////////////////////////////////////////////////
// wxFile test case

class LargeFileTest_wxFile : public LargeFileTest
{
public:
    LargeFileTest_wxFile() : LargeFileTest("wxFile streams") { }

protected:
    wxInputStream *MakeInStream(const wxString& name) const;
    wxOutputStream *MakeOutStream(const wxString& name) const;
    bool HasLFS() const { return (wxFileOffset)0xffffffff > 0; }
};

wxInputStream *LargeFileTest_wxFile::MakeInStream(const wxString& name) const
{
    auto_ptr<wxFileInputStream> in(new wxFileInputStream(name));
    CPPUNIT_ASSERT(in->Ok());
    return in.release();
}

wxOutputStream *LargeFileTest_wxFile::MakeOutStream(const wxString& name) const
{
    wxFile file(name, wxFile::write);
    CPPUNIT_ASSERT(file.IsOpened());
    int fd = file.fd();
    file.Detach();
    MakeSparse(name, fd);
    return new wxFileOutputStream(fd);
}


///////////////////////////////////////////////////////////////////////////////
// wxFFile test case

class LargeFileTest_wxFFile : public LargeFileTest
{
public:
    LargeFileTest_wxFFile() : LargeFileTest("wxFFile streams") { }

protected:
    wxInputStream *MakeInStream(const wxString& name) const;
    wxOutputStream *MakeOutStream(const wxString& name) const;
    bool HasLFS() const;
};

wxInputStream *LargeFileTest_wxFFile::MakeInStream(const wxString& name) const
{
    auto_ptr<wxFFileInputStream> in(new wxFFileInputStream(name));
    CPPUNIT_ASSERT(in->Ok());
    return in.release();
}

wxOutputStream *LargeFileTest_wxFFile::MakeOutStream(const wxString& name) const
{
    wxFFile file(name, _T("w"));
    CPPUNIT_ASSERT(file.IsOpened());
    FILE *fp = file.fp();
    file.Detach();
    MakeSparse(name, fileno(fp));
    return new wxFFileOutputStream(fp);
}

bool LargeFileTest_wxFFile::HasLFS() const
{
#ifdef HAVE_FSEEKO
    return (wxFileOffset)0xffffffff > 0;
#else
    return false;
#endif
}


///////////////////////////////////////////////////////////////////////////////
// The suite

class largeFile : public CppUnit::TestSuite
{
public:
    largeFile() : CppUnit::TestSuite("largeFile") { }

    static CppUnit::Test *suite();
};

CppUnit::Test *largeFile::suite()
{
    largeFile *suite = new largeFile;

    suite->addTest(new LargeFileTest_wxFile);
    suite->addTest(new LargeFileTest_wxFFile);

    return suite;
}


///////////////////////////////////////////////////////////////////////////////
// Implement the helpers
//
// Ideally these tests will be part of the default suite so that regressions
// are picked up. However this is only possible when sparse files are
// supported otherwise the tests require too much disk space.
//
// On unix, most filesystems support sparse files, though not all. So for now
// I'm not assuming sparse file support on unix. On Windows it's possible to
// test, and sparse files should be available on Win 5+ with NTFS.

#ifdef __WXMSW__

#ifndef FILE_SUPPORTS_SPARSE_FILES
#define FILE_SUPPORTS_SPARSE_FILES 0x00000040
#endif

#ifndef FSCTL_SET_SPARSE

#   ifndef FILE_SPECIAL_ACCESS 
#       define FILE_SPECIAL_ACCESS FILE_ANY_ACCESS
#   endif
#   define FSCTL_SET_SPARSE CTL_CODE(FILE_DEVICE_FILE_SYSTEM, 49, \
                                     METHOD_BUFFERED, FILE_SPECIAL_ACCESS)
#endif

static DWORD  volumeFlags;
static wxChar volumeType[64];
static bool   volumeInfoInit;

void GetVolumeInfo(const wxString& path)
{
    // extract the volume 'C:\' or '\\tooter\share\' from the path
    wxString vol;
    
    if (path.substr(1, 2) == _T(":\\")) {
        vol = path.substr(0, 3);
    } else {
        if (path.substr(0, 2) == _T("\\\\")) {
            size_t i = path.find(_T('\\'), 2);

            if (i != wxString::npos && i > 2) {
                size_t j = path.find(_T('\\'), ++i);

                if (j != i)
                    vol = path.substr(0, j) + _T("\\");
            }
        }
    }

    // NULL means the current volume
    const wxChar *pVol = vol.empty() ? NULL : vol.c_str();

    if (!::GetVolumeInformation(pVol, NULL, 0, NULL, NULL,
                                &volumeFlags, 
                                volumeType,
                                WXSIZEOF(volumeType)))
        wxLogSysError(_T("GetVolumeInformation() failed"));

    volumeInfoInit = true;
}

bool IsFAT(const wxString& path)
{
    if (!volumeInfoInit)
        GetVolumeInfo(path);
    return wxString(volumeType).Upper().find(_T("FAT")) != wxString::npos;
}

void MakeSparse(const wxString& path, int fd)
{
    DWORD cb;

    if (!volumeInfoInit)
        GetVolumeInfo(path);
   
    if ((volumeFlags & FILE_SUPPORTS_SPARSE_FILES) != 0)
        if (!::DeviceIoControl((HANDLE)_get_osfhandle(fd),
                               FSCTL_SET_SPARSE,
                               NULL, 0, NULL, 0, &cb, NULL))
            volumeFlags &= ~FILE_SUPPORTS_SPARSE_FILES;
}

CppUnit::Test* GetlargeFileSuite()
{
    if (!volumeInfoInit) {
        wxFile file;
        wxString path = wxFileName::CreateTempFileName(_T("wxlfs-"), &file);
        MakeSparse(path, file.fd());
        wxRemoveFile(path);
    }

    if ((volumeFlags & FILE_SUPPORTS_SPARSE_FILES) != 0)
        return largeFile::suite();
    else
        return NULL;
}

#else // __WXMSW__

bool IsFAT(const wxString& WXUNUSED(path)) { return false; }
void MakeSparse(const wxString& WXUNUSED(path), int WXUNUSED(fd)) { }

CppUnit::Test* GetlargeFileSuite() { return NULL; }

#endif // __WXMSW__

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(largeFile, "largeFile");
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(largeFile, "Streams.largeFile");
