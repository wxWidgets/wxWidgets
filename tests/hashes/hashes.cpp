///////////////////////////////////////////////////////////////////////////////
// Name:        tests/hashes/hashes.cpp
// Purpose:     wxHashTable, wxHashMap, wxHashSet unit test
// Author:      Vadim Zeitlin, Mattia Barbon
// Modified:    Mike Wetherell
// Created:     2004-05-16
// Copyright:   (c) 2004 Vadim Zeitlin, Mattia Barbon, 2005 M. Wetherell
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include "wx/hash.h"
#include "wx/hashmap.h"
#include "wx/hashset.h"

#if defined wxLongLong_t && !defined wxLongLongIsLong
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
// tests
// --------------------------------------------------------------------------

TEST_CASE("Hashes::wxHashTable", "[hash]")
{
    const int COUNT = 100;

    {
        wxHashTable hash(wxKEY_INTEGER, 10), hash2(wxKEY_STRING);
        wxObject o[20];
        int i;

        for ( i = 0; i < COUNT; ++i )
            hash.Put(i, o + i);

        hash.BeginFind();
        wxHashTable::compatibility_iterator it = hash.Next();
        i = 0;

        while (it)
        {
            ++i;
            it = hash.Next();
        }

        CHECK( i == COUNT );

        for ( i = 99; i >= 0; --i )
            CHECK( hash.Get(i) == o + i );

        for ( i = 0; i < COUNT; ++i )
            hash.Put(i, o + i + 20);

        for ( i = 99; i >= 0; --i )
            CHECK( hash.Get(i) == o + i);

        for ( i = 0; i < COUNT/2; ++i )
            CHECK( hash.Delete(i) == o + i);

        for ( i = COUNT/2; i < COUNT; ++i )
            CHECK( hash.Get(i) == o + i);

        for ( i = 0; i < COUNT/2; ++i )
            CHECK( hash.Get(i) == o + i + 20);

        for ( i = 0; i < COUNT/2; ++i )
            CHECK( hash.Delete(i) == o + i + 20);

        for ( i = 0; i < COUNT/2; ++i )
            CHECK( hash.Get(i) == nullptr);

        hash2.Put(wxT("foo"), o + 1);
        hash2.Put(wxT("bar"), o + 2);
        hash2.Put(wxT("baz"), o + 3);

        CHECK(hash2.Get(wxT("moo")) == nullptr);
        CHECK(hash2.Get(wxT("bar")) == o + 2);

        hash2.Put(wxT("bar"), o + 0);

        CHECK(hash2.Get(wxT("bar")) == o + 2);
    }

    // and now some corner-case testing; 3 and 13 hash to the same bucket
    {
        wxHashTable hash(wxKEY_INTEGER, 10);
        wxObject dummy;

        hash.Put(3, &dummy);
        hash.Delete(3);

        CHECK(hash.Get(3) == nullptr);

        hash.Put(3, &dummy);
        hash.Put(13, &dummy);
        hash.Delete(3);

        CHECK(hash.Get(3) == nullptr);

        hash.Delete(13);

        CHECK(hash.Get(13) == nullptr);

        hash.Put(3, &dummy);
        hash.Put(13, &dummy);
        hash.Delete(13);

        CHECK(hash.Get(13) == nullptr);

        hash.Delete(3);

        CHECK(hash.Get(3) == nullptr);
    }

    // test for key + value access (specifically that supplying either
    // wrong key or wrong value returns nullptr)
    {
        wxHashTable hash(wxKEY_INTEGER, 10);
        wxObject dummy[8];

        hash.Put(3, 7, dummy + 7);
        hash.Put(4, 8, dummy + 8);

        CHECK(hash.Get(7) == nullptr);
        CHECK(hash.Get(3, 7) == dummy + 7);
        CHECK(hash.Get(4) == nullptr);
        CHECK(hash.Get(3) == nullptr);
        CHECK(hash.Get(8) == nullptr);
        CHECK(hash.Get(8, 4) == nullptr);

        CHECK(hash.Delete(7) == nullptr);
        CHECK(hash.Delete(3) == nullptr);
        CHECK(hash.Delete(3, 7) == dummy + 7);
    }

}

TEST_CASE("Hashes::wxUntypedHashTableDeleteContents", "[hash]")
{
    // need a nested scope for destruction
    {
        wxHashTable hash;
        hash.DeleteContents(true);

        CHECK( hash.GetCount() == 0 );
        CHECK( FooObject::count == 0 );

        static const int hashTestData[] =
        {
            0, 1, 17, -2, 2, 4, -4, 345, 3, 3, 2, 1,
        };

        size_t n;
        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            hash.Put(hashTestData[n], n, new FooObject(n));
        }

        CHECK( hash.GetCount() == WXSIZEOF(hashTestData) );
        CHECK( FooObject::count == WXSIZEOF(hashTestData) );

        // delete from hash without deleting object
        FooObject* foo = (FooObject*)hash.Delete(0l);

        CHECK( FooObject::count == WXSIZEOF(hashTestData) );
        delete foo;
        CHECK( FooObject::count == WXSIZEOF(hashTestData) - 1 );
    }

    // hash destroyed
    CHECK( FooObject::count == 0 );
}

WX_DECLARE_HASH(Foo, wxListFoos, wxHashFoos);

