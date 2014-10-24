/////////////////////////////////////////////////////////////////////////////
// Name:        src/generic/busyinfo.cpp
// Purpose:     Information window when app is busy
// Author:      Vaclav Slavik
// Copyright:   (c) 1999 Vaclav Slavik
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/wxprec.h"

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_BUSYINFO

// for all others, include the necessary headers
#ifndef WX_PRECOMP
    #include "wx/frame.h"
    #include "wx/stattext.h"
    #include "wx/panel.h"
    #include "wx/utils.h"
#endif

#include "wx/busyinfo.h"
#include "wx/generic/stattextg.h"

class WXDLLEXPORT wxInfoFrame : public wxFrame
{
public:
    wxInfoFrame(wxWindow *parent, const wxString& message);

private:
    wxDECLARE_NO_COPY_CLASS(wxInfoFrame);
};


wxInfoFrame::wxInfoFrame(wxWindow *parent, const wxString& message)
           : wxFrame(parent, wxID_ANY, wxString(),
                     wxDefaultPosition, wxDefaultSize,
                     wxSIMPLE_BORDER | wxFRAME_TOOL_WINDOW | wxSTAY_ON_TOP)
{
    wxPanel *panel = new wxPanel( this );
#ifdef __WXGTK__
    wxGenericStaticText *text = new wxGenericStaticText(panel, wxID_ANY, message);
#else
    wxStaticText *text = new wxStaticText(panel, wxID_ANY, message);
#endif

    panel->SetCursor(*wxHOURGLASS_CURSOR);
    text->SetCursor(*wxHOURGLASS_CURSOR);

    // make the frame of at least the standard size (400*80) but big enough
    // for the text we show
    wxSize size = text->GetBestSize();
    size.IncBy(ConvertDialogToPixels(wxPoint(10, 10)));
    size.IncTo(wxSize(400, 80));
    SetClientSize(size);

    // need to size the panel correctly first so that text->Centre() works
    panel->SetSize(GetClientSize());

    text->Centre(wxBOTH);
    Centre(wxBOTH);
}

wxBusyInfo::wxBusyInfo(const wxString& message, wxWindow *parent)
{
    m_InfoFrame = new wxInfoFrame(parent, message);
    m_InfoFrame->Show(true);
    m_InfoFrame->Refresh();
    m_InfoFrame->Update();
}

wxBusyInfo::~wxBusyInfo()
{
    m_InfoFrame->Show(false);
    m_InfoFrame->Close();
}

#endif // wxUSE_BUSYINFO
