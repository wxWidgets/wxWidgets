///////////////////////////////////////////////////////////////////////////////
// Name:        msw/ole/access.cpp
// Purpose:     implementation of wxIAccessible and wxAccessible
// Author:      Julian Smart
// Modified by:
// Created:     2003-02-12
// RCS-ID:      $Id$
// Copyright:   (c) 2003 Julian Smart
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
  #pragma implementation "access.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
  #pragma hdrstop
#endif
#ifndef WX_PRECOMP
#include "wx/window.h"
#endif

#include "wx/setup.h"

#if wxUSE_OLE && wxUSE_ACCESSIBILITY

#include "wx/log.h"
#include "wx/access.h"

#include <windows.h>

// for some compilers, the entire ole2.h must be included, not only oleauto.h
#if wxUSE_NORLANDER_HEADERS || defined(__WATCOMC__)
    #include <ole2.h>
#endif

#include <oleauto.h>
#include <oleacc.h>

#include "wx/msw/ole/oleutils.h"

#ifndef CHILDID_SELF
#define CHILDID_SELF 0
#endif

#ifndef OBJID_CLIENT
#define OBJID_CLIENT 0xFFFFFFFC
#endif

// Convert to Windows role
int wxConvertToWindowsRole(wxAccRole wxrole);

// Convert to Windows state
long wxConvertToWindowsState(long wxstate);

// Convert to Windows selection flag
int wxConvertToWindowsSelFlag(wxAccSelectionFlags sel);

// Convert from Windows selection flag
wxAccSelectionFlags wxConvertFromWindowsSelFlag(int sel);

// ----------------------------------------------------------------------------
// wxIEnumVARIANT interface implementation
// ----------------------------------------------------------------------------

class wxIEnumVARIANT : public IEnumVARIANT
{
public:
    wxIEnumVARIANT(const wxVariant& variant);
    ~wxIEnumVARIANT() { }

    DECLARE_IUNKNOWN_METHODS;

    // IEnumVARIANT
    STDMETHODIMP Next(ULONG celt, VARIANT *rgelt, ULONG *pceltFetched);
    STDMETHODIMP Skip(ULONG celt);
    STDMETHODIMP Reset();
    STDMETHODIMP Clone(IEnumVARIANT **ppenum);

private:
    wxVariant m_variant;  // List of further variants
    int       m_nCurrent; // Current enum position

    DECLARE_NO_COPY_CLASS(wxIEnumVARIANT)
};

// ----------------------------------------------------------------------------
// wxIEnumVARIANT
// ----------------------------------------------------------------------------

BEGIN_IID_TABLE(wxIEnumVARIANT)
    ADD_IID(Unknown)
    ADD_IID(EnumVARIANT)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxIEnumVARIANT)

// wxVariant contains a list of further variants.
wxIEnumVARIANT::wxIEnumVARIANT(const wxVariant& variant)
{
    m_variant = variant;
}

STDMETHODIMP wxIEnumVARIANT::Next(ULONG      celt,
                                    VARIANT *rgelt,
                                    ULONG     *pceltFetched)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumVARIANT::Next"));

    if ( celt > 1 ) {
        // we only return 1 element at a time - mainly because I'm too lazy to
        // implement something which you're never asked for anyhow
        return S_FALSE;
    }

    if (m_variant.GetType() != wxT("list"))
        return S_FALSE;

    if ( m_nCurrent < (int) m_variant.GetList().GetCount() ) {
        if (!wxConvertVariantToOle(m_variant[m_nCurrent++], rgelt[0]))
        {
            return S_FALSE;
        }

        // TODO: should we AddRef if this is an object?

        * pceltFetched = 1;
        return S_OK;
    }
    else {
        // bad index
        return S_FALSE;
    }
}

STDMETHODIMP wxIEnumVARIANT::Skip(ULONG celt)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumVARIANT::Skip"));

    if (m_variant.GetType() != wxT("list"))
        return S_FALSE;

    m_nCurrent += celt;
    if ( m_nCurrent < (int) m_variant.GetList().GetCount() )
        return S_OK;

    // no, can't skip this many elements
    m_nCurrent -= celt;

    return S_FALSE;
}

STDMETHODIMP wxIEnumVARIANT::Reset()
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumVARIANT::Reset"));

    m_nCurrent = 0;

    return S_OK;
}

STDMETHODIMP wxIEnumVARIANT::Clone(IEnumVARIANT **ppenum)
{
    wxLogTrace(wxTRACE_OleCalls, wxT("wxIEnumVARIANT::Clone"));

    wxIEnumVARIANT *pNew = new wxIEnumVARIANT(m_variant);
    pNew->AddRef();
    *ppenum = pNew;

    return S_OK;
}


// ----------------------------------------------------------------------------
// wxIAccessible implementation of IAccessible interface
// ----------------------------------------------------------------------------

class wxIAccessible : public IAccessible
{
public:
    wxIAccessible(wxAccessible *pAccessible);

    DECLARE_IUNKNOWN_METHODS;

// IAccessible

// Navigation and Hierarchy

        // Retrieves the child element or child object at a given point on the screen.
        // All visual objects support this method; sound objects do not support it.    

    STDMETHODIMP accHitTest(long xLeft, long yLeft, VARIANT* pVarID);

        // Retrieves the specified object's current screen location. All visual objects must
        // support this method; sound objects do not support it.    

