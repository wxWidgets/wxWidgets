///////////////////////////////////////////////////////////////////////////////
// Name:        wx/mac/corefoundation/cfwrappers.h
// Purpose:     Wrappers around some complex core foundation types
// Author:      Ryan Norton
// Modified by:
// Created:     17/04/2005
// RCS-ID:      $Id$
// Copyright:   (c) 2005 Ryan Norton
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef __WX_CFWRAPPERS_H__
#define __WX_CFWRAPPERS_H__

#include "wx/mac/corefoundation/cfstring.h"
#include "wx/intl.h" //wxLocale for wxCFString

#define wxCF_RELEASE true
#define wxCF_RETAIN  false

// ----------------------------------------------------------------------------
// wxCFDictionary
// ----------------------------------------------------------------------------

class wxCFDictionary
{
public:
    wxCFDictionary(CFTypeRef ref, bool bRetain = wxCF_RELEASE)
    {
        m_cfmdRef = (CFMutableDictionaryRef) ref;
        if(bRetain == wxCF_RETAIN && ref)
            CFRetain(ref);
    }
    
    wxCFDictionary(CFIndex cfiSize = 0)
    {
        CFDictionaryKeyCallBacks kcbs;
        CFDictionaryValueCallBacks vcbs;
        BuildKeyCallbacks(&kcbs);
        BuildValueCallbacks(&vcbs);

        m_cfmdRef = CFDictionaryCreateMutable(
            kCFAllocatorDefault, cfiSize, &kcbs, &vcbs);

    }
    
    ~wxCFDictionary() 
    { Clear();	}
    
    void Clear()
    {if(m_cfmdRef) CFRelease(m_cfmdRef);}
    
    static const void* RetainProc(CFAllocatorRef, const void* v)
    {	return (const void*) CFRetain(v);	}
    
    static void ReleaseProc(CFAllocatorRef, const void* v)
    {	CFRelease(v);	}

    void MakeMutable(CFIndex cfiSize = 0)
    {
        CFDictionaryRef oldref = (CFDictionaryRef) m_cfmdRef;
                
        m_cfmdRef = CFDictionaryCreateMutableCopy(
            kCFAllocatorDefault,
            cfiSize, 
            oldref);
            
        CFRelease(oldref);
    }
    
    void BuildKeyCallbacks(CFDictionaryKeyCallBacks* pCbs)
    {
        pCbs->version = 0;
        pCbs->retain = RetainProc;
        pCbs->release = ReleaseProc;
        pCbs->copyDescription = NULL;
        pCbs->equal = NULL;
        pCbs->hash = NULL;
    }

    void BuildValueCallbacks(CFDictionaryValueCallBacks* pCbs)
    {
        pCbs->version = 0;
        pCbs->retain = RetainProc;
        pCbs->release = ReleaseProc;
        pCbs->copyDescription = NULL;
        pCbs->equal = NULL;
    }

    operator CFTypeRef () const
    {	return (CFTypeRef)m_cfmdRef;	}
    
    CFDictionaryRef GetCFDictionary() const
    {	return (CFDictionaryRef)m_cfmdRef;	}

    CFMutableDictionaryRef GetCFMutableDictionary()
    {	return (CFMutableDictionaryRef) m_cfmdRef;	}

    CFTypeRef operator [] (CFTypeRef cftEntry) const
    {	
        wxASSERT(IsValid());
        return (CFTypeRef) CFDictionaryGetValue((CFDictionaryRef)m_cfmdRef, cftEntry);	
    }    
    
    CFIndex GetCount() const
    {	
        wxASSERT(IsValid());
        return CFDictionaryGetCount((CFDictionaryRef)m_cfmdRef);	
    }
    
    void Add(CFTypeRef cftKey, CFTypeRef cftValue)
    {
        wxASSERT(IsValid());
        wxASSERT(Exists(cftKey) == false);
        CFDictionaryAddValue(m_cfmdRef, cftKey, cftValue);	
    }
    
    void Remove(CFTypeRef cftKey)
    {	
        wxASSERT(IsValid());
        wxASSERT(Exists(cftKey));
        CFDictionaryRemoveValue(m_cfmdRef, cftKey);	
    }
    
    void Set(CFTypeRef cftKey, CFTypeRef cftValue)
    {	
        wxASSERT(IsValid());
        wxASSERT(Exists(cftKey));
        CFDictionarySetValue(m_cfmdRef, cftKey, cftValue);	
    }
    
