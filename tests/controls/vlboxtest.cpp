///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/vlboxtest.cpp
// Purpose:     wxVListBox unit tests
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_LISTBOX

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif

#include "wx/vlbox.h"
#include "wx/weakref.h"
#include "testableframe.h"

#include <initializer_list>
#include <memory>

namespace
{

class TestVListBox final : public wxVListBox
{
public:
    TestVListBox() = default;

    explicit TestVListBox(wxWindow* parent, long style = 0)
        : wxVListBox(parent, wxID_ANY, wxDefaultPosition, wxSize(180, 90),
                     style)
    {
    }

    void ResetMeasureCount() const { m_measureCount = 0; }
    size_t GetMeasureCount() const { return m_measureCount; }
    bool HadOutOfRangeMeasure() const { return m_outOfRangeMeasure; }
    void ArmClearOnMeasure() const { m_clearOnMeasure = true; }

protected:
    void OnDrawItem(wxDC&, const wxRect&, size_t) const override
    {
    }

    wxCoord OnMeasureItem(size_t item) const override
    {
        ++m_measureCount;

        if ( item >= GetItemCount() )
            m_outOfRangeMeasure = true;

        if ( m_clearOnMeasure )
        {
            m_clearOnMeasure = false;
            const_cast<TestVListBox *>(this)->SetItemCount(0);
        }

        return 12 + static_cast<wxCoord>((item % 5) * 3);
    }

private:
    mutable size_t m_measureCount{0};
    mutable bool m_outOfRangeMeasure{false};
    mutable bool m_clearOnMeasure{false};
};

} // anonymous namespace

class VListBoxTestCase
{
public:
    VListBoxTestCase()
    {
        m_list = make_unique<TestVListBox>(wxTheApp->GetTopWindow());
        m_list->SetItemCount(20);
    }

protected:
    std::unique_ptr<TestVListBox> m_list;

