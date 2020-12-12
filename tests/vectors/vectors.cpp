///////////////////////////////////////////////////////////////////////////////
// Name:        tests/vectors/vectors.cpp
// Purpose:     wxVector<T> unit test
// Author:      Vaclav Slavik
// Created:     2007-07-07
// Copyright:   (c) 2007 Vaclav Slavik
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/vector.h"

#if wxUSE_STD_CONTAINERS_COMPATIBLY
    #include <vector>
#endif // wxUSE_STD_CONTAINERS_COMPATIBLY

// ----------------------------------------------------------------------------
// simple class capable of detecting leaks of its objects
// ----------------------------------------------------------------------------

class CountedObject
{
public:
    CountedObject(int n = 0) : m_n(n) { ms_count++; }
    CountedObject(const CountedObject& co) : m_n(co.m_n) { ms_count++; }
    ~CountedObject() { ms_count--; }

    int GetValue() const { return m_n; }

    static int GetCount() { return ms_count; }

private:
    static int ms_count;

    int m_n;
};

int CountedObject::ms_count = 0;

// ----------------------------------------------------------------------------
// simple class capable of checking its "this" pointer validity
// ----------------------------------------------------------------------------

class SelfPointingObject
{
public:
    SelfPointingObject() { m_self = this; }
    SelfPointingObject(const SelfPointingObject&) { m_self = this; }
    ~SelfPointingObject() { CHECK( this == m_self ); }

    // the assignment operator should not modify our "this" pointer so
    // implement it just to prevent the default version from doing it
    SelfPointingObject& operator=(const SelfPointingObject&) { return *this; }

private:
    SelfPointingObject *m_self;
};

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

TEST_CASE("wxVector::Push/Pop", "[vector][push_back][pop_back]")
{
    wxVector<int> v;

    CHECK( v.size() == 0 );
    v.push_back(1);
    CHECK( v.size() == 1 );
    v.push_back(2);
    CHECK( v.size() == 2 );
    v.push_back(42);
    CHECK( v.size() == 3 );

    CHECK( v[0] == 1 );
    CHECK( v[1] == 2 );
    CHECK( v[2] == 42 );

    v.pop_back();
    CHECK( v.size() == 2 );
    CHECK( v[0] == 1 );
    CHECK( v[1] == 2 );

    v.pop_back();
    CHECK( v.size() == 1 );
    CHECK( v[0] == 1 );

    v.pop_back();
    CHECK( v.size() == 0 );
    CHECK( v.empty() );

    wxVector<char> vEmpty;
}

TEST_CASE("wxVector::Insert", "[vector][insert]")
{
    wxVector<char> v;

    v.insert(v.end(), 'a');
    CHECK( v.size() == 1 );
    CHECK( v[0] == 'a' );

    v.insert(v.end(), 'b');
    CHECK( v.size() == 2 );
    CHECK( v[0] == 'a' );
    CHECK( v[1] == 'b' );

    v.insert(v.begin(), '0');
    CHECK( v.size() == 3 );
    CHECK( v[0] == '0' );
    CHECK( v[1] == 'a' );
    CHECK( v[2] == 'b' );

    v.insert(v.begin() + 2, 'X');
    CHECK( v.size() == 4 );
    CHECK( v[0] == '0' );
    CHECK( v[1] == 'a' );
    CHECK( v[2] == 'X' );
    CHECK( v[3] == 'b' );

    v.insert(v.begin() + 3, 3, 'Z');
    REQUIRE( v.size() == 7 );
    CHECK( v[0] == '0' );
    CHECK( v[1] == 'a' );
    CHECK( v[2] == 'X' );
    CHECK( v[3] == 'Z' );
    CHECK( v[4] == 'Z' );
    CHECK( v[5] == 'Z' );
    CHECK( v[6] == 'b' );
}

TEST_CASE("wxVector::Erase", "[vector][erase]")
{
    wxVector<int> v;

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    CHECK( v.size() == 4 );

    v.erase(v.begin(), v.end()-1);
    CHECK( v.size() == 1 );
    CHECK( v[0] == 4 );

    v.clear();
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    CHECK( v.size() == 4 );

    v.erase(v.begin());
    CHECK( v.size() == 3 );
    CHECK( v[0] == 2 );
    CHECK( v[1] == 3 );
    CHECK( v[2] == 4 );
}

TEST_CASE("wxVector::Iterators", "[vector][iterator]")
{
    wxVector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);

    int value = 1;
    for ( wxVector<int>::iterator i = v.begin(); i != v.end(); ++i, ++value )
    {
        CHECK( *i == value );
    }
}

TEST_CASE("wxVector::Objects", "[vector]")
{
    wxVector<CountedObject> v;
    v.push_back(CountedObject(1));
    v.push_back(CountedObject(2));
    v.push_back(CountedObject(3));

    v.erase(v.begin());
    CHECK( v.size() == 2 );
    CHECK( CountedObject::GetCount() == 2 );

    v.clear();
    CHECK( CountedObject::GetCount() == 0 );
}

TEST_CASE("wxVector::NonPODs", "[vector]")
{
    wxVector<SelfPointingObject> v;
    v.push_back(SelfPointingObject());
    v.push_back(SelfPointingObject());
    v.push_back(SelfPointingObject());

    v.erase(v.begin());
    v.clear();

    // try the same with wxString, which is not POD, but is implemented in
    // a movable way (this won't assert, but would crash or show some memory
    // problems under Valgrind if wxString couldn't be safely moved with
    // memmove()):
    wxVector<wxString> vs;
    vs.push_back("one");
    vs.push_back("two");
    vs.push_back("three");

    vs.erase(vs.begin());
    vs.clear();
}

