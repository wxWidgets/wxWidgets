/****************************************************************************
*
*                       wxWindows HTML Applet Package
*
*               Copyright (C) 1991-2001 SciTech Software, Inc.
*                            All rights reserved.
*
*  ======================================================================
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  |                                                                    |
*  |This copyrighted computer code is a proprietary trade secret of     |
*  |SciTech Software, Inc., located at 505 Wall Street, Chico, CA 95928 |
*  |USA (www.scitechsoft.com).  ANY UNAUTHORIZED POSSESSION, USE,       |
*  |VIEWING, COPYING, MODIFICATION OR DISSEMINATION OF THIS CODE IS     |
*  |STRICTLY PROHIBITED BY LAW.  Unless you have current, express       |
*  |written authorization from SciTech to possess or use this code, you |
*  |may be subject to civil and/or criminal penalties.                  |
*  |                                                                    |
*  |If you received this code in error or you would like to report      |
*  |improper use, please immediately contact SciTech Software, Inc. at  |
*  |530-894-8400.                                                       |
*  |                                                                    |
*  |REMOVAL OR MODIFICATION OF THIS HEADER IS STRICTLY PROHIBITED BY LAW|
*  ======================================================================
*
* Language:     ANSI C++
* Environment:  Any
*
* Description:  Main wxApplet class implementation
*
****************************************************************************/

// For compilers that support precompilation
#include "wx/wxprec.h"

// Include private headers
#include "wx/applet/applet.h"

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
    const wxSize& size,
    long style)
{
    bool ret = wxPanel::Create(parent, -1, wxDefaultPosition, size, style);
    if (ret) {
        m_Parent = parent;
        }
    return ret;
}

/****************************************************************************
REMARKS:
Destructor for the wxApplet class.
****************************************************************************/
wxApplet::~wxApplet()
{
    m_Parent->RemoveApplet(this);           
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

