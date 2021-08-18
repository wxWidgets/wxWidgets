///////////////////////////////////////////////////////////////////////////////
// Name:        tests/streams/datastreamtest.cpp
// Purpose:     wxDataXXXStream Unit Test
// Author:      Ryan Norton
// Created:     2004-08-14
// Copyright:   (c) 2004 Ryan Norton
///////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include "testprec.h"


#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif // WX_PRECOMP

#include <vector>

#include "wx/datstrm.h"
#include "wx/wfstream.h"
#include "wx/math.h"

#include "testfile.h"

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
        CPPUNIT_TEST( StringRW );
#if wxUSE_LONGLONG
        CPPUNIT_TEST( LongLongRW );
#endif
#if wxHAS_INT64
        CPPUNIT_TEST( Int64RW );
#endif
        CPPUNIT_TEST( NaNRW );
        CPPUNIT_TEST( PseudoTest_UseBigEndian );
        CPPUNIT_TEST( FloatRW );
        CPPUNIT_TEST( DoubleRW );
        // Only test standard IEEE 754 formats if we're using IEEE extended
        // format by default, otherwise the tests above already covered them.
#if wxUSE_APPLE_IEEE
        CPPUNIT_TEST( PseudoTest_UseIEEE754 );
        CPPUNIT_TEST( FloatRW );
        CPPUNIT_TEST( DoubleRW );
        // Also retest little endian version with standard formats.
        CPPUNIT_TEST( PseudoTest_UseLittleEndian );
        CPPUNIT_TEST( FloatRW );
        CPPUNIT_TEST( DoubleRW );
#endif // wxUSE_APPLE_IEEE
    CPPUNIT_TEST_SUITE_END();

    wxFloat64 TestFloatRW(wxFloat64 fValue);

    void FloatRW();
    void DoubleRW();
    void StringRW();
#if wxUSE_LONGLONG
    void LongLongRW();
#endif
#if wxHAS_INT64
    void Int64RW();
#endif
    void NaNRW();

    void PseudoTest_UseBigEndian() { ms_useBigEndianFormat = true; }
    void PseudoTest_UseLittleEndian() { ms_useBigEndianFormat = false; }
#if wxUSE_APPLE_IEEE
    void PseudoTest_UseIEEE754() { ms_useIEEE754 = true; }
#endif // wxUSE_APPLE_IEEE

    static bool ms_useBigEndianFormat;
#if wxUSE_APPLE_IEEE
    static bool ms_useIEEE754;
#endif // wxUSE_APPLE_IEEE

    wxDECLARE_NO_COPY_CLASS(DataStreamTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( DataStreamTestCase );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( DataStreamTestCase, "DataStreamTestCase" );

bool DataStreamTestCase::ms_useBigEndianFormat = false;
#if wxUSE_APPLE_IEEE
bool DataStreamTestCase::ms_useIEEE754 = false;
#endif // wxUSE_APPLE_IEEE

DataStreamTestCase::DataStreamTestCase()
{
}

wxFloat64 DataStreamTestCase::TestFloatRW(wxFloat64 fValue)
{
    TempFile f("mytext.dat");

    {
        wxFileOutputStream pFileOutput( f.GetName() );
        wxDataOutputStream pDataOutput( pFileOutput );
        if ( ms_useBigEndianFormat )
            pDataOutput.BigEndianOrdered(true);

#if wxUSE_APPLE_IEEE
        if ( ms_useIEEE754 )
            pDataOutput.UseBasicPrecisions();
#endif // wxUSE_APPLE_IEEE

        pDataOutput << fValue;
    }

    wxFileInputStream pFileInput( f.GetName() );
    wxDataInputStream pDataInput( pFileInput );
    if ( ms_useBigEndianFormat )
        pDataInput.BigEndianOrdered(true);

#if wxUSE_APPLE_IEEE
    if ( ms_useIEEE754 )
        pDataInput.UseBasicPrecisions();
#endif // wxUSE_APPLE_IEEE

    wxFloat64 fInFloat;

    pDataInput >> fInFloat;

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

        TempFile f("mytext.dat");

        {
            wxFileOutputStream FileOutput( f.GetName() );
            wxDataOutputStream DataOutput( FileOutput );

            (DataOutput.*pfnWriter)(Values, Size);
        }

        {
            wxFileInputStream FileInput( f.GetName() );
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

    bool IsOk() const
    {
        return m_ok;
    }
};

template <class T>
static
T TestRW(const T &Value)
{
    T InValue;

    TempFile f("mytext.dat");

    {
        wxFileOutputStream FileOutput( f.GetName() );
        wxDataOutputStream DataOutput( FileOutput );

        DataOutput << Value;
    }

    {
        wxFileInputStream FileInput( f.GetName() );
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

void DataStreamTestCase::StringRW()
{
    wxString s(wxT("Test1"));
    CPPUNIT_ASSERT_EQUAL( TestRW(s), s );

#if wxUSE_UNICODE
    s.append(2, wxT('\0'));
    s.append(wxT("Test2"));
    CPPUNIT_ASSERT_EQUAL( TestRW(s), s );
#endif // wxUSE_UNICODE

    s = wxString::FromUTF8("\xc3\xbc"); // U+00FC LATIN SMALL LETTER U WITH DIAERESIS
    CPPUNIT_ASSERT_EQUAL( TestRW(s), s );
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
    CPPUNIT_ASSERT( TestMultiRW<wxLongLong>(ValuesLL, &wxDataOutputStream::WriteLL, &wxDataInputStream::ReadLL).IsOk() );
    CPPUNIT_ASSERT( TestMultiRW<wxULongLong>(ValuesULL, &wxDataOutputStream::WriteLL, &wxDataInputStream::ReadLL).IsOk() );
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
    CPPUNIT_ASSERT( TestMultiRW<wxInt64>(ValuesI64, &wxDataOutputStream::Write64, &wxDataInputStream::Read64).IsOk() );
    CPPUNIT_ASSERT( TestMultiRW<wxUint64>(ValuesUI64, &wxDataOutputStream::Write64, &wxDataInputStream::Read64).IsOk() );
}
#endif

void DataStreamTestCase::NaNRW()
{
    //TODO?
}