    STDMETHODIMP accLocation ( long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varID);

        // Traverses to another user interface element within a container and retrieves the object.
        // All visual objects must support this method.

    STDMETHODIMP accNavigate ( long navDir, VARIANT varStart, VARIANT* pVarEnd);

        // Retrieves the address of an IDispatch interface for the specified child.
        // All objects must support this property.
    
    STDMETHODIMP get_accChild ( VARIANT varChildID, IDispatch** ppDispChild);

        // Retrieves the number of children that belong to this object.
        // All objects must support this property.
    
    STDMETHODIMP get_accChildCount ( long* pCountChildren);

        // Retrieves the IDispatch interface of the object's parent.
        // All objects support this property.
    
    STDMETHODIMP get_accParent ( IDispatch** ppDispParent);

// Descriptive Properties and Methods

        // Performs the object's default action. Not all objects have a default
        // action.
    
    STDMETHODIMP accDoDefaultAction(VARIANT varID);

        // Retrieves a string that describes the object's default action.
        // Not all objects have a default action.
    
    STDMETHODIMP get_accDefaultAction ( VARIANT varID, BSTR* pszDefaultAction);

        // Retrieves a string that describes the visual appearance of the specified object.
        // Not all objects have a description.

    STDMETHODIMP get_accDescription ( VARIANT varID, BSTR* pszDescription);

        // Retrieves an object's Help property string.
        // Not all objects support this property.

    STDMETHODIMP get_accHelp ( VARIANT varID, BSTR* pszHelp);

        // Retrieves the full path of the WinHelp file associated with the specified
        // object and the identifier of the appropriate topic within that file.
        // Not all objects support this property.
        
    STDMETHODIMP get_accHelpTopic ( BSTR* pszHelpFile, VARIANT varChild, long* pidTopic);

        // Retrieves the specified object's shortcut key or access key, also known as
        // the mnemonic. All objects that have a shortcut key or access key support
        // this property.
        
    STDMETHODIMP get_accKeyboardShortcut ( VARIANT varID, BSTR* pszKeyboardShortcut);

        // Retrieves the name of the specified object.
        // All objects support this property.
    
    STDMETHODIMP get_accName ( VARIANT varID, BSTR* pszName);

        // Retrieves information that describes the role of the specified object.
        // All objects support this property.

    STDMETHODIMP get_accRole ( VARIANT varID, VARIANT* pVarRole);

        // Retrieves the current state of the specified object.
        // All objects support this property.
    
    STDMETHODIMP get_accState ( VARIANT varID, VARIANT* pVarState);
    
        // Retrieves the value of the specified object.
        // Not all objects have a value.

    STDMETHODIMP get_accValue ( VARIANT varID, BSTR* pszValue);

// Selection and Focus

        // Modifies the selection or moves the keyboard focus of the
        // specified object. All objects that select or receive the
        // keyboard focus must support this method.

    STDMETHODIMP accSelect ( long flagsSelect, VARIANT varID );

        // Retrieves the object that has the keyboard focus. All objects
        // that receive the keyboard focus must support this property.
        
    STDMETHODIMP get_accFocus ( VARIANT* pVarID);

        // Retrieves the selected children of this object. All objects
        // selected must support this property.
        
    STDMETHODIMP get_accSelection ( VARIANT * pVarChildren);

// Obsolete

    STDMETHODIMP put_accName(VARIANT varChild, BSTR szName) { return E_FAIL; }
    STDMETHODIMP put_accValue(VARIANT varChild, BSTR szName) { return E_FAIL; }

// IDispatch

        // Get type info

    STDMETHODIMP GetTypeInfo(unsigned int typeInfo, LCID lcid, ITypeInfo** ppTypeInfo);

        // Get type info count

    STDMETHODIMP GetTypeInfoCount(unsigned int* typeInfoCount);

        // Get ids of names

    STDMETHODIMP GetIDsOfNames(REFIID riid, OLECHAR** names, unsigned int cNames,
        LCID lcid, DISPID* dispId);

        // Invoke

    STDMETHODIMP Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, 
                        WORD wFlags, DISPPARAMS *pDispParams, 
                        VARIANT *pVarResult, EXCEPINFO *pExcepInfo, 
                        unsigned int *puArgErr );

private:
    wxAccessible *m_pAccessible;      // pointer to C++ class we belong to

    DECLARE_NO_COPY_CLASS(wxIAccessible)
};

// ============================================================================
// Implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxIAccessible implementation
// ----------------------------------------------------------------------------
BEGIN_IID_TABLE(wxIAccessible)
  ADD_IID(Unknown)
  ADD_IID(Accessible)
  ADD_IID(Dispatch)
END_IID_TABLE;

IMPLEMENT_IUNKNOWN_METHODS(wxIAccessible)

wxIAccessible::wxIAccessible(wxAccessible *pAccessible)
{
    wxASSERT( pAccessible != NULL );

    m_pAccessible = pAccessible;
}

// Retrieves the child element or child object at a given point on the screen.
// All visual objects support this method; sound objects do not support it.    

