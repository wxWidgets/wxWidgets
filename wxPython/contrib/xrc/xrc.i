/////////////////////////////////////////////////////////////////////////////
// Name:        xrc.i
// Purpose:     Wrappers for the XML based Resource system
//
// Author:      Robin Dunn
//
// Created:     4-June-2001
// RCS-ID:      $Id$
// Copyright:   (c) 2001 by Total Control Software
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

%module xrc


%{
#include "export.h"
#include "wx/xrc/xmlres.h"
%}

//---------------------------------------------------------------------------

%include typemaps.i
%include my_typemaps.i

%extern wx.i
%extern windows.i
%extern _defs.i
%extern events.i
%extern controls.i


//---------------------------------------------------------------------------

enum wxXmlResourceFlags
{
    wxXRC_USE_LOCALE     = 1,
    wxXRC_NO_SUBCLASSING = 2
};


// This class holds XML resources from one or more .xml files
// (or derived forms, either binary or zipped -- see manual for
// details).

class wxXmlResource : public wxObject
{
public:
    // Ctor.
    // Flags: wxXRC_USE_LOCALE
    //              translatable strings will be translated via _()
    //        wxXRC_NO_SUBCLASSING
    //              subclass property of object nodes will be ignored
    //              (useful for previews in XRC editors)
    %name(wxXmlResourceEmpty)wxXmlResource(int flags = wxXRC_USE_LOCALE);   // TODO, a better %name

    %addmethods {
        wxXmlResource(const wxString* filemask, int flags = wxXRC_USE_LOCALE) {
            wxXmlResource* res = new wxXmlResource(*filemask, flags);
            res->InitAllHandlers();
            return res;
        }
    }

    ~wxXmlResource();


    // Loads resources from XML files that match given filemask.
    // This method understands VFS (see filesys.h).
    bool Load(const wxString& filemask);

    // Initialize handlers for all supported controls/windows. This will
    // make the executable quite big because it forces linking against
    // most of wxWin library
    void InitAllHandlers();

    // Initialize only specific handler (or custom handler). Convention says
    // that handler name is equal to control's name plus 'XmlHandler', e.g.
    // wxTextCtrlXmlHandler, wxHtmlWindowXmlHandler. XML resource compiler
    // (xmlres) can create include file that contains initialization code for
    // all controls used within the resource.
    void AddHandler(wxXmlResourceHandler *handler);

    // Removes all handlers
    void ClearHandlers();

    // Loads menu from resource. Returns NULL on failure.
    wxMenu *LoadMenu(const wxString& name);

    // Loads menubar from resource. Returns NULL on failure.
    wxMenuBar *LoadMenuBar(const wxString& name);

    // Loads toolbar
    wxToolBar *LoadToolBar(wxWindow *parent, const wxString& name);

    // Loads dialog. dlg points to parent window (if any). Second form
    // is used to finish creation of already existing instance (main reason
    // for this is that you may want to use derived class with new event table)
    // Example (typical usage):
    //      MyDialog dlg;
    //      wxTheXmlResource->LoadDialog(&dlg, mainFrame, "my_dialog");
    //      dlg->ShowModal();
    wxDialog *LoadDialog(wxWindow *parent, const wxString& name);
    %name(LoadOnDialog)bool LoadDialog(wxDialog *dlg, wxWindow *parent, const wxString& name);

    // Loads panel. panel points to parent window (if any). Second form
    // is used to finish creation of already existing instance.
    wxPanel *LoadPanel(wxWindow *parent, const wxString& name);
    %name(LoadOnPanel)bool LoadPanel(wxPanel *panel, wxWindow *parent, const wxString& name);

    bool LoadFrame(wxFrame* frame, wxWindow *parent, const wxString& name);

    // Loads bitmap or icon resource from file:
    wxBitmap LoadBitmap(const wxString& name);
    wxIcon LoadIcon(const wxString& name);

    // Attaches unknown control into given panel/window/dialog:
    // (unknown controls are used in conjunction with <object class="unknown">)
    bool AttachUnknownControl(const wxString& name, wxWindow *control,
                              wxWindow *parent = NULL);

    // Returns numeric ID that is equivalent to string id used in XML
    // resource. To be used in event tables
    // Macro XMLID is provided for convenience
    static int GetXRCID(const wxString& str_id);

    // Returns version info (a.b.c.d = d+ 256*c + 256^2*b + 256^3*a)
    long GetVersion() const;

    // Compares resources version to argument. Returns -1 if resources version
    // is less than the argument, +1 if greater and 0 if they equal.
    int CompareVersion(int major, int minor, int release, int revision) const;


    // Gets global resources object or create one if none exists
    static wxXmlResource *Get();
    // Sets global resources object and returns pointer to previous one (may be NULL).
    static wxXmlResource *Set(wxXmlResource *res);

};

//----------------------------------------------------------------------

%pragma(python) code = "
def XRCID(str_id):
    return wxXmlResource_GetXRCID(str_id)

def XRCCTRL(window, str_id, *ignoreargs):
    return window.FindWindowById(XRCID(str_id))

XMLID = XRCID
XMLCTRL = XRCCTRL
"

//----------------------------------------------------------------------

// TODO:  Add wxXmlResourceHandler and allow it to be derived from.

//----------------------------------------------------------------------

%init %{

    wxClassInfo::CleanUpClasses();
    wxClassInfo::InitializeClasses();

    wxXmlInitResourceModule();
    wxXmlResource::Get()->InitAllHandlers();

%}

//----------------------------------------------------------------------
// This file gets appended to the shadow class file.
//----------------------------------------------------------------------

%pragma(python) include="_xrcextras.py";


