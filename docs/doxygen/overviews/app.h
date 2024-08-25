/////////////////////////////////////////////////////////////////////////////
// Name:        app.h
// Purpose:     topic overview
// Author:      wxWidgets team
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_app wxApp Overview

@tableofcontents

A wxWidgets application does not have a @e main function; the equivalent, i.e.
the entry point where the execution of the program begins, is the
wxApp::OnInit() member function defined in a class derived from wxApp (this
class is typically specified using wxIMPLEMENT_APP() macro).

@e OnInit usually creates the main application window and returns @true.
If it returns @false, the application will exit immediately, without starting
to run.

Note that the program's command line arguments, represented by @e argc and
@e argv, are available from within wxApp member functions.

An application closes by destroying all windows. Because all frames must be
destroyed for the application to exit, it is advisable to use parent frames
wherever possible when creating new frames, so that deleting the top level
frame will automatically delete child frames. The alternative is to explicitly
delete child frames in the top-level frame's wxCloseEvent handler.

In emergencies the wxExit function can be called to kill the application
however normally the application shuts down automatically, see
@ref overview_app_shutdown.

An example of defining an application follows:

@code
class DerivedApp : public wxApp
{
public:
    virtual bool OnInit() override;
};

wxIMPLEMENT_APP(DerivedApp);

bool DerivedApp::OnInit()
{
    wxFrame *the_frame = new wxFrame(nullptr, ID_MYFRAME, argv[0]);
    ...
    the_frame->Show(true);

    return true;
}
@endcode

Note the use of wxIMPLEMENT_APP(), which defines the application entry
point (either @c main() or @c WinMain() function, depending on the platform)
and tells wxWidgets which application class should be used.

You can also use wxDECLARE_APP(appClass) in a header file to declare the wxGetApp
function which returns a reference to the application object. Otherwise you can
only use the global @c wxTheApp pointer which is of type @c wxApp*.



@section overview_app_shutdown Application Shutdown

The application normally shuts down when the last of its top level windows is
closed. This is normally the expected behaviour and means that it is enough to
call wxWindow::Close() in response to the "Exit" menu command if your program
has a single top level window. If this behaviour is not desirable
wxApp::SetExitOnFrameDelete can be called to change it.

Note that such logic doesn't apply for the windows shown before the program
enters the main loop: in other words, you can safely show a dialog from
wxApp::OnInit and not be afraid that your application terminates when this
dialog -- which is the last top level window for the moment -- is closed.

Another aspect of the application shutdown is wxApp::OnExit which is called
when the application exits but @e before wxWidgets cleans up its internal
structures. You should delete all wxWidgets object that you created by the time
OnExit finishes.

In particular, do @b not destroy them from application class' destructor! For
example, this code may crash:

@code
class MyApp : public wxApp
{
public:
    wxCHMHelpController m_helpCtrl;
    ...
};
@endcode

The reason for that is that @c m_helpCtrl is a member object and is thus
destroyed from MyApp destructor. But MyApp object is deleted after wxWidgets
structures that wxCHMHelpController depends on were uninitialized! The solution
is to destroy HelpCtrl in @e OnExit:

@code
class MyApp : public wxApp
{
public:
    wxCHMHelpController *m_helpCtrl;
    ...
};

bool MyApp::OnInit()
{
    ...
    m_helpCtrl = new wxCHMHelpController;
    ...
}

int MyApp::OnExit()
{
    delete m_helpCtrl;
    return 0;
}
@endcode

*/
