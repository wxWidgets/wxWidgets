/////////////////////////////////////////////////////////////////////////////
// Name:        automtn.cpp
// Purpose:     OLE automation utilities
// Author:      Julian Smart
// Modified by:
// Created:     11/6/98
// RCS-ID:      $Id$
// Copyright:   (c) 1998, Julian Smart
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "automtn.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if defined(__BORLANDC__)
#pragma hdrstop
#endif

#include "wx/log.h"

#include <math.h>
#include <time.h>

#include "wx/msw/ole/automtn.h"

#include "wx/msw/private.h"

#include <ole2ver.h>
#include <oleauto.h>

// wrapper around BSTR type (by Vadim Zeitlin)

class WXDLLEXPORT BasicString
{
public:
  // ctors & dtor
  BasicString(const char *sz);
 ~BasicString();

  // accessors
    // just get the string
  operator BSTR() const { return m_wzBuf; }
    // retrieve a copy of our string - caller must SysFreeString() it later!
  BSTR Get() const { return SysAllocString(m_wzBuf); }

private:
  // @@@ not implemented (but should be)
  BasicString(const BasicString&);
  BasicString& operator=(const BasicString&);

  OLECHAR *m_wzBuf;     // actual string
};

// Convert variants
static bool ConvertVariantToOle(const wxVariant& variant, VARIANTARG& oleVariant) ;
static bool ConvertOleToVariant(const VARIANTARG& oleVariant, wxVariant& variant) ;

// Convert string to Unicode
static BSTR ConvertStringToOle(const wxString& str);

// Convert string from BSTR to wxString
static wxString ConvertStringFromOle(BSTR bStr);

// Verifies will fail if the needed buffer size is too large
#define MAX_TIME_BUFFER_SIZE    128         // matches that in timecore.cpp
#define MIN_DATE                (-657434L)  // about year 100
#define MAX_DATE                2958465L    // about year 9999

// Half a second, expressed in days
#define HALF_SECOND  (1.0/172800.0)

// One-based array of days in year at month start
static int rgMonthDays[13] =
	{0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334, 365};

static BOOL OleDateFromTm(WORD wYear, WORD wMonth, WORD wDay,
	WORD wHour, WORD wMinute, WORD wSecond, DATE& dtDest);
static BOOL TmFromOleDate(DATE dtSrc, struct tm& tmDest);

static void ClearVariant(VARIANTARG *pvarg) ;
static void ReleaseVariant(VARIANTARG *pvarg) ;
// static void ShowException(LPOLESTR szMember, HRESULT hr, EXCEPINFO *pexcep, unsigned int uiArgErr);

/*
 * wxAutomationObject
 */

wxAutomationObject::wxAutomationObject(WXIDISPATCH* dispatchPtr)
{
	m_dispatchPtr = dispatchPtr;
}

wxAutomationObject::~wxAutomationObject()
{
	if (m_dispatchPtr)
	{
		((IDispatch*)m_dispatchPtr)->Release();
		m_dispatchPtr = NULL;
	}
}

#define INVOKEARG(i) (args ? args[i] : *(ptrArgs[i]))

