///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/private/webkit.h
// Purpose:     wxWebKitGtk RAII wrappers declaration
// Author:      Jose Lorenzo
// Created:     2017-08-21
// Copyright:   (c) 2017 Jose Lorenzo <josee.loren@gmail.com>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GTK_PRIVATE_WEBKIT_H_
#define _WX_GTK_PRIVATE_WEBKIT_H_

#include "wx/buffer.h"

#include <webkit2/webkit2.h>
#include <JavaScriptCore/JSStringRef.h>

// ----------------------------------------------------------------------------
// RAII wrapper of WebKitJavascriptResult taking care of freeing it
// ----------------------------------------------------------------------------

class wxWebKitJavascriptResult
{
public:
    explicit wxWebKitJavascriptResult(WebKitJavascriptResult *r)
        : m_jsresult(r)
    {
    }

    ~wxWebKitJavascriptResult()
    {
        webkit_javascript_result_unref(m_jsresult);
    }

    operator WebKitJavascriptResult *() const { return m_jsresult; }

private:
    WebKitJavascriptResult *m_jsresult;

    wxDECLARE_NO_COPY_CLASS(wxWebKitJavascriptResult);
};

// ----------------------------------------------------------------------------
// RAII wrapper of JSStringRef, also providing conversion to wxString
// ----------------------------------------------------------------------------

class wxJSStringRef
{
public:
    explicit wxJSStringRef(JSStringRef r) : m_jssref(r) { }
    ~wxJSStringRef() { JSStringRelease(m_jssref); }

    wxString ToWxString() const
    {
        const size_t length = JSStringGetMaximumUTF8CStringSize(m_jssref);

        wxCharBuffer str(length);

        JSStringGetUTF8CString(m_jssref, str.data(), length);

        return wxString::FromUTF8(str);
    }

private:
    JSStringRef m_jssref;

    wxDECLARE_NO_COPY_CLASS(wxJSStringRef);
};

#endif // _WX_GTK_PRIVATE_WEBKIT_H_
