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

// For compilers that support precompilation
#include "wx/wxprec.h"

// Include private headers
#include "wx/wx.h"
#include "monitorapplet.h"

/*---------------------------- Global variables ---------------------------*/

// Implement the dynamic class so it can be constructed dynamically
IMPLEMENT_DYNAMIC_CLASS(MonitorApplet, wxApplet)

// Event handler table.
BEGIN_EVENT_TABLE(MonitorApplet, wxApplet)
    EVT_LISTBOX(ID_LISTBOX_MFTR,        MonitorApplet::OnChange)
    EVT_LISTBOX(ID_LISTBOX_MDL,         MonitorApplet::OnChange)
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
    char            buf[80] = "";
    int             i,selected = 0;
    MonitorEntry    *m;

    m_Mfr->Clear();
    for (m = m_Monitors,i = 0; m->m_Mfr[0] != 0; m++) {
        if (wxStricmp(buf,m->m_Mfr) != 0) {
            m_Mfr->Append(m->m_Mfr);
            if (wxStricmp(m_Data->m_Monitor.m_Mfr,m->m_Mfr) == 0)
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
    int             i,selected = 0;
    MonitorEntry    *m;
    wxString        mfrStr;

    mfrStr = m_Mfr->GetStringSelection();
    m_Model->Clear();
    for (m = m_Monitors,i = 0; m->m_Mfr[0] != 0; m++) {
        if (wxStricmp(mfrStr,m->m_Mfr) == 0) {
            m_Model->Append(m->m_Model);
            if (selectCurrent && wxStricmp(m_Data->m_Monitor.m_Model,m->m_Model) == 0)
                selected = i;
            i++;
            }
        }
    m_Model->Select(selected);
}

