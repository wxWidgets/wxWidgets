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
*               for parsing the <!--#if directive
*
****************************************************************************/

// For compilers that support precompilation
#include "wx/wxprec.h"
#include "wx/html/forcelnk.h"

// Include private headers
#include "wx/applet/prepifelse.h"
#include "wx/applet/ifelsevar.h"

/*---------------------------- Global variables ---------------------------*/


/*----------------------------- Implementation ----------------------------*/

/* {SECRET} */
/****************************************************************************
REMARKS:
None of the Reverse Find functions in wxWindows appear to work in a way that
can be used by our code. This includes the libstr rfind implementations which
do not correctly pass the given return value.
****************************************************************************/
int ReverseFind(
    const wxString &tstr,
    const wxString &str)
{
    wxASSERT( str.GetStringData()->IsValid() );

    // TODO could be made much quicker than that
    int p = tstr.Len()-str.Len()-1;
    while ( p >= 0 ) {
        if ( wxStrncmp(tstr.c_str() + p, str.c_str(), str.Len()) == 0 )
            return p;
        p--;
        }

    return -1;
}

/****************************************************************************
PARAMETERS:
text        - HTML to process for if/else blocks

RETURNS:
The string containing the processed HTML

REMARKS:
This function replaces #if, #else, and #endif directives with the text
contained within the blocks, dependant on the value of the given boolean
variable. The variable is created by making a sub class of wxIfElseVariable.
Dynamic class construction is used at run time internally to create an instance
of this class and access the value of the variable.

SEE ALSO:
wxIfElseVariable
****************************************************************************/
wxString wxIfElsePrep::Process(
    const wxString& text) const
{
	int b;
	char ft[] = "<!--#if ";
	
	// make a copy so we can replace text as we go without affecting the original
	wxString output = text;
    while ((b = ReverseFind(output.Lower(), ft)) != -1) {
		// Loop until every #echo directive is found
		// We search from the end of the string so that #if statements will properly recurse
		// and we avoid the hassle of matching statements with the correct <!--#endif-->
		int end, c, n;
        wxString usecode, code;
        wxString cname;
        wxString tag;
        bool value;

        code = wxString("");

        // grab the tag and get the name of the variable
        end = (output.Mid(b)).Find("-->");
        if (end == -1) {
#ifdef CHECKED		
            wxMessageBox("wxHTML #if error: Premature end of file while parsing #if.","Error",wxICON_ERROR);
#endif
            break;
			}

        end += 3;
        tag = output.Mid(b, end);
        output.Remove(b, end);

        c = tag.Find("-->");
        n = c;

        // find the classname
        c = (tag.Mid(8, n-8)).Find(" ");
	    if (c == -1) n -= 8;
        else n = c;
        cname = tag.Mid(8, n);

        cname.Trim(false);
        c = cname.Find("\"");
        if (c != -1) cname = cname.Mid(c+1);
    	c = cname.Find("\"");
	    if (c != -1) cname = cname.Mid(0, c);

        // Grab the value from the variable class identified by cname
        value = wxIfElseVariable::FindValue(cname);

        // Find the end of the tag (<!--#endif-->) and copy it all into the variable code
        end = ((output.Mid(b)).Lower()).Find("<!--#endif-->");
        if (end == -1) {
#ifdef CHECKED		
            wxMessageBox("wxHTML #if error: Premature end of file while searching for matching #endif.","Error",wxICON_ERROR);
#endif
            break;
			}

        code = output.Mid(b, end);
        output.Remove(b, end+13); // remove the entire #if block from original document

        // Find out if there is an else statement
        end = (code.Lower()).Find("<!--#else-->");
        if (end != -1) {
            if (!value) {
                // Use the else statement
                usecode = code.Mid(end+12);
                }
            else {
                // Use statement before #else
                usecode = code.Mid(0, end);
                }
            }
        else if (value) {
            // There is no #else statement
            usecode = code;
            }

        if (usecode != wxString(""))
            output = (output.Mid(0,b) + usecode + output.Mid(b));
		}

    return output;
}

FORCE_LINK(ifelsevar)				
