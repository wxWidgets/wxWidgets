/////////////////////////////////////////////////////////////////////////////
// Name:        common/clipcmn.cpp
// Purpose:     common (to all ports) wxClipboard functions
// Author:      Robert Roebling
// Modified by:
// Created:     28.06.99
// RCS-ID:      $Id$
// Copyright:   (c) Robert Roebling
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "clipboardbase.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#include "wx/clipbrd.h"
#include "wx/module.h"

// ----------------------------------------------------------------------------
// wxClipboardModule: module responsible for initializing the global clipboard
// object
// ----------------------------------------------------------------------------

class wxClipboardModule : public wxModule
{
public:
    bool OnInit();
    void OnExit();

private:
    DECLARE_DYNAMIC_CLASS(wxClipboardModule)
};

// ----------------------------------------------------------------------------
// global data defined here
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxClipboardModule, wxModule)

wxClipboard* wxTheClipboard = (wxClipboard *)NULL;

// ----------------------------------------------------------------------------
// implementation
// ----------------------------------------------------------------------------

wxClipboardBase::wxClipboardBase()
{
}

bool wxClipboardModule::OnInit()
{
    wxTheClipboard = new wxClipboard;

    return TRUE;
}

void wxClipboardModule::OnExit()
{
    delete wxTheClipboard;

    wxTheClipboard = (wxClipboard *)NULL;
}
