/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/fontdlg.cpp
// Purpose:     wxFontDialog class
// Author:      Julian Smart
// Modified by:
// Created:     01/02/97
// Copyright:   (c) Julian Smart
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

// ============================================================================
// declarations
// ============================================================================

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

// For compilers that support precompilation, includes "wx.h".
#include "wx/wxprec.h"

#ifdef __BORLANDC__
    #pragma hdrstop
#endif

#if wxUSE_FONTDLG

#include "wx/fontdlg.h"
#include "wx/modalhook.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcdlg.h"
    #include "wx/utils.h"
    #include "wx/dialog.h"
    #include "wx/log.h"
    #include "wx/math.h"
#endif

#include "wx/fontutil.h"
#include "wx/msw/private/dpiaware.h"

#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxFontDialog, wxDialog);

// ============================================================================
// implementation
// ============================================================================

// ----------------------------------------------------------------------------
// font dialog hook proc used for setting the dialog title if necessary
// ----------------------------------------------------------------------------

static
UINT_PTR CALLBACK
wxFontDialogHookProc(HWND hwnd,
                     UINT uiMsg,
                     WPARAM WXUNUSED(wParam),
                     LPARAM lParam)
{
    if ( uiMsg == WM_INITDIALOG )
    {
        CHOOSEFONT *pCH = (CHOOSEFONT *)lParam;
        wxFontDialog * const
            dialog = reinterpret_cast<wxFontDialog *>(pCH->lCustData);

        ::SetWindowText(hwnd, dialog->GetTitle().t_str());
    }

    return 0;
}

// ----------------------------------------------------------------------------
// wxFontDialog
// ----------------------------------------------------------------------------

void wxFontDialog::SetTitle(const wxString& title)
{
    // Just store the title here, we can't set it right now because the dialog
    // doesn't exist yet -- it will be created only when ShowModal() is called.
    m_title = title;
}

wxString wxFontDialog::GetTitle() const
{
    return m_title;
}

int wxFontDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindow* const parent = GetParentForModalDialog(m_parent, GetWindowStyle());
    WXHWND hWndParent = parent ? GetHwndOf(parent) : NULL;
    // It should be OK to always use GDI simulations
    DWORD flags = CF_SCREENFONTS /* | CF_NOSIMULATIONS */ ;

    LOGFONT logFont;

    CHOOSEFONT chooseFontStruct;
    wxZeroMemory(chooseFontStruct);

    chooseFontStruct.lStructSize = sizeof(CHOOSEFONT);
    chooseFontStruct.hwndOwner = hWndParent;
    chooseFontStruct.lpLogFont = &logFont;

    // Currently we only use the hook to set the title, so only set it up if
    // we really need to do this.
    if ( !m_title.empty() )
    {
        flags |= CF_ENABLEHOOK;
        chooseFontStruct.lCustData = (LPARAM)this;
        chooseFontStruct.lpfnHook = wxFontDialogHookProc;
    }

    if ( m_fontData.m_initialFont.IsOk() )
    {
        flags |= CF_INITTOLOGFONTSTRUCT;
        logFont = m_fontData.m_initialFont.GetNativeFontInfo()->lf;
    }

    if ( m_fontData.m_fontColour.IsOk() )
    {
        chooseFontStruct.rgbColors = wxColourToRGB(m_fontData.m_fontColour);
    }

    // CF_ANSIONLY flag is obsolete for Win32
    if ( !m_fontData.GetAllowSymbols() )
    {
      flags |= CF_SELECTSCRIPT;
      logFont.lfCharSet = ANSI_CHARSET;
    }

    if ( m_fontData.GetEnableEffects() )
      flags |= CF_EFFECTS;
    if ( m_fontData.GetShowHelp() )
      flags |= CF_SHOWHELP;
    if ( m_fontData.GetRestrictSelection() & wxFONTRESTRICT_SCALABLE )
      flags |= CF_SCALABLEONLY;
    if ( m_fontData.GetRestrictSelection() & wxFONTRESTRICT_FIXEDPITCH )
      flags |= CF_FIXEDPITCHONLY;

    if ( m_fontData.m_minSize != 0 || m_fontData.m_maxSize != 0 )
    {
        chooseFontStruct.nSizeMin = m_fontData.m_minSize;
        chooseFontStruct.nSizeMax = m_fontData.m_maxSize;
        flags |= CF_LIMITSIZE;
    }

    chooseFontStruct.Flags = flags;

    wxMSWImpl::AutoSystemDpiAware dpiAwareness;

    if ( ChooseFont(&chooseFontStruct) != 0 )
    {
        wxRGBToColour(m_fontData.m_fontColour, chooseFontStruct.rgbColors);
        m_fontData.m_chosenFont = wxFont(wxNativeFontInfo(logFont, this));
        m_fontData.EncodingInfo().facename = logFont.lfFaceName;
        m_fontData.EncodingInfo().charset = logFont.lfCharSet;

        return wxID_OK;
    }
    else
    {
        DWORD dwErr = CommDlgExtendedError();
        if ( dwErr != 0 )
        {
            wxLogError(_("Common dialog failed with error code %0lx."), dwErr);
        }
        //else: it was just cancelled

        return wxID_CANCEL;
    }
}

#endif // wxUSE_FONTDLG
