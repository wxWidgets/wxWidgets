///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/choicebooktest.cpp
// Purpose:     wxChoicebook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_CHOICEBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/choicebk.h"
#include "bookctrlbasetest.h"

#include <memory>

class ChoicebookTestCase : public BookCtrlBaseTestCase
{
public:
    ChoicebookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_choicebook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_CHOICEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_CHOICEBOOK_PAGE_CHANGING; }

    virtual bool HasBrokenMnemonics() const override { return true; }

    std::unique_ptr<wxChoicebook> m_choicebook;

    wxDECLARE_NO_COPY_CLASS(ChoicebookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(ChoicebookTestCase, "Choicebook",
                       "[choicebook][book]");

ChoicebookTestCase::ChoicebookTestCase()
{
    m_choicebook = make_unique<wxChoicebook>(
        wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}


TEST_CASE_METHOD(ChoicebookTestCase, "Choicebook::Choice", "[choicebook]")
{
    wxChoice* choice = m_choicebook->GetChoiceCtrl();

    CHECK(choice);
    CHECK(choice->GetCount() == 3);
    CHECK(choice->GetString(0) == "Panel 1");
}

#endif //wxUSE_CHOICEBOOK
