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
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Header file for the wxQlet class
*
****************************************************************************/

#ifndef __WX_PLUGIN_H
#define __WX_PLUGIN_H

// Forward declaration
class wxHtmlAppletWindow;

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Defines the abstract base class for wxQlet objects.
****************************************************************************/
class wxPlugIn : public wxObject {
private:
    DECLARE_ABSTRACT_CLASS(wxPlugIn);

    wxHtmlAppletWindow *m_parent;
public:
            // Constructor (called during dynamic creation)
            wxPlugIn() { m_parent = NULL; };

            // Psuedo virtual constructor
    virtual bool Create(wxHtmlAppletWindow *parent);

            // Virtual destructor
    virtual ~wxPlugIn();
    };
#endif // __WX_PLUGIN_H

