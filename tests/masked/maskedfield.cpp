///////////////////////////////////////////////////////////////////////////////
// Name:        tests/filename/maskedfield.cpp
// Purpose:     wxMaskedField unit test
// Author:      Julien Weinzorn
// Created:     20***-07-25
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
    #include "wx/wx.h"
#endif // WX_PRECOMP

#ifdef __WXMSW__
    #include "wx/msw/registry.h"
#endif // __WXMSW__

#include "wx/maskedfield.h"


// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class MaskedFieldTestCase : public CppUnit::TestCase
{
public:
    MaskedFieldTestCase() { }

private:
    CPPUNIT_TEST_SUITE( MaskedFieldTestCase );
        CPPUNIT_TEST( TestCreate      );
        CPPUNIT_TEST( TestIsCharValid );
        CPPUNIT_TEST( TestIsValid     );
        CPPUNIT_TEST( TestIsEmpty     );
        CPPUNIT_TEST( TestApplyFormatsCode );
        CPPUNIT_TEST( TestAddChoice        );
        CPPUNIT_TEST( TestAddChoices       );
        CPPUNIT_TEST( TestSetMask          );
        CPPUNIT_TEST( TestSetDefaultValue  );
        CPPUNIT_TEST( TestGetPlainValue    );
        CPPUNIT_TEST( TestGetEmptyedMask    );
    CPPUNIT_TEST_SUITE_END();

    void TestCreate();
    void TestIsCharValid();
    void TestIsValid();
    void TestIsEmpty();
    void TestApplyFormatsCode();
    void TestAddChoice();
    void TestAddChoices();
    void TestSetMask();
    void TestSetDefaultValue();
    void TestGetPlainValue();
    void TestGetEmptyedMask();

    DECLARE_NO_COPY_CLASS(MaskedFieldTestCase)
};

// register in the unnamed registry.
CPPUNIT_TEST_SUITE_REGISTRATION( MaskedFieldTestCase );

// Include in it's own registry.
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( MaskedFieldTestCase, "MaskedFieldTestCase" );

void MaskedFieldTestCase::TestCreate()
{
    static struct TestCreate
    {
        wxString mask;
        wxString maskRes;
        bool result;
    }
    listTest[]=
    {
        {wxT("")        , wxT("")        , true}, 
        {wxT("###")     , wxT("###")     , true},
        {wxT("###.")    , wxT("###.")    , true},
        {wxT("Aa*\\*")  , wxT("Aa*\\*")  , true},
        {wxT("A{10}##") , wxT("AAAAAAAAAA##") , true},
        {wxT("Aa\\")    , wxT("")        , false},
        {wxT("{43}A#")  , wxT("")        , false},
        {wxT("#{23A}*") , wxT("")        , false},
        {wxT("\\*{23}#"), wxT("")        , false},
        {wxT("##C}23}") , wxT("")        , false},
        {wxT("###{2}}") , wxT("")        , false},
        {wxT("#{###")   , wxT("")        , false},
        {wxT("#}###")   , wxT("")        , false},
        {wxT("A{aaA")   , wxT("")        , false},
        {wxT("Aa{5} A{7}")   , wxT("Aaaaaa AAAAAAA") , true},

    };

    for(unsigned int n = 0; n< WXSIZEOF(listTest); n++)
    {
        wxMaskedField mask(listTest[n].mask);
    
        CPPUNIT_ASSERT_EQUAL( listTest[n].result, mask.Create(listTest[n].mask));
        CPPUNIT_ASSERT(listTest[n].maskRes.Cmp(mask.GetMask()) == 0);    
    }

}
void MaskedFieldTestCase::TestIsCharValid()
{
    static const struct TestIsCharValid
    {
        char charTest;
        char maskTest;
        bool result;

    }
    maskedCharValid[]=
    {
        {'1' ,'#', true},
        {'1' ,'a', false},
        {'1' ,'A', false},
        {'1' ,'X', true},
        {'1' ,'&', false},
        {'1' ,'*', true},
        {'1' ,'C', false},
        {'1' ,'N', true},

        {'a' ,'#', false},
        {'a' ,'a', true},
        {'a' ,'A', false},
        {'a' ,'X', true},
        {'a' ,'&', false},
        {'a' ,'*', true},
        {'a' ,'C', true},
        {'a' ,'N', true},   
        
        {'A' ,'#', false},
        {'A' ,'a', false},
        {'A' ,'A', true},
        {'A' ,'X', true},
        {'A' ,'&', false},
        {'A' ,'*', true},
        {'A' ,'C', true},
        {'A' ,'N', true}, 
        
        {'.' ,'#', false},
        {'.' ,'a', false},
        {'.' ,'A', false},
        {'.' ,'X', true},
        {'.' ,'&', true},
        {'.' ,'*', true},
        {'.' ,'C', false},
        {'.' ,'N', false},    
        
        {'|' ,'#', false},
        {'|' ,'a', false},
        {'|' ,'A', false},
        {'|' ,'X', false},
        {'|' ,'&', false},
        {'|' ,'*', true},
        {'|' ,'C', false},
        {'|' ,'N', false},
    };


    for(unsigned int n = 1; n< WXSIZEOF(maskedCharValid); n++)
    {
       wxMaskedField mask(wxT("###"));
        CPPUNIT_ASSERT_EQUAL( mask.IsCharValid(maskedCharValid[n].maskTest, maskedCharValid[n].charTest) , maskedCharValid[n].result );    
    }

}


