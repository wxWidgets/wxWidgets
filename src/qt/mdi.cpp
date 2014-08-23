/////////////////////////////////////////////////////////////////////////////
// Name:        src/qt/mdi.cpp
// Author:      Peter Most
// Copyright:   (c) Peter Most
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#if wxUSE_MDI

#include "wx/mdi.h"

IMPLEMENT_DYNAMIC_CLASS(wxMDIParentFrame, wxFrame)

wxMDIParentFrame::wxMDIParentFrame()
{
}

wxMDIParentFrame::wxMDIParentFrame(wxWindow *WXUNUSED(parent),
                 wxWindowID WXUNUSED(id),
                 const wxString& WXUNUSED(title),
                 const wxPoint& WXUNUSED(pos),
                 const wxSize& WXUNUSED(size),
                 long WXUNUSED(style),
                 const wxString& WXUNUSED(name))
{
}

bool wxMDIParentFrame::Create(wxWindow *WXUNUSED(parent),
            wxWindowID WXUNUSED(id),
            const wxString& WXUNUSED(title),
            const wxPoint& WXUNUSED(pos),
            const wxSize& WXUNUSED(size),
            long WXUNUSED(style),
            const wxString& WXUNUSED(name))
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

wxMDIChildFrame::wxMDIChildFrame(wxMDIParentFrame *WXUNUSED(parent),
                wxWindowID WXUNUSED(id),
                const wxString& WXUNUSED(title),
                const wxPoint& WXUNUSED(pos),
                const wxSize& WXUNUSED(size),
                long WXUNUSED(style),
                const wxString& WXUNUSED(name))
{
}

bool wxMDIChildFrame::Create(wxMDIParentFrame *WXUNUSED(parent),
            wxWindowID WXUNUSED(id),
            const wxString& WXUNUSED(title),
            const wxPoint& WXUNUSED(pos),
            const wxSize& WXUNUSED(size),
            long WXUNUSED(style),
            const wxString& WXUNUSED(name))
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

bool wxMDIClientWindow::CreateClient(wxMDIParentFrame *WXUNUSED(parent), long WXUNUSED(style))
{
    return false;
}

#endif // wxUSE_MDI
