/////////////////////////////////////////////////////////////////////////////
// Name:        checkbox.cpp
// Purpose:     wxCheckBox
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/checkbox.h"
    #include "wx/brush.h"
    #include "wx/scrolwin.h"
    #include "wx/dcscreen.h"
    #include "wx/settings.h"
#endif

#include "wx/os2/private.h"

// ----------------------------------------------------------------------------
// macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxCheckBox, wxControl)
IMPLEMENT_DYNAMIC_CLASS(wxBitmapCheckBox, wxCheckBox)

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// wxCheckBox
// ----------------------------------------------------------------------------

bool wxCheckBox::OS2Command(
  WXUINT                            WXUNUSED(uParam)
, WXWORD                            WXUNUSED(wId)
)
{
    wxCommandEvent                  rEvent( wxEVT_COMMAND_CHECKBOX_CLICKED
                                           ,m_windowId
                                          );
    rEvent.SetInt(GetValue());
    rEvent.SetEventObject(this);
    ProcessCommand(rEvent);
    return TRUE;
} // end of wxCheckBox::OS2Command

bool wxCheckBox::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsLabel
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
#if wxUSE_VALIDATORS
, const wxValidator&                rValidator
#endif
, const wxString&                   rsName
)
{
    SetName(rsName);
#if wxUSE_VALIDATORS
    SetValidator(rValidator);
#endif
    if (pParent)
        pParent->AddChild(this);

    SetBackgroundColour(pParent->GetBackgroundColour());
    SetForegroundColour(pParent->GetForegroundColour());
    m_windowStyle = lStyle;

    wxString                        sLabel = rsLabel;

    if (sLabel == wxT(""))
        sLabel = wxT(" "); // Apparently needed or checkbox won't show

    if (vId == -1 )
        m_windowId = NewControlId();
    else
        m_windowId = vId;

    int                             nX      = rPos.x;
    int                             nY      = rPos.y;
    int                             nWidth  = rSize.x;
    int                             nHeight = rSize.y;
    long                            lSstyle = 0L;

    lSstyle = BS_AUTOCHECKBOX |
              WS_TABSTOP      |
              WS_VISIBLE;
    if (lStyle & wxCLIP_SIBLINGS )
        lSstyle |= WS_CLIPSIBLINGS;

    //
    // If the parent is a scrolled window the controls must
    // have this style or they will overlap the scrollbars
    //
    if (pParent)
        if (pParent->IsKindOf(CLASSINFO(wxScrolledWindow)) ||
            pParent->IsKindOf(CLASSINFO(wxGenericScrolledWindow)))
            lSstyle |= WS_CLIPSIBLINGS;

    m_hWnd = (WXHWND)::WinCreateWindow ( GetHwndOf(pParent)
                                        ,WC_BUTTON
                                        ,rsLabel.c_str()
                                        ,lSstyle
                                        ,0, 0, 0, 0
                                        ,GetWinHwnd(pParent)
                                        ,HWND_TOP
                                        ,(HMENU)m_windowId
                                        ,NULL
                                        ,NULL
                                       );

    //
    // Subclass again for purposes of dialog editing mode
    //
    SubclassWin(m_hWnd);

    LONG                            lColor = (LONG)m_backgroundColour.GetPixel();

    ::WinSetPresParam( m_hWnd
                      ,PP_BACKGROUNDCOLOR
                      ,sizeof(LONG)
                      ,(PVOID)&lColor
                     );

    SetFont(pParent->GetFont());

    SetSize( nX
            ,nY
            ,nWidth
            ,nHeight
           );
    return TRUE;
} // end of wxCheckBox::Create

void wxCheckBox::SetLabel(
  const wxString&                   rsLabel
)
{
    ::WinSetWindowText(GetHwnd(), rsLabel.c_str());
} // end of wxCheckBox::SetLabel

wxSize wxCheckBox::DoGetBestSize() const
{
    static int                      nCheckSize = 0;

    if (!nCheckSize)
    {
        wxScreenDC                  vDc;

        vDc.SetFont(wxSystemSettings::GetFont(wxSYS_DEFAULT_GUI_FONT));

        //
        // The height of a standard button in the dialog units is 8,
        // translate this to pixels (as one dialog unit is precisely equal to
        // 8 character heights, it's just the char height)
        //
        nCheckSize = vDc.GetCharHeight();
    }

    int                             nWidthCheckbox;
    int                             nHeightCheckbox;
    wxString                        sStr = wxGetWindowText(GetHWND());

    if (!sStr.IsEmpty())
    {
        GetTextExtent( sStr
                      ,&nWidthCheckbox
                      ,&nHeightCheckbox
                     );
        nWidthCheckbox += nCheckSize + GetCharWidth();

        if (nHeightCheckbox < nCheckSize)
            nHeightCheckbox = nCheckSize;
    }
    else
    {
        nWidthCheckbox  = nCheckSize;
        nHeightCheckbox = nCheckSize;
    }

    return wxSize( nWidthCheckbox
                  ,nHeightCheckbox
                 );
} // end of wxCheckBox::DoGetBestSize

void wxCheckBox::SetValue(
  bool                              bValue
)
{
    ::WinSendMsg(GetHwnd(), BM_SETCHECK, (MPARAM)bValue, 0);
} // end of wxCheckBox::SetValue

#ifndef BST_CHECKED
#define BST_CHECKED 0x0001
#endif

bool wxCheckBox::GetValue() const
{
    return((LONGFROMMR(::WinSendMsg(GetHwnd(), BM_QUERYCHECK, (MPARAM)0, (MPARAM)0)) == 1L));
} // end of wxCheckBox::GetValue

void wxCheckBox::Command (
  wxCommandEvent&                  rEvent
)
{
    SetValue((rEvent.GetInt() != 0));
    ProcessCommand(rEvent);
} // end of wxCheckBox:: Command

// ----------------------------------------------------------------------------
// wxBitmapCheckBox
// ----------------------------------------------------------------------------

bool wxBitmapCheckBox::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxBitmap*                   pLabel
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
#if wxUSE_VALIDATORS
, const wxValidator&                rValidator
#endif
, const wxString&                   rsName
)
{
    SetName(rsName);
#if wxUSE_VALIDATORS
    SetValidator(rValidator);
#endif
    if (pParent)
        pParent->AddChild(this);

    SetBackgroundColour(pParent->GetBackgroundColour()) ;
    SetForegroundColour(pParent->GetForegroundColour()) ;
    m_windowStyle = lStyle;

    if (vId == -1)
        m_windowId = NewControlId();
    else
        m_windowId = vId;

    int                             nX      = rPos.x;
    int                             nY      = rPos.y;
    int                             nWidth  = rSize.x;
    int                             nHeight = rSize.y;

    m_nCheckWidth = -1 ;
    m_nCheckHeight = -1 ;
//    long msStyle = CHECK_FLAGS;

    HWND hButton = 0; // TODO: Create the bitmap checkbox

    m_hWnd = (WXHWND)hButton;

    //
    // Subclass again for purposes of dialog editing mode
    //
    SubclassWin((WXHWND)hButton);

    SetSize( nX
            ,nY
            ,nWidth
            ,nHeight
           );

    ::WinShowWindow(hButton, TRUE);
    return TRUE;
} // end of wxBitmapCheckBox::Create

void wxBitmapCheckBox::SetLabel(
  const wxBitmap&                   rBitmap
)
{
    wxFAIL_MSG(wxT("not implemented"));
}  // end of wxBitmapCheckBox::SetLabel

