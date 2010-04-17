/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/mdi.cpp
// Author:      Peter Most
// Id:          $Id$
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#include "wx/mdi.h"

wxMDIParentFrame::wxMDIParentFrame()
{
}

wxMDIParentFrame::wxMDIParentFrame(wxWindow *parent,
                 wxWindowID id,
                 const wxString& title,
                 const wxPoint& pos,
                 const wxSize& size,
                 long style,
                 const wxString& name)
{
}

bool wxMDIParentFrame::Create(wxWindow *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    return false;
}

void wxMDIParentFrame::ActivateNext()
{
}

void wxMDIParentFrame::ActivatePrevious()
{
}

//##############################################################################

wxMDIChildFrame::wxMDIChildFrame()
{
}

wxMDIChildFrame::wxMDIChildFrame(wxMDIParentFrame *parent,
                wxWindowID id,
                const wxString& title,
                const wxPoint& pos,
                const wxSize& size,
                long style,
                const wxString& name)
{
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *parent,
            wxWindowID id,
            const wxString& title,
            const wxPoint& pos,
            const wxSize& size,
            long style,
            const wxString& name)
{
    return false;
}

void wxMDIChildFrame::Activate()
{
}

//##############################################################################

wxMDIClientWindow::wxMDIClientWindow()
{
}

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *parent, long style)
{
    return false;
}

