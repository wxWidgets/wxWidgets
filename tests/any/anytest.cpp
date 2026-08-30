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
// test data and fixture
// ----------------------------------------------------------------------------

// Let's use a number with first digit after decimal dot less than 5,
// so that we don't have to worry about whether conversion from float
// to int truncates or rounds.
const float TEST_FLOAT_CONST = 123.456f;
const double TEST_DOUBLE_CONST = 123.456;

const double FEQ_DELTA = 0.001;

wxObject* dummyWxObjectPointer = reinterpret_cast<wxObject*>(1234);
void* dummyVoidPointer = reinterpret_cast<void*>(1234);

namespace
{

// The fixture creating the wxAny objects used by the tests below in both
// possible ways: using the ctor and using the assignment operator.
class AnyTestCase
{
public:
    AnyTestCase();

protected:
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

    wxDECLARE_NO_COPY_CLASS(AnyTestCase);
};

AnyTestCase::AnyTestCase()
    : m_anySignedChar1((signed char)15),
      m_anySignedShort1((signed short)15),
      m_anySignedInt1((signed int)15),
      m_anySignedLong1((signed long)15),
      m_anySignedLongLong1((wxLongLong_t)15),
      m_anyUnsignedChar1((unsigned char)15),
      m_anyUnsignedShort1((unsigned short)15),
      m_anyUnsignedInt1((unsigned int)15),
      m_anyUnsignedLong1((unsigned long)15),
      m_anyUnsignedLongLong1((wxULongLong_t)15),
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
    m_anySignedLongLong2 = (wxLongLong_t)15;
    m_anyUnsignedChar2 = (unsigned char)15;
    m_anyUnsignedShort2 = (unsigned short)15;
    m_anyUnsignedInt2 = (unsigned int)15;
    m_anyUnsignedLong2 = (unsigned long)15;
    m_anyUnsignedLongLong2 = (wxULongLong_t)15;
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

} // anonymous namespace

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE_METHOD(AnyTestCase, "wxAny::CheckType", "[any]")
{
    wxAny nullAny;
    CHECK(!wxANY_CHECK_TYPE(nullAny, wxString));

    CHECK(wxANY_CHECK_TYPE(m_anyCharString2, const char*));
    CHECK(!wxANY_CHECK_TYPE(m_anyCharString2, wxString));
    CHECK(!wxANY_CHECK_TYPE(m_anyCharString2, const wchar_t*));
    CHECK(wxANY_CHECK_TYPE(m_anyWcharString2, const wchar_t*));
    CHECK(!wxANY_CHECK_TYPE(m_anyWcharString2, wxString));
    CHECK(!wxANY_CHECK_TYPE(m_anyWcharString2, const char*));

    // HasSameType()
    CHECK( m_anyWcharString1.HasSameType(m_anyWcharString2) );
    CHECK( !m_anyWcharString1.HasSameType(m_anyBool1) );
}

TEST_CASE_METHOD(AnyTestCase, "wxAny::Equality", "[any]")
{
    //
    // Currently this should work
    CHECK(m_anyUnsignedLong1 == 15L);
    CHECK(m_anyUnsignedLong1 != 30L);
    CHECK(m_anyUnsignedLong1 == 15UL);
    CHECK(m_anyUnsignedLong1 != 30UL);
    CHECK(m_anyStringString1 == wxString("abc"));
    CHECK(m_anyStringString1 != wxString("ABC"));
    CHECK(m_anyStringString1 == "abc");
    CHECK(m_anyStringString1 != "ABC");
    CHECK(m_anyStringString1 == L"abc");
    CHECK(m_anyStringString1 != L"ABC");
    CHECK(m_anyBool1 == true);
    CHECK(m_anyBool1 != false);
    CHECK(m_anyDoubleDouble1.As<double>() == Approx(m_anyFloatDouble1.As<double>()).margin(FEQ_DELTA));
    CHECK(TEST_FLOAT_CONST == Approx(m_anyFloatDouble1.As<float>()).margin(float(FEQ_DELTA)));
    CHECK(m_anyWxObjectPtr1.As<wxObject*>()
                        == dummyWxObjectPointer);
    CHECK(m_anyVoidPtr1.As<void*>() == dummyVoidPointer);

    CHECK(m_anySignedLong2 == 15);
    CHECK(m_anyStringString2 == wxString("abc"));
    CHECK(m_anyStringString2 == "abc");
    CHECK(m_anyStringString2 == L"abc");
    CHECK(m_anyBool2 == true);
    CHECK(m_anyDoubleDouble2.As<double>() == Approx(m_anyFloatDouble2.As<double>()).margin(FEQ_DELTA));
    CHECK(TEST_FLOAT_CONST == Approx(m_anyFloatDouble2.As<float>()).margin(float(FEQ_DELTA)));
    CHECK(m_anyWxObjectPtr2.As<wxObject*>()
                        == dummyWxObjectPointer);
    CHECK(m_anyVoidPtr2.As<void*>() == dummyVoidPointer);

    // Test sub-type system type compatibility
    CHECK(m_anySignedShort1.GetType()->
                  IsSameType(m_anySignedLongLong1.GetType()));
    CHECK(m_anyUnsignedShort1.GetType()->
                   IsSameType(m_anyUnsignedLongLong1.GetType()));
}

