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
* Description:  Implementation of wxIfElseVariable Class, Dynamically constructed
* objects representing variables in SSI #if, #else, and #endif directives
*
****************************************************************************/

// For compilers that support precompilation
#include "wx/wxprec.h"
#include "wx/html/forcelnk.h"

// Include private headers
#include "wx/applet/ifelsevar.h"

/*---------------------------- Global variables ---------------------------*/

// Implement the dynamic class so it can be constructed dynamically
IMPLEMENT_ABSTRACT_CLASS(wxIfElseVariable, wxObject);

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
cls     - The String name of the class

RETURNS:
The boolean value of the variable

REMARKS:
To grab a value from any class which is derived from this one simple use this
static function and the name of the derived class to get the value.
This static function is the only function implemented in this base class
basically this is provided for an easier method of grabbing a variable. We
keep all the dynamic object handling in this class to avoid confusing the source
where these are used.

SEE ALSO:
wxIfElsePrep
****************************************************************************/
static bool wxIfElseVariable::FindValue(
    const wxString &cls)
{
    wxObject * tmpclass;

    tmpclass = wxCreateDynamicObject(wxString("wxIfElseVariable") + cls);
    if (!tmpclass) {
#ifdef CHECKED		
        wxMessageBox(wxString("wxHTML #if error: Class not found (") + cls + wxString(")."),"Error",wxICON_ERROR);
#endif
        return wxString("");
        }

    wxIfElseVariable * ev = wxDynamicCast(tmpclass, wxIfElseVariable);
		
    if (!ev) {
#ifdef CHECKED		
        wxMessageBox(wxString("wxHTML #if error: Class is not a valid ifelse variable (") + cls + wxString(")."),"Error",wxICON_ERROR);
#endif		
        return wxString("");
		}

    return ev->GetValue();
}

/*------------------------ Macro Documentation ---------------------------*/

// Here we declare some fake functions to get doc-jet to properly document the macros

#undef BEGIN_IFELSE_VARIABLE
/****************************************************************************
PARAMETERS:
name    - The name of the variable to create

REMARKS:
This macro is used to create variables for use by the #if, #else and #endif
blocks in the HTML preprocessor.
To create a new variable include the code necessary to get the value of the
variable between a block of BEGIN_IFELSE_VARIABLE and END_IFELSE_VARIABLE macros.

EXAMPLE:
BEGIN_IFELSE_VARIABLE(UserName)
	// Get username from nucleus
	bool tmp = GA_HasRegistered();
END_IFELSE_VARIABLE(UserName, tmp)

SEE ALSO:
wxIfElseVariable, wxIfElsePrep, END_IFELSE_VARIABLE, IFELSE_VARIABLE
****************************************************************************/
void BEGIN_IFELSE_VARIABLE(
    const char *name);

#undef END_IFELSE_VARIABLE
/****************************************************************************
PARAMETERS:
name        - The name of the variable to end
returnval   - The boolean value which is the value of the variable

REMARKS:
This macro is used to create variables for use by the #if, #else and #endif
blocks in the HTML preprocessor.
To create a new variable include the code necessary to get the value of the
variable between a block of BEGIN_IFELSE_VARIABLE and END_IFELSE_VARIABLE macros.

EXAMPLE:
BEGIN_IFELSE_VARIABLE(UserName)
	// Get username from nucleus
	bool tmp = GA_HasRegistered();
END_IFELSE_VARIABLE(UserName, tmp)

SEE ALSO:
wxIfElseVariable, wxIfElsePrep, BEGIN_IFELSE_VARIABLE, IFELSE_VARIABLE
****************************************************************************/
void END_IFELSE_VARIABLE(
    const char *name,
    bool returnval);

#undef IFELSE_VARIABLE
/****************************************************************************
PARAMETERS:
name        - The name of the variable
state       - value of the variable

REMARKS:
This macro is used to create constant boolean variables for use by the
#if, #else and #endif blocks in the HTML preprocessor.
This MACRO creates a variable that simply returns the given state and is
not modifiable.

SEE ALSO:
wxIfElseVariable, wxIfElsePrep, BEGIN_IFELSE_VARIABLE, END_IFELSE_VARIABLE
****************************************************************************/
void IFELSE_VARIABLE(
    const char *name,
    bool state);


FORCE_LINK_ME(ifelsevar)
