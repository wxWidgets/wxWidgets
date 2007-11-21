///////////////////////////////////////////////////////////////////////////////
// Name:        tests/hashes/hashes.cpp
// Purpose:     wxHashTable, wxHashMap, wxHashSet unit test
// Author:      Vadim Zeitlin, Mattia Barbon
// Modified:    Mike Wetherell
// Created:     2004-05-16
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Vadim Zeitlin, Mattia Barbon, 2005 M. Wetherell
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

#if defined wxLongLong_t && !defined wxLongLongIsLong && \
        (!defined __VISUALC__ || __VISUALC__ > 1100)    // doesn't work on VC5
    #define TEST_LONGLONG
#endif

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
        CPPUNIT_TEST( StringHashMapTest );
        CPPUNIT_TEST( PtrHashMapTest );
        CPPUNIT_TEST( LongHashMapTest );
        CPPUNIT_TEST( ULongHashMapTest );
        CPPUNIT_TEST( UIntHashMapTest );
        CPPUNIT_TEST( IntHashMapTest );
        CPPUNIT_TEST( ShortHashMapTest );
        CPPUNIT_TEST( UShortHashMapTest );
#ifdef TEST_LONGLONG
        CPPUNIT_TEST( LLongHashMapTest );
        CPPUNIT_TEST( ULLongHashMapTest );
#endif
        CPPUNIT_TEST( wxHashSetTest );
    CPPUNIT_TEST_SUITE_END();

    void wxHashTableTest();
    void wxUntypedHashTableDeleteContents();
    void wxTypedHashTableTest();
    void StringHashMapTest();
    void PtrHashMapTest();
    void LongHashMapTest();
    void ULongHashMapTest();
    void UIntHashMapTest();
    void IntHashMapTest();
    void ShortHashMapTest();
    void UShortHashMapTest();
#ifdef TEST_LONGLONG
    void LLongHashMapTest();
    void ULLongHashMapTest();
#endif
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
WX_DEFINE_LIST(wxListFoos)
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

// test compilation of basic map types
WX_DECLARE_HASH_MAP( int*, int*, wxPointerHash, wxPointerEqual, myPtrHashMap );
WX_DECLARE_HASH_MAP( long, long, wxIntegerHash, wxIntegerEqual, myLongHashMap );
WX_DECLARE_HASH_MAP( unsigned long, unsigned, wxIntegerHash, wxIntegerEqual,
                     myUnsignedHashMap );
WX_DECLARE_HASH_MAP( unsigned int, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap1 );
WX_DECLARE_HASH_MAP( int, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap2 );
WX_DECLARE_HASH_MAP( short, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap3 );
WX_DECLARE_HASH_MAP( unsigned short, unsigned, wxIntegerHash, wxIntegerEqual,
                     myTestHashMap4 );

// same as:
// WX_DECLARE_HASH_MAP( wxString, wxString, wxStringHash, wxStringEqual,
//                      myStringHashMap );
WX_DECLARE_STRING_HASH_MAP(wxString, myStringHashMap);

#ifdef TEST_LONGLONG
    WX_DECLARE_HASH_MAP( wxLongLong_t, wxLongLong_t,
                         wxIntegerHash, wxIntegerEqual, myLLongHashMap );
    WX_DECLARE_HASH_MAP( wxULongLong_t, wxULongLong_t,
                         wxIntegerHash, wxIntegerEqual, myULLongHashMap );
#endif

// Helpers to generate a key value pair for item 'i', out of a total of 'count'
void MakeKeyValuePair(size_t i, size_t /*count*/, wxString& key, wxString& val)
{
    key.clear();
    key << i;
    val = wxT("A") + key + wxT("C");
}

// for integral keys generate a range of keys that will use all the bits of
// the key type
template <class IntT, class KeyT>
IntT MakeKey(size_t i, size_t count)
{
    IntT max = 1;
    max <<= sizeof(KeyT) * 8 - 2;
    max -= count / 4 + 1;
     
    return max / count * 4 * i + i / 3;
}

// make key/value pairs for integer types
template <class KeyT, class ValueT>
void MakeKeyValuePair(size_t i, size_t count, KeyT& key, ValueT& value)
{
    key = MakeKey<KeyT, KeyT>(i, count);
    value = wx_truncate_cast(ValueT, key);
}

// make key/values paris for pointer types
template <class T, class ValueT>
void MakeKeyValuePair(size_t i, size_t count, T*& key, ValueT& value)
{
    key = (T*)wxUIntToPtr(MakeKey<wxUIntPtr, T*>(i, count));
    value = wx_truncate_cast(ValueT, key);
}

