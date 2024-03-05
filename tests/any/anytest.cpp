///////////////////////////////////////////////////////////////////////////////
// Name:        tests/any/anytest.cpp
// Purpose:     Test the wxAny classes
// Author:      Jaakko Salli
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#if wxUSE_ANY

#include "wx/any.h"
#include "wx/datetime.h"
#include "wx/object.h"
#include "wx/vector.h"

#include <math.h>

namespace Catch
{
    template <>
    struct StringMaker<wxVariant>
    {
        static std::string convert(const wxVariant& v)
        {
            return v.MakeString().ToStdString(wxConvUTF8);
        }
    };
}

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class wxAnyTestCase : public CppUnit::TestCase
{
public:
    wxAnyTestCase();

private:
    CPPUNIT_TEST_SUITE( wxAnyTestCase );
        CPPUNIT_TEST( CheckType );
        CPPUNIT_TEST( Equality );
        CPPUNIT_TEST( As );
        CPPUNIT_TEST( GetAs );
        CPPUNIT_TEST( Null );
        CPPUNIT_TEST( wxVariantConversions );
        CPPUNIT_TEST( CustomTemplateSpecialization );
        CPPUNIT_TEST( Misc );
    CPPUNIT_TEST_SUITE_END();

    void CheckType();
    void Equality();
    void As();
    void GetAs();
    void Null();
    void wxVariantConversions();
    void CustomTemplateSpecialization();
    void Misc();

    wxDateTime m_testDateTime;

    wxAny   m_anySignedChar1;
    wxAny   m_anySignedShort1;
    wxAny   m_anySignedInt1;
    wxAny   m_anySignedLong1;
    wxAny   m_anySignedLongLong1;
    wxAny   m_anyUnsignedChar1;
    wxAny   m_anyUnsignedShort1;
    wxAny   m_anyUnsignedInt1;
    wxAny   m_anyUnsignedLong1;
    wxAny   m_anyUnsignedLongLong1;
    wxAny   m_anyStringString1;
    wxAny   m_anyCharString1;
    wxAny   m_anyWcharString1;
    wxAny   m_anyBool1;
    wxAny   m_anyFloatDouble1;
    wxAny   m_anyDoubleDouble1;
    wxAny   m_anyWxObjectPtr1;
    wxAny   m_anyVoidPtr1;
    wxAny   m_anyDateTime1;
    wxAny   m_anyUniChar1;

    wxAny   m_anySignedChar2;
    wxAny   m_anySignedShort2;
    wxAny   m_anySignedInt2;
    wxAny   m_anySignedLong2;
    wxAny   m_anySignedLongLong2;
    wxAny   m_anyUnsignedChar2;
    wxAny   m_anyUnsignedShort2;
    wxAny   m_anyUnsignedInt2;
    wxAny   m_anyUnsignedLong2;
    wxAny   m_anyUnsignedLongLong2;
    wxAny   m_anyStringString2;
    wxAny   m_anyCharString2;
    wxAny   m_anyWcharString2;
    wxAny   m_anyBool2;
    wxAny   m_anyFloatDouble2;
    wxAny   m_anyDoubleDouble2;
    wxAny   m_anyWxObjectPtr2;
    wxAny   m_anyVoidPtr2;
    wxAny   m_anyDateTime2;

    wxDECLARE_NO_COPY_CLASS(wxAnyTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( wxAnyTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( wxAnyTestCase, "wxAnyTestCase" );

// Let's use a number with first digit after decimal dot less than 5,
// so that we don't have to worry about whether conversion from float
// to int truncates or rounds.
const float TEST_FLOAT_CONST = 123.456f;
const double TEST_DOUBLE_CONST = 123.456;

const double FEQ_DELTA = 0.001;

wxObject* dummyWxObjectPointer = reinterpret_cast<wxObject*>(1234);
void* dummyVoidPointer = reinterpret_cast<void*>(1234);


//
// Test both 'creation' methods
wxAnyTestCase::wxAnyTestCase()
    : m_anySignedChar1((signed char)15),
      m_anySignedShort1((signed short)15),
      m_anySignedInt1((signed int)15),
      m_anySignedLong1((signed long)15),
#ifdef wxLongLong_t
      m_anySignedLongLong1((wxLongLong_t)15),
#endif
      m_anyUnsignedChar1((unsigned char)15),
      m_anyUnsignedShort1((unsigned short)15),
      m_anyUnsignedInt1((unsigned int)15),
      m_anyUnsignedLong1((unsigned long)15),
#ifdef wxLongLong_t
      m_anyUnsignedLongLong1((wxULongLong_t)15),
#endif
      m_anyStringString1(wxString("abc")),
      m_anyCharString1("abc"),
      m_anyWcharString1(L"abc"),
      m_anyBool1(true),
      m_anyFloatDouble1(TEST_FLOAT_CONST),
      m_anyDoubleDouble1(TEST_DOUBLE_CONST),
      m_anyWxObjectPtr1(dummyWxObjectPointer),
      m_anyVoidPtr1(dummyVoidPointer),
      m_anyDateTime1(wxDateTime::Now())
{
    m_testDateTime = wxDateTime::Now();
    m_anySignedChar2 = (signed char)15;
    m_anySignedShort2 = (signed short)15;
    m_anySignedInt2 = (signed int)15;
    m_anySignedLong2 = (signed long)15;
#ifdef wxLongLong_t
    m_anySignedLongLong2 = (wxLongLong_t)15;
#endif
    m_anyUnsignedChar2 = (unsigned char)15;
    m_anyUnsignedShort2 = (unsigned short)15;
    m_anyUnsignedInt2 = (unsigned int)15;
    m_anyUnsignedLong2 = (unsigned long)15;
#ifdef wxLongLong_t
    m_anyUnsignedLongLong2 = (wxULongLong_t)15;
#endif
    m_anyStringString2 = wxString("abc");
    m_anyCharString2 = "abc";
    m_anyWcharString2 = L"abc";
    m_anyBool2 = true;
    m_anyFloatDouble2 = TEST_FLOAT_CONST;
    m_anyDoubleDouble2 = TEST_DOUBLE_CONST;
    m_anyDateTime2 = m_testDateTime;
    m_anyUniChar1 = wxUniChar('A');
    m_anyWxObjectPtr2 = dummyWxObjectPointer;
    m_anyVoidPtr2 = dummyVoidPointer;
}

void wxAnyTestCase::CheckType()
{
    wxAny nullAny;
    CPPUNIT_ASSERT(!wxANY_CHECK_TYPE(nullAny, wxString));

    CPPUNIT_ASSERT(wxANY_CHECK_TYPE(m_anyCharString2, const char*));
    CPPUNIT_ASSERT(!wxANY_CHECK_TYPE(m_anyCharString2, wxString));
    CPPUNIT_ASSERT(!wxANY_CHECK_TYPE(m_anyCharString2, const wchar_t*));
    CPPUNIT_ASSERT(wxANY_CHECK_TYPE(m_anyWcharString2, const wchar_t*));
    CPPUNIT_ASSERT(!wxANY_CHECK_TYPE(m_anyWcharString2, wxString));
    CPPUNIT_ASSERT(!wxANY_CHECK_TYPE(m_anyWcharString2, const char*));

    // HasSameType()
    CPPUNIT_ASSERT( m_anyWcharString1.HasSameType(m_anyWcharString2) );
    CPPUNIT_ASSERT( !m_anyWcharString1.HasSameType(m_anyBool1) );
}

void wxAnyTestCase::Equality()
{
    //
    // Currently this should work
    CPPUNIT_ASSERT(m_anyUnsignedLong1 == 15L);
    CPPUNIT_ASSERT(m_anyUnsignedLong1 != 30L);
    CPPUNIT_ASSERT(m_anyUnsignedLong1 == 15UL);
    CPPUNIT_ASSERT(m_anyUnsignedLong1 != 30UL);
    CPPUNIT_ASSERT(m_anyStringString1 == wxString("abc"));
    CPPUNIT_ASSERT(m_anyStringString1 != wxString("ABC"));
    CPPUNIT_ASSERT(m_anyStringString1 == "abc");
    CPPUNIT_ASSERT(m_anyStringString1 != "ABC");
    CPPUNIT_ASSERT(m_anyStringString1 == L"abc");
    CPPUNIT_ASSERT(m_anyStringString1 != L"ABC");
    CPPUNIT_ASSERT(m_anyBool1 == true);
    CPPUNIT_ASSERT(m_anyBool1 != false);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m_anyFloatDouble1.As<double>(),
                                 m_anyDoubleDouble1.As<double>(),
                                 FEQ_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m_anyFloatDouble1.As<double>(),
                                 TEST_FLOAT_CONST,
                                 FEQ_DELTA);
    CPPUNIT_ASSERT(m_anyWxObjectPtr1.As<wxObject*>()
                        == dummyWxObjectPointer);
    CPPUNIT_ASSERT(m_anyVoidPtr1.As<void*>() == dummyVoidPointer);

    CPPUNIT_ASSERT(m_anySignedLong2 == 15);
    CPPUNIT_ASSERT(m_anyStringString2 == wxString("abc"));
    CPPUNIT_ASSERT(m_anyStringString2 == "abc");
    CPPUNIT_ASSERT(m_anyStringString2 == L"abc");
    CPPUNIT_ASSERT(m_anyBool2 == true);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m_anyFloatDouble2.As<double>(),
                                 m_anyDoubleDouble2.As<double>(),
                                 FEQ_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(m_anyFloatDouble2.As<double>(),
                                 TEST_FLOAT_CONST,
                                 FEQ_DELTA);
    CPPUNIT_ASSERT(m_anyWxObjectPtr2.As<wxObject*>()
                        == dummyWxObjectPointer);
    CPPUNIT_ASSERT(m_anyVoidPtr2.As<void*>() == dummyVoidPointer);

    // Test sub-type system type compatibility
    CPPUNIT_ASSERT(m_anySignedShort1.GetType()->
                  IsSameType(m_anySignedLongLong1.GetType()));
    CPPUNIT_ASSERT(m_anyUnsignedShort1.GetType()->
                   IsSameType(m_anyUnsignedLongLong1.GetType()));
}

void wxAnyTestCase::As()
{
    //
    // Test getting C++ data from wxAny without dynamic conversion
    signed char a = m_anySignedChar1.As<signed char>();
    CPPUNIT_ASSERT(a == (signed int)15);
    signed short b = m_anySignedShort1.As<signed short>();
    CPPUNIT_ASSERT(b == (signed int)15);
    signed int c = m_anySignedInt1.As<signed int>();
    CPPUNIT_ASSERT(c == (signed int)15);
    signed long d = m_anySignedLong1.As<signed long>();
    CPPUNIT_ASSERT(d == (signed int)15);
#ifdef wxLongLong_t
    wxLongLong_t e = m_anySignedLongLong1.As<wxLongLong_t>();
    CPPUNIT_ASSERT(e == (signed int)15);
#endif
    unsigned char f = m_anyUnsignedChar1.As<unsigned char>();
    CPPUNIT_ASSERT(f == (unsigned int)15);
    unsigned short g = m_anyUnsignedShort1.As<unsigned short>();
    CPPUNIT_ASSERT(g == (unsigned int)15);
    unsigned int h = m_anyUnsignedInt1.As<unsigned int>();
    CPPUNIT_ASSERT(h == (unsigned int)15);
    unsigned long i = m_anyUnsignedLong1.As<unsigned long>();
    CPPUNIT_ASSERT(i == (unsigned int)15);
#ifdef wxLongLong_t
    wxULongLong_t j = m_anyUnsignedLongLong1.As<wxULongLong_t>();
    CPPUNIT_ASSERT(j == (unsigned int)15);
#endif
    wxString k = m_anyStringString1.As<wxString>();
    CPPUNIT_ASSERT(k == "abc");
    wxString l = m_anyCharString1.As<wxString>();
    const char* cptr = m_anyCharString1.As<const char*>();
    CPPUNIT_ASSERT(l == "abc");
    CPPUNIT_ASSERT(cptr);
    wxString m = m_anyWcharString1.As<wxString>();
    const wchar_t* wcptr = m_anyWcharString1.As<const wchar_t*>();
    CPPUNIT_ASSERT(wcptr);
    CPPUNIT_ASSERT(m == "abc");
    bool n = m_anyBool1.As<bool>();
    CPPUNIT_ASSERT(n);

    // Make sure the stored float that comes back is -identical-.
    // So do not use delta comparison here.
    float o = m_anyFloatDouble1.As<float>();
    CPPUNIT_ASSERT_EQUAL(o, TEST_FLOAT_CONST);

    double p = m_anyDoubleDouble1.As<double>();
    CPPUNIT_ASSERT_EQUAL(p, TEST_DOUBLE_CONST);

    wxUniChar chr = m_anyUniChar1.As<wxUniChar>();
    CPPUNIT_ASSERT(chr == 'A');
    wxDateTime q = m_anyDateTime1.As<wxDateTime>();
    CPPUNIT_ASSERT(q == m_testDateTime);
    wxObject* r = m_anyWxObjectPtr1.As<wxObject*>();
    CPPUNIT_ASSERT(r == dummyWxObjectPointer);
    void* s = m_anyVoidPtr1.As<void*>();
    CPPUNIT_ASSERT(s == dummyVoidPointer);
}

void wxAnyTestCase::Null()
{
    wxAny a;
    CPPUNIT_ASSERT(a.IsNull());
    a = -127;
    CPPUNIT_ASSERT(a == -127);
    a.MakeNull();
    CPPUNIT_ASSERT(a.IsNull());
}

void wxAnyTestCase::GetAs()
{
    //
    // Test dynamic conversion
    bool res;
    long l = 0;
    short int si = 0;
    unsigned long ul = 0;
    wxString s;
    // Let's test against float instead of double, since the former
    // is not the native underlying type the code converts to, but
    // should still work, all the same.
    float f = 0.0;
    bool b = false;

    // Conversions from signed long type
    // The first check should be enough to make sure that the sub-type system
    // has not failed.
    res = m_anySignedLong1.GetAs(&si);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(si, 15);
    res = m_anySignedLong1.GetAs(&ul);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(ul, 15UL);
    res = m_anySignedLong1.GetAs(&s);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(s == "15");
    res = m_anySignedLong1.GetAs(&f);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(f, 15.0, FEQ_DELTA);
    res = m_anySignedLong1.GetAs(&b);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(b == true);

    // Conversions from unsigned long type
    res = m_anyUnsignedLong1.GetAs(&l);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(l == static_cast<signed long>(15));
    res = m_anyUnsignedLong1.GetAs(&s);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(s == "15");
    res = m_anyUnsignedLong1.GetAs(&f);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(f, 15.0, FEQ_DELTA);
    res = m_anyUnsignedLong1.GetAs(&b);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(b == true);

    // Conversions from default "abc" string to other types
    // should not work.
    CPPUNIT_ASSERT(!m_anyStringString1.GetAs(&l));
    CPPUNIT_ASSERT(!m_anyStringString1.GetAs(&ul));
    CPPUNIT_ASSERT(!m_anyStringString1.GetAs(&f));
    CPPUNIT_ASSERT(!m_anyStringString1.GetAs(&b));

    // Let's test some other conversions from string that should work.
    wxAny anyString;

    anyString = "15";
    res = anyString.GetAs(&l);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(l == static_cast<signed long>(15));
    res = anyString.GetAs(&ul);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(ul, static_cast<unsigned long>(15));
    res = anyString.GetAs(&f);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(f, 15.0, FEQ_DELTA);
    anyString = "TRUE";
    res = anyString.GetAs(&b);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(b == true);
    anyString = "0";
    res = anyString.GetAs(&b);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(b == false);

    // Conversions from bool type
    res = m_anyBool1.GetAs(&l);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(l == static_cast<signed long>(1));
    res = m_anyBool1.GetAs(&ul);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(ul, static_cast<unsigned long>(1));
    res = m_anyBool1.GetAs(&s);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(s == "true");
    CPPUNIT_ASSERT(!m_anyBool1.GetAs(&f));

    // Conversions from floating point type
    res = m_anyDoubleDouble1.GetAs(&l);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(l == static_cast<signed long>(123));
    res = m_anyDoubleDouble1.GetAs(&ul);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(ul, static_cast<unsigned long>(123));
    res = m_anyDoubleDouble1.GetAs(&s);
    CPPUNIT_ASSERT(res);
    double d2;
    res = s.ToCDouble(&d2);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(d2, TEST_FLOAT_CONST, FEQ_DELTA);
}


//
// Test user data type for wxAnyValueTypeImpl specialization
// any hand-built wxVariantData. Also for inplace allocation
// sanity checks.
//

class MyClass;

static wxVector<MyClass*> gs_myClassInstances;

class MyClass
{
public:
    MyClass( int someValue = 32768 )
    {
        Init();
        m_someValue = someValue;
    }
    MyClass( const MyClass& other )
    {
        Init();
        m_someValue = other.m_someValue;
    }
    virtual ~MyClass()
    {
        for ( size_t i=0; i<gs_myClassInstances.size(); i++ )
        {
            if ( gs_myClassInstances[i] == this )
            {
                gs_myClassInstances.erase(gs_myClassInstances.begin()+i);
            }
        }
    }

    int GetValue() const
    {
        return m_someValue;
    }

    wxString ToString()
    {
        return wxString::Format("%i", m_someValue);
    }

private:
    void Init()
    {
        // We use this for some sanity checking
        gs_myClassInstances.push_back(this);
    }

    int     m_someValue;
};


#if wxUSE_VARIANT

// For testing purposes, create dummy variant data implementation
// that does not have wxAny conversion code
class wxMyVariantData : public wxVariantData
{
public:
    wxMyVariantData(const MyClass& value)
        : m_value(value)
    {
    }

    virtual bool Eq(wxVariantData& WXUNUSED(data)) const override
    {
        return false;
    }

    // What type is it? Return a string name.
    virtual wxString GetType() const override { return "MyClass"; }

    virtual wxVariantData* Clone() const override
    {
        return new wxMyVariantData(m_value);
    }

protected:
    MyClass     m_value;
};

#endif // wxUSE_VARIANT


void wxAnyTestCase::wxVariantConversions()
{
#if wxUSE_VARIANT
    //
    // Test various conversions to and from wxVariant
    //
    bool res;

    // Prepare wxVariants
    wxVariant vLong(123L);
    wxVariant vString("ABC");
    wxVariant vDouble(TEST_FLOAT_CONST);
    wxVariant vBool((bool)true);
    wxVariant vChar('A');
#ifdef wxLongLong_t
    wxVariant vLongLong(wxLongLong(wxLL(0xAABBBBCCCC)));
    wxVariant vULongLong(wxULongLong(wxULL(123456)));
#endif
    wxArrayString arrstr;
    arrstr.push_back("test string");
    wxVariant vArrayString(arrstr);
    wxVariant vDateTime(m_testDateTime);
    wxVariant vVoidPtr(dummyVoidPointer);
    wxVariant vCustomType(new wxMyVariantData(MyClass(101)));
    wxVariant vList;

    vList.NullList();
    vList.Append(15);
    vList.Append("abc");

    // Convert to wxAnys, and then back to wxVariants
    wxVariant variant;

    wxAny any(vLong);
    CPPUNIT_ASSERT(any == 123L);
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant == 123L);

    // Make sure integer variant has correct type information
    CPPUNIT_ASSERT(variant.GetLong() == 123);
    CPPUNIT_ASSERT(variant.GetType() == "long");

    // Unsigned long wxAny should convert to "ulonglong" wxVariant
    any = 1000UL;
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "ulonglong");
    CPPUNIT_ASSERT(variant.GetLong() == 1000);

    any = vString;
    CPPUNIT_ASSERT(any == "ABC");
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetString() == "ABC");

    // Must be able to build string wxVariant from wxAny built from
    // string literal
    any = "ABC";
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "string");
    CPPUNIT_ASSERT(variant.GetString() == "ABC");
    any = L"ABC";
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "string");
    CPPUNIT_ASSERT(variant.GetString() == L"ABC");

    any = vDouble;
    double d = any.As<double>();
    CPPUNIT_ASSERT_DOUBLES_EQUAL(d, TEST_FLOAT_CONST, FEQ_DELTA);
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(variant.GetDouble(),
                                 TEST_FLOAT_CONST,
                                 FEQ_DELTA);

    any = vBool;
    CPPUNIT_ASSERT(any.As<bool>() == true);
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetBool() == true);

    any = wxAny(vChar);
    //CPPUNIT_ASSERT(any.As<wxUniChar>() == 'A');
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetChar() == 'A');