    bool Exists(CFTypeRef cftKey) const
    {
        wxASSERT(IsValid());
        return CFDictionaryContainsKey((CFDictionaryRef)m_cfmdRef, cftKey) == true;
    }
    
    bool IsOk() const {return m_cfmdRef != NULL;	}

    bool IsValid() const
    {
        return IsOk() && CFGetTypeID((CFTypeRef)m_cfmdRef) == CFDictionaryGetTypeID();
    }
    
    void PrintOut(wxString& sMessage)
    {
        PrintOutDictionary(sMessage, m_cfmdRef);
    }

    static void PrintOutDictionary(wxString& sMessage, CFDictionaryRef cfdRef)
    {
        CFIndex cfiCount = CFDictionaryGetCount(cfdRef);
        CFTypeRef* pKeys = new CFTypeRef[cfiCount];
        CFTypeRef* pValues = new CFTypeRef[cfiCount];

        CFDictionaryGetKeysAndValues(cfdRef, pKeys, pValues);
 
        for(CFIndex i = 0; i < cfiCount; ++i)
        {
            wxString sKey = wxMacCFStringHolder(CFCopyTypeIDDescription(CFGetTypeID(pKeys[i]))).AsString();
            wxString sValue = wxMacCFStringHolder(CFCopyTypeIDDescription(CFGetTypeID(pValues[i]))).AsString();
            
            sMessage << 
                wxString::Format(wxT("[{#%d} Key : %s]"), (int) i,
                        sKey.c_str());
            
            PrintOutType(sMessage, sKey, pKeys[i]);
            
            sMessage << 
                wxString::Format(wxT("\n\t[Value : %s]"), 
                        sValue.c_str());
                        
            PrintOutType(sMessage, sValue, pValues[i]);
            
            sMessage << wxT("\n");
        }
        
        delete[] pKeys;
        delete[] pValues;
    }
    
    static void PrintOutArray(wxString& sMessage, CFArrayRef cfaRef)
    {
    
        for(CFIndex i = 0; i < CFArrayGetCount(cfaRef); ++i)
        {
            wxString sValue = wxMacCFStringHolder(CFCopyTypeIDDescription(CFGetTypeID(
                CFArrayGetValueAtIndex(cfaRef, i)
                ))).AsString();
            
            sMessage << 
                wxString::Format(wxT("\t\t[{#%d} ArrayValue : %s]\n"), (int) i,
                        sValue.c_str());
            
            PrintOutType(sMessage, sValue, CFArrayGetValueAtIndex(cfaRef, i));
        }
    }
    
    static void PrintOutType(wxString& sMessage, wxString sValue, CFTypeRef cfRef)
    {
            sMessage << wxT(" {");
                
            if(sValue == wxT("CFString"))
            {
                 sMessage << wxMacCFStringHolder((CFStringRef)cfRef, false).AsString();
            }
            else if(sValue == wxT("CFNumber"))
            {
                int nOut;
                CFNumberGetValue((CFNumberRef)cfRef, kCFNumberIntType, &nOut);
                sMessage << nOut;
            }
            else if(sValue == wxT("CFDictionary"))
            {
                PrintOutDictionary(sMessage, (CFDictionaryRef)cfRef);
            }
            else if(sValue == wxT("CFArray"))
            {
                PrintOutArray(sMessage, (CFArrayRef)cfRef);
            }
            else if(sValue == wxT("CFBoolean"))
            {
                sMessage << (cfRef == kCFBooleanTrue ? wxT("true") : wxT("false"));
            }
            else if(sValue == wxT("CFURL"))
            {
                sMessage << wxMacCFStringHolder(CFURLCopyPath((CFURLRef) cfRef)).AsString();
            }
            else
            {
                sMessage << wxT("*****UNKNOWN TYPE******");
            }
            
            sMessage << wxT("} ");
    }
    
#if wxUSE_MIMETYPE
    void MakeValidXML();
#endif
    
    CFTypeRef WriteAsXML()
    {
        return CFPropertyListCreateXMLData(kCFAllocatorDefault, m_cfmdRef);
    }
    
