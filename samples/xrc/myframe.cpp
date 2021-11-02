//-----------------------------------------------------------------------------
// Name:        myframe.cpp
// Purpose:     XML resources sample: A derived frame, called MyFrame
// Author:      Robert O'Connor (rob@medicalmnemonics.com), Vaclav Slavik
// Copyright:   (c) Robert O'Connor and Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard wxWidgets headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"


// For all others, include the necessary headers (this file is usually all you
// need because it includes almost all "standard" wxWidgets headers)
#ifndef WX_PRECOMP
    #include "wx/wx.h"
#endif

#include "wx/sysopt.h"

//-----------------------------------------------------------------------------
// Header of this .cpp file
//-----------------------------------------------------------------------------

#include "myframe.h"

//-----------------------------------------------------------------------------
// Remaining headers: Needed wx headers, then wx/contrib headers, then application headers
//-----------------------------------------------------------------------------

// Since setting an icon
#include "wx/image.h"

//-----------------------------------------------------------------------------

#include "wx/xrc/xmlres.h"              // XRC XML resources

//-----------------------------------------------------------------------------

// Our derived dialog for the derived dialog example.
#include "derivdlg.h"
// Our custom class, for the custom class example.
#include "custclas.h"
// And our objref dialog, for the object reference and ID range example.
#include "objrefdlg.h"
// For functions to manipulate the corresponding controls.
#include "wx/animate.h"
#include "wx/generic/animate.h"
#include "wx/infobar.h"
#include "wx/treectrl.h"
#include "wx/listctrl.h"

//-----------------------------------------------------------------------------
// Regular resources (the non-XRC kind).
//-----------------------------------------------------------------------------

// the application icon (under Windows it is in resources and even
// though we could still include the XPM here it would be unused)
#ifndef wxHAS_IMAGES_IN_RESOURCES
    #include "../sample.xpm"
#endif

//-----------------------------------------------------------------------------
// Event table: connect the events to the handler functions to process them
//-----------------------------------------------------------------------------

// The event tables connect the wxWidgets events with the functions (event
// handlers) which process them. It can be also done at run-time, but for the
// simple menu events like this the static method is much simpler.
// The reason why the menuitems and tools are given the same name in the
// XRC file, is that both a tool (a toolbar item) and a menuitem are designed
// to fire the same kind of event (an EVT_MENU) and thus I give them the same
// ID name to help new users emphasize this point which is often overlooked
// when starting out with wxWidgets.
wxBEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(XRCID("unload_resource_menuitem"), MyFrame::OnUnloadResourceMenuCommand)
    EVT_MENU(XRCID("reload_resource_menuitem"), MyFrame::OnReloadResourceMenuCommand)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExitToolOrMenuCommand)
    EVT_MENU(XRCID("non_derived_dialog_tool_or_menuitem"), MyFrame::OnNonDerivedDialogToolOrMenuCommand)
    EVT_MENU(XRCID("derived_tool_or_menuitem"), MyFrame::OnDerivedDialogToolOrMenuCommand)
    EVT_MENU(XRCID("controls_tool_or_menuitem"), MyFrame::OnControlsToolOrMenuCommand)
    EVT_MENU(XRCID("uncentered_tool_or_menuitem"), MyFrame::OnUncenteredToolOrMenuCommand)
    EVT_MENU(XRCID("multiple_accels"), MyFrame::OnMultipleAccels)
    EVT_MENU(XRCID("aui_demo_tool_or_menuitem"), MyFrame::OnAuiDemoToolOrMenuCommand)
    EVT_MENU(XRCID("obj_ref_tool_or_menuitem"), MyFrame::OnObjRefToolOrMenuCommand)
    EVT_MENU(XRCID("custom_class_tool_or_menuitem"), MyFrame::OnCustomClassToolOrMenuCommand)
    EVT_MENU(XRCID("platform_property_tool_or_menuitem"), MyFrame::OnPlatformPropertyToolOrMenuCommand)
    EVT_MENU(XRCID("art_provider_tool_or_menuitem"), MyFrame::OnArtProviderToolOrMenuCommand)
    EVT_MENU(XRCID("variable_expansion_tool_or_menuitem"), MyFrame::OnVariableExpansionToolOrMenuCommand)
    EVT_MENU(XRCID("variants"), MyFrame::OnVariants)
    EVT_MENU(XRCID("recursive_load"), MyFrame::OnRecursiveLoad)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAboutToolOrMenuCommand)
