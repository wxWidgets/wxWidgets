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

class VListBoxTestCase : public CppUnit::TestCase
{
public:
    void setUp() override
    {
        m_list = new TestVListBox(wxTheApp->GetTopWindow());
        m_list->SetItemCount(20);
    }

    void tearDown() override
    {
        delete m_list;
        m_list = nullptr;
    }

private:
    CPPUNIT_TEST_SUITE( VListBoxTestCase );
        CPPUNIT_TEST( GeometryAndScroll );
        CPPUNIT_TEST( SelectionAndKeyboard );
        CPPUNIT_TEST( MultipleSelection );
        CPPUNIT_TEST( EmptyModelBoundaries );
        CPPUNIT_TEST( ReentrantModelMutation );
        CPPUNIT_TEST( DestroyDuringSizeDispatch );
        CPPUNIT_TEST( LargeVirtualModelIsBounded );
        CPPUNIT_TEST( DefaultCreateReparentAndLifetime );
    CPPUNIT_TEST_SUITE_END();

    void GeometryAndScroll();
    void SelectionAndKeyboard();
    void MultipleSelection();
    void EmptyModelBoundaries();
    void ReentrantModelMutation();
    void DestroyDuringSizeDispatch();
    void LargeVirtualModelIsBounded();
    void DefaultCreateReparentAndLifetime();

    TestVListBox* m_list{nullptr};
};

wxREGISTER_UNIT_TEST_WITH_TAGS(
    VListBoxTestCase,
    "[VListBoxTestCase][winui-generic-data]");

void VListBoxTestCase::GeometryAndScroll()
{
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(20), m_list->GetItemCount() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0),
                          m_list->GetVisibleRowsBegin() );

    const wxRect first = m_list->GetItemRect(0);
    const wxRect second = m_list->GetItemRect(1);
    CPPUNIT_ASSERT( !first.IsEmpty() );
    CPPUNIT_ASSERT( !second.IsEmpty() );
    CPPUNIT_ASSERT( second.height > first.height );
    CPPUNIT_ASSERT_EQUAL(
        0,
        m_list->VirtualHitTest(first.GetTop() + first.height / 2) );
    CPPUNIT_ASSERT_EQUAL(
        1,
        m_list->VirtualHitTest(second.GetTop() + second.height / 2) );

    CPPUNIT_ASSERT( m_list->ScrollToRow(15) );
    CPPUNIT_ASSERT( m_list->IsRowVisible(15) );
    CPPUNIT_ASSERT( m_list->GetVisibleRowsEnd() <= m_list->GetItemCount() );
}

void VListBoxTestCase::SelectionAndKeyboard()
{
    m_list->SetSelection(0);
    EventCounter selected(m_list, wxEVT_LISTBOX);

    wxKeyEvent down(wxEVT_KEY_DOWN);
    down.m_keyCode = WXK_DOWN;
    m_list->GetEventHandler()->ProcessEvent(down);

    CPPUNIT_ASSERT_EQUAL( 1, m_list->GetSelection() );
    CPPUNIT_ASSERT_EQUAL( 1, selected.GetCount() );

    m_list->SetItemCount(0);
    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->GetSelection() );
    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->GetCurrent() );

    for ( const int key : { WXK_HOME, WXK_END, WXK_SPACE, WXK_DOWN } )
    {
        wxKeyEvent emptyKey(wxEVT_KEY_DOWN);
        emptyKey.m_keyCode = key;
        m_list->GetEventHandler()->ProcessEvent(emptyKey);
        CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->GetSelection() );
        CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->GetCurrent() );
    }
    CPPUNIT_ASSERT_EQUAL( 1, selected.GetCount() );
}

void VListBoxTestCase::MultipleSelection()
{
    TestVListBox list(wxTheApp->GetTopWindow(), wxLB_MULTIPLE);
    list.SetItemCount(10);

    CPPUNIT_ASSERT( list.SelectRange(2, 5) );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(4), list.GetSelectedCount() );
    CPPUNIT_ASSERT( list.IsSelected(2) );
    CPPUNIT_ASSERT( list.IsSelected(5) );

    list.Toggle(3);
    CPPUNIT_ASSERT( !list.IsSelected(3) );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(3), list.GetSelectedCount() );
    CPPUNIT_ASSERT( list.DeselectAll() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0), list.GetSelectedCount() );
}

