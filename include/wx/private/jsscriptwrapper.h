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

    explicit wxJSScriptWrapper(const wxString& js, int* runScriptCount) : m_jsscript(js)
    {
        // __wx$counter is used to have a different variable on every
        // RunScript call, to not lose variable values between calls
        m_wx$counter = wxString::Format("__wx$%i", (*runScriptCount)++);
    }

    // This method is used to add a double quote level into a JavasSript code
    // in order to get it working when eval is called programmatically.
    const wxString GetWrappedCode()
    {
        // Adds one escape level if there is a single quote, double quotes or
        // esacape characters
        wxRegEx escapeDoubleQuotes("(\\\\*)(['\"\n\r\v\t\b\f])");
        escapeDoubleQuotes.Replace(&m_jsscript,"\\1\\1\\\\\\2");

        return wxString::Format("try { var %s = eval(\"%s\"); true; } \
            catch (e) { e.name + \": \" + e.message; }", m_wx$counter, m_jsscript);;
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
                                 m_wx$counter, m_wx$counter, m_wx$counter, m_wx$counter);
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
                m_wx$counter, m_wx$counter, m_wx$counter, m_wx$counter, m_wx$counter);
#else
        return m_wx$counter;
#endif
    }

    const wxString GetCleanUpCode()
    {
        return wxString::Format("%s = undefined;", m_wx$counter);
    }

    const wxString GetOutputJSVariable()
    {
        return m_wx$counter;
    }

private:
    wxString m_jsscript;
    wxString m_wx$counter;

    wxDECLARE_NO_COPY_CLASS(wxJSScriptWrapper);
};

#endif // _WX_PRIVATE_JSSCRIPTWRAPPER_H_
