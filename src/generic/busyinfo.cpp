/////////////////////////////////////////////////////////////////////////////
// Name:        busyinfo.cpp
// Purpose:     Information window when app is busy
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows Licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "busyinfo.h"
#endif

#include "wx/wxprec.h"

#ifdef __BORDLANDC__
#pragma hdrstop
#endif

#if wxUSE_BUSYINFO

#include "wx/busyinfo.h"
#include "wx/stattext.h"
#include "wx/panel.h"
#include "wx/utils.h"


wxInfoFrame::wxInfoFrame(wxWindow *parent, const wxString& message)
           : wxFrame(parent, -1, wxT("Busy"),
                     wxDefaultPosition, wxDefaultSize,
#if defined(__WXX11__)
                     wxTHICK_FRAME | wxFRAME_TOOL_WINDOW)
#else
                     wxSIMPLE_BORDER | wxFRAME_TOOL_WINDOW)
#endif
{
    wxPanel *panel = new wxPanel( this );
    wxStaticText *text = new wxStaticText(panel, -1, message);

    panel->SetCursor(*wxHOURGLASS_CURSOR);
    text->SetCursor(*wxHOURGLASS_CURSOR);

    // make the frame of at least the standard size (400*80) but big enough
    // for the text we show
    wxSize sizeText = text->GetBestSize();
    SetClientSize(wxMax(sizeText.x, 340) + 60, wxMax(sizeText.y, 40) + 40);

    // need to size the panel correctly first so that text->Centre() works
    panel->SetSize(GetClientSize());

    text->Centre(wxBOTH);
    Centre(wxBOTH);
}

wxBusyInfo::wxBusyInfo(const wxString& message, wxWindow *parent)
{
    m_InfoFrame = new wxInfoFrame( parent, message);
    m_InfoFrame->Show(TRUE);
    wxYield();
    m_InfoFrame->Refresh();
    wxYield();
}

wxBusyInfo::~wxBusyInfo()
{
    m_InfoFrame->Show(FALSE);
    m_InfoFrame->Close();
    wxYield();
}

#endif
  // wxUSE_BUSYINFO