// For Put/Get, no named arguments are allowed.
bool wxAutomationObject::Invoke(const wxString& member, int action,
        wxVariant& retValue, int noArgs, wxVariant args[], const wxVariant* ptrArgs[]) const
{
	if (!m_dispatchPtr)
		return FALSE;

	// nonConstMember is necessary because the wxString class doesn't have enough consts...
	wxString nonConstMember(member);

	int ch = nonConstMember.Find('.');
	if (ch != -1)
	{
		// Use dot notation to get the next object
		wxString member2(nonConstMember.Left((size_t) ch));
		wxString rest(nonConstMember.Right(nonConstMember.Length() - ch - 1));
		wxAutomationObject obj;
		if (!GetObject(obj, member2))
			return FALSE;
		return obj.Invoke(rest, action, retValue, noArgs, args, ptrArgs);
	}

	VARIANTARG vReturn;
	ClearVariant(& vReturn);

	VARIANTARG* vReturnPtr = & vReturn;

	// Find number of names args
	int namedArgCount = 0;
	int i;
	for (i = 0; i < noArgs; i++)
		if (!INVOKEARG(i).GetName().IsNull())
		{
			namedArgCount ++;
		}

	int namedArgStringCount = namedArgCount + 1;
	BSTR* argNames = new BSTR[namedArgStringCount];
	argNames[0] = ConvertStringToOle(member);

	// Note that arguments are specified in reverse order
	// (all totally logical; hey, we're dealing with OLE here.)

	int j = 0;
	for (i = 0; i < namedArgCount; i++)
	{	
		if (!INVOKEARG(i).GetName().IsNull())
		{
			argNames[(namedArgCount-j)] = ConvertStringToOle(INVOKEARG(i).GetName());
			j ++;
		}
	}

	// + 1 for the member name, + 1 again in case we're a 'put'
	DISPID* dispIds = new DISPID[namedArgCount + 2];

	HRESULT hr;
	DISPPARAMS dispparams;
	unsigned int uiArgErr;
	EXCEPINFO excep;
	
	// Get the IDs for the member and its arguments.  GetIDsOfNames expects the
	// member name as the first name, followed by argument names (if any).
	hr = ((IDispatch*)m_dispatchPtr)->GetIDsOfNames(IID_NULL, argNames,
								1 + namedArgCount, LOCALE_SYSTEM_DEFAULT, dispIds);
	if (FAILED(hr)) 
	{
//		ShowException(szMember, hr, NULL, 0);
		return FALSE;
	}

	// if doing a property put(ref), we need to adjust the first argument to have a
	// named arg of DISPID_PROPERTYPUT.
	if (action & (DISPATCH_PROPERTYPUT | DISPATCH_PROPERTYPUTREF))
	{
		namedArgCount = 1;
		dispIds[1] = DISPID_PROPERTYPUT;
		vReturnPtr = (VARIANTARG*) NULL;
	}

	// Convert the wxVariants to VARIANTARGs
	VARIANTARG* oleArgs = new VARIANTARG[noArgs];
	for (i = 0; i < noArgs; i++)
	{
		// Again, reverse args
		if (!ConvertVariantToOle(INVOKEARG((noArgs-1) - i), oleArgs[i]))
			return FALSE; // TODO: clean up memory at this point
	}

	dispparams.rgdispidNamedArgs = dispIds + 1;
	dispparams.rgvarg = oleArgs;
	dispparams.cArgs = noArgs;
	dispparams.cNamedArgs = namedArgCount;
	
	excep.pfnDeferredFillIn = NULL;
	
	hr = ((IDispatch*)m_dispatchPtr)->Invoke(dispIds[0], IID_NULL, LOCALE_SYSTEM_DEFAULT,
						action, &dispparams, vReturnPtr, &excep, &uiArgErr);

	for (i = 0; i < namedArgStringCount; i++)
	{
		SysFreeString(argNames[i]);
	}
	delete[] argNames;
	delete[] dispIds;
	
	for (i = 0; i < noArgs; i++)
		ReleaseVariant(& oleArgs[i]) ;
	delete[] oleArgs;
	
	if (FAILED(hr)) 
	{
		// display the exception information if appropriate:
//		ShowException((const char*) member, hr, &excep, uiArgErr);
	
		// free exception structure information
		SysFreeString(excep.bstrSource);
		SysFreeString(excep.bstrDescription);
		SysFreeString(excep.bstrHelpFile);

		if (vReturnPtr)
			ReleaseVariant(vReturnPtr);
		return FALSE;
	}
	else
	{
		if (vReturnPtr)
		{
			// Convert result to wxVariant form
			ConvertOleToVariant(vReturn, retValue);
			// Mustn't release the dispatch pointer
			if (vReturn.vt == VT_DISPATCH)
			{
				vReturn.pdispVal = (IDispatch*) NULL;
			}
			ReleaseVariant(& vReturn);
		}
	}
	return TRUE;
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
	if (!Invoke(member, DISPATCH_METHOD, retVariant, i, NULL, args))
	{
		retVariant.MakeNull();
	}
	delete[] args;
	return retVariant;
}