TEST_CASE_METHOD(AnyTestCase, "wxAny::As", "[any]")
{
    //
    // Test getting C++ data from wxAny without dynamic conversion
    signed char a = m_anySignedChar1.As<signed char>();
    CHECK(a == (signed int)15);
    signed short b = m_anySignedShort1.As<signed short>();
    CHECK(b == (signed int)15);
    signed int c = m_anySignedInt1.As<signed int>();
    CHECK(c == (signed int)15);
    signed long d = m_anySignedLong1.As<signed long>();
    CHECK(d == (signed int)15);
    wxLongLong_t e = m_anySignedLongLong1.As<wxLongLong_t>();
    CHECK(e == (signed int)15);
    unsigned char f = m_anyUnsignedChar1.As<unsigned char>();
    CHECK(f == (unsigned int)15);
    unsigned short g = m_anyUnsignedShort1.As<unsigned short>();
    CHECK(g == (unsigned int)15);
    unsigned int h = m_anyUnsignedInt1.As<unsigned int>();
    CHECK(h == (unsigned int)15);
    unsigned long i = m_anyUnsignedLong1.As<unsigned long>();
    CHECK(i == (unsigned int)15);
    wxULongLong_t j = m_anyUnsignedLongLong1.As<wxULongLong_t>();
    CHECK(j == (unsigned int)15);
    wxString k = m_anyStringString1.As<wxString>();
    CHECK(k == "abc");
    wxString l = m_anyCharString1.As<wxString>();
    const char* cptr = m_anyCharString1.As<const char*>();
    CHECK(l == "abc");
    CHECK(cptr);
    wxString m = m_anyWcharString1.As<wxString>();
    const wchar_t* wcptr = m_anyWcharString1.As<const wchar_t*>();
    CHECK(wcptr);
    CHECK(m == "abc");
    bool n = m_anyBool1.As<bool>();
    CHECK(n);

    // Make sure the stored float that comes back is -identical-.
    // So do not use delta comparison here.
    float o = m_anyFloatDouble1.As<float>();
    CHECK(TEST_FLOAT_CONST == o);

    double p = m_anyDoubleDouble1.As<double>();
    CHECK(TEST_DOUBLE_CONST == p);

    wxUniChar chr = m_anyUniChar1.As<wxUniChar>();
    CHECK(chr == 'A');
    wxDateTime q = m_anyDateTime1.As<wxDateTime>();
    CHECK(q == m_testDateTime);
    wxObject* r = m_anyWxObjectPtr1.As<wxObject*>();
    CHECK(r == dummyWxObjectPointer);
    void* s = m_anyVoidPtr1.As<void*>();
    CHECK(s == dummyVoidPointer);
}

TEST_CASE_METHOD(AnyTestCase, "wxAny::Null", "[any]")
{
    wxAny a;
    CHECK(a.IsNull());
    a = -127;
    CHECK(a == -127);
    a.MakeNull();
    CHECK(a.IsNull());
}

