/////////////////////////////////////////////////////////////////////////////
// Name:        button.cpp
// Purpose:     wxButton
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
    #include "wx/button.h"
    #include "wx/brush.h"
    #include "wx/panel.h"
    #include "wx/bmpbuttn.h"
    #include "wx/settings.h"
    #include "wx/dcscreen.h"
#endif

#include "wx/os2/private.h"

#define BUTTON_HEIGHT_FROM_CHAR_HEIGHT(cy) (11*EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy)/10)

//
// Should be at the very least less than winDEFAULT_BUTTON_MARGIN
//
#define FOCUS_MARGIN 3

#ifndef BST_CHECKED
#define BST_CHECKED 0x0001
#endif

IMPLEMENT_DYNAMIC_CLASS(wxButton, wxControl)

// Button

bool wxButton::Create(
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
    m_windowStyle = lStyle;
    pParent->AddChild((wxButton *)this);
    if (vId == -1)
        m_windowId = NewControlId();
    else
        m_windowId = vId;
    lStyle = WS_VISIBLE | WS_TABSTOP | BS_PUSHBUTTON;

    //
    // OS/2 PM does not have Right/Left/Top/Bottom styles.
    // We will have to define an additional style when we implement notebooks
    // for a notebook page button
    //
    if (m_windowStyle & wxCLIP_SIBLINGS )
        lStyle |= WS_CLIPSIBLINGS;
    //
    // If the parent is a scrolled window the controls must
    // have this style or they will overlap the scrollbars
    //
    if (pParent)
        if (pParent->IsKindOf(CLASSINFO(wxScrolledWindow)) ||
            pParent->IsKindOf(CLASSINFO(wxGenericScrolledWindow)))
            lStyle |= WS_CLIPSIBLINGS;

    m_hWnd = (WXHWND)::WinCreateWindow( GetHwndOf(pParent)   // Parent handle
                                       ,WC_BUTTON            // A Button class window
                                       ,(PSZ)rsLabel.c_str() // Button text
                                       ,lStyle               // Button style
                                       ,0, 0, 0, 0           // Location and size
                                       ,GetHwndOf(pParent)   // Owner handle
                                       ,HWND_TOP             // Top of Z-Order
                                       ,vId                  // Identifier
                                       ,NULL                 // No control data
                                       ,NULL                 // No Presentation parameters
                                      );
    if (m_hWnd == 0)
    {
        return FALSE;
    }

    //
    // Subclass again for purposes of dialog editing mode
    //
    SubclassWin(m_hWnd);
    SetFont(pParent->GetFont());
    SetSize( rPos.x
            ,rPos.y
            ,rSize.x
            ,rSize.y
           );
    return TRUE;
} // end of wxButton::Create

wxButton::~wxButton()
{
    wxPanel*                        pPanel = wxDynamicCast(GetParent(), wxPanel);

    if (pPanel)
    {
        if (pPanel->GetDefaultItem() == this)
        {
            //
            // Don't leave the panel with invalid default item
            //
            pPanel->SetDefaultItem(NULL);
        }
    }
} // end of wxButton::~wxButton

// ----------------------------------------------------------------------------
// size management including autosizing
// ----------------------------------------------------------------------------

wxSize wxButton::DoGetBestSize() const
{
    wxString                        rsLabel = wxGetWindowText(GetHWND());
    int                             nWidthButton;
    int                             nWidthChar;
    int                             nHeightChar;

    GetTextExtent( rsLabel
                  ,&nWidthButton
                  ,NULL
                 );

    wxGetCharSize( GetHWND()
                  ,&nWidthChar
                  ,&nHeightChar
                  ,(wxFont*)&GetFont()
                 );

    //
    // Add a margin - the button is wider than just its label
    //
    nWidthButton += 3 * nWidthChar;

    //
    // The button height is proportional to the height of the font used
    //
    int                             nHeightButton = BUTTON_HEIGHT_FROM_CHAR_HEIGHT(nHeightChar);

    //
    // Need a little extra to make it look right
    //
    nHeightButton += nHeightChar/1.5;

    wxSize                          vSize = GetDefaultSize();

    if (nWidthButton > vSize.x)
        vSize.x = nWidthButton;
    if (nHeightButton > vSize.y)
        vSize.y = nHeightButton;
    return vSize;
} // end of wxButton::DoGetBestSize

/* static */
wxSize wxButton::GetDefaultSize()
{
    static wxSize                   vSizeBtn;

    if (vSizeBtn.x == 0)
    {
        wxScreenDC                  vDc;

        vDc.SetFont(wxSystemSettings::GetSystemFont(wxSYS_DEFAULT_GUI_FONT));

        //
        // The size of a standard button in the dialog units is 50x14,
        // translate this to pixels
        // NB1: the multipliers come from the Windows convention
        // NB2: the extra +1/+2 were needed to get the size be the same as the
        //      size of the buttons in the standard dialog - I don't know how
        //      this happens, but on my system this size is 75x23 in pixels and
        //      23*8 isn't even divisible by 14... Would be nice to understand
        //      why these constants are needed though!
        vSizeBtn.x = (50 * (vDc.GetCharWidth() + 1))/4;
        vSizeBtn.y = ((14 * vDc.GetCharHeight()) + 2)/8;
    }
    return vSizeBtn;
} // end of wxButton::GetDefaultSize

