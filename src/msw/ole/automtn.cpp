/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/ole/automtn.cpp
// Purpose:     OLE automation utilities
// Author:      Julian Smart
// Created:     11/6/98
// Copyright:   (c) 1998, Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"


#ifndef WX_PRECOMP
    #include "wx/log.h"
    #include "wx/math.h"
#endif

#ifndef _FORCENAMELESSUNION
    #define _FORCENAMELESSUNION
#endif

#include "wx/msw/private.h"
#include "wx/msw/ole/oleutils.h"
#include "wx/msw/ole/automtn.h"

#include <time.h>

#include <wtypes.h>
#include <unknwn.h>

#include <ole2.h>
#define _huge

#include <ole2ver.h>

#include <oleauto.h>

#if wxUSE_DATETIME
#include "wx/datetime.h"
#endif // wxUSE_DATETIME

#if wxUSE_OLE_AUTOMATION

#include <wx/vector.h>

// Report an OLE error when calling the specified method to the user via wxLog.
static void
ShowException(const wxString& member,
              HRESULT hr,
              EXCEPINFO *pexcep = nullptr,
              unsigned int uiArgErr = 0);

// wxAutomationObject

wxAutomationObject::wxAutomationObject(WXIDISPATCH* dispatchPtr)
{
    m_dispatchPtr = dispatchPtr;
    m_lcid = LOCALE_SYSTEM_DEFAULT;
    m_convertVariantFlags = wxOleConvertVariant_Default;
}

wxAutomationObject::~wxAutomationObject()
{
    if (m_dispatchPtr)
    {
        ((IDispatch*)m_dispatchPtr)->Release();
        m_dispatchPtr = nullptr;
    }
}

namespace
{

// A simple helper that ensures that VARIANT is destroyed on scope exit.
struct wxOleVariantArg : VARIANTARG
{
    wxOleVariantArg()  { VariantInit(this);  }
    ~wxOleVariantArg() { VariantClear(this); }
};

} // anonymous namespace


#define INVOKEARG(i) (args ? args[i] : *(ptrArgs[i]))