#ifdef wxLongLong_t
    any = wxAny(vLongLong);
    CPPUNIT_ASSERT(any == wxLL(0xAABBBBCCCC));
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "longlong");
    CPPUNIT_ASSERT(variant.GetLongLong() == wxLongLong(wxLL(0xAABBBBCCCC)));

#if LONG_MAX == wxINT64_MAX
    // As a sanity check, test that wxVariant of type 'long' converts
    // seamlessly to 'longlong' (on some 64-bit systems)
    any = 0xAABBBBCCCCL;
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(variant.GetLongLong() == wxLongLong(wxLL(0xAABBBBCCCC)));
#endif

    any = wxAny(vULongLong);
    CPPUNIT_ASSERT(any == wxLL(123456));
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "ulonglong");
    CPPUNIT_ASSERT(variant.GetULongLong() == wxULongLong(wxULL(123456)));

    any = (wxLongLong_t)-1;
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "long");
    CPPUNIT_ASSERT(variant.GetLong() == -1);
#endif

    // Cannot test equality for the rest, just test that they convert
    // back correctly.
    any = wxAny(vArrayString);
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    wxArrayString arrstr2 = variant.GetArrayString();
    CPPUNIT_ASSERT(arrstr2 == arrstr);

    any = m_testDateTime;
    CPPUNIT_ASSERT(any.As<wxDateTime>() == m_testDateTime);
    any = wxAny(vDateTime);
    CPPUNIT_ASSERT(any.As<wxDateTime>() == m_testDateTime);
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant == m_testDateTime);

    any = wxAny(vVoidPtr);
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetVoidPtr() == dummyVoidPointer);

    any = wxAny(vList);
    CPPUNIT_ASSERT(wxANY_CHECK_TYPE(any, wxAnyList));
    wxAnyList anyList = any.As<wxAnyList>();
    CPPUNIT_ASSERT(anyList.GetCount() == 2);
    CPPUNIT_ASSERT((*anyList[0]).As<int>() == 15);
    CPPUNIT_ASSERT((*anyList[1]).As<wxString>() == "abc");
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "list");
    CPPUNIT_ASSERT(variant.GetCount() == 2);
    CPPUNIT_ASSERT(variant[0].GetLong() == 15);
    CPPUNIT_ASSERT(variant[1].GetString() == "abc");
    // Avoid the memory leak.
    WX_CLEAR_LIST(wxAnyList, anyList);

    any = wxAny(vCustomType);
    CPPUNIT_ASSERT(wxANY_CHECK_TYPE(any, wxVariantData*));
    res = any.GetAs(&variant);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT(variant.GetType() == "MyClass");