STDMETHODIMP wxIAccessible::accHitTest(long xLeft, long yLeft, VARIANT* pVarID)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    wxAccessible* childObject = NULL;
    int childId = 0;
    VariantInit(pVarID);
    
    wxAccStatus status = m_pAccessible->HitTest(wxPoint(xLeft, yLeft), & childId, & childObject);
    
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->accHitTest(xLeft, yLeft, pVarID);
    }
    
    if (childObject)
    {
        if (childObject == m_pAccessible)
        {
            pVarID->vt = VT_I4;
            pVarID->lVal = CHILDID_SELF;
            return S_OK;
        }
        else
        {
            wxIAccessible* childIA = childObject->GetIAccessible();
            if (!childIA)
                return E_FAIL;

	        if (childIA->QueryInterface(IID_IDispatch, (LPVOID*) pVarID->pdispVal) != S_OK)
	            return E_FAIL;
            
            pVarID->vt = VT_DISPATCH;
            pVarID->pdispVal->AddRef();
            return S_OK;
        }
    }
    else if (childId > 0)
    {
        pVarID->vt = VT_I4;
        pVarID->lVal = childId;
        return S_OK;
    }
    else
    {
        pVarID->vt = VT_EMPTY;
        return S_FALSE;
    }
    
    return E_FAIL;
}

// Retrieves the specified object's current screen location. All visual objects must
// support this method; sound objects do not support it.    

STDMETHODIMP wxIAccessible::accLocation ( long* pxLeft, long* pyTop, long* pcxWidth, long* pcyHeight, VARIANT varID)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    wxRect rect;

    wxAccStatus status = m_pAccessible->GetLocation(rect, varID.lVal);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->accLocation(pxLeft, pyTop, pcxWidth, pcyHeight, varID);
    }
    else
    {
        *pxLeft = rect.x;
        *pyTop = rect.y;
        *pcxWidth = rect.width;
        *pcyHeight = rect.height;
        return S_OK;
    }

    return E_FAIL;
}

// Traverses to another user interface element within a container and retrieves the object.
// All visual objects must support this method.

STDMETHODIMP wxIAccessible::accNavigate ( long navDir, VARIANT varStart, VARIANT* pVarEnd)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;

    if (varStart.vt != VT_I4)
        return E_INVALIDARG;
    
    wxAccessible* elementObject = NULL;
    int elementId = 0;
    VariantInit(pVarEnd);
    wxNavDir navDirWX = wxNAVDIR_FIRSTCHILD;

    switch (navDir)
    {
    case NAVDIR_DOWN:
        navDirWX = wxNAVDIR_DOWN;
        break;

    case NAVDIR_FIRSTCHILD:
        navDirWX = wxNAVDIR_FIRSTCHILD;
        break;

    case NAVDIR_LASTCHILD:
        navDirWX = wxNAVDIR_LASTCHILD;
        break;

    case NAVDIR_LEFT:
        navDirWX = wxNAVDIR_LEFT;
        break;

    case NAVDIR_NEXT:
        navDirWX = wxNAVDIR_NEXT;
        break;

    case NAVDIR_PREVIOUS:
        navDirWX = wxNAVDIR_PREVIOUS;
        break;

    case NAVDIR_RIGHT:
        navDirWX = wxNAVDIR_RIGHT;
        break;

    case NAVDIR_UP:
        navDirWX = wxNAVDIR_UP;
        break;
    }
    
    wxAccStatus status = m_pAccessible->Navigate(navDirWX, varStart.lVal, & elementId,
        & elementObject);

    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->accNavigate ( navDir, varStart, pVarEnd);
    }
    else
    {
        if (elementObject)
        {
            wxIAccessible* objectIA = elementObject->GetIAccessible();
            if (!objectIA)
                return E_FAIL;

	        if (objectIA->QueryInterface(IID_IDispatch, (LPVOID*) pVarEnd->pdispVal) != S_OK)
	            return E_FAIL;
            
            pVarEnd->vt = VT_DISPATCH;
            pVarEnd->pdispVal->AddRef();
            return S_OK;
        }
        else if (elementId > 0)
        {
            pVarEnd->vt = VT_I4;
            pVarEnd->lVal = elementId;
            return S_OK;
        }
        else
        {
            pVarEnd->vt = VT_EMPTY;
            return S_FALSE;
        }
    }

    return E_FAIL;
}

// Retrieves the address of an IDispatch interface for the specified child.
// All objects must support this property.
    
STDMETHODIMP wxIAccessible::get_accChild ( VARIANT varChildID, IDispatch** ppDispChild)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varChildID.vt != VT_I4)
        return E_INVALIDARG;

    if (varChildID.lVal == CHILDID_SELF)
    {
        *ppDispChild = this;
        AddRef();
        return S_OK;
    }
    
    wxAccessible* child = NULL;

    wxAccStatus status = m_pAccessible->GetChild(varChildID.lVal, & child);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accChild (varChildID, ppDispChild);
    }
    else
    {
        if (child)
        {
            wxIAccessible* objectIA = child->GetIAccessible();
            if (!objectIA)
                return E_FAIL;
            *ppDispChild = objectIA;
            objectIA->AddRef();
            return S_OK;
        }
        else
            return S_FALSE; // Indicates it's not an accessible object
    }

    return E_FAIL;
}

// Retrieves the number of children that belong to this object.
// All objects must support this property.
    
STDMETHODIMP wxIAccessible::get_accChildCount ( long* pCountChildren)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    int childCount = 0;
    wxAccStatus status = m_pAccessible->GetChildCount(& childCount);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accChildCount (pCountChildren);
    }
    else
    {
        * pCountChildren = (long) childCount;
        return S_OK;
    }

    return E_FAIL;
}

