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
    static wxString createVariableWithJavaScriptResult (const wxString& javascript, int* runScriptCount, wxString* counter) 
    {
        *counter = wxString::Format("%i", (*runScriptCount)++);
        wxString javascriptCopy = javascript;

        wxRegEx escapeDoubleQuotes("(\\\\*)(['\"\n\r\v\t\b\f])");
        escapeDoubleQuotes.Replace(&javascriptCopy,"\\1\\1\\\\\\2");
        
        return "try { var __wx$" + *counter + " = eval(\"" +
                javascriptCopy +
                "\"); true; } catch (e) { e.name + \": \" + e.message; }";
    }

private:
    wxDECLARE_NO_COPY_CLASS(wxWebViewUtils);
};

#endif // _WX_PRIVATE_WEBVIEWUTILS_H_