    bool ReadAsXML(CFTypeRef cfData, wxString* pErrorMsg = NULL)
    {
        Clear();
        CFStringRef cfsError=NULL;
        m_cfmdRef = (CFMutableDictionaryRef) CFPropertyListCreateFromXMLData(
                    kCFAllocatorDefault,
                    (CFDataRef)cfData,
                    kCFPropertyListMutableContainersAndLeaves,
                    &cfsError	);
        if(cfsError)
        {
            if(pErrorMsg)
                *pErrorMsg = wxMacCFStringHolder(cfsError).AsString();
            else
                CFRelease(cfsError);
        } 

        return m_cfmdRef != NULL;
    }
 private:    
    CFMutableDictionaryRef m_cfmdRef;
};

// ----------------------------------------------------------------------------
// wxCFArray
// ----------------------------------------------------------------------------

class wxCFArray
{
public:
    wxCFArray(CFTypeRef ref, bool bRetain = wxCF_RELEASE)
    {
        m_cfmaRef = (CFMutableArrayRef)ref;
        if(bRetain == wxCF_RETAIN && ref)
            CFRetain(ref);
    }
    
    wxCFArray(CFIndex cfiSize = 0) : m_cfmaRef(NULL)
    {	Create(cfiSize);	}
    
    ~wxCFArray() 
    { Clear();	}
    
    void MakeMutable(CFIndex cfiSize = 0)
    {
        wxASSERT(IsValid());
      
        CFMutableArrayRef oldref = m_cfmaRef;
        m_cfmaRef = CFArrayCreateMutableCopy(
                kCFAllocatorDefault,
                cfiSize, 
                (CFArrayRef)oldref);    
        CFRelease(oldref);
    }
    
    void BuildCallbacks(CFArrayCallBacks* pCbs)
    {
        pCbs->version = 0;
        pCbs->retain = RetainProc;
        pCbs->release = ReleaseProc;
        pCbs->copyDescription = NULL;
        pCbs->equal = NULL;
    }

    void Create(CFIndex cfiSize = 0)
    {
        Clear();
        CFArrayCallBacks cb;
        BuildCallbacks(&cb);
        
        m_cfmaRef = CFArrayCreateMutable(kCFAllocatorDefault, cfiSize, &cb);
    }
    
    void Clear()
    {if(m_cfmaRef) CFRelease(m_cfmaRef);}
    
    static const void* RetainProc(CFAllocatorRef, const void* v)
    {	return (const void*) CFRetain(v);	}
    
    static void ReleaseProc(CFAllocatorRef, const void* v)
    {	CFRelease(v);	}
    
    operator CFTypeRef () const
    {	return (CFTypeRef)m_cfmaRef;	}
    
    CFArrayRef GetCFArray() const
    {	return (CFArrayRef)m_cfmaRef;	}

    CFMutableArrayRef GetCFMutableArray()
    {	return (CFMutableArrayRef) m_cfmaRef;	}

    CFTypeRef operator [] (CFIndex cfiIndex) const
    {	
        wxASSERT(IsValid());
        return (CFTypeRef) CFArrayGetValueAtIndex((CFArrayRef)m_cfmaRef, cfiIndex);	
    }    
    
    CFIndex GetCount()
    {	
        wxASSERT(IsValid());
        return CFArrayGetCount((CFArrayRef)m_cfmaRef);	
    }
    
    void Add(CFTypeRef cftValue)
    {	
        wxASSERT(IsValid());
        CFArrayAppendValue(m_cfmaRef, cftValue);	
    }
    
    void Remove(CFIndex cfiIndex)
    {	
        wxASSERT(IsValid());
        wxASSERT(cfiIndex < GetCount());
        CFArrayRemoveValueAtIndex(m_cfmaRef, cfiIndex);	
    }
    
    void Set(CFIndex cfiIndex, CFTypeRef cftValue)
    {	
        wxASSERT(IsValid());
        wxASSERT(cfiIndex < GetCount());
        CFArraySetValueAtIndex(m_cfmaRef, cfiIndex, cftValue);	
    }

    bool IsOk() const {return m_cfmaRef != NULL;	}

    bool IsValid() const
    {
        return IsOk() && CFGetTypeID((CFTypeRef)m_cfmaRef) == CFArrayGetTypeID();
    }

#if wxUSE_MIMETYPE
    void MakeValidXML();
#endif    
    
private:
    CFMutableArrayRef m_cfmaRef;
};

// ----------------------------------------------------------------------------
// wxCFString
// ----------------------------------------------------------------------------

class wxCFString 
{
public:
    wxCFString(CFTypeRef ref, bool bRetain = wxCF_RELEASE) : m_Holder((CFStringRef)ref, bRetain == wxCF_RELEASE)
    {			}
    
