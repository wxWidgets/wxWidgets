///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbox.cpp
// Purpose:     wxListBox unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/listbox.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"
#include "testableframe.h"
#include "wx/uiaction.h"

#include <memory>

class ListBoxTestCase : public ItemContainerTestCase
{
public:
    ListBoxTestCase();

protected:
    virtual wxItemContainer *GetContainer() const override
    { return m_list.get(); }
    virtual wxWindow *GetContainerWindow() const override
    { return m_list.get(); }

    // Recreate the list box as an owner-drawn one, only used under MSW.
    void MakeOwnerDrawn()
    {
        m_list = make_unique<wxListBox>(wxTheApp->GetTopWindow(), wxID_ANY,
                                        wxDefaultPosition, wxSize(300, 200),
                                        0, nullptr, wxLB_OWNERDRAW);
    }

    std::unique_ptr<wxListBox> m_list;

    wxDECLARE_NO_COPY_CLASS(ListBoxTestCase);
};

wxITEM_CONTAINER_TESTS(ListBoxTestCase, "ListBox",
                       "[listbox][item-container]");

// Under MSW the tests below are run twice, for the normal and the owner-drawn
// list boxes. There is no need to run the wxItemContainer tests for the latter
// as they're already tested with wxCheckListBox which is always owner-drawn.
#ifdef __WXMSW__
    #define wxLISTBOX_TEST_OWNERDRAWN()                    \
        const bool ownerDrawn = GENERATE(false, true);      \
        CAPTURE(ownerDrawn);                                \
        if ( ownerDrawn )                                   \
            MakeOwnerDrawn()
#else
    #define wxLISTBOX_TEST_OWNERDRAWN() ((void)0)
#endif

ListBoxTestCase::ListBoxTestCase()
{
    m_list = make_unique<wxListBox>(wxTheApp->GetTopWindow(), wxID_ANY,
                                    wxDefaultPosition, wxSize(300, 200));
}


TEST_CASE_METHOD(ListBoxTestCase, "ListBox::Sort", "[listbox]")
{
    wxLISTBOX_TEST_OWNERDRAWN();

#ifndef __WXOSX__
    m_list = make_unique<wxListBox>(wxTheApp->GetTopWindow(), wxID_ANY,
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    nullptr, wxLB_SORT);

    wxArrayString testitems;
    testitems.Add("aaa");
    testitems.Add("Aaa");
    testitems.Add("aba");
    testitems.Add("aaab");
    testitems.Add("aab");
    testitems.Add("AAA");

    m_list->Append(testitems);

#if defined(__WXQT__) && defined(__WINDOWS__)
    CHECK(m_list->GetString(0) == "aaa");
    CHECK(m_list->GetString(1) == "Aaa");
    CHECK(m_list->GetString(2) == "AAA");
    CHECK(m_list->GetString(3) == "aaab");
    CHECK(m_list->GetString(4) == "aab");
    CHECK(m_list->GetString(5) == "aba");

    m_list->Append("a", wxUIntToPtr(1));

    CHECK(m_list->GetString(0) == "a");
    CHECK(m_list->GetClientData(0) == wxUIntToPtr(1));
#else
    CHECK(m_list->GetString(0) == "AAA");
    CHECK(m_list->GetString(1) == "Aaa");
    CHECK(m_list->GetString(2) == "aaa");
    CHECK(m_list->GetString(3) == "aaab");
    CHECK(m_list->GetString(4) == "aab");
    CHECK(m_list->GetString(5) == "aba");
#endif
#endif
}

TEST_CASE_METHOD(ListBoxTestCase, "ListBox::MultipleSelect", "[listbox]")
{
    wxLISTBOX_TEST_OWNERDRAWN();

    m_list = make_unique<wxListBox>(wxTheApp->GetTopWindow(), wxID_ANY,
                                    wxDefaultPosition, wxDefaultSize, 0,
                                    nullptr, wxLB_MULTIPLE);

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    m_list->Append(testitems);

    m_list->SetSelection(0);

    wxArrayInt selected;
    m_list->GetSelections(selected);

    CHECK(selected.Count() == 1);
    CHECK(selected.Item(0) == 0);

    m_list->SetSelection(2);

    m_list->GetSelections(selected);

    CHECK(selected.Count() == 2);
    CHECK(selected.Item(1) == 2);

    m_list->Deselect(0);

    m_list->GetSelections(selected);

    CHECK(selected.Count() == 1);
    CHECK(selected.Item(0) == 2);

    CHECK(!m_list->IsSelected(0));
    CHECK(!m_list->IsSelected(1));
    CHECK(m_list->IsSelected(2));
    CHECK(!m_list->IsSelected(3));

    m_list->SetSelection(0);
    m_list->SetSelection(wxNOT_FOUND);

    m_list->GetSelections(selected);
    CHECK(selected.Count() == 0);
}

TEST_CASE_METHOD(ListBoxTestCase, "ListBox::ClickEvents", "[listbox]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    wxLISTBOX_TEST_OWNERDRAWN();

    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                              wxTestableFrame);

    EventCounter selected(frame, wxEVT_LISTBOX);
    EventCounter dclicked(frame, wxEVT_LISTBOX_DCLICK);

    wxUIActionSimulator sim;

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_list->Append(testitems);

    m_list->Refresh();
    m_list->Update();

    sim.MouseMove(m_list->ClientToScreen(wxPoint(10, 10)));
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK(selected.GetCount() == 1);

    sim.MouseDblClick();
    wxYield();

    CHECK(dclicked.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(ListBoxTestCase, "ListBox::ClickNotOnItem", "[listbox]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    wxLISTBOX_TEST_OWNERDRAWN();

    wxTestableFrame* frame = wxStaticCast(wxTheApp->GetTopWindow(),
                                              wxTestableFrame);

    EventCounter selected(frame, wxEVT_LISTBOX);
    EventCounter dclicked(frame, wxEVT_LISTBOX_DCLICK);

    wxUIActionSimulator sim;

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_list->Append(testitems);

    // It is important to set a valid selection: if the control doesn't have
    // any, clicking anywhere in it, even outside of any item, selects the
    // first item in the control under GTK resulting in a selection changed
    // event. This is not a wx bug, just the native platform behaviour so
    // simply avoid it by starting with a valid selection.
    m_list->SetSelection(0);

    m_list->Refresh();
    m_list->Update();

    sim.MouseMove(m_list->ClientToScreen(wxPoint(m_list->GetSize().x - 10, m_list->GetSize().y - 10)));
    wxYield();

    sim.MouseClick();
    wxYield();

    sim.MouseDblClick();
    wxYield();

    //If we are not clicking on an item we shouldn't have any events
    CHECK(selected.GetCount() == 0);
    CHECK(dclicked.GetCount() == 0);
#endif
}

TEST_CASE_METHOD(ListBoxTestCase, "ListBox::HitTest", "[listbox]")
{
    wxLISTBOX_TEST_OWNERDRAWN();

    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_list->Append(testitems);

#ifdef __WXGTK__
    // The control needs to be realized for HitTest() to work.
    wxYield();
#endif

    wxPoint p(5, 5);
#ifdef __WXOSX__
    // On macOS >= 11 wxListBox has a new layout because underlying
    // NSTableView has a new style with padding so we need to move
    // the point to be tested to another position.
    if ( wxCheckOsVersion(11, 0) )
    {
        p = wxPoint(10, 10);
    }
#endif
    CHECK( m_list->HitTest(p) == 0 );

    CHECK( m_list->HitTest(290, 190) == wxNOT_FOUND );
}

#endif //wxUSE_LISTBOX