// Arguments with a non-empty name are passed as named arguments, the others
// are passed by position and must precede all the named ones. When putting a
// property, the last argument is the new value and its name, if any, is
// ignored.
// WARNING: if args contain IDispatches, their reference count will be decreased
// by one after Invoke() returns!
bool wxAutomationObject::Invoke(const wxString& member, int action,
        wxVariant& retValue, int noArgs, wxVariant args[], const wxVariant* ptrArgs[]) const
{
    if (!m_dispatchPtr)
        return false;

    int ch = member.Find('.');
    if (ch != -1)
    {
        // Use dot notation to get the next object
        wxString member2(member.Left((size_t) ch));
        wxString rest(member.Right(member.length() - ch - 1));
        wxAutomationObject obj;
        if (!GetObject(obj, member2))
            return false;
        return obj.Invoke(rest, action, retValue, noArgs, args, ptrArgs);
    }

    wxOleVariantArg vReturn;
    wxOleVariantArg* vReturnPtr = & vReturn;

    // When putting a property, the last argument is the new value which is
    // passed as the named argument DISPID_PROPERTYPUT.
    const bool isPut = (action & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF)) != 0;
    wxCHECK_MSG( !isPut || noArgs > 0, false, "Property value must be specified" );

    const int noIndexArgs = isPut ? noArgs - 1 : noArgs;

    // Separate named and positional arguments.
    wxVector<int> namedArgs, positionalArgs;
    int i;
    for (i = 0; i < noIndexArgs; i++)
    {
        if ( INVOKEARG(i).GetName().empty() )
        {
            // Positional arguments are bound to the first parameters, so
            // allowing them after the named ones could silently bind them to
            // a parameter that was also passed by name.
            wxCHECK_MSG( namedArgs.empty(), false,
                         "Positional arguments must precede named ones" );
            positionalArgs.push_back(i);
        }
        else
        {
            namedArgs.push_back(i);
        }
    }

    const int namedArgCount = static_cast<int>(namedArgs.size());

    // GetIDsOfNames expects the member name as the first name, followed by
    // argument names (if any).
    wxVector<wxBasicString> argNames(1 + namedArgCount);
    argNames[0].AssignFromString(member);
    for (i = 0; i < namedArgCount; i++)
        argNames[1 + i].AssignFromString(INVOKEARG(namedArgs[i]).GetName());

    wxVector<DISPID> dispIds(1 + namedArgCount);

    HRESULT hr;
    DISPPARAMS dispparams;
    unsigned int uiArgErr;

    hr = ((IDispatch*)m_dispatchPtr)->GetIDsOfNames(IID_NULL,
                                // We rely on the fact that wxBasicString is
                                // just BSTR with some methods here.
                                reinterpret_cast<BSTR *>(&argNames[0]),
                                1 + namedArgCount, m_lcid, &dispIds[0]);
    if (FAILED(hr))
    {
        ShowException(member, hr);
        return false;
    }

    // IDispatch::Invoke() expects the named arguments first, followed by the
    // positional ones in reverse order (all totally logical; hey, we're
    // dealing with OLE here).
    wxVector<int> argOrder;
    wxVector<DISPID> namedDispIds;
    argOrder.reserve(noArgs);

    if (isPut)
    {
        argOrder.push_back(noArgs - 1);
        namedDispIds.push_back(DISPID_PROPERTYPUT);
        vReturnPtr = nullptr;
    }

    for (i = 0; i < namedArgCount; i++)
    {
        argOrder.push_back(namedArgs[i]);
        namedDispIds.push_back(dispIds[1 + i]);
    }

    for (i = static_cast<int>(positionalArgs.size()) - 1; i >= 0; i--)
        argOrder.push_back(positionalArgs[i]);

    // Convert the wxVariants to VARIANTARGs
    wxVector<wxOleVariantArg> oleArgs(noArgs);
    for (i = 0; i < noArgs; i++)
    {
        if (!wxConvertVariantToOle(INVOKEARG(argOrder[i]), oleArgs[i]))
            return false;
    }

    dispparams.rgdispidNamedArgs = namedDispIds.empty() ? nullptr : &namedDispIds[0];
    dispparams.rgvarg = oleArgs.empty() ? nullptr : &oleArgs[0];
    dispparams.cArgs = noArgs;
    dispparams.cNamedArgs = static_cast<UINT>(namedDispIds.size());

    EXCEPINFO excep;
    wxZeroMemory(excep);

    hr = ((IDispatch*)m_dispatchPtr)->Invoke(dispIds[0], IID_NULL, m_lcid,
                        (WORD)action, &dispparams, vReturnPtr, &excep, &uiArgErr);

    if (FAILED(hr))
    {
        // display the exception information if appropriate:
        ShowException(member, hr, &excep, uiArgErr);

        // free exception structure information
        SysFreeString(excep.bstrSource);
        SysFreeString(excep.bstrDescription);
        SysFreeString(excep.bstrHelpFile);

        return false;
    }
    else
    {
        if (vReturnPtr)
        {
            // Convert result to wxVariant form
            if (!wxConvertOleToVariant(vReturn, retValue, m_convertVariantFlags))
                return false;
            // Mustn't release the dispatch pointer
            if (vReturn.vt == VT_DISPATCH)
            {
                vReturn.pdispVal = nullptr;
            }
            // Mustn't free the SAFEARRAY if it is contained in the retValue
            if ((vReturn.vt & VT_ARRAY) &&
                    retValue.GetType() == wxS("safearray"))
            {
                vReturn.parray = nullptr;
            }
        }
    }
    return true;
}

// Invoke a member function
wxVariant wxAutomationObject::CallMethod(const wxString& member, int noArgs, wxVariant args[])
{
    wxVariant retVariant;
    if (!Invoke(member, DISPATCH_METHOD, retVariant, noArgs, args))
    {
        retVariant.MakeNull();
    }
    return retVariant;
}

wxVariant wxAutomationObject::CallMethodArray(const wxString& member, int noArgs, const wxVariant **args)
{
    wxVariant retVariant;
    if (!Invoke(member, DISPATCH_METHOD, retVariant, noArgs, nullptr, args))
    {
        retVariant.MakeNull();
    }
    return retVariant;
}

