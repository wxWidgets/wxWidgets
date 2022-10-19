///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/frametest.cpp
// Purpose:     wxFrame  unit test
// Author:      Steven Lamerton
// Created:     2010-07-10
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/frame.h"
#endif // WX_PRECOMP

#include "testableframe.h"

class FrameTestCase : public CppUnit::TestCase
{
public:
    FrameTestCase() { }

    void setUp() override;
    void tearDown() override;

private:
    CPPUNIT_TEST_SUITE( FrameTestCase );
        CPPUNIT_TEST( Iconize );
        CPPUNIT_TEST( Close );
    CPPUNIT_TEST_SUITE_END();

    void Iconize();
    void Close();

    wxFrame *m_frame;

    wxDECLARE_NO_COPY_CLASS(FrameTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( FrameTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( FrameTestCase, "FrameTestCase" );

void FrameTestCase::setUp()
{
    m_frame = new wxFrame(nullptr, wxID_ANY, "test frame");
    m_frame->Show();
}

void FrameTestCase::tearDown()
{
    m_frame->Destroy();
}

void FrameTestCase::Iconize()
{
#ifdef __WXMSW__
    EventCounter iconize(m_frame, wxEVT_ICONIZE);

    m_frame->Iconize();
    m_frame->Iconize(false);

    CPPUNIT_ASSERT_EQUAL(2, iconize.GetCount());
#endif
}

void FrameTestCase::Close()
{
    EventCounter close(m_frame, wxEVT_CLOSE_WINDOW);

    m_frame->Close();

    CPPUNIT_ASSERT_EQUAL(1, close.GetCount());
}
