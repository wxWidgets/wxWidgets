///////////////////////////////////////////////////////////////////////////////
// Name:        tests/hashes/hashes.cpp
// Purpose:     wxArray unit test
// Author:      Vadim Zeitlin, Mattia Barbon
// Created:     2004-05-16
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

#include "wx/hash.h"
#include "wx/hashmap.h"
#include "wx/hashset.h"

// --------------------------------------------------------------------------
// helper class for typed/untyped wxHashTable
// --------------------------------------------------------------------------

struct Foo
{
    Foo(int n_) { n = n_; count++; }
    ~Foo() { count--; }

    int n;

    static size_t count;
};

size_t Foo::count = 0;

struct FooObject : public wxObject
{
    FooObject(int n_) { n = n_; count++; }
    ~FooObject() { count--; }

    int n;

    static size_t count;
};

size_t FooObject::count = 0;

// --------------------------------------------------------------------------
// test class
// --------------------------------------------------------------------------

class HashesTestCase : public CppUnit::TestCase
{
public:
    HashesTestCase() { }

private:
    CPPUNIT_TEST_SUITE( HashesTestCase );
        CPPUNIT_TEST( wxHashTableTest );
        CPPUNIT_TEST( wxUntypedHashTableDeleteContents );
        CPPUNIT_TEST( wxTypedHashTableTest );
        CPPUNIT_TEST( wxHashMapTest );
        CPPUNIT_TEST( wxHashSetTest );
    CPPUNIT_TEST_SUITE_END();

    void wxHashTableTest();
    void wxUntypedHashTableDeleteContents();
    void wxTypedHashTableTest();
    void wxHashMapTest();
    void wxHashSetTest();

