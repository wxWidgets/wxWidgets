///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/datastreamtest.cpp
// Purpose:     wxDataXXXStream Unit Test
// Author:      Ryan Norton
// Created:     2004-08-14
// RCS-ID:      $Id$
// Copyright:   (c) 2004 Ryan Norton
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

#include "wx/datstrm.h"
#include "wx/wfstream.h"
#include "wx/math.h"

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class DataStreamTestCase : public CppUnit::TestCase
{
public:
    DataStreamTestCase();

private:
    CPPUNIT_TEST_SUITE( DataStreamTestCase );
        CPPUNIT_TEST( FloatRW );
        CPPUNIT_TEST( DoubleRW );
        CPPUNIT_TEST( NaNRW );
    CPPUNIT_TEST_SUITE_END();

    void FloatRW();
    void DoubleRW();
    void NaNRW();

    DECLARE_NO_COPY_CLASS(DataStreamTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DataStreamTestCase );

// also include in it's own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataStreamTestCase, "DataStreamTestCase" );

DataStreamTestCase::DataStreamTestCase()
{
}

wxFloat64 TestFloatRW(wxFloat64 fValue)
{
    wxFileOutputStream* pFileOutput = new wxFileOutputStream( _T("mytext.dat") );
    wxDataOutputStream* pDataOutput = new wxDataOutputStream( *pFileOutput );

    *pDataOutput << fValue;

    delete pDataOutput;
    delete pFileOutput;

    wxFileInputStream* pFileInput = new wxFileInputStream( _T("mytext.dat") );
    wxDataInputStream* pDataInput = new wxDataInputStream( *pFileInput );

    wxFloat64 fInFloat;

    *pDataInput >> fInFloat;

    delete pDataInput;
    delete pFileInput;

    return fInFloat;
}

void DataStreamTestCase::FloatRW()
{
    CPPUNIT_ASSERT( TestFloatRW(5.5) == 5.5 );
    CPPUNIT_ASSERT( TestFloatRW(5) == 5 );
    CPPUNIT_ASSERT( TestFloatRW(5.55) == 5.55 );
    CPPUNIT_ASSERT( TestFloatRW(55555.555555) == 55555.555555 );
}

void DataStreamTestCase::DoubleRW()
{
    CPPUNIT_ASSERT( TestFloatRW(2132131.1232132) == 2132131.1232132 );
    CPPUNIT_ASSERT( TestFloatRW(21321343431.1232143432) == 21321343431.1232143432 );
}

void DataStreamTestCase::NaNRW()
{
    //TODO?
}


