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
* Description:  Main wxApplet sample program header file
*
****************************************************************************/

#ifndef __SAMPLE_H

/*------------------------------ Constants --------------------------------*/

enum {
    // Menu items
    Minimal_Quit = 1,
    Minimal_About,
    Minimal_Back,
    Minimal_Forward,

    // Controls start here (the numbers are, of course, arbitrary)
    Minimal_Text = 1000,
    };

/*--------------------------- Class Definitions ---------------------------*/

/****************************************************************************
REMARKS:
Define a new application type, each program should derive a class from wxApp
****************************************************************************/
class MyApp : public wxApp {
public:
    // Initialise the application on startup
    virtual bool OnInit();
    };

/****************************************************************************
REMARKS:
Define a new frame type: this is going to be our main frame
****************************************************************************/
class MyFrame : public wxFrame {
private:
    DECLARE_EVENT_TABLE()       // Declare event table
    wxHtmlAppletWindow *html;   // Pointer to the html applet window

public:
            // Constructor
            MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

            // Event handlers
            void OnQuit(wxCommandEvent& event);
            void OnAbout(wxCommandEvent& event);
            void OnBack(wxCommandEvent& event);
            void OnForward(wxCommandEvent& event);
    };

#endif  // __SAMPLE_H

