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

