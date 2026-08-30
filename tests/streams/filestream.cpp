///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/filestream.cpp
// Purpose:     Test wxFileInputStream/wxFileOutputStream
// Author:      Hans Van Leemputten
// Copyright:   (c) 2004 Hans Van Leemputten
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "testprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wfstream.h"

#include "bstream.h"

#define DATABUFFER_SIZE     1024

static const wxString FILENAME_FILEINSTREAM = wxT("fileinstream.test");
static const wxString FILENAME_FILEOUTSTREAM = wxT("fileoutstream.test");

///////////////////////////////////////////////////////////////////////////////
// The test case
//
// Try to fully test wxFileInputStream and wxFileOutputStream

class fileStream : public BaseStreamTestCase<wxFileInputStream, wxFileOutputStream>
{
public:
    fileStream();

private:
    // Implement base class functions.
    virtual wxFileInputStream  *DoCreateInStream() override;
    virtual wxFileOutputStream *DoCreateOutStream() override;
    virtual void DoDeleteOutStream() override;

private:
    wxString GetInFileName() const;
};

fileStream::fileStream()
{
    m_bSeekInvalidBeyondEnd = false;
}

wxFileInputStream *fileStream::DoCreateInStream()
{
    wxFileInputStream *pFileInStream = new wxFileInputStream(GetInFileName());
    REQUIRE(pFileInStream->IsOk());
    return pFileInStream;
}
wxFileOutputStream *fileStream::DoCreateOutStream()
{
    wxFileOutputStream *pFileOutStream = new wxFileOutputStream(FILENAME_FILEOUTSTREAM);
    REQUIRE(pFileOutStream->IsOk());
    return pFileOutStream;
}

void fileStream::DoDeleteOutStream()
{
    ::wxRemoveFile(FILENAME_FILEOUTSTREAM);
}

wxString fileStream::GetInFileName() const
{
    class AutoRemoveFile
    {
    public:
        AutoRemoveFile()
        {
            m_created = false;
        }

        ~AutoRemoveFile()
        {
            if ( m_created )
                wxRemoveFile(FILENAME_FILEINSTREAM);
        }

        bool ShouldCreate()
        {
            if ( m_created )
                return false;

            m_created = true;

            return true;
        }

    private:
        bool m_created;
    };

    static AutoRemoveFile autoFile;
    if ( autoFile.ShouldCreate() )
    {
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

// Base class stream tests the fileStream supports.
WX_STREAM_TEST_CASE(fileStream, Input_GetSize)
WX_STREAM_TEST_CASE(fileStream, Input_GetC)
WX_STREAM_TEST_CASE(fileStream, Input_Read)
WX_STREAM_TEST_CASE(fileStream, Input_Eof)
WX_STREAM_TEST_CASE(fileStream, Input_LastRead)
WX_STREAM_TEST_CASE(fileStream, Input_CanRead)
WX_STREAM_TEST_CASE(fileStream, Input_SeekI)
WX_STREAM_TEST_CASE(fileStream, Input_TellI)
WX_STREAM_TEST_CASE(fileStream, Input_Peek)
WX_STREAM_TEST_CASE(fileStream, Input_Ungetch)

WX_STREAM_TEST_CASE(fileStream, Output_PutC)
WX_STREAM_TEST_CASE(fileStream, Output_Write)
WX_STREAM_TEST_CASE(fileStream, Output_LastWrite)
WX_STREAM_TEST_CASE(fileStream, Output_SeekO)
WX_STREAM_TEST_CASE(fileStream, Output_TellO)