void wxButton::Command (
  wxCommandEvent&                   rEvent
)
{
    ProcessCommand (rEvent);
} // end of wxButton::Command

// ----------------------------------------------------------------------------
// helpers
// ----------------------------------------------------------------------------

bool wxButton::SendClickEvent()
{
    wxCommandEvent                  vEvent( wxEVT_COMMAND_BUTTON_CLICKED
                                           ,GetId()
                                          );

    vEvent.SetEventObject(this);
    return ProcessCommand(vEvent);
} // end of wxButton::SendClickEvent

void wxButton::SetDefault()
{
    wxWindow*                       pParent = GetParent();
    wxButton*                       pBtnOldDefault = NULL;
    wxPanel*                        pPanel = wxDynamicCast(pParent, wxPanel);
    long                            lStyle = 0L;

    if (pParent)
    {
        wxWindow*                   pWinOldDefault = pParent->SetDefaultItem(this);

        pBtnOldDefault = wxDynamicCast(pWinOldDefault, wxButton);
    }
    if (pBtnOldDefault && pBtnOldDefault != this)
    {
        //
        // Remove the BS_DEFPUSHBUTTON style from the other button
        //
        lStyle = ::WinQueryWindowULong(GetHwndOf(pBtnOldDefault), QWL_STYLE);

        //
        // Don't do it with the owner drawn buttons because it will reset
        // BS_OWNERDRAW style bit too (BS_OWNERDRAW & BS_DEFPUSHBUTTON != 0)!
        //
        if ((lStyle & BS_USERBUTTON) != BS_USERBUTTON)
        {
            lStyle &= ~BS_DEFAULT;
            ::WinSetWindowULong(GetHwndOf(pBtnOldDefault), QWL_STYLE, lStyle);
        }
        else
        {
            //
            // Redraw the button - it will notice itself that it's not the
            // default one any longer
            //
            pBtnOldDefault->Refresh();
        }
    }

    //
    // Set this button as the default
    //
    lStyle = ::WinQueryWindowULong(GetHwnd(), QWL_STYLE);
    if ((lStyle & BS_USERBUTTON) != BS_USERBUTTON)
    {
        lStyle != BS_DEFAULT;
        ::WinSetWindowULong(GetHwnd(), QWL_STYLE, lStyle);
    }
} // end of wxButton::SetDefault

// ----------------------------------------------------------------------------
// event/message handlers
// ----------------------------------------------------------------------------

bool wxButton::OS2Command(
  WXUINT                            uParam
, WXWORD                            wId
)
{
    bool                            bProcessed = FALSE;

    switch (uParam)
    {
        case BN_CLICKED:            // normal buttons send this
        case BN_DBLCLICKED:         // owner-drawn ones also send this
            bProcessed = SendClickEvent();
            break;
    }
    return bProcessed;
} // end of wxButton::OS2Command

WXHBRUSH wxButton::OnCtlColor(
  WXHDC                             pDC
, WXHWND                            pWnd
, WXUINT                            nCtlColor
, WXUINT                            uMessage
, WXWPARAM                          wParam
, WXLPARAM                          lParam
)
{
    wxBrush*                        pBackgroundBrush = wxTheBrushList->FindOrCreateBrush( GetBackgroundColour()
                                                                                         ,wxSOLID
                                                                                        );

    return (WXHBRUSH)pBackgroundBrush->GetResourceHandle();
} // end of wxButton::OnCtlColor

void wxButton::MakeOwnerDrawn()
{
    long                            lStyle = 0L;

    lStyle = ::WinQueryWindowULong(GetHwnd(), QWL_STYLE);
    if ((lStyle & BS_USERBUTTON) != BS_USERBUTTON)
    {
        //
        // Make it so
        //
        lStyle |= BS_USERBUTTON;
        ::WinSetWindowULong(GetHwnd(), QWL_STYLE, lStyle);
    }
} // end of wxCButton::MakeOwnerDrawn

MRESULT wxButton::WindowProc(
  WXUINT                            uMsg
, WXWPARAM                          wParam
, WXLPARAM                          lParam
)
{
    //
    // When we receive focus, we want to become the default button in our
    // parent panel
    //
    if (uMsg == WM_SETFOCUS)
    {
        SetDefault();

        //
        // Let the default processign take place too
        //
    }

    else if (uMsg == WM_BUTTON1DBLCLK)
    {
        //
        // Emulate a click event to force an owner-drawn button to change its
        // appearance - without this, it won't do it
        //
        (void)wxControl::OS2WindowProc( WM_BUTTON1DOWN
                                       ,wParam
                                       ,lParam
                                      );

        //
        // And conitnue with processing the message normally as well
        //
    }

    //
    // Let the base class do all real processing
    //
    return (wxControl::OS2WindowProc( uMsg
                                     ,wParam
                                     ,lParam
                                    ));
} // end of wxW indowProc

