/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ========================================================================
*
*    The contents of this file are subject to the wxWindows License
*    Version 3.0 (the "License"); you may not use this file except in
*    compliance with the License. You may obtain a copy of the License at
*    http://www.wxwindows.org/licence3.txt
*
*    Software distributed under the License is distributed on an
*    "AS IS" basis, WITHOUT WARRANTY OF ANY KIND, either express or
*    implied. See the License for the specific language governing
*    rights and limitations under the License.
*
*  ========================================================================
*
* Language:		ANSI C++
* Environment:	Any
*
* Description:  This file is the implementation of the Preprocessor object
*               for parsing the <!--#echo directive
*
****************************************************************************/

// Include private headers
#include "wx/applet/prepecho.h"
#include "wx/applet/echovar.h"

// Force Link macro
#include "wx/html/forcelnk.h"

// wxWindows headers
#include "wx/msgdlg.h"

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
text        - HTML to process for echo directives

RETURNS:
The string containing the processed HTML

REMARKS:
This function replaces #echo directives with a variable retrieved from the
class given in the HTML directive. These classes are created by making a sub
class of wxEchoVariable. Dynamic class construction is used at run time
internally to create an instance of this class and access the value of the
variable.

SEE ALSO:
wxEchoVariable
****************************************************************************/
wxString wxEchoPrep::Process(
    const wxString& text) const
{
	int i;
	char ft[] = "<!--#echo ";
	
	// make a copy so we can replace text as we go without affecting the original
	wxString output = text;
	
	while ((i = (output.Lower()).Find(ft)) != -1) {
		// Loop until every #echo directive is found
		int n, c, end;
        wxString cname, parms;
        wxString tag;

        // grab the tag and remove it from the file
        end = (output.Mid(i)).Find("-->");
        if (end == -1) {
#ifdef CHECKED		
            wxMessageBox("wxHTML #echo error: Premature end of file while parsing #echo.","Error",wxICON_ERROR);
#endif
            break;
			}

        end += 3;
        tag = output.Mid(i, end);
        output.Remove(i, end);

        n = (tag.Lower()).Find(" parm=");
        c = tag.Find("-->");
        if (n == -1) {
            n = c;
            // find the classname
            c = (tag.Mid(10, n-10)).Find(" ");
	    	if (c == -1) n -= 10;
            else n = c;
            cname = tag.Mid(10, n);

            // grab the value from the class, put it in tag since the data is no longer needed
            tag = wxEchoVariable::GetValue(cname, NULL);
            }
        else {
            // Find the parms
            parms = tag.Mid(n+6, c-n-6);
      		// Clip off any quotation marks if they exist. (don't die if there arn't any)
           	c = parms.Find("\"");
            if (c != -1) parms = parms.Mid(c+1);
    		c = parms.Find("\"");
	    	if (c != -1) parms = parms.Mid(0, c);
            // find the classname
            c = (tag.Mid(10, n-10)).Find(" ");
	    	if (c == -1) n -= 10;
            else n = c;
            cname = tag.Mid(10, n);

            // grab the value from the class, put it in tag since the data is no longer needed
            tag = wxEchoVariable::GetValue(cname, parms.c_str());
            }
        // remove ampersands and <> chars
        tag.Replace("&", "&amp;");
        tag.Replace("<", "&lt;");
        tag.Replace(">", "&gt;");

        output = (output.Mid(0,i) + tag + output.Mid(i));
		}
    return output;
}

FORCE_LINK(echovar)
			
