/////////////////////////////////////////////////////////////////////////////
// Name:        helloworld.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_helloworld Hello World Example

This page shows a very simple wxWidgets program that can be used as a skeleton
for your own code. While it does nothing very useful, it introduces a couple of
important concepts and explains how to write a working wxWidgets application.

First, you have to include wxWidgets' header files, of course. This can
be done on a file by file basis (such as @c wx/window.h</tt>) or using one
global include (@c wx/wx.h) which includes most of the commonly needed headers
(although not all of them as there are simply too many wxWidgets headers to
pull in all of them). For the platforms with support for precompiled headers,
as indicated by @c WX_PRECOMP, this global header is already included by @c
wx/wxprec.h so we only include it for the other ones:

@code
//   wxWidgets "Hello world" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif
@endcode

Practically every app should define a new class derived from wxApp. By
overriding wxApp's OnInit() virtual method the program can be initialized, e.g.
by creating a new main window.

@code
class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};
@endcode

The main window is created by deriving a class from wxFrame and
giving it a menu and a status bar in its constructor. Also, any class
that wishes to respond to any "event" (such as mouse clicks or
messages from the menu or a button) must declare an event table
using the macro below.

Finally, the way to react to such events must be done in "handlers".
In our sample, we react to three menu items, one for our custom menu
command and two for the standard "Exit" and "About" commands (any program
should normally implement the latter two). Notice that these handlers
don't need to be neither virtual nor public.

@code
class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};
@endcode

In order to be able to react to a menu command, it must be given a unique
identifier which can be defined as a const variable or an enum element. The
latter is often used because typically many such constants will be needed:

@code
enum
{
    ID_Hello = 1
};
@endcode

Notice that you don't need to define identifiers for the "About" and "Exit"
We then proceed to actually implement an event table in which the events
are routed to their respective handler functions in the class MyFrame.

There are predefined macros for routing all common events, ranging from
the selection of a list box entry to a resize event when a user resizes
a window on the screen. If @c wxID_ANY is given as the ID, the given handler will be
invoked for any event of the specified type, so that you could add just
one entry in the event table for all menu commands or all button commands etc.

The origin of the event can still be distinguished in the event handler as
the (only) parameter in an event handler is a reference to a wxEvent object,
which holds various information about the event (such as the ID of and a
pointer to the class, which emitted the event).

@code
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello,   MyFrame::OnHello)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()
@endcode

As in all programs there must be a "main" function. Under wxWidgets main is implemented
using this macro, which creates an application instance and starts the program.

@code
wxIMPLEMENT_APP(MyApp)
@endcode

As mentioned above, wxApp::OnInit() is called upon startup and should be
used to initialize the program, maybe showing a "splash screen" and creating
the main window (or several). The frame should get a title bar text ("Hello World")
and a position and start-up size. One frame can also be declared to be the
top window. Returning @true indicates a successful initialization.

@code
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "Hello World", wxPoint(50,50), wxSize(450,340) );
    frame->Show( true );
    return true;
}
@endcode

In the constructor of the main window (or later on) we create a menu with our menu
items as well as a status bar to be shown at the bottom of the main window. Both have
to be associated with the frame with respective calls.

@code
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );

    SetMenuBar( menuBar );

    CreateStatusBar();
    SetStatusText( "Welcome to wxWidgets!" );
}
@endcode

Notice that we don't need to specify the labels for the standard menu items
@c wxID_ABOUT and @c wxID_EXIT, they will be given standard (even correctly
translated) labels and also standard accelerators correct for the current
platform making your program behaviour more native. For this reason you should
prefer reusing the standard ids (see @ref page_stockitems) if possible.

Here are the standard event handlers implementations. MyFrame::OnExit() closes
the main window by calling Close(). The parameter @true indicates that other
windows have no veto power such as after asking "Do you really want to close?".
If there is no other main window left, the application will quit.

@code
void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}
@endcode

MyFrame::OnAbout() will display a small window with some text in it. In this
case a typical "About" window with information about the program.

@code
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( "This is a wxWidgets' Hello world sample",
                  "About Hello World", wxOK | wxICON_INFORMATION );
}
@endcode

The implementation of custom menu command handler may perform whatever task
your program needs to do, in this case we will simply show a message from it as
befits a hello world example:
@code
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
@endcode

Here is the entire program that can be copied and pasted:
@code
//   wxWidgets "Hello world" Program

// For compilers that support precompilation, includes "wx/wx.h".
#include <wx/wxprec.h>

#ifndef WX_PRECOMP
    #include <wx/wx.h>
#endif

class MyApp: public wxApp
{
public:
    virtual bool OnInit();
};

class MyFrame: public wxFrame
{
public:
    MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size);

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);

    wxDECLARE_EVENT_TABLE();
};

enum
{
    ID_Hello = 1
};

wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(ID_Hello,   MyFrame::OnHello)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExit)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAbout)
wxEND_EVENT_TABLE()

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame( "Hello World", wxPoint(50,50), wxSize(450,340) );
    frame->Show( true );
    return true;
}

MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
        : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append( menuFile, "&File" );
    menuBar->Append( menuHelp, "&Help" );

    SetMenuBar( menuBar );

    CreateStatusBar();
    SetStatusText( "Welcome to wxWidgets!" );
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close( true );
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox( "This is a wxWidgets' Hello world sample",
                  "About Hello World", wxOK | wxICON_INFORMATION );
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
@endcode

*/

