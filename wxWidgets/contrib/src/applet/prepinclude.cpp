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
* for parsing the <!--#include
*
****************************************************************************/

// Include private headers
#include "wx/applet/prepinclude.h"
#include "wx/applet/echovar.h"

// wxWindows
#include "wx/filesys.h"
#include "wx/msgdlg.h"

/*----------------------------- Implementation ----------------------------*/

#define RECURSE_LIMIT 50

/****************************************************************************
PARAMETERS:
text        - text to process for echo variables

RETURNS:
The string containing the processed filename

REMARKS:
This routine searches through the text of the filename for variables contained
in % percent signs
****************************************************************************/
wxString ParseFilename(
    wxString &text)
{
    int f = 0;
    int e;
    while ((f = text.find('%', f)) != wxString::npos) {
        f++;
        e = text.find('%', f);
#ifdef CHECKED	
        if (e == wxString::npos) {
            wxMessageBox(wxString("wxHTML #include error: % signs should bracket variable names in file attribute. To use a percent sign in a filename write double percents (%%)."), "Error" ,wxICON_ERROR);
            return text;
            }
#endif			
        if (e == f)
            text.replace(f-1, 2, "%");
        else {
            wxString varname = text.Mid(f, (e-f));
            text.replace(f-1, (e-f)+2, wxEchoVariable::GetValue(varname));
            }
        }
    return text;
}

/****************************************************************************
PARAMETERS:
text        - HTML to process for include directives

RETURNS:
The string containing the processed HTML

REMARKS:
This is the only implemented method of the Preprocessor class. It is a constant
function that parses a string. Basically we load the string search for include
statements then replace them with the correct file. Wash rinse and repeat until
all recursive cases are handled.
****************************************************************************/
wxString wxIncludePrep::Process(
    const wxString& text) const
{
    int i;
	char ft[] = "<!--#include virtual=";
    int openedcount = 0;

    // make a copy so we can replace text as we go without affecting the original
	wxString output = text;
	while ((i = (output.Lower()).Find(ft)) != -1) {
		// This loop makes more recursion unnecessary since each iteration adds
		// the new include files to output.
		int n, c;
		wxString fname;

        n = (output.Mid(i+21)).Find("-->");
		
		if (n == -1) {
#ifdef CHECKED			
            wxMessageBox("wxHTML #include error: Could not read filename. Premature end of file.","Error",wxICON_ERROR);
#endif			
            break;
            }

		fname = output.Mid(i+21, n);
		
		// Clip off any quotation marks if they exist. (don't die if there arn't any)
		c = fname.Find("\"");
        if (c != -1) fname = fname.Mid(c+1);
		c = fname.Find("\"");
		if (c != -1) fname = fname.Mid(0, c);
		
		// remove the #include tag
        output.Remove(i, n+21+3);

        wxFSFile * file;
        file = m_FS->OpenFile(ParseFilename(fname));
	
        if (!file) {
#ifdef CHECKED		
			wxMessageBox(wxString("wxHTML #include error: File not Found ") + fname + wxString("."),"Error",wxICON_ERROR);
#endif
            delete file;
            continue;
			}			
							
		wxString tmp;
    	
        do {
			char tmp2[257];
			(file->GetStream())->Read(tmp2, 256);
			c = (file->GetStream())->LastRead();
            tmp2[c] = 0;
			tmp += wxString(tmp2);
			} while (c == 256);

		output = (output.Mid(0,i) + tmp + output.Mid(i));
#ifdef CHECKED	
        if (openedcount > RECURSE_LIMIT) {
            wxMessageBox(wxString("wxHTML #include error: More than RECURSE_LIMIT files have been #included you may have a file that is directly or indirectly including itself, causing an endless loop"), "Error" ,wxICON_ERROR);
            break;
            }
#endif			
        openedcount++;
        delete file;
        }
	
	return output;
}

/****************************************************************************
PARAMETERS:
dir     - Default directory to get included HTML files from

REMARKS:
This function sets the directory to get included HTML files from. The default
value is the current directory. Directorys may be given as a relative path.
****************************************************************************/
void wxIncludePrep::ChangeDirectory(
    wxFileSystem *fs)
{
     m_FS = fs;
}

