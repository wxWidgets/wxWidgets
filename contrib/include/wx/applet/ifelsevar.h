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

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
This class is used to create variables for the HTML preprocessor #if, #else,
and #endif directives.

SEE ALSO:
wxIfElsePrep
****************************************************************************/
class wxIfElseVariable : public wxObject {
private:
    DECLARE_ABSTRACT_CLASS(wxIfElseVariable);

public:
    wxIfElseVariable() : wxObject() {}
    ~wxIfElseVariable() {}
	
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
    virtual bool GetValue() const = 0;


public:
    // static function to retrieve any variable avaliable
    static bool FindValue(const wxString &cls);
    };

/*--------------------------------- MACROS --------------------------------*/

#define BEGIN_IFELSE_VARIABLE(name)                                         \
    class wxIfElseVariable##name : public wxIfElseVariable {                \
    private:                                                                \
        DECLARE_DYNAMIC_CLASS(wxIfElseVariable##name##);                    \
    public:                                                                 \
        wxIfElseVariable##name##() : wxIfElseVariable() {}                  \
        virtual bool GetValue() const;                                      \
        };                                                                  \
    IMPLEMENT_DYNAMIC_CLASS(wxIfElseVariable##name##, wxIfElseVariable);    \
    bool wxIfElseVariable##name :: GetValue() const {

#define END_IFELSE_VARIABLE(returnval)                                      \
    return returnval;                                                       \
    }

#define IFELSE_VARIABLE(name, state)                                        \
    BEGIN_IFELSE_VARIABLE(##name##);                                        \
    END_IFELSE_VARIABLE(bool (state))

#endif // __WX_IFELSEVAR_H

