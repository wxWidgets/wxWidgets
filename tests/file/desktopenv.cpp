///////////////////////////////////////////////////////////////////////////////
// Name:        tests/file/desktopenv.cpp
// Purpose:     wxDesktopEnv unit test
// Author:      Igor Korot
// Created:     2015-11-17
// Copyright:   
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/app.h"
#include "wx/desktopenv.h"

//#include "testableframe.h"
#include "wx/wfstream.h"
#include "wx/uiaction.h"

#define DATABUFFER_SIZE     1024

class DesktopEnvTestCase : public CppUnit::TestCase
{
public:
    DesktopEnvTestCase();
    ~DesktopEnvTestCase();
private:
    CPPUNIT_TEST_SUITE( DesktopEnvTestCase );
    CPPUNIT_TEST( MoveToTrash );
    CPPUNIT_TEST_SUITE_END();
    void MoveToTrash();
    wxDesktopEnv *m_env;

    wxDECLARE_NO_COPY_CLASS( DesktopEnvTestCase );
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DesktopEnvTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DesktopEnvTestCase, "DesktopEnvTestCase" );

#if wxUSE_UIACTIONSIMULATOR

DesktopEnvTestCase::DesktopEnvTestCase()
{
    m_env = new wxDesktopEnv();
}

DesktopEnvTestCase::~DesktopEnvTestCase()
{
    delete m_env;
}

void DesktopEnvTestCase::MoveToTrash()
{
    char buf[DATABUFFER_SIZE];
    std::ofstream out( "ffileinstream.test", std::ofstream::out );

    // Init the data buffer.
    for( size_t i = 0; i < DATABUFFER_SIZE; i++ )
    {
        buf[i] = (i % 0xFF);
        // Save the data
        out << buf << std::endl;
    }
    out.close();
#if defined( __WINDOWS__ )
    CPPUNIT_ASSERT( m_env->MoveFileToRecycleBin( "C:\\wxWidgets\\tests\\ffileinstream.test" ) );
#else
    CPPUNIT_ASSERT( m_env->MoveFileToRecycleBin( "ffileinstream.test" ) );
#endif
    wxMkdir( "TrashTest" );
    std::ofstream out1( "TrashTest/ffileinstream.test", std::ofstream::out );

    // Init the data buffer.
    for( size_t i = 0; i < DATABUFFER_SIZE; i++ )
    {
        buf[i] = (i % 0xFF);
        // Save the data
        out1 << buf << std::endl;
    }
    out1.close();
#if defined( __WINDOWS__ )
    CPPUNIT_ASSERT( m_env->MoveFileToRecycleBin( "C:\\wxWidgets\\tests\\TrashTest" ) );
#else
    CPPUNIT_ASSERT( m_env->MoveFileToRecycleBin( "TrashTest" ) );
#endif
}

#endif
