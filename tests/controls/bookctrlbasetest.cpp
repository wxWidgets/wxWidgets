///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/bookctrlbasetest.cpp
// Purpose:     wxBookCtrlBase unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/bookctrl.h"
#include "bookctrlbasetest.h"
#include "testableframe.h"

void BookCtrlBaseTestCase::AddPanels()
{
    wxBookCtrlBase * const base = GetBase();

    m_panel1 = new wxPanel(base);
    m_panel2 = new wxPanel(base);
    m_panel3 = new wxPanel(base);

    base->AddPage(m_panel1, "Panel 1");
    base->AddPage(m_panel2, "Panel 2");
    base->AddPage(m_panel3, "Panel 3");
}

void BookCtrlBaseTestCase::Selection()
{
    wxBookCtrlBase * const base = GetBase();

    base->SetSelection(0);

    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel1, wxWindow), base->GetCurrentPage());

    base->AdvanceSelection(false);

    CPPUNIT_ASSERT_EQUAL(2, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel3, wxWindow), base->GetCurrentPage());

    base->AdvanceSelection();

    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel1, wxWindow), base->GetCurrentPage());

    base->ChangeSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(wxStaticCast(m_panel2, wxWindow), base->GetCurrentPage());
}

void BookCtrlBaseTestCase::Text()
{
    wxBookCtrlBase * const base = GetBase();

    CPPUNIT_ASSERT_EQUAL("Panel 1", base->GetPageText(0));

    base->SetPageText(1, "Some other string");

    CPPUNIT_ASSERT_EQUAL("Some other string", base->GetPageText(1));

    base->SetPageText(2, "string with /nline break");

    CPPUNIT_ASSERT_EQUAL("string with /nline break", base->GetPageText(2));
}

void BookCtrlBaseTestCase::PageManagement()
{
    wxBookCtrlBase * const base = GetBase();

    base->InsertPage(1, new wxPanel(base), "New Panel", true);

    CPPUNIT_ASSERT_EQUAL(1, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(4, base->GetPageCount());

    base->DeletePage(1);

    CPPUNIT_ASSERT_EQUAL(3, base->GetPageCount());

    base->RemovePage(0);

    CPPUNIT_ASSERT_EQUAL(2, base->GetPageCount());

    base->DeleteAllPages();

    CPPUNIT_ASSERT_EQUAL(0, base->GetPageCount());

    AddPanels();
}

void BookCtrlBaseTestCase::ChangeEvents()
{
    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                          wxTestableFrame);

    wxBookCtrlBase * const base = GetBase();

    base->SetSelection(0);

    EventCounter count(base, GetChangingEvent());
    EventCounter count1(base, GetChangedEvent());

    base->SetSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(GetChangingEvent()));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(GetChangedEvent()));

    base->ChangeSelection(2);

    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(GetChangingEvent()));
    CPPUNIT_ASSERT_EQUAL(0, frame->GetEventCount(GetChangedEvent()));

    base->AdvanceSelection();

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(GetChangingEvent()));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(GetChangedEvent()));

    base->AdvanceSelection(false);

    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(GetChangingEvent()));
    CPPUNIT_ASSERT_EQUAL(1, frame->GetEventCount(GetChangedEvent()));
}