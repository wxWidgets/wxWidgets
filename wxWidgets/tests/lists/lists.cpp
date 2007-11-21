///////////////////////////////////////////////////////////////////////////////
// Name:        tests/lists/lists.cpp
// Purpose:     wxList unit test
// Author:      Vadim Zeitlin, Mattia Barbon
// Created:     2004-12-08
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin, Mattia Barbon
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/list.h"

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class ListsTestCase : public CppUnit::TestCase
{
public:
    ListsTestCase() { }

private:
    CPPUNIT_TEST_SUITE( ListsTestCase );
        CPPUNIT_TEST( wxListTest );
        CPPUNIT_TEST( wxStdListTest );
        CPPUNIT_TEST( wxListCtorTest );
    CPPUNIT_TEST_SUITE_END();

    void wxListTest();
    void wxStdListTest();
    void wxListCtorTest();

    DECLARE_NO_COPY_CLASS(ListsTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListsTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ListsTestCase, "ListsTestCase" );

class Baz // Foo is already taken in the hash test
{
public:
    Baz(const wxString& name) : m_name(name) { ms_bars++; }
    Baz(const Baz& bar) : m_name(bar.m_name) { ms_bars++; }
   ~Baz() { ms_bars--; }

   static size_t GetNumber() { return ms_bars; }

   const wxChar *GetName() const { return m_name; }

private:
   wxString m_name;

   static size_t ms_bars;
};

size_t Baz::ms_bars = 0;

#include "wx/list.h"

WX_DECLARE_LIST(Baz, wxListBazs);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxListBazs)

WX_DECLARE_LIST(int, wxListInt);
WX_DEFINE_LIST(wxListInt)

void ListsTestCase::wxListTest()
{
    wxListInt list1;
    int dummy[5];
    int i;

    for ( i = 0; i < 5; ++i )
        list1.Append(dummy + i);

    CPPUNIT_ASSERT( list1.GetCount() == 5 );
    CPPUNIT_ASSERT( list1.Item(3)->GetData() == dummy + 3 );
    CPPUNIT_ASSERT( list1.Find(dummy + 4) );

    wxListInt::compatibility_iterator node = list1.GetFirst();
    i = 0;

    while (node)
    {
        CPPUNIT_ASSERT( node->GetData() == dummy + i );
        node = node->GetNext();
        ++i;
    }

    CPPUNIT_ASSERT( size_t(i) == list1.GetCount() );

    list1.Insert(dummy + 0);
    list1.Insert(1, dummy + 1);
    list1.Insert(list1.GetFirst()->GetNext()->GetNext(), dummy + 2);

    node = list1.GetFirst();
    i = 0;

    while (i < 3)
    {
        int* t = node->GetData();
        CPPUNIT_ASSERT( t == dummy + i );
        node = node->GetNext();
        ++i;
    }
}

void ListsTestCase::wxStdListTest()
{
    wxListInt list1;
    wxListInt::iterator it, en;
    wxListInt::reverse_iterator rit, ren;
    int i;
    for ( i = 0; i < 5; ++i )
        list1.push_back(i + &i);

    for ( it = list1.begin(), en = list1.end(), i = 0;
          it != en; ++it, ++i )
    {
        CPPUNIT_ASSERT( *it == i + &i );
    }

    for ( rit = list1.rbegin(), ren = list1.rend(), i = 4;
          rit != ren; ++rit, --i )
    {
        CPPUNIT_ASSERT( *rit == i + &i );
    }

    CPPUNIT_ASSERT( *list1.rbegin() == *--list1.end() &&
                    *list1.begin() == *--list1.rend() );
    CPPUNIT_ASSERT( *list1.begin() == *--++list1.begin() &&
                    *list1.rbegin() == *--++list1.rbegin() );

    CPPUNIT_ASSERT( list1.front() == &i && list1.back() == &i + 4 );

    list1.erase(list1.begin());
    list1.erase(--list1.end());

    for ( it = list1.begin(), en = list1.end(), i = 1;
          it != en; ++it, ++i )
    {
        CPPUNIT_ASSERT( *it == i + &i );
    }
}

void ListsTestCase::wxListCtorTest()
{
    {
        wxListBazs list1;
        list1.Append(new Baz(_T("first")));
        list1.Append(new Baz(_T("second")));

        CPPUNIT_ASSERT( list1.GetCount() == 2 );
        CPPUNIT_ASSERT( Baz::GetNumber() == 2 );

        wxListBazs list2;
        list2 = list1;

        CPPUNIT_ASSERT( list1.GetCount() == 2 );
        CPPUNIT_ASSERT( list2.GetCount() == 2 );
        CPPUNIT_ASSERT( Baz::GetNumber() == 2 );

#if !wxUSE_STL
        list1.DeleteContents(true);
#else
        WX_CLEAR_LIST(wxListBazs, list1);
#endif
    }

    CPPUNIT_ASSERT( Baz::GetNumber() == 0 );
}

