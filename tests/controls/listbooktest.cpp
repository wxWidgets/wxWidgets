///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/listbooktest.cpp
// Purpose:     wxListbook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/listbook.h"
#include "wx/listctrl.h"
#include "bookctrlbasetest.h"

#include <memory>

class ListbookTestCase : public BookCtrlBaseTestCase
{
public:
    ListbookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_listbook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_LISTBOOK_PAGE_CHANGING; }

    virtual bool HasBrokenMnemonics() const override { return true; }

    std::unique_ptr<wxListbook> m_listbook;

    wxDECLARE_NO_COPY_CLASS(ListbookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(ListbookTestCase, "Listbook",
                       "[listbook][book]");

ListbookTestCase::ListbookTestCase()
{
    m_listbook = make_unique<wxListbook>(wxTheApp->GetTopWindow(), wxID_ANY,
                                         wxDefaultPosition, wxSize(400, 300));
    AddPanels();
}


TEST_CASE_METHOD(ListbookTestCase, "Listbook::ListView", "[listbook]")
{
    wxListView* listview = m_listbook->GetListView();

    CHECK(listview);
    CHECK(listview->GetItemCount() == 3);
    CHECK(listview->GetItemText(0) == "Panel 1");
}

#endif //wxUSE_LISTBOOK
