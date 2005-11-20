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
* Description:  Loadpage event class implementation
*
****************************************************************************/

// Include private headers
#include "wx/applet/loadpage.h"

// wxWindows forcelink macro
#include "wx/html/forcelnk.h"

/*------------------------- Implementation --------------------------------*/

// Implement the class functions for wxLoadPageEvent
IMPLEMENT_DYNAMIC_CLASS(wxLoadPageEvent, wxEvent)

// Implement the class functions for wxPageLoadedEvent
IMPLEMENT_DYNAMIC_CLASS(wxPageLoadedEvent, wxEvent)

// Define our custom event ID for load page
DEFINE_EVENT_TYPE(wxEVT_LOAD_PAGE)

// Define our custom event ID for page loaded
DEFINE_EVENT_TYPE(wxEVT_PAGE_LOADED)

/****************************************************************************
REMARKS:
Constructor for the wxLoadPageEvent class
****************************************************************************/
wxLoadPageEvent::wxLoadPageEvent(
    const wxString &hRef,
    wxHtmlAppletWindow *htmlWindow)
    : m_hRef(hRef), m_htmlWindow(htmlWindow)
{
    m_eventType = wxEVT_LOAD_PAGE;
}

/****************************************************************************
REMARKS:
Constructor for the wxPageLoadedEvent class
****************************************************************************/
wxPageLoadedEvent::wxPageLoadedEvent()
{
    m_eventType = wxEVT_LOAD_PAGE;
}

// This is out little force link hack
FORCE_LINK_ME(loadpage)

