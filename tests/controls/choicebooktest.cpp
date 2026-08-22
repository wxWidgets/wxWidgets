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

class ChoicebookTestCase : public BookCtrlBaseTestCase, public CppUnit::TestCase
{
public:
    ChoicebookTestCase() { }

    virtual void setUp() override;
    virtual void tearDown() override;

private:
    virtual wxBookCtrlBase *GetBase() const override { return m_choicebook; }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_CHOICEBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_CHOICEBOOK_PAGE_CHANGING; }

    virtual bool HasBrokenMnemonics() const override { return true; }

    CPPUNIT_TEST_SUITE( ChoicebookTestCase );
        wxBOOK_CTRL_BASE_TESTS();
        CPPUNIT_TEST( Choice );
        CPPUNIT_TEST( ControllerInsertFailureRollsBackOwnership );
        CPPUNIT_TEST( DeleteAllKeepsModelsCoherentDuringControllerRemoval );
    CPPUNIT_TEST_SUITE_END();

    void Choice();
    void ControllerInsertFailureRollsBackOwnership();
    void DeleteAllKeepsModelsCoherentDuringControllerRemoval();

    ChoicebookForTesting *m_choicebook;

    wxDECLARE_NO_COPY_CLASS(ChoicebookTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ChoicebookTestCase );

// also include in its own registry so that these tests can be run alone
wxREGISTER_UNIT_TEST_WITH_TAGS(
    ChoicebookTestCase,
    "[ChoicebookTestCase][winui-v0-supported]");

void ChoicebookTestCase::setUp()
{
    m_choicebook = new ChoicebookForTesting(
        wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}

void ChoicebookTestCase::tearDown()
{
    wxDELETE(m_choicebook);
}

void ChoicebookTestCase::Choice()
{
    wxChoice* choice = m_choicebook->GetChoiceCtrl();

    CPPUNIT_ASSERT(choice);
    CPPUNIT_ASSERT_EQUAL(3, choice->GetCount());
    CPPUNIT_ASSERT_EQUAL("Panel 1", choice->GetString(0));
}

void ChoicebookTestCase::ControllerInsertFailureRollsBackOwnership()
{
    ChoicebookForTesting* const book = new ChoicebookForTesting(
        wxTheApp->GetTopWindow(), wxID_ANY);
    book->InstallFailingController();
    wxPanel* const candidate = new wxPanel(book);

    const bool inserted =
        book->InsertPage(0, candidate, "failing insertion");

    CPPUNIT_ASSERT(!inserted);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), book->GetPageCount());
    CPPUNIT_ASSERT(book->FindPage(candidate) == wxNOT_FOUND);
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0),
                         book->GetChoiceCtrl()->GetCount());

    delete candidate;
    delete book;
}

void ChoicebookTestCase::DeleteAllKeepsModelsCoherentDuringControllerRemoval()
{
    auto* const book = new ChoicebookForTesting(
        wxTheApp->GetTopWindow(), wxID_ANY);
    ReentrantDeleteChoice* const choice =
        book->InstallReentrantDeleteController();
    CPPUNIT_ASSERT(book->AddPage(new wxPanel(book), "One", true));
    CPPUNIT_ASSERT(book->AddPage(new wxPanel(book), "Two"));

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

    CPPUNIT_ASSERT(book->DeleteAllPages());
    choice->SetDeleteHook({});

    CPPUNIT_ASSERT(callbackSeen);
    // The existing single-page removal slot keeps both models at the old
    // topology while the controller callback is running, then commits them
    // together. The global DeleteAll transaction still rejects all writers.
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(2), modelCount);
    CPPUNIT_ASSERT(!insertResult);
    CPPUNIT_ASSERT(!removeResult);
    CPPUNIT_ASSERT(!nestedDeleteResult);
    CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(0), book->GetPageCount());
    CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), choice->GetCount());

    delete candidate;
    delete book;
}

#endif //wxUSE_CHOICEBOOK
