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
#include "wx/choice.h"
#include "wx/sizer.h"
#include "bookctrlbasetest.h"

#include <functional>
#include <memory>
#include <utility>

class FailingInsertChoice final : public wxChoice
{
public:
    using wxChoice::wxChoice;

protected:
    int DoInsertItems(const wxArrayStringsAdapter& WXUNUSED(items),
                      unsigned int WXUNUSED(pos),
                      void **WXUNUSED(clientData),
                      wxClientDataType WXUNUSED(type)) override
    {
        return wxNOT_FOUND;
    }
};

class ReentrantDeleteChoice final : public wxChoice
{
public:
    using wxChoice::wxChoice;

    void SetDeleteHook(std::function<void()> hook)
    {
        m_hook = std::move(hook);
    }

protected:
    void DoDeleteOneItem(unsigned int pos) override
    {
        if ( m_hook )
            m_hook();
        wxChoice::DoDeleteOneItem(pos);
    }

private:
    std::function<void()> m_hook;
};

class ChoicebookForTesting final : public wxChoicebook
{
public:
    using wxChoicebook::wxChoicebook;

    void InstallFailingController()
    {
        wxChoice* const oldChoice = GetChoiceCtrl();
        m_controlSizer->Detach(oldChoice);
        delete oldChoice;

        m_bookctrl = new FailingInsertChoice(
            this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        m_controlSizer->Add(m_bookctrl, wxSizerFlags(1).Expand());
    }

    ReentrantDeleteChoice* InstallReentrantDeleteController()
    {
        wxChoice* const oldChoice = GetChoiceCtrl();
        m_controlSizer->Detach(oldChoice);
        delete oldChoice;

        auto* const choice = new ReentrantDeleteChoice(
            this, wxID_ANY, wxDefaultPosition, wxDefaultSize);
        m_bookctrl = choice;
        m_controlSizer->Add(m_bookctrl, wxSizerFlags(1).Expand());
        return choice;
    }
};

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

    std::unique_ptr<ChoicebookForTesting> m_choicebook;

    wxDECLARE_NO_COPY_CLASS(ChoicebookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(ChoicebookTestCase, "Choicebook",
                       "[choicebook][book][ChoicebookTestCase][winui-v0-supported]");

ChoicebookTestCase::ChoicebookTestCase()
{
    m_choicebook = make_unique<ChoicebookForTesting>(
        wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}


TEST_CASE_METHOD(ChoicebookTestCase, "Choicebook::Choice",
                 "[choicebook][ChoicebookTestCase][winui-v0-supported]")
{
    wxChoice* choice = m_choicebook->GetChoiceCtrl();

    CHECK(choice);
    CHECK(choice->GetCount() == 3);
    CHECK(choice->GetString(0) == "Panel 1");
}

TEST_CASE_METHOD(ChoicebookTestCase, "Choicebook::ControllerInsertFailureRollsBackOwnership",
                 "[choicebook][ChoicebookTestCase][winui-v0-supported]")
{
    ChoicebookForTesting* const book = new ChoicebookForTesting(
        wxTheApp->GetTopWindow(), wxID_ANY);
    book->InstallFailingController();
    wxPanel* const candidate = new wxPanel(book);

    const bool inserted =
        book->InsertPage(0, candidate, "failing insertion");

    REQUIRE(!inserted);
    REQUIRE(book->GetPageCount() == static_cast<size_t>(0));
    REQUIRE(book->FindPage(candidate) == wxNOT_FOUND);
    REQUIRE(book->GetChoiceCtrl()->GetCount() == static_cast<unsigned int>(0));

    delete candidate;
    delete book;
}

TEST_CASE_METHOD(ChoicebookTestCase, "Choicebook::DeleteAllKeepsModelsCoherentDuringControllerRemoval",
                 "[choicebook][ChoicebookTestCase][winui-v0-supported]")
{
    auto* const book = new ChoicebookForTesting(
        wxTheApp->GetTopWindow(), wxID_ANY);
    ReentrantDeleteChoice* const choice =
        book->InstallReentrantDeleteController();
    REQUIRE(book->AddPage(new wxPanel(book), "One", true));
    REQUIRE(book->AddPage(new wxPanel(book), "Two"));

    wxPanel* const candidate = new wxPanel(book);
    bool callbackSeen = false;
    bool insertResult = true;
    bool removeResult = true;
    bool nestedDeleteResult = true;
    size_t modelCount = 99;
    choice->SetDeleteHook(
        [=, &callbackSeen, &insertResult, &removeResult,
            &nestedDeleteResult, &modelCount]()
        {
            if ( callbackSeen )
                return;

            callbackSeen = true;
            modelCount = book->GetPageCount();
            insertResult = book->AddPage(candidate, "nested");
            removeResult = book->RemovePage(0);
            nestedDeleteResult = book->DeleteAllPages();
        });

    REQUIRE(book->DeleteAllPages());
    choice->SetDeleteHook({});

    REQUIRE(callbackSeen);
    // The existing single-page removal slot keeps both models at the old
    // topology while the controller callback is running, then commits them
    // together. The global DeleteAll transaction still rejects all writers.
    REQUIRE(modelCount == static_cast<size_t>(2));
    REQUIRE(!insertResult);
    REQUIRE(!removeResult);
    REQUIRE(!nestedDeleteResult);
    REQUIRE(book->GetPageCount() == static_cast<size_t>(0));
    REQUIRE(choice->GetCount() == static_cast<unsigned int>(0));

    delete candidate;
    delete book;
}

#endif //wxUSE_CHOICEBOOK