wxVariant wxAutomationObject::CallMethod(const wxString& member,
        const wxVariant& arg1, const wxVariant& arg2,
        const wxVariant& arg3, const wxVariant& arg4,
        const wxVariant& arg5, const wxVariant& arg6)
{
    const wxVariant** args = new const wxVariant*[6];
    int i = 0;
    if (!arg1.IsNull())
    {
        args[i] = & arg1;
        i ++;
    }
    if (!arg2.IsNull())
    {
        args[i] = & arg2;
        i ++;
    }
    if (!arg3.IsNull())
    {
        args[i] = & arg3;
        i ++;
    }
    if (!arg4.IsNull())
    {
        args[i] = & arg4;
        i ++;
    }
    if (!arg5.IsNull())
    {
        args[i] = & arg5;
        i ++;
    }
    if (!arg6.IsNull())
    {
        args[i] = & arg6;
        i ++;
    }
    wxVariant retVariant;
    if (!Invoke(member, DISPATCH_METHOD, retVariant, i, nullptr, args))
    {
        retVariant.MakeNull();
    }
    delete[] args;
    return retVariant;
}

// Get/Set property
wxVariant wxAutomationObject::GetPropertyArray(const wxString& property, int noArgs, const wxVariant **args) const
{
    wxVariant retVariant;
    if (!Invoke(property, DISPATCH_PROPERTYGET, retVariant, noArgs, nullptr, args))
    {
        retVariant.MakeNull();
    }
    return retVariant;
}
wxVariant wxAutomationObject::GetProperty(const wxString& property, int noArgs, wxVariant args[]) const
{
    wxVariant retVariant;
    if (!Invoke(property, DISPATCH_PROPERTYGET, retVariant, noArgs, args))
    {
        retVariant.MakeNull();
    }
    return retVariant;
}

wxVariant wxAutomationObject::GetProperty(const wxString& property,
        const wxVariant& arg1, const wxVariant& arg2,
        const wxVariant& arg3, const wxVariant& arg4,
        const wxVariant& arg5, const wxVariant& arg6)
{
    const wxVariant** args = new const wxVariant*[6];
    int i = 0;
    if (!arg1.IsNull())
    {
        args[i] = & arg1;
        i ++;
    }
    if (!arg2.IsNull())
    {
        args[i] = & arg2;
        i ++;
    }
    if (!arg3.IsNull())
    {
        args[i] = & arg3;
        i ++;
    }
    if (!arg4.IsNull())
    {
        args[i] = & arg4;
        i ++;
    }
    if (!arg5.IsNull())
    {
        args[i] = & arg5;
        i ++;
    }
    if (!arg6.IsNull())
    {
        args[i] = & arg6;
        i ++;
    }
    wxVariant retVariant;
    if (!Invoke(property, DISPATCH_PROPERTYGET, retVariant, i, nullptr, args))
    {
        retVariant.MakeNull();
    }
    delete[] args;
    return retVariant;
}

bool wxAutomationObject::PutProperty(const wxString& property, int noArgs, wxVariant args[])
{
    wxVariant retVariant;
    if (!Invoke(property, DISPATCH_PROPERTYPUT, retVariant, noArgs, args))
    {
        return false;
    }
    return true;
}

bool wxAutomationObject::PutPropertyArray(const wxString& property, int noArgs, const wxVariant **args)
{
    wxVariant retVariant;
    if (!Invoke(property, DISPATCH_PROPERTYPUT, retVariant, noArgs, nullptr, args))
    {
        return false;
    }
    return true;
}

bool wxAutomationObject::PutProperty(const wxString& property,
        const wxVariant& arg1, const wxVariant& arg2,
        const wxVariant& arg3, const wxVariant& arg4,
        const wxVariant& arg5, const wxVariant& arg6)
{
    const wxVariant** args = new const wxVariant*[6];
    int i = 0;
    if (!arg1.IsNull())
    {
        args[i] = & arg1;
        i ++;
    }
    if (!arg2.IsNull())
    {
        args[i] = & arg2;
        i ++;
    }
    if (!arg3.IsNull())
    {
        args[i] = & arg3;
        i ++;
    }
    if (!arg4.IsNull())
    {
        args[i] = & arg4;
        i ++;
    }
    if (!arg5.IsNull())
    {
        args[i] = & arg5;
        i ++;
    }
    if (!arg6.IsNull())
    {
        args[i] = & arg6;
        i ++;
    }
    wxVariant retVariant;
    bool ret = Invoke(property, DISPATCH_PROPERTYPUT, retVariant, i, nullptr, args);
    delete[] args;
    return ret;
}


