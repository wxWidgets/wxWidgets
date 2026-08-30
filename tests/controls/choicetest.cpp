///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/choice.cpp
// Purpose:     wxChoice unit test
// Author:      Steven Lamerton
// Created:     2010-06-29
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_CHOICE


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/choice.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"

#include <memory>

class ChoiceTestCase : public ItemContainerTestCase
{
public:
    ChoiceTestCase();

protected:
    virtual wxItemContainer *GetContainer() const override
    { return m_choice.get(); }
    virtual wxWindow *GetContainerWindow() const override
    { return m_choice.get(); }

    std::unique_ptr<wxChoice> m_choice;

    wxDECLARE_NO_COPY_CLASS(ChoiceTestCase);
};

wxITEM_CONTAINER_TESTS(ChoiceTestCase, "Choice",
                       "[choice][item-container]");

ChoiceTestCase::ChoiceTestCase()
{
    m_choice = make_unique<wxChoice>(wxTheApp->GetTopWindow(), wxID_ANY);
}


TEST_CASE_METHOD(ChoiceTestCase, "Choice::Sort", "[choice]")
{
#if !defined(__WXOSX__)
    m_choice = make_unique<wxChoice>(wxTheApp->GetTopWindow(), wxID_ANY,
                                     wxDefaultPosition, wxDefaultSize, 0,
                                     nullptr, wxCB_SORT);

    wxArrayString testitems;
    testitems.Add("aaa");
    testitems.Add("Aaa");
    testitems.Add("aba");
    testitems.Add("aaab");
    testitems.Add("aab");
    testitems.Add("AAA");

    m_choice->Append(testitems);

    CHECK(m_choice->GetString(0) == "AAA");
    CHECK(m_choice->GetString(1) == "Aaa");
    CHECK(m_choice->GetString(2) == "aaa");
    CHECK(m_choice->GetString(3) == "aaab");
    CHECK(m_choice->GetString(4) == "aab");
    CHECK(m_choice->GetString(5) == "aba");

    m_choice->Append("a");

    CHECK(m_choice->GetString(0) == "a");
#endif
}

TEST_CASE_METHOD(ChoiceTestCase, "Choice::GetBestSize", "[choice]")
{
    wxArrayString testitems;
    testitems.Add("1");
    testitems.Add("11");
    m_choice->Append(testitems);

    SECTION("Normal best size")
    {
        // nothing to do here
    }

    // Ensure that the hidden control return a valid best size too.
    SECTION("Hidden best size")
    {
        m_choice->Hide();
    }

    wxYield();

    m_choice->InvalidateBestSize();
    const wxSize bestSize = m_choice->GetBestSize();

    CHECK(bestSize.GetWidth() > m_choice->FromDIP(30));
    CHECK(bestSize.GetWidth() < m_choice->FromDIP(120));
    CHECK(bestSize.GetHeight() > m_choice->FromDIP(15));
    CHECK(bestSize.GetHeight() < m_choice->FromDIP(35));
}

#endif //wxUSE_CHOICE
