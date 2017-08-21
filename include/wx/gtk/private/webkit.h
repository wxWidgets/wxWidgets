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

#include <webkit2/webkit2.h>

// ----------------------------------------------------------------------------
// Convenience class for WebKitJavascriptResult on scope exit automatically
// ----------------------------------------------------------------------------

class wxWebKitJavascriptResult
{
public:
    explicit wxWebKitJavascriptResult(WebKitJavascriptResult *r) : m_jsresult(r) { }
    ~wxWebKitJavascriptResult() { webkit_javascript_result_unref (m_jsresult); }

    operator WebKitJavascriptResult *() const { return m_jsresult; }

private:
    WebKitJavascriptResult *m_jsresult;

    wxDECLARE_NO_COPY_CLASS(wxWebKitJavascriptResult);
};

#include <JavaScriptCore/JSStringRef.h>

class wxJSStringRef
{
public:
    explicit wxJSStringRef(JSStringRef r) : m_jssref(r) { }
    ~wxJSStringRef() { JSStringRelease (m_jssref); }
   
    const wxString ToWxString()
    {
        gsize length = JSStringGetMaximumUTF8CStringSize (m_jssref);
        wxGtkString str(g_new(gchar, length));

        JSStringGetUTF8CString(m_jssref, &*str, length);

        return wxString::FromUTF8(str);
    }

private:
    JSStringRef m_jssref;

    wxDECLARE_NO_COPY_CLASS(wxJSStringRef);
};

#endif // _WX_GTK_PRIVATE_WEBKIT_H_