// Retrieves the IDispatch interface of the object's parent.
// All objects support this property.
    
STDMETHODIMP wxIAccessible::get_accParent ( IDispatch** ppDispParent)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    wxAccessible* parent = NULL;
    wxAccStatus status = m_pAccessible->GetParent(& parent);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accParent (ppDispParent);
    }
    else
    {
        if (parent)
        {
            wxIAccessible* objectIA = parent->GetIAccessible();
            if (!objectIA)
                return E_FAIL;

	        if (objectIA->QueryInterface(IID_IDispatch, (LPVOID*) ppDispParent) != S_OK)
	            return E_FAIL;
            
            (*ppDispParent)->AddRef();
            return S_OK;
/*
            wxIAccessible* objectIA = parent->GetIAccessible();
            if (!objectIA)
                return E_FAIL;
            objectIA->AddRef();
            *ppDispParent = objectIA;
            return S_OK;
*/
        }
        else
        {
            *ppDispParent = NULL;
            return S_OK;
        }
    }

    return E_FAIL;
}

// Performs the object's default action. Not all objects have a default
// action.
    
STDMETHODIMP wxIAccessible::accDoDefaultAction(VARIANT varID)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxAccStatus status = m_pAccessible->DoDefaultAction(varID.lVal);
    if (status == wxACC_FAIL)
        return E_FAIL;

    if (status == wxACC_NOT_SUPPORTED)
        return DISP_E_MEMBERNOTFOUND;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->accDoDefaultAction(varID);
    }
    return E_FAIL;
}

// Retrieves a string that describes the object's default action.
// Not all objects have a default action.
    
STDMETHODIMP wxIAccessible::get_accDefaultAction ( VARIANT varID, BSTR* pszDefaultAction)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxString defaultAction;
    wxAccStatus status = m_pAccessible->GetDefaultAction(varID.lVal, & defaultAction);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_SUPPORTED)
        return DISP_E_MEMBERNOTFOUND;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accDefaultAction (varID, pszDefaultAction);
    }
    else
    {
        if (defaultAction.IsEmpty())
        {
            * pszDefaultAction = NULL;
            return S_FALSE;
        }
        else
        {
            wxBasicString basicString(defaultAction);
            * pszDefaultAction = basicString.Get();
            return S_OK;
        }
    }
    return E_FAIL;
}

// Retrieves a string that describes the visual appearance of the specified object.
// Not all objects have a description.

STDMETHODIMP wxIAccessible::get_accDescription ( VARIANT varID, BSTR* pszDescription)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxString description;
    wxAccStatus status = m_pAccessible->GetDescription(varID.lVal, & description);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accDescription (varID, pszDescription);
    }
    else
    {
        if (description.IsEmpty())
        {
            * pszDescription = NULL;
            return S_FALSE;
        }
        else
        {
            wxBasicString basicString(description);
            * pszDescription = basicString.Get();
            return S_OK;
        }        
    }
    return E_FAIL;
}

// Retrieves an object's Help property string.
// Not all objects support this property.

STDMETHODIMP wxIAccessible::get_accHelp ( VARIANT varID, BSTR* pszHelp)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxString helpString;
    wxAccStatus status = m_pAccessible->GetHelpText(varID.lVal, & helpString);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accHelp (varID, pszHelp);
    }
    else
    {
        if (helpString.IsEmpty())
        {
            * pszHelp = NULL;
            return S_FALSE;
        }
        else
        {
            wxBasicString basicString(helpString);
            * pszHelp = basicString.Get();
            return S_OK;
        }        
    }
    return E_FAIL;
}

// Retrieves the full path of the WinHelp file associated with the specified
// object and the identifier of the appropriate topic within that file.
// Not all objects support this property.
// NOTE: not supported by wxWindows at this time. Use
// GetHelpText instead.
        
STDMETHODIMP wxIAccessible::get_accHelpTopic ( BSTR* pszHelpFile, VARIANT varChild, long* pidTopic)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varChild.vt != VT_I4)
        return E_INVALIDARG;
    
    wxAccStatus status = wxACC_NOT_IMPLEMENTED;
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accHelpTopic (pszHelpFile, varChild, pidTopic);
    }
    return E_FAIL;
}

// Retrieves the specified object's shortcut key or access key, also known as
// the mnemonic. All objects that have a shortcut key or access key support
// this property.
        
STDMETHODIMP wxIAccessible::get_accKeyboardShortcut ( VARIANT varID, BSTR* pszKeyboardShortcut)
{
    *pszKeyboardShortcut = NULL;

    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxString keyboardShortcut;
    wxAccStatus status = m_pAccessible->GetHelpText(varID.lVal, & keyboardShortcut);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accKeyboardShortcut(varID, pszKeyboardShortcut);
    }
    else
    {
        if (keyboardShortcut.IsEmpty())
        {
            * pszKeyboardShortcut = NULL;
            return S_FALSE;
        }
        else
        {
            wxBasicString basicString(keyboardShortcut);
            * pszKeyboardShortcut = basicString.Get();
            return S_OK;
        }        
    }
    return E_FAIL;
}

// Retrieves the name of the specified object.
// All objects support this property.
    
