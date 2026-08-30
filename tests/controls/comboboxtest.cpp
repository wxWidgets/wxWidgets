///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/comboboxtest.cpp
// Purpose:     wxComboBox unit test
// Author:      Vadim Zeitlin
// Created:     2007-09-25
// Copyright:   (c) 2007 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_COMBOBOX


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/combobox.h"
#endif // WX_PRECOMP

#include "textentrytest.h"
#include "itemcontainertest.h"
#include "testableframe.h"

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class ComboBoxTestCase : public TextEntryTestCase, public ItemContainerTestCase
{
public:
    ComboBoxTestCase();

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

    std::unique_ptr<wxComboBox> m_combo;

    wxDECLARE_NO_COPY_CLASS(ComboBoxTestCase);
};

wxITEM_CONTAINER_TESTS(ComboBoxTestCase, "ComboBox",
                       "[combobox][item-container]");

#ifdef __WXOSX__

// Under macOS the Editable test only works interactively and WriteText() is
// not implemented, so run all the other wxTextEntry tests only.
#define wxCOMBOBOX_TEXT_ENTRY_TEST(name) \
    wxTEXT_ENTRY_TEST_CASE(ComboBoxTestCase, "ComboBox", name, \
                           "[combobox][text-entry]")

wxCOMBOBOX_TEXT_ENTRY_TEST(SetValue)
wxCOMBOBOX_TEXT_ENTRY_TEST(TextChangeEvents)
wxCOMBOBOX_TEXT_ENTRY_TEST(Selection)
wxCOMBOBOX_TEXT_ENTRY_TEST(InsertionPoint)
wxCOMBOBOX_TEXT_ENTRY_TEST(Replace)
wxCOMBOBOX_TEXT_ENTRY_TEST(Hint)
wxCOMBOBOX_TEXT_ENTRY_TEST(CopyPaste)
wxCOMBOBOX_TEXT_ENTRY_TEST(UndoRedo)

#undef wxCOMBOBOX_TEXT_ENTRY_TEST

#else // !__WXOSX__

wxTEXT_ENTRY_TESTS(ComboBoxTestCase, "ComboBox",
                   "[combobox][text-entry]");

#endif // __WXOSX__/!__WXOSX__

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

ComboBoxTestCase::ComboBoxTestCase()
{
    m_combo = make_unique<wxComboBox>(wxTheApp->GetTopWindow(), wxID_ANY);
}


// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(ComboBoxTestCase, "ComboBox::Size", "[combobox]")
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

TEST_CASE_METHOD(ComboBoxTestCase, "ComboBox::PopDismiss", "[combobox]")
{
#if defined(__WXMSW__) || defined(__WXGTK210__) || defined(__WXQT__)
    EventCounter drop(m_combo.get(), wxEVT_COMBOBOX_DROPDOWN);
    EventCounter close(m_combo.get(), wxEVT_COMBOBOX_CLOSEUP);

    m_combo->Popup();
    CHECK(drop.GetCount() == 1);

    m_combo->Dismiss();

#if defined(__WXGTK__) && !defined(__WXGTK3__)
    // Under wxGTK2, the event is sent only during idle time and not
    // immediately, so we need this yield to get it.
    wxYield();
#endif // wxGTK2

    CHECK(close.GetCount() == 1);
#endif
}

TEST_CASE_METHOD(ComboBoxTestCase, "ComboBox::Sort", "[combobox]")
{
#if !defined(__WXOSX__)
    m_combo = make_unique<wxComboBox>(wxTheApp->GetTopWindow(), wxID_ANY, "",
                                      wxDefaultPosition, wxDefaultSize, 0,
                                      nullptr, wxCB_SORT);

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
#endif
}

TEST_CASE_METHOD(ComboBoxTestCase, "ComboBox::ReadOnly", "[combobox]")
{
    wxArrayString testitems;
    testitems.Add("item 1");
    testitems.Add("item 2");

    m_combo = make_unique<wxComboBox>(wxTheApp->GetTopWindow(), wxID_ANY, "",
                                      wxDefaultPosition, wxDefaultSize,
                                      testitems, wxCB_READONLY);

    m_combo->SetValue("item 1");

    CHECK(m_combo->GetValue() == "item 1");

    m_combo->SetValue("not an item");

    CHECK(m_combo->GetValue() == "item 1");

    // Since this uses FindString it is case insensitive
    m_combo->SetValue("ITEM 2");

    CHECK(m_combo->GetValue() == "item 2");
}

TEST_CASE_METHOD(ComboBoxTestCase, "ComboBox::IsEmpty", "[combobox]")
{
    CHECK( m_combo->IsListEmpty() );
    CHECK( m_combo->IsTextEmpty() );

    m_combo->Append("foo");
    CHECK( !m_combo->IsListEmpty() );
    CHECK( m_combo->IsTextEmpty() );

    m_combo->SetValue("bar");
    CHECK( !m_combo->IsListEmpty() );
    CHECK( !m_combo->IsTextEmpty() );

    m_combo->Clear();
    CHECK( m_combo->IsListEmpty() );
    CHECK( m_combo->IsTextEmpty() );

#ifdef TEST_INVALID_COMBOBOX_ISEMPTY
    // Compiling this should fail, see failtest target definition in test.bkl.
    m_combo->IsEmpty();
#endif
}

TEST_CASE_METHOD(ComboBoxTestCase, "ComboBox::SetStringSelection",
                 "[combobox]")
{
    m_combo->Append("foo");
    m_combo->Append("bar");
    m_combo->Append("baz");

    EventCounter events(m_combo.get(), wxEVT_COMBOBOX);
    m_combo->SetStringSelection("bar");
    CHECK( events.GetCount() == 0 );

    m_combo->SetStringSelection("foo");
    CHECK( events.GetCount() == 0 );
}

TEST_CASE("wxComboBox::ProcessEnter", "[wxComboBox][enter]")
{
    class ComboBoxCreator : public TextLikeControlCreator
    {
    public:
        virtual wxControl* Create(wxWindow* parent, int style) const override
        {
            const wxString choices[] = { "foo", "bar", "baz" };

            return new wxComboBox(parent, wxID_ANY, wxString(),
                                  wxDefaultPosition, wxDefaultSize,
                                  WXSIZEOF(choices), choices,
                                  style);
        }
    };

    TestProcessEnter(ComboBoxCreator());
}

#else

#ifdef TEST_INVALID_COMBOBOX_ISEMPTY
#error provoke failing here
#endif

#endif //wxUSE_COMBOBOX
