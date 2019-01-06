///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/safearrayconverttest.cpp
// Purpose:     Test conversions between wxVariant and OLE VARIANT using SAFEARRAYs
// Author:      PB
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#ifdef __WINDOWS__

#if wxUSE_OLE && wxUSE_VARIANT

#include "wx/msw/ole/oleutils.h"
#include "wx/msw/ole/safearray.h"

// need this to be able to use CPPUNIT_ASSERT_EQUAL with wxVariant objects
inline std::ostream& operator<<(std::ostream& ostr, const wxVariant& v)
{
    ostr << v.GetString();
    return ostr;
}

// ----------------------------------------------------------------------------
// test class
// ----------------------------------------------------------------------------

class SafeArrayConvertTestCase  : public CppUnit::TestCase
{
public:
    SafeArrayConvertTestCase () { }

private:
     CPPUNIT_TEST_SUITE( SafeArrayConvertTestCase  );
        CPPUNIT_TEST( VariantListDefault );
        CPPUNIT_TEST( VariantStringsDefault );
        CPPUNIT_TEST( VariantListReturnSafeArray );
        CPPUNIT_TEST( StringsReturnSafeArray );
     CPPUNIT_TEST_SUITE_END();

    void VariantListDefault();
    void VariantStringsDefault();

    void VariantListReturnSafeArray();
    void StringsReturnSafeArray();

    wxDECLARE_NO_COPY_CLASS(SafeArrayConvertTestCase);
};

// register in the unnamed registry so that these tests are run by default
CPPUNIT_TEST_SUITE_REGISTRATION( SafeArrayConvertTestCase  );

// also include in its own registry so that these tests can be run alone
CPPUNIT_TEST_SUITE_NAMED_REGISTRATION( SafeArrayConvertTestCase, "SafeArrayConvertTestCase" );



// test converting a wxVariant with the list type to an OLE VARIANT
// and back to wxVariant the list type
void SafeArrayConvertTestCase::VariantListDefault()
{
    wxVariant variant;
    VARIANT oleVariant;

    variant.NullList();
    variant.Append(true);
    variant.Append(12.34);
    variant.Append(42L);
    variant.Append("ABC");
    CPPUNIT_ASSERT( wxConvertVariantToOle(variant, oleVariant) );

    wxVariant variantCopy;

    CPPUNIT_ASSERT( wxConvertOleToVariant(oleVariant, variantCopy) );
    CPPUNIT_ASSERT( variant == variantCopy );
}

// test converting a wxVariant with the arrstring type to an OLE VARIANT
// and back to a wxVariant with the arrstring type
void SafeArrayConvertTestCase::VariantStringsDefault()
{
    wxVariant variant;
    wxArrayString as;
    VARIANT oleVariant;

    as.push_back("abc");
    as.push_back("def");
    as.push_back("ghi");
    variant = as;
    CPPUNIT_ASSERT( wxConvertVariantToOle(variant, oleVariant) );

    wxVariant variantCopy;

    CPPUNIT_ASSERT( wxConvertOleToVariant(oleVariant, variantCopy) );
    CPPUNIT_ASSERT( variant == variantCopy );
}

// test converting a wxVariant with the list type to an OLE VARIANT
// and then to a wxVariant with the safearray type
void SafeArrayConvertTestCase::VariantListReturnSafeArray()
{
    wxVariant variant;
    VARIANT oleVariant;

    variant.NullList();
    variant.Append(true);
    variant.Append(12.34);
    variant.Append(42L);
    variant.Append("test");
    CPPUNIT_ASSERT( wxConvertVariantToOle(variant, oleVariant) );

    wxVariant variantCopy;

    CPPUNIT_ASSERT(
        wxConvertOleToVariant(oleVariant, variantCopy,
                              wxOleConvertVariant_ReturnSafeArrays)
    );
    CPPUNIT_ASSERT( variantCopy.GetType() == wxT("safearray") );

    wxSafeArray<VT_VARIANT> safeArray;
    wxVariantDataSafeArray*
        vsa = wxStaticCastVariantData(variantCopy.GetData(),
                                      wxVariantDataSafeArray);
    long bound wxDUMMY_INITIALIZE(0);

    CPPUNIT_ASSERT( vsa );
    CPPUNIT_ASSERT( safeArray.Attach(vsa->GetValue()) );
    CPPUNIT_ASSERT_EQUAL( 1, safeArray.GetDim() );
    CPPUNIT_ASSERT( safeArray.GetLBound(1, bound) );
    CPPUNIT_ASSERT_EQUAL( 0, bound );
    CPPUNIT_ASSERT( safeArray.GetUBound(1, bound) );

    const long count = variant.GetCount();

    // bound + 1 because safearray elements are accessed by index ranging from
    // LBound to UBound inclusive
    CPPUNIT_ASSERT_EQUAL( bound + 1, count );

    wxVariant variantItem;

    for ( long i = 0; i < count; i++ )
    {
        CPPUNIT_ASSERT( safeArray.GetElement(&i, variantItem) );
        CPPUNIT_ASSERT_EQUAL( variantItem, variant[i] );
    }
}

// test converting a wxArrayString to an OLE VARIANT
// and then to a wxVariant with the safearray type
void SafeArrayConvertTestCase::StringsReturnSafeArray()
{
    wxArrayString as;
    wxSafeArray<VT_BSTR> safeArray;

    as.push_back("abc");
    as.push_back("def");
    as.push_back("ghi");
    CPPUNIT_ASSERT( safeArray.CreateFromArrayString(as) );

    VARIANT oleVariant;
    wxVariant variant;

    oleVariant.vt = VT_BSTR | VT_ARRAY;
    oleVariant.parray = safeArray.Detach();
    CPPUNIT_ASSERT( oleVariant.parray );
    CPPUNIT_ASSERT(
        wxConvertOleToVariant(oleVariant, variant,
                              wxOleConvertVariant_ReturnSafeArrays)
    );
    CPPUNIT_ASSERT( variant.GetType() == wxT("safearray") );

    wxVariantDataSafeArray*
        vsa = wxStaticCastVariantData(variant.GetData(),
                                      wxVariantDataSafeArray);
    long bound wxDUMMY_INITIALIZE(0);

    CPPUNIT_ASSERT( vsa );
    CPPUNIT_ASSERT( safeArray.Attach(vsa->GetValue()) );
    CPPUNIT_ASSERT_EQUAL( 1, safeArray.GetDim() );
    CPPUNIT_ASSERT( safeArray.GetLBound(1, bound) );
    CPPUNIT_ASSERT_EQUAL( 0, bound );
    CPPUNIT_ASSERT( safeArray.GetUBound(1, bound) );

    const long count = as.size();
    CPPUNIT_ASSERT_EQUAL( bound + 1, count );

    wxString str;

    for ( long i = 0; i < count; i++ )
    {
        CPPUNIT_ASSERT( safeArray.GetElement(&i, str) );
        CPPUNIT_ASSERT( str == as[i] );
    }
}

#endif // __WINDOWS__

#endif // wxUSE_OLE && wxUSE_VARIANT
