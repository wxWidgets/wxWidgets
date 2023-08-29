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
    enum OutputType
    {
        JS_OUTPUT_STRING, // All return types are converted to a string
        JS_OUTPUT_WEBKIT, // Some return types will be processed
        JS_OUTPUT_IE, // Most return types will be processed
        JS_OUTPUT_RAW // The return types is returned as is
    };

    wxJSScriptWrapper(const wxString& js, OutputType outputType)
        : m_outputType(outputType)
    {
        // Adds one escape level.
        const char *charsNeededToBeEscaped = "\\\"\n\r\v\t\b\f";
        m_escapedCode.reserve(js.size());
        for (wxString::const_iterator it = js.begin(); it != js.end(); ++it)
        {
            if (wxStrchr(charsNeededToBeEscaped, *it))
            {
                m_escapedCode += '\\';
                switch ((wxChar) *it)
                {
                case 0x0A: // '\n'
                    m_escapedCode += 'n';
                    break;
                case 0x0D: // '\r'
                    m_escapedCode += 'r';
                    break;
                case 0x0B: // '\v'
                    m_escapedCode += 'v';
                    break;
                case 0x09: // '\t'
                    m_escapedCode += 't';
                    break;
                case 0x08: // '\b'
                    m_escapedCode += 'b';
                    break;
                case 0x0C: // '\f'
                    m_escapedCode += 'f';
                    break;
                default:
                    m_escapedCode += *it;
                }
            }
            else
                m_escapedCode += *it;
        }
    }

    // Get the code to execute, its returned value will be either the value,
    // if it executed successfully, or the exception message prefixed with
    // "__wxexc:" if an error occurred.
    //
    // Either use SetOutput() to specify the script result or access it directly
    // Using GetOutputRef()
    //
    // Execute ExtractOutput() later to get the real output after successful
    // execution of this code or the proper error message.
    wxString GetWrappedCode() const
    {
        wxString code = wxString::Format(
            wxASCII_STR("(function () { try { var res = eval(\"%s\"); "),
            m_escapedCode);

        switch (m_outputType)
        {
            case JS_OUTPUT_STRING:
                code += wxASCII_STR(
                            "if (typeof res == 'object') return JSON.stringify(res);"
                            "else if (typeof res == 'undefined') return 'undefined';"
                            "else return String(res);"
                        );
                break;
            case JS_OUTPUT_WEBKIT:
                code += wxASCII_STR(
                            "if (typeof res == 'object') return JSON.stringify(res);"
                            "else if (typeof res == 'undefined') return 'undefined';"
                            "else return res;"
                        );
                break;
            case JS_OUTPUT_IE:
                code += wxASCII_STR(
                    "try {"
                    "return (res == null || typeof res != 'object') ? String(res)"
                               ": JSON.stringify(res);"
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
                         "return __wx$stringifyJSON(res);"
                     "}"
                     "catch (e) { return \"__wxexc:\" + e.name + \": \" + e.message; }"
                    "}");
                break;
            case JS_OUTPUT_RAW:
                code += wxASCII_STR("return res;");
                break;
        }

        code +=
            wxASCII_STR("} catch (e) { return \"__wxexc:\" + e.name + \": \" + e.message; }"
                        "})()");
        return code;
    }

    // Extract the output value
    //
    // Returns true if executed successfully
    //    string of the result will be put into output
    // Returns false when an exception occurred
    //    string will be the exception message
    static bool ExtractOutput(const wxString& result, wxString* output)
    {
        if (output)
            *output = result;

        if (result.starts_with(wxASCII_STR("__wxexc:")))
        {
            if (output)
                output->Remove(0, 8);
            return false;
        }
        else
        {
            return true;
        }
    }

private:
    wxString m_escapedCode;
    OutputType m_outputType;

    wxDECLARE_NO_COPY_CLASS(wxJSScriptWrapper);
};

#endif // _WX_PRIVATE_JSSCRIPTWRAPPER_H_
