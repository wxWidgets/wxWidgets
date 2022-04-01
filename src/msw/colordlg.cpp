/////////////////////////////////////////////////////////////////////////////
// Name:        src/msw/colordlg.cpp
// Purpose:     wxColourDialog class
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


#if wxUSE_COLOURDLG

#include "wx/colordlg.h"
#include "wx/modalhook.h"

#ifndef WX_PRECOMP
    #include "wx/msw/wrapcdlg.h"
    #include <stdio.h>
    #include "wx/colour.h"
    #include "wx/gdicmn.h"
    #include "wx/utils.h"
    #include "wx/math.h"
#endif

#include "wx/scopeguard.h"

#include "wx/msw/private.h"
#include "wx/msw/private/dpiaware.h"

#include <stdlib.h>
#include <string.h>

// ----------------------------------------------------------------------------
// globals
// ----------------------------------------------------------------------------

// standard colors dialog size for the Windows systems
// this is ok if color dialog created with standart color
// and "Define Custom Colors" extension not shown
static wxRect gs_rectDialog(0, 0, 222, 324);

// The dialog currently being shown or null.
static wxColourDialog* gs_activeDialog = NULL;

// ----------------------------------------------------------------------------
// wxWin macros
// ----------------------------------------------------------------------------

wxIMPLEMENT_DYNAMIC_CLASS(wxColourDialog, wxDialog);

// ============================================================================
// implementation
// ============================================================================

#ifndef COLORBOXES
    #define COLORBOXES 64
#endif

// Undocumented property storing the COLORINFO struct in the standard dialog.
#ifndef COLORPROP
    #define COLORPROP (LPCTSTR) 0xA000L
#endif

namespace
{

// The private and undocumented Windows structure used by the standard dialog.
// See https://social.msdn.microsoft.com/Forums/en-US/c5fcfd9f-6b27-4848-bb9d-94bec105eabd/get-the-current-clicked-color-from-choosecolor-dialog?forum=windowsgeneraldevelopmentissues
struct COLORINFO
{
    UINT           ApiType;
    LPCHOOSECOLOR  pCC;
    HANDLE         hLocal;
    HANDLE         hDialog;
    HPALETTE       hPal;
    DWORD          currentRGB;
    WORD           currentHue;
    WORD           currentSat;
    WORD           currentLum;
    WORD           nHueWidth;
    WORD           nSatHeight;
    WORD           nLumHeight;
    WORD           nCurMix;
    WORD           nCurDsp;
    WORD           nCurBox;
    WORD           nHuePos;
    WORD           nSatPos;
    WORD           nLumPos;
    RECT           rOriginal;
    RECT           rRainbow;
    RECT           rLumScroll;
    RECT           rLumPaint;
    RECT           rCurrentColor;
    RECT           rNearestPure;
    RECT           rColorSamples;
    BOOL           bFoldOut;
    DWORD          rgbBoxColor[COLORBOXES];
};

} // anonymous namespace

// ----------------------------------------------------------------------------
// colour dialog hook proc
// ----------------------------------------------------------------------------

UINT_PTR CALLBACK
wxColourDialogHookProc(HWND hwnd,
                       UINT uiMsg,
                       WPARAM WXUNUSED(wParam),
                       LPARAM WXUNUSED(lParam))
{
    switch ( uiMsg )
    {
        case WM_INITDIALOG:
            {
                const wxString title = gs_activeDialog->GetTitle();
                if ( !title.empty() )
                    ::SetWindowText(hwnd, title.t_str());

                gs_activeDialog->MSWOnInitDone((WXHWND)hwnd);
            }
            break;

        default:
            // Check if the currently selected colour changed.
            //
            // Doing it for all messages might be an overkill, we probably
            // could only do it for keyboard/mouse ones.
            if ( const COLORINFO* pCI = (COLORINFO*)::GetProp(hwnd, COLORPROP) )
            {
                gs_activeDialog->MSWCheckIfCurrentChanged(pCI->currentRGB);
            }
            break;
    }

    return 0;
}

// ----------------------------------------------------------------------------
// wxColourDialog
// ----------------------------------------------------------------------------

void wxColourDialog::Init()
{
    m_movedWindow = false;
    m_centreDir = 0;

    // reset to zero, otherwise the wx routines won't size the window the
    // second time the dialog is shown, because they would believe it already
    // has the requested size/position
    gs_rectDialog.x =
    gs_rectDialog.y = 0;
}

bool wxColourDialog::Create(wxWindow *parent, const wxColourData *data)
{
    m_parent = parent;
    if (data)
        m_colourData = *data;

    return true;
}

