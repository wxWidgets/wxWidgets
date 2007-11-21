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

#include "wx/applet/echovar.h"
#include "wx/msgdlg.h"
#include "wx/html/forcelnk.h"

// Include private headers

/*---------------------------- Global variables ---------------------------*/

static wxEchoVariable   *wxEchoVariable::sm_first = NULL;
static wxHashTable      *wxEchoVariable::sm_varTable = NULL;

/*----------------------------- Implementation ----------------------------*/

/****************************************************************************
PARAMETERS:
varName         - The String name of the class
getValueFn      - Pointer to the function that returns the echo variable value

REMARKS:
Constructor for the wxEchoVariable class that self registers itself with
the list of all echo variables when the static class instance is created
at program init time (remember all the constructors get called before
the main program function!).
****************************************************************************/
wxEchoVariable::wxEchoVariable(
    const char *varName,
    wxEchoVariableGetValueFn getValueFn)
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
void wxEchoVariable::Initialize()
{
    wxEchoVariable::sm_varTable = new wxHashTable(wxKEY_STRING);

    // Index all class infos by their class name
    wxEchoVariable *info = sm_first;
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
void wxEchoVariable::CleanUp()
{
    delete wxEchoVariable::sm_varTable;
    wxEchoVariable::sm_varTable = NULL;
}

/****************************************************************************
PARAMETERS:
varName         - The String name of the class
parms           - Parameter string for the echo variable

REMARKS:
Constructor for the wxEchoVariable class that self registers itself with
the list of all echo variables when the static class instance is created
at program init time (remember all the constructors get called before
the main program function!).
****************************************************************************/
wxString wxEchoVariable::GetValue(
    const wxChar *varName,
    const wxChar *parms)
{
    wxEchoVariable *info = wxEchoVariable::FindVariable(varName);
    if (info)
        return info->m_getValueFn(parms);
#ifdef CHECKED		
    wxMessageBox(wxString("wxHTML #echo error: Class is not a valid echo variable (") + varName + wxString(")."),"Error",wxICON_ERROR);
#endif		
    return wxString("");
}

/****************************************************************************
PARAMETERS:
varName       - The String name of the class

RETURNS:
True if the echo variable exists, false if not.
****************************************************************************/
bool wxEchoVariable::Exists(
    const wxChar *varName)
{
    return wxEchoVariable::FindVariable(varName) != NULL;
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

