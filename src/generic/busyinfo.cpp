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

#ifdef __BORLANDC__
#pragma hdrstop
#endif

#if wxUSE_BUSYINFO

#include "wx/stattext.h"
#include "wx/panel.h"
#include "wx/utils.h"
#include "wx/busyinfo.h"


wxInfoFrame::wxInfoFrame(wxWindow *parent, const wxString& message)
           : wxFrame(parent, -1, wxT("Busy"),
                     wxDefaultPosition, wxDefaultSize,
#if defined(__WXX11__)
                     wxTHICK_FRAME
#else
                     wxSIMPLE_BORDER
#endif
                     | wxFRAME_TOOL_WINDOW)
{
    wxPanel *panel = new wxPanel( this );
    wxStaticText *text = new wxStaticText(panel, -1, message);

    panel->SetCursor(*wxHOURGLASS_CURSOR);
    text->SetCursor(*wxHOURGLASS_CURSOR);

    // make the frame of at least the standard size (400*80) but big enough
    // for the text we show
    wxSize sizeText = text->GetBestSize();
#ifdef __WXPM__
    int                             nX = 0;
    int                             nY = 0;
    int                             nWidth = 0;
    int                             nHeight = 0;
    int                             nParentHeight = parent->GetClientSize().y;
    int                             nParentWidth = parent->GetClientSize().x;
    int                             nColor;

    SetBackgroundColour("WHITE");
    nColor = (LONG)GetBackgroundColour().GetPixel();

    ::WinSetPresParam( GetHwnd()
                      ,PP_BACKGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&nColor
                     );
    panel->SetBackgroundColour("WHITE");
    nColor = (LONG)panel->GetBackgroundColour().GetPixel();

    ::WinSetPresParam( GetHwndOf(panel)
                      ,PP_BACKGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&nColor
                     );
    nWidth = wxMax(sizeText.x, 340) + 60;
    nHeight = wxMax(sizeText.y, 40) + 40;
    nX = (nParentWidth - nWidth) / 2;
    nY = (nParentHeight / 2) - (nHeight / 2);
    nY = nParentHeight - (nY + nHeight);
    ::WinSetWindowPos( m_hFrame
                      ,HWND_TOP
                      ,nX
                      ,nY
                      ,nWidth
                      ,nHeight
                      ,SWP_SIZE | SWP_MOVE | SWP_ACTIVATE
                     );
    text->SetBackgroundColour("WHITE");
    nColor = (LONG)text->GetBackgroundColour().GetPixel();

    ::WinSetPresParam( GetHwndOf(text)
                      ,PP_BACKGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&nColor
                     );
    text->Center(wxBOTH);
#else
    SetClientSize(wxMax(sizeText.x, 340) + 60, wxMax(sizeText.y, 40) + 40);

    // need to size the panel correctly first so that text->Centre() works
    panel->SetSize(GetClientSize());

    text->Centre(wxBOTH);
    Centre(wxBOTH);
#endif
}

wxBusyInfo::wxBusyInfo(const wxString& message, wxWindow *parent)
{
    m_InfoFrame = new wxInfoFrame( parent, message);
    m_InfoFrame->Show(TRUE);
#ifdef __WXMAC__
    m_InfoFrame->Update();
#else
    wxYield();
    m_InfoFrame->Refresh();
    wxYield();
#endif
}

wxBusyInfo::~wxBusyInfo()
{
    m_InfoFrame->Show(FALSE);
    m_InfoFrame->Close();
    wxYield();
}

#endif
  // wxUSE_BUSYINFO