    wxDECLARE_NO_COPY_CLASS(VListBoxTestCase);
};

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::GeometryAndScroll",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    REQUIRE( m_list->GetItemCount() == static_cast<size_t>(20) );
    REQUIRE( m_list->GetVisibleRowsBegin() == static_cast<size_t>(0) );

    const wxRect first = m_list->GetItemRect(0);
    const wxRect second = m_list->GetItemRect(1);
    REQUIRE_FALSE( first.IsEmpty() );
    REQUIRE_FALSE( second.IsEmpty() );
    REQUIRE( second.height > first.height );
    REQUIRE( m_list->VirtualHitTest(first.GetTop() + first.height / 2) == 0 );
    REQUIRE( m_list->VirtualHitTest(second.GetTop() + second.height / 2) == 1 );

    REQUIRE( m_list->ScrollToRow(15) );
    REQUIRE( m_list->IsRowVisible(15) );
    REQUIRE( m_list->GetVisibleRowsEnd() <= m_list->GetItemCount() );
}

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::SelectionAndKeyboard",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    m_list->SetSelection(0);
    EventCounter selected(m_list.get(), wxEVT_LISTBOX);

    wxKeyEvent down(wxEVT_KEY_DOWN);
    down.m_keyCode = WXK_DOWN;
    m_list->GetEventHandler()->ProcessEvent(down);

    REQUIRE( m_list->GetSelection() == 1 );
    REQUIRE( selected.GetCount() == 1 );

    m_list->SetItemCount(0);
    REQUIRE( m_list->GetSelection() == wxNOT_FOUND );
    REQUIRE( m_list->GetCurrent() == wxNOT_FOUND );

    for ( const int key : { WXK_HOME, WXK_END, WXK_SPACE, WXK_DOWN } )
    {
        wxKeyEvent emptyKey(wxEVT_KEY_DOWN);
        emptyKey.m_keyCode = key;
        m_list->GetEventHandler()->ProcessEvent(emptyKey);
        REQUIRE( m_list->GetSelection() == wxNOT_FOUND );
        REQUIRE( m_list->GetCurrent() == wxNOT_FOUND );
    }
    REQUIRE( selected.GetCount() == 1 );
}

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::MultipleSelection",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    TestVListBox list(wxTheApp->GetTopWindow(), wxLB_MULTIPLE);
    list.SetItemCount(10);

    REQUIRE( list.SelectRange(2, 5) );
    REQUIRE( list.GetSelectedCount() == static_cast<size_t>(4) );
    REQUIRE( list.IsSelected(2) );
    REQUIRE( list.IsSelected(5) );

    list.Toggle(3);
    REQUIRE_FALSE( list.IsSelected(3) );
    REQUIRE( list.GetSelectedCount() == static_cast<size_t>(3) );
    REQUIRE( list.DeselectAll() );
    REQUIRE( list.GetSelectedCount() == static_cast<size_t>(0) );
}

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::EmptyModelBoundaries",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    m_list->SetItemCount(0);
    m_list->ResetMeasureCount();

    REQUIRE( m_list->VirtualHitTest(-1) == wxNOT_FOUND );
    REQUIRE( m_list->VirtualHitTest(0) == wxNOT_FOUND );
    REQUIRE_FALSE( m_list->ScrollRowPages(-1) );
    REQUIRE( m_list->GetMeasureCount() == static_cast<size_t>(0) );
    REQUIRE_FALSE( m_list->HadOutOfRangeMeasure() );
}

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::ReentrantModelMutation",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    EventCounter selected(m_list.get(), wxEVT_LISTBOX);

    m_list->ArmClearOnMeasure();
    REQUIRE( m_list->GetItemRect(0).IsEmpty() );
    REQUIRE( m_list->GetItemCount() == static_cast<size_t>(0) );
    REQUIRE_FALSE( m_list->HadOutOfRangeMeasure() );

    m_list->SetItemCount(20);
    m_list->ArmClearOnMeasure();

    wxMouseEvent click(wxEVT_LEFT_DOWN);
    click.m_x = 2;
    click.m_y = 2;
    m_list->GetEventHandler()->ProcessEvent(click);

    REQUIRE( m_list->GetItemCount() == static_cast<size_t>(0) );
    REQUIRE( m_list->GetCurrent() == wxNOT_FOUND );
    REQUIRE( m_list->GetSelection() == wxNOT_FOUND );
    REQUIRE( selected.GetCount() == 0 );
    REQUIRE_FALSE( m_list->HadOutOfRangeMeasure() );
}

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::DestroyDuringSizeDispatch",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    auto list = make_unique<TestVListBox>(wxTheApp->GetTopWindow());
    list->SetItemCount(10);

    const wxWeakRef<wxWindow> weakList(list.get());
    list->Bind(wxEVT_SIZE, [&list](wxSizeEvent&)
    {
        TestVListBox* const doomed = list.release();
        delete doomed;
    });

    wxSizeEvent sizeEvent(wxSize(200, 100), list->GetId());
    sizeEvent.SetEventObject(list.get());
    wxEvtHandler* const handler = list->GetEventHandler();
    handler->ProcessEvent(sizeEvent);

    REQUIRE_FALSE( weakList );
    REQUIRE_FALSE( list );
}

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::LargeVirtualModelIsBounded",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    constexpr size_t ItemCount = 100000;
    m_list->SetItemCount(ItemCount);
    m_list->ResetMeasureCount();

    REQUIRE( m_list->ScrollToRow(ItemCount - 1) );
    REQUIRE( m_list->IsRowVisible(ItemCount - 1) );
    REQUIRE( m_list->GetVisibleRowsEnd() <= ItemCount );

    // A virtual list must not materialize or measure its entire model merely
    // to reach its final row.
    REQUIRE( m_list->GetMeasureCount() < ItemCount / 10 );
}

TEST_CASE_METHOD(VListBoxTestCase, "VListBox::DefaultCreateReparentAndLifetime",
                 "[vlistbox][VListBoxTestCase][winui-generic-data]")
{
    wxWindow* const top = wxTheApp->GetTopWindow();
    auto parent1 = make_unique<wxPanel>(top);
    auto parent2 = make_unique<wxPanel>(top);

    auto twoStep = make_unique<TestVListBox>();
    REQUIRE( twoStep->Create(parent1.get(), wxID_ANY,
                             wxDefaultPosition, wxSize(120, 70)) );
    twoStep->SetItemCount(3);
    REQUIRE( twoStep->Reparent(parent2.get()) );
    REQUIRE( twoStep->GetParent() == parent2.get() );
    twoStep.reset();

    for ( int i = 0; i < 100; ++i )
    {
        auto list = make_unique<TestVListBox>(parent1.get());
        list->SetItemCount(100);
        list->ScrollToRow(50);
    }
}

#endif // wxUSE_LISTBOX
