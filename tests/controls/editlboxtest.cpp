/////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/editlboxtest.cpp
// Purpose:     Public model and command contracts of wxEditableListBox
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_EDITABLELISTBOX && wxUSE_LISTCTRL && wxUSE_BMPBUTTON

#include "wx/app.h"
#include "wx/bmpbuttn.h"
#include "wx/editlbox.h"
#include "wx/listctrl.h"
#include "wx/utils.h"
#include "testableframe.h"

namespace
{
class EditableListFixture
{
public:
    EditableListFixture()
        : control(new wxEditableListBox(wxTheApp->GetTopWindow(), wxID_ANY,
                    "Editable items", wxDefaultPosition, wxSize(420, 240)))
    {
        control->SetStrings(wxArrayString{"Alpha", "Beta", "Gamma"});
    }

    ~EditableListFixture() { DeleteTestWindow(control); }

    wxArrayString Strings() const
    {
        wxArrayString result;
        control->GetStrings(result);
        return result;
    }

    void Select(long index)
    {
        REQUIRE(control->GetListCtrl()->SetItemState(
            index, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED));
        wxYield();
    }

    // Exercise the public button's command route without injecting physical
    // input. The WinUI-specific suite separately invokes real native peers.
    void Command(wxBitmapButton* button)
    {
        REQUIRE(button);
        REQUIRE(button->IsEnabled());
        wxCommandEvent event(wxEVT_BUTTON, button->GetId());
        event.SetEventObject(button);
        REQUIRE(button->GetEventHandler()->ProcessEvent(event));
        wxYield();
    }

    wxEditableListBox* control;
};
}

TEST_CASE("EditableListBox::StringsKeepOneEmptyInsertionRow", "[editablelist]")
{
    EditableListFixture fixture;
    auto* const list = fixture.control->GetListCtrl();
    REQUIRE(list);
    CHECK((fixture.Strings() == wxArrayString{"Alpha", "Beta", "Gamma"}));
    REQUIRE(list->GetItemCount() == 4);
    CHECK(list->GetItemText(3).empty());

    fixture.control->SetStrings(wxArrayString{});
    CHECK(fixture.Strings().empty());
    REQUIRE(list->GetItemCount() == 1);
    CHECK(list->GetItemText(0).empty());
    CHECK_FALSE(fixture.control->GetDelButton()->IsEnabled());
    CHECK_FALSE(fixture.control->GetUpButton()->IsEnabled());
    CHECK_FALSE(fixture.control->GetDownButton()->IsEnabled());

    fixture.control->SetStrings(wxArrayString{"replacement"});
    CHECK((fixture.Strings() == wxArrayString{"replacement"}));
    REQUIRE(list->GetItemCount() == 2);
    CHECK(list->GetItemText(1).empty());
}

TEST_CASE("EditableListBox::CommandsPreserveInsertionRowAndItemData", "[editablelist]")
{
    EditableListFixture fixture;
    auto* const list = fixture.control->GetListCtrl();
    REQUIRE(list->SetItemData(0, 10));
    REQUIRE(list->SetItemData(1, 20));
    REQUIRE(list->SetItemData(2, 30));
    fixture.Select(1);
    fixture.Command(fixture.control->GetUpButton());
    CHECK((fixture.Strings() == wxArrayString{"Beta", "Alpha", "Gamma"}));
    CHECK(list->GetItemData(0) == 20);
    CHECK(list->GetItemData(1) == 10);
    CHECK_FALSE(fixture.control->GetUpButton()->IsEnabled());

    fixture.Command(fixture.control->GetDownButton());
    CHECK((fixture.Strings() == wxArrayString{"Alpha", "Beta", "Gamma"}));
    CHECK(list->GetItemData(0) == 10);
    CHECK(list->GetItemData(1) == 20);
    fixture.Command(fixture.control->GetDelButton());
    CHECK((fixture.Strings() == wxArrayString{"Alpha", "Gamma"}));
    REQUIRE(list->GetItemCount() == 3);
    CHECK(list->GetItemText(2).empty());
    CHECK(list->GetItemData(1) == 30);
    fixture.Select(2);
    CHECK_FALSE(fixture.control->GetDelButton()->IsEnabled());
    CHECK_FALSE(fixture.control->GetUpButton()->IsEnabled());
    CHECK_FALSE(fixture.control->GetDownButton()->IsEnabled());
}

TEST_CASE("EditableListBox::ProgrammaticSelectionReplacesSingleSelectedRow",
          "[editablelist][listctrl]")
{
    EditableListFixture fixture;
    auto* const list = fixture.control->GetListCtrl();
    REQUIRE(list->HasFlag(wxLC_SINGLE_SEL));

    fixture.Select(1);
    CHECK(list->GetSelectedItemCount() == 1);
    CHECK_FALSE(list->GetItemState(0, wxLIST_STATE_SELECTED));
    CHECK(list->GetItemState(1, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    CHECK(fixture.control->GetUpButton()->IsEnabled());

    fixture.Select(0);
    CHECK(list->GetSelectedItemCount() == 1);
    CHECK(list->GetItemState(0, wxLIST_STATE_SELECTED) == wxLIST_STATE_SELECTED);
    CHECK_FALSE(list->GetItemState(1, wxLIST_STATE_SELECTED));
    CHECK_FALSE(fixture.control->GetUpButton()->IsEnabled());
    CHECK((fixture.Strings() == wxArrayString{"Alpha", "Beta", "Gamma"}));
}

#endif // wxUSE_EDITABLELISTBOX && wxUSE_LISTCTRL && wxUSE_BMPBUTTON
