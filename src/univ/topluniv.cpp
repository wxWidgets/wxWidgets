/////////////////////////////////////////////////////////////////////////////
// Name:        topluniv.cpp
// Author:      Vaclav Slavik
// Id:          $Id$
// Copyright:   (c) 2001 SciTech Software, Inc. (www.scitechsoft.com)
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#ifdef __GNUG__
    #pragma implementation "univtoplevel.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#include "wx/defs.h"
#include "wx/toplevel.h"


// ----------------------------------------------------------------------------
// event tables
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTopLevelWindow, wxWindow)


// ============================================================================
// implementation
// ============================================================================

void wxTopLevelWindow::Init()
{
}

bool wxTopLevelWindow::Create(wxWindow *parent,
                              wxWindowID id,
                              const wxString& title,
                              const wxPoint& pos,
                              const wxSize& sizeOrig,
                              long style,
                              const wxString &name)
{
    if ( !wxTopLevelWindowNative::Create(parent, id, title, pos, 
                                         sizeOrig, style, name) )
        return FALSE;
        
    // FIXME_MGL -- this is temporary; we assume for now that native TLW
    //              can do decorations, which is not true for MGL

    return TRUE;
}

bool wxTopLevelWindow::ShowFullScreen(bool show, long style)
{
    if ( show == IsFullScreen() ) return FALSE;
    
    return wxTopLevelWindowNative::ShowFullScreen(show, style);
    
    // FIXME_MGL -- must handle caption hiding here if not in
    //              native decorations mode
}