void MaskedFieldTestCase::TestIsValid()
{
    wxString formatString;
    static struct TestValid
    {
        wxString mask;
        wxString formatCodes;
        wxString test;
        bool result;
    }
    listTest[]=
    {
        {wxT("")     , wxT("F")  , wxT("")     , true}, 
        {wxT("")     , wxT("_!") , wxT("azd")  , false},
        {wxT("###")  , wxT("_")  , wxT("123")  , true},
        {wxT("###")  , wxT("")   , wxT("12")   , true},
        {wxT("###")  , wxT("")   , wxT("Az3")  , false},
        {wxT("###")  , wxT("")   , wxT("wx.;") , false},
        {wxT("###.") , wxT("")   , wxT("1.2")  , false},
        {wxT("###.") , wxT("")   , wxT("1  .") , false},
        {wxT("###.") , wxT("_")   , wxT("1")   , true},
        {wxT("###.") , wxT("")   , wxT("1.")   , false},
        {wxT("###.") , wxT("_")  , wxT("1.")   , false},
        {wxT("###.") , wxT("_")  , wxT("111.")   , true},


        {wxT("aaa")      , wxT("_")  , wxT("a")       , true},
        {wxT("AaA.")     , wxT("_")  , wxT("a")       , false},
        {wxT("Aaa\\*.")  , wxT("_")  , wxT("A")       , true},
        {wxT("Aaa\\*.A") , wxT("_")  , wxT("Aaa*.Aa") , false},
        {wxT("Aaa\\*.A") , wxT("_")  , wxT("Aaa*")  , true},

        {wxT("AAa.#X*") , wxT("F"), wxT("AZc.3,|") , true},
        {wxT("AAa.#X*") , wxT("F"), wxT("AZc3,|")  , false},
        {wxT("AAa.#X*") , wxT("F"), wxT("aZc.|.|") , false},
        {wxT("AAa.#X*") , wxT("F"), wxT("")        , false},
        {wxT("AAa.#X*") , wxT("F"), wxT("AZc.3,|4"), false},

        {wxT("###\\*###") , wxT("F"), wxT("")         , false},
        {wxT("###\\*###") , wxT("F"), wxT("123*593")  , true},
        {wxT("###\\*###") , wxT("F"), wxT("123456")   , false},
        {wxT("###\\*###") , wxT("F"), wxT("124\\*45") , false},
        
        {wxT("#{4}") , wxT("F"), wxT("1234")  , true},
        {wxT("Aa{5} A{5}"), wxT("F!_"), wxT("A"), true},

        {wxT("###")  , wxT("F-")   , wxT("1")  , true},
        {wxT("###")  , wxT("F-_")   , wxT("1")  , true},
        {wxT("###")  , wxT("F-")   , wxT("111")  , true},
        {wxT("###")  , wxT("F-")   , wxT("-111") , true},
 
    };

    for(unsigned int n = 0; n< WXSIZEOF(listTest); n++)
    {
        printf("N= %d\n", n);

        wxMaskedField mask(listTest[n].mask, listTest[n].formatCodes);
        formatString = mask.ApplyFormatCodes(listTest[n].test);
        CPPUNIT_ASSERT_EQUAL( listTest[n].result, mask.IsValid(formatString));
    }

}

