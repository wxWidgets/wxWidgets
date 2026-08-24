///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/checklistlistbox.cpp
// Purpose:     wxCheckListBox unit test
// Author:      Steven Lamerton
// Created:     2010-06-30
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_CHECKLISTBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/checklst.h"
#endif // WX_PRECOMP

#include "itemcontainertest.h"
#include "testableframe.h"

#include <memory>

class CheckListBoxTestCase : public ItemContainerTestCase
{
public:
    CheckListBoxTestCase();

protected:
    virtual wxItemContainer *GetContainer() const override
    { return m_check.get(); }
    virtual wxWindow *GetContainerWindow() const override
    { return m_check.get(); }

    std::unique_ptr<wxCheckListBox> m_check;

    wxDECLARE_NO_COPY_CLASS(CheckListBoxTestCase);
};

wxITEM_CONTAINER_TESTS(CheckListBoxTestCase, "CheckListBox",
                       "[checklistbox][item-container]");

CheckListBoxTestCase::CheckListBoxTestCase()
{
    m_check = make_unique<wxCheckListBox>(wxTheApp->GetTopWindow(), wxID_ANY);
}


TEST_CASE_METHOD(CheckListBoxTestCase, "CheckListBox::Check", "[checklistbox]")
{
    EventCounter toggled(m_check.get(), wxEVT_CHECKLISTBOX);

    wxArrayInt checkedItems;
    wxArrayString testitems;
    testitems.Add("item 0");
    testitems.Add("item 1");
    testitems.Add("item 2");
    testitems.Add("item 3");

    m_check->Append(testitems);

    m_check->Check(0);
    m_check->Check(1);
    m_check->Check(1, false);

    //We should not get any events when changing this from code
    CHECK(toggled.GetCount() == 0);
    CHECK(m_check->IsChecked(0) == true);
    CHECK(m_check->IsChecked(1) == false);

    CHECK(m_check->GetCheckedItems(checkedItems) == 1);
    CHECK(checkedItems[0] == 0);

    //Make sure a double check of an items doesn't deselect it
    m_check->Check(0);

    CHECK(m_check->IsChecked(0) == true);
}

#endif // wxUSE_CHECKLISTBOX
