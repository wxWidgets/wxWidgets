/////////////////////////////////////////////////////////////////////////////
// Name:        automtn.h
// Purpose:     OLE automation utilities
// Author:      Julian Smart
// Modified by:
// Created:     11/6/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998, Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma interface "automtn.h"
#endif

#ifndef _WX_AUTOMTN_H_
#define _WX_AUTOMTN_H_

#include "wx/object.h"
#include "wx/variant.h"

typedef void            WXIDISPATCH;
typedef unsigned short* WXBSTR;

#ifdef GetObject
#undef GetObject
#endif

/*
 * wxAutomationObject
 * Wraps up an IDispatch pointer and invocation; does variant conversion.
 */

class WXDLLEXPORT wxAutomationObject: public wxObject
{
public:
    wxAutomationObject(WXIDISPATCH* dispatchPtr = NULL);
    ~wxAutomationObject();

	// Set/get dispatch pointer
    inline void SetDispatchPtr(WXIDISPATCH* dispatchPtr) { m_dispatchPtr = dispatchPtr; };
    inline WXIDISPATCH* GetDispatchPtr() const { return m_dispatchPtr; }

	// Get a dispatch pointer from the current object associated
	// with a class id, such as "Excel.Application"
	bool GetInstance(const wxString& classId) const;

	// Get a dispatch pointer from a new instance of the the class
	bool CreateInstance(const wxString& classId) const;

	// Low-level invocation function. Pass either an array of variants,
	// or an array of pointers to variants.
    bool Invoke(const wxString& member, int action,
        wxVariant& retValue, int noArgs, wxVariant args[], const wxVariant* ptrArgs[] = 0) const;

	// Invoke a member function
	wxVariant CallMethod(const wxString& method, int noArgs, wxVariant args[]);

	// Convenience function
	wxVariant CallMethod(const wxString& method,
		const wxVariant& arg1 = wxNullVariant, const wxVariant& arg2 = wxNullVariant,
		const wxVariant& arg3 = wxNullVariant, const wxVariant& arg4 = wxNullVariant,
		const wxVariant& arg5 = wxNullVariant, const wxVariant& arg6 = wxNullVariant);

	// Get/Put property
    wxVariant GetProperty(const wxString& property, int noArgs = 0, wxVariant args[] = (wxVariant*) NULL) const;
	wxVariant GetProperty(const wxString& property,
		const wxVariant& arg1, const wxVariant& arg2 = wxNullVariant,
		const wxVariant& arg3 = wxNullVariant, const wxVariant& arg4 = wxNullVariant,
		const wxVariant& arg5 = wxNullVariant, const wxVariant& arg6 = wxNullVariant);

    bool PutProperty(const wxString& property, int noArgs, wxVariant args[]) ;
	bool PutProperty(const wxString& property,
		const wxVariant& arg1, const wxVariant& arg2 = wxNullVariant,
		const wxVariant& arg3 = wxNullVariant, const wxVariant& arg4 = wxNullVariant,
		const wxVariant& arg5 = wxNullVariant, const wxVariant& arg6 = wxNullVariant);

	// Uses DISPATCH_PROPERTYGET
	// and returns a dispatch pointer. The calling code should call Release
	// on the pointer, though this could be implicit by constructing an wxAutomationObject
	// with it and letting the destructor call Release.
    WXIDISPATCH* GetDispatchProperty(const wxString& property, int noArgs, wxVariant args[]) const;

	// A way of initialising another wxAutomationObject with a dispatch object,
	// without having to deal with nasty IDispatch pointers.
	bool GetObject(wxAutomationObject& obj, const wxString& property, int noArgs = 0, wxVariant args[] = (wxVariant*) NULL) const;

public:
    WXIDISPATCH*  m_dispatchPtr;
};


#endif
    // _WX_AUTOMTN_H_
