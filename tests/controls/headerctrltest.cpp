///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/headerctrltest.cpp
// Purpose:     wxHeaderCtrl unit test
// Author:      Vadim Zeitlin
// Created:     2008-11-26
// Copyright:   (c) 2008 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/headerctrl.h"
#include "wx/private/columnorder.h"

#if defined(__WXMSW__) && !defined(__WXWINUI__)
    #include "wx/msw/private.h"
    #include "wx/msw/wrapcctl.h"
#endif

#include <algorithm>
#include <functional>
#include <limits>
#include <utility>
#include <vector>

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TestHeaderCtrl : public wxHeaderCtrlSimple
{
public:
    explicit TestHeaderCtrl(wxWindow* parent)
        : wxHeaderCtrlSimple(parent)
    {
    }

    const wxHeaderColumn& GetTestColumn(unsigned int idx) const
    {
        return GetColumn(idx);
    }

    void SetBestWidthHook(std::function<int(unsigned int)> hook)
    {
        m_bestWidthHook = std::move(hook);
    }

#ifdef wxHAS_GENERIC_HEADERCTRL
    int GetTestColumnTitleWidth(unsigned int idx)
    {
        return GetColumnTitleWidth(idx);
    }
#endif // wxHAS_GENERIC_HEADERCTRL

protected:
    int GetBestFittingWidth(unsigned int idx) const override
    {
        return m_bestWidthHook ? m_bestWidthHook(idx) : -1;
    }

private:
    std::function<int(unsigned int)> m_bestWidthHook;
};

class HeaderCtrlTestCase
{
public:
    HeaderCtrlTestCase();

protected:
    std::unique_ptr<TestHeaderCtrl> m_header;

