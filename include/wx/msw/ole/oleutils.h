///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/ole/oleutils.h
// Purpose:     OLE helper routines, OLE debugging support &c
// Author:      Vadim Zeitlin
// Created:     19.02.1998
// Copyright:   (c) 1998 Vadim Zeitlin <zeitlin@dptmaths.ens-cachan.fr>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef   _WX_OLEUTILS_H
#define   _WX_OLEUTILS_H

#include "wx/defs.h"

#if wxUSE_OLE

// ole2.h includes windows.h, so include wrapwin.h first
#include "wx/msw/wrapwin.h"
// get IUnknown, REFIID &c
#include <ole2.h>
#include "wx/intl.h"
#include "wx/log.h"
#include "wx/variant.h"

#include "wx/msw/ole/comimpl.h"

// ============================================================================
// General purpose functions and macros
// ============================================================================

// ----------------------------------------------------------------------------
// initialize/cleanup OLE
// ----------------------------------------------------------------------------

// Simple wrapper for OleInitialize().
//
// Avoid using it directly, use wxOleInitializer instead.
//
// return true if ok, false otherwise
inline bool wxOleInitialize()
{
    const HRESULT
    hr = ::OleInitialize(nullptr);

    // RPC_E_CHANGED_MODE indicates that OLE had been already initialized
    // before, albeit with different mode. Don't consider it to be an error as
    // we don't actually care ourselves about the mode used so this allows the
    // main application to call OleInitialize() on its own before we do if it
    // needs non-default mode.
    if ( hr != RPC_E_CHANGED_MODE && FAILED(hr) )
    {
        wxLogError(wxGetTranslation("Cannot initialize OLE"));

        return false;
    }

    return true;
}

inline void wxOleUninitialize()
{
    ::OleUninitialize();
}

// wrapper around BSTR type (by Vadim Zeitlin)

class WXDLLIMPEXP_CORE wxBasicString
{
public:
    // Constructs with the owned BSTR set to nullptr
    wxBasicString() : m_bstrBuf(nullptr) {}

    // Constructs with the owned BSTR created from a wxString
    wxBasicString(const wxString& str)
        : m_bstrBuf(SysAllocString(str.wc_str())) {}

    // Constructs with the owned BSTR as a copy of the BSTR owned by bstr
    wxBasicString(const wxBasicString& bstr) : m_bstrBuf(bstr.Copy()) {}

    // Frees the owned BSTR
    ~wxBasicString() { SysFreeString(m_bstrBuf); }

    // Creates the owned BSTR from a wxString
    void AssignFromString(const wxString& str);

    // Returns the owned BSTR and gives up its ownership,
    // the caller is responsible for freeing it
    wxNODISCARD BSTR Detach();

    // Returns a copy of the owned BSTR,
    // the caller is responsible for freeing it
    wxNODISCARD BSTR Copy() const { return SysAllocString(m_bstrBuf); }

    // Returns the address of the owned BSTR, not to be called
    // when wxBasicString already contains a non-null BSTR
    BSTR* ByRef();

    // Sets its BSTR to a copy of the BSTR owned by bstr
    wxBasicString& operator=(const wxBasicString& bstr);

    /// Returns the owned BSTR while keeping its ownership
    operator BSTR() const { return m_bstrBuf; }

    // retrieve a copy of our string - caller must SysFreeString() it later!
    wxDEPRECATED_MSG("use Copy() instead")
    BSTR Get() const { return Copy(); }
private:
    // actual string
    BSTR m_bstrBuf;
};

#if wxUSE_VARIANT
// Convert variants
class WXDLLIMPEXP_FWD_BASE wxVariant;

// wrapper for CURRENCY type used in VARIANT (VARIANT.vt == VT_CY)
class WXDLLIMPEXP_CORE wxVariantDataCurrency : public wxVariantData
{
public:
    wxVariantDataCurrency() { VarCyFromR8(0.0, &m_value); }
    wxVariantDataCurrency(CURRENCY value) { m_value = value; }

