/////////////////////////////////////////////////////////////////////////////
// Name:        helpview.h
// Purpose:     HelpView application class
// Author:      Vaclav Slavik, Julian Smart
// Modified by:
// Created:     2002-07-09
// RCS-ID:      $Id$
// Copyright:   (c) 2002 Vaclav Slavik, Julian Smart and others
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_HELPVIEW_H_
#define _WX_HELPVIEW_H_

#if defined(__GNUG__) && !defined(__APPLE__)
#pragma interface "help.cpp"
#endif

// Define a new application type, each program should derive a class from wxApp
class hvApp : public wxApp
{
    public:
        // override base class virtuals
        // ----------------------------

        // this one is called on application startup and is a good place for the app
        // initialization (doing it here and not in the ctor allows to have an error
        // return: if OnInit() returns false, the application terminates)

        virtual bool OnInit();
        virtual int OnExit();

        // Prompt the user for a book to open
        bool OpenBook(wxHtmlHelpController* controller);

    private:
        wxHtmlHelpController *help;
};

#endif
  // _WX_HELPVIEW_H_