TEST_CASE_METHOD(AnyTestCase, "wxAny::GetAs", "[any]")
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
    CHECK(res);
    CHECK(15 == si);
    res = m_anySignedLong1.GetAs(&ul);
    CHECK(res);
    CHECK(15UL == ul);
    res = m_anySignedLong1.GetAs(&s);
    CHECK(res);
    CHECK(s == "15");
    res = m_anySignedLong1.GetAs(&f);
    CHECK(res);
    CHECK(15.0f == Approx(f).margin(float(FEQ_DELTA)));
    res = m_anySignedLong1.GetAs(&b);
    CHECK(res);
    CHECK(b == true);

    // Conversions from unsigned long type
    res = m_anyUnsignedLong1.GetAs(&l);
    CHECK(res);
    CHECK(l == static_cast<signed long>(15));
    res = m_anyUnsignedLong1.GetAs(&s);
    CHECK(res);
    CHECK(s == "15");
    res = m_anyUnsignedLong1.GetAs(&f);
    CHECK(res);
    CHECK(15.0f == Approx(f).margin(float(FEQ_DELTA)));
    res = m_anyUnsignedLong1.GetAs(&b);
    CHECK(res);
    CHECK(b == true);

    // Conversions from default "abc" string to other types
    // should not work.
    CHECK(!m_anyStringString1.GetAs(&l));
    CHECK(!m_anyStringString1.GetAs(&ul));
    CHECK(!m_anyStringString1.GetAs(&f));
    CHECK(!m_anyStringString1.GetAs(&b));

    // Let's test some other conversions from string that should work.
    wxAny anyString;

    anyString = "15";
    res = anyString.GetAs(&l);
    CHECK(res);
    CHECK(l == static_cast<signed long>(15));
    res = anyString.GetAs(&ul);
    CHECK(res);
    CHECK(static_cast<unsigned long>(15) == ul);
    res = anyString.GetAs(&f);
    CHECK(res);
    CHECK(15.0f == Approx(f).margin(float(FEQ_DELTA)));
    anyString = "TRUE";
    res = anyString.GetAs(&b);
    CHECK(res);
    CHECK(b == true);
    anyString = "0";
    res = anyString.GetAs(&b);
    CHECK(res);
    CHECK(b == false);

    // Conversions from bool type
    res = m_anyBool1.GetAs(&l);
    CHECK(res);
    CHECK(l == static_cast<signed long>(1));
    res = m_anyBool1.GetAs(&ul);
    CHECK(res);
    CHECK(static_cast<unsigned long>(1) == ul);
    res = m_anyBool1.GetAs(&s);
    CHECK(res);
    CHECK(s == "true");
    CHECK(!m_anyBool1.GetAs(&f));

    // Conversions from floating point type
    res = m_anyDoubleDouble1.GetAs(&l);
    CHECK(res);
    CHECK(l == static_cast<signed long>(123));
    res = m_anyDoubleDouble1.GetAs(&ul);
    CHECK(res);
    CHECK(static_cast<unsigned long>(123) == ul);
    res = m_anyDoubleDouble1.GetAs(&s);
    CHECK(res);
    double d2;
    res = s.ToCDouble(&d2);
    CHECK(res);
    CHECK(TEST_DOUBLE_CONST == Approx(d2).margin(FEQ_DELTA));
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

TEST_CASE_METHOD(AnyTestCase, "wxAny::wxVariantConversions", "[any]")
{
#if wxUSE_VARIANT
    //
    // Test various conversions to and from wxVariant
    //
    bool res;

    // Prepare wxVariants
    wxVariant vLong(123L);
    wxVariant vString("ABC");
    wxVariant vDouble(TEST_DOUBLE_CONST);
    wxVariant vBool((bool)true);
    wxVariant vChar('A');
    wxVariant vLongLong(wxLongLong(wxLL(0xAABBBBCCCC)));
    wxVariant vULongLong(wxULongLong(wxULL(123456)));
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
    CHECK(any == 123L);
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant == 123L);

    // Make sure integer variant has correct type information
    CHECK(variant.GetLong() == 123);
    CHECK(variant.GetType() == "long");

    // Unsigned long wxAny should convert to "ulonglong" wxVariant
    any = 1000UL;
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "ulonglong");
    CHECK(variant.GetLong() == 1000);

    any = vString;
    CHECK(any == "ABC");
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetString() == "ABC");

    // Must be able to build string wxVariant from wxAny built from
    // string literal
    any = "ABC";
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "string");
    CHECK(variant.GetString() == "ABC");
    any = L"ABC";
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "string");
    CHECK(variant.GetString() == L"ABC");

    any = vDouble;
    double d = any.As<double>();
    CHECK(TEST_DOUBLE_CONST == Approx(d).margin(FEQ_DELTA));
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(TEST_DOUBLE_CONST == Approx(variant.GetDouble()).margin(FEQ_DELTA));

    any = vBool;
    CHECK(any.As<bool>() == true);
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetBool() == true);

    any = wxAny(vChar);
    //CHECK(any.As<wxUniChar>() == 'A');
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetChar() == 'A');

    any = wxAny(vLongLong);
    CHECK(any == wxLL(0xAABBBBCCCC));
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "longlong");
    CHECK(variant.GetLongLong() == wxLongLong(wxLL(0xAABBBBCCCC)));

