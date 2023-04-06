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
for your own code.

@note Creating the UI entirely from C++ code is fine for a simple example, but
more realistic programs can find it more convenient to define their UI at least
partially in @ref overview_xrc "XRC resource files".

While this program does nothing very useful, it introduces a couple of
important concepts and explains how to write a working wxWidgets application.
Trying building and running this application is also a good way of checking
that wxWidgets is correctly installed on your system. And if you haven't
installed wxWidgets yet, please @ref overview_install "do it first".

Without further ado, let's write our first application.

First, you have to include wxWidgets' header files, of course. This can be done
on a file by file basis (such as @c wx/frame.h) or using one global include
(@c wx/wx.h) which includes most of the commonly needed headers (although not
all of them as there are simply too many wxWidgets headers to pull in all of
them).

@code
// Start of wxWidgets "Hello World" Program
#include <wx/wx.h>
@endcode

Execution of a "classic" C++ program starts in its `main()` function (putting
aside the constructors of the global objects), but GUI applications must use a
different "entry function" on some platforms (such as `WinMain()` used under
Microsoft Windows). wxWidgets provides a convenient ::wxIMPLEMENT_APP() macro,
which allows to define the appropriate entry function on all platforms. Please
note that you can avoid using macros if you're so inclined by defining your own
entry function and using wxInitializer class for the library initialization,
but this example won't do it for brevity and simplicity.

This macro takes a single parameter which is the name of the application class,
that must be defined in the program. This class must derive from wxApp and,
at the very least, override wxApp::OnInit() virtual function, as it will be
called by wxWidgets to initialize the application. Let's do this:

@code
class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

// This defines the equivalent of main() for the current platform.
wxIMPLEMENT_APP(MyApp);
@endcode

The main window of a typical application is a wxFrame object. While it's
possible to just use this class directly, it's usually more convenient to
derive a custom class from it, as this allows to store additional data and
handle events (such as mouse clicks, messages from the menu, or a button) in
the methods of this class — allowing them to access this data easily.

So, even if we don't have any data in this toy example, let's define such a
custom class with a few "event handlers", i.e. functions taking the @c event
parameter of the type corresponding to the event being handled, which is
wxCommandEvent for the events from simple controls such as buttons, text fields
and also menu items. In our example, we react to three menu items: our custom
"Hello", and the "Exit" and "About" items (any program should normally
implement the latter two). Notice that these handlers don't need to be virtual
or public.

@code
class MyFrame : public wxFrame
{
public:
    MyFrame();

private:
    void OnHello(wxCommandEvent& event);
    void OnExit(wxCommandEvent& event);
    void OnAbout(wxCommandEvent& event);
};
@endcode

Now that we have this class, we can implement `OnInit()` which, as you
remember, is called upon application startup and simply create the main
window, represented by this class, in it:

@code
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show();
    return true;
}
@endcode

Please note the following points:

- Contrary to appearances, there is no memory leak in this example:
  wxWidgets takes ownership of all the window objects and destroys them
  automatically when the corresponding on-screen window is destroyed. For
  the frames (i.e. top-level windows) this happens when the user closes it.
  For the other windows, it happens when their parent is destroyed. But,
  generally speaking, all windows are owned by the framework and must @b not
  be deleted by the application.

- Frames, unlike all the other windows, are created hidden by default in
  order to allow filling them with their contents before showing everything
  at once. Please don't forget to call `Show()` to make this happen.

- Returning @true from `OnInit()` allows the application to start running.
  By default, it will exit once all top-level windows have been closed (but
  see wxApp::SetExitOnFrameDelete()), so this function should normally
  create at least one window (but can, of course, create more than one).
  Otherwise it can just return @false to exit immediately instead.


In the constructor of the main window, we create a menu with our
menu items, as well as a status bar to be shown at the bottom of the main
window.

In order to be able to react to a menu command, it must be given a unique
identifier which can be defined as a constant or an enum element. The
latter is often used because typically many such constants will be needed:

@code
enum
{
    ID_Hello = 1
};
@endcode