    wxCFString(const wxChar* szString) : m_Holder(wxString(szString), wxLocale::GetSystemEncoding())
    {			}
    
    wxCFString(const wxString& sString) : m_Holder(sString, wxLocale::GetSystemEncoding())
    {			}
    
    operator CFTypeRef() const
    {return (CFTypeRef) ((CFStringRef) m_Holder);	}
    
    bool IsOk() { return ((CFTypeRef)(*this)) != NULL;	}
    
    wxString BuildWXString() {return m_Holder.AsString();	}

private:
    wxMacCFStringHolder m_Holder;
};

// ----------------------------------------------------------------------------
// wxCFNumber
// ----------------------------------------------------------------------------

class wxCFNumber
{
public:
    wxCFNumber(int nValue) 
    {
        m_cfnRef = CFNumberCreate(kCFAllocatorDefault,
									kCFNumberIntType, &nValue);
    }
    
    wxCFNumber(CFTypeRef ref, bool bRetain = wxCF_RELEASE) : m_cfnRef((CFNumberRef)ref)
    {
        if(bRetain == wxCF_RETAIN && ref)
            CFRetain(ref);
    }
    
    ~wxCFNumber()
    {  if(m_cfnRef)         CFRelease(m_cfnRef);	}
    
    
    operator CFTypeRef() const
    {	return (CFTypeRef) m_cfnRef;	}

    int GetValue()
    {	
        int nOut;
        CFNumberGetValue(
				m_cfnRef,	
				kCFNumberIntType,
                &nOut
				);
                
        return nOut;
    }

    bool IsOk() { return m_cfnRef != NULL;	}

private:
    CFNumberRef m_cfnRef;
};

// ----------------------------------------------------------------------------
// wxCFURL
// ----------------------------------------------------------------------------

class wxCFURL
{
public:
    wxCFURL(CFTypeRef ref, bool bRetain = wxCF_RELEASE) : m_cfurlRef((CFURLRef)ref) 
    {
        if(bRetain == wxCF_RETAIN && ref)
            CFRetain(ref);
    }
    wxCFURL(const wxCFString& URLString, CFTypeRef BaseURL = NULL)
    {
        m_cfurlRef = CFURLCreateWithString(
            kCFAllocatorDefault,
            (CFStringRef)(CFTypeRef)URLString,
            (CFURLRef) BaseURL);
    }
    ~wxCFURL() {if(m_cfurlRef) CFRelease(m_cfurlRef);}

    wxString BuildWXString()
    {
        return wxCFString(CFURLCopyPath(m_cfurlRef)).BuildWXString();
    }
    
    operator CFTypeRef() const
    {	return (CFTypeRef)m_cfurlRef;	}
    
    bool IsOk()	{	return m_cfurlRef != NULL;	}
private:
    CFURLRef m_cfurlRef;
};

// ----------------------------------------------------------------------------
// wxCFData
// ----------------------------------------------------------------------------

#define wxCFDATA_RELEASEBUFFER  1
#define wxCFDATA_RETAINBUFFER   0

class wxCFData
{
public:
    wxCFData(CFTypeRef ref, bool bRetain = wxCF_RELEASE) : m_cfdaRef((CFDataRef)ref) 
    {
        if(bRetain == wxCF_RETAIN && ref)
            CFRetain(ref);
    }
    wxCFData(const UInt8* pBytes, CFIndex len, bool bKeep = wxCFDATA_RELEASEBUFFER)
    {
        if(bKeep == wxCFDATA_RELEASEBUFFER)
        {
            m_cfdaRef = CFDataCreateWithBytesNoCopy
                            (kCFAllocatorDefault, pBytes, len, kCFAllocatorDefault);
        }
        else
        {
            m_cfdaRef = CFDataCreate(kCFAllocatorDefault, pBytes, len);
        }
    }
    ~wxCFData() {if(m_cfdaRef) CFRelease(m_cfdaRef);}

    const UInt8* GetValue()
    {
        return CFDataGetBytePtr(m_cfdaRef);
    }
    
    CFIndex GetCount()
    {
        return CFDataGetLength(m_cfdaRef);
    }
    
    operator CFTypeRef() const
    {	return (CFTypeRef)m_cfdaRef;	}
    
    bool IsOk()	{	return m_cfdaRef != NULL;	}
private:
    CFDataRef m_cfdaRef;
};

#endif //__WX_CFWRAPPERS_H__