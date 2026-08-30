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
#include "wx/mstream.h"
#include "wx/math.h"

#include "testfile.h"

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

// Write the given value to a stream using the given format and read it back.
static wxFloat64 TestFloatRW(wxFloat64 fValue, bool bigEndian, bool ieee754)
{
    TempFile f("mytext.dat");

    {
        wxFileOutputStream pFileOutput( f.GetName() );
        wxDataOutputStream pDataOutput( pFileOutput );
        if ( bigEndian )
            pDataOutput.BigEndianOrdered(true);

#if wxUSE_APPLE_IEEE
        if ( ieee754 )
            pDataOutput.UseBasicPrecisions();
#endif // wxUSE_APPLE_IEEE

        pDataOutput << fValue;
    }

    wxFileInputStream pFileInput( f.GetName() );
    wxDataInputStream pDataInput( pFileInput );
    if ( bigEndian )
        pDataInput.BigEndianOrdered(true);

#if wxUSE_APPLE_IEEE
    if ( ieee754 )
        pDataInput.UseBasicPrecisions();
#else // !wxUSE_APPLE_IEEE
    wxUnusedVar(ieee754);
#endif // wxUSE_APPLE_IEEE/!wxUSE_APPLE_IEEE

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

TEST_CASE("DataStream::FloatRW", "[datastream][stream]")
{
    // Run the test for all the combinations of the possible formats.
    const bool bigEndian = GENERATE(false, true);
#if wxUSE_APPLE_IEEE
    // The standard IEEE 754 formats are only worth testing separately if the
    // extended precision format is used by default.
    const bool ieee754 = GENERATE(false, true);
#else // !wxUSE_APPLE_IEEE
    const bool ieee754 = false;
#endif // wxUSE_APPLE_IEEE/!wxUSE_APPLE_IEEE
    CAPTURE(bigEndian, ieee754);

    CHECK( TestFloatRW(5.5, bigEndian, ieee754) == 5.5 );
    CHECK( TestFloatRW(5, bigEndian, ieee754) == 5 );
    CHECK( TestFloatRW(5.55, bigEndian, ieee754) == 5.55 );
    CHECK( TestFloatRW(55555.555555, bigEndian, ieee754) == 55555.555555 );
}

TEST_CASE("DataStream::DoubleRW", "[datastream][stream]")
{
    // Run the test for all the combinations of the possible formats.
    const bool bigEndian = GENERATE(false, true);
#if wxUSE_APPLE_IEEE
    // The standard IEEE 754 formats are only worth testing separately if the
    // extended precision format is used by default.
    const bool ieee754 = GENERATE(false, true);
#else // !wxUSE_APPLE_IEEE
    const bool ieee754 = false;
#endif // wxUSE_APPLE_IEEE/!wxUSE_APPLE_IEEE
    CAPTURE(bigEndian, ieee754);

    CHECK( TestFloatRW(2132131.1232132, bigEndian, ieee754) == 2132131.1232132 );
    CHECK( TestFloatRW(21321343431.1232143432, bigEndian, ieee754)
            == 21321343431.1232143432 );
}

TEST_CASE("DataStream::StringRW", "[datastream][stream]")
{
    wxString s(wxT("Test1"));
    CHECK( s == TestRW(s) );

    s.append(2, wxT('\0'));
    s.append(wxT("Test2"));
    CHECK( s == TestRW(s) );

    s = wxString::FromUTF8("ü");
    CHECK( s == TestRW(s) );
}

TEST_CASE("DataStream::ReadTruncatedString", "[datastream][stream]")
{
    // A string is stored as a 32 bit length followed by that many bytes. If the
    // length is larger than the number of bytes actually present (a corrupt or
    // malicious stream), ReadString() must not decode the uninitialised tail of
    // its temporary buffer but return an empty string and put the stream into
    // an error state.
    const unsigned char data[] =
    {
        0x04, 0x00, 0x00, 0x00,     // little endian length: claims 4 bytes
        'H', 'i'                    // but only 2 bytes follow
    };

    wxMemoryInputStream input(data, sizeof(data));
    wxDataInputStream dis(input);

    CHECK( dis.ReadString() == wxString() );
    CHECK( !dis.IsOk() );
}

TEST_CASE("DataStream::ReadTruncatedValue", "[datastream][stream]")
{
    // Reading a fixed size value from a truncated stream must also fail instead
    // of returning a value built from uninitialised memory.
    const unsigned char data[] = { 0x12, 0x34 };  // only 2 of the 4 bytes

    wxMemoryInputStream input(data, sizeof(data));
    wxDataInputStream dis(input);

    CHECK( dis.Read32() == 0u );
    CHECK( !dis.IsOk() );
}

TEST_CASE("DataStream::LongLongRW", "[datastream][stream]")
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

    CHECK( TestRW(wxLongLong(0x12345678l)) == wxLongLong(0x12345678l) );
    CHECK( TestRW(wxLongLong(0x12345678l, 0xabcdef01l)) == wxLongLong(0x12345678l, 0xabcdef01l) );
    CHECK( TestMultiRW<wxLongLong>(ValuesLL, &wxDataOutputStream::WriteLL, &wxDataInputStream::ReadLL).IsOk() );
    CHECK( TestMultiRW<wxULongLong>(ValuesULL, &wxDataOutputStream::WriteLL, &wxDataInputStream::ReadLL).IsOk() );
}

TEST_CASE("DataStream::Int64RW", "[datastream][stream]")
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

    CHECK( TestRW(wxUint64(0x12345678l)) == wxUint64(0x12345678l) );
    CHECK( TestRW((wxUint64(0x12345678l) << 32) + wxUint64(0xabcdef01l)) == (wxUint64(0x12345678l) << 32) + wxUint64(0xabcdef01l) );
    CHECK( TestMultiRW<wxInt64>(ValuesI64, &wxDataOutputStream::Write64, &wxDataInputStream::Read64).IsOk() );
    CHECK( TestMultiRW<wxUint64>(ValuesUI64, &wxDataOutputStream::Write64, &wxDataInputStream::Read64).IsOk() );
}

TEST_CASE("DataStream::NaNRW", "[datastream][stream]")
{
    //TODO?
}