Notice that you don't need to define identifiers for "About" and "Exit", as
wxWidgets already predefines standard values such as ::wxID_ABOUT and
::wxID_EXIT (see @ref page_stockitems for the full list). You should use
these whenever possible, as they can be handled in a special way by a
particular platform and also have predefined labels associated with them,
which allows us to omit them when appending the corresponding menu items.
For our custom item, we specify the label, also containing an accelerator,
separated by `Tab` from the label itself, allowing to invoke this command
from keyboard:

@code
MyFrame::MyFrame()
        : wxFrame(nullptr, wxID_ANY, "Hello World")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl+H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar(menuBar);

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    ... continued below ...
@endcode

Note that, as with the windows, menu pointers don't need to (and, in fact,
must not) be destroyed because they are owned by the menu bar, which is
itself owned by the frame, which is owned, i.e. will be destroyed, by
wxWidgets.

We also have to connect our event handlers to the events we want to handle in
them. We do this by calling Bind() to send all the menu events (identified by
wxEVT_MENU event type) with the specified ID to the given function. The
parameters we pass to Bind() are

-# The event type, e.g. wxEVT_MENU, wxEVT_BUTTON, wxEVT_SIZE, or one
   of many other events used by wxWidgets.
-# A pointer to the method to call, and the object to call it on. In
   this case, we just call our own function, and pass the `this` pointer
   for the object itself. We could instead call the method of another object,
   or a non-member function — in fact, any object that can be called with a
   wxCommandEvent, can be used here.
-# An optional identifier, allowing us to select just some events of wxEVT_MENU
   type, namely those from the menu item with the given ID, instead of handling
   all of them in the provided handler. This is mainly useful with menu items
   and rarely with other kinds of events.

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
    Close(true);
}
@endcode

MyFrame::OnAbout() will display a small window with some text in it. In this
case a typical "About" window with information about the program.

@code
void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}
@endcode

The implementation of custom menu command handler may perform whatever task
your program needs to do, in this case we will simply show a message from it as
befits a Hello World example:

@code
void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
@endcode

@note It can be convenient to use unnamed lambdas instead of functions for
    event handlers, especially for such short functions. Here, for example,
    we could replace the handler above with just

@code
    Bind(wxEVT_MENU, [=](wxCommandEvent&) { wxLogMessage("Hello from a lambda!"); }, ID_Hello);
@endcode


Here is the entire program that can be copied and pasted:

@code
// Start of wxWidgets "Hello World" Program
#include <wx/wx.h>

class MyApp : public wxApp
{
public:
    bool OnInit() override;
};

wxIMPLEMENT_APP(MyApp);

class MyFrame : public wxFrame
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

bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame();
    frame->Show(true);
    return true;
}

MyFrame::MyFrame()
    : wxFrame(nullptr, wxID_ANY, "Hello World")
{
    wxMenu *menuFile = new wxMenu;
    menuFile->Append(ID_Hello, "&Hello...\tCtrl-H",
                     "Help string shown in status bar for this menu item");
    menuFile->AppendSeparator();
    menuFile->Append(wxID_EXIT);

    wxMenu *menuHelp = new wxMenu;
    menuHelp->Append(wxID_ABOUT);

    wxMenuBar *menuBar = new wxMenuBar;
    menuBar->Append(menuFile, "&File");
    menuBar->Append(menuHelp, "&Help");

    SetMenuBar( menuBar );

    CreateStatusBar();
    SetStatusText("Welcome to wxWidgets!");

    Bind(wxEVT_MENU, &MyFrame::OnHello, this, ID_Hello);
    Bind(wxEVT_MENU, &MyFrame::OnAbout, this, wxID_ABOUT);
    Bind(wxEVT_MENU, &MyFrame::OnExit, this, wxID_EXIT);
}

void MyFrame::OnExit(wxCommandEvent& event)
{
    Close(true);
}

void MyFrame::OnAbout(wxCommandEvent& event)
{
    wxMessageBox("This is a wxWidgets Hello World example",
                 "About Hello World", wxOK | wxICON_INFORMATION);
}

void MyFrame::OnHello(wxCommandEvent& event)
{
    wxLogMessage("Hello world from wxWidgets!");
}
@endcode

*/
