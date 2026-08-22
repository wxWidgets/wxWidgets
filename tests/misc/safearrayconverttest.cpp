///////////////////////////////////////////////////////////////////////////////
// Name:        tests/misc/safearrayconverttest.cpp
// Purpose:     Test conversions between wxVariant and OLE VARIANT using SAFEARRAYs
// Author:      PB
// Copyright:   (c) the wxWidgets team
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#include "testprec.h"


#ifdef __WINDOWS__

#if wxUSE_OLE && wxUSE_VARIANT

#include "wx/msw/ole/oleutils.h"
#include "wx/msw/ole/safearray.h"

// need this to be able to compare wxVariant objects in Catch macros
inline std::ostream& operator<<(std::ostream& ostr, const wxVariant& v)
{
    ostr << v.GetString();
    return ostr;
}

// ----------------------------------------------------------------------------
// tests
// ----------------------------------------------------------------------------

// test converting a wxVariant with the list type to an OLE VARIANT
// and back to wxVariant the list type
TEST_CASE("SafeArrayConvert::VariantListDefault", "[safearray][msw]")
{
    wxVariant variant;
    VARIANT oleVariant;

    variant.NullList();
    variant.Append(true);
    variant.Append(12.34);
    variant.Append(42L);
    variant.Append("ABC");
    CHECK( wxConvertVariantToOle(variant, oleVariant) );

    wxVariant variantCopy;

    CHECK( wxConvertOleToVariant(oleVariant, variantCopy) );
    CHECK( variant == variantCopy );
}

// test converting a wxVariant with the arrstring type to an OLE VARIANT
// and back to a wxVariant with the arrstring type
TEST_CASE("SafeArrayConvert::VariantStringsDefault", "[safearray][msw]")
{
    wxVariant variant;
    wxArrayString as;
    VARIANT oleVariant;

    as.push_back("abc");
    as.push_back("def");
    as.push_back("ghi");
    variant = as;
    CHECK( wxConvertVariantToOle(variant, oleVariant) );

    wxVariant variantCopy;

    CHECK( wxConvertOleToVariant(oleVariant, variantCopy) );
    CHECK( variant == variantCopy );
}

// test converting a wxVariant with the list type to an OLE VARIANT
// and then to a wxVariant with the safearray type
TEST_CASE("SafeArrayConvert::VariantListReturnSafeArray", "[safearray][msw]")
{
    wxVariant variant;
    VARIANT oleVariant;

    variant.NullList();
    variant.Append(true);
    variant.Append(12.34);
    variant.Append(42L);
    variant.Append("test");
    CHECK( wxConvertVariantToOle(variant, oleVariant) );

    wxVariant variantCopy;

    CHECK(wxConvertOleToVariant(oleVariant, variantCopy,
                              wxOleConvertVariant_ReturnSafeArrays) );
    CHECK( variantCopy.GetType() == wxT("safearray") );

    wxSafeArray<VT_VARIANT> safeArray;
    wxVariantDataSafeArray*
        vsa = wxStaticCastVariantData(variantCopy.GetData(),
                                      wxVariantDataSafeArray);
    long bound wxDUMMY_INITIALIZE(0);

    CHECK( vsa );
    CHECK( safeArray.Attach(vsa->GetValue()) );
    CHECK( safeArray.GetDim() == 1 );
    CHECK( safeArray.GetLBound(1, bound) );
    CHECK( bound == 0 );
    CHECK( safeArray.GetUBound(1, bound) );

    const long count = variant.GetCount();

    // bound + 1 because safearray elements are accessed by index ranging from
    // LBound to UBound inclusive
    CHECK( count == bound + 1 );

    wxVariant variantItem;

    for ( long i = 0; i < count; i++ )
    {
        CHECK( safeArray.GetElement(&i, variantItem) );
        CHECK( variant[i] == variantItem );
    }
}

// test converting a wxArrayString to an OLE VARIANT
// and then to a wxVariant with the safearray type
TEST_CASE("SafeArrayConvert::StringsReturnSafeArray", "[safearray][msw]")
{
    wxArrayString as;
    wxSafeArray<VT_BSTR> safeArray;

    as.push_back("abc");
    as.push_back("def");
    as.push_back("ghi");
    CHECK( safeArray.CreateFromArrayString(as) );

    VARIANT oleVariant;
    wxVariant variant;

    oleVariant.vt = VT_BSTR | VT_ARRAY;
    oleVariant.parray = safeArray.Detach();
    CHECK( oleVariant.parray );
    CHECK(wxConvertOleToVariant(oleVariant, variant,
                              wxOleConvertVariant_ReturnSafeArrays) );
    CHECK( variant.GetType() == wxT("safearray") );

    wxVariantDataSafeArray*
        vsa = wxStaticCastVariantData(variant.GetData(),
                                      wxVariantDataSafeArray);
    long bound wxDUMMY_INITIALIZE(0);

    CHECK( vsa );
    CHECK( safeArray.Attach(vsa->GetValue()) );
    CHECK( safeArray.GetDim() == 1 );
    CHECK( safeArray.GetLBound(1, bound) );
    CHECK( bound == 0 );
    CHECK( safeArray.GetUBound(1, bound) );

    const long count = as.size();
    CHECK( count == bound + 1 );

    wxString str;

    for ( long i = 0; i < count; i++ )
    {
        CHECK( safeArray.GetElement(&i, str) );
        CHECK( str == as[i] );
    }
}

#endif // __WINDOWS__

#endif // wxUSE_OLE && wxUSE_VARIANT
