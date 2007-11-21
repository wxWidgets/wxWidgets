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
* Description:  Header file for the wxApplet class
*
****************************************************************************/

#ifndef __WX_APPLET_H
#define __WX_APPLET_H

#include "wx/panel.h"
#include "wx/html/htmlwin.h"

// Forward declaration
class wxHtmlAppletWindow;
class wxAppletEvent;
/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Defines the abstract base class for wxApplet objects.
****************************************************************************/
class wxApplet : public wxPanel {
private:
    DECLARE_ABSTRACT_CLASS(wxApplet);
    DECLARE_EVENT_TABLE()

protected:
    //wxHtmlAppletWindow  *m_parent;

            // Special handler for background erase messages
            void OnEraseBackground(wxEraseEvent&);

public:
            // Constructor (called during dynamic creation)
            wxApplet() { m_parent = NULL; };

            // Psuedo virtual constructor
    virtual bool Create(
                wxHtmlAppletWindow *parent,
                const wxHtmlTag& params,
                const wxSize& size,
                long style = wxTAB_TRAVERSAL | wxNO_BORDER);

            // Virtual destructor
    virtual ~wxApplet();

            // Handle HTML navigation to a new URL
    virtual void OnLinkClicked(const wxHtmlLinkInfo& link) = 0;

            // Handle HTML navigation forward command in applet
    virtual void OnHistoryForward() = 0;

            // Handle HTML navigation back command in applet
    virtual void OnHistoryBack() = 0;

            // Handle messages from the wxAppletManager and other applets
    virtual void OnMessage(wxAppletEvent& msg) = 0;
    };



#endif // __WX_APPLET_H

