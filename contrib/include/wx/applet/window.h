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
* Description:  Header file for the wxHtmlAppletWindow class
*
****************************************************************************/

#ifndef __WX_APPLET_WINDOW_H
#define __WX_APPLET_WINDOW_H

#include "wx/html/htmlwin.h"
#include "wx/process.h"

// Forward declare
class wxApplet;
class wxQlet;
class wxLoadPageEvent;
class wxPageLoadedEvent;
class wxIncludePrep;
class wxToolBarBase;

// Declare a linked list of wxApplet pointers
WX_DECLARE_LIST(wxApplet, wxAppletList);

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Defines the class for virtual-link data types
****************************************************************************/
class VirtualData : public wxObject {
private:
    wxString m_name;
    wxString m_group;
    wxString m_href;

public:
            // Ctors
            VirtualData(
                wxString& name,
                wxString& group,
                wxString& href );

            // Gets
            wxString GetName(){ return m_name;};
            wxString GetGroup(){ return m_group;};
            wxString GetHref(){ return m_href;};

            // Sets
            void SetName (wxString& s){ m_name = s; };
            void SetGroup(wxString& s){ m_group = s; };
            void SetHref (wxString& s){ m_href = s; };
    };

/****************************************************************************
REMARKS:
Defines the class for wxAppletWindow. This class is derived from the
wxHtmlWindow class and extends it with functionality to handle embedded
wxApplet's on the HTML pages.
****************************************************************************/
class wxHtmlAppletWindow : public wxHtmlWindow {
private:
    DECLARE_CLASS(wxHtmlAppletWindow);
    DECLARE_EVENT_TABLE();

    bool m_mutexLock;
    wxIncludePrep *incPreprocessor;  // deleted by list it is added too in constructor
protected:
	wxAppletList		m_AppletList;		
	static wxHashTable	m_Cookies;
    wxToolBarBase       *m_NavBar;
    int                 m_NavBackId;
    int                 m_NavForwardId;
	wxString            m_DocRoot;	
public:
            // Constructor
            wxHtmlAppletWindow(
                wxWindow *parent,
                wxWindowID id = -1,
                wxToolBarBase *navBar = NULL,
                int navBackId = -1,
                int navForwardId = -1,
                const wxPoint& pos = wxDefaultPosition,
                const wxSize& size = wxDefaultSize,
                long style = wxHW_SCROLLBAR_AUTO,
                const wxString& name = "htmlAppletWindow",
                const wxString& docroot = "" );

            // Destructor
            ~wxHtmlAppletWindow();

            // Create an instance of an applet based on it's class name
            wxApplet *CreateApplet(
                const wxString& classId,
                const wxString& iName,
                const wxHtmlTag &params,
                const wxSize& size);

            // Create an instance of an Qlet based on it's class name
            bool CreatePlugIn(const wxString& classId );

            // Find an instance of an applet based on it's class name
            wxApplet *FindApplet(const wxString& className);

            // Remove an applet from the window. Called during applet destruction
            bool RemoveApplet(const wxApplet *applet);

            // Load a new URL page
    virtual bool LoadPage(const wxString& location);

            // Called when users clicked on hypertext link.
    virtual void OnLinkClicked(const wxHtmlLinkInfo& link);

            // Handles forward navigation within the HTML stack
            bool HistoryForward();

            // Handles backwards navigation within the HTML stack
            bool HistoryBack();

            // Broadcast a message to all applets on the page
            void SendMessage(wxEvent& msg);

            // Register a cookie of data in the applet manager
            static bool RegisterCookie(const wxString& name,wxObject *cookie);

            // UnRegister a cookie of data in the applet manager
            static bool UnRegisterCookie(const wxString& name);

            // Find a cookie of data given it's public name
            static wxObject *FindCookie(const wxString& name);
			
            // Event handlers to load a new page	
			void OnLoadPage(wxLoadPageEvent &event);

            // Event handlers to load a new page	
    		void OnPageLoaded(wxPageLoadedEvent &event);

            // LoadPage mutex locks
            void Lock(){ m_mutexLock = true;};
            void UnLock(){ m_mutexLock = false;};

            // Returns TRUE if the mutex is locked, FALSE otherwise.
            bool IsLocked(){ return m_mutexLock;};

            // Tries to lock the mutex. If it can't, returns immediately with false.
            bool TryLock();

    };

/****************************************************************************
REMARKS:
Defines the class for AppetProcess
***************************************************************************/
class AppletProcess : public wxProcess {
public:
            AppletProcess(
                wxWindow *parent)
                : wxProcess(parent)
            {
            }

            // instead of overriding this virtual function we might as well process the
            // event from it in the frame class - this might be more convenient in some
            // cases
    virtual void OnTerminate(int pid, int status);

    };

/****************************************************************************
REMARKS:
Defines the class for wxHtmlAppletCell
***************************************************************************/
class wxHtmlAppletCell : public wxHtmlCell
{
public:
    wxHtmlAppletCell(wxWindow *wnd, int w = 0);
    ~wxHtmlAppletCell() { m_Wnd->Destroy(); }
    virtual void Draw(wxDC& dc, int x, int y, int view_y1, int view_y2);
    virtual void DrawInvisible(wxDC& dc, int x, int y);
    virtual void Layout(int w);

protected:
    wxWindow* m_Wnd;
            // width float is used in adjustWidth (it is in percents)
};


#endif // __WX_APPLET_WINDOW_H