    wxDECLARE_NO_COPY_CLASS(HeaderCtrlTestCase);
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

HeaderCtrlTestCase::HeaderCtrlTestCase()
{
    m_header = make_unique<TestHeaderCtrl>(wxTheApp->GetTopWindow());
}


// ----------------------------------------------------------------------------
// the tests themselves
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::AddDelete", "[headerctrl]")
{
    CHECK( m_header->GetColumnCount() == 0 );

    m_header->AppendColumn(wxHeaderColumnSimple("Column 1"));
    CHECK( m_header->GetColumnCount() == 1 );

    m_header->AppendColumn(wxHeaderColumnSimple("Column 2"));
    CHECK( m_header->GetColumnCount() == 2 );

    m_header->InsertColumn(wxHeaderColumnSimple("Column 0"), 0);
    CHECK( m_header->GetColumnCount() == 3 );

    m_header->DeleteColumn(2);
    CHECK( m_header->GetColumnCount() == 2 );
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::BestSize", "[headerctrl]")
{
    const wxSize sizeEmpty = m_header->GetBestSize();
    // this fails under wxGTK where wxControl::GetBestSize() is 0 in horizontal
    // direction
    //CHECK( sizeEmpty.x > 0 );
    CHECK( sizeEmpty.y > 0 );

    m_header->AppendColumn(wxHeaderColumnSimple("Foo"));
    m_header->AppendColumn(wxHeaderColumnSimple("Bar"));
    const wxSize size = m_header->GetBestSize();
    CHECK( size.y == sizeEmpty.y );
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::Reorder", "[headerctrl]")
{
    static const int COL_COUNT = 4;

    int n;

    for ( n = 0; n < COL_COUNT; n++ )
        m_header->AppendColumn(wxHeaderColumnSimple(wxString::Format("%d", n)));

    wxArrayInt order = m_header->GetColumnsOrder(); // initial order: [0 1 2 3]
    for ( n = 0; n < COL_COUNT; n++ )
        CHECK( order[n] == n );

    wxHeaderCtrl::MoveColumnInOrderArray(order, 0, 2);
    m_header->SetColumnsOrder(order);   // change order to [1 2 0 3]

    order = m_header->GetColumnsOrder();
    CHECK( order[0] == 1 );
    CHECK( order[1] == 2 );
    CHECK( order[2] == 0 );
    CHECK( order[3] == 3 );

    order[2] = 3;
    order[3] = 0;
    m_header->SetColumnsOrder(order);   // and now [1 2 3 0]
    order = m_header->GetColumnsOrder();
    CHECK( order[0] == 1 );
    CHECK( order[1] == 2 );
    CHECK( order[2] == 3 );
    CHECK( order[3] == 0 );

    wxHeaderCtrl::MoveColumnInOrderArray(order, 1, 3);
    m_header->SetColumnsOrder(order);    // finally [2 3 0 1]
    order = m_header->GetColumnsOrder();
    CHECK( order[0] == 2 );
    CHECK( order[1] == 3 );
    CHECK( order[2] == 0 );
    CHECK( order[3] == 1 );
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::ReentrantBestWidthReplacement", "[headerctrl]")
{
    m_header->AppendColumn(wxHeaderColumnSimple("Original", 80));
    m_header->SetBestWidthHook(
        [this](unsigned int)
        {
            m_header->DeleteColumn(0);
            m_header->AppendColumn(wxHeaderColumnSimple("Replacement", 42));
            return 200;
        });

    wxHeaderCtrlEvent event(wxEVT_HEADER_SEPARATOR_DCLICK, m_header->GetId());
    event.SetEventObject(m_header.get());
    event.SetColumn(0);
    m_header->GetEventHandler()->ProcessEvent(event);

    REQUIRE( (m_header->GetColumnCount()) == (1) );
    REQUIRE( (m_header->GetTestColumn(0).GetTitle()) == (wxString("Replacement")) );
    REQUIRE( (m_header->GetTestColumn(0).GetWidth()) == (42) );
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::OrderMutationPrimitive", "[headerctrl]")
{
    wxArrayInt order;
    std::vector<unsigned int> oracle;
    unsigned int count = 0;
    unsigned int random = 0x9e3779b9u;

    const auto nextRandom = [&random]()
    {
        random = random * 1664525u + 1013904223u;
        return random;
    };

    for ( unsigned int iteration = 0; iteration < 10000; ++iteration )
    {
        const unsigned int action = nextRandom() % 3;
        if ( count == 0 || (action == 0 && count < 64) )
        {
            const unsigned int firstId = nextRandom() % (count + 1);
            const unsigned int displayPos = nextRandom() % (count + 1);
            const unsigned int inserted =
                1 + nextRandom() % wxMin(3u, 64u - count);

            REQUIRE(
                wxPrivate::ColumnOrderMutation::Insert(
                    order, count, firstId, inserted, displayPos));

            for ( unsigned int& id : oracle )
            {
                if ( id >= firstId )
                    id += inserted;
            }
            for ( unsigned int offset = 0; offset < inserted; ++offset )
            {
                oracle.insert(
                    oracle.begin() + displayPos + offset,
                    firstId + offset);
            }
            count += inserted;
        }
        else if ( action == 1 )
        {
            const unsigned int firstId = nextRandom() % count;
            const unsigned int erased =
                1 + nextRandom() % wxMin(3u, count - firstId);

            REQUIRE(
                wxPrivate::ColumnOrderMutation::Erase(
                    order, count, firstId, erased));

            const unsigned int endId = firstId + erased;
            std::vector<unsigned int> remaining;
            for ( const unsigned int id : oracle )
            {
                if ( id < firstId || id >= endId )
                    remaining.push_back(id >= endId ? id - erased : id);
            }
            oracle.swap(remaining);
            count -= erased;
        }
        else
        {
            const unsigned int id = nextRandom() % count;
            const unsigned int displayPos = nextRandom() % count;

            REQUIRE(
                wxPrivate::ColumnOrderMutation::Move(
                    order, count, id, displayPos));

            const auto oldPos =
                std::find(oracle.begin(), oracle.end(), id);
            REQUIRE(oldPos != oracle.end());
            oracle.erase(oldPos);
            oracle.insert(oracle.begin() + displayPos, id);
        }

        REQUIRE(
            wxPrivate::ColumnOrderMutation::IsValid(order, count));
        REQUIRE( (order.size()) == (static_cast<size_t>(count)) );
        REQUIRE( (order.size()) == (oracle.size()) );
        for ( size_t pos = 0; pos < oracle.size(); ++pos )
        {
            REQUIRE( (static_cast<unsigned int>(order[pos])) == (oracle[pos]) );
        }
    }

    wxArrayInt invalid;
    invalid.push_back(0);
    invalid.push_back(0);
    const wxArrayInt unchanged = invalid;
    REQUIRE(
        !wxPrivate::ColumnOrderMutation::Insert(
            invalid, 2, 1, 1, 1));
    REQUIRE(invalid == unchanged);
    REQUIRE(
        !wxPrivate::ColumnOrderMutation::Move(invalid, 2, 2, 0));
    REQUIRE(invalid == unchanged);
    REQUIRE(
        !wxPrivate::ColumnOrderMutation::Erase(invalid, 2, 1, 2));
    REQUIRE(invalid == unchanged);

    wxArrayInt natural;
    REQUIRE(
        !wxPrivate::ColumnOrderMutation::Insert(
            natural,
            static_cast<unsigned int>(std::numeric_limits<int>::max()),
            0,
            1,
            0));
    REQUIRE(natural.empty());
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::ReorderMutation", "[headerctrl]")
{
    for ( int n = 0; n < 4; ++n )
    {
        m_header->AppendColumn(
            wxHeaderColumnSimple(wxString::Format("Column %d", n), 20 + n));
    }

    wxArrayInt order;
    order.push_back(2);
    order.push_back(0);
    order.push_back(3);
    order.push_back(1);
    m_header->SetColumnsOrder(order);
    m_header->ShowSortIndicator(1);

    m_header->InsertColumn(wxHeaderColumnSimple("Inserted", 31), 1);

    const wxArrayInt insertedOrder = m_header->GetColumnsOrder();
    const int expectedInserted[] = { 3, 1, 0, 4, 2 };
    REQUIRE( (insertedOrder.size()) == (WXSIZEOF(expectedInserted)) );
    for ( size_t pos = 0; pos < WXSIZEOF(expectedInserted); ++pos )
    {
        REQUIRE( (insertedOrder[pos]) == (expectedInserted[pos]) );
    }
    REQUIRE(!m_header->GetTestColumn(1).IsSortKey());
    REQUIRE(m_header->GetTestColumn(2).IsSortKey());

    m_header->DeleteColumn(1);

    const wxArrayInt restoredOrder = m_header->GetColumnsOrder();
    const int expectedRestored[] = { 2, 0, 3, 1 };
    REQUIRE( (restoredOrder.size()) == (WXSIZEOF(expectedRestored)) );
    for ( size_t pos = 0; pos < WXSIZEOF(expectedRestored); ++pos )
    {
        REQUIRE( (restoredOrder[pos]) == (expectedRestored[pos]) );
    }
    REQUIRE(m_header->GetTestColumn(1).IsSortKey());

#ifdef wxHAS_GENERIC_HEADERCTRL
    const int expectedBestWidth = 20 + 21 + 22 + 23;
    REQUIRE( (m_header->GetBestSize().x) == (expectedBestWidth) );

    m_header->HideColumn(2);
    REQUIRE( (m_header->GetBestSize().x) == (expectedBestWidth - 22) );
#endif // wxHAS_GENERIC_HEADERCTRL

    m_header->DeleteColumn(1);
    for ( unsigned int col = 0; col < m_header->GetColumnCount(); ++col )
        REQUIRE(!m_header->GetTestColumn(col).IsSortKey());
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::SortedInsertion", "[headerctrl]")
{
    m_header->AppendColumn(wxHeaderColumnSimple("Old sort", 20));
    m_header->ShowSortIndicator(0);

    wxHeaderColumnSimple sorted("Incoming sort", 30);
    sorted.SetSortOrder(false);
    m_header->InsertColumn(sorted, 0);

    REQUIRE(m_header->GetTestColumn(0).IsSortKey());
    REQUIRE(
        !m_header->GetTestColumn(0).IsSortOrderAscending());
    REQUIRE(!m_header->GetTestColumn(1).IsSortKey());

    m_header->RemoveSortIndicator();
    for ( unsigned int col = 0; col < m_header->GetColumnCount(); ++col )
        REQUIRE(!m_header->GetTestColumn(col).IsSortKey());
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::AllHiddenOrder", "[headerctrl]")
{
    m_header->AppendColumn(wxHeaderColumnSimple("First", 20));
    m_header->AppendColumn(wxHeaderColumnSimple("Second", 30));
    m_header->HideColumn(0);
    m_header->HideColumn(1);

    wxArrayInt order;
    order.push_back(1);
    order.push_back(0);
    m_header->SetColumnsOrder(order);

    REQUIRE(m_header->GetColumnsOrder() == order);
    REQUIRE(m_header->GetTestColumn(0).IsHidden());
    REQUIRE(m_header->GetTestColumn(1).IsHidden());
}

#ifdef wxHAS_GENERIC_HEADERCTRL

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::GenericGeometry", "[headerctrl]")
{
    m_header->SetSize(0, 0, 240, 40);
    m_header->AppendColumn(wxHeaderColumnSimple("Default width"));

    const int widthTitle = m_header->GetTestColumnTitleWidth(0);
    REQUIRE(widthTitle > 0);
    REQUIRE( (m_header->GetBestSize().x) == (widthTitle) );

    int clicked = -1;
    m_header->Bind(
        wxEVT_HEADER_CLICK,
        [&clicked](wxHeaderCtrlEvent& event)
        {
            clicked = event.GetColumn();
        });

    wxMouseEvent click(wxEVT_LEFT_UP);
    click.SetId(m_header->GetId());
    click.SetEventObject(m_header.get());
    click.SetPosition(wxPoint(widthTitle / 2, 1));
    m_header->ProcessWindowEvent(click);
    REQUIRE( (clicked) == (0) );

    m_header->DeleteAllColumns();
    m_header->AppendColumn(
        wxHeaderColumnSimple(
            "Huge 1", std::numeric_limits<int>::max()));
    m_header->AppendColumn(
        wxHeaderColumnSimple(
            "Huge 2", std::numeric_limits<int>::max()));
    REQUIRE( (m_header->GetBestSize().x) == (std::numeric_limits<int>::max()) );

    m_header->DeleteAllColumns();
    m_header->AppendColumn(wxHeaderColumnSimple("Resizable", 20));
    REQUIRE( (m_header->GetBestSize().x) == (20) );

    wxHeaderCtrlEvent resizing(wxEVT_HEADER_RESIZING, m_header->GetId());
    resizing.SetEventObject(m_header.get());
    resizing.SetColumn(0);
    resizing.SetWidth(47);
    m_header->ProcessWindowEvent(resizing);
    REQUIRE( (m_header->GetBestSize().x) == (47) );
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::GenericReentrantMutation", "[headerctrl]")
{
    m_header->SetSize(0, 0, 240, 40);
    m_header->AppendColumn(wxHeaderColumnSimple("Original", 80));

    int beginEvents = 0;
    m_header->Bind(
        wxEVT_HEADER_BEGIN_REORDER,
        [this, &beginEvents](wxHeaderCtrlEvent&)
        {
            ++beginEvents;
            m_header->InsertColumn(
                wxHeaderColumnSimple("Inserted in callback", 40), 0);
        });

    wxMouseEvent down(wxEVT_LEFT_DOWN);
    down.SetId(m_header->GetId());
    down.SetEventObject(m_header.get());
    down.SetPosition(wxPoint(10, 1));
    m_header->ProcessWindowEvent(down);

    REQUIRE( (beginEvents) == (1) );
    REQUIRE( (m_header->GetColumnCount()) == (2) );
    REQUIRE(!m_header->HasCapture());

    // A later pointer event must not resume the gesture with its old column.
    wxMouseEvent motion(wxEVT_MOTION);
    motion.SetId(m_header->GetId());
    motion.SetEventObject(m_header.get());
    motion.SetPosition(wxPoint(25, 1));
    m_header->ProcessWindowEvent(motion);
    REQUIRE(!m_header->HasCapture());
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::GenericActiveGestureMutation", "[headerctrl]")
{
    m_header->SetSize(0, 0, 240, 40);
    m_header->AppendColumn(wxHeaderColumnSimple("Dragged", 80));

    int cancellations = 0;
    int cancelledColumn = -1;
    m_header->Bind(
        wxEVT_HEADER_DRAGGING_CANCELLED,
        [&cancellations, &cancelledColumn](wxHeaderCtrlEvent& event)
        {
            ++cancellations;
            cancelledColumn = event.GetColumn();
        });

    wxMouseEvent down(wxEVT_LEFT_DOWN);
    down.SetId(m_header->GetId());
    down.SetEventObject(m_header.get());
    down.SetPosition(wxPoint(10, 1));
    m_header->ProcessWindowEvent(down);
    REQUIRE(m_header->HasCapture());

    // Mutate between two pointer events, while native capture is active.
    m_header->DeleteColumn(0);
    REQUIRE( (m_header->GetColumnCount()) == (0) );
    REQUIRE(!m_header->HasCapture());
    REQUIRE( (cancellations) == (1) );
    REQUIRE( (cancelledColumn) == (0) );

    wxMouseEvent motion(wxEVT_MOTION);
    motion.SetId(m_header->GetId());
    motion.SetEventObject(m_header.get());
    motion.SetPosition(wxPoint(25, 1));
    m_header->ProcessWindowEvent(motion);
    REQUIRE( (cancellations) == (1) );
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::GenericSortMutationDuringCancellation", "[headerctrl]")
{
    m_header->SetSize(0, 0, 240, 40);
    m_header->AppendColumn(wxHeaderColumnSimple("First", 80));
    m_header->AppendColumn(wxHeaderColumnSimple("Second", 80));
    m_header->ShowSortIndicator(0);

    int cancellations = 0;
    m_header->Bind(
        wxEVT_HEADER_DRAGGING_CANCELLED,
        [this, &cancellations](wxHeaderCtrlEvent&)
        {
            ++cancellations;
            m_header->DeleteAllColumns();
        });

    wxMouseEvent down(wxEVT_LEFT_DOWN);
    down.SetId(m_header->GetId());
    down.SetEventObject(m_header.get());
    down.SetPosition(wxPoint(90, 1));
    m_header->ProcessWindowEvent(down);
    REQUIRE(m_header->HasCapture());

    // Removing the previous sort indicator updates its native/generic item,
    // which cancels the gesture. The cancellation callback is allowed to
    // replace the entire topology; the outer sort request must then stop.
    m_header->ShowSortIndicator(1);
    REQUIRE( (cancellations) == (1) );
    REQUIRE( (m_header->GetColumnCount()) == (0) );
    REQUIRE(!m_header->HasCapture());
}

#endif // wxHAS_GENERIC_HEADERCTRL

#if defined(__WXMSW__) && !defined(__WXWINUI__)

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::NativeActiveGestureMutation", "[headerctrl]")
{
    m_header->AppendColumn(wxHeaderColumnSimple("Native", 80));

    wxWindowList::compatibility_iterator childNode =
        m_header->GetChildren().GetFirst();
    REQUIRE(childNode);
    wxWindow* const native = childNode->GetData();
    const HWND hwndNative = GetHwndOf(native);
    REQUIRE(hwndNative);

    int beginEvents = 0;
    int cancellations = 0;
    bool mutateInBegin = false;
    m_header->Bind(
        wxEVT_HEADER_BEGIN_REORDER,
        [this, &beginEvents, &mutateInBegin](wxHeaderCtrlEvent&)
        {
            ++beginEvents;
            if ( mutateInBegin )
            {
                m_header->InsertColumn(
                    wxHeaderColumnSimple("Reentrant", 40), 0);
            }
        });
    m_header->Bind(
        wxEVT_HEADER_DRAGGING_CANCELLED,
        [&cancellations](wxHeaderCtrlEvent&)
        {
            ++cancellations;
        });

    const auto sendBeginDrag =
        [hwndNative]()
        {
            NMHEADER notification = {};
            notification.hdr.hwndFrom = hwndNative;
            notification.hdr.idFrom =
                static_cast<UINT_PTR>(
                    ::GetWindowLongPtr(hwndNative, GWLP_ID));
            notification.hdr.code = HDN_BEGINDRAG;
            notification.iItem = 0;

            ::SendMessage(
                ::GetParent(hwndNative),
                WM_NOTIFY,
                notification.hdr.idFrom,
                reinterpret_cast<LPARAM>(&notification));
        };

    sendBeginDrag();
    REQUIRE( (beginEvents) == (1) );

    // Mutating between native notifications must cancel the remembered
    // gesture before rebuilding the common control's item array.
    m_header->DeleteColumn(0);
    REQUIRE( (cancellations) == (1) );
    REQUIRE( (m_header->GetColumnCount()) == (0) );

    m_header->AppendColumn(wxHeaderColumnSimple("Native again", 80));
    mutateInBegin = true;
    sendBeginDrag();

    // This second cycle mutates synchronously from the wx begin callback and
    // exercises the native revision/lifetime gate on return from ProcessEvent.
    REQUIRE( (beginEvents) == (2) );
    REQUIRE( (cancellations) == (2) );
    REQUIRE( (m_header->GetColumnCount()) == (2) );
}

TEST_CASE_METHOD(HeaderCtrlTestCase, "HeaderCtrl::NativeReleaseCaptureClassification", "[headerctrl]")
{
    m_header->AppendColumn(wxHeaderColumnSimple("Native", 80));

    wxWindowList::compatibility_iterator childNode =
        m_header->GetChildren().GetFirst();
    REQUIRE(childNode);
    wxWindow* const native = childNode->GetData();
    const HWND hwndNative = GetHwndOf(native);
    REQUIRE(hwndNative);

    int beginEvents = 0;
    int endEvents = 0;
    int cancellations = 0;
    int cancelledColumn = -1;
    m_header->Bind(
        wxEVT_HEADER_BEGIN_RESIZE,
        [&beginEvents](wxHeaderCtrlEvent&) { ++beginEvents; });
    m_header->Bind(
        wxEVT_HEADER_END_RESIZE,
        [&endEvents](wxHeaderCtrlEvent&) { ++endEvents; });
    m_header->Bind(
        wxEVT_HEADER_DRAGGING_CANCELLED,
        [&cancellations, &cancelledColumn](wxHeaderCtrlEvent& event)
        {
            ++cancellations;
            cancelledColumn = event.GetColumn();
        });

    const auto notify =
        [hwndNative](UINT code, HDITEM* item)
        {
            NMHEADER notification = {};
            notification.hdr.hwndFrom = hwndNative;
            notification.hdr.idFrom =
                static_cast<UINT_PTR>(
                    ::GetWindowLongPtr(hwndNative, GWLP_ID));
            notification.hdr.code = code;
            notification.iItem = 0;
            notification.pitem = item;

            ::SendMessage(
                ::GetParent(hwndNative),
                WM_NOTIFY,
                notification.hdr.idFrom,
                reinterpret_cast<LPARAM>(&notification));
        };

    HDITEM item = {};
    item.mask = HDI_WIDTH;
    item.cxy = 80;
    notify(HDN_BEGINTRACK, &item);
    item.cxy = 96;
    notify(HDN_ENDTRACK, &item);
    notify(NM_RELEASEDCAPTURE, nullptr);

    REQUIRE( (beginEvents) == (1) );
    REQUIRE( (endEvents) == (1) );
    REQUIRE( (cancellations) == (0) );

    // Releasing capture without a matching end notification is a real
    // cancellation and must retain the tracked logical column identity.
    notify(HDN_BEGINTRACK, &item);
    notify(NM_RELEASEDCAPTURE, nullptr);

    REQUIRE( (beginEvents) == (2) );
    REQUIRE( (endEvents) == (1) );
    REQUIRE( (cancellations) == (1) );
    REQUIRE( (cancelledColumn) == (0) );
}

#endif // native wxMSW
