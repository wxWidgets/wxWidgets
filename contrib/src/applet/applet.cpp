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
* Description:  Main wxApplet class implementation
*
****************************************************************************/

// Include private headers
#include "wx/applet/applet.h"
#include "wx/applet/window.h"

/*------------------------- Implementation --------------------------------*/

// Empty event handler. We include this event handler simply so that
// sub-classes of wxApplet can reference wxApplet in the event tables
// that they create as necessary.
BEGIN_EVENT_TABLE(wxApplet, wxPanel)
     EVT_ERASE_BACKGROUND(wxApplet::OnEraseBackground)
END_EVENT_TABLE()

// Implement the abstract class functions
IMPLEMENT_ABSTRACT_CLASS(wxApplet, wxPanel);

/****************************************************************************
REMARKS:
Psuedo virtual constructor for the wxApplet class.
****************************************************************************/
bool wxApplet::Create(
    wxHtmlAppletWindow *parent,
    const wxHtmlTag& ,
    const wxSize& size,
    long style)
{
    bool ret = wxPanel::Create(parent, -1, wxDefaultPosition, size, style);
    if (ret) {
        m_parent = parent;
        }
    return ret;
}

/****************************************************************************
REMARKS:
Destructor for the wxApplet class.
****************************************************************************/
wxApplet::~wxApplet()
{
    ((wxHtmlAppletWindow *) m_parent)->RemoveApplet(this);
}

/****************************************************************************
REMARKS:
Special handler for background erase messages. We do nothing in here which
causes the background to not be erased which is exactly what we want. All
the wxApplet classes display over an HTML window, so we want the HTML
background to show through.
****************************************************************************/
void wxApplet::OnEraseBackground(wxEraseEvent&)
{
}

