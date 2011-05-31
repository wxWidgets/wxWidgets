//-----------------------------------------------------------------------------
// Name:        myframe.cpp
// Purpose:     XML resources sample: A derived frame, called MyFrame
// Author:      Robert O'Connor (rob@medicalmnemonics.com), Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Robert O'Connor and Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Standard wxWidgets headers
//-----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx/wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

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

#include "wx/xrc/xmlres.h"              // XRC XML resouces

//-----------------------------------------------------------------------------

// Our derived dialog for the derived dialog example.
#include "derivdlg.h"
// Our custom class, for the custom class example.
#include "custclas.h"
// And our objref dialog, for the object reference and ID range example.
#include "objrefdlg.h"
// For functions to manipulate our wxTreeCtrl and wxListCtrl
#include "wx/treectrl.h"
#include "wx/listctrl.h"

//-----------------------------------------------------------------------------
// Regular resources (the non-XRC kind).
//-----------------------------------------------------------------------------

// the application icon (under Windows and OS/2 it is in resources and even
// though we could still include the XPM here it would be unused)
#if !defined(__WXMSW__) && !defined(__WXPM__)
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
BEGIN_EVENT_TABLE(MyFrame, wxFrame)
    EVT_MENU(XRCID("unload_resource_menuitem"), MyFrame::OnUnloadResourceMenuCommand)
    EVT_MENU(XRCID("reload_resource_menuitem"), MyFrame::OnReloadResourceMenuCommand)
    EVT_MENU(wxID_EXIT,  MyFrame::OnExitToolOrMenuCommand)
    EVT_MENU(XRCID("non_derived_dialog_tool_or_menuitem"), MyFrame::OnNonDerivedDialogToolOrMenuCommand)
    EVT_MENU(XRCID("derived_tool_or_menuitem"), MyFrame::OnDerivedDialogToolOrMenuCommand)
    EVT_MENU(XRCID("controls_tool_or_menuitem"), MyFrame::OnControlsToolOrMenuCommand)
    EVT_MENU(XRCID("uncentered_tool_or_menuitem"), MyFrame::OnUncenteredToolOrMenuCommand)
    EVT_MENU(XRCID("obj_ref_tool_or_menuitem"), MyFrame::OnObjRefToolOrMenuCommand)
    EVT_MENU(XRCID("custom_class_tool_or_menuitem"), MyFrame::OnCustomClassToolOrMenuCommand)
    EVT_MENU(XRCID("platform_property_tool_or_menuitem"), MyFrame::OnPlatformPropertyToolOrMenuCommand)
    EVT_MENU(XRCID("art_provider_tool_or_menuitem"), MyFrame::OnArtProviderToolOrMenuCommand)
    EVT_MENU(XRCID("variable_expansion_tool_or_menuitem"), MyFrame::OnVariableExpansionToolOrMenuCommand)
    EVT_MENU(XRCID("recursive_load"), MyFrame::OnRecursiveLoad)
    EVT_MENU(wxID_ABOUT, MyFrame::OnAboutToolOrMenuCommand)
END_EVENT_TABLE()

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
    wxXmlResource::Get()->LoadFrame(this, parent, wxT("main_frame"));

    // Set the icon for the frame.
    SetIcon(wxICON(sample));

    // Load the menubar from XRC and set this frame's menubar to it.
    SetMenuBar(wxXmlResource::Get()->LoadMenuBar(wxT("main_menu")));
    // Load the toolbar from XRC and set this frame's toolbar to it.
    // NOTE: For toolbars you currently should do it exactly like this.
    // With toolbars, you currently can't create one, and set it later. It
    // needs to be all in one step.
    wxSystemOptions::SetOption ( wxT("msw.remap"), 0 );
    SetToolBar(wxXmlResource::Get()->LoadToolBar(this, wxT("main_toolbar")));

