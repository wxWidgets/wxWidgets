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
* Description:  Header file for wxIfElseVariable Class, Dynamically constructed
* objects representing variables in SSI #if, #else and #endif directives
*
****************************************************************************/

#ifndef __WX_IFELSEVAR_H
#define __WX_IFELSEVAR_H

#include "wx/object.h"
#include "wx/hash.h"

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
RETURNS:
The boolean value of the variable

REMARKS:
To create new variables for the #if, #else and #endif HTML preprocessing
blocks you need to derive classes from wxIfElseVariable and override the
pure virtual GetValue function. However this should not be done directly
but by using the BEGIN_IFELSE_VARIABLE and END_IFELSE_VARIABLE macros

SEE ALSO:
wxIfElsePrep, BEGIN_IFELSE_VARIABLE, END_IFELSE_VARIABLE
****************************************************************************/
typedef bool (*wxIfElseVariableGetValueFn)();

/****************************************************************************
REMARKS:
wxIfElseVariable class Definition
****************************************************************************/
class wxIfElseVariable : public wxObject {
protected:
    const wxChar                *m_varName;
    wxIfElseVariableGetValueFn  m_getValueFn;
    static wxIfElseVariable     *sm_first;
    wxIfElseVariable            *m_next;
    static wxHashTable          *sm_varTable;
    bool                        forced;
    bool                        forceVal;

    static inline wxIfElseVariable *wxIfElseVariable::FindVariable(const wxChar *varName);

public:
    // Constructor to create the echo variable and register the class
    wxIfElseVariable(
        const char *varName,
        wxIfElseVariableGetValueFn getValueFn);

    // Member variable access functions
    const wxChar *GetClassName() const            { return m_varName; }
    wxIfElseVariableGetValueFn GetValueFn() const { return m_getValueFn; }
    static const wxIfElseVariable* GetFirst()     { return sm_first; }
    const wxIfElseVariable* GetNext() const       { return m_next; }
	
    // Static functions to retrieve any variable avaliable
    static bool GetValue(const wxChar *varName);
    static bool Exists(const wxChar *varName);
    static void Force(const wxChar *varName, bool val);

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
inline wxIfElseVariable *wxIfElseVariable::FindVariable(
    const wxChar *varName)
{
    if (sm_varTable)
        return (wxIfElseVariable*)sm_varTable->Get(varName);
    else {
        wxIfElseVariable *info = sm_first;
        while (info) {
            if (info->m_varName && wxStrcmp(info->m_varName, varName) == 0)
                return info;
            info = info->m_next;
            }
        return NULL;
        }
}

/*--------------------------------- MACROS --------------------------------*/

#define BEGIN_IFELSE_VARIABLE(name)                                         \
bool wxIfElseVariableFn##name();                                            \
wxIfElseVariable wxIfElseVariable##name(#name,wxIfElseVariableFn##name);    \
bool wxIfElseVariableFn##name() {                                           \

#define END_IFELSE_VARIABLE(returnval)                                      \
    return returnval;                                                       \
    }

#define IFELSE_VARIABLE(name, state)                                        \
    BEGIN_IFELSE_VARIABLE(##name##);                                        \
    END_IFELSE_VARIABLE(bool (state))

#endif // __WX_IFELSEVAR_H

