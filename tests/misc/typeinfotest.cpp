///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/typeinfotest.cpp
// Purpose:     Test typeinfo.h
// Author:      Jaakko Salli
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#include "wx/typeinfo.h"

// ----------------------------------------------------------------------------
// test types
// ----------------------------------------------------------------------------

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

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

TEST_CASE("TypeInfo::Compare", "[typeinfo]")
{
    UserNameSpace::UserType1 uns_ut1;
    UserNameSpace::UserType1* uns_ut1_p = new UserNameSpace::UserType1();
    UserType1 ut1;
    UserType1* ut1_p = new UserType1();
    UserType2 ut2;
    UserType2* ut2_p = new UserType2();

    // These type comparison should match
    CHECK(wxTypeId(uns_ut1) == wxTypeId(*uns_ut1_p));
    CHECK(wxTypeId(ut1) == wxTypeId(*ut1_p));
    CHECK(wxTypeId(ut2) == wxTypeId(*ut2_p));

    // These type comparison should not match
    CHECK(wxTypeId(uns_ut1) != wxTypeId(ut1));
    CHECK(wxTypeId(uns_ut1) != wxTypeId(ut2));
    CHECK(wxTypeId(ut1) != wxTypeId(ut2));

    delete uns_ut1_p;
    delete ut1_p;
    delete ut2_p;
}

