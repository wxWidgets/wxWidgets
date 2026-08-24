///////////////////////////////////////////////////////////////////////////////
// Name:        tests/controls/simplebooktest.cpp
// Purpose:     wxSimplebook unit test
// Author:      Vadim Zeitlin
// Created:     2013-06-23
// Copyright:   (c) 2013 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_BOOKCTRL


#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/panel.h"
#endif // WX_PRECOMP

#include "wx/simplebook.h"
#include "bookctrlbasetest.h"

#include <memory>

class SimplebookTestCase : public BookCtrlBaseTestCase
{
public:
    SimplebookTestCase();

protected:
    virtual wxBookCtrlBase *GetBase() const override
    { return m_simplebook.get(); }

    virtual wxEventType GetChangedEvent() const override
    { return wxEVT_BOOKCTRL_PAGE_CHANGED; }

    virtual wxEventType GetChangingEvent() const override
    { return wxEVT_BOOKCTRL_PAGE_CHANGING; }

    std::unique_ptr<wxSimplebook> m_simplebook;

    wxDECLARE_NO_COPY_CLASS(SimplebookTestCase);
};

wxBOOK_CTRL_BASE_TESTS(SimplebookTestCase, "Simplebook",
                       "[simplebook][book]");

SimplebookTestCase::SimplebookTestCase()
{
    m_simplebook = make_unique<wxSimplebook>(
        wxTheApp->GetTopWindow(), wxID_ANY);
    AddPanels();
}


#endif // wxUSE_BOOKCTRL

