/////////////////////////////////////////////////////////////////////////////
// Name:        textctrl.cpp
// Purpose:     wxTextCtrl
// Author:      David Webster
// Modified by:
// Created:     10/17/99
// RCS-ID:      $Id$
// Copyright:   (c) David Webster
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifndef WX_PRECOMP
    #include "wx/textctrl.h"
    #include "wx/settings.h"
    #include "wx/brush.h"
    #include "wx/utils.h"
    #include "wx/log.h"
#endif

#if wxUSE_CLIPBOARD
    #include "wx/app.h"
    #include "wx/clipbrd.h"
#endif

#include "wx/textfile.h"

#include "wx/os2/private.h"

#include <string.h>
#include <stdlib.h>
#include <sys/types.h>

#if wxUSE_IOSTREAMH
#   include <fstream.h>
#else
#   include <fstream>
#endif


// ----------------------------------------------------------------------------
// event tables and other macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS(wxTextCtrl, wxControl)

BEGIN_EVENT_TABLE(wxTextCtrl, wxControl)
    EVT_CHAR(wxTextCtrl::OnChar)
    EVT_DROP_FILES(wxTextCtrl::OnDropFiles)

    EVT_MENU(wxID_CUT, wxTextCtrl::OnCut)
    EVT_MENU(wxID_COPY, wxTextCtrl::OnCopy)
    EVT_MENU(wxID_PASTE, wxTextCtrl::OnPaste)
    EVT_MENU(wxID_UNDO, wxTextCtrl::OnUndo)
    EVT_MENU(wxID_REDO, wxTextCtrl::OnRedo)

    EVT_UPDATE_UI(wxID_CUT, wxTextCtrl::OnUpdateCut)
    EVT_UPDATE_UI(wxID_COPY, wxTextCtrl::OnUpdateCopy)
    EVT_UPDATE_UI(wxID_PASTE, wxTextCtrl::OnUpdatePaste)
    EVT_UPDATE_UI(wxID_UNDO, wxTextCtrl::OnUpdateUndo)
    EVT_UPDATE_UI(wxID_REDO, wxTextCtrl::OnUpdateRedo)
END_EVENT_TABLE()


// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// creation
// ----------------------------------------------------------------------------

wxTextCtrl::wxTextCtrl()
{
}

