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
* Description:  Header file for the wxLoadPage Event class
*
****************************************************************************/

#ifndef __WX_LOAD_PAGE_H
#define __WX_LOAD_PAGE_H

#include "wx/html/htmlwin.h"

// Forward declaration
class wxHtmlAppletWindow;

// If we are compiling this code into a library that links against
// the DLL, we need to remove all the __declspec(dllimports) that
// would declare our classes below incorrectly.

#ifndef WXMAKINGDLL
#undef  WXDLLEXPORT
#define WXDLLEXPORT
#endif
// Declare our local load page event type
BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EVENT_TYPE(wxEVT_LOAD_PAGE,   wxEVT_USER_FIRST+1)
    DECLARE_EVENT_TYPE(wxEVT_PAGE_LOADED, wxEVT_USER_FIRST+2)
END_DECLARE_EVENT_TYPES()

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Defines the class for load page events.
****************************************************************************/
class wxLoadPageEvent : public wxEvent {
    DECLARE_DYNAMIC_CLASS(wxLoadPageEvent);

protected:
    wxString            m_hRef;
    wxHtmlAppletWindow  *m_htmlWindow;

public:
            // Constructor
            wxLoadPageEvent(const wxString &hRef = "",wxHtmlAppletWindow *htmlWindow = NULL);

            // Destructor
            virtual ~wxLoadPageEvent() {}

            // Clone Virtual
            virtual wxEvent *Clone() const { return new wxLoadPageEvent(m_hRef, m_htmlWindow); }

            // Return the hmtl window for the load page operation
            wxHtmlAppletWindow  *GetHtmlWindow() { return m_htmlWindow; };

            // Get the hRef string for the load page operation
            const wxString & GetHRef() { return m_hRef; };

    };


// Define the macro to create our event type
typedef void (wxEvtHandler::*wxLoadPageEventFunction)(wxLoadPageEvent&);
#define EVT_LOAD_PAGE(fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_LOAD_PAGE, -1, -1, (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxLoadPageEventFunction, & fn ), (wxObject *) NULL ),

/****************************************************************************
REMARKS:
Defines the class for pageloaded events.
****************************************************************************/
class wxPageLoadedEvent : public wxEvent {
    DECLARE_DYNAMIC_CLASS(wxPageLoadedEvent);

public:
            // Constructor
            wxPageLoadedEvent();

            // Destructor
            virtual ~wxPageLoadedEvent() {}

            // Clone Virtual
            virtual wxEvent *Clone() const {
                return new wxPageLoadedEvent(); }

    };

// Define the macro to create our event type
typedef void (wxEvtHandler::*wxPageLoadedEventFunction)(wxPageLoadedEvent&);
#define EVT_PAGE_LOADED(fn) DECLARE_EVENT_TABLE_ENTRY( wxEVT_PAGE_LOADED, -1, -1, (wxObjectEventFunction)(wxEventFunction) wxStaticCastEvent( wxPageLoadedEventFunction, & fn ), (wxObject *) NULL ),


#endif // __WX_LOAD_PAGE_H