    DECLARE_NO_COPY_CLASS(HashesTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( HashesTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( HashesTestCase, "HashesTestCase" );

void HashesTestCase::wxHashTableTest()
{
    const int COUNT = 100;

    {
        wxHashTable hash(wxKEY_INTEGER, 10), hash2(wxKEY_STRING);
        wxObject o;
        int i;

        for ( i = 0; i < COUNT; ++i )
            hash.Put(i, &o + i);

        hash.BeginFind();
        wxHashTable::compatibility_iterator it = hash.Next();
        i = 0;

        while (it)
        {
            ++i;
            it = hash.Next();
        }

        CPPUNIT_ASSERT( i == COUNT );

        for ( i = 99; i >= 0; --i )
            CPPUNIT_ASSERT( hash.Get(i) == &o + i );

        for ( i = 0; i < COUNT; ++i )
            hash.Put(i, &o + i + 20);

        for ( i = 99; i >= 0; --i )
            CPPUNIT_ASSERT( hash.Get(i) == &o + i);

        for ( i = 0; i < COUNT/2; ++i )
            CPPUNIT_ASSERT( hash.Delete(i) == &o + i);

        for ( i = COUNT/2; i < COUNT; ++i )
            CPPUNIT_ASSERT( hash.Get(i) == &o + i);

        for ( i = 0; i < COUNT/2; ++i )
            CPPUNIT_ASSERT( hash.Get(i) == &o + i + 20);

        for ( i = 0; i < COUNT/2; ++i )
            CPPUNIT_ASSERT( hash.Delete(i) == &o + i + 20);

        for ( i = 0; i < COUNT/2; ++i )
            CPPUNIT_ASSERT( hash.Get(i) == NULL);

        hash2.Put(_T("foo"), &o + 1);
        hash2.Put(_T("bar"), &o + 2);
        hash2.Put(_T("baz"), &o + 3);

        CPPUNIT_ASSERT(hash2.Get(_T("moo")) == NULL);
        CPPUNIT_ASSERT(hash2.Get(_T("bar")) == &o + 2);

        hash2.Put(_T("bar"), &o + 0);

        CPPUNIT_ASSERT(hash2.Get(_T("bar")) == &o + 2);
    }

    // and now some corner-case testing; 3 and 13 hash to the same bucket
    {
        wxHashTable hash(wxKEY_INTEGER, 10);
        wxObject dummy;

        hash.Put(3, &dummy);
        hash.Delete(3);

        CPPUNIT_ASSERT(hash.Get(3) == NULL);

        hash.Put(3, &dummy);
        hash.Put(13, &dummy);
        hash.Delete(3);

        CPPUNIT_ASSERT(hash.Get(3) == NULL);

        hash.Delete(13);

        CPPUNIT_ASSERT(hash.Get(13) == NULL);

        hash.Put(3, &dummy);
        hash.Put(13, &dummy);
        hash.Delete(13);

        CPPUNIT_ASSERT(hash.Get(13) == NULL);

        hash.Delete(3);

        CPPUNIT_ASSERT(hash.Get(3) == NULL);
    }

    // test for key + value access (specifically that supplying either
    // wrong key or wrong value returns NULL)
    {
        wxHashTable hash(wxKEY_INTEGER, 10);
        wxObject dummy;

        hash.Put(3, 7, &dummy + 7);
        hash.Put(4, 8, &dummy + 8);

        CPPUNIT_ASSERT(hash.Get(7) == NULL);
        CPPUNIT_ASSERT(hash.Get(3, 7) == &dummy + 7);
        CPPUNIT_ASSERT(hash.Get(4) == NULL);
        CPPUNIT_ASSERT(hash.Get(3) == NULL);
        CPPUNIT_ASSERT(hash.Get(8) == NULL);
        CPPUNIT_ASSERT(hash.Get(8, 4) == NULL);

        CPPUNIT_ASSERT(hash.Delete(7) == NULL);
        CPPUNIT_ASSERT(hash.Delete(3) == NULL);
        CPPUNIT_ASSERT(hash.Delete(3, 7) == &dummy + 7);
    }

}

void HashesTestCase::wxUntypedHashTableDeleteContents()
{
    // need a nested scope for destruction
    {
        wxHashTable hash;
        hash.DeleteContents(true);

        CPPUNIT_ASSERT( hash.GetCount() == 0 );
        CPPUNIT_ASSERT( FooObject::count == 0 );

        static const int hashTestData[] =
        {
            0, 1, 17, -2, 2, 4, -4, 345, 3, 3, 2, 1,
        };

        size_t n;
        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            hash.Put(hashTestData[n], n, new FooObject(n));
        }

        CPPUNIT_ASSERT( hash.GetCount() == WXSIZEOF(hashTestData) );
        CPPUNIT_ASSERT( FooObject::count == WXSIZEOF(hashTestData) );

        // delete from hash without deleting object
        FooObject* foo = (FooObject*)hash.Delete(0l);

        CPPUNIT_ASSERT( FooObject::count == WXSIZEOF(hashTestData) );
        delete foo;
        CPPUNIT_ASSERT( FooObject::count == WXSIZEOF(hashTestData) - 1 );
    }

    // hash destroyed
    CPPUNIT_ASSERT( FooObject::count == 0 );
}

#if WXWIN_COMPATIBILITY_2_4
WX_DECLARE_LIST(Foo, wxListFoos);
#endif

WX_DECLARE_HASH(Foo, wxListFoos, wxHashFoos);

#if WXWIN_COMPATIBILITY_2_4
#include "wx/listimpl.cpp"
WX_DEFINE_LIST(wxListFoos);
#endif

void HashesTestCase::wxTypedHashTableTest()
{
    // need a nested scope for destruction
    {
        wxHashFoos hash;
        hash.DeleteContents(true);

        CPPUNIT_ASSERT( hash.GetCount() == 0 );
        CPPUNIT_ASSERT( Foo::count == 0 );

        static const int hashTestData[] =
        {
            0, 1, 17, -2, 2, 4, -4, 345, 3, 3, 2, 1,
        };

        size_t n;
        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            hash.Put(hashTestData[n], n, new Foo(n));
        }

        CPPUNIT_ASSERT( hash.GetCount() == WXSIZEOF(hashTestData) );
        CPPUNIT_ASSERT( Foo::count == WXSIZEOF(hashTestData) );

        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            Foo *foo = hash.Get(hashTestData[n], n);

            CPPUNIT_ASSERT( foo != NULL );
            CPPUNIT_ASSERT( foo->n == (int)n );
        }

        // element not in hash
        CPPUNIT_ASSERT( hash.Get(1234) == NULL );
        CPPUNIT_ASSERT( hash.Get(1, 0) == NULL );

        // delete from hash without deleting object
        Foo* foo = hash.Delete(0);

        CPPUNIT_ASSERT( Foo::count == WXSIZEOF(hashTestData) );
        delete foo;
        CPPUNIT_ASSERT( Foo::count == WXSIZEOF(hashTestData) - 1 );
    }

    // hash destroyed
    CPPUNIT_ASSERT( Foo::count == 0 );
}

void HashesTestCase::wxHashMapTest()
{
}

void HashesTestCase::wxHashSetTest()
{
}
