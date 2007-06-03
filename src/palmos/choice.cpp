/////////////////////////////////////////////////////////////////////////////
// Name:        src/palmos/choice.cpp
// Purpose:     wxChoice
// Author:      William Osborne - minimal working wxPalmOS port
// Created:     10/13/04
// RCS-ID:      $Id$
// Copyright:   (c) William Osborne
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

#if wxUSE_CHOICE

#include "wx/choice.h"

#ifndef WX_PRECOMP
    #include "wx/utils.h"
    #include "wx/log.h"
    #include "wx/brush.h"
    #include "wx/settings.h"
#endif

#include "wx/palmos/private.h"

IMPLEMENT_DYNAMIC_CLASS(wxChoice, wxControl)


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      int n, const wxString choices[],
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    return false;
}

bool wxChoice::CreateAndInit(wxWindow *parent,
                             wxWindowID id,
                             const wxPoint& pos,
                             const wxSize& size,
                             int n, const wxString choices[],
                             long style,
                             const wxValidator& validator,
                             const wxString& name)
{
    return false;
}

bool wxChoice::Create(wxWindow *parent,
                      wxWindowID id,
                      const wxPoint& pos,
                      const wxSize& size,
                      const wxArrayString& choices,
                      long style,
                      const wxValidator& validator,
                      const wxString& name)
{
    return false;
}

bool wxChoice::MSWShouldPreProcessMessage(WXMSG *pMsg)
{
    return false;
}

WXDWORD wxChoice::MSWGetStyle(long style, WXDWORD *exstyle) const
{
    return 0;
}

wxChoice::~wxChoice()
{
}

// ----------------------------------------------------------------------------
// adding/deleting items to/from the list
// ----------------------------------------------------------------------------

int wxChoice::DoAppend(const wxString& item)
{
    return 0;
}

int wxChoice::DoInsert(const wxString& item, unsigned int pos)
{
    return 0;
}

void wxChoice::Delete(unsigned int n)
{
}

void wxChoice::Clear()
{
}

void wxChoice::Free()
{
}

// ----------------------------------------------------------------------------
// selection
// ----------------------------------------------------------------------------

int wxChoice::GetSelection() const
{
    return 0;
}

void wxChoice::SetSelection(int n)
{
}

// ----------------------------------------------------------------------------
// string list functions
// ----------------------------------------------------------------------------

unsigned int wxChoice::GetCount() const
{
    return 0;
}

void wxChoice::SetString(unsigned int n, const wxString& s)
{
}

wxString wxChoice::GetString(unsigned int n) const
{
    return wxEmptyString;
}

// ----------------------------------------------------------------------------
// client data
// ----------------------------------------------------------------------------

void wxChoice::DoSetItemClientData(unsigned int n, void* clientData)
{
}

void* wxChoice::DoGetItemClientData(unsigned int n) const
{
    return (void *)NULL;
}

void wxChoice::DoSetItemClientObject(unsigned int n, wxClientData* clientData )
{
}

wxClientData* wxChoice::DoGetItemClientObject(unsigned int n) const
{
    return (wxClientData *)DoGetItemClientData(n);
}

// ----------------------------------------------------------------------------
// wxMSW specific helpers
// ----------------------------------------------------------------------------

void wxChoice::UpdateVisibleHeight()
{
}

void wxChoice::DoMoveWindow(int x, int y, int width, int height)
{
}

void wxChoice::DoGetSize(int *w, int *h) const
{
}

void wxChoice::DoSetSize(int x, int y,
                         int width, int height,
                         int sizeFlags)
{
}

wxSize wxChoice::DoGetBestSize() const
{
    return wxSize(0,0);
}

WXLRESULT wxChoice::MSWWindowProc(WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam)
{
    return 0;
}

bool wxChoice::MSWCommand(WXUINT param, WXWORD WXUNUSED(id))
{
    return false;
}

#endif // wxUSE_CHOICE
