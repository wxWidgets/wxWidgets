///////////////////////////////////////////////////////////////////////////////
// Name:        tests/masked/maskededit.cpp
// Purpose:     wxMaskedEdit unit test
// Author:      Julien Weinzorn
// Created:     2010-06-02
// RCS-ID:      $Id: ??????????????????????????????? $
// Copyright:   (c) 2009 Julien Weinzorn
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "testfile.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MaskedEditTestCase : public CppUnit::TestCase
{
public:
    MaskedEditTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MaskedEditTestCase );
        CPPUNIT_TEST( GetPlainValueTest );
    CPPUNIT_TEST_SUITE_END();

    void GetPlainValueTest();

    wxDECLARE_NO_COPY_CLASS(MaskedEditTestCase);
};

// ----------------------------------------------------------------------------
// CppUnit macros
// ----------------------------------------------------------------------------

CPPUNIT_TEST_SUITE_REGISTRATION( MaskedEditTestCase );
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MaskedEditTestCase, "MaskedEditTestCase" );


// ----------------------------------------------------------------------------
// tests implementation
// ----------------------------------------------------------------------------

void MaskedEditTestCase::GetPlainValueTest()
{

}