STDMETHODIMP wxIAccessible::get_accName ( VARIANT varID, BSTR* pszName)
{
    *pszName = NULL;

    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;

    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxString name;
    
    wxAccStatus status = m_pAccessible->GetName(varID.lVal, & name);
    
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accName (varID, pszName);
    }
    else
    {
        wxBasicString basicString(name);
        *pszName = basicString.Get();
        return S_OK;
    }
    return E_FAIL;
}

// Retrieves information that describes the role of the specified object.
// All objects support this property.

STDMETHODIMP wxIAccessible::get_accRole ( VARIANT varID, VARIANT* pVarRole)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    VariantInit(pVarRole);

    wxAccRole role = wxROLE_NONE;
    
    wxAccStatus status = m_pAccessible->GetRole(varID.lVal, & role);
    
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accRole (varID, pVarRole);
    }
    else
    {
        if (role == wxROLE_NONE)
        {
            pVarRole->vt = VT_EMPTY;
            return S_OK;
        }

        pVarRole->lVal = wxConvertToWindowsRole(role);
        pVarRole->vt = VT_I4;

        return S_OK;
    }
    return E_FAIL;
}

// Retrieves the current state of the specified object.
// All objects support this property.
    
STDMETHODIMP wxIAccessible::get_accState ( VARIANT varID, VARIANT* pVarState)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;

    long wxstate = 0;
    
    wxAccStatus status = m_pAccessible->GetState(varID.lVal, & wxstate);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accState (varID, pVarState);
    }
    else
    {
        long state = wxConvertToWindowsState(wxstate);
        pVarState->lVal = state;
        pVarState->vt = VT_I4;
        return S_OK;
    }
    return E_FAIL;
}
    
// Retrieves the value of the specified object.
// Not all objects have a value.

STDMETHODIMP wxIAccessible::get_accValue ( VARIANT varID, BSTR* pszValue)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxString strValue;
    
    wxAccStatus status = m_pAccessible->GetValue(varID.lVal, & strValue);
    
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accValue (varID, pszValue);
    }
    else
    {
        wxBasicString basicString(strValue);
        * pszValue = basicString.Get();
        return S_OK;
    }
    return E_FAIL;
}

// Modifies the selection or moves the keyboard focus of the
// specified object. All objects that select or receive the
// keyboard focus must support this method.

STDMETHODIMP wxIAccessible::accSelect ( long flagsSelect, VARIANT varID )
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    if (varID.vt != VT_I4)
        return E_INVALIDARG;
    
    wxAccSelectionFlags wxsel = wxConvertFromWindowsSelFlag(flagsSelect);

    wxAccStatus status = m_pAccessible->Select(varID.lVal, wxsel);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->accSelect ( flagsSelect, varID );
    }
    else
        return S_OK;

    return E_FAIL;
}

// Retrieves the object that has the keyboard focus. All objects
// that receive the keyboard focus must support this property.
        
STDMETHODIMP wxIAccessible::get_accFocus ( VARIANT* pVarID)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;
    
    wxAccessible* childObject = NULL;
    int childId = 0;
    VariantInit(pVarID);
    
    wxAccStatus status = m_pAccessible->GetFocus(& childId, & childObject);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accFocus (pVarID);
    }
    if (childObject)
    {
        if (childObject == m_pAccessible)
        {
            pVarID->vt = VT_I4;
            pVarID->lVal = CHILDID_SELF;
            return S_OK;
        }
        else
        {
            wxIAccessible* childIA = childObject->GetIAccessible();
            if (!childIA)
                return E_FAIL;

	        if (childIA->QueryInterface(IID_IDispatch, (LPVOID*) pVarID->pdispVal) != S_OK)
	            return E_FAIL;
            
            pVarID->vt = VT_DISPATCH;
            pVarID->pdispVal->AddRef();
            return S_OK;
        }
    }
    else if (childId > 0)
    {
        pVarID->vt = VT_I4;
        pVarID->lVal = childId;
        return S_OK;
    }
    else
    {
        pVarID->vt = VT_EMPTY;
        return S_FALSE;
    }
    
    return E_FAIL;
}

// Retrieves the selected children of this object. All objects
// selected must support this property.
        
STDMETHODIMP wxIAccessible::get_accSelection ( VARIANT * pVarChildren)
{
    wxASSERT (m_pAccessible != NULL);
    if (!m_pAccessible)
        return E_FAIL;

    VariantInit(pVarChildren);
    
    wxVariant selections;
    wxAccStatus status = m_pAccessible->GetSelections(& selections);
    if (status == wxACC_FAIL)
        return E_FAIL;
    
    if (status == wxACC_NOT_IMPLEMENTED)
    {
        // Use standard interface instead.
        IAccessible* stdInterface = (IAccessible*)m_pAccessible->GetIAccessibleStd();
        if (!stdInterface)
            return E_FAIL;
        else
            return stdInterface->get_accSelection (pVarChildren);
    }
    else
    {
        if (selections.GetType() == wxT("long"))
        {
            pVarChildren->vt = VT_I4;
            pVarChildren->lVal = selections.GetLong();

            return S_OK;
        }
        else if (selections.GetType() == wxT("void*"))
        {
            wxAccessible* childObject = (wxAccessible*) selections.GetVoidPtr();
            wxIAccessible* childIA = childObject->GetIAccessible();
            if (!childIA)
                return E_FAIL;

	        if (childIA->QueryInterface(IID_IDispatch, (LPVOID*) pVarChildren->pdispVal) != S_OK)
	            return E_FAIL;
            
            pVarChildren->vt = VT_DISPATCH;
            pVarChildren->pdispVal->AddRef();

            return S_OK;
        }
        else if (selections.GetType() == wxT("list"))
        {
            // TODO: should we AddRef for every "void*" member??

            wxIEnumVARIANT* enumVariant = new wxIEnumVARIANT(selections);
            enumVariant->AddRef();

            pVarChildren->vt = VT_UNKNOWN;
            pVarChildren->punkVal = enumVariant;

            return S_OK;
        }
    }

    return E_FAIL;
}

