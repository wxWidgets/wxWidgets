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
// The test helper

namespace
{

enum Action { DONOTHING, CLOSE, COMMIT, DISCARD };

// the common test code
//
void DoTest(Action action, bool shouldHaveCommited)
{
    TestFile temp;

    {
        wxTempFileOutputStream out(temp.GetName());
        out.Write("Affer", 5);
        CHECK(out.SeekO(2) == 2);
        out.Write("t", 1);
        CHECK(out.IsSeekable());
        CHECK(out.GetLength() == 5);
        CHECK(out.TellO() == 3);

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
    CHECK(strcmp(buf, shouldHaveCommited ? "After" : "Before") == 0);
}


} // anonymous namespace

///////////////////////////////////////////////////////////////////////////////
// The tests

TEST_CASE("tempStream::DoNothing", "[stream][tempStream]")
{
    DoTest(DONOTHING, false);
}

TEST_CASE("tempStream::Close", "[stream][tempStream]")
{
    DoTest(CLOSE, true);
}

TEST_CASE("tempStream::Commit", "[stream][tempStream]")
{
    DoTest(COMMIT, true);
}

TEST_CASE("tempStream::Discard", "[stream][tempStream]")
{
    DoTest(DISCARD, false);
}

#endif // wxUSE_STREAMS && wxUSE_FILE
