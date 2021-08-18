///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/jsscriptwrapper.h
// Purpose:     JS Script Wrapper for wxWebView
// Author:      Jose Lorenzo
// Created:     2017-08-12
// Copyright:   (c) 2017 Jose Lorenzo <josee.loren@gmail.com>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_JSSCRIPTWRAPPER_H_
#define _WX_PRIVATE_JSSCRIPTWRAPPER_H_

#include "wx/regex.h"

// ----------------------------------------------------------------------------
// Helper for wxWebView::RunScript()
// ----------------------------------------------------------------------------

// This class provides GetWrappedCode(), GetOutputCode() and GetCleanUpCode()
// functions that should be executed in the backend-appropriate way by each
// wxWebView implementation in order to actually execute the user-provided
// JavaScript code, retrieve its result (if it executed successfully) and
// perform the cleanup at the end.
class wxJSScriptWrapper
{
public:
    wxJSScriptWrapper(const wxString& js, int* runScriptCount)
        : m_escapedCode(js)
    {
        // We assign the return value of JavaScript snippet we execute to the
        // variable with this name in order to be able to access it later if
        // needed.
        //
        // Note that we use a different name for it for each call to
        // RunScript() (which creates a new wxJSScriptWrapper every time) to
        // avoid any possible conflict between different calls.
        m_outputVarName = wxString::Format(wxASCII_STR("__wxOut%i"), (*runScriptCount)++);

        // Adds one escape level.
        const char *charsNeededToBeEscaped = "\\\"\n\r\v\t\b\f";
        for (
            size_t pos = m_escapedCode.find_first_of(charsNeededToBeEscaped, 0);
            pos != wxString::npos;
            pos = m_escapedCode.find_first_of(charsNeededToBeEscaped, pos)
        ) {
            switch (m_escapedCode[pos].GetValue())
            {
            case 0x0A: // '\n'
                m_escapedCode[pos] = 'n';
                break;
            case 0x0D: // '\r'
                m_escapedCode[pos] = 'r';
                break;
            case 0x0B: // '\v'
                m_escapedCode[pos] = 'v';
                break;
            case 0x09: // '\t'
                m_escapedCode[pos] = 't';
                break;
            case 0x08: // '\b'
                m_escapedCode[pos] = 'b';
                break;
            case 0x0C: // '\f'
                m_escapedCode[pos] = 'f';
                break;
            }
            m_escapedCode.insert(pos, '\\');
            pos += 2;
        }
    }

    // Get the code to execute, its returned value will be either boolean true,
    // if it executed successfully, or the exception message if an error
    // occurred.
    //
    // Execute GetOutputCode() later to get the real output after successful
    // execution of this code.
    wxString GetWrappedCode() const
    {
        return wxString::Format
               (
                wxASCII_STR("try { var %s = eval(\"%s\"); true; } "
                "catch (e) { e.name + \": \" + e.message; }"),
                m_outputVarName,
                m_escapedCode
               );
    }

    // Get code returning the result of the last successful execution of the
    // code returned by GetWrappedCode().
    wxString GetOutputCode() const
    {
#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT && defined(__WXOSX__)
        return wxString::Format
               (
                wxASCII_STR("if (typeof %s == 'object') JSON.stringify(%s);"
                "else if (typeof %s == 'undefined') 'undefined';"
                "else %s;"),
                m_outputVarName,
                m_outputVarName,
                m_outputVarName,
                m_outputVarName
               );
#elif wxUSE_WEBVIEW && wxUSE_WEBVIEW_IE
        return wxString::Format
               (
                wxASCII_STR("try {"
                    "(%s == null || typeof %s != 'object') ? String(%s)"
                                                          ": JSON.stringify(%s);"
                "}"
                "catch (e) {"
                    "try {"
                        "function __wx$stringifyJSON(obj) {"
                            "if (!(obj instanceof Object))"
                                "return typeof obj === \"string\""
                                    "? \'\"\' + obj + \'\"\'"
                                    ": \'\' + obj;"
                            "else if (obj instanceof Array) {"
                                "if (obj[0] === undefined)"
                                    "return \'[]\';"
                                "else {"
                                    "var arr = [];"
                                    "for (var i = 0; i < obj.length; i++)"
                                        "arr.push(__wx$stringifyJSON(obj[i]));"
                                    "return \'[\' + arr + \']\';"
                                "}"
                            "}"
                            "else if (typeof obj === \"object\") {"
                                "if (obj instanceof Date) {"
                                    "if (!Date.prototype.toISOString) {"
                                        "(function() {"
                                            "function pad(number) {"
                                                "return number < 10"
                                                    "? '0' + number"
                                                    ": number;"
                                            "}"
                                            "Date.prototype.toISOString = function() {"
                                                "return this.getUTCFullYear() +"
                                                    "'-' + pad(this.getUTCMonth() + 1) +"
                                                    "'-' + pad(this.getUTCDate()) +"
                                                    "'T' + pad(this.getUTCHours()) +"
                                                    "':' + pad(this.getUTCMinutes()) +"
                                                    "':' + pad(this.getUTCSeconds()) +"
                                                    "'.' + (this.getUTCMilliseconds() / 1000)"
                                                            ".toFixed(3).slice(2, 5) + 'Z\"';"
                                            "};"
                                        "}());"
                                    "}"
                                    "return '\"' + obj.toISOString(); + '\"'"
                                "}"
                                "var objElements = [];"
                                "for (var key in obj)"
                                "{"
                                    "if (typeof obj[key] === \"function\")"
                                        "return \'{}\';"
                                    "else {"
                                        "objElements.push(\'\"\'"
                                            "+ key + \'\":\' +"
                                            "__wx$stringifyJSON(obj[key]));"
                                    "}"
                                "}"
                                "return \'{\' + objElements + \'}\';"
                            "}"
                        "}"
                        "__wx$stringifyJSON(%s);"
                    "}"
                    "catch (e) { e.name + \": \" + e.message; }"
                "}"),
                m_outputVarName,
                m_outputVarName,
                m_outputVarName,
                m_outputVarName,
                m_outputVarName
               );
#else
        return m_outputVarName;
#endif
    }

    const wxString& GetUnwrappedOutputCode() { return m_outputVarName; }

    // Execute the code returned by this function to let the output of the code
    // we executed be garbage-collected.
    wxString GetCleanUpCode() const
    {
        return wxString::Format(wxASCII_STR("%s = undefined;"), m_outputVarName);
    }

private:
    wxString m_escapedCode;
    wxString m_outputVarName;

    wxDECLARE_NO_COPY_CLASS(wxJSScriptWrapper);
};

#endif // _WX_PRIVATE_JSSCRIPTWRAPPER_H_