void VListBoxTestCase::EmptyModelBoundaries()
{
    m_list->SetItemCount(0);
    m_list->ResetMeasureCount();

    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->VirtualHitTest(-1) );
    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->VirtualHitTest(0) );
    CPPUNIT_ASSERT( !m_list->ScrollRowPages(-1) );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0),
                          m_list->GetMeasureCount() );
    CPPUNIT_ASSERT( !m_list->HadOutOfRangeMeasure() );
}

void VListBoxTestCase::ReentrantModelMutation()
{
    EventCounter selected(m_list, wxEVT_LISTBOX);

    m_list->ArmClearOnMeasure();
    CPPUNIT_ASSERT( m_list->GetItemRect(0).IsEmpty() );
    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0),
                          m_list->GetItemCount() );
    CPPUNIT_ASSERT( !m_list->HadOutOfRangeMeasure() );

    m_list->SetItemCount(20);
    m_list->ArmClearOnMeasure();

    wxMouseEvent click(wxEVT_LEFT_DOWN);
    click.m_x = 2;
    click.m_y = 2;
    m_list->GetEventHandler()->ProcessEvent(click);

    CPPUNIT_ASSERT_EQUAL( static_cast<size_t>(0),
                          m_list->GetItemCount() );
    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->GetCurrent() );
    CPPUNIT_ASSERT_EQUAL( wxNOT_FOUND, m_list->GetSelection() );
    CPPUNIT_ASSERT_EQUAL( 0, selected.GetCount() );
    CPPUNIT_ASSERT( !m_list->HadOutOfRangeMeasure() );
}

void VListBoxTestCase::DestroyDuringSizeDispatch()
{
    TestVListBox* list = new TestVListBox(wxTheApp->GetTopWindow());
    list->SetItemCount(10);

    const wxWeakRef<wxWindow> weakList(list);
    list->Bind(wxEVT_SIZE, [&list](wxSizeEvent&)
    {
        TestVListBox* const doomed = list;
        list = nullptr;
        delete doomed;
    });

    wxSizeEvent sizeEvent(wxSize(200, 100), list->GetId());
    sizeEvent.SetEventObject(list);
    wxEvtHandler* const handler = list->GetEventHandler();
    handler->ProcessEvent(sizeEvent);

    CPPUNIT_ASSERT( !weakList );
    CPPUNIT_ASSERT_EQUAL( static_cast<TestVListBox *>(nullptr), list );
}

void VListBoxTestCase::LargeVirtualModelIsBounded()
{
    constexpr size_t ItemCount = 100000;
    m_list->SetItemCount(ItemCount);
    m_list->ResetMeasureCount();

    CPPUNIT_ASSERT( m_list->ScrollToRow(ItemCount - 1) );
    CPPUNIT_ASSERT( m_list->IsRowVisible(ItemCount - 1) );
    CPPUNIT_ASSERT( m_list->GetVisibleRowsEnd() <= ItemCount );

    // A virtual list must not materialize or measure its entire model merely
    // to reach its final row.
    CPPUNIT_ASSERT( m_list->GetMeasureCount() < ItemCount / 10 );
}

void VListBoxTestCase::DefaultCreateReparentAndLifetime()
{
    wxWindow* const top = wxTheApp->GetTopWindow();
    wxPanel* const parent1 = new wxPanel(top);
    wxPanel* const parent2 = new wxPanel(top);

    TestVListBox* const twoStep = new TestVListBox;
    CPPUNIT_ASSERT( twoStep->Create(parent1, wxID_ANY,
                                    wxDefaultPosition, wxSize(120, 70)) );
    twoStep->SetItemCount(3);
    CPPUNIT_ASSERT( twoStep->Reparent(parent2) );
    CPPUNIT_ASSERT_EQUAL( parent2,
                          static_cast<wxWindow*>(twoStep->GetParent()) );
    delete twoStep;

    for ( int i = 0; i < 100; ++i )
    {
        TestVListBox* const list = new TestVListBox(parent1);
        list->SetItemCount(100);
        list->ScrollToRow(50);
        delete list;
    }

    delete parent2;
    delete parent1;
}

#endif // wxUSE_LISTBOX