void MaskedFieldTestCase::TestIsEmpty()
{
    static struct TestEmpty
    {
        wxString mask;      
        wxString test;
        bool result;


    }
    maskedEmpty[]=
    {
        {wxT("###.###.###.###"), wxT("...")            , false},
        {wxT("###.###.###.###"), wxT("   .   .   .   "), true },
        {wxT("###.###.###.###"), wxT("1  .2  .123.111"), false},
        {wxT("###.###.###.###"), wxT("123.000.000.111"), false},
        
        {wxT("\\CAA--#{3}\\*") , wxT("\\CAA--#{3}\\*") , false},
        {wxT("\\CAA--#{3}\\*") , wxT("CAF--123*")      , false},
        {wxT("\\CAA--#{3}\\*") , wxT("C  --   *")      , true},
    };


    for(unsigned int n = 0; n< WXSIZEOF(maskedEmpty); n++)
    {
        wxMaskedField mask(maskedEmpty[n].mask, wxT("F_")); 
        CPPUNIT_ASSERT_EQUAL( maskedEmpty[n].result , mask.IsEmpty(maskedEmpty[n].test) );    
    }

}
void MaskedFieldTestCase::TestApplyFormatsCode()
{
    static struct TestFormatCode
    {
        wxString mask;      
        wxString formatCodes;
        wxString test;
        wxString result;


    }
    maskedFormatCode[]=
    {
        {wxT("")               , wxT("F_") , wxT("azd")     , wxT("azd")},
        {wxT("Aaa")            , wxT("F_") , wxT("a")     , wxT("a")},
        {wxT("###")            , wxT("F_") , wxT("")        , wxT("")},
        {wxT("###.###.###.###"), wxT("F_") , wxT("1.2.3.4") , wxT("1.2.3.4")},
        {wxT("###.A"), wxT("F!"), wxT("111.a"), wxT("111.A")},
        {wxT("###.A"), wxT("F!"), wxT("111.aaa"), wxT("111.aaa")},
        {wxT("###"), wxT("F_"), wxT("1a"), wxT("1a")},
        {wxT("#A#."), wxT("F_"), wxT("11"), wxT("11")},
        {wxT("###."), wxT("F_"), wxT("1111"), wxT("1111")},
        {wxT("Aaa\\*."), wxT("F_"), wxT("A"), wxT("A")},
        {wxT("Aaa\\*."), wxT("F!_"), wxT("abc"), wxT("Abc")},
        
        {wxT("###.AAA.aC\\&"), wxT("F!"), wxT("111.aaa.ab&"), wxT("111.AAA.ab&")},
        {wxT("CX.X"), wxT("F!_"), wxT("rt."), wxT("rt.")},
        {wxT("CX.X"), wxT("F!_"), wxT("2242"), wxT("2242")},

        {wxT("###."), wxT("F!_"), wxT("1."), wxT("1.")},

        //test with space
        {wxT("###")            , wxT("F_") , wxT("   ") , wxT("   ")},
        {wxT("###.###.###.###"), wxT("F_") , wxT("1  .2  .3  .4") , wxT("1  .2  .3  .4")},
        {wxT("###.A"), wxT("F!_"), wxT("1  .a"), wxT("1  .A")},
        {wxT("###.A"), wxT("F!"), wxT("1  .a "), wxT("1  .a ")},
        {wxT("###"), wxT("F_"), wxT("1  a"), wxT("1  a")},
 
    };


    for(unsigned int n = 0; n< WXSIZEOF(maskedFormatCode); n++)
    {
        wxMaskedField mask(maskedFormatCode[n].mask, maskedFormatCode[n].formatCodes);
        wxString res = mask.ApplyFormatCodes(maskedFormatCode[n].test);

        CPPUNIT_ASSERT( res.Cmp(maskedFormatCode[n].result) == 0 );
    }

}

