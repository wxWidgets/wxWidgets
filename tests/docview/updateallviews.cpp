///////////////////////////////////////////////////////////////////////////////
// Name:        tests/docview/doc-updateallviews.cpp
// Purpose:     Unit test for wxDocument::UpdateAllViews()
// Author:      Bill Su
// Created:     2026-08-16
// Copyright:   (c) 2026 Bill Su
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#if wxUSE_DOC_VIEW_ARCHITECTURE

#include <memory>

#include "asserthelper.h"

#include "wx/docview.h"

// ----------------------------------------------------------------------------
// tests helpers
// ----------------------------------------------------------------------------

namespace
{

class MyDocument : public wxDocument
{
public:
    virtual ~MyDocument() override;
    virtual void OnChangedViewList() override;

protected:
};

class MyView : public wxView
{
public:
    explicit MyView(int serialNumber) : m_serialNumber(serialNumber) {}
    virtual void OnUpdate(wxView* sender, wxObject* hint = nullptr) override;

    virtual void OnDraw(wxDC* WXUNUSED(dc)) override { /* do nothing */ }

private:
    const int m_serialNumber;
};

MyDocument::~MyDocument()
{
    // avoid memory leak of views
    DeleteAllViews();
}

void MyDocument::OnChangedViewList()
{
    // base class deletes this when no views remain,
    // but dtor removes views, so base class version
    // must not be called
}

struct Accum : public wxObject
{
    enum Command
    {
        SIMPLE,
        SERNO_50_REMOVE_SELF,
        SERNO_50_REMOVE_ODD,
    };

    Accum(Command c) : command(c) {}

    const Command command;
    wxVector<int> v;
};

void MyView::OnUpdate(wxView* WXUNUSED(sender), wxObject* hint /*= nullptr*/)
{
    Accum* const accum = static_cast<Accum*>(hint);
    switch ( accum->command )
    {
        case Accum::SIMPLE:
            accum->v.push_back(m_serialNumber);
            break;

        case Accum::SERNO_50_REMOVE_SELF:
            if (m_serialNumber == 50)
            {
                GetDocument()->RemoveView(this);
                delete this;
            }
            break;

        case Accum::SERNO_50_REMOVE_ODD:
            if (m_serialNumber == 50)
            {
                wxDocument* const doc = GetDocument();
                const wxViewVector initialState = doc->GetViewsVector();
                for (wxView* const view : initialState)
                {
                    MyView* const myView = static_cast<MyView*>(view);
                    if (myView->m_serialNumber % 2 == 1)
                    {
                        doc->RemoveView(view);
                        delete view;
                    }
                }
            }
            break;
    }
}

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

TEST_CASE("wxDocument::UpdateAllViews", "[docview]")
{
    const std::unique_ptr<wxDocument> doc(new MyDocument());
    wxView* view50 = nullptr;
    for (int i = 0; i < 100; ++i)
    {
        wxView* view = new MyView(i);
        if (i == 50)
        {
            view50 = view;
        }
        doc->AddView(view);
        view->SetDocument(doc.get());
    }

    // test UpdateAllViews() when not removing view(s)
    SECTION("Simple")
    {
        Accum accum(Accum::SIMPLE);
        doc->UpdateAllViews(nullptr, &accum);
        // order of wxView::OnUpdate() is not guaranteed
        wxVectorSort(accum.v);
        REQUIRE( accum.v.size() == 100 );
        REQUIRE( accum.v.front() == 0 );
        REQUIRE( accum.v.back() == 99 );
    }

    // test UpdateAllViews() doesn't call sender
    SECTION("SkipSender")
    {
        Accum accum(Accum::SIMPLE);
        doc->UpdateAllViews(view50, &accum);
        // order of wxView::OnUpdate() is not guaranteed
        wxVectorSort(accum.v);
        REQUIRE( accum.v.size() == 99 );
        REQUIRE( accum.v.front() == 0 );
        REQUIRE( accum.v[49] == 49 );
        REQUIRE( accum.v[50] == 51 );
        REQUIRE( accum.v.back() == 99 );
    }

    // test UpdateAllViews() when view removes (only) itself
    SECTION("RemoveSelf")
    {
        {
            Accum accum(Accum::SERNO_50_REMOVE_SELF);
            doc->UpdateAllViews(nullptr, &accum);
        }
        Accum accum(Accum::SIMPLE);
        doc->UpdateAllViews(nullptr, &accum);
        // order of wxView::OnUpdate() is not guaranteed
        wxVectorSort(accum.v);
        REQUIRE( accum.v.size() == 99 );
        REQUIRE( accum.v.front() == 0 );
        REQUIRE( accum.v[49] == 49);
        REQUIRE( accum.v[50] == 51 );
        REQUIRE( accum.v.back() == 99 );
    }

    // test UpdateAllViews() when view removes many views
    SECTION("RemoveOdd")
    {
        {
            Accum accum(Accum::SERNO_50_REMOVE_ODD);
            doc->UpdateAllViews(nullptr, &accum);
        }
        Accum accum(Accum::SIMPLE);
        doc->UpdateAllViews(nullptr, &accum);
        // order of wxView::OnUpdate() is not guaranteed
        wxVectorSort(accum.v);
        REQUIRE( accum.v.size() == 50 );
        REQUIRE( accum.v.front() == 0 );
        REQUIRE( accum.v.back() == 98 );
    }
}

#endif // wxUSE_DOC_VIEW_ARCHITECTURE
