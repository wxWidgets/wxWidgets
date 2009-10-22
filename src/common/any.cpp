/////////////////////////////////////////////////////////////////////////////
// Name:        src/common/any.cpp
// Purpose:     wxAny class, container for any type
// Author:      Jaakko Salli
// Modified by:
// Created:     07/05/2009
// RCS-ID:      $Id$
// Copyright:   (c) wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/any.h"

#if wxUSE_ANY

#ifndef WX_PRECOMP
    #include "wx/math.h"
    #include "wx/crt.h"
#endif

#include "wx/vector.h"
#include "wx/module.h"

using namespace wxPrivate;

//-------------------------------------------------------------------------
// wxAnyValueTypeGlobals
//-------------------------------------------------------------------------

//
// Helper class to manage wxAnyValueType instances and other
// related global variables.
//
// NB: We really need to have wxAnyValueType instances allocated
//     in heap. They are stored as static template member variables,
//     and with them we just can't be too careful (eg. not allocating
//     them in heap broke the type identification in GCC).
//
class wxAnyValueTypeGlobals
{
public:
    wxAnyValueTypeGlobals()
    {
    }
    ~wxAnyValueTypeGlobals()
    {
        for ( size_t i=0; i<m_valueTypes.size(); i++ )
            delete m_valueTypes[i];
    }

    void RegisterValueType(wxAnyValueType* valueType)
    {
        m_valueTypes.push_back(valueType);
    }

private:
    wxVector<wxAnyValueType*>   m_valueTypes;
};

static wxAnyValueTypeGlobals* g_wxAnyValueTypeGlobals = NULL;

//
// This class is to make sure that wxAnyValueType instances
// etc. get freed correctly. We must use a separate wxAnyValueTypeGlobals
// because wxModule itself is instantiated too late.
//
class wxAnyValueTypeGlobalsManager : public wxModule
{
    DECLARE_DYNAMIC_CLASS(wxAnyValueTypeGlobalsManager)
public:
    wxAnyValueTypeGlobalsManager() : wxModule() { }
    virtual ~wxAnyValueTypeGlobalsManager() { }

    virtual bool OnInit()
    {
        return true;
    }
    virtual void OnExit()
    {
        delete g_wxAnyValueTypeGlobals;
        g_wxAnyValueTypeGlobals = NULL;
    }
private:
};

IMPLEMENT_DYNAMIC_CLASS(wxAnyValueTypeGlobalsManager, wxModule)


//-------------------------------------------------------------------------
// wxAnyValueType
//-------------------------------------------------------------------------

wxAnyValueType::wxAnyValueType()
{
    if ( !g_wxAnyValueTypeGlobals )
        g_wxAnyValueTypeGlobals = new wxAnyValueTypeGlobals();

    g_wxAnyValueTypeGlobals->RegisterValueType(this);
}

//-------------------------------------------------------------------------
// Dynamic conversion member functions
//-------------------------------------------------------------------------

//
// Define integer minimum and maximum as helpers
#ifdef wxLongLong_t
    #define UseIntMin  (wxINT64_MIN)
    #define UseIntMax  (wxINT64_MAX)
    #define UseUintMax (wxUINT64_MAX)
#else
    #define UseIntMin  (LONG_MIN)
    #define UseIntMax  (LONG_MAX)
    #define UseUintMax (ULONG_MAX)
#endif

namespace
{

const double UseIntMinF = static_cast<double>(UseIntMin);
const double UseIntMaxF = static_cast<double>(UseIntMax);
const double UseUintMaxF = static_cast<double>(UseUintMax);

} // anonymous namespace

bool wxAnyValueTypeImplInt::ConvertValue(const wxAnyValueBuffer& src,
                                         wxAnyValueType* dstType,
                                         wxAnyValueBuffer& dst) const
{
    wxAnyBaseIntType value = GetValue(src);
    if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxString) )
    {
#ifdef wxLongLong_t
        wxLongLong ll(value);
        wxString s = ll.ToString();
#else
        wxString s = wxString::Format(wxS("%ld"), (long)value);
#endif
        wxAnyValueTypeImpl<wxString>::SetValue(s, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseUintType) )
    {
        if ( value < 0 )
            return false;
        wxAnyBaseUintType ul = (wxAnyBaseUintType) value;
        wxAnyValueTypeImplUint::SetValue(ul, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, double) )
    {
        double value2 = static_cast<double>(value);
        wxAnyValueTypeImplDouble::SetValue(value2, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, bool) )
    {
        bool value2 = value ? true : false;
        wxAnyValueTypeImpl<bool>::SetValue(value2, dst);
    }
    else
        return false;

    return true;
}

bool wxAnyValueTypeImplUint::ConvertValue(const wxAnyValueBuffer& src,
                                          wxAnyValueType* dstType,
                                          wxAnyValueBuffer& dst) const
{
    wxAnyBaseUintType value = GetValue(src);
    if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxString) )
    {
#ifdef wxLongLong_t
        wxULongLong ull(value);
        wxString s = ull.ToString();
#else
        wxString s = wxString::Format(wxS("%lu"), (long)value);
#endif
        wxAnyValueTypeImpl<wxString>::SetValue(s, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseIntType) )
    {
        if ( value > UseIntMax )
            return false;
        wxAnyBaseIntType l = (wxAnyBaseIntType) value;
        wxAnyValueTypeImplInt::SetValue(l, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, double) )
    {
#ifndef __VISUALC6__
        double value2 = static_cast<double>(value);
#else
        // VC6 doesn't implement conversion from unsigned __int64 to double
        wxAnyBaseIntType value0 = static_cast<wxAnyBaseIntType>(value);
        double value2 = static_cast<double>(value0);
#endif
        wxAnyValueTypeImplDouble::SetValue(value2, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, bool) )
    {
        bool value2 = value ? true : false;
        wxAnyValueTypeImpl<bool>::SetValue(value2, dst);
    }
    else
        return false;

    return true;
}

