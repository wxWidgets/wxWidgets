/////////////////////////////////////////////////////////////////////////////
// Name:        combobox.cpp
// Purpose:     wxComboBox class
// Author:      David Webster
// Modified by:
// Created:     10/13/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#include "wx/combobox.h"

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/setup.h"
    #include "wx/settings.h"
#endif

#if wxUSE_COMBOBOX

#include "wx/combobox.h"
#include "wx/clipbrd.h"
#include "wx/os2/private.h"

#define OWNER_DRAWN_LISTBOX_EXTRA_SPACE    (1)

MRESULT EXPENTRY wxComboEditWndProc( HWND   hWnd
                                    ,UINT   uMessage
                                    ,MPARAM wParam
                                    ,MPARAM lParam
                                   );
//
// The pointer to standard wnd proc
//
static WXFARPROC gfnWndprocEdit     = (WXFARPROC)NULL;

IMPLEMENT_DYNAMIC_CLASS(wxComboBox, wxControl)

bool wxComboBox::OS2Command(
  WXUINT                            uParam
, WXWORD                            WXUNUSED(wId)
)
{
    long                            lSel = -1L;
    wxString                        sValue;

    switch (uParam)
    {
        case CBN_LBSELECT:
            if (GetSelection() > -1)
            {
                wxCommandEvent      vEvent( wxEVT_COMMAND_COMBOBOX_SELECTED
                                           ,GetId()
                                          );

                vEvent.SetInt(GetSelection());
                vEvent.SetEventObject(this);
                vEvent.SetString((char*)GetStringSelection().c_str());
                ProcessCommand(vEvent);
            }
            break;

        case CBN_EFCHANGE:
            {
                wxCommandEvent      vEvent( wxEVT_COMMAND_TEXT_UPDATED
                                           ,GetId()
                                          );

                if (lSel == -1L)
                    sValue = GetValue();
                else
                    SetValue(sValue);
                vEvent.SetString((char*)GetValue().c_str());
                vEvent.SetEventObject(this);
                ProcessCommand(vEvent);
            }
            break;
    }
    //
    // There is no return value for the CBN_ notifications, so always return
    // FALSE from here to pass the message to DefWindowProc()
    //
    return FALSE;
} // end of wxComboBox::OS2Command

bool wxComboBox::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsValue
, const wxPoint&                    rPos
, const wxSize&                     rSize
, const wxArrayString&              asChoices
, long                              lStyle
, const wxValidator&                rValidator
, const wxString&                   rsName
)
{
    wxCArrayString chs(asChoices);

    return Create(pParent, vId, rsValue, rPos, rSize, chs.GetCount(),
                  chs.GetStrings(), lStyle, rValidator, rsName);
}

bool wxComboBox::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsValue
, const wxPoint&                    rPos
, const wxSize&                     rSize
, int                               n
, const wxString                    asChoices[]
, long                              lStyle
, const wxValidator&                rValidator
, const wxString&                   rsName
)
{
    m_isShown = FALSE;

    if (!CreateControl( pParent
                       ,vId
                       ,rPos
                       ,rSize
                       ,lStyle
                       ,rValidator
                       ,rsName
                      ))
        return FALSE;

    //
    // Get the right style
    //
    long                            lSstyle = 0L;

    lSstyle = WS_TABSTOP   |
              WS_VISIBLE;

    if (lStyle & wxCLIP_SIBLINGS )
        lSstyle |= WS_CLIPSIBLINGS;
    if (lStyle & wxCB_READONLY)
        lSstyle |= CBS_DROPDOWNLIST;
    else if (lStyle & wxCB_SIMPLE)
        lSstyle |= CBS_SIMPLE; // A list (shown always) and edit control
    else
        lSstyle |= CBS_DROPDOWN;


    if (!OS2CreateControl( "COMBOBOX"
                          ,lSstyle
                         ))
        return FALSE;

    //
    // A choice/combobox normally has a white background (or other, depending
    // on global settings) rather than inheriting the parent's background colour.
    //
    SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_WINDOW));

    SetFont(*wxSMALL_FONT);

    int                             i;
    for (i = 0; i < n; i++)
    {
        Append(asChoices[i]);
    }

    SetSize( rPos.x
            ,rPos.y
            ,rSize.x
            ,rSize.y
           );
    if (!rsValue.IsEmpty())
    {
        SetValue(rsValue);
    }
    gfnWndprocEdit = (WXFARPROC)::WinSubclassWindow( (HWND)GetHwnd()
                                                    ,(PFNWP)wxComboEditWndProc
                                                   );
    ::WinSetWindowULong(GetHwnd(), QWL_USER, (ULONG)this);
    Show(TRUE);
    return TRUE;
} // end of wxComboBox::Create