wxEND_EVENT_TABLE()

//-----------------------------------------------------------------------------
// Public methods
//-----------------------------------------------------------------------------

// Constructor
MyFrame::MyFrame(wxWindow* parent)
{
    // Load up this frame from XRC. [Note, instead of making a class's
    // constructor take a wxWindow* parent with a default value of NULL,
    // we could have just had designed MyFrame class with an empty
    // constructor and then written here:
    // wxXmlResource::Get()->LoadFrame(this, (wxWindow* )NULL, "main_frame");
    // since this frame will always be the top window, and thus parentless.
    // However, the current approach has source code that can be recycled
    // for other frames that aren't the top level window.]
    wxXmlResource::Get()->LoadFrame(this, parent, "main_frame");

    // Set the icon for the frame.
    SetIcon(wxICON(sample));

    // Load the menubar from XRC and set this frame's menubar to it.
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar("main_menu"));
    // Load the toolbar from XRC and set this frame's toolbar to it.
    // NOTE: For toolbars you currently should do it exactly like this.
    // With toolbars, you currently can't create one, and set it later. It
    // needs to be all in one step.
    wxSystemOptions::SetOption ( "msw.remap", 0 );
    SetToolBar(wxXmlResource::Get()->LoadToolBar(this, "main_toolbar"));

#if wxUSE_STATUSBAR
    // Give the frame an optional statusbar. The '1' just means one field.
    // A gripsizer will automatically get put on into the corner, if that
    // is the normal OS behaviour for frames on that platform. Helptext
    // for menu items and toolbar tools will automatically get displayed
    // here.
    CreateStatusBar( 1 );
#endif // wxUSE_STATUSBAR

    // As we have created the toolbar and status bar after loading the main
    // frame from resources, we need to readjust its minimal size to fit both
    // its client area contains and the bars.
    GetSizer()->SetSizeHints(this);
}

//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------

void MyFrame::OnUnloadResourceMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    if ( wxXmlResource::Get()->Unload("rc/basicdlg.xrc") )
    {
        wxLogMessage("Basic dialog resource has now been unloaded, you "
                     "won't be able to use it before loading it again");
    }
    else
    {
        wxLogWarning("Failed to unload basic dialog resource");
    }
}

void MyFrame::OnReloadResourceMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    if ( wxXmlResource::Get()->Load("rc/basicdlg.xrc") )
    {
        wxLogStatus("Basic dialog resource has been loaded.");
    }
    else
    {
        wxLogError("Failed to load basic dialog resource");
    }
}

void MyFrame::OnExitToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    // true is to force the frame to close.
    Close(true);
}


void MyFrame::OnNonDerivedDialogToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    // "non_derived_dialog" is the name of the wxDialog XRC node that should
    // be loaded.
    if ( wxXmlResource::Get()->LoadDialog(&dlg, this, "non_derived_dialog") )
        dlg.ShowModal();
}


void MyFrame::OnDerivedDialogToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    // Make an instance of our derived dialog, passing it "this" window
    // (the main frame) as the parent of the dialog. This allows the dialog
    // to be destructed automatically when the parent is destroyed.
    PreferencesDialog preferencesDialog(this);
    // Show the instance of the dialog, modally.
    preferencesDialog.ShowModal();
}

