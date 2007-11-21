///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/filestream.cpp
// Purpose:     Test wxFileInputStream/wxFileOutputStream
// Author:      Hans Van Leemputten
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Hans Van Leemputten
// Licence:     wxWidgets licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
// and "wx/cppunit.h"
#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wfstream.h"

#include "bstream.h"

#define DATABUFFER_SIZE     1024

static const wxString FILENAME_FILEINSTREAM = _T("fileinstream.test");
static const wxString FILENAME_FILEOUTSTREAM = _T("fileoutstream.test");

///////////////////////////////////////////////////////////////////////////////
// The test case
//
// Try to fully test wxFileInputStream and wxFileOutputStream

class fileStream : public BaseStreamTestCase<wxFileInputStream, wxFileOutputStream>
{
public:
    fileStream();
    virtual ~fileStream();

    CPPUNIT_TEST_SUITE(fileStream);
        // Base class stream tests the fileStream supports.
        CPPUNIT_TEST(Input_GetSize);
        CPPUNIT_TEST(Input_GetC);
        CPPUNIT_TEST(Input_Read);
        CPPUNIT_TEST(Input_Eof);
        CPPUNIT_TEST(Input_LastRead);
        CPPUNIT_TEST(Input_SeekI);
        CPPUNIT_TEST(Input_TellI);
        CPPUNIT_TEST(Input_Peek);
        CPPUNIT_TEST(Input_Ungetch);

        CPPUNIT_TEST(Output_PutC);
        CPPUNIT_TEST(Output_Write);
        CPPUNIT_TEST(Output_LastWrite);
        CPPUNIT_TEST(Output_SeekO);
        CPPUNIT_TEST(Output_TellO);

        // Other test specific for File stream test case.
    CPPUNIT_TEST_SUITE_END();

protected:
    // Add own test here.

private:
    // Implement base class functions.
    virtual wxFileInputStream  *DoCreateInStream();
    virtual wxFileOutputStream *DoCreateOutStream();
    virtual void DoDeleteOutStream();

private:
    wxString GetInFileName() const;
};

fileStream::fileStream()
{
    m_bSeekInvalidBeyondEnd = false;
}

fileStream::~fileStream()
{
    // Remove the temp test file...
    ::wxRemoveFile(FILENAME_FILEINSTREAM);
    ::wxRemoveFile(FILENAME_FILEOUTSTREAM);
}

wxFileInputStream *fileStream::DoCreateInStream()
{
    wxFileInputStream *pFileInStream = new wxFileInputStream(GetInFileName());
    CPPUNIT_ASSERT(pFileInStream->IsOk());
    return pFileInStream;
}
wxFileOutputStream *fileStream::DoCreateOutStream()
{
    wxFileOutputStream *pFileOutStream = new wxFileOutputStream(FILENAME_FILEOUTSTREAM);
    CPPUNIT_ASSERT(pFileOutStream->IsOk());
    return pFileOutStream;
}

void fileStream::DoDeleteOutStream()
{
    ::wxRemoveFile(FILENAME_FILEOUTSTREAM);
}

wxString fileStream::GetInFileName() const
{
    static bool bFileCreated = false;
    if (!bFileCreated)
    {
        // Create the file only once
        bFileCreated = true;

        // Make sure we have a input file...
        char buf[DATABUFFER_SIZE];
        wxFileOutputStream out(FILENAME_FILEINSTREAM);

        // Init the data buffer.
        for (size_t i = 0; i < DATABUFFER_SIZE; i++)
            buf[i] = (i % 0xFF);

        // Save the data
        out.Write(buf, DATABUFFER_SIZE);
    }

    return FILENAME_FILEINSTREAM;
}

// Register the stream sub suite, by using some stream helper macro.
// Note: Don't forget to connect it to the base suite (See: bstream.cpp => StreamCase::suite())
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(fileStream)
