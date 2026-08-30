///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/ffilestream.cpp
// Purpose:     Test wxFFileInputStream/wxFFileOutputStream
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

static const wxString FILENAME_FFILEINSTREAM = wxT("ffileinstream.test");
static const wxString FILENAME_FFILEOUTSTREAM = wxT("ffileoutstream.test");

///////////////////////////////////////////////////////////////////////////////
// The test case
//
// Try to fully test wxFFileInputStream and wxFFileOutputStream

class ffileStream : public BaseStreamTestCase<wxFFileInputStream, wxFFileOutputStream>
{
public:
    ffileStream();

private:
    // Implement base class functions.
    virtual wxFFileInputStream  *DoCreateInStream() override;
    virtual wxFFileOutputStream *DoCreateOutStream() override;
    virtual void DoDeleteOutStream() override;

private:
    wxString GetInFileName() const;
};

ffileStream::ffileStream()
{
    m_bSeekInvalidBeyondEnd = false;
    m_bEofAtLastRead = false;
}

wxFFileInputStream *ffileStream::DoCreateInStream()
{
    wxFFileInputStream *pFileInStream = new wxFFileInputStream(GetInFileName());
    REQUIRE(pFileInStream->IsOk());
    return pFileInStream;
}
wxFFileOutputStream *ffileStream::DoCreateOutStream()
{
    wxFFileOutputStream *pFileOutStream = new wxFFileOutputStream(FILENAME_FFILEOUTSTREAM);
    REQUIRE(pFileOutStream->IsOk());
    return pFileOutStream;
}

void ffileStream::DoDeleteOutStream()
{
    ::wxRemoveFile(FILENAME_FFILEOUTSTREAM);
}

wxString ffileStream::GetInFileName() const
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
                wxRemoveFile(FILENAME_FFILEINSTREAM);
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
        wxFFileOutputStream out(FILENAME_FFILEINSTREAM);

        // Init the data buffer.
        for (size_t i = 0; i < DATABUFFER_SIZE; i++)
            buf[i] = (i % 0xFF);

        // Save the data
        out.Write(buf, DATABUFFER_SIZE);
    }

    return FILENAME_FFILEINSTREAM;
}

// Base class stream tests the ffileStream supports.
WX_STREAM_TEST_CASE(ffileStream, Input_GetSize)
WX_STREAM_TEST_CASE(ffileStream, Input_GetC)
WX_STREAM_TEST_CASE(ffileStream, Input_Read)
WX_STREAM_TEST_CASE(ffileStream, Input_Eof)
WX_STREAM_TEST_CASE(ffileStream, Input_LastRead)
WX_STREAM_TEST_CASE(ffileStream, Input_CanRead)
WX_STREAM_TEST_CASE(ffileStream, Input_SeekI)
WX_STREAM_TEST_CASE(ffileStream, Input_TellI)
WX_STREAM_TEST_CASE(ffileStream, Input_Peek)
WX_STREAM_TEST_CASE(ffileStream, Input_Ungetch)

WX_STREAM_TEST_CASE(ffileStream, Output_PutC)
WX_STREAM_TEST_CASE(ffileStream, Output_Write)
WX_STREAM_TEST_CASE(ffileStream, Output_LastWrite)
WX_STREAM_TEST_CASE(ffileStream, Output_SeekO)
WX_STREAM_TEST_CASE(ffileStream, Output_TellO)
