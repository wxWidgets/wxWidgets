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
* Description:  Header file for the wxApplet class
*
****************************************************************************/

#ifndef __WX_APPLET_H
#define __WX_APPLET_H

#include "wx/panel.h"
#include "wx/html/htmlwin.h"

// Forward declaration
class wxHtmlAppletWindow;

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Defines the abstract base class for wxApplet objects.
****************************************************************************/
class wxApplet : public wxPanel {
private:
    DECLARE_ABSTRACT_CLASS(wxApplet);
    DECLARE_EVENT_TABLE();

protected:
    wxHtmlAppletWindow  *m_parent;

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
    virtual void OnMessage(wxEvent& msg) = 0;
    };



#endif // __WX_APPLET_H

