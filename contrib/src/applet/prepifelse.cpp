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

// Include private headers
#include "wx/applet/prepifelse.h"
#include "wx/applet/ifelsevar.h"
#include "wx/applet/echovar.h"
#include "wx/string.h"

// Force link macro
#include "wx/html/forcelnk.h"
// wxWindows
#include "wx/msgdlg.h"

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
    const wxString &str,
    int start = -1)
{
    wxASSERT( str.GetStringData()->IsValid() );

    // TODO could be made much quicker than that
    int p = tstr.Len()-str.Len()-1;
    int p2 = start-str.Len();

    // if the user supplied a valid start point, use it
    if (start != -1 && p > p2) p = p2;
    while ( p >= 0 ) {
        if ( wxStrncmp(tstr.c_str() + p, str.c_str(), str.Len()) == 0 )
            return p;
        p--;
        }
    return -1;
}

/* {SECRET} */
/****************************************************************************
REMARKS:
tells if a character is a letter.
replace this when wxWindows gets regex library. (without strange licensing
restrictions)
****************************************************************************/
bool IsLetter(
    char c, bool acceptspace = false)
{
    if (acceptspace && (c == ' ')) return true;
    if (c >= '0' && c <= '9') return true;
    return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || c == '\"' || c == '\'' );
}

#define IsQuote(c) (c == '\'' || c == '\"')

/* {SECRET} */
/****************************************************************************
REMARKS:
tells if a character is a letter.
replace this when wxWindows gets regex library. (without strange licensing
restrictions)
****************************************************************************/
wxString GetEquals(
    wxString var,
    wxString value)
{
    if (!wxEchoVariable::Exists(var)) {
        // TODO: when we implement the set variable, check for a set variable as well
        #ifdef CHECKED		
            wxMessageBox(wxString("wxHTML #if\\else error: Variable ") + var + wxString(" not found."),"Error",wxICON_ERROR);
        #endif
        return wxString("0"); // false
        }

    wxString tmp = wxEchoVariable::GetValue(var);

    if (IsQuote( value.GetChar(0) ))
        value = value.Mid(1);
    if (IsQuote(value.GetChar(value.Length()-1)))
        value = value.Mid(0,value.Length()-1);

    if (tmp.CmpNoCase(value) == 0) return wxString("1");
    return wxString("0");
}