// Get type info

STDMETHODIMP wxIAccessible::GetTypeInfo(unsigned int typeInfo, LCID lcid, ITypeInfo** ppTypeInfo)
{
    *ppTypeInfo = NULL;
    return E_NOTIMPL;
}

// Get type info count

STDMETHODIMP wxIAccessible::GetTypeInfoCount(unsigned int* typeInfoCount)
{
    *typeInfoCount = 0;
    return E_NOTIMPL;
}

// Get ids of names

STDMETHODIMP wxIAccessible::GetIDsOfNames(REFIID riid, OLECHAR** names, unsigned int cNames,
        LCID lcid, DISPID* dispId)
{
    return E_NOTIMPL;
}

// Invoke

STDMETHODIMP wxIAccessible::Invoke(DISPID dispIdMember, REFIID riid, LCID lcid, 
                        WORD wFlags, DISPPARAMS *pDispParams, 
                        VARIANT *pVarResult, EXCEPINFO *pExcepInfo, 
                        unsigned int *puArgErr )
{
    return E_NOTIMPL;
}

// ----------------------------------------------------------------------------
// wxAccessible implementation
// ----------------------------------------------------------------------------

// ctors

// common part of all ctors
void wxAccessible::Init()
{
    m_pIAccessibleStd = NULL;
    m_pIAccessible = new wxIAccessible(this);
    m_pIAccessible->AddRef();
}

wxAccessible::wxAccessible(wxWindow* win)
            : wxAccessibleBase(win)
{
    Init();
}

wxAccessible::~wxAccessible()
{
    m_pIAccessible->Release();
    if (m_pIAccessibleStd)
        ((IAccessible*)m_pIAccessibleStd)->Release();
}

// Gets or creates a standard interface for this object.
void* wxAccessible::GetIAccessibleStd()
{
    if (m_pIAccessibleStd)
        return m_pIAccessibleStd;

    if (GetWindow())
    {
#if 0
        HRESULT retCode = ::CreateStdAccessibleProxy((HWND) GetWindow()->GetHWND(),
                wxT("wxWindowClass"), OBJID_CLIENT, IID_IAccessible, (void**) & m_pIAccessibleStd);
#else
        HRESULT retCode = ::CreateStdAccessibleObject((HWND) GetWindow()->GetHWND(),
                OBJID_CLIENT, IID_IAccessible, (void**) & m_pIAccessibleStd);
#endif
        if (retCode == S_OK)
            return m_pIAccessibleStd;
        else
        {
            m_pIAccessibleStd = NULL;
            return NULL;
        }
    }
    return NULL;
}

// Utilities