// the test
template <class HashMapT>
void HashMapTest()
{
#if wxUSE_STL && defined HAVE_STL_HASH_MAP
    typedef typename HashMapT::value_type::second_type value_type;
#else
    typedef typename HashMapT::value_type::t2 value_type;
#endif
    typedef typename HashMapT::key_type key_type;
    typedef typename HashMapT::iterator Itor;

    HashMapT sh(0); // as small as possible
    key_type buf;
    value_type value;
    size_t i;
    const size_t count = 10000;

    // init with some data
    for( i = 0; i < count; ++i )
    {
        MakeKeyValuePair(i, count, buf, value);
        sh[buf] = value;
    }

    // test that insertion worked
    CPPUNIT_ASSERT( sh.size() == count );

    for( i = 0; i < count; ++i )
    {
        MakeKeyValuePair(i, count, buf, value);
        CPPUNIT_ASSERT( sh[buf] == value );
    }

    // check that iterators work
    Itor it;
    for( i = 0, it = sh.begin(); it != sh.end(); ++it, ++i )
    {
        CPPUNIT_ASSERT( i != count );
        CPPUNIT_ASSERT( it->second == sh[it->first] );
    }

    CPPUNIT_ASSERT( sh.size() == i );

    // test copy ctor, assignment operator
    HashMapT h1( sh ), h2( 0 );
    h2 = sh;

    for( i = 0, it = sh.begin(); it != sh.end(); ++it, ++i )
    {
        CPPUNIT_ASSERT( h1[it->first] == it->second );
        CPPUNIT_ASSERT( h2[it->first] == it->second );
    }

    // other tests
    for( i = 0; i < count; ++i )
    {
        MakeKeyValuePair(i, count, buf, value);
        size_t sz = sh.size();

        // test find() and erase(it)
        if( i < 100 )
        {
            it = sh.find( buf );
            CPPUNIT_ASSERT( it != sh.end() );

            sh.erase( it );

            CPPUNIT_ASSERT( sh.find( buf ) == sh.end() );
        }
        else
        // test erase(key)
        {
            size_t c = sh.erase( buf );
            CPPUNIT_ASSERT( c == 1 );
            CPPUNIT_ASSERT( sh.find( buf ) == sh.end() );
        }

        // count should decrease
        CPPUNIT_ASSERT( sh.size() == sz - 1 );
    }
}

void HashesTestCase::StringHashMapTest() { HashMapTest<myStringHashMap>();   }
void HashesTestCase::PtrHashMapTest()    { HashMapTest<myPtrHashMap>();      }
void HashesTestCase::LongHashMapTest()   { HashMapTest<myLongHashMap>();     }
void HashesTestCase::ULongHashMapTest()  { HashMapTest<myUnsignedHashMap>(); }
void HashesTestCase::UIntHashMapTest()   { HashMapTest<myTestHashMap1>();    }
void HashesTestCase::IntHashMapTest()    { HashMapTest<myTestHashMap2>();    }
void HashesTestCase::ShortHashMapTest()  { HashMapTest<myTestHashMap3>();    }
void HashesTestCase::UShortHashMapTest() { HashMapTest<myTestHashMap4>();    }

#ifdef TEST_LONGLONG
void HashesTestCase::LLongHashMapTest()  { HashMapTest<myLLongHashMap>();    }
void HashesTestCase::ULLongHashMapTest() { HashMapTest<myULLongHashMap>();   }
#endif

// test compilation of basic set types
WX_DECLARE_HASH_SET( int*, wxPointerHash, wxPointerEqual, myPtrHashSet );
WX_DECLARE_HASH_SET( long, wxIntegerHash, wxIntegerEqual, myLongHashSet );
WX_DECLARE_HASH_SET( unsigned long, wxIntegerHash, wxIntegerEqual,
                     myUnsignedHashSet );
WX_DECLARE_HASH_SET( unsigned int, wxIntegerHash, wxIntegerEqual,
                     myTestHashSet1 );
WX_DECLARE_HASH_SET( int, wxIntegerHash, wxIntegerEqual,
                     myTestHashSet2 );
WX_DECLARE_HASH_SET( short, wxIntegerHash, wxIntegerEqual,
                     myTestHashSet3 );
WX_DECLARE_HASH_SET( unsigned short, wxIntegerHash, wxIntegerEqual,
                     myTestHashSet4 );
WX_DECLARE_HASH_SET( wxString, wxStringHash, wxStringEqual,
                     myTestHashSet5 );

struct MyStruct
{
    int* ptr;
    wxString str;
};

class MyHash
{
public:
    unsigned long operator()(const MyStruct& s) const
        { return m_dummy(s.ptr); }
    MyHash& operator=(const MyHash&) { return *this; }
private:
    wxPointerHash m_dummy;
};

class MyEqual
{
public:
    bool operator()(const MyStruct& s1, const MyStruct& s2) const
        { return s1.ptr == s2.ptr; }
    MyEqual& operator=(const MyEqual&) { return *this; }
};

WX_DECLARE_HASH_SET( MyStruct, MyHash, MyEqual, mySet );

typedef myTestHashSet5 wxStringHashSet;

void HashesTestCase::wxHashSetTest()
{
    wxStringHashSet set1;

    set1.insert( _T("abc") );

    CPPUNIT_ASSERT( set1.size() == 1 );

    set1.insert( _T("bbc") );
    set1.insert( _T("cbc") );

    CPPUNIT_ASSERT( set1.size() == 3 );

    set1.insert( _T("abc") );

    CPPUNIT_ASSERT( set1.size() == 3 );

    mySet set2;
    int dummy;
    MyStruct tmp;

    tmp.ptr = &dummy; tmp.str = _T("ABC");
    set2.insert( tmp );
    tmp.ptr = &dummy + 1;
    set2.insert( tmp );
    tmp.ptr = &dummy; tmp.str = _T("CDE");
    set2.insert( tmp );

    CPPUNIT_ASSERT( set2.size() == 2 );

    mySet::iterator it = set2.find( tmp );

    CPPUNIT_ASSERT( it != set2.end() );
    CPPUNIT_ASSERT( it->ptr == &dummy );
    CPPUNIT_ASSERT( it->str == _T("ABC") );
}
