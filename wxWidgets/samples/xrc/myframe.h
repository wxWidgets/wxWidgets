//-----------------------------------------------------------------------------
// Name:        myframe.h
// Purpose:     XML resources sample: A derived frame, called MyFrame
// Author:      Robert O'Connor (rob@medicalmnemonics.com), Vaclav Slavik
// RCS-ID:      $Id$
// Copyright:   (c) Robert O'Connor and Vaclav Slavik
// Licence:     wxWindows licence
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Begin single inclusion of this .h file condition
//-----------------------------------------------------------------------------

#ifndef _MYFRAME_H_
#define _MYFRAME_H_

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------

#include "wx/frame.h"

//-----------------------------------------------------------------------------
// Class definition: MyFrame
//-----------------------------------------------------------------------------

// Define a new frame type: this is going to be our main frame
class MyFrame : public wxFrame
{

public:

    // Constructor.
    MyFrame( wxWindow* parent=(wxWindow *)NULL);

private:

    // Event handlers (these functions should _not_ be virtual)
    void OnUnloadResourceMenuCommand(wxCommandEvent& event);
    void OnReloadResourceMenuCommand(wxCommandEvent& event);
    void OnExitToolOrMenuCommand(wxCommandEvent& event);
    void OnAboutToolOrMenuCommand(wxCommandEvent& event);
    void OnNonDerivedDialogToolOrMenuCommand(wxCommandEvent& event);
    void OnDerivedDialogToolOrMenuCommand(wxCommandEvent& event);
    void OnControlsToolOrMenuCommand(wxCommandEvent& event);
    void OnUncenteredToolOrMenuCommand(wxCommandEvent& event);
    void OnCustomClassToolOrMenuCommand(wxCommandEvent& event);
    void OnPlatformPropertyToolOrMenuCommand(wxCommandEvent& event);
    void OnArtProviderToolOrMenuCommand(wxCommandEvent& event);
    void OnVariableExpansionToolOrMenuCommand(wxCommandEvent& event);
    void OnAnimationCtrlPlay(wxCommandEvent& event);

    // Any class wishing to process wxWidgets events must use this macro
    DECLARE_EVENT_TABLE()

};

//-----------------------------------------------------------------------------
// End single inclusion of this .h file condition
//-----------------------------------------------------------------------------

#endif  // _MYFRAME_H_
