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

#include "wx/maskededit.h"

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
        CPPUNIT_TEST( IsValidTest      );
        CPPUNIT_TEST( SetMaskTest      );
        CPPUNIT_TEST( AddChoiceTest    );
        CPPUNIT_TEST( AddChoicesTest   );
        CPPUNIT_TEST( SetMaskFieldTest );
        CPPUNIT_TEST( AddChoiceFieldTest);
    CPPUNIT_TEST_SUITE_END();

    void GetPlainValueTest();
    void IsValidTest();
    void SetMaskTest();
    void AddChoiceTest();
    void AddChoicesTest();
    void SetMaskFieldTest();
    void AddChoiceFieldTest();

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
    static struct TestPlainValue
    {
        wxString mask;      
        wxString formatCodes;
        wxString test;
        wxString result;


    }
    maskedPlainValue[]=
    {
        {wxT("###.###.###.###"), wxT("F_"), wxT("1.2.3.4"), wxT("1  2  3  4  ")},
        {wxT("###.|###.|###.|###"), wxT("F_"), wxT("1.2.3.4"), wxT("1  2  3  4  ")},
        {wxT("###.AAA.aC\\&"), wxT("F!"), wxT("111.aaa.a2&"), wxT("111AAAaZ")},
        {wxT("###.A|AA|.aX|\\&"), wxT("F!"), wxT("111.aaa.a2&"), wxT("111AAAaZ")},
        {wxT("#XX."), wxT("F!_"), wxT("3rt."), wxT("3rt")},
        {wxT("#X|X."), wxT("F!_"), wxT("3rt."), wxT("3rt")},
    };


    for(unsigned int n = 0; n< WXSIZEOF(maskedPlainValue); n++)
    {
        printf("%d\n", n);
        wxMaskedEdit mask(maskedPlainValue[n].mask, maskedPlainValue[n].formatCodes); 
        wxString res = mask.GetPlainValue(maskedPlainValue[n].test); 
        CPPUNIT_ASSERT( res.Cmp(maskedPlainValue[n].result) == 0 );    
    }


}

void MaskedEditTestCase::IsValidTest()
{
    static struct TestValid
    {
        wxString mask;
        wxString test;
        bool result;
    }
    listTest[]=
    {
        //same as maskedfield
        {wxT("") , wxT("") , true}, 
        {wxT("") , wxT("azd") , false},
        {wxT("###") , wxT("123") , true},
        {wxT("###") , wxT("Az3") , false},
        {wxT("###") , wxT("wx.;") , false},
        {wxT("###") , wxT("12,") , false},

        {wxT("AAa.#X*") , wxT("AZc.3,|") , true},
        {wxT("AAa.#X*") , wxT("AZc3,|")  , false}, // correct or not?
        {wxT("AAa.#X*") , wxT("aZc.|.|") , false},
        {wxT("AAa.#X*") , wxT("")        , false},  // correct or not?
        {wxT("AAa.#X*") , wxT("AZc.3,|4"), false},

        {wxT("###\\*###") , wxT("") , true},
        {wxT("###\\*###") , wxT("123*593") , true},
        {wxT("###\\*###") , wxT("123456") , false}, //correct or not?
        {wxT("###\\*###") , wxT("124\\*45") , false},

        //with more than one field
        {wxT("AAa|.#X*") , wxT("AZc.3,|") , true},
        {wxT("AA|a.#|X*"), wxT("AZc3,|")  , false}, // correct or not?
        {wxT("AAa.#X*")  , wxT("aZc.|.|") , false},
        {wxT("A|Aa.#X*") , wxT("")        , false},  // correct or not?
        {wxT("AAa.|#X*") , wxT("AZc.3,|4"), false},

        {wxT("#|##\\*#|##") , wxT("") , true},
        {wxT("###\\*###")   , wxT("123*593") , true},
        {wxT("|###\\*#|##") , wxT("123456") , false}, //correct or not?
        {wxT("|###\\*###|") , wxT("124\\*45") , false}

    
    };

    for(unsigned int n = 0; n< WXSIZEOF(listTest); n++)
    {
        wxMaskedEdit mask(listTest[n].mask, wxT(""));
        CPPUNIT_ASSERT( mask.IsValid(listTest[n].test) == listTest[n].result); 
    }

}