bool wxAnyValueTypeImplString::ConvertValue(const wxAnyValueBuffer& src,
                                            wxAnyValueType* dstType,
                                            wxAnyValueBuffer& dst) const
{
    wxString value = GetValue(src);
    if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseIntType) )
    {
        wxAnyBaseIntType value2;
#ifdef wxLongLong_t
        if ( !value.ToLongLong(&value2) )
#else
        if ( !value.ToLong(&value2) )
#endif
            return false;
        wxAnyValueTypeImplInt::SetValue(value2, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseUintType) )
    {
        wxAnyBaseUintType value2;
#ifdef wxLongLong_t
        if ( !value.ToULongLong(&value2) )
#else
        if ( !value.ToULong(&value2) )
#endif
            return false;
        wxAnyValueTypeImplUint::SetValue(value2, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, double) )
    {
        double value2;
        if ( !value.ToDouble(&value2) )
            return false;
        wxAnyValueTypeImplDouble::SetValue(value2, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, bool) )
    {
        bool value2;
        value.MakeLower();
        if ( value == wxS("true") ||
             value == wxS("yes") ||
             value == wxS('1') )
            value2 = true;
        else if ( value == wxS("false") ||
                  value == wxS("no") ||
                  value == wxS('0') )
            value2 = false;
        else
            return false;

        wxAnyValueTypeImpl<bool>::SetValue(value2, dst);
    }
    else
        return false;

    return true;
}

bool wxAnyValueTypeImpl<bool>::ConvertValue(const wxAnyValueBuffer& src,
                                            wxAnyValueType* dstType,
                                            wxAnyValueBuffer& dst) const
{
    bool value = GetValue(src);
    if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseIntType) )
    {
        wxAnyBaseIntType value2 = static_cast<wxAnyBaseIntType>(value);
        wxAnyValueTypeImplInt::SetValue(value2, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseUintType) )
    {
        wxAnyBaseIntType value2 = static_cast<wxAnyBaseUintType>(value);
        wxAnyValueTypeImplUint::SetValue(value2, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxString) )
    {
        wxString s;
        if ( value )
            s = wxS("true");
        else
            s = wxS("false");
        wxAnyValueTypeImpl<wxString>::SetValue(s, dst);
    }
    else
        return false;

    return true;
}

bool wxAnyValueTypeImplDouble::ConvertValue(const wxAnyValueBuffer& src,
                                            wxAnyValueType* dstType,
                                            wxAnyValueBuffer& dst) const
{
    double value = GetValue(src);
    if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseIntType) )
    {
        if ( value < UseIntMinF || value > UseIntMaxF )
            return false;
        wxAnyBaseUintType ul = static_cast<wxAnyBaseUintType>(value);
        wxAnyValueTypeImplUint::SetValue(ul, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxAnyBaseUintType) )
    {
        if ( value < 0.0 || value > UseUintMaxF )
            return false;
        wxAnyBaseUintType ul = static_cast<wxAnyBaseUintType>(value);
        wxAnyValueTypeImplUint::SetValue(ul, dst);
    }
    else if ( wxANY_VALUE_TYPE_CHECK_TYPE(dstType, wxString) )
    {
        wxString s = wxString::Format(wxS("%.14g"), value);
        wxAnyValueTypeImpl<wxString>::SetValue(s, dst);
    }
    else
        return false;

    return true;
}

WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImplInt)
WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImplUint)
WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImplString)
WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<bool>)
WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImplDouble)

WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<wxDateTime>)
//WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<wxObject*>)
//WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<wxArrayString>)

//-------------------------------------------------------------------------
// wxAnyNullValueType implementation
//-------------------------------------------------------------------------

class wxAnyNullValue
{
private:
    void*   m_dummy;
};

template <>
class wxAnyValueTypeImpl<wxAnyNullValue> : public wxAnyValueType
{
    WX_DECLARE_ANY_VALUE_TYPE(wxAnyValueTypeImpl<wxAnyNullValue>)
public:
    // Dummy implementations
    virtual void DeleteValue(wxAnyValueBuffer& buf) const
    {
        wxUnusedVar(buf);
    }

    virtual void CopyBuffer(const wxAnyValueBuffer& src,
                            wxAnyValueBuffer& dst) const
    {
        wxUnusedVar(src);
        wxUnusedVar(dst);
    }

    virtual bool ConvertValue(const wxAnyValueBuffer& src,
                              wxAnyValueType* dstType,
                              wxAnyValueBuffer& dst) const
    {
        wxUnusedVar(src);
        wxUnusedVar(dstType);
        wxUnusedVar(dst);
        return false;
    }

private:
};

WX_IMPLEMENT_ANY_VALUE_TYPE(wxAnyValueTypeImpl<wxAnyNullValue>)

wxAnyValueType* wxAnyNullValueType =
    wxAnyValueTypeImpl<wxAnyNullValue>::GetInstance();

#endif // wxUSE_ANY
