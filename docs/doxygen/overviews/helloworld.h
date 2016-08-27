/////////////////////////////////////////////////////////////////////////////
// Name:        helloworld.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_helloworld Hello World Example

@tableofcontents

This page shows a very simple wxWidgets program that can be used as a skeleton
for your own code. While it does nothing very useful, it introduces a couple of
important concepts and explains how to write a working wxWidgets application.

First, you have to include wxWidgets' header files, of course. This can be done
on a file by file basis (such as @c wx/window.h) or using one global include
(@c wx/wx.h) which includes most of the commonly needed headers (although not
all of them as there are simply too many wxWidgets headers to pull in all of
them). For the platforms with support for precompiled headers, as indicated by
@c WX_PRECOMP, this global header is already included by @c wx/wxprec.h so we
only include it for the other ones:

@code
// wxWidgets "Hello world" Program

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

The main window is created by deriving a class from wxFrame and giving it a
menu and a status bar in its constructor. Also, any class that wishes to
respond to any "event" (such as mouse clicks or messages from the menu or a
button) must declare an event table using the macro below.

Finally, the way to react to such events must be done in "event handlers" which
are just functions (or functors, including lambdas if you're using C++11)
taking the @c event parameter of the type corresponding to the event being
handled, e.g. wxCommandEvent for the events from simple controls such as
buttons, text fields and also menu items. In our sample, we react to three menu
items, one for our custom menu command and two for the standard "Exit" and
"About" commands (any program should normally implement the latter two). Notice
that these handlers don't need to be neither virtual nor public.

@code
class MyFrame: public wxFrame
{
public:
    MyFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
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

Notice that you don't need to define identifiers for the "About" and "Exit" as
wxWidgets already predefines and the standard values such as wxID_ABOUT and
wxID_EXIT should be used whenever possible, as they can be handled in a special
way by a particular platform.

As in all programs there must be a "main" function. Under wxWidgets main is
implemented inside ::wxIMPLEMENT_APP() macro, which creates an application
instance of the specified class and starts running the GUI event loop. It is
used simply as:

@code
wxIMPLEMENT_APP(MyApp);
@endcode

As mentioned above, wxApp::OnInit() is called upon startup and should be used
to initialize the program, maybe showing a "splash screen" and creating the
main window (or several). As frames are created hidden by default, to allow
creating their child windows before showing them, we also need to explicitly
show it to make it appear on the screen. Finally, we return @true from this
method to indicate successful initialization:

@code
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show( true );
    return true;
}
@endcode

In the constructor of the main window (or later on) we create a menu with our
menu items as well as a status bar to be shown at the bottom of the main
window. Both have to be associated with the frame with respective calls.

@code
MyFrame::MyFrame()
        : wxFrame(NULL, wxID_ANY, "Hello World")
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

    ... continued below ...
@endcode

Notice that we don't need to specify the labels for the standard menu items
@c wxID_ABOUT and @c wxID_EXIT, they will be given standard (even correctly
translated) labels and also standard accelerators correct for the current
platform making your program behaviour more native. For this reason you should
prefer reusing the standard ids (see @ref page_stockitems) if possible.

We also have to actually connect our event handlers to the events we want to
handle in them, by calling Bind() to send all the menu events, identified by
wxEVT_MENU event type, with the specified ID to the given function. The
parameters we pass to Bind() are

-# The event type, e.g. wxEVT_MENU or wxEVT_BUTTON or wxEVT_SIZE or one
   of many other events used by wxWidgets.
-# Pointer to the member function to call and the object to call it on. In
   this case we just call our own function, but we could call a member
   function of another object too. And we could could also use a non-member
   function here, anything that can be called passing it a wxCommandEvent
   could be used here.
-# The optional identifier allowing to select just some events of wxEVT_MENU
   type, namely those from the menu item with the given ID, instead of handling
   all of them in the provided handler. This is especially useful with the menu
   items and rarely used with other kinds of events.

@code
    ... continued from above ...

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}
@endcode

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
// wxWidgets "Hello world" Program

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
    MyFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};

enum
{
    ID_Hello = 1
};

wxIMPLEMENT_APP(MyApp);

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show( true );
    return true;
}

MyFrame::MyFrame()
        : wxFrame(NULL, wxID_ANY, "Hello World")
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

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
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
