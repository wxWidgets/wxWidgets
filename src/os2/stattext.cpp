/////////////////////////////////////////////////////////////////////////////
// Name:        stattext.cpp
// Purpose:     wxStaticText
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifdef __GNUG__
#pragma implementation "stattext.h"
#endif

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
#include "wx/event.h"
#include "wx/app.h"
#include "wx/brush.h"
#endif

#include "wx/stattext.h"
#include "wx/os2/private.h"
#include <stdio.h>

IMPLEMENT_DYNAMIC_CLASS(wxStaticText, wxControl)

bool wxStaticText::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsLabel
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
, const wxString&                   rsName
)
{
    SetName(rsName);
    if (pParent)
        pParent->AddChild(this);

    SetBackgroundColour(pParent->GetBackgroundColour()) ;
    SetForegroundColour(pParent->GetForegroundColour()) ;

    if ( id == -1 )
        m_windowId = (int)NewControlId();
    else
        m_windowId = vId;

    int                             nX      = rPos.x;
    int                             nY      = rPos.y;
    int                             nWidth  = rSize.x;
    int                             nHeight = rSize.y;

    m_windowStyle = lStyle;

    long                            lSstyle = 0L;

    lSstyle = WS_VISIBLE | SS_TEXT | DT_VCENTER;
    if (m_windowStyle & wxALIGN_CENTRE)
        lSstyle |= DT_CENTER;
    else if (m_windowStyle & wxALIGN_RIGHT)
        lSstyle |= DT_RIGHT;
    else
        lSstyle |= DT_LEFT;
    m_hWnd = (WXHWND)::WinCreateWindow( (HWND)GetHwndOf(pParent) // Parent window handle
                                       ,WC_STATIC                // Window class
                                       ,(PSZ)rsLabel.c_str()     // Initial Text
                                       ,(ULONG)lSstyle           // Style flags
                                       ,0L, 0L, 0L, 0L           // Origin -- 0 size
                                       ,(HWND)GetHwndOf(pParent) // owner window handle (same as parent
                                       ,HWND_TOP                 // initial z position
                                       ,(ULONG)m_windowId        // Window identifier
                                       ,NULL                     // no control data
                                       ,NULL                     // no Presentation parameters
                                      );

    wxCHECK_MSG(m_hWnd, FALSE, wxT("Failed to create static ctrl"));

    SubclassWin(m_hWnd);
    wxControl::SetFont(pParent->GetFont());
    SetSize(nX, nY, nWidth, nHeight);
    return FALSE;
} // end of wxStaticText::Create

wxSize wxStaticText::DoGetBestSize() const
{
    wxString                        sText(wxGetWindowText(GetHWND()));
    int                             nWidthTextMax = 0;
    int                             nWidthLine = 0;
    int                             nHeightTextTotal = 0;
    int                             nHeightLineDefault = 0;
    int                             nHeightLine = 0;
    wxString                        sCurLine;

    for ( const wxChar *pc = sText; ; pc++ )
    {
        if ( *pc == wxT('\n') || *pc == wxT('\0') )
        {
            if (!sCurLine )
            {
                //
                // We can't use GetTextExtent - it will return 0 for both width
                // and height and an empty line should count in height
                // calculation
                //
                if (!nHeightLineDefault)
                    nHeightLineDefault = nHeightLine;
                if (!nHeightLineDefault)
                    GetTextExtent(_T("W"), NULL, &nHeightLineDefault);
                nHeightTextTotal += nHeightLineDefault;
            }
            else
            {
                GetTextExtent( sCurLine
                              ,&nWidthLine
                              ,&nHeightLine
                             );
                if (nWidthLine > nWidthTextMax)
                    nWidthTextMax = nWidthLine;
                nHeightTextTotal += nHeightLine;
            }

            if ( *pc == wxT('\n') )
            {
                sCurLine.Empty();
            }
            else
            {
               break;
            }
        }
        else
        {
            sCurLine += *pc;
        }
    }
    return wxSize( nWidthTextMax
                  ,nHeightTextTotal
                 );
} // end of wxStaticText::DoGetBestSize

void wxStaticText::SetLabel(
  const wxString&                   rsLabel
)
{
    ::WinSetWindowText(GetHwnd(), rsLabel.c_str());

    //
    // Adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    //
    if (!(GetWindowStyle() & wxST_NO_AUTORESIZE))
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }
    DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
} // end of wxStaticText::SetLabel

bool wxStaticText::SetFont(
  const wxFont&                     rFont
)
{
    bool                            bRet = wxControl::SetFont(rFont);

    //
    // Adjust the size of the window to fit to the label unless autoresizing is
    // disabled
    //
    if ( !(GetWindowStyle() & wxST_NO_AUTORESIZE) )
    {
        DoSetSize(-1, -1, -1, -1, wxSIZE_AUTO_WIDTH | wxSIZE_AUTO_HEIGHT);
    }
    return bRet;
} // end of wxStaticText::SetFont

MRESULT wxStaticText::OS2WindowProc(
  WXUINT                            uMsg
, WXWPARAM                          wParam
, WXLPARAM                          lParam
)
{
    return wxWindow::OS2WindowProc( uMsg
                                   ,wParam
                                   ,lParam
                                  );
} // end of wxStaticText::OS2WindowProc


