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
* Description:  Main wxPlugIn class implementation
*
****************************************************************************/

// Include private headers
#include "wx/applet/plugin.h"
#include "wx/applet/window.h"

/*------------------------- Implementation --------------------------------*/

// Implement the abstract class functions
IMPLEMENT_ABSTRACT_CLASS(wxPlugIn, wxEvtHandler);

/****************************************************************************
REMARKS:
Psuedo virtual constructor for the wxPlugIn class.
****************************************************************************/
bool wxPlugIn::Create(
    wxHtmlAppletWindow *parent)
{
    m_parent = parent;
    return true;
}

/****************************************************************************
REMARKS:
Destructor for the wxPlugIn class.
****************************************************************************/
wxPlugIn::~wxPlugIn()
{
}

/****************************************************************************
REMARKS:
Destructor for the wxPlugIn class.
****************************************************************************/
void wxPlugIn::Run(
    const wxString& WXUNUSED(cmdLine))
{
}

