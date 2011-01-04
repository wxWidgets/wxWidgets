/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/button.cpp
// Purpose:     wxButton
// Author:      William Osborne - minimal working wxPalmOS port
// Modified by: Wlodzimierz ABX Skiba - native wxButton implementation
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne, Wlodzimierz Skiba
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_BUTTON

#include "wx/button.h"

#ifndef WX_PRECOMP
    #include "wx/app.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
    #include "wx/frame.h"
    #include "wx/dialog.h"
#endif

#include "wx/stockitem.h"

#include <Control.h>
#include <Form.h>

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

// this macro tries to adjust the default button height to a reasonable value
// using the char height as the base
#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation/destruction
// ----------------------------------------------------------------------------

bool wxButton::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxString& label,
                      const wxPoint& pos,
                      const wxSize& size,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    // Default coordinates based on the knowledgebase recipe "Buttons"
    wxSize palmSize(size.x==wxDefaultCoord?36:size.x,
                    size.y==wxDefaultCoord?12:size.y);

    // Default placement depends on dialog vs. frame type of parent
    wxPoint palmPos(pos);
    if((palmPos.x==wxDefaultCoord)||(palmPos.y==wxDefaultCoord))
    {
        wxSize parentSize(parent->GetClientSize());
        wxWindow* parentTLW = parent;
        while ( parentTLW && !parentTLW->IsTopLevel() )
        {
            parentTLW = parentTLW->GetParent();
        }

        if(wxDynamicCast(parentTLW, wxFrame)!=NULL)
        {
            if(palmPos.x==wxDefaultCoord)
                palmPos.x = 0;
            if(palmPos.y==wxDefaultCoord)
                palmPos.y = parentSize.y-palmSize.y;
        }
        else if(wxDynamicCast(parentTLW, wxDialog)!=NULL)
        {
            if(palmPos.x==wxDefaultCoord)
                palmPos.x = 4;
            if(palmPos.y==wxDefaultCoord)
                palmPos.y = parentSize.y-palmSize.y-5;
        }
        else
        {
            // something seriously broken
            return false;
        }
    }

    // take the stock label
    wxString palmLabel = label;
    if( palmLabel.empty() && wxIsStockID(id) )
        palmLabel = wxGetStockLabel(id, wxSTOCK_NOFLAGS);

    if(!wxControl::Create(parent, id, palmPos, palmSize, style, validator, name))
        return false;

    return wxControl::PalmCreateControl(buttonCtl, palmLabel, palmPos, palmSize);
}

wxButton::~wxButton()
{
}

// ----------------------------------------------------------------------------
// size management including autosizing
// ----------------------------------------------------------------------------

wxSize wxButton::DoGetBestSize() const
{
    return wxSize(36,12);
}

/* static */
wxSize wxButtonBase::GetDefaultSize()
{
    return wxSize(36,12);
}

wxWindow *wxButton::SetDefault()
{
    FormType* form = (FormType* )GetParentForm();
    if(form==NULL)
        return NULL;
#ifdef __WXPALMOS6__
    FrmSetDefaultButtonID(form,GetId());
#endif // __WXPALMOS6__

    return wxButtonBase::SetDefault();
}

void wxButton::SetTmpDefault()
{
}

void wxButton::UnsetTmpDefault()
{
}

/* static */
void
wxButton::SetDefaultStyle(wxButton *btn, bool on)
{
}

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

bool wxButton::SendClickEvent()
{
    wxCommandEvent event(wxEVT_COMMAND_BUTTON_CLICKED, GetId());
    event.SetEventObject(this);
    return ProcessCommand(event);
}

void wxButton::Command(wxCommandEvent &event)
{
    ProcessCommand(event);
}

#endif // wxUSE_BUTTON
