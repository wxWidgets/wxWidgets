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

class FrameTestCase
{
public:
    FrameTestCase();
    ~FrameTestCase();

protected:
    wxFrame *m_frame;

    wxDECLARE_NO_COPY_CLASS(FrameTestCase);
};

FrameTestCase::FrameTestCase()
{
    m_frame = new wxFrame(nullptr, wxID_ANY, "test frame");
    m_frame->Show();
}

FrameTestCase::~FrameTestCase()
{
    m_frame->Destroy();
}

TEST_CASE_METHOD(FrameTestCase, "Frame::Iconize", "[frame]")
{
#ifdef __WXMSW__
    EventCounter iconize(m_frame, wxEVT_ICONIZE);

    m_frame->Iconize();
    m_frame->Iconize(false);

    CHECK(iconize.GetCount() == 2);
#endif
}

TEST_CASE_METHOD(FrameTestCase, "Frame::Close", "[frame]")
{
    EventCounter close(m_frame, wxEVT_CLOSE_WINDOW);

    m_frame->Close();

    CHECK(close.GetCount() == 1);
}