void MaskedFieldTestCase::TestAddChoice()
{
    static struct TestChoice
    {
        wxString mask;      
        wxString test;
        bool result;


    }
    maskedChoice[]=
    {
        {wxT("###.###.###.###"), wxT("111.222.333.444"), true},
        {wxT("###.###.###.###"), wxT("111.2z2.333.444"), false},
        {wxT("###.###.###.###"), wxT("111.222.3.444"),   false},
        {wxT("###.###.###.###"), wxT(""), false},
    };


    for(unsigned int n = 0; n< WXSIZEOF(maskedChoice); n++)
    {
        wxMaskedField mask(maskedChoice[n].mask); 
        CPPUNIT_ASSERT( mask.AddChoice(maskedChoice[n].test) ==  
                        maskedChoice[n].result );    
    }

}
void MaskedFieldTestCase::TestAddChoices()
{
    wxMaskedField mask(wxT("###.###.AA\\#"));
        
    wxArrayString testChoices = wxArrayString();



    CPPUNIT_ASSERT( mask.AddChoices(testChoices) == true );    
    testChoices.Add(wxT("111.111.CD#"));
    CPPUNIT_ASSERT( mask.AddChoices(testChoices) == true );
    testChoices.Add(wxT("111.AAA.CD#"));
    CPPUNIT_ASSERT( mask.AddChoices(testChoices) == false );
    
    testChoices.Add(wxT("111.111.CD#"));
    CPPUNIT_ASSERT( mask.AddChoices(testChoices) == false );


}
//FIXME  how to test this
void MaskedFieldTestCase::TestSetMask()
{

    
}


void MaskedFieldTestCase::TestSetDefaultValue()
{
   static struct TestDefaultValue
    {
        wxString mask;      
        wxString formatCodes;
        wxString test;
        bool result;


    }
    maskedDefaultValue[]=
    {
        {wxT("###.###.###.###") , wxT("F_")  , wxT("111.111.111.1") , true},
        {wxT("###.AAA.aC\\&")   , wxT("F!")  , wxT("111.aaa.aZ&")   , true},
        {wxT("#XX.")            , wxT("F!_") , wxT("3rt.")          , true},
        {wxT("###.")            , wxT("F_")  , wxT("1.")            , false},
        {wxT("###")             , wxT("F")   , wxT("111")           , true},

    };


    wxString tmp;
    wxString res;
    for(unsigned int n = 0; n< WXSIZEOF(maskedDefaultValue); n++)
    {
        wxMaskedField mask(maskedDefaultValue[n].mask, maskedDefaultValue[n].formatCodes);
        
        tmp = mask.ApplyFormatCodes(maskedDefaultValue[n].test);
        CPPUNIT_ASSERT_EQUAL( maskedDefaultValue[n].result, mask.SetDefaultValue(tmp)  );    
    }

}


void MaskedFieldTestCase::TestGetPlainValue()
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
        {wxT("###.###.###.###") , wxT("F_")  , wxT("1.2.3.4")    , wxT("1.2.3.4")},
        {wxT("###.AAA.aC\\&")   , wxT("F!")  , wxT("111.aaa.aZ&"), wxT("111AAAaZ")},
        {wxT("#XX.")            , wxT("F!_") , wxT("3rt.")       , wxT("3rt")},
        {wxT("###.")            , wxT("F_")  , wxT("1.")         , wxT("1.")}
    };


    wxString tmp;
    wxString res;
    for(unsigned int n = 0; n< WXSIZEOF(maskedPlainValue); n++)
    {
        wxMaskedField mask(maskedPlainValue[n].mask, maskedPlainValue[n].formatCodes);
        
        tmp = mask.ApplyFormatCodes(maskedPlainValue[n].test);
        res = mask.GetPlainValue(tmp); 
        CPPUNIT_ASSERT( res.Cmp(maskedPlainValue[n].result) == 0 );    
    }
}

void MaskedFieldTestCase::TestGetEmptyedMask()
{ 
    static struct TestEmptyedMask
    {
        wxString mask;      
        wxString result;


    }
    maskedEmpty[]=
    {
        {wxT("###.###.###.###") , wxT("   .   .   .   ")},
        {wxT("###.AAA.aC\\&")   , wxT("   .   .  &")},
        {wxT("#XX.")            , wxT("   .")},
        {wxT("(###) - ###-####"), wxT("(   ) -    -    ")}
    };


    for(unsigned int n = 0; n< WXSIZEOF(maskedEmpty); n++)
    {
        wxMaskedField mask(maskedEmpty[n].mask, wxT("F"));
        
        CPPUNIT_ASSERT( mask.GetEmptyMask().Cmp(maskedEmpty[n].result) == 0 );    
    }



}
