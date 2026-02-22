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

#include <memory>

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

namespace
{

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

} // anonymous namespace

#include "wx/list.h"

WX_DECLARE_LIST(Baz, wxListBazs);
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxListBazs)

WX_DECLARE_LIST(int, wxListInt);
WX_DEFINE_LIST(wxListInt)

TEST_CASE("wxList", "[list]")
{
    wxListInt list1;
    int dummy[5];
    size_t i;

    for ( i = 0; i < WXSIZEOF(dummy); ++i )
        list1.Append(dummy + i);

    CHECK( list1.GetCount()  == WXSIZEOF(dummy) );
    CHECK( list1.Item(3)->GetData()  == dummy + 3 );
    CHECK( list1.Find(dummy + 4) );

    wxListInt::compatibility_iterator node;
    for ( i = 0, node = list1.GetFirst(); node; ++i, node = node->GetNext() )
    {
        CHECK( node->GetData()  == dummy + i );
    }

    CHECK( list1.GetCount()  == i );

    list1.Insert(dummy + 0);
    list1.Insert(1, dummy + 1);
    list1.Insert(list1.GetFirst()->GetNext()->GetNext(), dummy + 2);

    for ( i = 0, node = list1.GetFirst(); i < 3; ++i, node = node->GetNext() )
    {
        int* t = node->GetData();
        CHECK( t  == dummy + i );
    }
}

TEST_CASE("wxList::std", "[list][std]")
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
        CHECK( *it == i + &i );
    }

    for ( rit = list1.rbegin(), ren = list1.rend(), i = 4;
          rit != ren; ++rit, --i )
    {
        CHECK( *rit == i + &i );
    }

    CHECK( *list1.rbegin() == *--list1.end() );
    CHECK( *list1.begin() == *--list1.rend() );
    CHECK( *list1.begin() == *--++list1.begin() );
    CHECK( *list1.rbegin() == *--++list1.rbegin() );

    CHECK( list1.front() == &i );
    CHECK( list1.back() == &i + 4 );

    list1.erase(list1.begin());
    list1.erase(--list1.end());

    for ( it = list1.begin(), en = list1.end(), i = 1;
          it != en; ++it, ++i )
    {
        CHECK( *it == i + &i );
    }

    list1.clear();
    CHECK( list1.empty() );

    it = list1.insert(list1.end(), (int *)1);
    CHECK( *it  == (int *)1 );
    CHECK( it == list1.begin() );
    CHECK( list1.front()  == (int *)1 );

    it = list1.insert(list1.end(), (int *)2);
    CHECK( *it  == (int *)2 );
    CHECK( ++it == list1.end() );
    CHECK( list1.back()  == (int *)2 );

    it = list1.begin();
    wxListInt::iterator it2 = list1.insert(++it, (int *)3);
    CHECK( *it2  == (int *)3 );

    it = list1.begin();
    it = list1.erase(++it, list1.end());
    CHECK( list1.size()  == 1 );
    CHECK( it == list1.end() );

    wxListInt list2;
    list2.push_back((int *)3);
    list2.push_back((int *)4);
    list1.insert(list1.begin(), list2.begin(), list2.end());
    CHECK( list1.size()  == 3 );
    CHECK( list1.front()  == (int *)3 );

    list1.insert(list1.end(), list2.begin(), list2.end());
    CHECK( list1.size()  == 5 );
    CHECK( list1.back()  == (int *)4 );

    // Sort the list in the reverse order.
    list1.Sort([](const void *a, const void *b) -> int {
        return *static_cast<const int*>(b) - *static_cast<const int*>(a);
    });
    REQUIRE( list1.size() == 5 );
    CHECK( list1.front() == (int *)4 );
    CHECK( list1.back() == (int *)1 );
}

TEST_CASE("wxList::ctor", "[list]")
{
    {
        wxListBazs list1;
        list1.Append(new Baz(wxT("first")));
        list1.Append(new Baz(wxT("second")));

        CHECK( list1.GetCount() == 2 );
        CHECK( Baz::GetNumber() == 2 );

        wxListBazs list2;
        list2 = list1;

        CHECK( list1.GetCount() == 2 );
        CHECK( list2.GetCount() == 2 );
        CHECK( Baz::GetNumber() == 2 );

        list1.DeleteContents(true);
    }

    CHECK( Baz::GetNumber() == 0 );
}

TEST_CASE("wxList::iterator::cmp", "[list]")
{
    int dummy[2];

    wxListInt list;
    list.push_back(dummy);
    list.push_back(dummy + 1);

    wxListInt::compatibility_iterator it = list.GetFirst();
    CHECK( it == it );
    CHECK( it != nullptr );

    const wxListInt::compatibility_iterator last = list.GetLast();
    CHECK( last != nullptr );
    CHECK( it != last );

    it = list.Item(1);
    CHECK( it == last );

    it = list.Find(dummy + 2);
    CHECK( it == nullptr );
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
    std::unique_ptr<ListElementBase> elb(el);

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
