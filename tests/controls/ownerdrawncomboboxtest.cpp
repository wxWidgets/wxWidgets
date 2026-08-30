///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/ownerdrawncomboboxtest.cpp
// Purpose:     OwnerDrawnComboBox unit test
// Author:      Jaakko Salli
// Created:     2010-12-17
// Copyright:   (c) 2010 Jaakko Salli
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_ODCOMBOBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/odcombo.h"

#include "textentrytest.h"
#include "itemcontainertest.h"
#include "testableframe.h"

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class OwnerDrawnComboBoxTestCase : public TextEntryTestCase,
                                   public ItemContainerTestCase
{
public:
    OwnerDrawnComboBoxTestCase();

protected:
    virtual wxTextEntry *GetTestEntry() const override
    { return m_combo.get(); }
    virtual wxWindow *GetTestWindow() const override { return m_combo.get(); }

    virtual wxItemContainer *GetContainer() const override
    { return m_combo.get(); }
    virtual wxWindow *GetContainerWindow() const override
    { return m_combo.get(); }

    virtual void CheckStringSelection(const char * WXUNUSED(sel)) override
    {
        // do nothing here, as explained in TextEntryTestCase comment, our
        // GetStringSelection() is the wxChoice, not wxTextEntry, one and there
        // is no way to return the selection contents directly
    }

    std::unique_ptr<wxOwnerDrawnComboBox> m_combo;

    wxDECLARE_NO_COPY_CLASS(OwnerDrawnComboBoxTestCase);
};

wxTEXT_ENTRY_TESTS(OwnerDrawnComboBoxTestCase, "OwnerDrawnComboBox",
                   "[ownerdrawncombobox][text-entry]");

wxITEM_CONTAINER_TESTS(OwnerDrawnComboBoxTestCase, "OwnerDrawnComboBox",
                       "[ownerdrawncombobox][item-container]");

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

OwnerDrawnComboBoxTestCase::OwnerDrawnComboBoxTestCase()
{
    m_combo = make_unique<wxOwnerDrawnComboBox>(wxTheApp->GetTopWindow(),
                                                wxID_ANY);
}


// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(OwnerDrawnComboBoxTestCase, "OwnerDrawnComboBox::Size",
                 "[ownerdrawncombobox]")
{
    // under MSW changing combobox size is a non-trivial operation because of
    // confusion between the size of the control with and without dropdown, so
    // check that it does work as expected

    const int heightOrig = m_combo->GetSize().y;

    // check that the height doesn't change if we don't touch it
    m_combo->SetSize(100, -1);
    CHECK( m_combo->GetSize().y == heightOrig );

    // check that setting both big and small (but not too small, there is a
    // limit on how small the control can become under MSW) heights works
    m_combo->SetSize(-1, 50);
    CHECK( m_combo->GetSize().y == 50 );

    m_combo->SetSize(-1, 10);
    CHECK( m_combo->GetSize().y == 10 );

    // and also that restoring it works (this used to be broken before 2.9.1)
    m_combo->SetSize(-1, heightOrig);
    CHECK( m_combo->GetSize().y == heightOrig );
}

TEST_CASE_METHOD(OwnerDrawnComboBoxTestCase, "OwnerDrawnComboBox::PopDismiss",
                 "[ownerdrawncombobox]")
{
    EventCounter drop(m_combo.get(), wxEVT_COMBOBOX_DROPDOWN);
    EventCounter close(m_combo.get(), wxEVT_COMBOBOX_CLOSEUP);

    m_combo->Popup();
    m_combo->Dismiss();

    CHECK(drop.GetCount() == 1);
    CHECK(close.GetCount() == 1);
}

TEST_CASE_METHOD(OwnerDrawnComboBoxTestCase, "OwnerDrawnComboBox::Sort",
                 "[ownerdrawncombobox]")
{
    m_combo = make_unique<wxOwnerDrawnComboBox>(wxTheApp->GetTopWindow(),
                                                wxID_ANY, "",
                                                wxDefaultPosition,
                                                wxDefaultSize, 0, nullptr,
                                                wxCB_SORT);

    m_combo->Append("aaa");
    m_combo->Append("Aaa");
    m_combo->Append("aba");
    m_combo->Append("aaab");
    m_combo->Append("aab");
    m_combo->Append("AAA");

    CHECK(m_combo->GetString(0) == "AAA");
    CHECK(m_combo->GetString(1) == "Aaa");
    CHECK(m_combo->GetString(2) == "aaa");
    CHECK(m_combo->GetString(3) == "aaab");
    CHECK(m_combo->GetString(4) == "aab");
    CHECK(m_combo->GetString(5) == "aba");

    m_combo->Append("a");

    CHECK(m_combo->GetString(0) == "a");
}

TEST_CASE_METHOD(OwnerDrawnComboBoxTestCase, "OwnerDrawnComboBox::ReadOnly",
                 "[ownerdrawncombobox]")
{
    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_combo = make_unique<wxOwnerDrawnComboBox>(wxTheApp->GetTopWindow(),
                                                wxID_ANY, "",
                                                wxDefaultPosition,
                                                wxDefaultSize, testitems,
                                                wxCB_READONLY);

    m_combo->SetValue("item 1");

    CHECK(m_combo->GetValue() == "item 1");

    m_combo->SetValue("not an item");

    CHECK(m_combo->GetValue() == "item 1");

    // Since this uses FindString it is case insensitive
    m_combo->SetValue("ITEM 2");

    CHECK(m_combo->GetValue() == "item 2");
}

#endif // wxUSE_ODCOMBOBOX
