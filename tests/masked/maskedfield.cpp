///////////////////////////////////////////////////////////////////////////////
// Name:        tests/filename/maskedfield.cpp
// Purpose:     wxMaskedField unit test
// Author:      Julien Weinzorn
// Created:     2004-07-25
// RCS-ID:      $Id: ??????????????? $
// Copyright:   (c) 2010 Julien Weinzorn
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifndef WX_PRECOMP
    #include "wx/utils.h"
#endif // WX_PRECOMP

#ifdef __WXMSW__
    #include "wx/msw/registry.h"
#endif // __WXMSW__

#include "wx/maskedfield.h"

// ----------------------------------------------------------------------------
// test data
// ----------------------------------------------------------------------------

static struct TestMaskedField
{
    wxString* test;
    wxString* mask;        
    wxString* formatCode;
    wxArrayString* choices;
    bool autoSelect;
    wxChar groupChar;
    wxChar decimalPoint;
    bool useParensForNegatives;

}

masked[]=
    {
        // default parameter and empty string test
        {wxT("")}
        {wxT(""), wxT(""), NULL , true , '', '?', true},
        {wxT(""), wxT(""), NULL , true , '', '?', true},
        {wxT(""), wxT(""), NULL , true , '', '.', false},
        {wxT(""), wxT(""), NULL , false, '', '' , false},

        //various mask and different format code
        {wxT("127.000.000.001") , wxT("###.###.###.###") , wxT("F") , NULL, true , '' , '.', false},
        {wxT("127.AVc.bAc") , wxt("###.CCC.aAC") , wxT("F!"), NULL, true , ' ', '.', false},
        {wxT("12#.AC?fA") , wxT("##\#.C\C?a\A"), wxT("F") , NULL, false, ' ', '.', false}
    };

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MaskedFieldTestCase : public CppUnit::TestCase
{
public:
    MaskedFieldTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MaskedFieldTestCase );
        CPPUNIT_TEST( TestIsNumber );
    CPPUNIT_TEST_SUITE_END();

    void TestValid();

    DECLARE_NO_COPY_CLASS(MaskedFieldTestCase)
};

void TestIsNumber()
{
    static struct TestNumber
    {
        wxChar test;
        wxChar result;

    }
    maskedNumber[]=
    {
        {'0' , true},
        {'/' , false},
        {'9' , true},
        {':' , false},
    };


    for(unsigned int n = 0; n< WXSIZEOF(maskedNumber); n++)
    {
         CPPUNIT_ASSERT_EQUAL( IsValid(maskedNumber[n].test), maskedNumber[n].result );    
    }
}



// register in the unnamed registry.
CPPUNIT_TEST_SUITE_REGISTRATION( MaskedFieldTestCase );

// Include in it's own registry.
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MaskedFieldTestCase, "MaskedFieldTestCase" );