// Get/Set property
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
	if (!Invoke(property, DISPATCH_PROPERTYGET, retVariant, i, NULL, args))
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
		return FALSE;
	}
	return TRUE;
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
	bool ret = Invoke(property, DISPATCH_PROPERTYPUT, retVariant, i, NULL, args);
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
		if (retVariant.GetType() == _T("void*"))
		{
			return (WXIDISPATCH*) retVariant.GetVoidPtr();
		}
		else
		{
			return (WXIDISPATCH*) NULL;
		}
	}
	else
		return (WXIDISPATCH*) NULL;
}

// A way of initialising another wxAutomationObject with a dispatch object
bool wxAutomationObject::GetObject(wxAutomationObject& obj, const wxString& property, int noArgs, wxVariant args[]) const
{
	WXIDISPATCH* dispatch = GetDispatchProperty(property, noArgs, args);
	if (dispatch)
	{
		obj.SetDispatchPtr(dispatch);
		return TRUE;
	}
	else
		return FALSE;
}

// Get a dispatch pointer from the current object associated
// with a class id
bool wxAutomationObject::GetInstance(const wxString& classId) const
{
	if (m_dispatchPtr)
		return FALSE;

	CLSID clsId;
	IUnknown * pUnk = NULL;

	BasicString unicodeName(classId.mb_str());
	
	if (FAILED(CLSIDFromProgID((BSTR) unicodeName, &clsId))) 
	{
		wxLogWarning(_T("Cannot obtain CLSID from ProgID"));
		return FALSE;
	}

	if (FAILED(GetActiveObject(clsId, NULL, &pUnk)))
	{
		wxLogWarning(_T("Cannot find an active object"));
		return FALSE;
	}
	
	if (pUnk->QueryInterface(IID_IDispatch, (LPVOID*) &m_dispatchPtr) != S_OK)
	{
		wxLogWarning(_T("Cannot find IDispatch interface"));
		return FALSE;
	}

	return TRUE;
}

// Get a dispatch pointer from a new object associated
// with the given class id
bool wxAutomationObject::CreateInstance(const wxString& classId) const
{
	if (m_dispatchPtr)
		return FALSE;

	CLSID clsId;

	BasicString unicodeName(classId.mb_str());
	
	if (FAILED(CLSIDFromProgID((BSTR) unicodeName, &clsId))) 
	{
		wxLogWarning(_T("Cannot obtain CLSID from ProgID"));
		return FALSE;
	}

	// start a new copy of Excel, grab the IDispatch interface
	if (FAILED(CoCreateInstance(clsId, NULL, CLSCTX_LOCAL_SERVER, IID_IDispatch, (void**)&m_dispatchPtr))) 
	{
		wxLogWarning(_T("Cannot start an instance of this class."));
		return FALSE;
	}
	
	return TRUE;
}


