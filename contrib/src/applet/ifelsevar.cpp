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

// Include private headers
#include "wx/applet/ifelsevar.h"

// wxWindows forcelink macro
#include "wx/html/forcelnk.h"
#include "wx/msgdlg.h"

/*---------------------------- Global variables ---------------------------*/

static wxIfElseVariable *wxIfElseVariable::sm_first = NULL;
static wxHashTable      *wxIfElseVariable::sm_varTable = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
varName         - The String name of the class
getValueFn      - Pointer to the function that returns the echo variable value

REMARKS:
Constructor for the wxIfElseVariable class that self registers itself with
the list of all echo variables when the static class instance is created
at program init time (remember all the constructors get called before
the main program function!).
****************************************************************************/
wxIfElseVariable::wxIfElseVariable(
    const char *varName,
    wxIfElseVariableGetValueFn getValueFn)
{
    m_varName = varName;
    m_getValueFn = getValueFn;
    m_next = sm_first;
    sm_first = this;
}

/****************************************************************************
REMARKS:
Initializes parent pointers and hash table for fast searching for echo
variables.
****************************************************************************/
void wxIfElseVariable::Initialize()
{
    wxIfElseVariable::sm_varTable = new wxHashTable(wxKEY_STRING);

    // Index all class infos by their class name
    wxIfElseVariable *info = sm_first;
    while (info) {
        if (info->m_varName)
            sm_varTable->Put(info->m_varName, info);
        info = info->m_next;
        }
}

/****************************************************************************
REMARKS:
Clean up echo variable hash tables on application exit.
****************************************************************************/
void wxIfElseVariable::CleanUp()
{
    delete wxIfElseVariable::sm_varTable;
    wxIfElseVariable::sm_varTable = NULL;
}

/****************************************************************************
PARAMETERS:
varName       - The String name of the class

REMARKS:
Constructor for the wxIfElseVariable class that self registers itself with
the list of all ifelse variables when the static class instance is created
at program init time (remember all the constructors get called before
the main program function!).
****************************************************************************/
bool wxIfElseVariable::GetValue(
    const wxChar *varName)
{
    wxIfElseVariable *info = wxIfElseVariable::FindVariable(varName);
    if (info) {
        // Return the forced value if the variable has been forced.
        if (info->forced)
            return info->forceVal;
        return info->m_getValueFn();
        }
#ifdef CHECKED		
    wxMessageBox(wxString("wxHTML #if error: Class is not a valid if else variable (") + varName + wxString(")."),"Error",wxICON_ERROR);
#endif		
    return wxString("");
}

/****************************************************************************
PARAMETERS:
varName       - The String name of the class

RETURNS:
True if the if/else variable exists, false if not.
****************************************************************************/
bool wxIfElseVariable::Exists(
    const wxChar *varName)
{
    return wxIfElseVariable::FindVariable(varName) != NULL;
}

/****************************************************************************
PARAMETERS:
varName     - The String name of the class
val         - Value to force the if/else variable with

REMARKS:
Function to forcibly override the value of an if/else variable for
testing purposes. Once the variable has been forced, it will always return
the forced value until the application exists.

NOTE:   This is only available when compiled in CHECKED mode.
****************************************************************************/
void wxIfElseVariable::Force(
    const wxChar *varName,
    bool val)
{
    wxIfElseVariable *info = wxIfElseVariable::FindVariable(varName);
    if (info) {
        info->forced = true;
        info->forceVal = val;
        }
    else {
#ifdef CHECKED		
        wxMessageBox(wxString("wxHTML #if error: Class is not a valid if else variable (") + varName + wxString(")."),"Error",wxICON_ERROR);
#endif
        }		
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