void MyFrame::OnAnimationCtrlPlay(wxCommandEvent& event)
{
#if wxUSE_ANIMATIONCTRL
    // get the pointers we need
    wxButton *btn = wxDynamicCast(event.GetEventObject(), wxButton);
    if (!btn || !btn->GetParent()) return;

    wxWindow *win = btn->GetParent();
    wxAnimationCtrl *ctrl = XRCCTRL(*win, "controls_animation_ctrl", wxAnimationCtrl);
    wxGenericAnimationCtrl *generic =
        XRCCTRL(*win, "controls_generic_animation_ctrl", wxGenericAnimationCtrl);
    if (ctrl->IsPlaying())
    {
        ctrl->Stop();
        generic->Stop();
        btn->SetLabel("Play");
    }
    else
    {
        if ( ctrl->Play() && generic->Play() )
            btn->SetLabel("Stop");
        else
            wxLogError("Cannot play the animation...");
    }
#endif
}

void MyFrame::OnControlsToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "controls_dialog");

#if wxUSE_LISTCTRL
    // The resource file specifies the columns of the control as they are
    // typically static while the items themselves are added from here as
    // usually they are not static (but if they are, they can be defined in the
    // resources too, see the two other list controls definitions in
    // controls.xrc)

    // Insert some items into the listctrl: notice that we can access it using
    // XRCCTRL
    wxListCtrl * const list = XRCCTRL(dlg, "controls_listctrl", wxListCtrl);

    list->InsertItem(0, "Athos", 0);   list->SetItem(0, 1, "90", 2);
    list->InsertItem(1, "Porthos", 5); list->SetItem(1, 1, "120", 3);
    list->InsertItem(2, "Aramis", 1);  list->SetItem(2, 1, "80", 4);
#endif // wxUSE_LISTCTRL

#if wxUSE_TREECTRL
    // There is no data in the tree ctrl. These lines will add some.
    // (1) Instead of having to write out
    // XRCCTRL(dlg, "controls_treectrl", wxTreeCtrl)->SomeFunction()
    // each time (which is also OK), this example code will shown how
    // to make a pointer to the XRC control, so we can use
    // treectrl->SomeFunction() as a short cut. This is useful if you
    // will be referring to this control often in the code.
    wxTreeCtrl* treectrl = XRCCTRL(dlg, "controls_treectrl", wxTreeCtrl);
    // (2) Add a root node
    treectrl->AddRoot(_("Godfather"));
    // (3)Append some items to the root node.
    treectrl->AppendItem(treectrl->GetRootItem(), _("Evil henchman 1"));
    treectrl->AppendItem(treectrl->GetRootItem(), _("Evil henchman 2"));
    treectrl->AppendItem(treectrl->GetRootItem(), _("Evil accountant"));
#endif

#if wxUSE_ANIMATIONCTRL
    // dynamically connect our event handler for the "clicked" event of the "play" button
    // in the animation ctrl page of our dialog
    dlg.Bind(wxEVT_BUTTON, &MyFrame::OnAnimationCtrlPlay, this,
             XRCID("controls_animation_button_play"));
#endif

#if wxUSE_INFOBAR
    // Show the message on button click
    dlg.Bind(wxEVT_BUTTON, &MyFrame::OnInfoBarShowMessage, this,
        XRCID("controls_infobar_button_message"));
#endif

    // All done. Show the dialog.
    dlg.ShowModal();
}


void MyFrame::OnUncenteredToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "uncentered_dialog");
    dlg.ShowModal();
}

