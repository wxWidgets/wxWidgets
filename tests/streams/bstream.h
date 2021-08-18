///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/bstream.h
// Purpose:     Template class for testing base stream functions.
// Author:      Hans Van Leemputten
// Copyright:   (c) 2004 Hans Van Leemputten
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TESTBSTREAM_H__
#define _WX_TESTBSTREAM_H__

///////////////////////////////////////////////////////////////////////////////
// Some macros preventing us from typing too much ;-)
//

#define STREAM_TEST_NAME "Streams"
#define COMPOSE_TEST_NAME(Name) \
    STREAM_TEST_NAME "." #Name
#define STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(Name) \
    CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( Name, COMPOSE_TEST_NAME(Name) );

///////////////////////////////////////////////////////////////////////////////
// Template class that implements a test for all base stream functions.
//

template <class TStreamIn, class TStreamOut> class BaseStreamTestCase : public CppUnit::TestCase
{
protected:
    typedef BaseStreamTestCase<TStreamIn, TStreamOut> StreamTestCase;

    class CleanupHelper
    {
    public:
        CleanupHelper(StreamTestCase *value)
            :m_pCleanup(value)
        {}
        ~CleanupHelper()
        {
            m_pCleanup->DeleteInStream();
            m_pCleanup->DeleteOutStream();
        }
    private:
        StreamTestCase   *m_pCleanup;
    };
    friend class CleanupHelper;

public:
    BaseStreamTestCase()
        :m_bSimpleTellITest(false),
         m_bSimpleTellOTest(false),
         m_bSeekInvalidBeyondEnd(true),
         m_bEofAtLastRead(true),
         m_pCurrentIn(NULL),
         m_pCurrentOut(NULL)
    { /* Nothing extra */ }
    virtual ~BaseStreamTestCase()
    {
        // Prevent mem leaks!
        delete m_pCurrentIn;
        delete m_pCurrentOut;
    }

protected:
    /*
     * Input stream tests.
     */

    // Just try to perform a GetSize() on the input stream.
    void Input_GetSize()
    {
        CleanupHelper cleanup(this);
        const TStreamIn &stream_in = CreateInStream();
        CPPUNIT_ASSERT(!stream_in.Eof());

        // Size should be greater than zero.
        CPPUNIT_ASSERT(stream_in.GetSize() != 0);
    }

    // The variant for non-seekable streams.
    void Input_GetSizeFail()
    {
        CleanupHelper cleanup(this);
        const TStreamIn &stream_in = CreateInStream();

        CPPUNIT_ASSERT(stream_in.GetSize() == 0);
    }

    // Just try to perform a GetC() on the input stream.
    void Input_GetC()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();
        CPPUNIT_ASSERT(!stream_in.Eof());

        // If no exception occurs the test is successful.
        (void)stream_in.GetC();
    }

    // Just try to perform a Read() on the input stream.
    void Input_Read()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();
        CPPUNIT_ASSERT(!stream_in.Eof());

        // Note: the input stream should at least be of min size +10!

        char buf[10];
        (void)stream_in.Read(buf, 10);

        CPPUNIT_ASSERT(!stream_in.Eof());

        DoCheckInputStream(stream_in);

        // Test the stream version as well.
        TStreamOut &stream_out = CreateOutStream();
        (void)stream_in.Read(stream_out);

        // The output stream should have read the input stream till the end.
        CPPUNIT_ASSERT(stream_in.Eof());
    }

    // Test and see what happens to the EOF when we
    // read after EOF was encountered.
    void Input_Eof()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();
        CPPUNIT_ASSERT(!stream_in.Eof());
        // Double check to see if Eof it self doesn't changes the Eof status.
        CPPUNIT_ASSERT(!stream_in.Eof());

        // Travel to the end of the stream.
        while(!stream_in.Eof())
        {
            CPPUNIT_ASSERT_MESSAGE( "unexpected non-EOF stream error",
                                    stream_in.IsOk() );

            // Read, we move one byte along.
            (void)stream_in.GetC();
#if 0
            // EOF behaviour is different in streams, disabled (for now?)

            if (m_bEofAtLastRead)
                // EOF should only occur after the last successful get.
                CPPUNIT_ASSERT_MESSAGE("Eof is detected too late.", !(stream_in.LastRead() != 1 && stream_in.Eof()));
            else
                // EOF should only occur after a failed get.
                CPPUNIT_ASSERT_MESSAGE("Eof is detected too soon.", !(stream_in.LastRead() == 1 && stream_in.Eof()));
#endif
        }

        // Check EOF stream state.
        CPPUNIT_ASSERT_MESSAGE("EOF is not EOF?", stream_in.Eof());

        // Ok we found the end, let's see if we can go past it.
        for (size_t i = 0; i < 100; i++)
            (void)stream_in.GetC();

        // Check for EOF correctness.
        CPPUNIT_ASSERT_MESSAGE("EOF is wrong when we read past EOF!", stream_in.Eof());
        CPPUNIT_ASSERT_MESSAGE("Last error is not EOF while stream_in.Eof() is true", stream_in.GetLastError() == wxSTREAM_EOF);
    }

    // Just try to perform a LastRead() on the input stream.
    void Input_LastRead()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();
        CPPUNIT_ASSERT(!stream_in.Eof());

        char buf[5];
        (void)stream_in.Read(buf, 5);
        REQUIRE( stream_in.GetLastError() == wxSTREAM_NO_ERROR );
        CHECK( stream_in.LastRead() == 5 );
        (void)stream_in.GetC();
        REQUIRE( stream_in.GetLastError() == wxSTREAM_NO_ERROR );
        CHECK( stream_in.LastRead() == 1 );
    }

    void Input_CanRead()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();

        CPPUNIT_ASSERT( stream_in.CanRead() );

        // read the entire contents
        (void)stream_in.Read(CreateOutStream());

        CPPUNIT_ASSERT( !stream_in.CanRead() );
    }

    // Just try to perform a SeekI() on the input stream.
    void Input_SeekI()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();

        CPPUNIT_ASSERT( stream_in.IsSeekable() );
        CPPUNIT_ASSERT(!stream_in.Eof());

        // Try to Seek in the stream...
        CPPUNIT_ASSERT_EQUAL(2, stream_in.SeekI(2, wxFromStart));
        CPPUNIT_ASSERT_EQUAL(4, stream_in.SeekI(2, wxFromCurrent));
        // Not sure the following line is correct, so test it differently.
        //CPPUNIT_ASSERT_EQUAL(stream_in.GetSize()-2, stream_in.SeekI(-2, wxFromEnd));
        CPPUNIT_ASSERT(stream_in.SeekI(-2, wxFromEnd) != wxInvalidOffset);
        // Go beyond the stream size.
        CPPUNIT_ASSERT((stream_in.SeekI(10, wxFromCurrent) == wxInvalidOffset) == m_bSeekInvalidBeyondEnd);
    }

    void Input_SeekIFail()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();

        CPPUNIT_ASSERT( !stream_in.IsSeekable() );
    }

    // Just try to perform a TellI() on the input stream.
    void Input_TellI()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();

        CPPUNIT_ASSERT(!stream_in.Eof());

        // Try to Get the location in the stream...
        CPPUNIT_ASSERT_EQUAL(0, stream_in.TellI());
        (void)stream_in.GetC();
        CPPUNIT_ASSERT_EQUAL(1, stream_in.TellI());
        if (!m_bSimpleTellITest)
        {
            wxFileOffset pos = stream_in.SeekI(5, wxFromStart);
            CPPUNIT_ASSERT_EQUAL(pos, stream_in.TellI());
            (void)stream_in.GetC();
            CPPUNIT_ASSERT_EQUAL(6, stream_in.TellI());
            pos = stream_in.SeekI(2, wxFromCurrent);
            CPPUNIT_ASSERT_EQUAL(pos, stream_in.TellI());
            pos = stream_in.SeekI(5, wxFromStart);
            CPPUNIT_ASSERT_EQUAL(pos, stream_in.TellI());
        }
    }

    // Just try to perform a Peek() on the input stream.
    void Input_Peek()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();

        // Test the full stream
        while (stream_in.IsOk())
        {
            char peekChar = stream_in.Peek();
            char getChar = stream_in.GetC();
            if (stream_in.LastRead() == 1)
                CPPUNIT_ASSERT_EQUAL(getChar, peekChar);
        }
    }

    // Just try to perform a Ungetch() on the input stream.
    void Input_Ungetch()
    {
        CleanupHelper cleanup(this);
        TStreamIn &stream_in = CreateInStream();
        CPPUNIT_ASSERT(!stream_in.Eof());

        const char *ungetstr = "test";
        size_t ungetsize = stream_in.Ungetch(ungetstr, strlen(ungetstr) + 1);
        if (ungetsize != 0)
        {
            CPPUNIT_ASSERT_EQUAL(strlen(ungetstr) + 1, ungetsize);
            char buf[10];
            (void)stream_in.Read(buf, ungetsize);
            CPPUNIT_ASSERT(strcmp(buf, ungetstr) == 0);
        }

        if (stream_in.Ungetch('a'))
        {
            CPPUNIT_ASSERT_EQUAL(int('a'), stream_in.GetC());
        }
    }

    /*
     * Output stream tests.
     */

    // Just try to perform a PutC() on the output stream.
    void Output_PutC()
    {
        CleanupHelper cleanup(this);
        TStreamOut &stream_out = CreateOutStream();

        const char *buf = "Some text";
        const wxFileOffset len = strlen(buf);
        for ( int i = 0; i < len; i++ )
            stream_out.PutC(buf[i]);

        if ( stream_out.IsSeekable() )
            CPPUNIT_ASSERT_EQUAL(len, stream_out.TellO());
    }

    // Just try to perform a Write() on the output stream.
    void Output_Write()
    {
        CleanupHelper cleanup(this);
        TStreamOut &stream_out = CreateOutStream();

        // Do the buffer version.
        const char *buf = "Some text";
        const wxFileOffset len = strlen(buf);
        (void)stream_out.Write(buf, len);
        if ( stream_out.IsSeekable() )
            CPPUNIT_ASSERT_EQUAL( len, stream_out.TellO() );

        // Do the Stream version.
        TStreamIn &stream_in = CreateInStream();
        (void)stream_out.Write(stream_in);

        if ( stream_out.IsSeekable() )
            CPPUNIT_ASSERT(stream_out.TellO() > len);
    }

    // Just try to perform a LastWrite() on the output stream.
    void Output_LastWrite()
    {
        CleanupHelper cleanup(this);
        TStreamOut &stream_out = CreateOutStream();

        const char *buf = "12345";
        (void)stream_out.Write(buf, 5);
        CPPUNIT_ASSERT_EQUAL(5, stream_out.LastWrite());
        (void)stream_out.PutC('1');
        CPPUNIT_ASSERT_EQUAL(1, stream_out.LastWrite());
    }

    // Just try to perform a SeekO() on the output stream.
    void Output_SeekO()
    {
        CleanupHelper cleanup(this);
        TStreamOut &stream_out = CreateOutStream();

        CPPUNIT_ASSERT( stream_out.IsSeekable() );

        // First put some data in the stream, so it is not empty.
        const char *buf = "1234567890";
        (void)stream_out.Write(buf, 10);

        // Try to Seek in the stream...
        CPPUNIT_ASSERT_EQUAL(2, stream_out.SeekO(2, wxFromStart));
        CPPUNIT_ASSERT_EQUAL(4, stream_out.SeekO(2, wxFromCurrent));
        // Not sure the following line is correct, so test it differently.
        //CPPUNIT_ASSERT_EQUAL(stream_in.GetSize()-2, stream_out.SeekO(-2, wxFromEnd));
        CPPUNIT_ASSERT(stream_out.SeekO(-2, wxFromEnd) != wxInvalidOffset);
        // Go beyond the stream size.
        CPPUNIT_ASSERT((stream_out.SeekO(10, wxFromCurrent) == wxInvalidOffset) == m_bSeekInvalidBeyondEnd);
    }

    void Output_SeekOFail()
    {
        CleanupHelper cleanup(this);
        TStreamOut &stream_out = CreateOutStream();

        CPPUNIT_ASSERT( !stream_out.IsSeekable() );
    }

    // Just try to perform a TellO() on the output stream.
    void Output_TellO()
    {
        CleanupHelper cleanup(this);
        TStreamOut &stream_out = CreateOutStream();

        // Try to Get the location in the stream...
        CPPUNIT_ASSERT_EQUAL(0, stream_out.TellO());
        (void)stream_out.PutC('1');
        CPPUNIT_ASSERT_EQUAL(1, stream_out.TellO());
        if (!m_bSimpleTellOTest)
        {
            // First put some extra data in the stream, so it's not empty.
            const char *buf = "1234567890";
            (void)stream_out.Write(buf, 10);

            wxFileOffset pos = stream_out.SeekO(5, wxFromStart);
            CPPUNIT_ASSERT_EQUAL(pos, stream_out.TellO());
            (void)stream_out.PutC('1');
            CPPUNIT_ASSERT_EQUAL(6, stream_out.TellO());
            pos = stream_out.SeekO(2, wxFromCurrent);
            CPPUNIT_ASSERT_EQUAL(pos, stream_out.TellO());
            pos = stream_out.SeekO(5, wxFromStart);
            CPPUNIT_ASSERT_EQUAL(pos, stream_out.TellO());
        }
    }

