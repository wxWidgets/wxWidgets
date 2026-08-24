///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/virtlistctrltest.cpp
// Purpose:     wxListCtrl unit tests for virtual mode
// Author:      Vadim Zeitlin
// Created:     2010-11-13
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_LISTCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
#endif // WX_PRECOMP

#include "wx/listctrl.h"
#include "testableframe.h"
#include "wx/uiaction.h"

#include <memory>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class VirtListCtrlTestCase
{
public:
    VirtListCtrlTestCase();

protected:
    std::unique_ptr<wxListCtrl> m_list;

    wxDECLARE_NO_COPY_CLASS(VirtListCtrlTestCase);
};

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

VirtListCtrlTestCase::VirtListCtrlTestCase()
{
    // Define a class overriding OnGetItemText() which must be overridden for
    // any virtual list control.
    class VirtListCtrl : public wxListCtrl
    {
    public:
        VirtListCtrl()
            : wxListCtrl(wxTheApp->GetTopWindow(), wxID_ANY,
                         wxPoint(0, 0), wxSize(400, 200),
                         wxLC_REPORT | wxLC_VIRTUAL)
        {
        }

    protected:
        virtual wxString OnGetItemText(long item, long column) const override
        { return wxString::Format("Row %ld, col %ld", item, column); }
    };

    m_list = make_unique<VirtListCtrl>();
    m_list->AppendColumn("Col0");
}


TEST_CASE_METHOD(VirtListCtrlTestCase, "VirtListCtrl::UpdateSelection", "[listctrl][virtual]")
{
    m_list->SetItemCount(10);
    CHECK( m_list->GetSelectedItemCount() == 0 );

    m_list->SetItemState(7, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    CHECK( m_list->GetSelectedItemCount() == 1 );

    m_list->SetItemState(0, wxLIST_STATE_SELECTED, wxLIST_STATE_SELECTED);
    CHECK( m_list->GetSelectedItemCount() == 2 );

    // The item 7 is now invalid and so shouldn't be counted as selected any
    // more. Notice that under wxQt, the selection is lost/cleared when the
    // model is reset
    m_list->SetItemCount(5);
#ifndef __WXQT__
    CHECK( m_list->GetSelectedItemCount() == 1 );
#else
    CHECK( m_list->GetSelectedItemCount() == 0 );
#endif
}

TEST_CASE_METHOD(VirtListCtrlTestCase, "VirtListCtrl::DeselectedEvent", "[listctrl][virtual]")
{
#if wxUSE_UIACTIONSIMULATOR
    if ( !EnableUITests() )
        return;

    m_list->SetItemCount(1);
    wxListCtrl* const list = m_list.get();

    EventCounter selected(list, wxEVT_LIST_ITEM_SELECTED);
    EventCounter deselected(list, wxEVT_LIST_ITEM_DESELECTED);

    wxUIActionSimulator sim;

    wxRect pos;
    list->GetItemRect(0, pos);

    //We move in slightly so we are not on the edge
    wxPoint point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 10);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    // We want a point within the listctrl but below any items
    point = list->ClientToScreen(pos.GetPosition()) + wxPoint(10, 50);

    sim.MouseMove(point);
    wxYield();

    sim.MouseClick();
    wxYield();

    CHECK(selected.GetCount() == 1);
    CHECK(deselected.GetCount() == 1);
#endif
}

#endif // wxUSE_LISTCTRL
