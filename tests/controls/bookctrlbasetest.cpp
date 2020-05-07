///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/bookctrlbasetest.cpp
// Purpose:     wxBookCtrlBase unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BOOKCTRL

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/artprov.h"
#include "wx/imaglist.h"
#include "wx/bookctrl.h"
#include "bookctrlbasetest.h"
#include "testableframe.h"

void BookCtrlBaseTestCase::AddPanels()
{
    wxBookCtrlBase * const base = GetBase();

    wxSize size(32, 32);

    m_list = new wxImageList(size.x, size.y);
    m_list->Add(wxArtProvider::GetIcon(wxART_INFORMATION, wxART_OTHER, size));
    m_list->Add(wxArtProvider::GetIcon(wxART_QUESTION, wxART_OTHER, size));
    m_list->Add(wxArtProvider::GetIcon(wxART_WARNING, wxART_OTHER, size));

    base->AssignImageList(m_list);

    Realize();

    m_panel1 = new wxPanel(base);
    m_panel2 = new wxPanel(base);
    m_panel3 = new wxPanel(base);

    base->AddPage(m_panel1, "Panel 1", false, 0);
    base->AddPage(m_panel2, "Panel 2", false, 1);
    base->AddPage(m_panel3, "Panel 3", false, 2);
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

    base->InsertPage(0, new wxPanel(base), "New Panel", true, 0);

    Realize();

    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());
    CPPUNIT_ASSERT_EQUAL(4, base->GetPageCount());

    // Change the selection to verify that deleting a page before the currently
    // selected one correctly updates the selection.
    base->SetSelection(2);
    CPPUNIT_ASSERT_EQUAL(2, base->GetSelection());

    base->DeletePage(1);

    CPPUNIT_ASSERT_EQUAL(3, base->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(1, base->GetSelection());

    base->RemovePage(0);

    CPPUNIT_ASSERT_EQUAL(2, base->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(0, base->GetSelection());

    base->DeleteAllPages();

    CPPUNIT_ASSERT_EQUAL(0, base->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(-1, base->GetSelection());
}

void BookCtrlBaseTestCase::ChangeEvents()
{
    wxBookCtrlBase * const base = GetBase();

    base->SetSelection(0);

    EventCounter changing(base, GetChangingEvent());
    EventCounter changed(base, GetChangedEvent());

    base->SetSelection(1);

    CPPUNIT_ASSERT_EQUAL(1, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());

    changed.Clear();
    changing.Clear();
    base->ChangeSelection(2);

    CPPUNIT_ASSERT_EQUAL(0, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(0, changed.GetCount());

    base->AdvanceSelection();

    CPPUNIT_ASSERT_EQUAL(1, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());

    changed.Clear();
    changing.Clear();
    base->AdvanceSelection(false);

    CPPUNIT_ASSERT_EQUAL(1, changing.GetCount());
    CPPUNIT_ASSERT_EQUAL(1, changed.GetCount());
}

void BookCtrlBaseTestCase::Image()
{
    wxBookCtrlBase * const base = GetBase();

    //Check AddPanels() set things correctly
    CPPUNIT_ASSERT_EQUAL(m_list, base->GetImageList());
    CPPUNIT_ASSERT_EQUAL(0, base->GetPageImage(0));
    CPPUNIT_ASSERT_EQUAL(1, base->GetPageImage(1));
    CPPUNIT_ASSERT_EQUAL(2, base->GetPageImage(2));

    base->SetPageImage(0, 2);

    CPPUNIT_ASSERT_EQUAL(2, base->GetPageImage(2));
}

#endif