protected:
    // Some tests can be configured... here you can find the config settings
    bool m_bSimpleTellITest;    // if true, no SeekI will be used by the TellI test.
                                // Default false.
    bool m_bSimpleTellOTest;    // if true, no SeekO will be used by the TellI test.
                                // Default false.
    bool m_bSeekInvalidBeyondEnd; // if true a SeekI|O beyond the end of the stream should return wxInvalidOffset
                                  // Default true.
    bool m_bEofAtLastRead;      // Does EOF occur at the moment the last byte is read or when read past the last byte.
                                // Default true.
protected:
    TStreamIn &CreateInStream()
    {
        if (m_pCurrentIn)
        {
            wxFAIL_MSG(wxT("Error in test case, the previous input stream needs to be delete first!"));
        }

        m_pCurrentIn = DoCreateInStream();
        wxASSERT(m_pCurrentIn != NULL);
        return *m_pCurrentIn;
    }
    TStreamOut &CreateOutStream()
    {
        if (m_pCurrentOut)
        {
            wxFAIL_MSG(wxT("Error in test case, the previous output stream needs to be delete first!"));
        }

        m_pCurrentOut = DoCreateOutStream();
        wxASSERT(m_pCurrentOut != NULL);
        return *m_pCurrentOut;
    }

    void DeleteInStream()
    {
        if (m_pCurrentIn == NULL)
            return;
        delete m_pCurrentIn;
        m_pCurrentIn = NULL;
        // In case something extra needs to be done.
        DoDeleteInStream();
    }
    void DeleteOutStream()
    {
        if (m_pCurrentOut == NULL)
            return;

        CPPUNIT_ASSERT(m_pCurrentOut->Close());

        delete m_pCurrentOut;
        m_pCurrentOut = NULL;
        // In case something extra needs to be done.
        DoDeleteOutStream();
    }

protected:
    // Items that need to be implemented by a derived class!
    virtual TStreamIn  *DoCreateInStream() = 0;
    virtual TStreamOut *DoCreateOutStream() = 0;
    virtual void DoCheckInputStream(TStreamIn& stream_in)
    {
        CPPUNIT_ASSERT(stream_in.IsOk());
    }

    virtual void DoDeleteInStream()  { /* Depends on the base class */ }
    virtual void DoDeleteOutStream() { /* Depends on the base class */ }

private:
    TStreamIn  *m_pCurrentIn;
    TStreamOut *m_pCurrentOut;
};

#endif


