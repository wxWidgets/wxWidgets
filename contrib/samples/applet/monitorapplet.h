/****************************************************************************
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
* Description:  Header file for the MonitorApplet class
*
****************************************************************************/

#ifndef __WX_MONITORAPPLET_H
#define __WX_MONITORAPPLET_H

#include "wx/applet/applet.h"
#include "combobox.h"
#include "dialogs_wdr.h"

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Structure defining the simple monitor database records.
****************************************************************************/
struct MonitorEntry {
    char    m_Mfr[60];
    char    m_Model[60];
    };

/****************************************************************************
REMARKS:
Defines our wxMonitorData cookie object that is stored to maintain state
information for this MonitorApplet.
****************************************************************************/
class MonitorData : public wxObject {
public:
    MonitorEntry    m_Monitor;
    };

// Name used to track the monitor data cookie   
#define MONITOR_COOKIE_NAME "MonitorData"   
    
/****************************************************************************
REMARKS:
Defines our wxMonitor applet class
****************************************************************************/
class MonitorApplet : public wxApplet {
private:
    DECLARE_DYNAMIC_CLASS(MonitorApplet);
    DECLARE_EVENT_TABLE();
    
protected:
    ComboBox            *m_Mfr;             
    ComboBox            *m_Model;
    MonitorData         *m_Data;    
    static MonitorEntry m_Monitors[];
    
            // Flush the current state to a cookie
            void SaveCurrentState();
            
public:
            // Constructor (called during dynamic creation)
            MonitorApplet();

            // Psuedo virtual constructor
    virtual bool Create(
                wxHtmlAppletWindow *parent,
                const wxSize& size,
                long style);
                
            // Virtual destructor
    virtual ~MonitorApplet();

            // Handle HTML navigation to a new URL
    virtual void OnLinkClicked(const wxHtmlLinkInfo& link); 
            
            // Handle HTML navigation forward command in applet
    virtual void OnHistoryForward();
            
            // Handle HTML navigation back command in applet
    virtual void OnHistoryBack();
            
            // Handle messages from the wxAppletManager and other applets
    virtual void OnMessage(wxEvent& msg);

            // Update the model and menufacturer lists
            void ReadMfrList();
            void ReadModelList(bool selectCurrent);

            // Event handlers   
            void OnChange(wxCommandEvent &event);
    };
    
#endif // __WX_MONITORAPPLET_H

