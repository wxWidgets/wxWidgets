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
* Description:  Implementation of wxEchoVariable Class, Dynamically constructed
* objects representing variables in SSI #echo directive
*
****************************************************************************/

// For compilers that support precompilation
#include "wx/wxprec.h"
#include "wx/html/forcelnk.h"

// Include private headers
#include "wx/applet/echovar.h"

/*---------------------------- Global variables ---------------------------*/

// Implement the dynamic class so it can be constructed dynamically
IMPLEMENT_ABSTRACT_CLASS(wxEchoVariable, wxObject);

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
cls     - The String name of the class
parms   - an optional parameter string to pass off to the child class

RETURNS:
The string value of the variable

REMARKS:
To grab a value from any class which is derived from this one simple use this
static function and the name of the derived class to get the value.
This static function is the only function implemented in this base class
basically this is provided for an easier method of grabbing a variable. We
keep all the dynamic object handling in this class to avoid confusing the source
where these are used.

SEE ALSO:
wxEchoPrep
****************************************************************************/
static wxString wxEchoVariable::FindValue(
    const wxString &cls,
    const char *parms)
{
    wxObject * tmpclass;

    tmpclass = wxCreateDynamicObject(wxString("wxEchoVariable") + cls);
    if (!tmpclass) {
#ifdef CHECKED		
        wxMessageBox(wxString("wxHTML #echo error: Class not found (") + cls + wxString(")."),"Error",wxICON_ERROR);
#endif
        return wxString("");
        }

    wxEchoVariable * ev = wxDynamicCast(tmpclass, wxEchoVariable);
		
    if (!ev) {
#ifdef CHECKED		
        wxMessageBox(wxString("wxHTML #echo error: Class is not a valid echo variable (") + cls + wxString(")."),"Error",wxICON_ERROR);
#endif		
        return wxString("");
		}

    return ev->GetValue(parms);
}


/*------------------------ Macro Documentation ---------------------------*/

// Here we declare some fake functions to get doc-jet to properly document the macros

#undef ECHO_PARM
/****************************************************************************
RETURNS:
The value of the parameter string from the HTML parm= field

REMARKS:
This is a macro to retrieve the parameter string passed in the parm= field.
Use this macro to get the correct variable within a BEGIN_ECHO_VARIABLE and
END_ECHO_VARIABLE block.

SEE ALSO:
wxEchoVariable, wxEchoPrep, BEGIN_ECHO_VARIABLE, END_ECHO_VARIABLE
****************************************************************************/
wxString ECHO_PARM();


#undef BEGIN_ECHO_VARIABLE
/****************************************************************************
PARAMETERS:
name    - The name of the variable to create

REMARKS:
This macro is used to create variables for use by the #echo directive
the HTML preprocessor.
To create a new variable include the code necessary to get the value of the
variable between a block of BEGIN_ECHO_VARIABLE and END_ECHO_VARIABLE macros.
Use the ECHO_PARM macro to grab the optional parameter string passed from the
'parm=' field in the html file.

EXAMPLE:
BEGIN_ECHO_VARIABLE(UserName)
	// Get username from nucleus
	wxString tmp = GA_GetUserName();
END_ECHO_VARIABLE(UserName, tmp)

SEE ALSO:
wxEchoVariable, wxEchoPrep, END_ECHO_VARIABLE, ECHO_PARM, STRING_ECHO_VARIABLE
****************************************************************************/
void BEGIN_ECHO_VARIABLE(
    const char *name);

#undef END_ECHO_VARIABLE
/****************************************************************************
PARAMETERS:
name        - The name of the variable to end
returnval   - The value which should be sent back as the value of the variable

REMARKS:
This macro is used to create variables for use by the #echo directive
the HTML preprocessor.
To create a new variable include the code necessary to get the value of the
variable between a block of BEGIN_ECHO_VARIABLE and END_ECHO_VARIABLE macros.

EXAMPLE:
BEGIN_ECHO_VARIABLE(UserName)
	// Get username from nucleus
	wxString tmp = GA_GetUserName();
END_ECHO_VARIABLE(UserName, tmp)

SEE ALSO:
wxEchoVariable, wxEchoPrep, BEGIN_ECHO_VARIABLE, ECHO_PARM, STRING_ECHO_VARIABLE
****************************************************************************/
void END_ECHO_VARIABLE(
    const char *name,
    wxString returnval);

#undef STRING_ECHO_VARIABLE
/****************************************************************************
PARAMETERS:
name        - The name of the variable
returnval   - String to return as the value of the variable

REMARKS:
This macro is used to create constant string variables for use by the #echo
directive in the HTML preprocessor.
This MACRO creates a variable that simply returns the given string and is
not modifiable.

SEE ALSO:
wxEchoVariable, wxEchoPrep, BEGIN_ECHO_VARIABLE, END_ECHO_VARIABLE
****************************************************************************/
void STRING_ECHO_VARIABLE(
    const char *name,
    wxString string);

// hack to make this file link
FORCE_LINK_ME(echovar)				
