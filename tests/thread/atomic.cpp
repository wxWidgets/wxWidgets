///////////////////////////////////////////////////////////////////////////////
// Name:        tests/thread/atomic.cpp
// Purpose:     wxAtomic??? unit test
// Author:      Armel Asselin
// Created:     2006-12-14
// Copyright:   (c) 2006 Armel Asselin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/atomic.h"
#include "wx/thread.h"
#include "wx/dynarray.h"
#include "wx/log.h"
#include "wx/vector.h"

typedef wxVector<wxThread*> wxArrayThread;

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

static const wxInt32 ITERATIONS_NUM = 10000;

// ----------------------------------------------------------------------------
// test helper thread
// ----------------------------------------------------------------------------

namespace
{
    enum ETestType
    {
        IncAndDecMixed,
        IncOnly,
        DecOnly
    };

    class MyThread : public wxThread
    {
    public:
        MyThread(wxAtomicInt &operateOn, ETestType testType) : wxThread(wxTHREAD_JOINABLE),
            m_operateOn(operateOn), m_testType(testType) {}

        // thread execution starts here
        virtual void *Entry() override;

    public:
        wxAtomicInt &m_operateOn;
        ETestType m_testType;
    };
} // anonymous namespace

// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("Atomic::NoThread", "[atomic]")
{
    wxAtomicInt int1 = 0,
                int2 = 0;

    for ( wxInt32 i = 0; i < ITERATIONS_NUM; ++i )
    {
        wxAtomicInc(int1);
        wxAtomicDec(int2);
    }

    CHECK( int1 == ITERATIONS_NUM );
    CHECK( int2 == -ITERATIONS_NUM );
}

TEST_CASE("Atomic::ReturnValue", "[atomic]")
{
    wxAtomicInt i(0);
    REQUIRE( wxAtomicInc(i) == 1 );
    REQUIRE( wxAtomicInc(i) == 2 );

    REQUIRE( wxAtomicDec(i) == 1 );
    REQUIRE( wxAtomicDec(i) == 0 );
}

TEST_CASE("Atomic::WithThreads", "[atomic]")
{
    int count wxDUMMY_INITIALIZE(0);
    ETestType testType wxDUMMY_INITIALIZE(DecOnly);

    SECTION( "2 threads using inc and dec") { count =  2; testType = IncAndDecMixed; }
    SECTION("10 threads using inc and dec") { count = 10; testType = IncAndDecMixed; }
    SECTION( "2 threads using inc or dec" ) { count =  2; testType = IncOnly; }
    SECTION("10 threads using inc or dec" ) { count = 10; testType = IncOnly; }

    wxAtomicInt    int1=0;

    wxArrayThread  threads;

    int i;
    for ( i = 0; i < count; ++i )
    {
        ETestType actualThreadType;
        switch(testType)
        {
        default:
            actualThreadType = testType;
            break;
        case IncOnly:
            actualThreadType =  (i&1)==0 ? IncOnly : DecOnly;
            break;
        }

        MyThread *thread = new MyThread(int1, actualThreadType);

        if ( thread->Create() != wxTHREAD_NO_ERROR )
        {
            wxLogError(wxT("Can't create thread!"));
            delete thread;
        }
        else
            threads.push_back(thread);
    }

    for ( i = 0; i < count; ++i )
    {
        threads[i]->Run();
    }


    for ( i = 0; i < count; ++i )
    {
        // each thread should return 0, else it detected some problem
        CHECK (threads[i]->Wait() == (wxThread::ExitCode)nullptr);
        delete threads[i];
    }

    CHECK( int1 == 0 );
}

// ----------------------------------------------------------------------------

void *MyThread::Entry()
{
    wxInt32 negativeValuesSeen = 0;

    for ( wxInt32 i = 0; i < ITERATIONS_NUM; ++i )
    {
        switch ( m_testType )
        {
            case IncAndDecMixed:
                wxAtomicInc(m_operateOn);
                if ( wxAtomicDec(m_operateOn) < 0 )
                    ++negativeValuesSeen;
                break;

            case IncOnly:
                wxAtomicInc(m_operateOn);
                break;

            case DecOnly:
                wxAtomicDec(m_operateOn);
                break;
        }
    }

    return wxUIntToPtr(negativeValuesSeen);
}
