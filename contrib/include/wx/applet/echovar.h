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
* Description:  Header file for wxEchoVariable Class, Dynamically constructed
* objects representing variables in SSI #echo directive
*
****************************************************************************/

#ifndef __WX_ECHOVAR_H
#define __WX_ECHOVAR_H

#include "wx/object.h"
#include "wx/hash.h"

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
RETURNS:
The string value of the variable

PARAMETERS:
parms   - Optional parameter string passed from parm= field in HTML

REMARKS:
To create new variables for the #echo HTML preprocessing directives
you need to derive classes from wxEchoVariable and override the
pure virtual GetValue function. However this should not be done directly
but by using the BEGIN_ECHO_VARIABLE and END_ECHO_VARIABLE macros

SEE ALSO:
wxEchoPrep, BEGIN_ECHO_VARIABLE, END_ECHO_VARIABLE
****************************************************************************/
typedef wxString (*wxEchoVariableGetValueFn)(const char *parms);

/****************************************************************************
REMARKS:
wxEchoVariable class Definition
****************************************************************************/
class wxEchoVariable : public wxObject {
protected:
    const wxChar                *m_varName;
    wxEchoVariableGetValueFn    m_getValueFn;
    static wxEchoVariable       *sm_first;
    wxEchoVariable              *m_next;
    static wxHashTable          *sm_varTable;

    static inline wxEchoVariable *wxEchoVariable::FindVariable(const wxChar *varName);

public:
    // Constructor to create the echo variable and register the class
    wxEchoVariable(
        const char *varName,
        wxEchoVariableGetValueFn getValueFn);

    // Member variable access functions
    const wxChar *GetClassName() const          { return m_varName; }
    wxEchoVariableGetValueFn GetValueFn() const { return m_getValueFn; }
    static const wxEchoVariable* GetFirst()     { return sm_first; }
    const wxEchoVariable* GetNext() const       { return m_next; }
	
    // Static functions to retrieve any variable avaliable
    static wxString GetValue(const wxChar *varName,const wxChar *parms = NULL);
    static bool Exists(const wxChar *varName);

    // Initializes parent pointers and hash table for fast searching.
    static void Initialize();

    // Cleans up hash table used for fast searching.
    static void CleanUp();
    };
	
/****************************************************************************
PARAMETERS:
class   - Name of class for echo variable to find

RETURNS:
Pointer to the echo variable class

REMARKS:
Inline helper function to find the echo variable from it's class name.
****************************************************************************/
inline wxEchoVariable *wxEchoVariable::FindVariable(
    const wxChar *varName)
{
    if (sm_varTable)
        return (wxEchoVariable*)sm_varTable->Get(varName);
    else {
        wxEchoVariable *info = sm_first;
        while (info) {
            if (info->m_varName && wxStrcmp(info->m_varName, varName) == 0)
                return info;
            info = info->m_next;
            }
        return NULL;
        }
}

/*--------------------------------- MACROS --------------------------------*/

#define BEGIN_ECHO_VARIABLE(name)                                           \
wxString wxEchoVariableFn##name(const char *parms);                         \
wxEchoVariable wxEchoVariable##name(#name,wxEchoVariableFn##name);          \
wxString wxEchoVariableFn##name(const char *parms) {                        \
    wxString _BEV_parm = wxString(parms);

#define END_ECHO_VARIABLE(returnval)                                        \
    return returnval;                                                       \
    }

#define STRING_ECHO_VARIABLE(name, string)                                  \
    BEGIN_ECHO_VARIABLE(##name##);                                          \
    END_ECHO_VARIABLE(wxString(##string##))
	
#endif // __WX_ECHOVAR_H

