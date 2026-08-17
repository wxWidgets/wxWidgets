///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/iostreams.cpp
// Purpose:     unit test for input/output streams
// Author:      Vadim Zeitlin
// Created:     2008-06-15
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#if wxUSE_STREAMS

#include "wx/filename.h"
#include "wx/wfstream.h"

// --------------------------------------------------------------------------
// test fixture
// --------------------------------------------------------------------------

namespace
{

class IOStreamsTestCase
{
public:
    IOStreamsTestCase() = default;

    ~IOStreamsTestCase()
    {
        if ( !m_fnTemp.empty() )
            wxRemoveFile(m_fnTemp);
    }

protected:
    wxString GetTempFName()
    {
        m_fnTemp = wxFileName::CreateTempFileName("wxtest");
        return m_fnTemp;
    }

    template <class Stream>
    void DoTest(Stream& s)
    {
        s.PutC('x');
        CHECK( s.LastWrite() == 1 );

        s.SeekI(0);
        CHECK( s.GetC() == int('x') );
    }

private:
    wxString m_fnTemp;

    wxDECLARE_NO_COPY_CLASS(IOStreamsTestCase);
};

} // anonymous namespace

// --------------------------------------------------------------------------
// tests
// --------------------------------------------------------------------------

TEST_CASE_METHOD(IOStreamsTestCase, "IOStreams::FStream", "[iostream][stream]")
{
    wxFileStream s(GetTempFName());
    DoTest(s);
}

TEST_CASE_METHOD(IOStreamsTestCase, "IOStreams::FFStream", "[iostream][stream]")
{
    wxFFileStream s(GetTempFName());
    DoTest(s);
}

#endif // wxUSE_STREAMS