bool ConvertVariantToOle(const wxVariant& variant, VARIANTARG& oleVariant)
{
	ClearVariant(&oleVariant);
	if (variant.IsNull())
	{
		oleVariant.vt = VT_NULL;
		return TRUE;
	}

    wxString type(variant.GetType());

    if (type == _T("long"))
    {
        oleVariant.vt = VT_I4;
        oleVariant.lVal = variant.GetLong() ;
    }
    else if (type == _T("double"))
    {
        oleVariant.vt = VT_R8;
        oleVariant.dblVal = variant.GetDouble();
    }
    else if (type == _T("bool"))
    {
        oleVariant.vt = VT_BOOL;
        // 'bool' required for VC++ 4 apparently
#if defined(__WATCOMC__) || (defined(__VISUALC__) && (__VISUALC__ <= 1000))
        oleVariant.bool = variant.GetBool();
#else
        oleVariant.boolVal = variant.GetBool();
#endif
    }
    else if (type == _T("string"))
    {
        wxString str( variant.GetString() );
        oleVariant.vt = VT_BSTR;
        oleVariant.bstrVal = ConvertStringToOle(str);
    }
    else if (type == _T("date"))
    {
        wxDate date( variant.GetDate() );
        oleVariant.vt = VT_DATE;

		if (!OleDateFromTm(date.GetYear(), date.GetMonth(), date.GetDay(),
				0, 0, 0, oleVariant.date))
			return FALSE;
    }
    else if (type == _T("time"))
    {
        wxTime time( variant.GetTime() );
        oleVariant.vt = VT_DATE;

		if (!OleDateFromTm(time.GetYear(), time.GetMonth(), time.GetDay(),
			time.GetHour(), time.GetMinute(), time.GetSecond(), oleVariant.date))
			return FALSE;
    }
    else if (type == _T("void*"))
    {
        oleVariant.vt = VT_DISPATCH;
        oleVariant.pdispVal = (IDispatch*) variant.GetVoidPtr();
    }
    else if (type == _T("list") || type == _T("stringlist"))
    {
        oleVariant.vt = VT_VARIANT | VT_ARRAY;

	    SAFEARRAY *psa;
	    SAFEARRAYBOUND saBound;
	    VARIANTARG *pvargBase;
	    VARIANTARG *pvarg;
	    int i, j;

        int iCount = variant.GetCount();
	
	    saBound.lLbound = 0;
	    saBound.cElements = iCount;
	
	    psa = SafeArrayCreate(VT_VARIANT, 1, &saBound);
	    if (psa == NULL)
		    return FALSE;
	
	    SafeArrayAccessData(psa, (void**)&pvargBase);
	
	    pvarg = pvargBase;
	    for (i = 0; i < iCount; i++)
	    {
		    // copy each string in the list of strings
            wxVariant eachVariant(variant[i]);
            if (!ConvertVariantToOle(eachVariant, * pvarg))
            {
			    // memory failure:  back out and free strings alloc'ed up to
			    // now, and then the array itself.
			    pvarg = pvargBase;
			    for (j = 0; j < i; j++)
			    {
				    SysFreeString(pvarg->bstrVal);
				    pvarg++;
			    }
			    SafeArrayDestroy(psa);
			    return FALSE;
		    }
		    pvarg++;
	    }
	
	    SafeArrayUnaccessData(psa);

        oleVariant.parray = psa;
    }
    else
    {
        oleVariant.vt = VT_NULL;
        return FALSE;
    }
    return TRUE;
}

#ifndef VT_TYPEMASK
#define VT_TYPEMASK 0xfff
#endif

