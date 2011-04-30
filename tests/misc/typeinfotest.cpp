///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/typeinfotest.cpp
// Purpose:     Test typeinfo.h
// Author:      Jaakko Salli
// RCS-ID:      $Id$
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
#   pragma hdrstop
#endif

#include "wx/typeinfo.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class TypeInfoTestCase : public CppUnit::TestCase
{
public:
    TypeInfoTestCase() { }

private:
    CPPUNIT_TEST_SUITE( TypeInfoTestCase );
        CPPUNIT_TEST( Test );
    CPPUNIT_TEST_SUITE_END();

    void Test();

    DECLARE_NO_COPY_CLASS(TypeInfoTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( TypeInfoTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( TypeInfoTestCase, "TypeInfoTestCase" );


namespace UserNameSpace {
    class UserType1
    {
        WX_DECLARE_TYPEINFO_INLINE(UserType1)
    public:
        virtual ~UserType1() { }
    };
}

class UserType1
{
    WX_DECLARE_TYPEINFO_INLINE(UserType1)
public:
    virtual ~UserType1() { }
};

class UserType2
{
    WX_DECLARE_TYPEINFO(UserType2)
public:
    virtual ~UserType2() { }
};

WX_DEFINE_TYPEINFO(UserType2)

void TypeInfoTestCase::Test()
{
    UserNameSpace::UserType1 uns_ut1;
    UserNameSpace::UserType1* uns_ut1_p = new UserNameSpace::UserType1();
    UserType1 ut1;
    UserType1* ut1_p = new UserType1();
    UserType2 ut2;
    UserType2* ut2_p = new UserType2();

    // These type comparison should match
    CPPUNIT_ASSERT(wxTypeId(uns_ut1) == wxTypeId(*uns_ut1_p));
    CPPUNIT_ASSERT(wxTypeId(ut1) == wxTypeId(*ut1_p));
    CPPUNIT_ASSERT(wxTypeId(ut2) == wxTypeId(*ut2_p));

    // These type comparison should not match
    CPPUNIT_ASSERT(wxTypeId(uns_ut1) != wxTypeId(ut1));
    CPPUNIT_ASSERT(wxTypeId(uns_ut1) != wxTypeId(ut2));
    CPPUNIT_ASSERT(wxTypeId(ut1) != wxTypeId(ut2));

    delete uns_ut1_p;
    delete ut1_p;
    delete ut2_p;
}