#if LONG_MAX == wxINT64_MAX
    // As a sanity check, test that wxVariant of type 'long' converts
    // seamlessly to 'longlong' (on some 64-bit systems)
    any = 0xAABBBBCCCCL;
    res = any.GetAs(&variant);
    CHECK(variant.GetLongLong() == wxLongLong(wxLL(0xAABBBBCCCC)));
#endif

    any = wxAny(vULongLong);
    CHECK(any == wxLL(123456));
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "ulonglong");
    CHECK(variant.GetULongLong() == wxULongLong(wxULL(123456)));

    any = (wxLongLong_t)-1;
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "long");
    CHECK(variant.GetLong() == -1);

    // Cannot test equality for the rest, just test that they convert
    // back correctly.
    any = wxAny(vArrayString);
    res = any.GetAs(&variant);
    CHECK(res);
    wxArrayString arrstr2 = variant.GetArrayString();
    CHECK(arrstr2 == arrstr);

    any = m_testDateTime;
    CHECK(any.As<wxDateTime>() == m_testDateTime);
    any = wxAny(vDateTime);
    CHECK(any.As<wxDateTime>() == m_testDateTime);
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant == m_testDateTime);

    any = wxAny(vVoidPtr);
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetVoidPtr() == dummyVoidPointer);

    any = wxAny(vList);
    CHECK(wxANY_CHECK_TYPE(any, wxAnyList));
    wxAnyList anyList = any.As<wxAnyList>();
    CHECK(anyList.GetCount() == 2);
    CHECK((*anyList[0]).As<int>() == 15);
    CHECK((*anyList[1]).As<wxString>() == "abc");
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "list");
    CHECK(variant.GetCount() == 2);
    CHECK(variant[0].GetLong() == 15);
    CHECK(variant[1].GetString() == "abc");
    // Avoid the memory leak.
    wxClearList(anyList);

    any = wxAny(vCustomType);
    CHECK(wxANY_CHECK_TYPE(any, wxVariantData*));
    res = any.GetAs(&variant);
    CHECK(res);
    CHECK(variant.GetType() == "MyClass");

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

TEST_CASE_METHOD(AnyTestCase, "wxAny::CustomTemplateSpecialization", "[any]")
{
    // Do only a minimal CheckType() test, as dynamic type conversion already
    // uses it a lot.
    bool res;
    MyClass myObject;
    wxAny any = myObject;

    CHECK( wxANY_CHECK_TYPE(any, MyClass) );
    MyClass myObject2 = any.As<MyClass>();
    wxUnusedVar(myObject2);

    wxString str;
    res = any.GetAs(&str);
    CHECK(res);
    CHECK(myObject.ToString() == str);
}

TEST_CASE_METHOD(AnyTestCase, "wxAny::Misc", "[any]")
{
    // Do some (inplace) allocation sanity checks
    {

        // Do it inside a scope so we can easily test instance count
        // afterwards
        MyClass myObject(15);
        wxAny any = myObject;

        // There must be two instances - first in myObject,
        // and second copied in any.
        CHECK(2 == gs_myClassInstances.size());

        // Check that it is allocated in-place, as supposed
        if ( sizeof(MyClass) <= WX_ANY_VALUE_BUFFER_SIZE )
        {
            // Memory block of the instance second must be inside the any
            size_t anyBegin = reinterpret_cast<size_t>(&any);
            size_t anyEnd = anyBegin + sizeof(wxAny);
            size_t pos = reinterpret_cast<size_t>(gs_myClassInstances[1]);
            CHECK( pos >= anyBegin );
            CHECK( pos < anyEnd );
        }

        wxAny any2 = any;
        CHECK( any2.As<MyClass>().GetValue() == 15 );
    }

    // Make sure allocations and deallocations match
    CHECK(0 == gs_myClassInstances.size());
}

#endif // wxUSE_ANY