bool ConvertOleToVariant(const VARIANTARG& oleVariant, wxVariant& variant)
{
	switch (oleVariant.vt & VT_TYPEMASK)
	{
	case VT_BSTR:
		{
			wxString str(ConvertStringFromOle(oleVariant.bstrVal));
			variant = str;
			break;
		}
	case VT_DATE:
		{
			struct tm tmTemp;
			if (!TmFromOleDate(oleVariant.date, tmTemp))
				return FALSE;

			wxDate date(tmTemp.tm_yday, tmTemp.tm_mon, tmTemp.tm_year);
			wxTime time(date, tmTemp.tm_hour, tmTemp.tm_min, tmTemp.tm_sec);

			variant = time;
			break;
		}
	case VT_I4:
		{
			variant = (long) oleVariant.lVal;
			break;
		}
	case VT_I2:
		{
			variant = (long) oleVariant.iVal;
			break;
		}

	case VT_BOOL:
		{
#if defined(__WATCOMC__) || (defined(_MSC_VER) && (_MSC_VER <= 1000)) //GC
#ifndef HAVE_BOOL // Can't use bool operator if no native bool type
			variant = (long) (oleVariant.bool != 0);
#else
			variant = (bool) (oleVariant.bool != 0);
#endif
#else
			variant = (bool) (oleVariant.boolVal != 0);
#endif
			break;
		}
	case VT_R8:
		{
			variant = oleVariant.dblVal;
			break;
		}
	case VT_ARRAY:
		{
			variant.ClearList();

			int cDims, cElements, i;
			VARIANTARG* pvdata;

			// Iterate the dimensions: number of elements is x*y*z
			for (cDims = 0, cElements = 1;
				cDims < oleVariant.parray->cDims; cDims ++)
					cElements *= oleVariant.parray->rgsabound[cDims].cElements;

			// Get a pointer to the data
			HRESULT hr = SafeArrayAccessData(oleVariant.parray, (void HUGEP* FAR*) & pvdata);
			if (hr != NOERROR)
				return FALSE;
			// Iterate the data.
			for (i = 0; i < cElements; i++)
			{
				VARIANTARG& oleElement = pvdata[i];
				wxVariant vElement;
				if (!ConvertOleToVariant(oleElement, vElement))
					return FALSE;
				
				variant.Append(vElement);
			}
			SafeArrayUnaccessData(oleVariant.parray);
			break;
		}
	case VT_DISPATCH:
		{
			variant = (void*) oleVariant.pdispVal;
			break;
		}
	case VT_NULL:
		{
			variant.MakeNull();
			break;
		}
	case VT_EMPTY:
		{
			break;	// Ignore Empty Variant, used only during destruction of objects
		}
	default:
		{
			wxLogError(_T("wxAutomationObject::ConvertOleToVariant: Unknown variant value type"));
			return FALSE;
		}
	}
    return TRUE;
}

static BSTR ConvertStringToOle(const wxString& str)
{
/*
	unsigned int len = strlen((const char*) str);
	unsigned short* s = new unsigned short[len*2+2];
	unsigned int i;
	memset(s, 0, len*2+2);
	for (i=0; i < len; i++)
		s[i*2] = str[i];
*/
	BasicString bstr(str.mb_str());
	return bstr.Get();
}

static wxString ConvertStringFromOle(BSTR bStr)
{
	int len = SysStringLen(bStr) + 1;
	char    *buf = new char[len];
	(void)wcstombs( buf, bStr, len);

	wxString str(buf);
	delete[] buf;
	return str;
}

// ----------------------------------------------------------------------------
// BasicString
// ----------------------------------------------------------------------------

// ctor takes an ANSI string and transforms it to Unicode
BasicString::BasicString(const char *sz)
{
  // get the size of required buffer
  UINT lenAnsi = strlen(sz);
  #ifdef __MWERKS__
  UINT lenWide = lenAnsi * 2 ;
  #else
  UINT lenWide = mbstowcs(NULL, sz, lenAnsi);
  #endif

  if ( lenWide > 0 ) {
    m_wzBuf = new OLECHAR[lenWide + 1];
    mbstowcs(m_wzBuf, sz, lenAnsi);
    m_wzBuf[lenWide] = L'\0';
  }
  else {
    m_wzBuf = NULL;
  }
}

// dtor frees memory
BasicString::~BasicString()
{
  delete [] m_wzBuf;
}

/////////////////////////////////////////////////////////////////////////////
// COleDateTime class HELPERS - implementation