void wxComboBox::SetValue(
  const wxString&                   rsValue
)
{
    if ( HasFlag(wxCB_READONLY) )
        SetStringSelection(rsValue);
    else
        ::WinSetWindowText(GetHwnd(), rsValue.c_str());
} // end of wxComboBox::SetValue

//
// Clipboard operations
//
void wxComboBox::Copy()
{
    HWND                            hWnd = GetHwnd();

    ::WinSendMsg(hWnd, EM_COPY, (MPARAM)0, (MPARAM)0);
} // end of wxComboBox::Copy

void wxComboBox::Cut()
{
    HWND                            hWnd = GetHwnd();

    ::WinSendMsg(hWnd, EM_CUT, (MPARAM)0, (MPARAM)0);
} // end of wxComboBox::Cut

void wxComboBox::Paste()
{
    HWND                            hWnd = GetHwnd();

    ::WinSendMsg(hWnd, EM_PASTE, (MPARAM)0, (MPARAM)0);
} // end of wxComboBox::Paste

void wxComboBox::SetEditable(
  bool                              bEditable
)
{
    HWND                            hWnd = GetHwnd();

    ::WinSendMsg(hWnd, EM_SETREADONLY, (MPARAM)!bEditable, (MPARAM)0L);
} // end of wxComboBox::SetEditable

void wxComboBox::SetInsertionPoint(
  long                              lPos
)
{
    HWND                            hWnd = GetHwnd();

    ::WinSendMsg(hWnd, EM_SETFIRSTCHAR, MPFROMLONG(lPos), (MPARAM)0);
} // end of wxComboBox::SetInsertionPoint

void wxComboBox::SetInsertionPointEnd()
{
    long                            lPos = GetLastPosition();

    SetInsertionPoint(lPos);
} // end of wxComboBox::SetInsertionPointEnd

long wxComboBox::GetInsertionPoint() const
{
    long                            lPos = LONGFROMMR(::WinSendMsg( GetHwnd()
                                                                   ,LM_QUERYSELECTION
                                                                   ,(MPARAM)0
                                                                   ,(MPARAM)0
                                                                  ));
   if (lPos == LIT_NONE)
        return wxNOT_FOUND;
   return lPos;
} // end of wxComboBox::GetInsertionPoint

long wxComboBox::GetLastPosition() const
{
    long                            lLineLength = 0L;
    WNDPARAMS                       vParams;

    //
    // Get number of characters in the last (only) line. We'll add this to the character
    // index for the last line, 1st position.
    //


    vParams.fsStatus = WPM_CCHTEXT;
    if (::WinSendMsg( GetHwnd()
                     ,WM_QUERYWINDOWPARAMS
                     ,&vParams
                     ,0
                    ))
    {
        lLineLength = (long)vParams.cchText;
    }
    else
        lLineLength = 0L;
    return lLineLength;
} // end of wxComboBox::GetLastPosition

void wxComboBox::Replace(
  long                              lFrom
, long                              lTo
, const wxString&                   rsValue
)
{
#if wxUSE_CLIPBOARD
    HWND                            hWnd = GetHwnd();

    //
    // Set selection and remove it
    //
    ::WinSendMsg(hWnd, EM_SETSEL, MPFROM2SHORT((USHORT)lFrom, (USHORT)lTo), 0);
    ::WinSendMsg(hWnd, EM_CUT, (MPARAM)0, (MPARAM)0);

    //
    // Now replace with 'value', by pasting.
    //
    wxSetClipboardData( wxDF_TEXT
                       ,(wxObject *)rsValue.c_str()
                       ,0
                       ,0
                      );

    //
    // Paste into edit control
    //
    ::WinSendMsg(hWnd, EM_PASTE, (MPARAM)0, (MPARAM)0L);
#endif
} // end of wxComboBox::Replace