bool wxTextCtrl::Create(
  wxWindow*                         pParent
, wxWindowID                        vId
, const wxString&                   rsValue
, const wxPoint&                    rPos
, const wxSize&                     rSize
, long                              lStyle
#if wxUSE_VALIDATORS
, const wxValidator&                rValidator
#endif
, const wxString&                   rsName
)
{
    //
    // Base initialization
    //
    if ( !CreateBase( pParent
                     ,vId
                     ,rPos
                     ,rSize
                     ,lStyle
#if wxUSE_VALIDATORS
                     ,rValidator
#endif
                     ,rsName
                    ))
        return FALSE;

    if (pParent )
        pParent->AddChild(this);

    m_windowStyle = lStyle;

    long                            lSstyle = WS_VISIBLE | WS_TABSTOP;

    //
    // Single and multiline edit fields are two different controls in PM
    //
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        m_bIsMLE = TRUE;
        m_windowStyle |= wxTE_PROCESS_ENTER;

        if ((m_windowStyle & wxTE_NO_VSCROLL) == 0)
            lSstyle |= MLS_VSCROLL;
        if (m_windowStyle & wxHSCROLL)
            lSstyle |= MLS_HSCROLL;
        if (m_windowStyle & wxTE_READONLY)
            lSstyle |= MLS_READONLY;
    }
    else
    {
        lSstyle |= ES_LEFT;

        if (m_windowStyle & wxHSCROLL)
            lSstyle |=  ES_AUTOSCROLL;
        if (m_windowStyle & wxTE_READONLY)
            lSstyle |= ES_READONLY;
        if (m_windowStyle & wxTE_PASSWORD) // hidden input
            lSstyle |= ES_UNREADABLE;
    }
    if (m_bIsMLE)
    {
        m_hWnd = (WXHWND)::WinCreateWindow( (HWND)GetHwndOf(pParent) // Parent window handle
                                           ,WC_MLE                   // Window class
                                           ,(PSZ)rsValue.c_str()     // Initial Text
                                           ,(ULONG)lSstyle           // Style flags
                                           ,(LONG)rPos.x             // X pos of origin
                                           ,(LONG)rPos.y             // Y pos of origin
                                           ,(LONG)rSize.x            // field width
                                           ,(LONG)rSize.y            // field height
                                           ,(HWND)GetHwndOf(pParent) // owner window handle (same as parent
                                           ,HWND_TOP                 // initial z position
                                           ,(ULONG)vId               // Window identifier
                                           ,NULL                     // no control data
                                           ,NULL                     // no Presentation parameters
                                          );
    }
    else
    {
        m_hWnd = (WXHWND)::WinCreateWindow( (HWND)GetHwndOf(pParent) // Parent window handle
                                           ,WC_ENTRYFIELD            // Window class
                                           ,(PSZ)rsValue.c_str()     // Initial Text
                                           ,(ULONG)lSstyle           // Style flags
                                           ,(LONG)rPos.x             // X pos of origin
                                           ,(LONG)rPos.y             // Y pos of origin
                                           ,(LONG)rSize.x            // field width
                                           ,(LONG)rSize.y            // field height
                                           ,(HWND)GetHwndOf(pParent) // owner window handle (same as parent
                                           ,HWND_TOP                 // initial z position
                                           ,(ULONG)vId               // Window identifier
                                           ,NULL                     // no control data
                                           ,NULL                     // no Presentation parameters
                                          );
    }

    if (m_hWnd == 0)
    {
        return FALSE;
    }

    SubclassWin(GetHWND());

    //
    // Set font, position, size and initial value
    //
    wxFont&                         vFontParent = pParent->GetFont();

    if (vFontParent.Ok())
    {
        SetFont(vFontParent);
    }
    else
    {
        SetFont(wxSystemSettings::GetSystemFont(wxSYS_SYSTEM_FONT));
    }
    if (!rsValue.IsEmpty())
    {
        SetValue(rsValue);
    }
    SetupColours();
    SetSize( rPos.x
            ,rPos.y
            ,rSize.x
            ,rSize.y
           );
    return TRUE;
} // end of wxTextCtrl::Create

//
// Make sure the window style (etc.) reflects the HWND style (roughly)
//
void wxTextCtrl::AdoptAttributesFromHWND()
{
    HWND                            hWnd = GetHwnd();
    LONG                            lStyle = ::WinQueryWindowULong(hWnd, QWL_STYLE);

    wxWindow::AdoptAttributesFromHWND();

    if (m_bIsMLE)
    {
        m_windowStyle |= wxTE_MULTILINE;
        if (lStyle & MLS_READONLY)
            m_windowStyle |= wxTE_READONLY;
    }
    else
    {
        if (lStyle & ES_UNREADABLE)
            m_windowStyle |= wxTE_PASSWORD;
        if (lStyle & ES_READONLY)
            m_windowStyle |= wxTE_READONLY;
    }
}

void wxTextCtrl::SetupColours()
{
    // FIXME why is bg colour not inherited from parent?
    SetBackgroundColour(wxSystemSettings::GetSystemColour(wxSYS_COLOUR_WINDOW));
    SetForegroundColour(GetParent()->GetForegroundColour());
}

// ----------------------------------------------------------------------------
// set/get the controls text
// ----------------------------------------------------------------------------

wxString wxTextCtrl::GetValue() const
{
    return wxGetWindowText(GetHWND());
}

void wxTextCtrl::SetValue(const wxString& value)
{
// TODO:
/*
    wxString valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);

    SetWindowText(GetHwnd(), valueDos);

    AdjustSpaceLimit();
*/
}

