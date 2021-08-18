///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/tempfile.cpp
// Purpose:     Test wxTempFileOutputStream
// Author:      Mike Wetherell
// Copyright:   (c) 2005 Mike Wetherell
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/wfstream.h"
#include "wx/filename.h"
#include "bstream.h"

#if wxUSE_STREAMS && wxUSE_FILE

#include "testfile.h"

///////////////////////////////////////////////////////////////////////////////
// The test case

class tempStream : public CppUnit::TestCase
{
    CPPUNIT_TEST_SUITE(tempStream);
        CPPUNIT_TEST(DoNothing);
        CPPUNIT_TEST(Close);
        CPPUNIT_TEST(Commit);
        CPPUNIT_TEST(Discard);
    CPPUNIT_TEST_SUITE_END();

    void DoNothing() { DoTest(DONOTHING, false); }
    void Close()     { DoTest(CLOSE, true); }
    void Commit()    { DoTest(COMMIT, true); }
    void Discard()   { DoTest(DISCARD, false); }

    enum Action { DONOTHING, CLOSE, COMMIT, DISCARD };
    void DoTest(Action action, bool shouldHaveCommited);
};

// the common test code
//
void tempStream::DoTest(Action action, bool shouldHaveCommited)
{
    TestFile temp;

    {
        wxTempFileOutputStream out(temp.GetName());
        out.Write("Affer", 5);
        CPPUNIT_ASSERT(out.SeekO(2) == 2);
        out.Write("t", 1);
        CPPUNIT_ASSERT(out.IsSeekable());
        CPPUNIT_ASSERT(out.GetLength() == 5);
        CPPUNIT_ASSERT(out.TellO() == 3);

        switch (action) {
            case DONOTHING: break;
            case COMMIT:    out.Commit(); break;
            case DISCARD:   out.Discard(); break;
            case CLOSE:     out.Close();
        }
    }

    wxFileInputStream in(temp.GetName());
    char buf[32];
    in.Read(buf, sizeof(buf));
    buf[in.LastRead()] = 0;
    CPPUNIT_ASSERT(strcmp(buf, shouldHaveCommited ? "After" : "Before") == 0);
}


// Register the stream sub suite, by using some stream helper macro.
// Note: Don't forget to connect it to the base suite (See: bstream.cpp => StreamCase::suite())
STREAM_TEST_SUBSUITE_NAMED_REGISTRATION(tempStream)

#endif // wxUSE_STREAMS && wxUSE_FILE