// Uses DISPATCH_PROPERTYGET
// and returns a dispatch pointer. The calling code should call Release
// on the pointer, though this could be implicit by constructing an wxAutomationObject
// with it and letting the destructor call Release.
WXIDISPATCH* wxAutomationObject::GetDispatchProperty(const wxString& property, int noArgs, wxVariant args[]) const
{
    wxVariant retVariant;
    if (Invoke(property, DISPATCH_PROPERTYGET, retVariant, noArgs, args))
    {
        if (retVariant.GetType() == wxT("void*"))
        {
            return (WXIDISPATCH*) retVariant.GetVoidPtr();
        }
    }

    return nullptr;
}

// Uses DISPATCH_PROPERTYGET
// and returns a dispatch pointer. The calling code should call Release
// on the pointer, though this could be implicit by constructing an wxAutomationObject
// with it and letting the destructor call Release.
WXIDISPATCH* wxAutomationObject::GetDispatchProperty(const wxString& property, int noArgs, const wxVariant **args) const
{
    wxVariant retVariant;
    if (Invoke(property, DISPATCH_PROPERTYGET, retVariant, noArgs, nullptr, args))
    {
        if (retVariant.GetType() == wxT("void*"))
        {
            return (WXIDISPATCH*) retVariant.GetVoidPtr();
        }
    }

    return nullptr;
}


// A way of initialising another wxAutomationObject with a dispatch object
bool wxAutomationObject::GetObject(wxAutomationObject& obj, const wxString& property, int noArgs, wxVariant args[]) const
{
    WXIDISPATCH* dispatch = GetDispatchProperty(property, noArgs, args);
    if (dispatch)
    {
        obj.SetDispatchPtr(dispatch);
        obj.SetLCID(GetLCID());
        obj.SetConvertVariantFlags(GetConvertVariantFlags());
        return true;
    }
    else
        return false;
}

// A way of initialising another wxAutomationObject with a dispatch object
bool wxAutomationObject::GetObject(wxAutomationObject& obj, const wxString& property, int noArgs, const wxVariant **args) const
{
    WXIDISPATCH* dispatch = GetDispatchProperty(property, noArgs, args);
    if (dispatch)
    {
        obj.SetDispatchPtr(dispatch);
        obj.SetLCID(GetLCID());
        obj.SetConvertVariantFlags(GetConvertVariantFlags());
        return true;
    }
    else
        return false;
}

namespace
{

HRESULT wxCLSIDFromProgID(const wxString& progId, CLSID& clsId)
{
    HRESULT hr = CLSIDFromProgID(wxBasicString(progId), &clsId);
    if ( FAILED(hr) )
    {
        wxLogSysError(hr, _("Failed to find CLSID of \"%s\""), progId);
    }
    return hr;
}

void *DoCreateInstance(const wxString& progId, const CLSID& clsId)
{
    // get the server IDispatch interface
    //
    // NB: using CLSCTX_INPROC_HANDLER results in failure when getting
    //     Automation interface for Microsoft Office applications so don't use
    //     CLSCTX_ALL which includes it
    void *pDispatch = nullptr;
    HRESULT hr = CoCreateInstance(clsId, nullptr, CLSCTX_SERVER,
                                  IID_IDispatch, &pDispatch);
    if (FAILED(hr))
    {
        wxLogSysError(hr, _("Failed to create an instance of \"%s\""), progId);
        return nullptr;
    }

    return pDispatch;
}

} // anonymous namespace

