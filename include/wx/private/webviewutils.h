///////////////////////////////////////////////////////////////////////////////
// Name:        wx/private/webviewutils.h
// Purpose:     Common code for wxWebView
// Author:      Jose Lorenzo
// Created:     2017-08-12
// Copyright:   (c) 2017 Jose Lorenzo <josee.loren@gmail.com>
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PRIVATE_WEBVIEWUTILS_H_
#define _WX_PRIVATE_WEBVIEWUTILS_H_

#include "wx/regex.h"

// ----------------------------------------------------------------------------
// Common wxWebView code
// ----------------------------------------------------------------------------

class wxWebViewUtils
{
public:

    // This method is used to add a double quote level into a JavasSript code
    // in order to get it working when eval is called programmatically.
    static wxString WrapJavaScript
        (const wxString& javascript, int* runScriptCount, wxString* counter)
    {
        // __wx$counter is used to have a different variable on every
        // RunScript call, to not lose variable values between calls
        *counter = wxString::Format("%i", (*runScriptCount)++);
        wxString javascriptCopy = javascript;

        // Adds one escape level if there is a single quote, double quotes or
        // esacape characters
        wxRegEx escapeDoubleQuotes("(\\\\*)(['\"\n\r\v\t\b\f])");
        escapeDoubleQuotes.Replace(&javascriptCopy,"\\1\\1\\\\\\2");
        
        return wxString::Format("try { var __wx$%s = eval(\"%s\"); true; } \
            catch (e) { e.name + \": \" + e.message; }", *counter, javascriptCopy);;
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxWebViewUtils);
};

#endif // _WX_PRIVATE_WEBVIEWUTILS_H_
