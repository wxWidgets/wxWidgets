/////////////////////////////////////////////////////////////////////////////
// Name:        _cshelp.i
// Purpose:     Context sensitive help classes, and etc.
//
// Author:      Robin Dunn
//
// Created:     28-July-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// not a %module

//---------------------------------------------------------------------------
%newgroup

%{
%}

//----------------------------------------------------------------------

%constant wxEventType wxEVT_HELP;
%constant wxEventType wxEVT_DETAILED_HELP;


%pythoncode { 
EVT_HELP = wx.PyEventBinder( wxEVT_HELP, 1)
EVT_HELP_RANGE = wx.PyEventBinder(  wxEVT_HELP, 2)
EVT_DETAILED_HELP = wx.PyEventBinder( wxEVT_DETAILED_HELP, 1)
EVT_DETAILED_HELP_RANGE = wx.PyEventBinder( wxEVT_DETAILED_HELP, 2)
}

//----------------------------------------------------------------------

DocStr(wxHelpEvent,
"A help event is sent when the user has requested context-sensitive
help. This can either be caused by the application requesting
context-sensitive help mode via wx.ContextHelp, or (on MS Windows) by
the system generating a WM_HELP message when the user pressed F1 or
clicked on the query button in a dialog caption.

A help event is sent to the window that the user clicked on, and is
propagated up the window hierarchy until the event is processed or
there are no more event handlers. The application should call
event.GetId to check the identity of the clicked-on window, and then
either show some suitable help or call event.Skip if the identifier is
unrecognised. Calling Skip is important because it allows wxWindows to
generate further events for ancestors of the clicked-on
window. Otherwise it would be impossible to show help for container
windows, since processing would stop after the first window found.",
"

Events
-------
    ==============      =========================================
    EVT_HELP            Sent when the user has requested context-
                        sensitive help.
    EVT_HELP_RANGE      Allows to catch EVT_HELP for a range of IDs
    ==============      =========================================

:see: `wx.ContextHelp`, `wx.ContextHelpButton`
");


class wxHelpEvent : public wxCommandEvent
{
public:
    // how was this help event generated?
    enum Origin
    {
        Origin_Unknown,    // unrecognized event source
        Origin_Keyboard,   // event generated from F1 key press
        Origin_HelpButton  // event from [?] button on the title bar (Windows)
    };

    DocCtorStr(
        wxHelpEvent(wxEventType type = wxEVT_NULL,
                    wxWindowID winid = 0,
                    const wxPoint& pt = wxDefaultPosition,
                    Origin origin = Origin_Unknown ),
        "", "");
    

    DocDeclStr(
        const wxPoint , GetPosition() const,
        "Returns the left-click position of the mouse, in screen
coordinates. This allows the application to position the help
appropriately.", "");
    
    DocDeclStr(
        void , SetPosition(const wxPoint& pos),
        "Sets the left-click position of the mouse, in screen coordinates.", "");
    

    DocDeclStr(
        const wxString& , GetLink() const,
        "Get an optional link to further help", "");
    
    DocDeclStr(
        void , SetLink(const wxString& link),
        "Set an optional link to further help", "");
    

    DocDeclStr(
        const wxString& , GetTarget() const,
        "Get an optional target to display help in. E.g. a window specification", "");
    
    DocDeclStr(
        void , SetTarget(const wxString& target),
        "Set an optional target to display help in. E.g. a window specification", "");

    // optional indication of the event source
    DocDeclStr(
        Origin , GetOrigin() const,
        "Optiononal indication of the source of the event.", "");
    
    DocDeclStr(
        void , SetOrigin(Origin origin),
        "", "");
        
};

//---------------------------------------------------------------------------


DocStr(wxContextHelp,
"This class changes the cursor to a query and puts the application into
a 'context-sensitive help mode'. When the user left-clicks on a window
within the specified window, a ``EVT_HELP`` event is sent to that
control, and the application may respond to it by popping up some
help.

There are a couple of ways to invoke this behaviour implicitly:

    * Use the wx.WS_EX_CONTEXTHELP extended style for a dialog or frame
      (Windows only). This will put a question mark in the titlebar,
      and Windows will put the application into context-sensitive help
      mode automatically, with further programming.

    * Create a `wx.ContextHelpButton`, whose predefined behaviour is
      to create a context help object. Normally you will write your
      application so that this button is only added to a dialog for
      non-Windows platforms (use ``wx.WS_EX_CONTEXTHELP`` on
      Windows).

:see: `wx.ContextHelpButton`
", "");       

MustHaveApp(wxContextHelp);

class wxContextHelp : public wxObject {
public:
    DocCtorStr(
        wxContextHelp(wxWindow* window = NULL, bool doNow = true),
        "Constructs a context help object, calling BeginContextHelp if doNow is
true (the default).

If window is None, the top window is used.", "");
    
    ~wxContextHelp();

    DocDeclStr(
        bool , BeginContextHelp(wxWindow* window = NULL),
        "Puts the application into context-sensitive help mode. window is the
window which will be used to catch events; if NULL, the top window
will be used.

Returns true if the application was successfully put into
context-sensitive help mode. This function only returns when the event
loop has finished.", "");
    
    DocDeclStr(
        bool , EndContextHelp(),
        "Ends context-sensitive help mode. Not normally called by the
application.", "");
    
};


//----------------------------------------------------------------------

DocStr(wxContextHelpButton,
"Instances of this class may be used to add a question mark button that
when pressed, puts the application into context-help mode. It does
this by creating a wx.ContextHelp object which itself generates a
``EVT_HELP`` event when the user clicks on a window.

On Windows, you may add a question-mark icon to a dialog by use of the
``wx.DIALOG_EX_CONTEXTHELP`` extra style, but on other platforms you
will have to add a button explicitly, usually next to OK, Cancel or
similar buttons.

:see: `wx.ContextHelp`, `wx.ContextHelpButton`
", "");

MustHaveApp(wxContextHelpButton);

class wxContextHelpButton : public wxBitmapButton {
public:
    %pythonAppend wxContextHelpButton "self._setOORInfo(self)"

    DocCtorStr(
        wxContextHelpButton(wxWindow* parent, wxWindowID id = wxID_CONTEXT_HELP,
                            const wxPoint& pos = wxDefaultPosition,
                            const wxSize& size = wxDefaultSize,
                            long style = wxBU_AUTODRAW),
        "Constructor, creating and showing a context help button.", "");
};


//----------------------------------------------------------------------

DocStr(wxHelpProvider,
"wx.HelpProvider is an abstract class used by a program
implementing context-sensitive help to show the help text for the
given window.

The current help provider must be explicitly set by the
application using wx.HelpProvider.Set().", "");

class wxHelpProvider 
{
public:
    
    ~wxHelpProvider();
    
    
    %disownarg( wxHelpProvider *helpProvider );
    %newobject Set;
    DocDeclStr(
        static wxHelpProvider *, Set(wxHelpProvider *helpProvider),
        "Sset the current, application-wide help provider. Returns the previous
one.  Unlike some other classes, the help provider is not created on
demand. This must be explicitly done by the application.", "");
    %cleardisown( wxHelpProvider *helpProvider );
    
    DocDeclStr(
        static wxHelpProvider *, Get(),
        "Return the current application-wide help provider.", "");
    

    DocDeclStr(
        wxString , GetHelp(const wxWindow *window),
        "Gets the help string for this window. Its interpretation is dependent
on the help provider except that empty string always means that no
help is associated with the window.", "");
    
    DocDeclStr(
        bool , ShowHelp(wxWindow *window),
        "Shows help for the given window. Uses GetHelp internally if
applicable. Returns True if it was done, or False if no help was
available for this window.", "");

    DocDeclStr(
        virtual bool , ShowHelpAtPoint(wxWindowBase *window,
                                       const wxPoint& pt,
                                       wxHelpEvent::Origin origin),
        "Show help for the given window (uses window.GetHelpAtPoint()
internally if applicable), return true if it was done or false if no
help available for this window.", "");
    

    
    DocDeclStr(
        void , AddHelp(wxWindow *window, const wxString& text),
        "Associates the text with the given window.", "");

    DocDeclStrName(
        void , AddHelp(wxWindowID id, const wxString& text),
        "This version associates the given text with all windows with this
id. May be used to set the same help string for all Cancel buttons in
the application, for example.", "",
        AddHelpById);

    DocDeclStr(
        void , RemoveHelp(wxWindow* window),
        "Removes the association between the window pointer and the help
text. This is called by the wx.Window destructor. Without this, the
table of help strings will fill up and when window pointers are
reused, the wrong help string will be found.", "");
    
    
    %pythonAppend Destroy "args[0].thisown = 0"
    %extend { void Destroy() { delete self; } }
};

 
//----------------------------------------------------------------------

DocStr(wxSimpleHelpProvider,
"wx.SimpleHelpProvider is an implementation of `wx.HelpProvider` which
supports only plain text help strings, and shows the string associated
with the control (if any) in a tooltip.", "");

class wxSimpleHelpProvider : public wxHelpProvider
{
public:
    wxSimpleHelpProvider();
};


//----------------------------------------------------------------------

// TODO: Add this once the wxHelpController is in wxPython...

//  class WXDLLEXPORT wxHelpControllerHelpProvider : public wxSimpleHelpProvider
//  {
//  public:
//      wxHelpControllerHelpProvider(wxHelpController* hc = NULL);
//      void SetHelpController(wxHelpController* hc);
//      wxHelpController* GetHelpController();
//  };




//----------------------------------------------------------------------
//---------------------------------------------------------------------------