void MaskedEditTestCase::SetMaskTest()
{
    static struct TestMask
    {
        wxString mask;
        wxString newMask;
        bool result;
    }
    masked[]=
    {
        {wxT("AAa.#X*") , wxT("AZc.3,|") , false},
        {wxT("AA|a.#|X*") , wxT("AZc3,|")  , false},
        {wxT("AAa.#X*") , wxT("AZc..") , true},
        {wxT("A|Aa.#X*") , wxT("")        , false},
        {wxT("AAa.|#X*") , wxT("AZc.3,|4"), false},
    }; 

    for(unsigned int n = 0; n< WXSIZEOF(masked); n++)
    {
        wxMaskedEdit mask(masked[n].mask, wxT(""));
        CPPUNIT_ASSERT( mask.SetMask(masked[n].newMask) == masked[n].result); 
    }
}

void MaskedEditTestCase::AddChoiceTest()
{
    static struct TestChoice
    {
        wxString mask;
        wxString choice;
        bool result;
    }
    masked[]=
    {
        {wxT("###.###") , wxT("111.111") , true},
        {wxT("###|.###") , wxT("111.111")  , false},
        {wxT("###.###") , wxT("1.1") , false},
        {wxT("###|.###") , wxT("111.111")  , false},
        {wxT("###.###") , wxT("1a2.34t") , false},
        {wxT("###|.###") , wxT("123|.111")  , false},
    }; 

    for(unsigned int n = 0; n< WXSIZEOF(masked); n++)
    {
        wxMaskedEdit mask(masked[n].mask, wxT(""));
        CPPUNIT_ASSERT( mask.AddChoice(masked[n].choice) == masked[n].result); 
    }


}
//FIXME how to test this
void MaskedEditTestCase::AddChoicesTest()
{
    
}

void MaskedEditTestCase::SetMaskFieldTest()
{
    static struct TestMask
    {
        wxString mask;
        wxString newMask;
        unsigned int field;
        bool result;
    }
    masked[]=
    {
        {wxT("AAa.#X*")   , wxT("AZc.3,")  , 0 , true},
        {wxT("AA|a.#|X*") , wxT("AZc3,|")  , 1 , false},
        {wxT("AAa.#X*")   , wxT("AZc..")   , 1 , false},
        {wxT("A|Aa.#X*")  , wxT("")        , 0 , true},
        {wxT("AAa.|#X*")  , wxT("AZc.3,|4"), 2 , false},
    }; 

    for(unsigned int n = 0; n< WXSIZEOF(masked); n++)
    {
        wxMaskedEdit mask(masked[n].mask, wxT(""));
        CPPUNIT_ASSERT( mask.SetMask(masked[n].field, masked[n].newMask) == masked[n].result); 
    }
   
}

void MaskedEditTestCase::AddChoiceFieldTest()
{
    static struct TestChoice
    {
        wxString mask;
        wxString choice;
        unsigned int field;
        bool result;
    }
    masked[]=
    {
        {wxT("###.###")  , wxT("111.111") , 0 , true},
        {wxT("###|.###") , wxT("111.111") , 0 , false},
        {wxT("###.###")  , wxT("1.1")     , 0 , false},
        {wxT("###|.###") , wxT("111")     , 0 , true},
        {wxT("###|.###") , wxT("111")     , 1 , false},
        {wxT("###.###")  , wxT("1a2.34t") , 1 , false},
        {wxT("###|.###") , wxT("123|.111"), 1 , false},
    }; 

    for(unsigned int n = 0; n< WXSIZEOF(masked); n++)
    {
        wxMaskedEdit mask(masked[n].mask, wxT(""));
        CPPUNIT_ASSERT( mask.AddChoice(masked[n].choice) == masked[n].result); 
    }
}