/****************************************************************************
PARAMETERS:
str        - text of #if statement

RETURNS:
true or false depending on how it evaluated

REMARKS:
TODO: rewrite this whole thing using regular expressions when they are done.

SEE ALSO:
wxIfElseVariable
****************************************************************************/
bool ParseIfStatementValue(
    wxString &str)
{
    // Find out if the tag has parenthesis
    // recursive to parse the text within the parenthesis,
    // replacing the text with 1 or 0, (hardcoded true or false)
    int b;
    while ((b = str.Find('(')) != -1) {
        int e;
        // Find the matching parenthesis
        int nextbeg, nextend;
        int parencount = 1, min = b+1;
        do {
            nextbeg = str.find('(', min);
            nextend = str.find(')', min);
            if (nextbeg < nextend && nextbeg != wxString::npos) {
                parencount++;
                min = nextbeg+1;
                }
            else {
                parencount--;
                min = nextend+1;
                }

            if (nextend == wxString::npos) {
#ifdef CHECKED		
                wxMessageBox("wxHTML #if\\else error: Unmatched parenthesis in #if expression.","Error",wxICON_ERROR);
#endif
                return true;
                }
            // once parencount reaches 0 again we have found our matchin )
            } while (parencount > 0);

        e = nextend;

        // Extract the expression from the parenthesis block and recurse
        // to solve it.
        wxString tag;
        tag = str.Mid(b+1, e-b-1);
        bool val = ParseIfStatementValue(tag);
        // Add extra spaces just in case of NOT(VAL)
        if (val) str = str.Mid(0, b) + " 1" + str.Mid(e+1);
        else str = str.Mid(0, b) + " 0" + str.Mid(e+1);
        }

    // Remove spaces from left and right
    str.Trim(false);
    str.Trim(true);

    // Convert text method of operators "AND" and "OR" to c style
    // this makes only one special case necessary for each later on
    str.Replace(" AND ", "&&");
    str.Replace(" OR ", "||");
    str.Replace(" EQUALS ", "==");

    // Check for equals statements
    // == statements are special because they are evaluated as a single block
    int equ;
    equ = str.find("==");
    while (equ != wxString::npos) {
        int begin, end;
        int begin2, end2; // ends of words
        begin = equ-1;
        end = equ+2;

        // remove spaces, find extents
        while (end < str.Length() && str.GetChar(end) == ' ')
            end++;
        while (begin >= 0 && str.GetChar(begin) == ' ')
            begin--;
        end2 = end;
        begin2 = begin;
        if (str.GetChar(end2) == '\'' || str.GetChar(end2) == '\"') {
            end2++;
            while (end2 < str.Length() && str.GetChar(end2) != '\'' && str.GetChar(end2) != '\"' )
                end2++;
            end2++;
            }
        else {
            while (end2 < str.Length() && IsLetter(str.GetChar(end2)))
                end2++;
            }
        while (begin >= 0 && IsLetter(str.GetChar(begin)))
            begin--;

        if (begin < 0) begin = 0;
        else begin++;
        if (end2 >= str.Length()) end2 = str.Length();

        wxString tmpeq = GetEquals(str.Mid(begin, begin2-begin+1), str.Mid(end, end2-end));
        str = str.Mid(0, begin) + wxString(" ") + tmpeq + wxString(" ") +
            str.Mid(end2);
        equ = str.find("==");

        // Remove spaces from left and right
        str.Trim(false);
        str.Trim(true);
        }

    // We use ReverseFind so that the whole left expression gets evaluated agains
    // the right single item, creating a left -> right evaluation
    // Search for || operators, recurse to solve (so we don't have to handle special cases here)
    int and, or;
    and = ReverseFind(str, "&&");
    or = ReverseFind(str, "||");
    if ( (and != -1) || (or != -1) ) {
        wxString tag1, tag2;
        // handle the rightmost first to force left->right evaluation
        if ( (and > or) ) {
            return (
                ParseIfStatementValue(tag2 = str.Mid(and+2)) &&
                ParseIfStatementValue(tag1 = str.Mid(0, and)) );
            }
        else {
            return (
                ParseIfStatementValue(tag2 = str.Mid(or+2)) ||
                ParseIfStatementValue(tag1 = str.Mid(0, or)) );
            }
        }

    // By the time we get to this place in the function we are guarenteed to have a single
    // variable operation, perhaps with a NOT or ! operator
    bool notval = false;

    // search for a NOT or ! operator
    if (str.Mid(0, 1) == "!") {
        str.Remove(0, 1);
        str.Trim(false); // trim spaces from left
        notval = true;
        }
    else if (str.Mid(0,4).CmpNoCase("NOT ") == 0) {
        str.Remove(0, 4);
        str.Trim(false); // trim any extra spaces from left
        notval = true;
        }

    // now all we have left is the name of the class or a hardcoded 0 or 1
    if (str == "") {
#ifdef CHECKED		
        wxMessageBox("wxHTML #if\\else error: Empty expression in #if\\#elif statement.","Error",wxICON_ERROR);
#endif
        return true;
        }

    // check for hardcoded 0 and 1 cases, (these are used by parenthesis catcher)
    // this just decomplicates the recursion algorithm
    if (str == "0") return notval;
    if (str == "1") return !notval;

    // Grab the value from the variable class identified by cname
    bool value = wxIfElseVariable::GetValue(str);
    if (notval) value = !value;
    return value;

}
/****************************************************************************
PARAMETERS:
text        - HTML to process for if/else blocks

RETURNS:
The string containing the processed HTML

REMARKS:
This function replaces #if, #else, #elif, and #endif directives with the text
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
	char ftend[] = "<!--#endif-->";
    char ftelse[] = "<!--#else-->";
    char ftnot[] = "<!--#if not ";
    char ftnot2[] = "<!--#if !";
    char ftelif[] = "<!--#elif ";
	
    // make a copy so we can replace text as we go without affecting the original
	wxString output = text;

    // Avoid duplication of our parsing code by turning any #elif blocks into appropriate
    // else/if blocks
    while ((b = ReverseFind(output.Lower(), ftelif)) != -1) {
        int e;
        // Replace beginning of block
        e = output.find("-->", b + strlen(ftelif));

        if (e == wxString::npos) {
#ifdef CHECKED		
            wxMessageBox("wxHTML #elif error: Premature end of file while parsing #elif.","Error",wxICON_ERROR);
#endif
            break;
            }

        // Convert to lower case so find is easy, grab everything after #elif tag
        wxString remains = (output.Mid(e+strlen("-->"))).Lower();

        // find matching else or endif
        int nextif, nextendif;
        int ifcount = 1, min = 0;
        do {
            nextif = remains.find(ft, min);
            nextendif = remains.find(ftend, min);
            if (nextif < nextendif && nextif != wxString::npos) {
                ifcount++;
                min = nextif+1;
                }
            else {
                ifcount--;
                min = nextendif+1;
                }

            if (nextendif == wxString::npos) {
#ifdef CHECKED		
                wxMessageBox("wxHTML #elif error: Premature end of file before finding #endif.","Error",wxICON_ERROR);
#endif
                break;
                }
            // once ifcount reaches 0 again we have found our matchin #endif
            } while (ifcount > 0);

        // If it couldn't be found die gracefully
        if (nextendif == wxString::npos) {
            // We already displayed a message, just break all the way out
            break;
            }

        int elifsize = e - (b + strlen(ftelif)) + strlen("-->");
        // Create the #if/else block, removing the #elif code
        output = output.Mid(0, b) +
            wxString(wxString(ftelse)+wxString(ft)) +
            output.Mid(b+strlen(ftelif), elifsize+nextendif) +
            wxString(ftend) +
            output.Mid(b+strlen(ftelif)+elifsize+nextendif);
        }
	
    // Parse out the if else blocks themselves
    while ((b = ReverseFind(output.Lower(), ft)) != -1) {
		// Loop until every #if directive is found
		// We search from the end of the string so that #if statements will properly recurse
		// and we avoid the hassle of matching statements with the correct <!--#endif-->
        bool notval = false;
        int off = 0;
        int end;
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
        // remove the <!--#if and --> sections from the tag before passing it on to be parsed
        tag = output.Mid(b+strlen(ft), end-strlen(ft)-3);
        output.Remove(b, end);

        value = ParseIfStatementValue(tag);

        // Find the end of the tag (<!--#endif-->) and copy it all into the variable code
        end = ((output.Mid(b)).Lower()).Find(ftend);
        if (end == -1) {
#ifdef CHECKED		
            wxMessageBox("wxHTML #if error: Premature end of file while searching for matching #endif.","Error",wxICON_ERROR);
#endif
            break;
			}

        code = output.Mid(b, end);
        output.Remove(b, end+strlen(ftend)); // remove the entire #if block from original document

        // Find out if there is an else statement
        end = (code.Lower()).Find(ftelse);
        if (end != -1) {
            if (!value) {
                // Use the else statement
                usecode = code.Mid(end+strlen(ftelse));
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
				