#endif // wxUSE_VARIANT
}

template<>
class wxAnyValueTypeImpl<MyClass> :
    public wxAnyValueTypeImplBase<MyClass>
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImpl<MyClass>)
public:
    wxAnyValueTypeImpl() :
        wxAnyValueTypeImplBase<MyClass>() { }
    virtual ~wxAnyValueTypeImpl() { }

    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const override
    {
        MyClass value = GetValue(src);

        if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxString) )
        {
            wxString s = value.ToString();
            wxAnyValueTypeImpl<wxString>::SetValue(s, dst);
        }
        else
            return false;

        return true;
    }
};

//
// Following must be placed somewhere in your source code
WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<MyClass>)

void wxAnyTestCase::CustomTemplateSpecialization()
{
    // Do only a minimal CheckType() test, as dynamic type conversion already
    // uses it a lot.
    bool res;
    MyClass myObject;
    wxAny any = myObject;

    CPPUNIT_ASSERT( wxANY_CHECK_TYPE(any, MyClass) );
    MyClass myObject2 = any.As<MyClass>();
    wxUnusedVar(myObject2);

    wxString str;
    res = any.GetAs(&str);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(str, myObject.ToString());
}

void wxAnyTestCase::Misc()
{
    // Do some (inplace) allocation sanity checks
    {

        // Do it inside a scope so we can easily test instance count
        // afterwards
        MyClass myObject(15);
        wxAny any = myObject;

        // There must be two instances - first in myObject,
        // and second copied in any.
        CPPUNIT_ASSERT_EQUAL(gs_myClassInstances.size(), 2);

        // Check that it is allocated in-place, as supposed
        if ( sizeof(MyClass) <= WX_ANY_VALUE_BUFFER_SIZE )
        {
            // Memory block of the instance second must be inside the any
            size_t anyBegin = reinterpret_cast<size_t>(&any);
            size_t anyEnd = anyBegin + sizeof(wxAny);
            size_t pos = reinterpret_cast<size_t>(gs_myClassInstances[1]);
            CPPUNIT_ASSERT( pos >= anyBegin );
            CPPUNIT_ASSERT( pos < anyEnd );
        }

        wxAny any2 = any;
        CPPUNIT_ASSERT( any2.As<MyClass>().GetValue() == 15 );
    }

    // Make sure allocations and deallocations match
    CPPUNIT_ASSERT_EQUAL(gs_myClassInstances.size(), 0);
}

#endif // wxUSE_ANY

