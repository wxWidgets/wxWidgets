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

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
wxEchoVariable class Definition
****************************************************************************/
class wxEchoVariable : public wxObject {
private:
    DECLARE_ABSTRACT_CLASS(wxEchoVariable);

public:
    wxEchoVariable() : wxObject() {}
    ~wxEchoVariable() {}
	
    /****************************************************************************
    RETURNS:
    The boolean value of the variable

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
    virtual wxString GetValue(const char *parms = NULL) const = 0;


public:
    // static function to retrieve any variable avaliable
    static wxString FindValue(const wxString &cls, const char *parms = NULL);
    };
	
/*--------------------------------- MACROS --------------------------------*/

#define ECHO_PARM (_BEV_parm)
#define BEGIN_ECHO_VARIABLE(name)                                           \
    class wxEchoVariable##name : public wxEchoVariable {                    \
    private:                                                                \
        DECLARE_DYNAMIC_CLASS(wxEchoVariable##name##);                      \
    public:                                                                 \
        wxEchoVariable##name##() : wxEchoVariable() {}                      \
        virtual wxString GetValue(const char *parms = NULL) const;          \
        };                                                                  \
    IMPLEMENT_DYNAMIC_CLASS(wxEchoVariable##name##, wxEchoVariable);        \
    wxString wxEchoVariable##name :: GetValue(const char *parms) const {    \
    wxString _BEV_parm = wxString(parms);

#define END_ECHO_VARIABLE(returnval)                                        \
    return returnval;                                                       \
    }

#define STRING_ECHO_VARIABLE(name, string)                                  \
    BEGIN_ECHO_VARIABLE(##name##);                                          \
    END_ECHO_VARIABLE(wxString(##string##))
	
#endif // __WX_ECHOVAR_H

