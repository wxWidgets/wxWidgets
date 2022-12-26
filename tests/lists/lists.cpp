///////////////////////////////////////////////////////////////////////////////
// Name:        tests/lists/lists.cpp
// Purpose:     wxList unit test
// Author:      Vadim Zeitlin, Mattia Barbon
// Created:     2004-12-08
// Copyright:   (c) 2004 Vadim Zeitlin, Mattia Barbon
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/list.h"
#include "wx/scopedptr.h"

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

    wxDECLARE_NO_COPY_CLASS(ListsTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( ListsTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( ListsTestCase, "ListsTestCase" );

class Baz // Foo is already taken in the hash test
{
public:
    Baz(const wxString& name) : m_name(name) { ms_bars++; }
    Baz(const Baz& bar) : m_name(bar.m_name) { ms_bars++; }
   ~Baz() { ms_bars--; }

   static size_t GetNumber() { return ms_bars; }

   const wxChar *GetName() const { return m_name.c_str(); }

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
    size_t i;

    for ( i = 0; i < WXSIZEOF(dummy); ++i )
        list1.Append(dummy + i);

    CPPUNIT_ASSERT_EQUAL( WXSIZEOF(dummy), list1.GetCount() );
    CPPUNIT_ASSERT_EQUAL( dummy + 3, list1.Item(3)->GetData() );
    CPPUNIT_ASSERT( list1.Find(dummy + 4) );

    wxListInt::compatibility_iterator node;
    for ( i = 0, node = list1.GetFirst(); node; ++i, node = node->GetNext() )
    {
        CPPUNIT_ASSERT_EQUAL( dummy + i, node->GetData() );
    }

    CPPUNIT_ASSERT_EQUAL( i, list1.GetCount() );

    list1.Insert(dummy + 0);
    list1.Insert(1, dummy + 1);
    list1.Insert(list1.GetFirst()->GetNext()->GetNext(), dummy + 2);

    for ( i = 0, node = list1.GetFirst(); i < 3; ++i, node = node->GetNext() )
    {
        int* t = node->GetData();
        CPPUNIT_ASSERT_EQUAL( dummy + i, t );
    }
}

void ListsTestCase::wxStdListTest()
{
    wxListInt list1;
    wxListInt::iterator it, en;
    wxListInt::reverse_iterator rit, ren;
    int buf[4];
    int& i = buf[0];
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

    CPPUNIT_ASSERT( *list1.rbegin() == *--list1.end() );
    CPPUNIT_ASSERT( *list1.begin() == *--list1.rend() );
    CPPUNIT_ASSERT( *list1.begin() == *--++list1.begin() );
    CPPUNIT_ASSERT( *list1.rbegin() == *--++list1.rbegin() );

    CPPUNIT_ASSERT( list1.front() == &i );
    CPPUNIT_ASSERT( list1.back() == &i + 4 );

    list1.erase(list1.begin());
    list1.erase(--list1.end());

    for ( it = list1.begin(), en = list1.end(), i = 1;
          it != en; ++it, ++i )
    {
        CPPUNIT_ASSERT( *it == i + &i );
    }

    list1.clear();
    CPPUNIT_ASSERT( list1.empty() );

    it = list1.insert(list1.end(), (int *)1);
    CPPUNIT_ASSERT_EQUAL( (int *)1, *it );
    CPPUNIT_ASSERT( it == list1.begin() );
    CPPUNIT_ASSERT_EQUAL( (int *)1, list1.front() );

    it = list1.insert(list1.end(), (int *)2);
    CPPUNIT_ASSERT_EQUAL( (int *)2, *it );
    CPPUNIT_ASSERT( ++it == list1.end() );
    CPPUNIT_ASSERT_EQUAL( (int *)2, list1.back() );

    it = list1.begin();
    wxListInt::iterator it2 = list1.insert(++it, (int *)3);
    CPPUNIT_ASSERT_EQUAL( (int *)3, *it2 );

    it = list1.begin();
    it = list1.erase(++it, list1.end());
    CPPUNIT_ASSERT_EQUAL( 1, list1.size() );
    CPPUNIT_ASSERT( it == list1.end() );

    wxListInt list2;
    list2.push_back((int *)3);
    list2.push_back((int *)4);
    list1.insert(list1.begin(), list2.begin(), list2.end());
    CPPUNIT_ASSERT_EQUAL( 3, list1.size() );
    CPPUNIT_ASSERT_EQUAL( (int *)3, list1.front() );

    list1.insert(list1.end(), list2.begin(), list2.end());
    CPPUNIT_ASSERT_EQUAL( 5, list1.size() );
    CPPUNIT_ASSERT_EQUAL( (int *)4, list1.back() );
}

void ListsTestCase::wxListCtorTest()
{
    {
        wxListBazs list1;
        list1.Append(new Baz(wxT("first")));
        list1.Append(new Baz(wxT("second")));

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

// Check for WX_DECLARE_LIST_3 which is used to define wxWindowList: we can't
// use this class itself here, as it's in the GUI library, so declare something
// similar.
struct ListElementBase
{
    virtual ~ListElementBase() { }
};

struct ListElement : ListElementBase
{
    explicit ListElement(int n) : m_n(n) { }

    int m_n;
};

WX_DECLARE_LIST_3(ListElement, ListElementBase, ElementsList, ElementsListNode, class);

#if wxUSE_STD_CONTAINERS

#include "wx/listimpl.cpp"
WX_DEFINE_LIST(ElementsList)

#else // !wxUSE_STD_CONTAINERS

void ElementsListNode::DeleteData()
{
    delete static_cast<ListElement *>(GetData());
}

#endif // wxUSE_STD_CONTAINERS/!wxUSE_STD_CONTAINERS

TEST_CASE("wxWindowList::Find", "[list]")
{
    ListElement* const el = new ListElement(17);
    wxScopedPtr<ListElementBase> elb(el);

    ElementsList l;
    l.Append(el);

    // We should be able to call Find() with the base class pointer.
    ElementsList::compatibility_iterator it = l.Find(elb.get());
    CHECK( it == l.GetFirst() );
}

#include <list>

// Check that we convert wxList to std::list using the latter's ctor taking 2
// iterators: this used to be broken in C++11 because wxList iterators didn't
// fully implement input iterator requirements.
TEST_CASE("wxList::iterator", "[list][std][iterator]")
{
    Baz baz1("one"),
        baz2("two");

    wxListBazs li;
    li.push_back(&baz1);
    li.push_back(&baz2);

    std::list<Baz*> stdli(li.begin(), li.end());
    CHECK( stdli.size() == 2 );

    const wxListBazs cli;
    CHECK( std::list<Baz*>(cli.begin(), cli.end()).empty() );
}
