/****************************************************************************
*
*						wxWindows HTML Applet Package
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
* Language:		ANSI C++
* Environment:	Any
*
* Description:  Header file for the wxHtmlAppletWindow class
*
****************************************************************************/

#ifndef __WX_APPLET_WINDOW_H
#define __WX_APPLET_WINDOW_H

#include "wx/html/htmlwin.h"

/*--------------------------- Class Definitions ---------------------------*/

// Declare a linked list of wxApplet pointers
class wxApplet;
WX_DECLARE_LIST(wxApplet, wxAppletList);

/****************************************************************************
MEMBERS:
appletModules	- List of register applet modules	
appletList		- List of all active applets instances
cookies			- Hash table for managing cookies		

REMARKS:
Defines the class for wxAppletWindow. This class is derived from the
wxHtmlWindow class and extends it with functionality to handle embedded
wxApplet's on the HTML pages.
****************************************************************************/
class wxHtmlAppletWindow : public wxHtmlWindow {
private:
    DECLARE_CLASS(wxHtmlAppletWindow);
    DECLARE_EVENT_TABLE();
	
protected:
	wxAppletList	m_AppletList;		
	wxHashTable		m_Cookies;
		
public:
			// Constructor
			wxHtmlAppletWindow(
				wxWindow *parent,
				wxWindowID id = -1,
                const wxPoint& pos = wxDefaultPosition,
				const wxSize& size = wxDefaultSize,
				long style = wxHW_SCROLLBAR_AUTO,
				const wxString& name = "htmlAppletWindow");
				
			// Destructor
			~wxHtmlAppletWindow();
	
			// Create an instance of an applet based on it's class name
			wxApplet *CreateApplet(
				const wxString& className,			
				const wxSize& size);
				
			// Find an instance of an applet based on it's class name
			wxApplet *FindApplet(const wxString& className);			
			
			// Remove an applet from the window. Called during applet destruction
			bool RemoveApplet(const wxApplet *applet);			

			// Load a new URL page
			bool LoadPage(const wxString& hRef);
			
			// Called when users clicked on hypertext link.
			void OnLinkClicked(const wxHtmlLinkInfo& link);
			
			// Handles forward navigation within the HTML stack
			bool HistoryForward();
			
			// Handles backwards navigation within the HTML stack
			bool HistoryBack();
			
			// Broadcast a message to all applets on the page
			void SendMessage(wxEvent& msg);
			
			// Register a cookie of data in the applet manager
			bool RegisterCookie(const wxString& name,wxObject *cookie);
			
			// UnRegister a cookie of data in the applet manager
			bool UnRegisterCookie(const wxString& name);
			
			// Find a cookie of data given it's public name
			wxObject *FindCookie(const wxString& name);
	};
	
#endif // __WX_APPLET_WINDOW_H