// Get a dispatch pointer from the current object associated
// with a ProgID
bool wxAutomationObject::GetInstance(const wxString& progId, int flags) const
{
    if (m_dispatchPtr)
        return false;

    CLSID clsId;
    HRESULT hr = wxCLSIDFromProgID(progId, clsId);
    if (FAILED(hr))
        return false;

    IUnknown *pUnk = nullptr;
    hr = GetActiveObject(clsId, nullptr, &pUnk);
    if (FAILED(hr))
    {
        if ( flags & wxAutomationInstance_CreateIfNeeded )
        {
            const_cast<wxAutomationObject *>(this)->
                m_dispatchPtr = DoCreateInstance(progId, clsId);
            if ( m_dispatchPtr )
                return true;
        }
        else
        {
            // Log an error except if we're supposed to fail silently when the
            // error is that no current instance exists.
            if ( hr != MK_E_UNAVAILABLE ||
                    !(flags & wxAutomationInstance_SilentIfNone) )
            {
                wxLogSysError(hr,
                              _("Cannot get an active instance of \"%s\""),
                              progId);
            }
        }

        return false;
    }

    hr = pUnk->QueryInterface(IID_IDispatch, const_cast<void**>(&m_dispatchPtr));
    if (FAILED(hr))
    {
        wxLogSysError(hr,
                      _("Failed to get OLE automation interface for \"%s\""),
                      progId);
        return false;
    }

    return true;
}

// Get a dispatch pointer from a new object associated
// with the given ProgID
bool wxAutomationObject::CreateInstance(const wxString& progId) const
{
    if (m_dispatchPtr)
        return false;

    CLSID clsId;
    HRESULT hr = wxCLSIDFromProgID(progId, clsId);
    if (FAILED(hr))
        return false;

    const_cast<wxAutomationObject *>(this)->
        m_dispatchPtr = DoCreateInstance(progId, clsId);

    return m_dispatchPtr != nullptr;
}

WXLCID wxAutomationObject::GetLCID() const
{
    return m_lcid;
}

void wxAutomationObject::SetLCID(WXLCID lcid)
{
    m_lcid = lcid;
}

long wxAutomationObject::GetConvertVariantFlags() const
{
    return m_convertVariantFlags;
}

void wxAutomationObject::SetConvertVariantFlags(long flags)
{
    m_convertVariantFlags = flags;
}


static void
ShowException(const wxString& member,
              HRESULT hr,
              EXCEPINFO *pexcep,
              unsigned int uiArgErr)
{
    wxString message;
    switch (GetScode(hr))
    {
        case DISP_E_UNKNOWNNAME:
            message = _("Unknown name or named argument.");
            break;

        case DISP_E_BADPARAMCOUNT:
            message = _("Incorrect number of arguments.");
            break;

        case DISP_E_EXCEPTION:
            if ( pexcep )
            {
                if ( pexcep->bstrDescription )
                    message << pexcep->bstrDescription << wxS(" ");
                message += wxString::Format(wxS("error code %u"), pexcep->wCode);
            }
            else
            {
                message = _("Unknown exception");
            }
            break;

        case DISP_E_MEMBERNOTFOUND:
            message = _("Method or property not found.");
            break;

        case DISP_E_OVERFLOW:
            message = _("Overflow while coercing argument values.");
            break;

        case DISP_E_NONAMEDARGS:
            message = _("Object implementation does not support named arguments.");
            break;

        case DISP_E_UNKNOWNLCID:
            message = _("The locale ID is unknown.");
            break;

        case DISP_E_PARAMNOTOPTIONAL:
            message = _("Missing a required parameter.");
            break;

        case DISP_E_PARAMNOTFOUND:
            message.Printf(_("Argument %u not found."), uiArgErr);
            break;

        case DISP_E_TYPEMISMATCH:
            message.Printf(_("Type mismatch in argument %u."), uiArgErr);
            break;

        case ERROR_FILE_NOT_FOUND:
            message = _("The system cannot find the file specified.");
            break;

        case REGDB_E_CLASSNOTREG:
            message = _("Class not registered.");
            break;

        default:
            message.Printf(_("Unknown error %08x"), hr);
            break;
    }

    wxLogError(_("OLE Automation error in %s: %s"), member, message);
}

#endif // wxUSE_OLE_AUTOMATION
