///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/largefile.cpp
// Purpose:     Tests for large file support
// Author:      Mike Wetherell
// Copyright:   (c) 2004 Mike Wetherell
// Licence:     wxWindows licence
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
// On systems supporting sparse files they will also be registered in the
// Streams subsuite so that they run by default.
//

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/filename.h"
#include "wx/scopedptr.h"
#include "wx/wfstream.h"

#ifdef __WINDOWS__
    #include "wx/msw/wrapwin.h"
    #ifdef __VISUALC__
        // 'nonstandard extension used : nameless struct/union' occurs inside
        // winioctl.h
        #pragma warning(disable:4201)
    #endif
    #include <winioctl.h>
    #ifdef __VISUALC__
        #pragma warning(default:4201)
    #endif
#endif

#ifdef __VISUALC__
    #define fileno _fileno
#endif


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
    void runTest() override;

    virtual wxInputStream *MakeInStream(const wxString& name) const = 0;
    virtual wxOutputStream *MakeOutStream(const wxString& name) const = 0;
    virtual bool HasLFS() const = 0;
};

void LargeFileTest::runTest()
{
    // self deleting temp file
    struct TmpFile {
        TmpFile() : m_name(wxFileName::CreateTempFileName(wxT("wxlfs-"))) { }
        ~TmpFile() { if (!m_name.empty()) wxRemoveFile(m_name); }
        wxString m_name;
    } tmpfile;

    CPPUNIT_ASSERT(!tmpfile.m_name.empty());

    bool haveLFS = true;
    bool fourGigLimit = false;

    if (!HasLFS()) {
        haveLFS = false;
        wxString n(getName().c_str(), *wxConvCurrent);
        wxLogInfo(n + wxT(": No large file support, testing up to 2GB only"));
    }
    else if (IsFAT(tmpfile.m_name)) {
        fourGigLimit = true;
        wxString n(getName().c_str(), *wxConvCurrent);
        wxLogInfo(n + wxT(": FAT volumes are limited to 4GB files"));
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
        wxScopedPtr<wxOutputStream> out(MakeOutStream(tmpfile.m_name));

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
        wxScopedPtr<wxInputStream> in(MakeInStream(tmpfile.m_name));
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
    wxInputStream *MakeInStream(const wxString& name) const override;
    wxOutputStream *MakeOutStream(const wxString& name) const override;
    bool HasLFS() const override { return (wxFileOffset)0xffffffff > 0; }
};

wxInputStream *LargeFileTest_wxFile::MakeInStream(const wxString& name) const
{
    wxScopedPtr<wxFileInputStream> in(new wxFileInputStream(name));
    CPPUNIT_ASSERT(in->IsOk());
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
    wxInputStream *MakeInStream(const wxString& name) const override;
    wxOutputStream *MakeOutStream(const wxString& name) const override;
    bool HasLFS() const override;
};

wxInputStream *LargeFileTest_wxFFile::MakeInStream(const wxString& name) const
{
    wxScopedPtr<wxFFileInputStream> in(new wxFFileInputStream(name));
    CPPUNIT_ASSERT(in->IsOk());
    return in.release();
}

wxOutputStream *LargeFileTest_wxFFile::MakeOutStream(const wxString& name) const
{
    wxFFile file(name, wxT("w"));
    CPPUNIT_ASSERT(file.IsOpened());
    FILE *fp = file.fp();
    file.Detach();
    MakeSparse(name, fileno(fp));
    return new wxFFileOutputStream(fp);
}

bool LargeFileTest_wxFFile::HasLFS() const
{
#ifdef wxHAS_LARGE_FFILES
    return true;
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

#ifdef __WINDOWS__

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

    if (path.substr(1, 2) == wxT(":\\")) {
        vol = path.substr(0, 3);
    } else {
        if (path.substr(0, 2) == wxT("\\\\")) {
            size_t i = path.find(wxT('\\'), 2);

            if (i != wxString::npos && i > 2) {
                size_t j = path.find(wxT('\\'), ++i);

                if (j != i)
                    vol = path.substr(0, j) + wxT("\\");
            }
        }
    }

    // nullptr means the current volume
    const wxChar *pVol = vol.empty() ? (const wxChar *)nullptr
                                     : vol.c_str();

    if (!::GetVolumeInformation(pVol, nullptr, 0, nullptr, nullptr,
                                &volumeFlags,
                                volumeType,
                                WXSIZEOF(volumeType)))
    {
        wxLogSysError(wxT("GetVolumeInformation() failed"));
    }

    volumeInfoInit = true;
}

bool IsFAT(const wxString& path)
{
    if (!volumeInfoInit)
        GetVolumeInfo(path);
    return wxString(volumeType).Upper().find(wxT("FAT")) != wxString::npos;
}

void MakeSparse(const wxString& path, int fd)
{
    DWORD cb;

    if (!volumeInfoInit)
        GetVolumeInfo(path);

    if ((volumeFlags & FILE_SUPPORTS_SPARSE_FILES) != 0)
        if (!::DeviceIoControl((HANDLE)_get_osfhandle(fd),
                               FSCTL_SET_SPARSE,
                               nullptr, 0, nullptr, 0, &cb, nullptr))
            volumeFlags &= ~FILE_SUPPORTS_SPARSE_FILES;
}

// return the suite if sparse files are supported, otherwise return nullptr
//
CppUnit::Test* GetlargeFileSuite()
{
    if (!volumeInfoInit) {
        wxString path;
        {
            wxFile file;
            path = wxFileName::CreateTempFileName(wxT("wxlfs-"), &file);
            MakeSparse(path, file.fd());
        }
        wxRemoveFile(path);
    }

    if ((volumeFlags & FILE_SUPPORTS_SPARSE_FILES) != 0)
        return largeFile::suite();
    else
        return nullptr;
}

#else // __WINDOWS__

bool IsFAT(const wxString& WXUNUSED(path)) { return false; }
void MakeSparse(const wxString& WXUNUSED(path), int WXUNUSED(fd)) { }

// return the suite if sparse files are supported, otherwise return nullptr
//
CppUnit::Test* GetlargeFileSuite()
{
    wxString path;
    struct stat st1, st2;
    memset(&st1, 0, sizeof(st1));
    memset(&st2, 0, sizeof(st2));

    {
        wxFile file;
        path = wxFileName::CreateTempFileName(wxT("wxlfs-"), &file);

        fstat(file.fd(), &st1);
        file.Seek(st1.st_blksize);
        file.Write("x", 1);
        fstat(file.fd(), &st1);

        file.Seek(0);
        file.Write("x", 1);
        fstat(file.fd(), &st2);
    }

    wxRemoveFile(path);

    if (st1.st_blocks != st2.st_blocks)
        return largeFile::suite();
    else
        return nullptr;
}

#endif // __WINDOWS__

CPPUNIT_TEST_SUITE_NAMED_REGISTRATION(largeFile, "Streams.largeFile");