void MyFrame::OnMultipleAccels(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
#if defined(__WXOSX_COCOA__)
    wxString main = "Cmd-W";
    wxString extra1 = "Cmd-T";
    wxString extra2 = "Shift-Cmd-W";
#else
    wxString main = "Ctrl-W";
    wxString extra1 = "Ctrl-T";
    wxString extra2 = "Shift-Ctrl-W";
#endif
    msg.Printf(
        "You can open this dialog with any of '%s' (main), '%s' or '%s' (extra) accelerators.",
        main, extra1, extra2
    );

    wxMessageBox(msg, _("Multiple accelerators demo"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnAuiDemoToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
#if wxUSE_AUI
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "aui_dialog");
    dlg.ShowModal();
#else
    wxLogWarning("wxUSE_AUI must be set to 1 in 'setup.h' to view the AUI demo.");
#endif
}

void MyFrame::OnObjRefToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    // The dialog redirects log messages, so save the old log target first
    wxLog* oldlogtarget = wxLog::SetActiveTarget(NULL);

    // Make an instance of the dialog
    ObjrefDialog* objrefDialog = new ObjrefDialog(this);
    // Show the instance of the dialog, modally.
    objrefDialog->ShowModal();
    objrefDialog->Destroy();

    // Restore the old log target
    delete wxLog::SetActiveTarget(oldlogtarget);
}


void MyFrame::OnCustomClassToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "custom_class_dialog");

    // Make an instance of our new custom class.
    MyResizableListCtrl* a_myResizableListCtrl = new MyResizableListCtrl(&dlg,
                                                wxID_ANY,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                wxLC_REPORT,
                                                wxDefaultValidator);

    // "custom_control_placeholder" is the name of the "unknown" tag in the
    // custctrl.xrc XRC file.
    wxXmlResource::Get()->AttachUnknownControl("custom_control_placeholder",
                                                a_myResizableListCtrl);
    dlg.ShowModal();
}


void MyFrame::OnPlatformPropertyToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "platform_property_dialog");
    dlg.ShowModal();
}


void MyFrame::OnArtProviderToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "art_provider_dialog");
    dlg.ShowModal();
}


void MyFrame::OnVariableExpansionToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "variable_expansion_dialog");
    dlg.ShowModal();
}

void MyFrame::OnVariants(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, "variants_dialog");
    dlg.ShowModal();
}

void MyFrame::OnRecursiveLoad(wxCommandEvent& WXUNUSED(event))
{
    // this dialog is created manually to show how you can inject a single
    // control from XRC into an existing dialog
    //
    // this is a slightly contrived example, please keep in mind that it's done
    // only to demonstrate LoadObjectRecursively() in action and is not the
    // recommended to do this
    wxDialog dlg(NULL, wxID_ANY, "Recursive Load Example",
                 wxDefaultPosition, wxDefaultSize,
                 wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER);
    wxSizer * const sizer = new wxBoxSizer(wxVERTICAL);
    sizer->Add
    (
        new wxStaticText
        (
            &dlg,
            wxID_ANY,
            "The entire tree book control below is loaded from XRC"
        ),
        wxSizerFlags().Expand().Border()
    );

    sizer->Add
    (
        static_cast<wxWindow *>
        (
            // notice that controls_treebook is defined inside a notebook page
            // inside a dialog defined in controls.xrc and so LoadObject()
            // wouldn't find it -- but LoadObjectRecursively() does
            wxXmlResource::Get()->
                LoadObjectRecursively(&dlg, "controls_treebook", "wxTreebook")
        ),
        wxSizerFlags(1).Expand().Border()
    );

    dlg.SetSizer(sizer);
    dlg.SetClientSize(400, 200);

    dlg.ShowModal();
}

void MyFrame::OnAboutToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxString msg;
    msg.Printf( "This is the about dialog of XML resources demo.\n"
                "Welcome to %s", wxVERSION_STRING);

    wxMessageBox(msg, _("About XML resources demo"), wxOK | wxICON_INFORMATION, this);
}

void MyFrame::OnInfoBarShowMessage(wxCommandEvent& event)
{
#if wxUSE_INFOBAR
    // get the pointers we need
    wxButton *btn = wxDynamicCast(event.GetEventObject(), wxButton);
    if ( !btn || !btn->GetParent() )
        return;

    wxWindow *win = btn->GetParent();
    wxInfoBar *ctrl = XRCCTRL(*win, "controls_infobar", wxInfoBar);
    ctrl->ShowMessage("Message", wxICON_QUESTION);
#endif

}
