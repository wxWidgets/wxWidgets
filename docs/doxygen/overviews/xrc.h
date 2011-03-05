/////////////////////////////////////////////////////////////////////////////
// Name:        xrc.h
// Purpose:     topic overview
// Author:      wxWidgets team
// RCS-ID:      $Id$
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

/**

@page overview_xrc XML Based Resource System (XRC)

Classes: wxXmlResource, wxXmlResourceHandler

The XML-based resource system, known as XRC, allows user interface elements
such as dialogs, menu bars and toolbars, to be stored in text files and loaded
into the application at run-time. XRC files can also be compiled into binary
XRS files or C++ code (the former makes it possible to store all resources in a
single file and the latter is useful when you want to embed the resources into
the executable).

There are several advantages to using XRC resources:

@li Recompiling and linking an application is not necessary if the resources
    change.
@li If you use a dialog designer that generates C++ code, it can be hard to
    reintegrate this into existing C++ code. Separation of resources and code
    is a more elegant solution.
@li You can choose between different alternative resource files at run time, if
    necessary.
@li The XRC format uses sizers for flexibility, allowing dialogs to be
    resizable and highly portable.
@li The XRC format is a wxWidgets standard, and can be generated or
    postprocessed by any program that understands it. As it is basedon the XML
    standard, existing XML editors can be used for simple editing purposes.

XRC was written by Vaclav Slavik.

@li @ref overview_xrc_gettingstarted
@li @ref overview_xrc_xrcsample
@li @ref overview_xrc_binaryresourcefiles
@li @ref overview_xrc_embeddedresource
@li @ref overview_xrc_cppheader
@li @ref overview_xrc_newresourcehandlers

See also the separate @ref overview_xrcformat page for more information, and
details about the XRC file format.


@section overview_xrc_gettingstarted Getting Started with XRC

<b> Creating an XRC file </b>

You will need to write an XRC file. Though this @e can be done by hand in a
text editor, for all but the smallest files it is advisable to use a
specialised tool. Examples of these include:

@e Non-free:
@li wxDesigner <http://www.wxdesigner-software.de/>, a commercial dialog
    designer/RAD tool.
@li DialogBlocks <http://www.anthemion.co.uk/dialogblocks/>, a commercial
    dialog editor.

@e Free:
@li XRCed <http://xrced.sf.net/>, a wxPython-based dialog editor that you
    can find in the wxPython/tools subdirectory of the wxWidgets SVN archive.
@li wxFormBuilder <http://wxformbuilder.org/>, a C++-based dialog editor that
    can output C++, XRC or python.

There's a more complete list at <http://www.wxwidgets.org/wiki/index.php/Tools>

This small demonstration XRC file contains a simple dialog:
@code
<?xml version="1.0" ?>
<resource version="2.3.0.1">
  <object class="wxDialog" name="SimpleDialog">
    <title>Simple dialog</title>
    <object class="wxBoxSizer">
      <orient>wxVERTICAL</orient>
      <object class="sizeritem">
        <object class="wxTextCtrl" name="text"/>
        <option>1</option>
        <flag>wxALL|wxEXPAND</flag>
        <border>10</border>
      </object>
      <object class="sizeritem">
        <object class="wxBoxSizer">
          <object class="sizeritem">
            <object class="wxButton" name="clickme_btn">
              <label>Click</label>
            </object>
            <flag>wxRIGHT</flag>
            <border>10</border>
          </object>
          <object class="sizeritem">
            <object class="wxButton" name="wxID_OK">
              <label>OK</label>
            </object>
            <flag>wxLEFT</flag>
            <border>10</border>
          </object>
          <orient>wxHORIZONTAL</orient>
        </object>
        <flag>wxALL|wxALIGN_CENTRE</flag>
        <border>10</border>
      </object>
    </object>
  </object>
</resource>
@endcode

You can keep all your XRC elements together in one file, or split them between
several.

<b> Loading XRC files </b>

Before you can use XRC in an app, it must first be loaded. This code fragment
shows how to load a single XRC file "resource.xrc" from the current working
directory, plus all the *.xrc files contained in the subdirectory "rc".

@code
#include "wx/xrc/xmlres.h"

bool MyApp::OnInit()
{
    ...
    wxXmlResource::Get()->InitAllHandlers();

    wxXmlResource::Get()->Load("resource.xrc");
    wxXmlResource::Get()->LoadAllFiles("rc");
    ...
}
@endcode

It's normal to load any XRC files at the beginning of an app. Though it is
possible to unload a file later, it's seldom necessary.


<b> Using an XRC item </b>

The XRC file(s) are now loaded into the app's virtual filesystem. From there,
you must do another sort of load when you want to use an individual object.
Yes, it's confusingly named, but you first Load() the file, and later load each
top-level object when its needed.

This is how you would use the above simple dialog in your code.

@code
void MyClass::ShowDialog()
{
    wxDialog dlg;
    if (wxXmlResource::Get()->LoadDialog(&dlg, NULL, "SimpleDialog"))
        dlg.ShowModal();
}
@endcode

See how simple the code is. All the instantiation is done invisibly by the XRC
system.

Though you'll most often use wxXmlResource::LoadDialog, there are also
equivalents that load a frame, a menu etc; and the generic
wxXmlResource::LoadObject. See wxXmlResource for more details.

<b> Accessing XRC child controls </b>

The last section showed how to load top-level windows like dialogs, but what
about child windows like the wxTextCtrl named "text" that the dialog contains?
You can't 'load' an individual child control in the same way. Instead you use
the XRCCTRL macro to get a pointer to the child. To expand the previous code:

@code
void MyClass::ShowDialog()
{
    wxDialog dlg;
    if (!wxXmlResource::Get()->LoadDialog(&dlg, NULL, "SimpleDialog"))
        return;

    wxTextCtrl* pText = XRCCTRL(dlg, "text", wxTextCtrl);
    if (pText)
        pText->ChangeValue("This is a simple dialog");

    dlg.ShowModal();
}
@endcode

XRCCTRL takes a reference to the parent container and uses wxWindow::FindWindow
to search inside it for a wxWindow with the supplied name (here "text"). It
returns a pointer to that control, cast to the type in the third parameter; so
a similar effect could be obtained by writing:

@code
pText = (wxTextCtrl*)(dlg.FindWindowByName("text"));
@endcode

<b> XRC and IDs </b>

The ID of a control is often needed, e.g. for use in an event table
or with wxEvtHandler::Bind. It can easily be found by passing the name of the
control to the XRCID macro:

@code
void MyClass::ShowDialog()
{
    wxDialog dlg;
    if (!wxXmlResource::Get()->LoadDialog(&dlg, NULL, "SimpleDialog"))
        return;

    XRCCTRL(dlg, "text", wxTextCtrl)->Bind(wxEVT_COMMAND_TEXT_UPDATED,
        wxTextEventHandler(MyClass::OnTextEntered), this, XRCID("text"));

    XRCCTRL(dlg, "clickme_btn", wxButton)->Bind(wxEVT_COMMAND_BUTTON_CLICKED,
        wxCommandEventHandler(MyClass::OnClickme), this, XRCID("clickme_btn"));

    dlg.ShowModal();
}
@endcode

A few points to note:
@li The value of the int returned by XRCID("foo") is guaranteed to be unique
within an app.
@li However that value isn't predictable, and you shouldn't rely on it being
consistent between runs. It certainly won't be the same in different apps.
@li @ref page_stockitems such as wxID_OK work correctly without requiring XRCID
(because, internally, XRCID("wxID_OK") is mapped to wxID_OK).
@li Both XRCID and XRCCTRL use the 'name' of the control (as in
wxWindow::GetName). This is different from the label that the user sees on
e.g. a wxButton.

<b> Subclassing in XRC </b>

You will often want to use subclassed wx controls in your code. There are three
ways to do this from XRC:
@li Very rarely you might need to
@ref overview_xrcformat_extending_custom "create your own wxXmlResourceHandler"
@li Occasionally wxXmlResource::AttachUnknownControl may be best. See
@ref overview_xrcformat_extending_unknown
@li Usually though, the simple 'subclass' keyword will suffice.

Suppose you wanted the wxTextCtrl named "text" to be created as your derived
class MyTextCtrl. The only change needed in the XRC file would be in this line:

@code
        <object class="wxTextCtrl" name="text" subclass="MyTextCtrl"/>
@endcode

The only change in your code would be to use MyTextCtrl in XRCCTRL. However for
the subclass to be created successfully, it's important to ensure that it uses
wxWidget's RTTI mechanism: see @ref overview_xrcformat_extending_subclass for
the details.



@section overview_xrc_xrcsample The XRC sample

A major resource for learning how to use XRC is the @sample{xrc}. This
demonstrates all of the standard uses of XRC, and some of the less common ones.
It is strongly suggested that you run it, and look at the well-commented
source code to see how it works.


@section overview_xrc_binaryresourcefiles Binary Resource Files

To compile binary resource files, use the command-line @c wxrc utility. It
takes one or more file parameters (the input XRC files) and the following
switches and options:

@li -h (--help): Show a help message.
@li -v (--verbose): Show verbose logging information.
@li -c (--cpp-code): Write C++ source rather than a XRS file.
@li -e (--extra-cpp-code): If used together with -c, generates C++ header file
    containing class definitions for the windows defined by the XRC file (see
    special subsection).
@li -u (--uncompressed): Do not compress XML files (C++ only).
@li -g (--gettext): Output underscore-wrapped strings that poEdit or gettext
    can scan. Outputs to stdout, or a file if -o is used.
@li -n (--function) @<name@>: Specify C++ function name (use with -c).
@li -o (--output) @<filename@>: Specify the output file, such as resource.xrs
    or resource.cpp.
@li -l (--list-of-handlers) @<filename@>: Output a list of necessary handlers
    to this file.

For example:

@code
$ wxrc resource.xrc
$ wxrc resource.xrc -o resource.xrs
$ wxrc resource.xrc -v -c -o resource.cpp
@endcode

@note XRS file is essentially a renamed ZIP archive which means that you can
manipulate it with standard ZIP tools. Note that if you are using XRS files,
you have to initialize the wxFileSystem archive handler first! It is a simple
thing to do:

@code
#include <wx/filesys.h>
#include <wx/fs_arc.h>
...
wxFileSystem::AddHandler(new wxArchiveFSHandler);
@endcode


@section overview_xrc_embeddedresource Using Embedded Resources

It is sometimes useful to embed resources in the executable itself instead of
loading an external file (e.g. when your app is small and consists only of one
exe file). XRC provides means to convert resources into regular C++ file that
can be compiled and included in the executable.

Use the @c -c switch to @c wxrc utility to produce C++ file with embedded
resources. This file will contain a function called @c InitXmlResource (unless
you override this with a command line switch). Use it to load the resource:

@code
extern void InitXmlResource(); // defined in generated file
...
wxXmlResource::Get()->InitAllHandlers();
InitXmlResource();
...
@endcode


@section overview_xrc_cppheader C++ header file generation

Using the @c -e switch together with @c -c, a C++ header file is written
containing class definitions for the GUI windows defined in the XRC file. This
code generation can make it easier to use XRC and automate program development.
The classes can be used as basis for development, freeing the programmer from
dealing with most of the XRC specifics (e.g. @c XRCCTRL).

For each top level window defined in the XRC file a C++ class definition is
generated, containing as class members the named widgets of the window. A
default constructor for each class is also generated. Inside the constructor
all XRC loading is done and all class members representing widgets are
initialized.

A simple example will help understand how the scheme works. Suppose you have a
XRC file defining a top level window @c TestWnd_Base, which subclasses wxFrame
(any other class like @c wxDialog will do also), and has subwidgets wxTextCtrl A
and wxButton B.

The XRC file and corresponding class definition in the header file will be
something like:

@code
<?xml version="1.0"?>
<resource version="2.3.0.1">
    <object class="wxFrame" name="TestWnd_Base">
        <size>-1,-1</size>
        <title>Test</title>
        <object class="wxBoxSizer">
            <orient>wxHORIZONTAL</orient>
            <object class="sizeritem">
                <object class="wxTextCtrl" name="A">
                    <label>Test label</label>
                </object>
            </object>
            <object class="sizeritem">
                <object class="wxButton" name="B">
                    <label>Test button</label>
                </object>
            </object>
        </object>
    </object>
</resource>


class TestWnd_Base : public wxFrame
{
protected:
    wxTextCtrl* A;
    wxButton* B;

private:
    void InitWidgetsFromXRC()
    {
        wxXmlResource::Get()->LoadObject(this, NULL, "TestWnd", "wxFrame");
        A = XRCCTRL(*this, "A", wxTextCtrl);
        B = XRCCTRL(*this, "B", wxButton);
    }
public:
    TestWnd::TestWnd()
    {
        InitWidgetsFromXRC();
    }
};
@endcode

The generated window class can be used as basis for the full window class. The
class members which represent widgets may be accessed by name instead of using
@c XRCCTRL every time you wish to reference them (note that they are
@c protected class members), though you must still use @c XRCID to refer to
widget IDs in the event table.

Example:

@code
#include "resource.h"

class TestWnd : public TestWnd_Base
{
public:
    TestWnd()
    {
        // A, B already initialised at this point
        A->SetValue("Updated in TestWnd::TestWnd");
        B->SetValue("Nice :)");
    }
    void OnBPressed(wxEvent& event)
    {
        Close();
    }
    DECLARE_EVENT_TABLE();
};

BEGIN_EVENT_TABLE(TestWnd,TestWnd_Base)
    EVT_BUTTON(XRCID("B"), TestWnd::OnBPressed)
END_EVENT_TABLE()
@endcode

It is also possible to access the wxSizerItem of a sizer that is part of a
resource. This can be done using @c XRCSIZERITEM as shown.

The resource file can have something like this for a sizer item.

@code
<object class="spacer" name="area">
  <size>400, 300</size>
</object>
@endcode

The code can then access the sizer item by using @c XRCSIZERITEM and @c XRCID
together.

@code
wxSizerItem* item = XRCSIZERITEM(*this, "area");
@endcode


@section overview_xrc_newresourcehandlers Adding New Resource Handlers

Adding a new resource handler is pretty easy.

Typically, to add an handler for the @c MyControl class, you'll want to create
the @c xh_mycontrol.h and @c xh_mycontrol.cpp files.

The header needs to contains the @c MyControlXmlHandler class definition:

@code
class MyControlXmlHandler : public wxXmlResourceHandler
{
public:
    // Constructor.
    MyControlXmlHandler();

    // Creates the control and returns a pointer to it.
    virtual wxObject *DoCreateResource();

    // Returns true if we know how to create a control for the given node.
    virtual bool CanHandle(wxXmlNode *node);

    // Register with wxWidgets' dynamic class subsystem.
    DECLARE_DYNAMIC_CLASS(MyControlXmlHandler)
};
@endcode

The implementation of your custom XML handler will typically look as:

@code
// Register with wxWidgets' dynamic class subsystem.
IMPLEMENT_DYNAMIC_CLASS(MyControlXmlHandler, wxXmlResourceHandler)

MyControlXmlHandler::MyControlXmlHandler()
{
    // this call adds support for all wxWindows class styles
    // (e.g. wxBORDER_SIMPLE, wxBORDER_SUNKEN, wxWS_EX_* etc etc)
    AddWindowStyles();

    // if MyControl class supports e.g. MYCONTROL_DEFAULT_STYLE
    // you should use:
    //     XRC_ADD_STYLE(MYCONTROL_DEFAULT_STYLE);
}

wxObject *MyControlXmlHandler::DoCreateResource()
{
    // the following macro will init a pointer named "control"
    // with a new instance of the MyControl class, but will NOT
    // Create() it!
    XRC_MAKE_INSTANCE(control, MyControl)

    // this is the point where you'll typically need to do the most
    // important changes: here the control is created and initialized.
    // You'll want to use the wxXmlResourceHandler's getters to
    // do most of your work.
    // If e.g. the MyControl::Create function looks like:
    //
    // bool MyControl::Create(wxWindow *parent, int id,
    //                        const wxBitmap &first, const wxPoint &posFirst,
    //                        const wxBitmap &second, const wxPoint &posSecond,
    //                        const wxString &theTitle, const wxFont &titleFont,
    //                        const wxPoint &pos, const wxSize &size,
    //                        long style = MYCONTROL_DEFAULT_STYLE,
    //                        const wxString &name = wxT("MyControl"));
    //
    // Then the XRC for your component should look like:
    //
    //    <object class="MyControl" name="some_name">
    //      <first-bitmap>first.xpm</first-bitmap>
    //      <second-bitmap>text.xpm</second-bitmap>
    //      <first-pos>3,3</first-pos>
    //      <second-pos>4,4</second-pos>
    //      <the-title>a title</the-title>
    //      <title-font>
    //        <!-- Standard XRC tags for a font: <size>, <style>, <weight>, etc -->
    //      </title-font>
    //      <!-- XRC also accepts other usual tags for wxWindow-derived classes:
    //           like e.g. <name>, <style>, <size>, <position>, etc -->
    //    </object>
    //
    // And the code to read your custom tags from the XRC file is just:
    control->Create(m_parentAsWindow, GetID(),
                    GetBitmap(wxT("first-bitmap")),
                    GetPosition(wxT("first-pos")),
                    GetBitmap(wxT("second-bitmap")),
                    GetPosition(wxT("second-pos")),
                    GetText(wxT("the-title")),
                    GetFont(wxT("title-font")),
                    GetPosition(), GetSize(), GetStyle(), GetName());

    SetupWindow(control);

    return control;
}

bool MyControlXmlHandler::CanHandle(wxXmlNode *node)
{
    // this function tells XRC system that this handler can parse
    // the <object class="MyControl"> tags
    return IsOfClass(node, wxT("MyControl"));
}
@endcode

You may want to check the wxXmlResourceHandler documentation to see how many
built-in getters it contains. It's very easy to retrieve also complex
structures out of XRC files using them.

*/

