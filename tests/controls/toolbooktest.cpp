///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/toolbooktest.cpp
// Purpose:     wxToolbook unit test
// Author:      Steven Lamerton
// Created:     2010-07-02
// Copyright:   (c) 2010 Steven Lamerton
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_TOOLBOOK


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/toolbook.h"
#include "wx/toolbar.h"
#include "bookctrlbasetest.h"

#include <memory>

class ToolbookTestCase : public BookCtrlBaseTestCase
{
public:
    ToolbookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_toolbook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_TOOLBOOK_PAGE_CHANGING; }

    virtual void Realize() override { m_toolbook->GetToolBar()->Realize(); }

    std::unique_ptr<wxToolbook> m_toolbook;

    wxDECLARE_NO_COPY_CLASS(ToolbookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(ToolbookTestCase, "Toolbook",
                       "[toolbook][book]");

ToolbookTestCase::ToolbookTestCase()
{
    m_toolbook = make_unique<wxToolbook>(wxTheApp->GetTopWindow(), wxID_ANY,
                                         wxDefaultPosition, wxSize(400, 200));
    AddPanels();
}


TEST_CASE_METHOD(ToolbookTestCase, "Toolbook::ToolBar", "[toolbook]")
{
    wxToolBar* toolbar = static_cast<wxToolBar*>(m_toolbook->GetToolBar());

    CHECK(toolbar);
    CHECK(toolbar->GetToolsCount() == 3);
}

#endif //wxUSE_TOOLBOOK