void wxTextCtrl::WriteText(const wxString& value)
{
// TODO:
/*
    wxString valueDos = wxTextFile::Translate(value, wxTextFileType_Dos);

    SendMessage(GetHwnd(), EM_REPLACESEL, 0, (LPARAM)valueDos.c_str());

    AdjustSpaceLimit();
*/
}

void wxTextCtrl::AppendText(const wxString& text)
{
// TODO:
/*
    SetInsertionPointEnd();
    WriteText(text);
*/
}

void wxTextCtrl::Clear()
{
//    SetWindowText(GetHwnd(), wxT(""));
}

// ----------------------------------------------------------------------------
// Clipboard operations
// ----------------------------------------------------------------------------

void wxTextCtrl::Copy()
{
    if (CanCopy())
    {
        HWND hWnd = GetHwnd();
        if (m_bIsMLE)
            ::WinSendMsg(hWnd, MLM_COPY, 0, 0);
        else
            ::WinSendMsg(hWnd, EM_COPY, 0, 0);
    }
} // end of wxTextCtrl::Copy

void wxTextCtrl::Cut()
{
    if (CanCut())
    {
        HWND hWnd = GetHwnd();

        if (m_bIsMLE)
            ::WinSendMsg(hWnd, MLM_CUT, 0, 0);
        else
            ::WinSendMsg(hWnd, EM_CUT, 0, 0);
    }
}

void wxTextCtrl::Paste()
{
    if (CanPaste())
    {
        HWND hWnd = GetHwnd();
//        SendMessage(hWnd, WM_PASTE, 0, 0L);
    }
}

