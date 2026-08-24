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

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class HeaderCtrlTestCase
{
public:
    HeaderCtrlTestCase();

protected:
    std::unique_ptr<wxHeaderCtrlSimple> m_header;

    wxDECLARE_NO_COPY_CLASS(HeaderCtrlTestCase);
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

HeaderCtrlTestCase::HeaderCtrlTestCase()
{
    m_header = make_unique<wxHeaderCtrlSimple>(wxTheApp->GetTopWindow());
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