// Convert to Windows role
int wxConvertToWindowsRole(wxAccRole wxrole)
{
    switch (wxrole)
    {
    case wxROLE_SYSTEM_ALERT:
        return ROLE_SYSTEM_ALERT;
    case wxROLE_SYSTEM_ANIMATION:
        return ROLE_SYSTEM_ANIMATION;
    case wxROLE_SYSTEM_APPLICATION:
        return ROLE_SYSTEM_APPLICATION;
    case wxROLE_SYSTEM_BORDER:
        return ROLE_SYSTEM_BORDER;
    case wxROLE_SYSTEM_BUTTONDROPDOWN:
        return ROLE_SYSTEM_BUTTONDROPDOWN;
    case wxROLE_SYSTEM_BUTTONDROPDOWNGRID:
        return ROLE_SYSTEM_BUTTONDROPDOWNGRID;
    case wxROLE_SYSTEM_BUTTONMENU:
        return ROLE_SYSTEM_BUTTONMENU;
    case wxROLE_SYSTEM_CARET:
        return ROLE_SYSTEM_CARET;
    case wxROLE_SYSTEM_CELL:
        return ROLE_SYSTEM_CELL;
    case wxROLE_SYSTEM_CHARACTER:
        return ROLE_SYSTEM_CHARACTER;
    case wxROLE_SYSTEM_CHART:
        return ROLE_SYSTEM_CHART;
    case wxROLE_SYSTEM_CHECKBUTTON:
        return ROLE_SYSTEM_CHECKBUTTON;
    case wxROLE_SYSTEM_CLIENT:
        return ROLE_SYSTEM_CLIENT;
    case wxROLE_SYSTEM_CLOCK:
        return ROLE_SYSTEM_CLOCK;
    case wxROLE_SYSTEM_COLUMN:
        return ROLE_SYSTEM_COLUMN;
    case wxROLE_SYSTEM_COLUMNHEADER:
        return ROLE_SYSTEM_COLUMNHEADER;
    case wxROLE_SYSTEM_COMBOBOX:
        return ROLE_SYSTEM_COMBOBOX;
    case wxROLE_SYSTEM_CURSOR:
        return ROLE_SYSTEM_CURSOR;
    case wxROLE_SYSTEM_DIAGRAM:
        return ROLE_SYSTEM_DIAGRAM;
    case wxROLE_SYSTEM_DIAL:
        return ROLE_SYSTEM_DIAL;
    case wxROLE_SYSTEM_DIALOG:
        return ROLE_SYSTEM_DIALOG;
    case wxROLE_SYSTEM_DOCUMENT:
        return ROLE_SYSTEM_DOCUMENT;
    case wxROLE_SYSTEM_DROPLIST:
        return ROLE_SYSTEM_DROPLIST;
    case wxROLE_SYSTEM_EQUATION:
        return ROLE_SYSTEM_EQUATION;
    case wxROLE_SYSTEM_GRAPHIC:
        return ROLE_SYSTEM_GRAPHIC;
    case wxROLE_SYSTEM_GRIP:
        return ROLE_SYSTEM_GRIP;
    case wxROLE_SYSTEM_GROUPING:
        return ROLE_SYSTEM_GROUPING;
    case wxROLE_SYSTEM_HELPBALLOON:
        return ROLE_SYSTEM_HELPBALLOON;
    case wxROLE_SYSTEM_HOTKEYFIELD:
        return ROLE_SYSTEM_HOTKEYFIELD;
    case wxROLE_SYSTEM_INDICATOR:
        return ROLE_SYSTEM_INDICATOR;
    case wxROLE_SYSTEM_LINK:
        return ROLE_SYSTEM_LINK;
    case wxROLE_SYSTEM_LIST:
        return ROLE_SYSTEM_LIST;
    case wxROLE_SYSTEM_LISTITEM:
        return ROLE_SYSTEM_LISTITEM;
    case wxROLE_SYSTEM_MENUBAR:
        return ROLE_SYSTEM_MENUBAR;
    case wxROLE_SYSTEM_MENUITEM:
        return ROLE_SYSTEM_MENUITEM;
    case wxROLE_SYSTEM_MENUPOPUP:
        return ROLE_SYSTEM_MENUPOPUP;
    case wxROLE_SYSTEM_OUTLINE:
        return ROLE_SYSTEM_OUTLINE;
    case wxROLE_SYSTEM_OUTLINEITEM:
        return ROLE_SYSTEM_OUTLINEITEM;
    case wxROLE_SYSTEM_PAGETAB:
        return ROLE_SYSTEM_PAGETAB;
    case wxROLE_SYSTEM_PAGETABLIST:
        return ROLE_SYSTEM_PAGETABLIST;
    case wxROLE_SYSTEM_PANE:
        return ROLE_SYSTEM_PANE;
    case wxROLE_SYSTEM_PROGRESSBAR:
        return ROLE_SYSTEM_PROGRESSBAR;
    case wxROLE_SYSTEM_PROPERTYPAGE:
        return ROLE_SYSTEM_PROPERTYPAGE;
    case wxROLE_SYSTEM_PUSHBUTTON:
        return ROLE_SYSTEM_PUSHBUTTON;
    case wxROLE_SYSTEM_RADIOBUTTON:
        return ROLE_SYSTEM_RADIOBUTTON;
    case wxROLE_SYSTEM_ROW:
        return ROLE_SYSTEM_ROW;
    case wxROLE_SYSTEM_ROWHEADER:
        return ROLE_SYSTEM_ROWHEADER;
    case wxROLE_SYSTEM_SCROLLBAR:
        return ROLE_SYSTEM_SCROLLBAR;
    case wxROLE_SYSTEM_SEPARATOR:
        return ROLE_SYSTEM_SEPARATOR;
    case wxROLE_SYSTEM_SLIDER:
        return ROLE_SYSTEM_SLIDER;
    case wxROLE_SYSTEM_SOUND:
        return ROLE_SYSTEM_SOUND;
    case wxROLE_SYSTEM_SPINBUTTON:
        return ROLE_SYSTEM_SPINBUTTON;
    case wxROLE_SYSTEM_STATICTEXT:
        return ROLE_SYSTEM_STATICTEXT;
    case wxROLE_SYSTEM_STATUSBAR:
        return ROLE_SYSTEM_STATUSBAR;
    case wxROLE_SYSTEM_TABLE:
        return ROLE_SYSTEM_TABLE;
    case wxROLE_SYSTEM_TEXT:
        return ROLE_SYSTEM_TEXT;
    case wxROLE_SYSTEM_TITLEBAR:
        return ROLE_SYSTEM_TITLEBAR;
    case wxROLE_SYSTEM_TOOLBAR:
        return ROLE_SYSTEM_TOOLBAR;
    case wxROLE_SYSTEM_TOOLTIP:
        return ROLE_SYSTEM_TOOLTIP;
    case wxROLE_SYSTEM_WHITESPACE:
        return ROLE_SYSTEM_WHITESPACE;
    case wxROLE_SYSTEM_WINDOW:
        return ROLE_SYSTEM_WINDOW;
    }
    return 0;
}