BOOL OleDateFromTm(WORD wYear, WORD wMonth, WORD wDay,
	WORD wHour, WORD wMinute, WORD wSecond, DATE& dtDest)
{
	// Validate year and month (ignore day of week and milliseconds)
	if (wYear > 9999 || wMonth < 1 || wMonth > 12)
		return FALSE;

	//  Check for leap year and set the number of days in the month
	BOOL bLeapYear = ((wYear & 3) == 0) &&
		((wYear % 100) != 0 || (wYear % 400) == 0);

	int nDaysInMonth =
		rgMonthDays[wMonth] - rgMonthDays[wMonth-1] +
		((bLeapYear && wDay == 29 && wMonth == 2) ? 1 : 0);

	// Finish validating the date
	if (wDay < 1 || wDay > nDaysInMonth ||
		wHour > 23 || wMinute > 59 ||
		wSecond > 59)
	{
		return FALSE;
	}

	// Cache the date in days and time in fractional days
	long nDate;
	double dblTime;

	//It is a valid date; make Jan 1, 1AD be 1
	nDate = wYear*365L + wYear/4 - wYear/100 + wYear/400 +
		rgMonthDays[wMonth-1] + wDay;

	//  If leap year and it's before March, subtract 1:
	if (wMonth <= 2 && bLeapYear)
		--nDate;

	//  Offset so that 12/30/1899 is 0
	nDate -= 693959L;

	dblTime = (((long)wHour * 3600L) +  // hrs in seconds
		((long)wMinute * 60L) +  // mins in seconds
		((long)wSecond)) / 86400.;

	dtDest = (double) nDate + ((nDate >= 0) ? dblTime : -dblTime);

	return TRUE;
}

BOOL TmFromOleDate(DATE dtSrc, struct tm& tmDest)
{
	// The legal range does not actually span year 0 to 9999.
	if (dtSrc > MAX_DATE || dtSrc < MIN_DATE) // about year 100 to about 9999
		return FALSE;

	long nDays;             // Number of days since Dec. 30, 1899
	long nDaysAbsolute;     // Number of days since 1/1/0
	long nSecsInDay;        // Time in seconds since midnight
	long nMinutesInDay;     // Minutes in day

	long n400Years;         // Number of 400 year increments since 1/1/0
	long n400Century;       // Century within 400 year block (0,1,2 or 3)
	long n4Years;           // Number of 4 year increments since 1/1/0
	long n4Day;             // Day within 4 year block
							//  (0 is 1/1/yr1, 1460 is 12/31/yr4)
	long n4Yr;              // Year within 4 year block (0,1,2 or 3)
	BOOL bLeap4 = TRUE;     // TRUE if 4 year block includes leap year

	double dblDate = dtSrc; // tempory serial date

	// If a valid date, then this conversion should not overflow
	nDays = (long)dblDate;

	// Round to the second
	dblDate += ((dtSrc > 0.0) ? HALF_SECOND : -HALF_SECOND);

	nDaysAbsolute = (long)dblDate + 693959L; // Add days from 1/1/0 to 12/30/1899

	dblDate = fabs(dblDate);
	nSecsInDay = (long)((dblDate - floor(dblDate)) * 86400.);

	// Calculate the day of week (sun=1, mon=2...)
	//   -1 because 1/1/0 is Sat.  +1 because we want 1-based
	tmDest.tm_wday = (int)((nDaysAbsolute - 1) % 7L) + 1;

	// Leap years every 4 yrs except centuries not multiples of 400.
	n400Years = (long)(nDaysAbsolute / 146097L);

	// Set nDaysAbsolute to day within 400-year block
	nDaysAbsolute %= 146097L;

	// -1 because first century has extra day
	n400Century = (long)((nDaysAbsolute - 1) / 36524L);

	// Non-leap century
	if (n400Century != 0)
	{
		// Set nDaysAbsolute to day within century
		nDaysAbsolute = (nDaysAbsolute - 1) % 36524L;

		// +1 because 1st 4 year increment has 1460 days
		n4Years = (long)((nDaysAbsolute + 1) / 1461L);

		if (n4Years != 0)
			n4Day = (long)((nDaysAbsolute + 1) % 1461L);
		else
		{
			bLeap4 = FALSE;
			n4Day = (long)nDaysAbsolute;
		}
	}
	else
	{
		// Leap century - not special case!
		n4Years = (long)(nDaysAbsolute / 1461L);
		n4Day = (long)(nDaysAbsolute % 1461L);
	}

	if (bLeap4)
	{
		// -1 because first year has 366 days
		n4Yr = (n4Day - 1) / 365;

		if (n4Yr != 0)
			n4Day = (n4Day - 1) % 365;
	}
	else
	{
		n4Yr = n4Day / 365;
		n4Day %= 365;
	}

	// n4Day is now 0-based day of year. Save 1-based day of year, year number
	tmDest.tm_yday = (int)n4Day + 1;
	tmDest.tm_year = n400Years * 400 + n400Century * 100 + n4Years * 4 + n4Yr;

	// Handle leap year: before, on, and after Feb. 29.
	if (n4Yr == 0 && bLeap4)
	{
		// Leap Year
		if (n4Day == 59)
		{
			/* Feb. 29 */
			tmDest.tm_mon = 2;
			tmDest.tm_mday = 29;
			goto DoTime;
		}

		// Pretend it's not a leap year for month/day comp.
		if (n4Day >= 60)
			--n4Day;
	}

	// Make n4DaY a 1-based day of non-leap year and compute
	//  month/day for everything but Feb. 29.
	++n4Day;

	// Month number always >= n/32, so save some loop time */
	for (tmDest.tm_mon = (n4Day >> 5) + 1;
		n4Day > rgMonthDays[tmDest.tm_mon]; tmDest.tm_mon++);

	tmDest.tm_mday = (int)(n4Day - rgMonthDays[tmDest.tm_mon-1]);

DoTime:
	if (nSecsInDay == 0)
		tmDest.tm_hour = tmDest.tm_min = tmDest.tm_sec = 0;
	else
	{
		tmDest.tm_sec = (int)nSecsInDay % 60L;
		nMinutesInDay = nSecsInDay / 60L;
		tmDest.tm_min = (int)nMinutesInDay % 60;
		tmDest.tm_hour = (int)nMinutesInDay / 60;
	}

	return TRUE;
}

