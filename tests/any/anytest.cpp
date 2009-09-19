///////////////////////////////////////////////////////////////////////////////
// Name:        tests/any/anytest.cpp
// Purpose:     Test the wxAny classes
// Author:      Jaakko Salli
// RCS-ID:      $Id$
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#if wxUSE_ANY

#include "wx/any.h"
#include "wx/datetime.h"

#include <math.h>

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class wxAnyTestCase : public CppUnit::TestCase
{
public:
    wxAnyTestCase();

private:
    CPPUNIT_TEST_SUITE( wxAnyTestCase );
        CPPUNIT_TEST( Equality );
        CPPUNIT_TEST( As );
        CPPUNIT_TEST( GetAs );
        CPPUNIT_TEST( Null );
        CPPUNIT_TEST( CustomTemplateSpecialization );
    CPPUNIT_TEST_SUITE_END();

    void Equality();
    void As();
    void GetAs();
    void Null();
    void CustomTemplateSpecialization();

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

    DECLARE_NO_COPY_CLASS(wxAnyTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( wxAnyTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( wxAnyTestCase, "wxAnyTestCase" );

// Let's use a number with first digit after decimal dot less than 5,
// so that we don't have to worry about whether conversion from float
// to int truncates or rounds.
const double TEST_FLOAT_CONST = 123.456;

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
      m_anyFloatDouble1((float)TEST_FLOAT_CONST),
      m_anyDoubleDouble1((double)TEST_FLOAT_CONST),
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
    m_anyFloatDouble2 = (float)TEST_FLOAT_CONST;
    m_anyDoubleDouble2 = (double)TEST_FLOAT_CONST;
    m_anyDateTime2 = m_testDateTime;
    m_anyWxObjectPtr2 = dummyWxObjectPointer;
    m_anyVoidPtr2 = dummyVoidPointer;
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
    CPPUNIT_ASSERT_DOUBLES_EQUAL(wxANY_AS(m_anyFloatDouble1, double),
                                 wxANY_AS(m_anyDoubleDouble1, double),
                                 FEQ_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(wxANY_AS(m_anyFloatDouble1, double),
                                 TEST_FLOAT_CONST,
                                 FEQ_DELTA);
    CPPUNIT_ASSERT(m_anyWxObjectPtr1.As<wxObject*>() == dummyWxObjectPointer);
    CPPUNIT_ASSERT(m_anyVoidPtr1.As<void*>() == dummyVoidPointer);

    CPPUNIT_ASSERT(m_anySignedLong2 == 15);
    CPPUNIT_ASSERT(m_anyStringString2 == wxString("abc"));
    CPPUNIT_ASSERT(m_anyStringString2 == "abc");
    CPPUNIT_ASSERT(m_anyStringString2 == L"abc");
    CPPUNIT_ASSERT(m_anyBool2 == true);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(wxANY_AS(m_anyFloatDouble2, double),
                                 wxANY_AS(m_anyDoubleDouble2, double),
                                 FEQ_DELTA);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(wxANY_AS(m_anyFloatDouble2, double),
                                 TEST_FLOAT_CONST,
                                 FEQ_DELTA);
    CPPUNIT_ASSERT(m_anyWxObjectPtr2.As<wxObject*>() == dummyWxObjectPointer);
    CPPUNIT_ASSERT(m_anyVoidPtr2.As<void*>() == dummyVoidPointer);
}

void wxAnyTestCase::As()
{
    //
    // Test getting C++ data from wxAny without dynamic conversion
    signed char a = wxANY_AS(m_anySignedChar1, signed char);
    CPPUNIT_ASSERT(a == (signed int)15);
    signed short b = wxANY_AS(m_anySignedShort1, signed short);
    CPPUNIT_ASSERT(b == (signed int)15);
    signed int c = wxANY_AS(m_anySignedInt1, signed int);
    CPPUNIT_ASSERT(c == (signed int)15);
    signed long d = wxANY_AS(m_anySignedLong1, signed long);
    CPPUNIT_ASSERT(d == (signed int)15);
#ifdef wxLongLong_t
    wxLongLong_t e = wxANY_AS(m_anySignedLongLong1, wxLongLong_t);
    CPPUNIT_ASSERT(e == (signed int)15);
#endif
    unsigned char f = wxANY_AS(m_anyUnsignedChar1, unsigned char);
    CPPUNIT_ASSERT(f == (unsigned int)15);
    unsigned short g = wxANY_AS(m_anyUnsignedShort1, unsigned short);
    CPPUNIT_ASSERT(g == (unsigned int)15);
    unsigned int h = wxANY_AS(m_anyUnsignedInt1, unsigned int);
    CPPUNIT_ASSERT(h == (unsigned int)15);
    unsigned long i = wxANY_AS(m_anyUnsignedLong1, unsigned long);
    CPPUNIT_ASSERT(i == (unsigned int)15);
#ifdef wxLongLong_t
    wxULongLong_t j = wxANY_AS(m_anyUnsignedLongLong1, wxULongLong_t);
    CPPUNIT_ASSERT(j == (unsigned int)15);
#endif
    wxString k = wxANY_AS(m_anyStringString1, wxString);
    CPPUNIT_ASSERT(k == "abc");
    wxString l = wxANY_AS(m_anyCharString1, wxString);
    CPPUNIT_ASSERT(l == "abc");
    wxString m = wxANY_AS(m_anyWcharString1, wxString);
    CPPUNIT_ASSERT(m == "abc");
    bool n = wxANY_AS(m_anyBool1, bool);
    CPPUNIT_ASSERT(n);
    float o = wxANY_AS(m_anyFloatDouble1, float);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(o, TEST_FLOAT_CONST, FEQ_DELTA);
    double p = wxANY_AS(m_anyDoubleDouble1, double);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(p, TEST_FLOAT_CONST, FEQ_DELTA);
    wxDateTime q = wxANY_AS(m_anyDateTime1, wxDateTime);
    CPPUNIT_ASSERT(q == m_testDateTime);
    wxObject* r = wxANY_AS(m_anyWxObjectPtr1, wxObject*);
    CPPUNIT_ASSERT(r == dummyWxObjectPointer);
    void* s = wxANY_AS(m_anyVoidPtr1, void*);
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
    unsigned long ul = 0;
    wxString s;
    // Let's test against float instead of double, since the former
    // is not the native underlying type the code converts to, but
    // should still work, all the same.
    float f = 0.0;
    bool b = false;

    // Conversions from signed long type
    res = m_anySignedLong1.GetAs(&ul);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(ul, static_cast<unsigned long>(15));
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
    res = s.ToDouble(&d2);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_DOUBLES_EQUAL(d2, TEST_FLOAT_CONST, FEQ_DELTA);
}

//
// Test user data type specialization of wxAnyValueTypeImpl
//

class MyClass
{
public:
    MyClass( int someValue = 32768 )
    {
        m_someValue = someValue;
    }

    wxString ToString()
    {
        return wxString::Format("%i", m_someValue);
    }

private:
    int     m_someValue;
};


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
                              wxAnyValueBuffer& dst) const
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
    MyClass myObject2 = wxANY_AS(any, MyClass);
    wxUnusedVar(myObject2);

    wxString str;
    res = any.GetAs(&str);
    CPPUNIT_ASSERT(res);
    CPPUNIT_ASSERT_EQUAL(str, myObject.ToString());
}

#endif // wxUSE_ANY