bool wxTextCtrl::CanCopy() const
{
    // Can copy if there's a selection
    long from = 0L;
    long to = 0L;
//    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanCut() const
{
    // Can cut if there's a selection
    long from = 0L;
    long to = 0L;
//    GetSelection(& from, & to);
    return (from != to);
}

bool wxTextCtrl::CanPaste() const
{
    bool                            bIsTextAvailable = FALSE;

    if (!IsEditable())
        return FALSE;

    //
    // Check for straight text on clipboard
    //
    if (::WinOpenClipbrd(vHabmain))
    {
        bIsTextAvailable = (::WinQueryClipbrdData(vHabmain, CF_TEXT) != 0);
        ::WinCloseClipbrd(vHabmain);
    }
    return bIsTextAvailable;
} // end of wxTextCtrl::CanPaste

// ----------------------------------------------------------------------------
// Accessors
// ----------------------------------------------------------------------------

void wxTextCtrl::SetEditable(bool editable)
{
    HWND hWnd = GetHwnd();
//    SendMessage(hWnd, EM_SETREADONLY, (WPARAM)!editable, (LPARAM)0L);
}

void wxTextCtrl::SetInsertionPoint(long pos)
{
// TODO:
/*
    HWND hWnd = GetHwnd();
    {
        SendMessage(hWnd, EM_SETSEL, pos, pos);
        SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
    }
    static const char *nothing = "";
    SendMessage(hWnd, EM_REPLACESEL, 0, (LPARAM)nothing);
*/
}

void wxTextCtrl::SetInsertionPointEnd()
{
}

long wxTextCtrl::GetInsertionPoint() const
{
    WXDWORD                         dwPos = 0L;

    if (m_bIsMLE)
        dwPos = (WXDWORD)::WinSendMsg(GetHwnd(), MLM_QUERYSEL, (MPARAM)MLFQS_MINSEL, 0);
    else
    {
        dwPos = (WXDWORD)::WinSendMsg(GetHwnd(), EM_QUERYSEL, 0, 0);
        dwPos = SHORT1FROMMP((MPARAM)dwPos);  // the first 16 bit value is the min pos
    }
    return (dwPos & 0xFFFF);
} // end of wxTextCtrl::GetInsertionPoint

long wxTextCtrl::GetLastPosition() const
{
    HWND                            hWnd = GetHwnd();
    long                            lCharIndex;
    long                            lLineLength;

    if (m_bIsMLE)
    {
        lCharIndex = 0;

        //
        // This just gets the total text length.  The last will be this value
        //
        lLineLength = (long)::WinSendMsg(hWnd, MLM_QUERYTEXTLENGTH, 0, 0);
    }
    else
    {
        WNDPARAMS                   vParams;

        lCharIndex = 0;
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
            lLineLength = 0;
    }
    return(lCharIndex + lLineLength);
} // end of wxTextCtrl::GetLastPosition

// If the return values from and to are the same, there is no
// selection.
void wxTextCtrl::GetSelection(long* from, long* to) const
{
    DWORD dwStart, dwEnd;
    MPARAM wParam = (MPARAM) (DWORD*) & dwStart; // receives starting position
    MPARAM lParam = (MPARAM) (DWORD*) & dwEnd;   // receives ending position

//    ::SendMessage(GetHwnd(), EM_GETSEL, wParam, lParam);

    *from = dwStart;
    *to = dwEnd;
}

bool wxTextCtrl::IsEditable() const
{
// TODO:
/*
    long style = ::GetWindowLong(GetHwnd(), GWL_STYLE);

    return ((style & ES_READONLY) == 0);
*/
    return FALSE;
}

// ----------------------------------------------------------------------------
// Editing
// ----------------------------------------------------------------------------

void wxTextCtrl::Replace(long from, long to, const wxString& value)
{
#if wxUSE_CLIPBOARD
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Set selection and remove it
//    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
//    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);

    // Now replace with 'value', by pasting.
    wxSetClipboardData(wxDF_TEXT, (wxObject *) (const wxChar *)value, 0, 0);

    // Paste into edit control
//    SendMessage(hWnd, WM_PASTE, (WPARAM)0, (LPARAM)0L);
#else
    wxFAIL_MSG("wxTextCtrl::Replace not implemented if wxUSE_CLIPBOARD is 0.");
#endif
}

void wxTextCtrl::Remove(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // Cut all selected text
//    SendMessage(hWnd, EM_SETSEL, fromChar, toChar);
//    SendMessage(hWnd, WM_CUT, (WPARAM)0, (LPARAM)0);
}

void wxTextCtrl::SetSelection(long from, long to)
{
    HWND hWnd = GetHwnd();
    long fromChar = from;
    long toChar = to;

    // if from and to are both -1, it means (in wxWindows) that all text should
    // be selected. Translate into Windows convention
    if ((from == -1) && (to == -1))
    {
      fromChar = 0;
      toChar = -1;
    }

//    SendMessage(hWnd, EM_SETSEL, (WPARAM)fromChar, (LPARAM)toChar);
//    SendMessage(hWnd, EM_SCROLLCARET, (WPARAM)0, (LPARAM)0);
}

bool wxTextCtrl::LoadFile(const wxString& file)
{
// TODO:
/*
    if ( wxTextCtrlBase::LoadFile(file) )
    {
        // update the size limit if needed
        AdjustSpaceLimit();

        return TRUE;
    }
*/
    return FALSE;
}

bool wxTextCtrl::IsModified() const
{
//    return (SendMessage(GetHwnd(), EM_GETMODIFY, 0, 0) != 0);
    return FALSE;
}

//
// Makes 'unmodified'
//
void wxTextCtrl::DiscardEdits()
{
    if (m_bIsMLE)
        ::WinSendMsg(GetHwnd(), MLM_SETCHANGED, MPFROMLONG(FALSE), 0);
    else
        //
        // EM controls do not have a SETCHANGED but issuing a query should reset it
        //
        ::WinSendMsg(GetHwnd(), EM_QUERYCHANGED, 0, 0);
} // end of wxTextCtrl::DiscardEdits

int wxTextCtrl::GetNumberOfLines() const
{
//    return (int)SendMessage(GetHwnd(), EM_GETLINECOUNT, (WPARAM)0, (LPARAM)0);
    return 0;
}

long wxTextCtrl::XYToPosition(long x, long y) const
{
    HWND hWnd = GetHwnd();

    // This gets the char index for the _beginning_ of this line
// TODO:
/*
    int charIndex = (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)y, (LPARAM)0);
    return (long)(x + charIndex);
*/
    return 0L;
}

bool wxTextCtrl::PositionToXY(long pos, long *x, long *y) const
{
    HWND hWnd = GetHwnd();

    // This gets the line number containing the character
    int lineNo = -1;
//    lineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, 0);

    if ( lineNo == -1 )
    {
        // no such line
        return FALSE;
    }

    // This gets the char index for the _beginning_ of this line
    int charIndex = 0; // TODO: (int)SendMessage(hWnd, EM_LINEINDEX, (WPARAM)lineNo, (LPARAM)0);
    if ( charIndex == -1 )
    {
        return FALSE;
    }

    // The X position must therefore be the different between pos and charIndex
    if ( x )
        *x = (long)(pos - charIndex);
    if ( y )
        *y = (long)lineNo;

    return TRUE;
}

void wxTextCtrl::ShowPosition(long pos)
{
    HWND hWnd = GetHwnd();

    // To scroll to a position, we pass the number of lines and characters
    // to scroll *by*. This means that we need to:
    // (1) Find the line position of the current line.
    // (2) Find the line position of pos.
    // (3) Scroll by (pos - current).
    // For now, ignore the horizontal scrolling.

    // Is this where scrolling is relative to - the line containing the caret?
    // Or is the first visible line??? Try first visible line.
//    int currentLineLineNo1 = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)-1, (LPARAM)0L);

// TODO:
/*
    int currentLineLineNo = (int)SendMessage(hWnd, EM_GETFIRSTVISIBLELINE, (WPARAM)0, (LPARAM)0L);

    int specifiedLineLineNo = (int)SendMessage(hWnd, EM_LINEFROMCHAR, (WPARAM)pos, (LPARAM)0L);

    int linesToScroll = specifiedLineLineNo - currentLineLineNo;

    if (linesToScroll != 0)
      (void)SendMessage(hWnd, EM_LINESCROLL, (WPARAM)0, (LPARAM)linesToScroll);
*/
}

int wxTextCtrl::GetLineLength(
  long                              lLineNo
) const
{
    long                            lLen = 0L;

    if (m_bIsMLE)
        lLen = (long)::WinSendMsg(GetHwnd(), MLM_QUERYLINELENGTH, 0, 0);
    else
    {
        WNDPARAMS                   vParams;

        vParams.fsStatus = WPM_CCHTEXT;
        if (::WinSendMsg( GetHwnd()
                         ,WM_QUERYWINDOWPARAMS
                         ,&vParams
                         ,0
                        ))
        {
            lLen = vParams.cchText;
        }
        else
            lLen = 32;
    }
    return lLen;
} // end of

wxString wxTextCtrl::GetLineText(long lineNo) const
{
    size_t len = (size_t)GetLineLength(lineNo) + 1;
    char *buf = (char *)malloc(len);
    *(WORD *)buf = len;
    int noChars = 0; // TODO:(int)SendMessage(GetHwnd(), EM_GETLINE, lineNo, (LPARAM)buf);
    buf[noChars] = 0;

    wxString str(buf);

    free(buf);

    return str;
}

// ----------------------------------------------------------------------------
// Undo/redo
// ----------------------------------------------------------------------------

void wxTextCtrl::Undo()
{
    if (CanUndo())
    {
//        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

void wxTextCtrl::Redo()
{
    if (CanRedo())
    {
        // Same as Undo, since Undo undoes the undo, i.e. a redo.
//        ::SendMessage(GetHwnd(), EM_UNDO, 0, 0);
    }
}

bool wxTextCtrl::CanUndo() const
{
    bool                            bOk;

    if (m_bIsMLE)
        bOk = (::WinSendMsg(GetHwnd(), MLM_QUERYUNDO, 0, 0) != 0);
    else
        bOk = FALSE; // can't undo regular edit fields in PM
    return bOk;
} // end of wxTextCtrl::CanUndo

bool wxTextCtrl::CanRedo() const
{
    bool                            bOk;

    if (m_bIsMLE)
        bOk = (::WinSendMsg(GetHwnd(), MLM_QUERYUNDO, 0, 0) != 0);
    else
        bOk = FALSE; // can't undo regular edit fields in PM
    return bOk;
} // end of wxTextCtrl::CanRedo

// ----------------------------------------------------------------------------
// implemenation details
// ----------------------------------------------------------------------------

void wxTextCtrl::Command(wxCommandEvent & event)
{
    SetValue(event.GetString());
    ProcessCommand (event);
}

void wxTextCtrl::OnDropFiles(wxDropFilesEvent& event)
{
    // By default, load the first file into the text window.
    if (event.GetNumberOfFiles() > 0)
    {
        LoadFile(event.GetFiles()[0]);
    }
}

WXHBRUSH wxTextCtrl::OnCtlColor(WXHDC pDC, WXHWND pWnd, WXUINT nCtlColor,
                                WXUINT message, WXWPARAM wParam,
                                WXLPARAM lParam)
{
    HDC hdc = (HDC)pDC;
// TODO:
/*
    SetBkMode(hdc, GetParent()->GetTransparentBackground() ? TRANSPARENT
                                                           : OPAQUE);

    ::SetBkColor(hdc, RGB(GetBackgroundColour().Red(), GetBackgroundColour().Green(), GetBackgroundColour().Blue()));
    ::SetTextColor(hdc, RGB(GetForegroundColour().Red(), GetForegroundColour().Green(), GetForegroundColour().Blue()));
*/
    wxBrush *backgroundBrush = wxTheBrushList->FindOrCreateBrush(GetBackgroundColour(), wxSOLID);

    return (WXHBRUSH) backgroundBrush->GetResourceHandle();
}

void wxTextCtrl::OnChar(wxKeyEvent& event)
{
    switch ( event.KeyCode() )
    {
// TODO:
/*
        case WXK_RETURN:
            if ( !(m_windowStyle & wxTE_MULTILINE) )
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_ENTER, m_windowId);
                event.SetEventObject( this );
                if ( GetEventHandler()->ProcessEvent(event) )
                    return;
            }
            //else: multiline controls need Enter for themselves

            break;

        case WXK_TAB:
            // always produce navigation event - even if we process TAB
            // ourselves the fact that we got here means that the user code
            // decided to skip processing of this TAB - probably to let it
            // do its default job.
            //
            // NB: Notice that Ctrl-Tab is handled elsewhere and Alt-Tab is
            //     handled by Windows
            {
                wxNavigationKeyEvent eventNav;
                eventNav.SetDirection(!event.ShiftDown());
                eventNav.SetWindowChange(FALSE);
                eventNav.SetEventObject(this);

                if ( GetEventHandler()->ProcessEvent(eventNav) )
                    return;
            }
            break;
*/
        default:
            event.Skip();
            return;
    }

    // don't just call event.Skip() because this will cause TABs and ENTERs
    // be passed upwards and we don't always want this - instead process it
    // right here

    // FIXME
    event.Skip();
}

bool wxTextCtrl::OS2Command(WXUINT param, WXWORD WXUNUSED(id))
{
    switch (param)
    {
// TODO:
/*
        case EN_SETFOCUS:
        case EN_KILLFOCUS:
            {
                wxFocusEvent event(param == EN_KILLFOCUS ? wxEVT_KILL_FOCUS
                        : wxEVT_SET_FOCUS,
                        m_windowId);
                event.SetEventObject( this );
                GetEventHandler()->ProcessEvent(event);
            }
            break;

        case EN_CHANGE:
            {
                wxCommandEvent event(wxEVT_COMMAND_TEXT_UPDATED, m_windowId);
                wxString val(GetValue());
                if ( !val.IsNull() )
                    event.m_commandString = WXSTRINGCAST val;
                event.SetEventObject( this );
                ProcessCommand(event);
            }
            break;

        case EN_ERRSPACE:
            // the text size limit has been hit - increase it
            AdjustSpaceLimit();
            break;

            // the other notification messages are not processed
        case EN_UPDATE:
        case EN_MAXTEXT:
        case EN_HSCROLL:
        case EN_VSCROLL:
*/
        default:
            return FALSE;
    }

    // processed
    return TRUE;
}

void wxTextCtrl::AdjustSpaceLimit()
{
    unsigned int                    uLen = 0;
    unsigned int                    uLimit = 0;

    uLen   = ::WinQueryWindowTextLength(GetHwnd());
    if (m_bIsMLE)
    {
        uLimit = (unsigned int)::WinSendMsg( GetHwnd()
                                            ,MLM_QUERYTEXTLIMIT
                                            ,0
                                            ,0
                                           );
    }
    else
    {
        ENTRYFDATA*                 pEfd;
        WNDPARAMS                   vParams;

        vParams.fsStatus = WPM_CBCTLDATA;
        vParams.cbCtlData = sizeof(ENTRYFDATA);

        if (::WinSendMsg( GetHwnd()
                         ,WM_QUERYWINDOWPARAMS
                         ,&vParams
                         ,0
                        ))
        {
            pEfd = (ENTRYFDATA*)vParams.pCtlData;
            uLimit = (unsigned int)pEfd->cchEditLimit;
        }
        else
            uLimit = 32; //PM's default
    }
    if (uLen >= uLimit)
    {
        uLimit = uLen + 0x8000;    // 32Kb
        if (uLimit > 0xffff)
        {
            uLimit = 0L;
        }
        if (m_bIsMLE)
            ::WinSendMsg(GetHwnd(), MLM_SETTEXTLIMIT, MPFROMLONG(uLimit), 0);
        else
            ::WinSendMsg(GetHwnd(), EM_SETTEXTLIMIT, MPFROMLONG(uLimit), 0);
    }
} // end of wxTextCtrl::AdjustSpaceLimit

bool wxTextCtrl::AcceptsFocus() const
{
    // we don't want focus if we can't be edited
    return IsEditable() && wxControl::AcceptsFocus();
}

wxSize wxTextCtrl::DoGetBestSize() const
{
    int cx, cy;
    wxGetCharSize(GetHWND(), &cx, &cy, (wxFont*)&GetFont());

    int wText = DEFAULT_ITEM_WIDTH;

    int hText = EDIT_HEIGHT_FROM_CHAR_HEIGHT(cy);
    if ( m_windowStyle & wxTE_MULTILINE )
    {
        hText *= wxMin(GetNumberOfLines(), 5);
    }
    //else: for single line control everything is ok

    return wxSize(wText, hText);
}

// ----------------------------------------------------------------------------
// standard handlers for standard edit menu events
// ----------------------------------------------------------------------------

void wxTextCtrl::OnCut(wxCommandEvent& event)
{
    Cut();
}

void wxTextCtrl::OnCopy(wxCommandEvent& event)
{
    Copy();
}

void wxTextCtrl::OnPaste(wxCommandEvent& event)
{
    Paste();
}

void wxTextCtrl::OnUndo(wxCommandEvent& event)
{
    Undo();
}

void wxTextCtrl::OnRedo(wxCommandEvent& event)
{
    Redo();
}

void wxTextCtrl::OnUpdateCut(wxUpdateUIEvent& event)
{
    event.Enable( CanCut() );
}

void wxTextCtrl::OnUpdateCopy(wxUpdateUIEvent& event)
{
    event.Enable( CanCopy() );
}

void wxTextCtrl::OnUpdatePaste(wxUpdateUIEvent& event)
{
    event.Enable( CanPaste() );
}

void wxTextCtrl::OnUpdateUndo(wxUpdateUIEvent& event)
{
    event.Enable( CanUndo() );
}

void wxTextCtrl::OnUpdateRedo(wxUpdateUIEvent& event)
{
    event.Enable( CanRedo() );
}

