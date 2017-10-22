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
// JS Script Wrapper for wxWebView
// ----------------------------------------------------------------------------

class wxJSScriptWrapper
{
public:
    wxJSScriptWrapper(const wxString& js, int* runScriptCount) : m_jsscript(js)
        : m_escapedCode(js)
    {
        // We assign the return value of JavaScript snippet we execute to the
        // variable with this name in order to be able to access it later if
        // needed.
        //
        // Note that we use a different name for it for each call to
        // RunScript() (which creates a new wxJSScriptWrapper every time) to
        // avoid any possible conflict between different calls.
        m_outputVarName = wxString::Format("__wxOut%i", (*runScriptCount)++);

        // Adds one escape level if there is a single quote, double quotes or
        // escape characters
        wxRegEx escapeDoubleQuotes("(\\\\*)(['\"\n\r\v\t\b\f])");
        escapeDoubleQuotes.Replace(&m_escapedCode,"\\1\\1\\\\\\2");
    }

    // This method is used to add a double quote level into a JavaScript code
    // in order to get it working when eval is called programmatically.
    const wxString GetWrappedCode()
    {
        return wxString::Format("try { var %s = eval(\"%s\"); true; } \
            catch (e) { e.name + \": \" + e.message; }", m_outputVarName, m_escapedCode);
    }

    const wxString GetOutputCode()
    {
#if wxUSE_WEBVIEW && wxUSE_WEBVIEW_WEBKIT && defined(__WXOSX__)
        return wxString::Format("if (typeof %s == 'object') \
                                     JSON.stringify(%s); \
                                 else if (typeof %s == 'undefined') \
                                     'undefined'; \
                                 else \
                                     %s;",
                                 m_outputVarName, m_outputVarName, m_outputVarName, m_outputVarName);
#elif wxUSE_WEBVIEW && wxUSE_WEBVIEW_IE
        return wxString::Format("try {(%s == null || typeof %s != 'object') ? String(%s) : JSON.stringify(%s);} \
		    catch (e) { \
				try \
				{ \
					function __wx$stringifyJSON(obj) \
					{ \
						var objElements = []; \
						if (!(obj instanceof Object)) \
							return typeof obj === \"string\" \
								? \'\"\' + obj + \'\"\' : \'\' + obj; \
						else if (obj instanceof Array) \
						{ \
							if (obj[0] === undefined) \
								return \'[]\'; \
							else \
							{ \
								var arr = []; \
								for (var i = 0; i < obj.length; i++) \
									arr.push(__wx$stringifyJSON(obj[i])); \
								return \'[\' + arr + \']\'; \
							} \
						} \
						else if (typeof obj === \"object\") \
						{ \
							if (obj instanceof Date) \
							{ \
								if (!Date.prototype.toISOString) \
								{ \
									(function() \
									{ \
										function pad(number) \
										{ \
											if (number < 10) \
												return '0' + number; \
											return number; \
										} \
										\
										Date.prototype.toISOString = function() \
										{ \
											return this.getUTCFullYear() + \
											'-' + pad(this.getUTCMonth() + 1) + \
											'-' + pad(this.getUTCDate()) + \
											'T' + pad(this.getUTCHours()) + \
											':' + pad(this.getUTCMinutes()) + \
											':' + pad(this.getUTCSeconds()) + \
											'.' + (this.getUTCMilliseconds() / 1000).toFixed(3).slice(2, 5) + \
											'Z\"'; \
										}; \
										\
									}()); \
								} \
								return '\"' + obj.toISOString(); + '\"' \
							} \
							for (var key in obj) \
							{ \
								if (typeof obj[key] === \"function\") \
									return \'{}\'; \
								else \
									objElements.push(\'\"\' \
									+ key + \'\":\' + \
									__wx$stringifyJSON(obj[key])); \
							} \
							return \'{\' + objElements + \'}\'; \
						} \
					} \
					\
					__wx$stringifyJSON(%s); \
				} \
				catch (e) { e.name + \": \" + e.message; }}",
                m_outputVarName, m_outputVarName, m_outputVarName, m_outputVarName, m_outputVarName);
#else
        return m_outputVarName;
#endif
    }

    const wxString GetCleanUpCode()
    {
        return wxString::Format("%s = undefined;", m_outputVarName);
    }

    const wxString GetOutputJSVariable()
    {
        return m_outputVarName;
    }

private:
    wxString m_escapedCode;
    wxString m_outputVarName;

    wxDECLARE_NO_COPY_CLASS(wxJSScriptWrapper);
};

#endif // _WX_PRIVATE_JSSCRIPTWRAPPER_H_