int wxColourDialog::ShowModal()
{
    WX_HOOK_MODAL_DIALOG();

    wxWindowDisabler disableOthers(this);

    wxWindow* const parent = GetParentForModalDialog(m_parent, GetWindowStyle());
    WXHWND hWndParent = parent ? GetHwndOf(parent) : NULL;

    // initialize the struct used by Windows
    CHOOSECOLOR chooseColorStruct;
    memset(&chooseColorStruct, 0, sizeof(CHOOSECOLOR));

    size_t i;

    // and transfer data from m_colourData to it
    COLORREF custColours[16];
    for ( i = 0; i < WXSIZEOF(custColours); i++ )
    {
        if ( m_colourData.GetCustomColour(i).IsOk() )
            custColours[i] = wxColourToRGB(m_colourData.GetCustomColour(i));
        else
            custColours[i] = RGB(255,255,255);
    }

    m_currentCol = wxColourToRGB(m_colourData.GetColour());

    chooseColorStruct.lStructSize = sizeof(CHOOSECOLOR);
    chooseColorStruct.hwndOwner = hWndParent;
    chooseColorStruct.rgbResult = m_currentCol;
    chooseColorStruct.lpCustColors = custColours;

    chooseColorStruct.Flags = CC_RGBINIT | CC_ENABLEHOOK;
    chooseColorStruct.lCustData = (LPARAM)this;
    chooseColorStruct.lpfnHook = wxColourDialogHookProc;

    if ( m_colourData.GetChooseFull() )
        chooseColorStruct.Flags |= CC_FULLOPEN;

    // Set the global pointer for the duration of the modal dialog life-time.
    gs_activeDialog = this;
    wxON_BLOCK_EXIT_NULL(gs_activeDialog);

    wxMSWImpl::AutoSystemDpiAware dpiAwareness;

    // do show the modal dialog
    if ( !::ChooseColor(&chooseColorStruct) )
    {
        // 0 error means the dialog was simply cancelled, i.e. no real error
        // occurred
        const DWORD err = CommDlgExtendedError();
        if ( err )
        {
            wxLogError(_("Colour selection dialog failed with error %0lx."), err);
        }

        return wxID_CANCEL;
    }


    // transfer the values chosen by user back into m_colourData
    for ( i = 0; i < WXSIZEOF(custColours); i++ )
    {
      wxRGBToColour(m_colourData.m_custColours[i], custColours[i]);
    }

    wxRGBToColour(m_colourData.GetColour(), chooseColorStruct.rgbResult);

    // this doesn't seem to work (contrary to what MSDN implies) on current
    // Windows versions: CC_FULLOPEN is never set on return if it wasn't
    // initially set and vice versa
    //m_colourData.SetChooseFull((chooseColorStruct.Flags & CC_FULLOPEN) != 0);

    return wxID_OK;
}

// ----------------------------------------------------------------------------
// title
// ----------------------------------------------------------------------------

void wxColourDialog::SetTitle(const wxString& title)
{
    m_title = title;
}

wxString wxColourDialog::GetTitle() const
{
    return m_title;
}

// ----------------------------------------------------------------------------
// position/size
// ----------------------------------------------------------------------------

void wxColourDialog::DoGetPosition(int *x, int *y) const
{
    if ( x )
        *x = gs_rectDialog.x;
    if ( y )
        *y = gs_rectDialog.y;
}

void wxColourDialog::DoCentre(int dir)
{
    m_centreDir = dir;

    // it's unnecessary to do anything else at this stage as we'll redo it in
    // MSWOnInitDone() anyhow
}

void wxColourDialog::DoMoveWindow(int x, int y, int WXUNUSED(w), int WXUNUSED(h))
{
    gs_rectDialog.x = x;
    gs_rectDialog.y = y;

    // our HWND is only set when we're called from MSWOnInitDone(), test if
    // this is the case
    HWND hwnd = GetHwnd();
    if ( hwnd )
    {
        // size of the dialog can't be changed because the controls are not
        // laid out correctly then
       ::SetWindowPos(hwnd, HWND_TOP, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE);
    }
    else // just remember that we were requested to move the window
    {
        m_movedWindow = true;

        // if Centre() had been called before, it shouldn't be taken into
        // account now
        m_centreDir = 0;
    }
}

void wxColourDialog::DoGetSize(int *width, int *height) const
{
    if ( width )
        *width = gs_rectDialog.width;
    if ( height )
        *height = gs_rectDialog.height;
}

void wxColourDialog::DoGetClientSize(int *width, int *height) const
{
    if ( width )
        *width = gs_rectDialog.width;
    if ( height )
        *height = gs_rectDialog.height;
}

void wxColourDialog::MSWOnInitDone(WXHWND hDlg)
{
    // set HWND so that our DoMoveWindow() works correctly
    TempHWNDSetter set(this, hDlg);

    if ( m_centreDir )
    {
        // now we have the real dialog size, remember it
        RECT rect;
        ::GetWindowRect((HWND)hDlg, &rect);
        gs_rectDialog = wxRectFromRECT(rect);

        // and position the window correctly: notice that we must use the base
        // class version as our own doesn't do anything except setting flags
        wxDialog::DoCentre(m_centreDir);
    }
    else if ( m_movedWindow ) // need to just move it to the correct place
    {
        SetPosition(GetPosition());
    }
}

void wxColourDialog::MSWCheckIfCurrentChanged(WXCOLORREF currentCol)
{
    if ( currentCol == m_currentCol )
        return;

    m_currentCol = currentCol;

    wxColourDialogEvent event(wxEVT_COLOUR_CHANGED, this, wxRGBToColour(currentCol));
    ProcessWindowEvent(event);
}

#endif // wxUSE_COLOURDLG
