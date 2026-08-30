///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/htmllboxtest.cpp
// Purpose:     wxSimpleHtmlListBoxNameStr unit test
// Author:      Vadim Zeitlin
// Created:     2010-11-27
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_HTML


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/filename.h"
#include "wx/filesys.h"
#include "wx/htmllbox.h"
#include "wx/html/htmlcell.h"
#include "wx/image.h"
#include "testfile.h"
#include "itemcontainertest.h"
#include "testableframe.h"

namespace
{

class CountingClientData final : public wxClientData
{
public:
    CountingClientData(const wxString& value, int& destroyed)
        : m_value(value),
          m_destroyed(destroyed)
    {
    }

    ~CountingClientData() override
    {
        ++m_destroyed;
    }

    const wxString& GetValue() const { return m_value; }

private:
    wxString m_value;
    int& m_destroyed;
};

class ReentrantHtmlListBox final : public wxHtmlListBox
{
public:
    explicit ReentrantHtmlListBox(wxWindow* parent)
        : wxHtmlListBox(parent, wxID_ANY,
                        wxDefaultPosition, wxSize(180, 90))
    {
    }

    void ArmClearOnMarkup() const { m_clearOnMarkup = true; }
    bool HadOutOfRangeMarkup() const { return m_outOfRangeMarkup; }

protected:
    wxString OnGetItem(size_t item) const override
    {
        if ( item >= GetItemCount() )
            m_outOfRangeMarkup = true;

        if ( m_clearOnMarkup )
        {
            m_clearOnMarkup = false;
            const_cast<ReentrantHtmlListBox *>(this)->SetItemCount(0);
        }

        return wxString::Format("<b>item %lu</b>",
                                static_cast<unsigned long>(item));
    }

private:
    mutable bool m_clearOnMarkup{false};
    mutable bool m_outOfRangeMarkup{false};
};

} // anonymous namespace

#include <memory>

class HtmlListBoxTestCase : public ItemContainerTestCase
{
public:
    HtmlListBoxTestCase();

protected:
    virtual wxItemContainer *GetContainer() const override
    { return m_htmllbox.get(); }
    virtual wxWindow *GetContainerWindow() const override
    { return m_htmllbox.get(); }

    std::unique_ptr<wxSimpleHtmlListBox> m_htmllbox;

    wxDECLARE_NO_COPY_CLASS(HtmlListBoxTestCase);
};

wxITEM_CONTAINER_TESTS(HtmlListBoxTestCase, "HtmlListBox",
                       "[htmllistbox][item-container]");

HtmlListBoxTestCase::HtmlListBoxTestCase()
{
    m_htmllbox = make_unique<wxSimpleHtmlListBox>(wxTheApp->GetTopWindow(),
                                                  wxID_ANY);
}


TEST_CASE_METHOD(HtmlListBoxTestCase, "HtmlListBox::MarkupGeometryAndMutation", "[htmllistbox]")
{
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append("<b>short</b>");
    m_htmllbox->Append("line 1<br>line 2<br>line 3");

    const wxRect first = m_htmllbox->GetItemRect(0);
    const wxRect second = m_htmllbox->GetItemRect(1);
    REQUIRE( !first.IsEmpty() );
    REQUIRE( !second.IsEmpty() );
    REQUIRE( second.height > first.height );
    REQUIRE( (m_htmllbox->VirtualHitTest(first.GetTop() + first.height / 2)) == (0) );
    REQUIRE( (m_htmllbox->VirtualHitTest(second.GetTop() + second.height / 2)) == (1) );

    m_htmllbox->SetSelection(1);
    m_htmllbox->SetString(1, "<i>replacement</i>");
    REQUIRE( (m_htmllbox->GetString(1)) == (wxString("<i>replacement</i>")) );

    m_htmllbox->Delete(0);
    REQUIRE( (m_htmllbox->GetCount()) == (1u) );
    REQUIRE( (m_htmllbox->GetString(0)) == (wxString("<i>replacement</i>")) );
}

TEST_CASE_METHOD(HtmlListBoxTestCase, "HtmlListBox::LinkAndSelectionRouting", "[htmllistbox]")
{
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append("<a href=\"local://target\">link</a>");
    m_htmllbox->Append("plain");

    EventCounter links(m_htmllbox.get(), wxEVT_HTML_LINK_CLICKED);
    EventCounter selections(m_htmllbox.get(), wxEVT_LISTBOX);
    EventCounter contexts(m_htmllbox.get(), wxEVT_CONTEXT_MENU);

    const wxRect linkRect = m_htmllbox->GetItemRect(0);
    REQUIRE( !linkRect.IsEmpty() );
    wxMouseEvent linkClick(wxEVT_LEFT_DOWN);
    linkClick.m_x = linkRect.x + 5;
    linkClick.m_y = linkRect.y + linkRect.height / 2;
    m_htmllbox->GetEventHandler()->ProcessEvent(linkClick);

    REQUIRE( (links.GetCount()) == (1) );
    REQUIRE( (selections.GetCount()) == (0) );
    REQUIRE( (m_htmllbox->GetSelection()) == (wxNOT_FOUND) );

    const wxRect plainRect = m_htmllbox->GetItemRect(1);
    REQUIRE( !plainRect.IsEmpty() );
    wxMouseEvent plainClick(wxEVT_LEFT_DOWN);
    plainClick.m_x = plainRect.x + 5;
    plainClick.m_y = plainRect.y + plainRect.height / 2;
    m_htmllbox->GetEventHandler()->ProcessEvent(plainClick);

    REQUIRE( (links.GetCount()) == (1) );
    REQUIRE( (selections.GetCount()) == (1) );
    REQUIRE( (m_htmllbox->GetSelection()) == (1) );

    wxContextMenuEvent context(wxEVT_CONTEXT_MENU, m_htmllbox->GetId(),
                               plainRect.GetPosition());
    context.SetEventObject(m_htmllbox.get());
    m_htmllbox->GetEventHandler()->ProcessEvent(context);
    REQUIRE( (contexts.GetCount()) == (1) );
}