TEST_CASE("wxVector::Resize", "[vector][resize]")
{
    wxVector<CountedObject> v;
    v.resize(3);

    CHECK( v.size() == 3 );
    CHECK( CountedObject::GetCount() == 3 );
    CHECK( v[0].GetValue() == 0 );
    CHECK( v[1].GetValue() == 0 );
    CHECK( v[2].GetValue() == 0 );

    v.resize(1);
    CHECK( v.size() == 1 );
    CHECK( CountedObject::GetCount() == 1 );

    v.resize(4, CountedObject(17));
    CHECK( v.size() == 4 );
    CHECK( CountedObject::GetCount() == 4 );
    CHECK( v[0].GetValue() == 0 );
    CHECK( v[1].GetValue() == 17 );
    CHECK( v[2].GetValue() == 17 );
    CHECK( v[3].GetValue() == 17 );
}

TEST_CASE("wxVector::Swap", "[vector][swap]")
{
    wxVector<int> v1, v2;
    v1.push_back(17);
    v1.swap(v2);
    CHECK( v1.empty() );
    CHECK( v2.size() == 1 );
    CHECK( v2[0] == 17 );

    v1.push_back(9);
    v2.swap(v1);
    CHECK( v1.size() == 1 );
    CHECK( v1[0] == 17 );
    CHECK( v2.size() == 1 );
    CHECK( v2[0] == 9 );

    v2.clear();
    v1.swap(v2);
    CHECK( v1.empty() );
}

TEST_CASE("wxVector::Sort", "[vector][sort]")
{
    size_t  idx;
    wxVector<int> v;

    v.push_back(5);
    v.push_back(7);
    v.push_back(2);
    v.push_back(9);
    v.push_back(4);
    v.push_back(1);
    v.push_back(3);
    v.push_back(8);
    v.push_back(0);
    v.push_back(6);

    wxVectorSort(v);

    for (idx=1; idx<v.size(); idx++)
    {
        CHECK( v[idx-1] <= v[idx] );
    }
}

TEST_CASE("wxVector::Contains", "[vector][contains]")
{
    wxVector<int> v;
    CHECK( !wxVectorContains(v, 0) );

    v.push_back(3);
    CHECK( wxVectorContains(v, 3) );

    v.push_back(2);
    v.push_back(3);

    CHECK( wxVectorContains(v, 2) );
    CHECK( wxVectorContains(v, 3) );
    CHECK( !wxVectorContains(v, 1) );
}

TEST_CASE("wxVector::operator==", "[vector][compare]")
{
    wxVector<wxString> v1, v2;
    CHECK( v1 == v2 );
    CHECK( !(v1 != v2) );

    v1.push_back("foo");
    CHECK( v1 != v2 );

    v2.push_back("foo");
    CHECK( v1 == v2 );

    v1.push_back("bar");
    v2.push_back("baz");
    CHECK( v1 != v2 );
}

TEST_CASE("wxVector::reverse_iterator", "[vector][reverse_iterator]")
{
    wxVector<int> v;
    for ( int i = 0; i < 10; ++i )
        v.push_back(i + 1);

    const wxVector<int>::reverse_iterator rb = v.rbegin();
    const wxVector<int>::reverse_iterator re = v.rend();
    CHECK( re - rb == 10 );

    wxVector<int>::reverse_iterator ri = rb;
    ++ri;
    CHECK( ri - rb == 1 );
    CHECK( re - ri == 9 );

    ri = rb + 2;
    CHECK( ri - rb == 2 );
    CHECK( re - ri == 8 );

    CHECK( rb < ri );
    CHECK( rb <= ri );
    CHECK( ri <= ri );
    CHECK( ri >= ri );
    CHECK( ri < re );
    CHECK( ri <= re );

    CHECK( rb.base() == v.end() );
    CHECK( re.base() == v.begin() );
    CHECK( *ri.base() == 9 );

#if wxUSE_STD_CONTAINERS_COMPATIBLY
    std::vector<int> stdvec(rb, re);
    REQUIRE( stdvec.size() == 10 );
    CHECK( stdvec[0] == 10 );
    CHECK( stdvec[9] == 1 );
#endif // wxUSE_STD_CONTAINERS_COMPATIBLY
}

TEST_CASE("wxVector::capacity", "[vector][capacity][shrink_to_fit]")
{
    wxVector<int> v;
    CHECK( v.capacity() == 0 );

    v.push_back(0);
    // When using the standard library vector, we don't know what growth
    // strategy it uses, so we can't rely on the stricter check passing, but
    // with our own one we can, allowing us to check that shrink_to_fit()
    // really shrinks the capacity below.
#if !wxUSE_STD_CONTAINERS
    CHECK( v.capacity() > 1 );
#else
    CHECK( v.capacity() >= 1 );
#endif

    // There is no shrink_to_fit() in STL build when not using C++11.
#if !wxUSE_STD_CONTAINERS || __cplusplus >= 201103L || wxCHECK_VISUALC_VERSION(10)
    v.shrink_to_fit();
    CHECK( v.capacity() == 1 );

    v.erase(v.begin());
    CHECK( v.capacity() == 1 );

    v.shrink_to_fit();
    CHECK( v.capacity() == 0 );
#endif
}