void wxComboBox::Remove(
  long                              lFrom
, long                              lTo
)
{
#if wxUSE_CLIPBOARD
    HWND                            hWnd = GetHwnd();

    ::WinSendMsg(hWnd, EM_SETSEL, MPFROM2SHORT((USHORT)lFrom, (USHORT)lTo), 0);
    ::WinSendMsg(hWnd, EM_CUT, (MPARAM)0, (MPARAM)0);
#endif
} // end of wxComboBox::Remove

void wxComboBox::SetSelection(
  long                              lFrom
, long                              lTo
)
{
    HWND                            hWnd = GetHwnd();
    long                            lFromChar = 0;
    long                            lToChar   = 0;

    //
    // If from and to are both -1, it means
    // (in wxWidgets) that all text should be selected.
    // This translates into Windows convention
    //
    if ((lFrom == -1L) && (lTo == -1L))
    {
        lFromChar = 0;
        lToChar = -1;
    }

    ::WinSendMsg( hWnd
                 ,EM_SETSEL
                 ,MPFROM2SHORT((USHORT)lFromChar, (USHORT)lToChar)
                 ,(MPARAM)0
                );
} // end of wxComboBox::SetSelection

void wxComboBox::DoSetSize(
  int                               nX
, int                               nY
, int                               nWidth
, int                               nHeight
, int                               nSizeFlags
)
{
    wxControl::DoSetSize( nX
                         ,nY
                         ,nWidth
                         ,nHeight
                         ,nSizeFlags
                        );
} // end of wxComboBox::DoSetSize

bool wxComboBox::ProcessEditMsg(
  WXUINT                            uMsg
, WXWPARAM                          wParam
, WXLPARAM                          lParam)
{
    SHORT                           vFlag;
    switch (uMsg)
    {
        case WM_CHAR:
            vFlag = SHORT1FROMMP(wParam);
            switch(vFlag)
            {
                case KC_CHAR:
                    return (HandleChar( wParam
                                       ,lParam
                                       ,TRUE /* isASCII */
                                      ));

                case KC_PREVDOWN:
                    return (HandleKeyDown( wParam
                                          ,lParam
                                         ));

                case KC_KEYUP:
                    return (HandleKeyUp( wParam
                                        ,lParam
                                       ));
            }
            break;

        case WM_SETFOCUS:
            if (SHORT1FROMMP((MPARAM)lParam) == TRUE)
                return(HandleSetFocus((WXHWND)(HWND)wParam));
            else
                return(HandleKillFocus((WXHWND)(HWND)wParam));
            break;
    }
    return FALSE;
} // end of WinGuiBase_CComboBox::ProcessEditMsg

MRESULT EXPENTRY wxComboEditWndProc(
  HWND                              hWnd
, UINT                              uMessage
, MPARAM                            wParam
, MPARAM                            lParam
)
{
    switch (uMessage)
    {
        //
        // Forward some messages to the combobox
        //
        case WM_SETFOCUS:
        case WM_CHAR:
            {
                wxComboBox* pCombo = (wxComboBox *)::WinQueryWindowULong( hWnd
                                                                         ,QWL_USER
                                                                        );

                if (pCombo->ProcessEditMsg( uMessage
                                           ,wParam
                                           ,lParam
                                          ))
                    return ((MRESULT)0);
            }
            break;

        //
        // TODO: Deal with tooltips here
        //
    }
    return (gfnWndprocEdit(hWnd, (ULONG)uMessage, (MPARAM)wParam, (MPARAM)lParam));
} // end of wxComboEditWndProc

#endif
 // wxUSE_COMBOBOX

