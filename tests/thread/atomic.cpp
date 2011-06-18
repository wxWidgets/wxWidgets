///////////////////////////////////////////////////////////////////////////////
// Name:        tests/thread/atomic.cpp
// Purpose:     wxAtomic??? unit test
// Author:      Armel Asselin
// Created:     2006-12-14
// RCS-ID:      $Id$
// Copyright:   (c) 2006 Armel Asselin
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
#endif // WX_PRECOMP

#include "wx/atomic.h"
#include "wx/thread.h"
#include "wx/dynarray.h"
#include "wx/log.h"

WX_DEFINE_ARRAY_PTR(wxThread *, wxArrayThread);

// ----------------------------------------------------------------------------
// constants
// ----------------------------------------------------------------------------

// number of times to run the loops: the code takes too long to run if we use
// the bigger value with generic atomic operations implementation
#ifdef wxHAS_ATOMIC_OPS
    static const wxInt32 ITERATIONS_NUM = 10000000;
#else
    static const wxInt32 ITERATIONS_NUM = 1000;
#endif

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class AtomicTestCase : public CppUnit::TestCase
{
public:
    AtomicTestCase() { }

    enum ETestType
    {
        IncAndDecMixed,
        IncOnly,
        DecOnly
    };

private:
    class MyThread : public wxThread
    {
    public:
        MyThread(wxAtomicInt &operateOn, ETestType testType) : wxThread(wxTHREAD_JOINABLE),
            m_operateOn(operateOn), m_testType(testType) {}

        // thread execution starts here
        virtual void *Entry();

    public:
        wxAtomicInt &m_operateOn;
        ETestType m_testType;
    };

    CPPUNIT_TEST_SUITE( AtomicTestCase );
        CPPUNIT_TEST( TestNoThread );
        CPPUNIT_TEST( TestDecReturn );
        CPPUNIT_TEST( TestTwoThreadsMix );
        CPPUNIT_TEST( TestTenThreadsMix );
        CPPUNIT_TEST( TestTwoThreadsSeparate );
        CPPUNIT_TEST( TestTenThreadsSeparate );
    CPPUNIT_TEST_SUITE_END();

    void TestNoThread();
    void TestDecReturn();
    void TestTenThreadsMix() { TestWithThreads(10, IncAndDecMixed); }
    void TestTwoThreadsMix() { TestWithThreads(2, IncAndDecMixed); }
    void TestTenThreadsSeparate() { TestWithThreads(10, IncOnly); }
    void TestTwoThreadsSeparate() { TestWithThreads(2, IncOnly); }
    void TestWithThreads(int count, ETestType testtype);

    DECLARE_NO_COPY_CLASS(AtomicTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( AtomicTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( AtomicTestCase, "AtomicTestCase" );

void AtomicTestCase::TestNoThread()
{
    wxAtomicInt int1 = 0,
                int2 = 0;

    for ( wxInt32 i = 0; i < ITERATIONS_NUM; ++i )
    {
        wxAtomicInc(int1);
        wxAtomicDec(int2);
    }

    CPPUNIT_ASSERT( int1 == ITERATIONS_NUM );
    CPPUNIT_ASSERT( int2 == -ITERATIONS_NUM );
}

void AtomicTestCase::TestDecReturn()
{
    wxAtomicInt i(0);
    wxAtomicInc(i);
    wxAtomicInc(i);
    CPPUNIT_ASSERT( i == 2 );

    CPPUNIT_ASSERT( wxAtomicDec(i) > 0 );
    CPPUNIT_ASSERT( wxAtomicDec(i) == 0 );
}

void AtomicTestCase::TestWithThreads(int count, ETestType testType)
{
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
        }
        else
            threads.Add(thread);
    }

    for ( i = 0; i < count; ++i )
    {
        threads[i]->Run();
    }


    for ( i = 0; i < count; ++i )
    {
        // each thread should return 0, else it detected some problem
        CPPUNIT_ASSERT (threads[i]->Wait() == (wxThread::ExitCode)0);
    }

    CPPUNIT_ASSERT( int1 == 0 );
}

// ----------------------------------------------------------------------------

void *AtomicTestCase::MyThread::Entry()
{
    wxInt32 negativeValuesSeen = 0;

    for ( wxInt32 i = 0; i < ITERATIONS_NUM; ++i )
    {
        switch ( m_testType )
        {
            case AtomicTestCase::IncAndDecMixed:
                wxAtomicInc(m_operateOn);
                wxAtomicDec(m_operateOn);

                if (m_operateOn < 0)
                    ++negativeValuesSeen;
                break;

            case AtomicTestCase::IncOnly:
                wxAtomicInc(m_operateOn);
                break;

            case AtomicTestCase::DecOnly:
                wxAtomicDec(m_operateOn);
                break;
        }
    }

    return wxUIntToPtr(negativeValuesSeen);
}
