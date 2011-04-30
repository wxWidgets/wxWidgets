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

#include <vector>

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
#if wxUSE_LONGLONG
        CPPUNIT_TEST( LongLongRW );
#endif
#if wxHAS_INT64
        CPPUNIT_TEST( Int64RW );
#endif
        CPPUNIT_TEST( NaNRW );
    CPPUNIT_TEST_SUITE_END();

    void FloatRW();
    void DoubleRW();
#if wxUSE_LONGLONG
    void LongLongRW();
#endif
#if wxHAS_INT64
    void Int64RW();
#endif
    void NaNRW();

    DECLARE_NO_COPY_CLASS(DataStreamTestCase)
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DataStreamTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataStreamTestCase, "DataStreamTestCase" );

DataStreamTestCase::DataStreamTestCase()
{
}

static
wxFloat64 TestFloatRW(wxFloat64 fValue)
{
    wxFileOutputStream* pFileOutput = new wxFileOutputStream( wxT("mytext.dat") );
    wxDataOutputStream* pDataOutput = new wxDataOutputStream( *pFileOutput );

    *pDataOutput << fValue;

    delete pDataOutput;
    delete pFileOutput;

    wxFileInputStream* pFileInput = new wxFileInputStream( wxT("mytext.dat") );
    wxDataInputStream* pDataInput = new wxDataInputStream( *pFileInput );

    wxFloat64 fInFloat;

    *pDataInput >> fInFloat;

    delete pDataInput;
    delete pFileInput;

    return fInFloat;
}

template <class T>
class TestMultiRW {
public:
    typedef std::vector<T> ValueArray;
    typedef void (wxDataOutputStream::*FnWriter)(const T *buffer, size_t size);
    typedef void (wxDataInputStream::*FnReader)(T *buffer, size_t size);

private:
    bool m_ok;

private:
    void ProcessData(const T *Values,
                     typename ValueArray::size_type Size,
                     FnWriter pfnWriter,
                     FnReader pfnReader)
    {
        ValueArray InValues(Size);

        {
            wxFileOutputStream FileOutput( wxT("mytext.dat") );
            wxDataOutputStream DataOutput( FileOutput );

            (DataOutput.*pfnWriter)(Values, Size);
        }

        {
            wxFileInputStream FileInput( wxT("mytext.dat") );
            wxDataInputStream DataInput( FileInput );

            (DataInput.*pfnReader)(&*InValues.begin(), InValues.size());
        }

        m_ok = true;
        for (typename ValueArray::size_type idx=0; idx!=Size; ++idx) {
            if (InValues[idx]!=Values[idx]) {
                m_ok = false;
                break;
            }
        }
    }


public:
    TestMultiRW(const T *Values,
                size_t Size,
                FnWriter pfnWriter,
                FnReader pfnReader)
    {
        ProcessData(Values, (typename ValueArray::size_type) Size, pfnWriter, pfnReader);
    }
    TestMultiRW(const ValueArray &Values,
                FnWriter pfnWriter,
                FnReader pfnReader)
    {
        ProcessData(&*Values.begin(), Values.size(), pfnWriter, pfnReader);
    }

    bool Ok(void) const {
        return m_ok;
    }
};

template <class T>
static
T TestRW(const T &Value)
{
    T InValue;

    {
        wxFileOutputStream FileOutput( wxT("mytext.dat") );
        wxDataOutputStream DataOutput( FileOutput );

        DataOutput << Value;
    }

    {
        wxFileInputStream FileInput( wxT("mytext.dat") );
        wxDataInputStream DataInput( FileInput );

        DataInput >> InValue;
    }

    return InValue;
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

#if wxUSE_LONGLONG
void DataStreamTestCase::LongLongRW()
{
    TestMultiRW<wxLongLong>::ValueArray ValuesLL;
    TestMultiRW<wxULongLong>::ValueArray ValuesULL;

    ValuesLL.push_back(wxLongLong(0l));
    ValuesLL.push_back(wxLongLong(1l));
    ValuesLL.push_back(wxLongLong(-1l));
    ValuesLL.push_back(wxLongLong(0x12345678l));
    ValuesLL.push_back(wxLongLong(0x12345678l, 0xabcdef01l));

    ValuesULL.push_back(wxULongLong(0l));
    ValuesULL.push_back(wxULongLong(1l));
    ValuesULL.push_back(wxULongLong(0x12345678l));
    ValuesULL.push_back(wxULongLong(0x12345678l, 0xabcdef01l));

    CPPUNIT_ASSERT( TestRW(wxLongLong(0x12345678l)) == wxLongLong(0x12345678l) );
    CPPUNIT_ASSERT( TestRW(wxLongLong(0x12345678l, 0xabcdef01l)) == wxLongLong(0x12345678l, 0xabcdef01l) );
    CPPUNIT_ASSERT( TestMultiRW<wxLongLong>(ValuesLL, &wxDataOutputStream::WriteLL, &wxDataInputStream::ReadLL).Ok() );
    CPPUNIT_ASSERT( TestMultiRW<wxULongLong>(ValuesULL, &wxDataOutputStream::WriteLL, &wxDataInputStream::ReadLL).Ok() );
}
#endif

#if wxHAS_INT64
void DataStreamTestCase::Int64RW()
{
    TestMultiRW<wxInt64>::ValueArray ValuesI64;
    TestMultiRW<wxUint64>::ValueArray ValuesUI64;

    ValuesI64.push_back(wxInt64(0l));
    ValuesI64.push_back(wxInt64(1l));
    ValuesI64.push_back(wxInt64(-1l));
    ValuesI64.push_back(wxInt64(0x12345678l));
    ValuesI64.push_back((wxInt64(0x12345678l) << 32) + wxInt64(0xabcdef01l));

    ValuesUI64.push_back(wxUint64(0l));
    ValuesUI64.push_back(wxUint64(1l));
    ValuesUI64.push_back(wxUint64(0x12345678l));
    ValuesUI64.push_back((wxUint64(0x12345678l) << 32) + wxUint64(0xabcdef01l));

    CPPUNIT_ASSERT( TestRW(wxUint64(0x12345678l)) == wxUint64(0x12345678l) );
    CPPUNIT_ASSERT( TestRW((wxUint64(0x12345678l) << 32) + wxUint64(0xabcdef01l)) == (wxUint64(0x12345678l) << 32) + wxUint64(0xabcdef01l) );
    CPPUNIT_ASSERT( TestMultiRW<wxInt64>(ValuesI64, &wxDataOutputStream::Write64, &wxDataInputStream::Read64).Ok() );
    CPPUNIT_ASSERT( TestMultiRW<wxUint64>(ValuesUI64, &wxDataOutputStream::Write64, &wxDataInputStream::Read64).Ok() );
}
#endif

void DataStreamTestCase::NaNRW()
{
    //TODO?
}