#if wxUSE_STATUSBAR
    // Give the frame an optional statusbar. The '1' just means one field.
    // A gripsizer will automatically get put on into the corner, if that
    // is the normal OS behaviour for frames on that platform. Helptext
    // for menu items and toolbar tools will automatically get displayed
    // here.
    CreateStatusBar( 1 );
#endif // wxUSE_STATUSBAR
}

//-----------------------------------------------------------------------------
// Private methods
//-----------------------------------------------------------------------------

void MyFrame::OnUnloadResourceMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    if ( wxXmlResource::Get()->Unload(wxT("rc/basicdlg.xrc")) )
    {
        wxLogMessage(wxT("Basic dialog resource has now been unloaded, you ")
                     wxT("won't be able to use it before loading it again"));
    }
    else
    {
        wxLogWarning(wxT("Failed to unload basic dialog resource"));
    }
}

void MyFrame::OnReloadResourceMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    if ( wxXmlResource::Get()->Load(wxT("rc/basicdlg.xrc")) )
    {
        wxLogStatus(wxT("Basic dialog resource has been loaded."));
    }
    else
    {
        wxLogError(wxT("Failed to load basic dialog resource"));
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
    if ( wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("non_derived_dialog")) )
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
    if (ctrl->IsPlaying())
    {
        ctrl->Stop();
        btn->SetLabel(wxT("Play"));
    }
    else
    {
        if (ctrl->Play())
            btn->SetLabel(wxT("Stop"));
        else
            wxLogError(wxT("Cannot play the animation..."));
    }
#endif
}

void MyFrame::OnControlsToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("controls_dialog"));

#if wxUSE_LISTCTRL
    // The resource file specifies the columns of the control as they are
    // typically static while the items themselves are added from here as
    // usually they are not static (but if they are, they can be defined in the
    // resources too, see the two other list controls definitions in
    // controls.xrc)

    // Insert some items into the listctrl: notice that we can access it using
    // XRCCTRL
    wxListCtrl * const list = XRCCTRL(dlg, "controls_listctrl", wxListCtrl);

    list->InsertItem(0, "Athos");   list->SetItem(0, 1, "90");
    list->InsertItem(1, "Porthos"); list->SetItem(1, 1, "120");
    list->InsertItem(2, "Aramis");  list->SetItem(2, 1, "80");
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
    dlg.Connect(XRCID("controls_animation_button_play"), wxEVT_COMMAND_BUTTON_CLICKED,
                wxCommandEventHandler(MyFrame::OnAnimationCtrlPlay));
#endif

    // All done. Show the dialog.
    dlg.ShowModal();
}


void MyFrame::OnUncenteredToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("uncentered_dialog"));
    dlg.ShowModal();
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
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("custom_class_dialog"));

    // Make an instance of our new custom class.
    MyResizableListCtrl* a_myResizableListCtrl = new MyResizableListCtrl(&dlg,
                                                wxID_ANY,
                                                wxDefaultPosition,
                                                wxDefaultSize,
                                                wxLC_REPORT,
                                                wxDefaultValidator);

    // "custom_control_placeholder" is the name of the "unknown" tag in the
    // custctrl.xrc XRC file.
    wxXmlResource::Get()->AttachUnknownControl(wxT("custom_control_placeholder"),
                                                a_myResizableListCtrl);
    dlg.ShowModal();
}


void MyFrame::OnPlatformPropertyToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("platform_property_dialog"));
    dlg.ShowModal();
}


void MyFrame::OnArtProviderToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("art_provider_dialog"));
    dlg.ShowModal();
}


void MyFrame::OnVariableExpansionToolOrMenuCommand(wxCommandEvent& WXUNUSED(event))
{
    wxDialog dlg;
    wxXmlResource::Get()->LoadDialog(&dlg, this, wxT("variable_expansion_dialog"));
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
    msg.Printf( wxT("This is the about dialog of XML resources demo.\n")
                wxT("Welcome to %s"), wxVERSION_STRING);

    wxMessageBox(msg, _("About XML resources demo"), wxOK | wxICON_INFORMATION, this);
}
