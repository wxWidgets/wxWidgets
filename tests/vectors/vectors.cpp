///////////////////////////////////////////////////////////////////////////////
// Name:        tests/vectors/vectors.cpp
// Purpose:     wxVector<T> unit test
// Author:      Vaclav Slavik
// Created:     2007-07-07
// RCS-ID:      $Id$
// Copyright:   (c) 2007 Vaclav Slavik
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

#include "wx/vector.h"

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
    ~SelfPointingObject() { CPPUNIT_ASSERT( this == m_self ); }

    // the assignment operator should not modify our "this" pointer so
    // implement it just to prevent the default version from doing it
    SelfPointingObject& operator=(const SelfPointingObject&) { return *this; }

private:
    SelfPointingObject *m_self;
};

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class VectorsTestCase : public CppUnit::TestCase
{
public:
    VectorsTestCase() {}

private:
    CPPUNIT_TEST_SUITE( VectorsTestCase );
        CPPUNIT_TEST( PushPopTest );
        CPPUNIT_TEST( Insert );
        CPPUNIT_TEST( Erase );
        CPPUNIT_TEST( Iterators );
        CPPUNIT_TEST( Objects );
        CPPUNIT_TEST( NonPODs );
        CPPUNIT_TEST( Resize );
        CPPUNIT_TEST( Swap );
        CPPUNIT_TEST( Sort );
    CPPUNIT_TEST_SUITE_END();

    void PushPopTest();
    void Insert();
    void Erase();
    void Iterators();
    void Objects();
    void NonPODs();
    void Resize();
    void Swap();
    void Sort();

    DECLARE_NO_COPY_CLASS(VectorsTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( VectorsTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( VectorsTestCase, "VectorsTestCase" );

void VectorsTestCase::PushPopTest()
{
    wxVector<int> v;

    CPPUNIT_ASSERT( v.size() == 0 );
    v.push_back(1);
    CPPUNIT_ASSERT( v.size() == 1 );
    v.push_back(2);
    CPPUNIT_ASSERT( v.size() == 2 );
    v.push_back(42);
    CPPUNIT_ASSERT( v.size() == 3 );

    CPPUNIT_ASSERT( v[0] == 1 );
    CPPUNIT_ASSERT( v[1] == 2 );
    CPPUNIT_ASSERT( v[2] == 42 );

    v.pop_back();
    CPPUNIT_ASSERT( v.size() == 2 );
    CPPUNIT_ASSERT( v[0] == 1 );
    CPPUNIT_ASSERT( v[1] == 2 );

    v.pop_back();
    CPPUNIT_ASSERT( v.size() == 1 );
    CPPUNIT_ASSERT( v[0] == 1 );

    v.pop_back();
    CPPUNIT_ASSERT( v.size() == 0 );
    CPPUNIT_ASSERT( v.empty() );

    wxVector<char> vEmpty;
}

void VectorsTestCase::Insert()
{
    wxVector<char> v;

    v.insert(v.end(), 'a');
    CPPUNIT_ASSERT( v.size() == 1 );
    CPPUNIT_ASSERT( v[0] == 'a' );

    v.insert(v.end(), 'b');
    CPPUNIT_ASSERT( v.size() == 2 );
    CPPUNIT_ASSERT( v[0] == 'a' );
    CPPUNIT_ASSERT( v[1] == 'b' );

    v.insert(v.begin(), '0');
    CPPUNIT_ASSERT( v.size() == 3 );
    CPPUNIT_ASSERT( v[0] == '0' );
    CPPUNIT_ASSERT( v[1] == 'a' );
    CPPUNIT_ASSERT( v[2] == 'b' );

    v.insert(v.begin() + 2, 'X');
    CPPUNIT_ASSERT( v.size() == 4 );
    CPPUNIT_ASSERT( v[0] == '0' );
    CPPUNIT_ASSERT( v[1] == 'a' );
    CPPUNIT_ASSERT( v[2] == 'X' );
    CPPUNIT_ASSERT( v[3] == 'b' );
}

void VectorsTestCase::Erase()
{
    wxVector<int> v;

    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    CPPUNIT_ASSERT( v.size() == 4 );

    v.erase(v.begin(), v.end()-1);
    CPPUNIT_ASSERT( v.size() == 1 );
    CPPUNIT_ASSERT( v[0] == 4 );

    v.clear();
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);
    CPPUNIT_ASSERT( v.size() == 4 );

    v.erase(v.begin());
    CPPUNIT_ASSERT( v.size() == 3 );
    CPPUNIT_ASSERT( v[0] == 2 );
    CPPUNIT_ASSERT( v[1] == 3 );
    CPPUNIT_ASSERT( v[2] == 4 );
}

void VectorsTestCase::Iterators()
{
    wxVector<int> v;
    v.push_back(1);
    v.push_back(2);
    v.push_back(3);
    v.push_back(4);

    int value = 1;
    for ( wxVector<int>::iterator i = v.begin(); i != v.end(); ++i, ++value )
    {
        CPPUNIT_ASSERT_EQUAL( value, *i );
    }
}

void VectorsTestCase::Objects()
{
    wxVector<CountedObject> v;
    v.push_back(CountedObject(1));
    v.push_back(CountedObject(2));
    v.push_back(CountedObject(3));

    v.erase(v.begin());
    CPPUNIT_ASSERT_EQUAL( 2, v.size() );
    CPPUNIT_ASSERT_EQUAL( 2, CountedObject::GetCount() );

    v.clear();
    CPPUNIT_ASSERT_EQUAL( 0, CountedObject::GetCount() );
}

void VectorsTestCase::NonPODs()
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

void VectorsTestCase::Resize()
{
    wxVector<CountedObject> v;
    v.resize(3);

    CPPUNIT_ASSERT_EQUAL( 3, v.size() );
    CPPUNIT_ASSERT_EQUAL( 3, CountedObject::GetCount() );
    CPPUNIT_ASSERT_EQUAL( 0, v[0].GetValue() );
    CPPUNIT_ASSERT_EQUAL( 0, v[1].GetValue() );
    CPPUNIT_ASSERT_EQUAL( 0, v[2].GetValue() );

    v.resize(1);
    CPPUNIT_ASSERT_EQUAL( 1, v.size() );
    CPPUNIT_ASSERT_EQUAL( 1, CountedObject::GetCount() );

    v.resize(4, CountedObject(17));
    CPPUNIT_ASSERT_EQUAL( 4, v.size() );
    CPPUNIT_ASSERT_EQUAL( 4, CountedObject::GetCount() );
    CPPUNIT_ASSERT_EQUAL( 0, v[0].GetValue() );
    CPPUNIT_ASSERT_EQUAL( 17, v[1].GetValue() );
    CPPUNIT_ASSERT_EQUAL( 17, v[2].GetValue() );
    CPPUNIT_ASSERT_EQUAL( 17, v[3].GetValue() );
}

void VectorsTestCase::Swap()
{
    wxVector<int> v1, v2;
    v1.push_back(17);
    v1.swap(v2);
    CPPUNIT_ASSERT( v1.empty() );
    CPPUNIT_ASSERT_EQUAL( 1, v2.size() );
    CPPUNIT_ASSERT_EQUAL( 17, v2[0] );

    v1.push_back(9);
    v2.swap(v1);
    CPPUNIT_ASSERT_EQUAL( 1, v1.size() );
    CPPUNIT_ASSERT_EQUAL( 17, v1[0] );
    CPPUNIT_ASSERT_EQUAL( 1, v2.size() );
    CPPUNIT_ASSERT_EQUAL( 9, v2[0] );

    v2.clear();
    v1.swap(v2);
    CPPUNIT_ASSERT( v1.empty() );
}


void VectorsTestCase::Sort()
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
        CPPUNIT_ASSERT( v[idx-1] <= v[idx] );
    }
}
