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
* Description:  Main wxApplet class implementation
*
****************************************************************************/

// For compilers that support precompilation
#include "wx/wxprec.h"

// Include private headers
#include "monitorapplet.h"

/*---------------------------- Global variables ---------------------------*/

// Implement the dynamic class so it can be constructed dynamically
IMPLEMENT_DYNAMIC_CLASS(MonitorApplet, wxApplet);
	
// Event handler table.
BEGIN_EVENT_TABLE(MonitorApplet, wxApplet)
	EVT_LISTBOX(ID_LISTBOX_MFTR, 		MonitorApplet::OnChange)
	EVT_LISTBOX(ID_LISTBOX_MDL, 		MonitorApplet::OnChange)
END_EVENT_TABLE()

// Include database of known monitors. Normally this would come from a
// real database on disk, but for this simple example we hard code all
// the values into a table.
#include "monitors.c"	
	
/*------------------------- Implementation --------------------------------*/

/****************************************************************************
REMARKS:
Constructor called during dynamic creation. Simple initialise all
internal values for the class so that it can be properly created later
via the virtual Create member function.
****************************************************************************/
MonitorApplet::MonitorApplet()
{	
	m_Mfr = NULL;
	m_Model = NULL;
	m_Data = NULL;
}

/****************************************************************************
REMARKS:
Psuedo virtual constructor for the MonitorApplet class.
****************************************************************************/
bool MonitorApplet::Create(
	wxHtmlAppletWindow *parent,
	const wxSize& size,
	long style)
{
    bool ret = wxApplet::Create(parent, size, style);
    if (ret) {
		// Read our cookie or create it if it does not exist
		if ((m_Data = (MonitorData*)parent->FindCookie(MONITOR_COOKIE_NAME)) == NULL) {
			m_Data = new MonitorData;
			memset(&m_Data->m_Monitor,0,sizeof(m_Data->m_Monitor));
			parent->RegisterCookie(MONITOR_COOKIE_NAME,m_Data);
			}
		
		// Create all the controls and initialise them
		MonitorDialogFunc(this,true,true);
		if ((m_Mfr = new ComboBox(this , ID_LISTBOX_MFTR, ID_TEXTCTRL_MFTR)) == NULL)
			return false;
		if ((m_Model = new ComboBox(this , ID_LISTBOX_MDL, ID_TEXTCTRL_MDL)) == NULL)
			return false;
		ReadMfrList();
		ReadModelList(true);
		}
    return ret;
}
				
/****************************************************************************
REMARKS:
Destructor for the MonitorApplet class.
****************************************************************************/
MonitorApplet::~MonitorApplet()
{
	delete m_Mfr;
	delete m_Model;
}

/****************************************************************************
REMARKS:
Save the current state for the applet to our cookie
****************************************************************************/
void MonitorApplet::SaveCurrentState()
{
	// Read currently selected strings into cookie
    strcpy(m_Data->m_Monitor.m_Mfr,m_Mfr->GetStringSelection());
    strcpy(m_Data->m_Monitor.m_Model,m_Model->GetStringSelection());
}

/****************************************************************************
REMARKS:
Handles user navigation away from the applet via an HTML link
****************************************************************************/
void MonitorApplet::OnLinkClicked(
	const wxHtmlLinkInfo&)	
{
	SaveCurrentState();
}
			
/****************************************************************************
REMARKS:
Handles user navigation away from the applet via the history forward command
****************************************************************************/
void MonitorApplet::OnHistoryForward()
{
	SaveCurrentState();
}
			
/****************************************************************************
REMARKS:
Handles user navigation away from the applet via the history back command
****************************************************************************/
void MonitorApplet::OnHistoryBack()
{
	SaveCurrentState();
}
			
/****************************************************************************
REMARKS:
Handles inter applet communication messages
****************************************************************************/
void MonitorApplet::OnMessage(	
	wxEvent& msg)
{
	msg.Skip(true);
}

/****************************************************************************
REMARKS:
****************************************************************************/
void MonitorApplet::OnChange(
	wxCommandEvent &evt)
{
	if (evt.GetId() == m_Mfr->GetListBoxId()) {
		m_Mfr->OnChange(evt);
		ReadModelList(true);
		}
	else if (evt.GetId() == m_Model->GetListBoxId()) {
		m_Model->OnChange(evt);   	
		}
}

/****************************************************************************
REMARKS:
Updates the manufacturer list for the dialog box from the database.
****************************************************************************/
void MonitorApplet::ReadMfrList()
{				
	char			buf[80] = "";
	int				i,selected = 0;
	MonitorEntry	*m;

    m_Mfr->Clear();
	for (m = m_Monitors,i = 0; m->m_Mfr[0] != 0; m++) {
		if (stricmp(buf,m->m_Mfr) != 0) {
            m_Mfr->Append(m->m_Mfr);
			if (stricmp(m_Data->m_Monitor.m_Mfr,m->m_Mfr) == 0)
				selected = i;
			strcpy(buf,m->m_Mfr);
			i++;
			}
		}
    m_Mfr->Select(selected);
}

/****************************************************************************
REMARKS:
Updates the model list for the dialog box for the currently selected
manufacturer type.
****************************************************************************/
void MonitorApplet::ReadModelList(
    bool selectCurrent)
{				
	int				i,selected = 0;
	MonitorEntry	*m;
    wxString  		mfrStr;
	
	mfrStr = m_Mfr->GetStringSelection();
    m_Model->Clear();
	for (m = m_Monitors,i = 0; m->m_Mfr[0] != 0; m++) {
		if (stricmp(mfrStr,m->m_Mfr) == 0) {
            m_Model->Append(m->m_Model);
			if (selectCurrent && stricmp(m_Data->m_Monitor.m_Model,m->m_Model) == 0)
				selected = i;
			i++;
			}
		}
    m_Model->Select(selected);
}

