///////////////////////////////////////////////////////////////////////////////
// Name:        tests/sizers/boxsizer.cpp
// Purpose:     Unit tests for wxBoxSizer
// Author:      Vadim Zeitlin
// Created:     2010-03-06
// RCS-ID:      $Id$
// Copyright:   (c) 2010 Vadim Zeitlin <vadim@wxwidgets.org>
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/sizer.h"
#endif // WX_PRECOMP

#include "asserthelper.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class BoxSizerTestCase : public CppUnit::TestCase
{
public:
    BoxSizerTestCase() { }

    virtual void setUp();
    virtual void tearDown();

private:
    CPPUNIT_TEST_SUITE( BoxSizerTestCase );
        CPPUNIT_TEST( Size1 );
        CPPUNIT_TEST( Size3 );
        CPPUNIT_TEST( CalcMin );
    CPPUNIT_TEST_SUITE_END();

    void Size1();
    void Size3();
    void CalcMin();

    wxWindow *m_win;
    wxSizer *m_sizer;

    DECLARE_NO_COPY_CLASS(BoxSizerTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( BoxSizerTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( BoxSizerTestCase, "BoxSizerTestCase" );

// ----------------------------------------------------------------------------
// test initialization
// ----------------------------------------------------------------------------

void BoxSizerTestCase::setUp()
{
    m_win = new wxWindow(wxTheApp->GetTopWindow(), wxID_ANY);
    m_win->SetClientSize(127, 35);

    m_sizer = new wxBoxSizer(wxHORIZONTAL);
    m_win->SetSizer(m_sizer);
}

void BoxSizerTestCase::tearDown()
{
    delete m_win;
    m_win = NULL;

    m_sizer = NULL;
}

// ----------------------------------------------------------------------------
// tests themselves
// ----------------------------------------------------------------------------

void BoxSizerTestCase::Size1()
{
    const wxSize sizeTotal = m_win->GetClientSize();
    const wxSize sizeChild = sizeTotal / 2;

    wxWindow * const
        child = new wxWindow(m_win, wxID_ANY, wxDefaultPosition, sizeChild);
    m_sizer->Add(child);
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( sizeChild, child->GetSize() );

    m_sizer->Clear();
    m_sizer->Add(child, wxSizerFlags(1));
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( wxSize(sizeTotal.x, sizeChild.y), child->GetSize() );

    m_sizer->Clear();
    m_sizer->Add(child, wxSizerFlags(1).Expand());
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( sizeTotal, child->GetSize() );

    m_sizer->Clear();
    m_sizer->Add(child, wxSizerFlags());
    m_sizer->SetItemMinSize(child, sizeTotal*2);
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( sizeTotal, child->GetSize() );

    m_sizer->Clear();
    m_sizer->Add(child, wxSizerFlags().Expand());
    m_sizer->SetItemMinSize(child, sizeTotal*2);
    m_win->Layout();
    CPPUNIT_ASSERT_EQUAL( sizeTotal, child->GetSize() );
}

void BoxSizerTestCase::Size3()
{
    // check that various combinations of minimal sizes and proportions work as
    // expected for different window sizes
    static const struct LayoutTestData
    {
        // proportions of the elements
        int prop[3];

        // minimal sizes of the elements in the sizer direction
        int minsize[3];

        // total size and the expected sizes of the elements
        int x,
            sizes[3];

        // if true, don't try the permutations of our test data
        bool dontPermute;


        // Add the given window to the sizer with the corresponding parameters
        void AddToSizer(wxSizer *sizer, wxWindow *win, int n) const
        {
            sizer->Add(win, wxSizerFlags(prop[n]));
            sizer->SetItemMinSize(win, wxSize(minsize[n], -1));
        }

    } layoutTestData[] =
    {
        // some really simple cases (no need to permute those, they're
        // symmetrical anyhow)
        { { 1, 1, 1, }, {  50,  50,  50, }, 150, {  50,  50,  50, }, true },
        { { 2, 2, 2, }, {  50,  50,  50, }, 600, { 200, 200, 200, }, true },

        // items with different proportions and min sizes when there is enough
        // space to lay them out
        { { 1, 2, 3, }, {   0,   0,   0, }, 600, { 100, 200, 300, } },
        { { 1, 2, 3, }, { 100, 100, 100, }, 600, { 100, 200, 300, } },
        { { 1, 2, 3, }, { 100,  50,  50, }, 600, { 100, 200, 300, } },
        { { 0, 1, 1, }, { 200, 100, 100, }, 600, { 200, 200, 200, } },
        { { 0, 1, 2, }, { 300, 100, 100, }, 600, { 300, 100, 200, } },
        { { 0, 1, 1, }, { 100,  50,  50, }, 300, { 100, 100, 100, } },
        { { 0, 1, 2, }, { 100,  50,  50, }, 400, { 100, 100, 200, } },

        // cases when there is not enough space to lay out the items correctly
        // while still respecting their min sizes
        { { 0, 1, 1, }, { 100, 150,  50, }, 300, { 100, 150,  50, } },
        { { 1, 2, 3, }, { 100, 100, 100, }, 300, { 100, 100, 100, } },
        { { 1, 2, 3, }, { 100,  50,  50, }, 300, { 100,  80, 120, } },
        { { 1, 2, 3, }, { 100,  10,  10, }, 150, { 100,  20,  30, } },

        // cases when there is not enough space even for the min sizes (don't
        // permute in these cases as the layout does depend on the item order
        // because the first ones have priority)
        { { 1, 2, 3, }, { 100,  50,  50, }, 150, { 100,  50,   0, }, true },
        { { 1, 2, 3, }, { 100, 100, 100, }, 200, { 100, 100,   0, }, true },
        { { 1, 2, 3, }, { 100, 100, 100, }, 150, { 100,  50,   0, }, true },
        { { 1, 2, 3, }, { 100, 100, 100, },  50, {  50,   0,   0, }, true },
        { { 1, 2, 3, }, { 100, 100, 100, },   0, {   0,   0,   0, }, true },
    };

    wxWindow *child[3];
    child[0] = new wxWindow(m_win, wxID_ANY);
    child[1] = new wxWindow(m_win, wxID_ANY);
    child[2] = new wxWindow(m_win, wxID_ANY);

    for ( unsigned i = 0; i < WXSIZEOF(layoutTestData); i++ )
    {
        LayoutTestData ltd = layoutTestData[i];

        // the results shouldn't depend on the order of items except in the
        // case when there is not enough space for even the fixed width items
        // (in which case the first ones might get enough of it but not the
        // last ones) so test a couple of permutations of test data unless
        // specifically disabled for this test case
        for ( unsigned p = 0; p < 3; p++)
        {
            switch ( p )
            {
                case 0:
                    // nothing to do, use original data
                    break;

                case 1:
                    // exchange first and last elements
                    wxSwap(ltd.prop[0], ltd.prop[2]);
                    wxSwap(ltd.minsize[0], ltd.minsize[2]);
                    wxSwap(ltd.sizes[0], ltd.sizes[2]);
                    break;

                case 2:
                    // exchange the original third and second elements
                    wxSwap(ltd.prop[0], ltd.prop[1]);
                    wxSwap(ltd.minsize[0], ltd.minsize[1]);
                    wxSwap(ltd.sizes[0], ltd.sizes[1]);
                    break;
            }

            m_sizer->Clear();

            unsigned j;
            for ( j = 0; j < WXSIZEOF(child); j++ )
                ltd.AddToSizer(m_sizer, child[j], j);

            m_win->SetClientSize(ltd.x, -1);
            m_win->Layout();

            for ( j = 0; j < WXSIZEOF(child); j++ )
            {
                WX_ASSERT_EQUAL_MESSAGE
                (
                    (
                        "test %lu, permutation #%lu: wrong size for child #%d "
                        "for total size %d",
                        static_cast<unsigned long>(i),
                        static_cast<unsigned long>(p),
                        j,
                        ltd.x
                    ),
                    ltd.sizes[j], child[j]->GetSize().x
                );
            }

            // don't try other permutations if explicitly disabled
            if ( ltd.dontPermute )
                break;
        }
    }
}

void BoxSizerTestCase::CalcMin()
{
    static const unsigned NUM_TEST_ITEM = 3;

    static const struct CalcMinTestData
    {
        // proportions of the elements, if one of them is -1 it means to not
        // use this window at all in this test
        int prop[NUM_TEST_ITEM];

        // minimal sizes of the elements in the sizer direction
        int minsize[NUM_TEST_ITEM];

        // the expected minimal sizer size
        int total;
    } calcMinTestData[] =
    {
        { {  1,  1, -1 }, {  30,  50,   0 },  100 },
        { {  1,  1,  0 }, {  30,  50,  20 },  120 },
        { { 10, 10, -1 }, {  30,  50,   0 },  100 },
        { {  1,  2,  2 }, {  50,  50,  80 },  250 },
        { {  1,  2,  2 }, { 100,  50,  80 },  500 },
    };

    unsigned n;
    wxWindow *child[NUM_TEST_ITEM];
    for ( n = 0; n < NUM_TEST_ITEM; n++ )
        child[n] = new wxWindow(m_win, wxID_ANY);

    for ( unsigned i = 0; i < WXSIZEOF(calcMinTestData); i++ )
    {
        m_sizer->Clear();

        const CalcMinTestData& cmtd = calcMinTestData[i];
        for ( n = 0; n < NUM_TEST_ITEM; n++ )
        {
            if ( cmtd.prop[n] != -1 )
            {
                child[n]->SetInitialSize(wxSize(cmtd.minsize[n], -1));
                m_sizer->Add(child[n], wxSizerFlags(cmtd.prop[n]));
            }
        }

        WX_ASSERT_EQUAL_MESSAGE
        (
            ("In test #%u", i),
            cmtd.total, m_sizer->CalcMin().x
        );
    }
}