TEST_CASE_METHOD(HtmlListBoxTestCase, "HtmlListBox::CellEventMutationIsSafe", "[htmllistbox]")
{
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append("<a href=\"local://cleared\">clear me</a>");

    int cellEvents = 0;
    EventCounter links(m_htmllbox.get(), wxEVT_HTML_LINK_CLICKED);
    EventCounter selections(m_htmllbox.get(), wxEVT_LISTBOX);
    m_htmllbox->Bind(wxEVT_HTML_CELL_CLICKED,
                     [this, &cellEvents](wxHtmlCellEvent& event)
    {
        ++cellEvents;
        m_htmllbox->Clear();

        // Request normal HTML processing after invalidating the exact cell
        // passed in the event. The control must detect this invalidation.
        event.Skip();
    });

    const wxRect rect = m_htmllbox->GetItemRect(0);
    REQUIRE( !rect.IsEmpty() );

    wxMouseEvent click(wxEVT_LEFT_DOWN);
    click.m_x = rect.x + 5;
    click.m_y = rect.y + rect.height / 2;
    m_htmllbox->GetEventHandler()->ProcessEvent(click);

    REQUIRE( (cellEvents) == (1) );
    REQUIRE( (m_htmllbox->GetCount()) == (0u) );
    REQUIRE( (links.GetCount()) == (0) );
    REQUIRE( (selections.GetCount()) == (0) );
    REQUIRE( (m_htmllbox->GetSelection()) == (wxNOT_FOUND) );
}

TEST_CASE_METHOD(HtmlListBoxTestCase, "HtmlListBox::ReentrantMarkupMutation", "[htmllistbox]")
{
    ReentrantHtmlListBox list(wxTheApp->GetTopWindow());
    list.SetItemCount(20);
    list.ArmClearOnMarkup();

    REQUIRE( list.GetItemRect(0).IsEmpty() );
    REQUIRE( (list.GetItemCount()) == (static_cast<size_t>(0)) );
    REQUIRE( !list.HadOutOfRangeMarkup() );
}

TEST_CASE_METHOD(HtmlListBoxTestCase, "HtmlListBox::LocalImageAndClientDataLifetime", "[htmllistbox]")
{
    TestFile imageFile;
    wxImage image(8, 8);
    image.SetRGB(wxRect(0, 0, 8, 8), 0, 80, 160);
    // BMP is a standard handler, even when this case runs alone or optional
    // PNG support is disabled. Don't depend on a prior image test to install
    // handlers (or on an image next to the executable).
    REQUIRE( image.SaveFile(imageFile.GetName(), wxBITMAP_TYPE_BMP) );

    const wxString imageUrl = wxFileSystem::FileNameToURL(
        wxFileName(imageFile.GetName()));
    m_htmllbox->SetSize(240, 120);
    m_htmllbox->Append(
        wxString::Format("<img src=\"%s\"> local image", imageUrl));

    const wxRect imageRect = m_htmllbox->GetItemRect(0);
    REQUIRE( !imageRect.IsEmpty() );
    REQUIRE( imageRect.height > 4 );

    m_htmllbox->Delete(0);
    wxYield();
    REQUIRE( (m_htmllbox->GetCount()) == (0u) );

    int destroyed = 0;
    m_htmllbox->Append(
        "zero", new CountingClientData("zero", destroyed));
    m_htmllbox->Append(
        "one", new CountingClientData("one", destroyed));
    m_htmllbox->Append(
        "two", new CountingClientData("two", destroyed));

    m_htmllbox->Delete(1);
    REQUIRE( (destroyed) == (1) );
    REQUIRE( (static_cast<CountingClientData *>(
            m_htmllbox->GetClientObject(0))->GetValue()) == (wxString("zero")) );
    REQUIRE( (static_cast<CountingClientData *>(
            m_htmllbox->GetClientObject(1))->GetValue()) == (wxString("two")) );

    m_htmllbox->Clear();
    REQUIRE( (destroyed) == (3) );
}

TEST_CASE_METHOD(HtmlListBoxTestCase, "HtmlListBox::LargeLocalModelAndLifetime", "[htmllistbox]")
{
    wxArrayString items;
    items.Alloc(10000);
    for ( int i = 0; i < 10000; ++i )
        items.push_back(wxString::Format("<b>row %d</b>", i));

    m_htmllbox->Append(items);
    REQUIRE( (m_htmllbox->GetCount()) == (10000u) );
    REQUIRE( m_htmllbox->ScrollToRow(9999) );
    REQUIRE( m_htmllbox->IsRowVisible(9999) );

    wxWindow* const top = wxTheApp->GetTopWindow();
    wxPanel* const parent1 = new wxPanel(top);
    wxPanel* const parent2 = new wxPanel(top);
    wxSimpleHtmlListBox* const twoStep = new wxSimpleHtmlListBox;
    REQUIRE( twoStep->Create(parent1, wxID_ANY,
                                    wxDefaultPosition, wxSize(160, 90)) );
    twoStep->Append("local");
    REQUIRE( twoStep->Reparent(parent2) );
    delete twoStep;

    for ( int i = 0; i < 100; ++i )
    {
        wxSimpleHtmlListBox* const list =
            new wxSimpleHtmlListBox(parent1, wxID_ANY);
        list->Append("<b>local</b>");
        delete list;
    }

    delete parent2;
    delete parent1;
}

#endif //wxUSE_HTML