TEST_CASE("Hashes::wxTypedHashTable", "[hash]")
{
    // need a nested scope for destruction
    {
        wxHashFoos hash;
        hash.DeleteContents(true);

        CHECK( hash.GetCount() == 0 );
        CHECK( Foo::count == 0 );

        static const int hashTestData[] =
        {
            0, 1, 17, -2, 2, 4, -4, 345, 3, 3, 2, 1,
        };

        size_t n;
        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            hash.Put(hashTestData[n], n, new Foo(n));
        }

        CHECK( hash.GetCount() == WXSIZEOF(hashTestData) );
        CHECK( Foo::count == WXSIZEOF(hashTestData) );

        for ( n = 0; n < WXSIZEOF(hashTestData); n++ )
        {
            Foo *foo = hash.Get(hashTestData[n], n);

            CHECK( foo != nullptr );
            CHECK( foo->n == (int)n );
        }

        // element not in hash
        CHECK( hash.Get(1234) == nullptr );
        CHECK( hash.Get(1, 0) == nullptr );

        // delete from hash without deleting object
        Foo* foo = hash.Delete(0);

        CHECK( Foo::count == WXSIZEOF(hashTestData) );
        delete foo;
        CHECK( Foo::count == WXSIZEOF(hashTestData) - 1 );
    }

    // hash destroyed
    CHECK( Foo::count == 0 );
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
void
#if defined(__GNUC__) && !defined(__clang__)
// At least g++ 4.8.2 (included in Ubuntu 14.04) is known to miscompile the
// code in this function and make all the loops below infinite when using -O2,
// so we need to turn off optimizations for it to allow the tests to run at
// all.
__attribute__((optimize("O0")))
#endif // g++
HashMapTest()
{
    typedef typename HashMapT::value_type::second_type value_type;
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
    CHECK( sh.size() == count );

    for( i = 0; i < count; ++i )
    {
        MakeKeyValuePair(i, count, buf, value);
        CHECK( sh[buf] == value );
    }

    // check that iterators work
    Itor it;
    for( i = 0, it = sh.begin(); it != sh.end(); ++it, ++i )
    {
        CHECK( i != count );
        CHECK( it->second == sh[it->first] );
    }

    CHECK( sh.size() == i );

    // test copy ctor, assignment operator
    HashMapT h1( sh ), h2( 0 );
    h2 = sh;

    for( i = 0, it = sh.begin(); it != sh.end(); ++it, ++i )
    {
        CHECK( h1[it->first] == it->second );
        CHECK( h2[it->first] == it->second );
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
            CHECK( it != sh.end() );

            sh.erase( it );

            CHECK( sh.find( buf ) == sh.end() );
        }
        else
        // test erase(key)
        {
            size_t c = sh.erase( buf );
            CHECK( c == 1 );
            CHECK( sh.find( buf ) == sh.end() );
        }

        // count should decrease
        CHECK( sh.size() == sz - 1 );
    }
}

TEST_CASE("Hashes::StringHashMap", "[hash]") { HashMapTest<myStringHashMap>(); }
TEST_CASE("Hashes::PtrHashMap", "[hash]") { HashMapTest<myPtrHashMap>(); }
TEST_CASE("Hashes::LongHashMap", "[hash]") { HashMapTest<myLongHashMap>(); }
TEST_CASE("Hashes::ULongHashMap", "[hash]") { HashMapTest<myUnsignedHashMap>(); }
TEST_CASE("Hashes::UIntHashMap", "[hash]") { HashMapTest<myTestHashMap1>(); }
TEST_CASE("Hashes::IntHashMap", "[hash]") { HashMapTest<myTestHashMap2>(); }
TEST_CASE("Hashes::ShortHashMap", "[hash]") { HashMapTest<myTestHashMap3>(); }
TEST_CASE("Hashes::UShortHashMap", "[hash]") { HashMapTest<myTestHashMap4>(); }

#ifdef TEST_LONGLONG
TEST_CASE("Hashes::LLongHashMap", "[hash]") { HashMapTest<myLLongHashMap>(); }
TEST_CASE("Hashes::ULLongHashMap", "[hash]") { HashMapTest<myULLongHashMap>(); }
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
    MyHash() = default;
    MyHash(const MyHash&) = default;
    unsigned long operator()(const MyStruct& s) const
        { return m_dummy(s.ptr); }
    MyHash& operator=(const MyHash&) { return *this; }
private:
    wxPointerHash m_dummy;
};

class MyEqual
{
public:
    MyEqual() = default;
    MyEqual(const MyEqual&) = default;
    bool operator()(const MyStruct& s1, const MyStruct& s2) const
        { return s1.ptr == s2.ptr; }
    MyEqual& operator=(const MyEqual&) { return *this; }
};

WX_DECLARE_HASH_SET( MyStruct, MyHash, MyEqual, mySet );

typedef myTestHashSet5 wxStringHashSet;

TEST_CASE("Hashes::wxHashSet", "[hash]")
{
    wxStringHashSet set1;

    set1.insert( wxT("abc") );

    CHECK( set1.size() == 1 );

    set1.insert( wxT("bbc") );
    set1.insert( wxT("cbc") );

    CHECK( set1.size() == 3 );

    set1.insert( wxT("abc") );

    CHECK( set1.size() == 3 );

    mySet set2;
    int dummy;
    MyStruct tmp;

    tmp.ptr = &dummy; tmp.str = wxT("ABC");
    set2.insert( tmp );
    tmp.ptr = &dummy + 1;
    set2.insert( tmp );
    tmp.ptr = &dummy; tmp.str = wxT("CDE");
    set2.insert( tmp );

    CHECK( set2.size() == 2 );

    mySet::iterator it = set2.find( tmp );

    CHECK( it != set2.end() );
    CHECK( it->ptr == &dummy );
    CHECK( it->str == wxT("ABC") );
}
