///////////////////////////////////////////////////////////////////////////////
// Name:        tests/thread/tls.cpp
// Purpose:     wxTlsValue unit test
// Author:      Vadim Zeitlin
// Created:     2008-08-28
// Copyright:   (c) 2008 Vadim Zeitlin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/thread.h"
#include "wx/tls.h"

#include <string>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

namespace
{

// NB: this struct must be a POD, so don't use wxString as its member
struct PerThreadData
{
    const char *name;
    int number;
};

wxTHREAD_SPECIFIC_DECL PerThreadData gs_threadData;
wxTHREAD_SPECIFIC_DECL int gs_threadInt;

// ----------------------------------------------------------------------------
// test thread
// ----------------------------------------------------------------------------

// this thread arbitrarily modifies all global thread-specific variables to
// make sure that the changes in it are not visible from the main thread
class TLSTestThread : public wxThread
{
public:
    // ctor both creates and starts the thread
    TLSTestThread() : wxThread(wxTHREAD_JOINABLE) { Create(); Run(); }

    virtual void *Entry() override
    {
        gs_threadInt = 17;

        gs_threadData.name = "worker";
        gs_threadData.number = 2;

        // We can't use Catch asserts outside of the main thread,
        // unfortunately.
        wxASSERT( gs_threadData.name == std::string("worker") );
        wxASSERT( gs_threadData.number == 2 );

        return nullptr;
    }
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("TLS::Int", "[tls]")
{
    CHECK( gs_threadInt == 0 );

    gs_threadInt++;
    CHECK( gs_threadInt == 1 );

    TLSTestThread().Wait();

    CHECK( gs_threadInt == 1 );
}

TEST_CASE("TLS::Struct", "[tls]")
{
    CHECK( gs_threadData.name == nullptr);
    CHECK( gs_threadData.number == 0 );

    gs_threadData.name = "main";
    gs_threadData.number = 1;

    CHECK( gs_threadData.number == 1 );

    TLSTestThread().Wait();

    CHECK( gs_threadData.name == std::string("main") );
    CHECK( gs_threadData.number == 1 );
}