    CURRENCY GetValue() const { return m_value; }
    void SetValue(CURRENCY value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const override;

#if wxUSE_STD_IOSTREAM
    virtual bool Write(std::ostream& str) const override;
#endif
    virtual bool Write(wxString& str) const override;

    wxNODISCARD wxVariantData* Clone() const override { return new wxVariantDataCurrency(m_value); }
    virtual wxString GetType() const override { return wxS("currency"); }

    DECLARE_WXANY_CONVERSION()

private:
    CURRENCY m_value;
};


// wrapper for SCODE type used in VARIANT (VARIANT.vt == VT_ERROR)
class WXDLLIMPEXP_CORE wxVariantDataErrorCode : public wxVariantData
{
public:
    wxVariantDataErrorCode(SCODE value = S_OK) { m_value = value; }

    SCODE GetValue() const { return m_value; }
    void SetValue(SCODE value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const override;

#if wxUSE_STD_IOSTREAM
    virtual bool Write(std::ostream& str) const override;
#endif
    virtual bool Write(wxString& str) const override;

    wxNODISCARD wxVariantData* Clone() const override { return new wxVariantDataErrorCode(m_value); }
    virtual wxString GetType() const override { return wxS("errorcode"); }

    DECLARE_WXANY_CONVERSION()

private:
    SCODE m_value;
};

// wrapper for SAFEARRAY, used for passing multidimensional arrays in wxVariant
class WXDLLIMPEXP_CORE wxVariantDataSafeArray : public wxVariantData
{
public:
    explicit wxVariantDataSafeArray(SAFEARRAY* value = nullptr)
    {
        m_value = value;
    }

    SAFEARRAY* GetValue() const { return m_value; }
    void SetValue(SAFEARRAY* value) { m_value = value; }

    virtual bool Eq(wxVariantData& data) const override;

#if wxUSE_STD_IOSTREAM
    virtual bool Write(std::ostream& str) const override;
#endif
    virtual bool Write(wxString& str) const override;

    wxNODISCARD wxVariantData* Clone() const override { return new wxVariantDataSafeArray(m_value); }
    virtual wxString GetType() const override { return wxS("safearray"); }

    DECLARE_WXANY_CONVERSION()

private:
    SAFEARRAY* m_value;
};

// Used by wxAutomationObject for its wxConvertOleToVariant() calls.
enum wxOleConvertVariantFlags
{
    wxOleConvertVariant_Default = 0,

    // If wxOleConvertVariant_ReturnSafeArrays  flag is set, SAFEARRAYs
    // contained in OLE VARIANTs will be returned as wxVariants
    // with wxVariantDataSafeArray type instead of wxVariants
    // with the list type containing the (flattened) SAFEARRAY's elements.
    wxOleConvertVariant_ReturnSafeArrays = 1
};

WXDLLIMPEXP_CORE
bool wxConvertVariantToOle(const wxVariant& variant, VARIANTARG& oleVariant);

WXDLLIMPEXP_CORE
bool wxConvertOleToVariant(const VARIANTARG& oleVariant, wxVariant& variant,
                           long flags = wxOleConvertVariant_Default);

#endif // wxUSE_VARIANT

// Convert string to Unicode
wxNODISCARD WXDLLIMPEXP_CORE BSTR wxConvertStringToOle(const wxString& str);

// Convert string from BSTR to wxString
WXDLLIMPEXP_CORE wxString wxConvertStringFromOle(BSTR bStr);

#else // !wxUSE_OLE

// ----------------------------------------------------------------------------
// stub functions to avoid #if wxUSE_OLE in the main code
// ----------------------------------------------------------------------------

inline bool wxOleInitialize() { return false; }
inline void wxOleUninitialize() { }

#endif // wxUSE_OLE/!wxUSE_OLE

// RAII class initializing OLE in its ctor and undoing it in its dtor.
class wxOleInitializer
{
public:
    wxOleInitializer()
        : m_ok(wxOleInitialize())
    {
    }

    bool IsOk() const
    {
        return m_ok;
    }

    ~wxOleInitializer()
    {
        if ( m_ok )
            wxOleUninitialize();
    }

private:
    const bool m_ok;

    wxDECLARE_NO_COPY_CLASS(wxOleInitializer);
};

#endif  //_WX_OLEUTILS_H