// this function is not used
#if 0
void TmConvertToStandardFormat(struct tm& tmSrc)
{
	// Convert afx internal tm to format expected by runtimes (_tcsftime, etc)
	tmSrc.tm_year -= 1900;  // year is based on 1900
	tmSrc.tm_mon -= 1;      // month of year is 0-based
	tmSrc.tm_wday -= 1;     // day of week is 0-based
	tmSrc.tm_yday -= 1;     // day of year is 0-based
}

double DoubleFromDate(DATE dt)
{
	// No problem if positive
	if (dt >= 0)
		return dt;

	// If negative, must convert since negative dates not continuous
	// (examples: -1.25 to -.75, -1.50 to -.50, -1.75 to -.25)
	double temp = ceil(dt);
	return temp - (dt - temp);
}

DATE DateFromDouble(double dbl)
{
	// No problem if positive
	if (dbl >= 0)
		return dbl;

	// If negative, must convert since negative dates not continuous
	// (examples: -.75 to -1.25, -.50 to -1.50, -.25 to -1.75)
	double temp = floor(dbl); // dbl is now whole part
	return temp + (temp - dbl);
}
#endif // 0

/*
 *  ClearVariant
 *
 *  Zeros a variant structure without regard to current contents
 */
static void ClearVariant(VARIANTARG *pvarg)
{
	pvarg->vt = VT_EMPTY;
	pvarg->wReserved1 = 0;
	pvarg->wReserved2 = 0;
	pvarg->wReserved3 = 0;
	pvarg->lVal = 0;
}

/*
 *  ReleaseVariant
 *
 *  Clears a particular variant structure and releases any external objects
 *  or memory contained in the variant.  Supports the data types listed above.
 */