// Convert to Windows state
long wxConvertToWindowsState(long wxstate)
{
    long state = 0;
    if (wxstate & wxACC_STATE_SYSTEM_ALERT_HIGH)
        state |= STATE_SYSTEM_ALERT_HIGH;

    if (wxstate & wxACC_STATE_SYSTEM_ALERT_MEDIUM)
        state |= STATE_SYSTEM_ALERT_MEDIUM;

    if (wxstate & wxACC_STATE_SYSTEM_ALERT_LOW)
        state |= STATE_SYSTEM_ALERT_LOW;

    if (wxstate & wxACC_STATE_SYSTEM_ANIMATED)
        state |= STATE_SYSTEM_ANIMATED;

    if (wxstate & wxACC_STATE_SYSTEM_BUSY)
        state |= STATE_SYSTEM_BUSY;

    if (wxstate & wxACC_STATE_SYSTEM_CHECKED)
        state |= STATE_SYSTEM_CHECKED;

    if (wxstate & wxACC_STATE_SYSTEM_COLLAPSED)
        state |= STATE_SYSTEM_COLLAPSED;

    if (wxstate & wxACC_STATE_SYSTEM_DEFAULT)
        state |= STATE_SYSTEM_DEFAULT;

    if (wxstate & wxACC_STATE_SYSTEM_EXPANDED)
        state |= STATE_SYSTEM_EXPANDED;

    if (wxstate & wxACC_STATE_SYSTEM_EXTSELECTABLE)
        state |= STATE_SYSTEM_EXTSELECTABLE;

    if (wxstate & wxACC_STATE_SYSTEM_FLOATING)
        state |= STATE_SYSTEM_FLOATING;

    if (wxstate & wxACC_STATE_SYSTEM_FOCUSABLE)
        state |= STATE_SYSTEM_FOCUSABLE;

    if (wxstate & wxACC_STATE_SYSTEM_FOCUSED)
        state |= STATE_SYSTEM_FOCUSED;

    if (wxstate & wxACC_STATE_SYSTEM_HOTTRACKED)
        state |= STATE_SYSTEM_HOTTRACKED;

    if (wxstate & wxACC_STATE_SYSTEM_INVISIBLE)
        state |= STATE_SYSTEM_INVISIBLE;

    if (wxstate & wxACC_STATE_SYSTEM_INVISIBLE)
        state |= STATE_SYSTEM_INVISIBLE;

    if (wxstate & wxACC_STATE_SYSTEM_MIXED)
        state |= STATE_SYSTEM_MIXED;

    if (wxstate & wxACC_STATE_SYSTEM_MULTISELECTABLE)
        state |= STATE_SYSTEM_MULTISELECTABLE;

    if (wxstate & wxACC_STATE_SYSTEM_OFFSCREEN)
        state |= STATE_SYSTEM_OFFSCREEN;

    if (wxstate & wxACC_STATE_SYSTEM_PRESSED)
        state |= STATE_SYSTEM_PRESSED;

//    if (wxstate & wxACC_STATE_SYSTEM_PROTECTED)
//        state |= STATE_SYSTEM_PROTECTED;

    if (wxstate & wxACC_STATE_SYSTEM_READONLY)
        state |= STATE_SYSTEM_READONLY;

    if (wxstate & wxACC_STATE_SYSTEM_SELECTABLE)
        state |= STATE_SYSTEM_SELECTABLE;

    if (wxstate & wxACC_STATE_SYSTEM_SELECTED)
        state |= STATE_SYSTEM_SELECTED;

    if (wxstate & wxACC_STATE_SYSTEM_SELFVOICING)
        state |= STATE_SYSTEM_SELFVOICING;

    if (wxstate & wxACC_STATE_SYSTEM_UNAVAILABLE)
        state |= STATE_SYSTEM_UNAVAILABLE;

    return state;
}

// Convert to Windows selection flag
int wxConvertToWindowsSelFlag(wxAccSelectionFlags wxsel)
{
    int sel = 0;

    if (sel & wxACC_SEL_TAKEFOCUS)
        sel |= SELFLAG_TAKEFOCUS;
    if (sel & wxACC_SEL_TAKESELECTION)
        sel |= SELFLAG_TAKESELECTION;
    if (sel & wxACC_SEL_EXTENDSELECTION)
        sel |= SELFLAG_EXTENDSELECTION;
    if (sel & wxACC_SEL_ADDSELECTION)
        sel |= SELFLAG_ADDSELECTION;
    if (sel & wxACC_SEL_REMOVESELECTION)
        sel |= SELFLAG_REMOVESELECTION;
    return sel;
}

// Convert from Windows selection flag
wxAccSelectionFlags wxConvertFromWindowsSelFlag(int sel)
{
    int wxsel = 0;

    if (sel & SELFLAG_TAKEFOCUS)
        wxsel |= wxACC_SEL_TAKEFOCUS;
    if (sel & SELFLAG_TAKESELECTION)
        wxsel |= wxACC_SEL_TAKESELECTION;
    if (sel & SELFLAG_EXTENDSELECTION)
        wxsel |= wxACC_SEL_EXTENDSELECTION;
    if (sel & SELFLAG_ADDSELECTION)
        wxsel |= wxACC_SEL_ADDSELECTION;
    if (sel & SELFLAG_REMOVESELECTION)
        wxsel |= wxACC_SEL_REMOVESELECTION;
    return (wxAccSelectionFlags) wxsel;
}


#endif  //USE_ACCESSIBILITY
