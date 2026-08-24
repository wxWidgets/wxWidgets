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
#endif // WX_PRECOMP

#include "wx/htmllbox.h"
#include "itemcontainertest.h"

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


#endif //wxUSE_HTML