static void ReleaseVariant(VARIANTARG *pvarg)
{
	VARTYPE vt;
	VARIANTARG _huge *pvargArray;
	long lLBound, lUBound, l;
	
	vt = pvarg->vt & 0xfff;		// mask off flags
	
	// check if an array.  If so, free its contents, then the array itself.
	if (V_ISARRAY(pvarg)) 
	{
		// variant arrays are all this routine currently knows about.  Since a
		// variant can contain anything (even other arrays), call ourselves
		// recursively.
		if (vt == VT_VARIANT) 
		{
			SafeArrayGetLBound(pvarg->parray, 1, &lLBound);
			SafeArrayGetUBound(pvarg->parray, 1, &lUBound);
			
			if (lUBound > lLBound) 
			{
				lUBound -= lLBound;
				
				SafeArrayAccessData(pvarg->parray, (void**)&pvargArray);
				
				for (l = 0; l < lUBound; l++) 
				{
					ReleaseVariant(pvargArray);
					pvargArray++;
				}
				
				SafeArrayUnaccessData(pvarg->parray);
			}
		}
		else 
		{
			wxLogWarning(_T("ReleaseVariant: Array contains non-variant type"));
		}
		
		// Free the array itself.
		SafeArrayDestroy(pvarg->parray);
	}
	else 
	{
		switch (vt) 
		{
			case VT_DISPATCH:
				if (pvarg->pdispVal)
					pvarg->pdispVal->Release();
				break;
				
			case VT_BSTR:
				SysFreeString(pvarg->bstrVal);
				break;
				
			case VT_I2:
			case VT_BOOL:
			case VT_R8:
			case VT_ERROR:		// to avoid erroring on an error return from Excel
				// no work for these types
				break;
				
			default:
				wxLogWarning(_T("ReleaseVariant: Unknown type"));
				break;
		}
	}
	
	ClearVariant(pvarg);
}

#if 0

void ShowException(LPOLESTR szMember, HRESULT hr, EXCEPINFO *pexcep, unsigned int uiArgErr)
{
	TCHAR szBuf[512];
	
	switch (GetScode(hr)) 
	{
		case DISP_E_UNKNOWNNAME:
			wsprintf(szBuf, L"%s: Unknown name or named argument.", szMember);
			break;
	
		case DISP_E_BADPARAMCOUNT:
			wsprintf(szBuf, L"%s: Incorrect number of arguments.", szMember);
			break;
			
		case DISP_E_EXCEPTION:
			wsprintf(szBuf, L"%s: Error %d: ", szMember, pexcep->wCode);
			if (pexcep->bstrDescription != NULL)
				lstrcat(szBuf, pexcep->bstrDescription);
			else
				lstrcat(szBuf, L"<<No Description>>");
			break;
			
		case DISP_E_MEMBERNOTFOUND:
			wsprintf(szBuf, L"%s: method or property not found.", szMember);
			break;
		
		case DISP_E_OVERFLOW:
			wsprintf(szBuf, L"%s: Overflow while coercing argument values.", szMember);
			break;
		
		case DISP_E_NONAMEDARGS:
			wsprintf(szBuf, L"%s: Object implementation does not support named arguments.",
						szMember);
		    break;
		    
		case DISP_E_UNKNOWNLCID:
			wsprintf(szBuf, L"%s: The locale ID is unknown.", szMember);
			break;
		
		case DISP_E_PARAMNOTOPTIONAL:
			wsprintf(szBuf, L"%s: Missing a required parameter.", szMember);
			break;
		
		case DISP_E_PARAMNOTFOUND:
			wsprintf(szBuf, L"%s: Argument not found, argument %d.", szMember, uiArgErr);
			break;
			
		case DISP_E_TYPEMISMATCH:
			wsprintf(szBuf, L"%s: Type mismatch, argument %d.", szMember, uiArgErr);
			break;

		default:
			wsprintf(szBuf, L"%s: Unknown error occured.", szMember);
			break;
	}
	
	wxLogWarning(szBuf);
}

#endif

